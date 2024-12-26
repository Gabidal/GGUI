#include <iostream>
#include <string>
#include <vector>
#include <git2.h>
#include <git2/diff.h>
#include <sstream>

class Hunk {
public:
    std::string file;
    int old_start, old_lines;  // Old file's line range
    int new_start, new_lines; // New file's line range

    Hunk(const std::string& file, int old_start, int old_lines, int new_start, int new_lines)
        : file(file), old_start(old_start), old_lines(old_lines), new_start(new_start), new_lines(new_lines) {}
};

class Commit {
public:
    std::string ID = "";        // Hash 
    std::string MSG = "";       // Commit message
    std::vector<Hunk> hunks;  // Store Hunk information

    Commit(const std::string& id, const std::string& summary) : ID(id), MSG(summary) {}

    void add_hunk(const std::string& file, int old_start, int old_lines, int new_start, int new_lines) {
        hunks.emplace_back(file, old_start, old_lines, new_start, new_lines);
    }
};

/**
 * @brief Checks for an error code and prints an error message if the code indicates an error.
 *
 * This function checks if the provided error code is less than 0. If it is, it retrieves the last
 * error message from the Git library and prints it along with the provided message to the standard
 * error stream. The program then exits with a failure status.
 *
 * @param error_code The error code to check.
 * @param message The message to print if an error is detected.
 */
void check_error(int error_code, const char* message) {
    if (error_code < 0) {
        const git_error* error = git_error_last();
        std::cerr << message << ": "
                  << (error && error->message ? error->message : "Unknown error")
                  << std::endl;
        exit(EXIT_FAILURE);
    }
}

/**
 * @brief Checks if a branch exists in the given Git repository.
 *
 * This function looks up a branch by its name in the specified Git repository.
 * It returns true if the branch exists, and false otherwise.
 *
 * @param repo A pointer to the Git repository object.
 * @param branch_name The name of the branch to check for existence.
 * @return true if the branch exists, false otherwise.
 */
bool branch_exists(git_repository* repo, const std::string& branch_name) {
    git_reference* ref = nullptr;
    std::string ref_name = "refs/heads/" + branch_name;
    int error = git_reference_lookup(&ref, repo, ref_name.c_str());
    if (ref) {
        git_reference_free(ref);
    }
    return error == 0;
}

/**
 * @brief Retrieves the diff for a commit.
 *
 * This function retrieves the diff between a commit and its parent.
 * The diff is stored as a string.
 *
 * @param commit The commit for which to retrieve the diff.
 * @param repo The repository object.
 * @return A vector of strings where each string represents a diff for a changed file.
 */
void get_commit_diff(git_commit* commit, git_repository* repo, Commit& commit_info) {
    git_tree* commit_tree = nullptr;
    check_error(git_commit_tree(&commit_tree, commit), "Failed to get commit tree");

    git_commit* parent_commit = nullptr;
    if (git_commit_parentcount(commit) > 0) {
        check_error(git_commit_parent(&parent_commit, commit, 0), "Failed to get parent commit");
    }

    git_tree* parent_tree = nullptr;
    if (parent_commit) {
        check_error(git_commit_tree(&parent_tree, parent_commit), "Failed to get parent tree");
    }

    git_diff* diff = nullptr;
    git_diff_options opts = GIT_DIFF_OPTIONS_INIT;  // Initialize with default options

    if (parent_commit) {
        // Generate diff from commit tree to parent tree
        check_error(git_diff_tree_to_tree(&diff, repo, parent_tree, commit_tree, &opts), "Failed to generate diff");
    } else {
        // For the initial commit, generate diff with the working directory
        check_error(git_diff_tree_to_workdir(&diff, repo, commit_tree, &opts), "Failed to generate diff with workdir");
    }

    // Callback for handling files
    auto file_cb = [](const git_diff_delta* delta, float, void* payload) -> int {
        auto* commit_info = static_cast<Commit*>(payload);
        commit_info->add_hunk(delta->old_file.path, 0, 0, 0, 0);  // Placeholder; hunks will fill in details
        return 0;
    };

    // Callback for handling hunks
    auto hunk_cb = [](const git_diff_delta* delta, const git_diff_hunk* hunk, void* payload) -> int {
        auto* commit_info = static_cast<Commit*>(payload);
        std::string file = delta->old_file.path ? delta->old_file.path : delta->new_file.path;

        // Add the Hunk details to the Commit
        commit_info->add_hunk(file, hunk->old_start, hunk->old_lines, hunk->new_start, hunk->new_lines);
        return 0;
    };

    // Binary callback (no-op for now, but can be extended)
    auto binary_cb = [](const git_diff_delta*, const git_diff_binary*, void*) -> int {
        return 0;  // Skip binary diffs for now
    };

    // Iterate over the diff
    check_error(git_diff_foreach(diff, file_cb, binary_cb, hunk_cb, nullptr, &commit_info),
                "Failed to iterate over diff");

    // Cleanup
    git_diff_free(diff);
    git_tree_free(commit_tree);
    if (parent_commit) {
        git_tree_free(parent_tree);
        git_commit_free(parent_commit);
    }
}


/**
 * @brief Lists commits that are in the compare_branch but not in the base_branch.
 *
 * This function uses libgit2 to find and list commits that are present in the 
 * compare_branch but not in the base_branch. It resolves the branch references 
 * to OIDs, looks up the commits, and uses a revision walker to find the 
 * commits that are in the compare_branch but not in the base_branch.
 *
 * @param repo Pointer to the git repository.
 * @param base_branch The name of the base branch.
 * @param compare_branch The name of the branch to compare against the base branch.
 * @return A vector of Commit objects representing the commits in the compare_branch 
 *         but not in the base_branch.
 */
std::vector<Commit> list_commits_between_branches(git_repository* repo, const std::string& base_branch, const std::string& compare_branch) {
    git_oid base_oid, compare_oid;
    git_commit *base_commit = nullptr, *compare_commit = nullptr;
    std::vector<Commit> result;

    // Resolve branch references to OIDs
    std::string base_ref_name = "refs/heads/" + base_branch;
    std::string compare_ref_name = "refs/heads/" + compare_branch;

    check_error(git_reference_name_to_id(&base_oid, repo, base_ref_name.c_str()), "Failed to resolve base branch");
    check_error(git_reference_name_to_id(&compare_oid, repo, compare_ref_name.c_str()), "Failed to resolve compare branch");

    // Get commits for each branch
    check_error(git_commit_lookup(&base_commit, repo, &base_oid), "Failed to lookup base commit");
    check_error(git_commit_lookup(&compare_commit, repo, &compare_oid), "Failed to lookup compare commit");

    // Create revwalk to find commits
    git_revwalk* walker;
    check_error(git_revwalk_new(&walker, repo), "Failed to create revision walker");

    git_revwalk_push(walker, &compare_oid);
    git_revwalk_hide(walker, &base_oid);

    git_oid oid;

    while (git_revwalk_next(&oid, walker) == 0) {
        git_commit* commit;
        check_error(git_commit_lookup(&commit, repo, &oid), "Failed to lookup commit");

        // Create a Commit object with the hash and summary
        Commit commit_info(git_oid_tostr_s(&oid), git_commit_summary(commit));

        // Populate the Commit object with its diffs and hunks
        get_commit_diff(commit, repo, commit_info);

        // Add the Commit object to the result list
        result.push_back(commit_info);

        // Free the commit object
        git_commit_free(commit);
    }

    // Cleanup
    git_revwalk_free(walker);
    git_commit_free(base_commit);
    git_commit_free(compare_commit);

    return result;
}

std::pair<std::string, std::string> fetch_hunk_text(
    git_repository* repo, const Commit& commit, const Hunk& hunk) {
    std::string old_content, new_content;

    if (commit.ID.empty()){
        std::cout << "Empty commit ID" << std::endl;
    }

    // Lookup the commit tree
    git_tree* commit_tree = nullptr;
    git_commit* git_commit_obj = nullptr;
    git_oid ID_ID;
    git_oid_fromstr(&ID_ID, commit.ID.c_str());
    check_error(git_commit_lookup(&git_commit_obj, repo, &ID_ID),
                "Failed to lookup commit");
    check_error(git_commit_tree(&commit_tree, git_commit_obj), "Failed to get commit tree");

    // Lookup the file in the tree
    git_tree_entry* entry = nullptr;
    check_error(git_tree_entry_bypath(&entry, commit_tree, hunk.file.c_str()), "Failed to get file entry");

    git_blob* blob = nullptr;
    check_error(git_blob_lookup(&blob, repo, git_tree_entry_id(entry)), "Failed to get blob");

    const char* file_content = (const char*)git_blob_rawcontent(blob);
    size_t file_size = git_blob_rawsize(blob);

    // Read file content into lines
    std::vector<std::string> file_lines;
    std::istringstream file_stream(std::string(file_content, file_size));
    std::string line;
    while (std::getline(file_stream, line)) {
        file_lines.push_back(line);
    }

    // Extract old lines (if applicable)
    if (hunk.old_start > 0 && hunk.old_lines > 0) {
        for (int i = 0; i < hunk.old_lines; ++i) {
            if (hunk.old_start - 1 + i < file_lines.size()) {
                old_content += file_lines[hunk.old_start - 1 + i] + "\n";
            }
        }
    }

    // Extract new lines (if applicable)
    if (hunk.new_start > 0 && hunk.new_lines > 0) {
        for (int i = 0; i < hunk.new_lines; ++i) {
            if (hunk.new_start - 1 + i < file_lines.size()) {
                new_content += file_lines[hunk.new_start - 1 + i] + "\n";
            }
        }
    }

    // Cleanup
    git_blob_free(blob);
    git_tree_entry_free(entry);
    git_tree_free(commit_tree);
    git_commit_free(git_commit_obj);

    return {old_content, new_content};
}


void print_hunk_differences(git_repository* repo, const Commit& commit) {
    for (const Hunk& hunk : commit.hunks) {
        auto [old_text, new_text] = fetch_hunk_text(repo, commit, hunk);
        std::cout << "File: " << hunk.file << "\n";
        std::cout << "Old Lines:\n" << old_text << "\n";
        std::cout << "New Lines:\n" << new_text << "\n";
        std::cout << "--------------------------\n";
    }
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <repo_path> <branch_name>\n";
        return EXIT_FAILURE;
    }

    const std::string repo_path = argv[1];
    const std::string compare_branch = argv[2];
    const std::string base_branch = "main";

    git_libgit2_init();

    git_repository* repo = nullptr;
    check_error(git_repository_open(&repo, repo_path.c_str()), "Failed to open repository");

    if (!branch_exists(repo, compare_branch)) {
        std::cerr << "Error: Branch '" << compare_branch << "' does not exist.\n";
        git_repository_free(repo);
        git_libgit2_shutdown();
        return EXIT_FAILURE;
    }

    std::vector<Commit> result = list_commits_between_branches(repo, base_branch, compare_branch);
    // for (const auto& commit : result) {
    //     std::cout << "Commit: " << commit.ID << " - " << commit.MSG << "\n";
    //     for (const auto& hunk : commit.hunks) {
    //         std::cout << "File: " << hunk.file << "\n";
    //         std::cout << "Old lines: " << hunk.old_start << "," << hunk.old_lines
    //                 << " -> New lines: " << hunk.new_start << "," << hunk.new_lines << "\n";
    //     }
    // }

    print_hunk_differences(repo, result.back());

    git_repository_free(repo);
    git_libgit2_shutdown();
    return EXIT_SUCCESS;
}