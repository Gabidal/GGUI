#include <iostream>
#include <string>
#include <vector>
#include <git2.h>
#include <git2/diff.h>
#include <sstream>

/**
 * @class Hunk
 * @brief Represents a hunk of changes in a file, typically used in version control systems.
 *
 * A hunk is a contiguous block of changes in a file, showing the differences between
 * an old version and a new version of the file.
 *
 * @param file The name of the file where the hunk is located.
 * @param old_start The starting line number of the hunk in the old version of the file.
 * @param old_lines The number of lines in the hunk in the old version of the file.
 * @param new_start The starting line number of the hunk in the new version of the file.
 * @param new_lines The number of lines in the hunk in the new version of the file.
 */
class Hunk {
public:
    std::string File_Name;
    int Old_Text_Start, Old_Text_Line_Count;   // Old file's line range
    int New_Text_Start, New_Text_Line_Count;   // New file's line range

    std::string Old_Text = "";
    std::string New_Text = "";

    Hunk(const std::string& file, int old_start, int old_lines, int new_start, int new_lines)
        : File_Name(file), Old_Text_Start(old_start), Old_Text_Line_Count(old_lines), New_Text_Start(new_start), New_Text_Line_Count(new_lines) {}
};

/**
 * @class Commit
 * @brief Represents a Git commit with its ID, message, and associated hunks.
 *
 * The Commit class stores information about a Git commit, including its unique
 * identifier (hash), commit message, and a list of hunks that describe changes
 * made in the commit.
 */
class Commit {
public:
    std::string ID = "";        // Hash 
    std::string MSG = "";       // Commit message
    std::vector<Hunk> Hunks;    // Store Hunk information

    /**
     * @brief Constructs a Commit object with the given ID and commit message.
     * 
     * @param id The unique identifier (hash) of the commit.
     * @param summary The commit message.
     */
    Commit(const std::string& id, const std::string& summary) : ID(id), MSG(summary) {}

    /**
     * @brief Adds a hunk to the commit.
     * 
     * @param file The name of the file where the hunk is applied.
     * @param old_start The starting line number in the original file.
     * @param old_lines The number of lines in the original file.
     * @param new_start The starting line number in the new file.
     * @param new_lines The number of lines in the new file.
     */
    void Add_Hunk(const std::string& file, int old_start, int old_lines, int new_start, int new_lines) {
        Hunks.emplace_back(file, old_start, old_lines, new_start, new_lines);
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
void Check_Error(int error_code, const char* message) {
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
bool Branch_Exists(git_repository* repo, const std::string& branch_name) {
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
void Get_Commit_Diff(git_commit* commit, git_repository* repo, Commit& commit_info) {
    git_tree* commit_tree = nullptr;
    Check_Error(git_commit_tree(&commit_tree, commit), "Failed to get commit tree");

    git_commit* parent_commit = nullptr;
    if (git_commit_parentcount(commit) > 0) {
        if (git_commit_parent(&parent_commit, commit, 0) < 0) {
            git_tree_free(commit_tree);
            Check_Error(-1, "Failed to get parent commit");
        }
    }

    git_tree* parent_tree = nullptr;
    if (parent_commit) {
        Check_Error(git_commit_tree(&parent_tree, parent_commit), "Failed to get parent tree");
    }

    git_diff* diff = nullptr;
    git_diff_options opts = GIT_DIFF_OPTIONS_INIT;  // Initialize with default options

    if (parent_commit) {
        // Generate diff from commit tree to parent tree
        Check_Error(git_diff_tree_to_tree(&diff, repo, parent_tree, commit_tree, &opts), "Failed to generate diff");
    } else {
        // For the initial commit, generate diff with the working directory
        Check_Error(git_diff_tree_to_workdir(&diff, repo, commit_tree, &opts), "Failed to generate diff with workdir");
    }

    auto file_cb = [](const git_diff_delta* delta, float, void* payload) -> int {
        return 0;
    };

    // Callback for handling hunks
    auto hunk_cb = [](const git_diff_delta* delta, const git_diff_hunk* hunk, void* payload) -> int {
        auto* commit_info = static_cast<Commit*>(payload);
        std::string file = delta->old_file.path ? delta->old_file.path : delta->new_file.path;

        // Add the Hunk details to the Commit
        commit_info->Add_Hunk(file, hunk->old_start, hunk->old_lines, hunk->new_start, hunk->new_lines);
        return 0;
    };

    // Binary callback (no-op for now, but can be extended)
    auto binary_cb = [](const git_diff_delta*, const git_diff_binary*, void*) -> int {
        return 0;  // Skip binary diffs for now
    };

    // Callback for handling lines (no-op for now, but can be extended)
    auto line_cb = [](const git_diff_delta*, const git_diff_hunk*, const git_diff_line*, void*) -> int {
        return 0;  // Skip line diffs for now
    };

    // Iterate over the diff
    Check_Error(git_diff_foreach(diff, file_cb, binary_cb, hunk_cb, line_cb, &commit_info),
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
std::vector<Commit> Get_Commits_Between_branches(git_repository* repo, const std::string& base_branch, const std::string& compare_branch) {
    git_oid base_oid, compare_oid;
    git_commit *base_commit = nullptr, *compare_commit = nullptr;
    std::vector<Commit> result;

    // Resolve branch references to OIDs
    std::string base_ref_name = "refs/heads/" + base_branch;
    std::string compare_ref_name = "refs/heads/" + compare_branch;

    Check_Error(git_reference_name_to_id(&base_oid, repo, base_ref_name.c_str()), "Failed to resolve base branch");
    Check_Error(git_reference_name_to_id(&compare_oid, repo, compare_ref_name.c_str()), "Failed to resolve compare branch");

    // Get commits for each branch
    Check_Error(git_commit_lookup(&base_commit, repo, &base_oid), "Failed to lookup base commit");
    Check_Error(git_commit_lookup(&compare_commit, repo, &compare_oid), "Failed to lookup compare commit");

    // Create revwalk to find commits
    git_revwalk* walker;
    Check_Error(git_revwalk_new(&walker, repo), "Failed to create revision walker");

    git_revwalk_push(walker, &compare_oid);
    git_revwalk_hide(walker, &base_oid);

    git_oid oid;

    while (git_revwalk_next(&oid, walker) == 0) {
        git_commit* commit;
        Check_Error(git_commit_lookup(&commit, repo, &oid), "Failed to lookup commit");

        // Create a Commit object with the hash and summary
        Commit commit_info(git_oid_tostr_s(&oid), git_commit_summary(commit));

        // Populate the Commit object with its diffs and hunks
        Get_Commit_Diff(commit, repo, commit_info);

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

void Fetch_Hunk_Content(git_repository* repo, const Commit& commit, Hunk& hunk) {
    std::string old_content, new_content;

    if (commit.ID.empty()){
        std::cout << "Empty commit ID" << std::endl;
    }

    // Lookup the commit tree
    git_tree* commit_tree = nullptr;
    git_commit* git_commit_obj = nullptr;
    git_oid ID_ID;
    git_oid_fromstr(&ID_ID, commit.ID.c_str());
    Check_Error(git_commit_lookup(&git_commit_obj, repo, &ID_ID),
                "Failed to lookup commit");
    Check_Error(git_commit_tree(&commit_tree, git_commit_obj), "Failed to get commit tree");

    // Lookup the file in the tree
    git_tree_entry* entry = nullptr;
    Check_Error(git_tree_entry_bypath(&entry, commit_tree, hunk.File_Name.c_str()), "Failed to get file entry");

    git_blob* blob = nullptr;
    Check_Error(git_blob_lookup(&blob, repo, git_tree_entry_id(entry)), "Failed to get blob");

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
    if (hunk.Old_Text_Start > 0 && hunk.Old_Text_Line_Count > 0) {
        for (int i = 0; i < hunk.Old_Text_Line_Count; ++i) {
            if (hunk.Old_Text_Start - 1 + i < file_lines.size()) {
                old_content += file_lines[hunk.Old_Text_Start - 1 + i] + "\n";
            }
        }
    }

    // Extract new lines (if applicable)
    if (hunk.New_Text_Start > 0 && hunk.New_Text_Line_Count > 0) {
        for (int i = 0; i < hunk.New_Text_Line_Count; ++i) {
            if (hunk.New_Text_Start - 1 + i < file_lines.size()) {
                new_content += file_lines[hunk.New_Text_Start - 1 + i] + "\n";
            }
        }
    }

    // Cleanup
    git_blob_free(blob);
    git_tree_entry_free(entry);
    git_tree_free(commit_tree);
    git_commit_free(git_commit_obj);

    hunk.Old_Text = old_content;
    hunk.New_Text = new_content;

    return;
}


void Print_Commit_Hunks(git_repository* repo, const Commit& commit) {
    for (const Hunk& hunk : commit.Hunks) {
        std::cout << "File: " << hunk.File_Name << "\n";
        std::cout << "Old Lines:\n" << hunk.Old_Text << "\n";
        std::cout << "New Lines:\n" << hunk.New_Text << "\n";
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
    Check_Error(git_repository_open(&repo, repo_path.c_str()), "Failed to open repository");

    if (!Branch_Exists(repo, compare_branch)) {
        std::cerr << "Error: Branch '" << compare_branch << "' does not exist.\n";
        git_repository_free(repo);
        git_libgit2_shutdown();
        return EXIT_FAILURE;
    }

    // Fetch all the commits which differ from the two branches.
    std::vector<Commit> Commits = Get_Commits_Between_branches(repo, base_branch, compare_branch);

    // Now we can fetch the text related to the hunks for each commit.
    for (auto& commit : Commits){
        for (auto& hunk : commit.Hunks){
            Fetch_Hunk_Content(repo, commit, hunk);
        }
    }

    // test
    Print_Commit_Hunks(repo, Commits.back());

    git_repository_free(repo);
    git_libgit2_shutdown();
    return EXIT_SUCCESS;
}