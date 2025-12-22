#include <iostream>
#include <string>
#include <filesystem>
#include <vector>
#include <fstream>
#include <algorithm>

using namespace std;

// returns all the file names in the dir.
std::vector<std::string> get_all_files(const std::string& directory) {
    std::vector<std::string> files;
    for (const auto & entry : std::filesystem::directory_iterator(directory)) {

        // Make sure that only header filers are selected:
        if(entry.path().extension() != ".h") continue;

        files.push_back(entry.path().filename().string());
    }

    return files;
}

void Compile_Headers(const std::string& destination, const std::string& source_root, bool ignore_autogen) {
    // Hardcoded header order based on dependencies
    std::vector<std::string> Header_Files_In_Order = {
        "src/core/utils/superString.h",
        "src/core/utils/conveyorAllocator.h",
        "src/core/utils/constants.h",
        "src/core/utils/color.h",
        "src/core/utils/types.h",
        "src/core/utils/utf.h",
        "src/core/utils/style.h",
        "src/core/utils/settings.h",
        "src/core/utils/utils.h",
        "src/core/utils/logger.h",
        "src/core/utils/fileStreamer.h",
        "src/core/utils/drm.h",
        "src/elements/element.h",
        "src/elements/listView.h",
        "src/elements/textField.h",
        "src/elements/canvas.h",
        "src/elements/HTML.h",
        "src/elements/progressBar.h",
        "src/elements/switch.h",
        "src/core/renderer.h",
        "src/core/addons/addons.h"
    };

    std::ofstream Output(destination);

    if (!Output.is_open()) {
        std::cerr << "Error: Could not open destination file for writing: " << destination << std::endl;
        return;
    }

    // =========================================================================
    // Write file header + TOC
    // =========================================================================
    Output << "// ============================================================================\n";
    Output << "//  GGUI - Amalgamated TUI Framework Header\n";
    Output << "//\n";
    Output << "//  This file was auto-generated via the ./bin/export.sh\n";
    Output << "//\n";
    Output << "//  TABLE OF CONTENTS\n";

    int sectionIndex = 1;
    for (const std::string& rel_path : Header_Files_In_Order) {
        Output << "//    [" << sectionIndex++ << "] " << rel_path << "\n";
    }

    Output << "// ============================================================================\n\n";
    Output << "#pragma once\n\n";

    // =========================================================================
    // Concatenate headers
    // =========================================================================
    for (const std::string& rel_path : Header_Files_In_Order) {
        std::string file_path = (std::filesystem::path(source_root) / rel_path).string();
        std::ifstream File(file_path);

        if (!File.is_open()) {
            std::cout << "Warning: Could not open file: " << file_path << std::endl;
            continue;
        }

        std::string Line;
        bool in_ignored_block = false;
        while (std::getline(File, Line)) {
            // Ignore marked regions when configured to do so
            if (ignore_autogen) {
                if (in_ignored_block) {
                    if (Line.find("autoGen: Ignore end") != std::string::npos) {
                        in_ignored_block = false;
                    }
                    continue;
                } else {
                    if (Line.find("autoGen: Ignore start") != std::string::npos) {
                        in_ignored_block = true;
                        continue;
                    }
                }
            }

            // Skip local includes of project headers
            if (Line.find("#include \"") != std::string::npos && Line.find(".h\"") != std::string::npos) {
                continue;
            }

            Output << Line << "\n";
        }
    }
}

/// @brief Gathers all cpp files under GGUI/src and returns them as usable source files for g++
std::vector<std::string> Get_Cpp_Files(){
    std::vector<std::string> cpp_files = {
        // Core utilities
        "src/core/utils/style.cpp",
        "src/core/utils/logger.cpp", 
        "src/core/utils/utils.cpp",
        "src/core/utils/fileStreamer.cpp",
        "src/core/utils/settings.cpp",
        "src/core/utils/drm.cpp",
        
        // Elements
        "src/elements/element.cpp",
        "src/elements/listView.cpp",
        "src/elements/textField.cpp",
        "src/elements/switch.cpp",
        "src/elements/canvas.cpp",
        "src/elements/progressBar.cpp",
        
        // Specialized elements
        "src/elements/HTML.cpp",
        
        // core functionalities
        "src/core/renderer.cpp",
        "src/core/thread.cpp",
        "src/core/addons/addons.cpp"
    };
    return cpp_files;
}

/// @brief Gets the corresponding object file names 
std::vector<std::string> Get_Object_Files(){
    std::vector<std::string> obj_files = {
        // Core utilities
        "style.o",
        "logger.o",
        "utils.o", 
        "fileStreamer.o",
        "settings.o",
        "drm.o",
        
        // Elements
        "element.o",
        "listView.o",
        "textField.o",
        "switch.o",
        "canvas.o",
        "progressBar.o",
        
        // Specialized elements
        "HTML.o",
        
        // core functionalities
        "renderer.o",
        "thread.o",
        "addons.o"
    };
    return obj_files;
}

int main(int argc, char** argv){
    std::string Double_Command_Mark = " ; ";

#if _WIN32
	Double_Command_Mark = " && ";
#endif
    // Parse simple CLI options for Meson integration
    bool headers_only = false;
    bool include_internal_sections = false;
    std::string out_header = "./ggui.h"; // default to current working directory
    // Default source_root keeps the previous behavior if run from bin/export in source tree
    std::string source_root = "./../../";

    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--headers-only") {
            headers_only = true;
        } else if (arg == "--out" && i + 1 < argc) {
            out_header = argv[++i];
        } else if (arg == "--source-root" && i + 1 < argc) {
            source_root = argv[++i];
        } else if (arg == "--include-internal") {
            include_internal_sections = true;
        }
    }

    // Compile the headers first
    bool ignore_autogen_sections = !include_internal_sections;
    Compile_Headers(out_header, source_root, ignore_autogen_sections);

    if (headers_only) {
        std::cout << "Generated header at: " << out_header << std::endl;
        return 0;
    }

    std::vector<std::string> CPP_Files = Get_Cpp_Files();
    std::vector<std::string> Object_Files = Get_Object_Files();

    // std::string SIMD_Support = Get_Machine_SIMD_Type();
    std::string SIMD_Support = "";

    if (SIMD_Support.size() > 0)
        std::cout << "Using SIMD type: " << SIMD_Support << std::endl;

    std::vector<std::string> flags = {
        "-c",
        "-O3",
        SIMD_Support,
        "-DGGUI_RELEASE",
        "--std=c++17",
        "-fpermissive",
        "-Wno-narrowing",
        "-march=native",
        "-flto",
        "-fwhole-program"
    };

    // Remove empty SIMD flag if not set
    if (SIMD_Support.empty()) {
        flags.erase(std::remove(flags.begin(), flags.end(), ""), flags.end());
    }

    std::string Base_Args = "";
    for (const auto& flag : flags) {
        if (!flag.empty()) {
            Base_Args += " " + flag;
        }
    }
    
#if _WIN32
    Base_Args += " -D_WIN32";
    std::string nativeName = "Win";
    std::string alienName = "Unix";
#else
    Base_Args += " -D__unix__";
    std::string nativeName = "Unix";
    std::string alienName = "Win";
#endif
    std::string virtualRoot = "cd ./../../ " + Double_Command_Mark;

    // Compile each source file individually
    std::cout << "Compiling individual source files..." << std::endl;
    for (size_t i = 0; i < CPP_Files.size(); ++i) {
        const std::string& cpp_file = CPP_Files[i];
        const std::string& obj_file = Object_Files[i];
        
        std::string compile_command = "g++" + Base_Args + " -o " + obj_file + " " + cpp_file;
        
        std::cout << "Compiling: " << cpp_file << " -> " << obj_file << std::endl;
        int result = system((virtualRoot + compile_command).c_str());
        if (result != 0) {
            std::cout << "Error compiling " << cpp_file << std::endl;
            return 1;
        }
    }

    // Create library from all object files
    std::string obj_files_list = "";
    for (const auto& obj : Object_Files) {
        obj_files_list += " " + obj;
    }

#if _WIN32
    std::string Command = "ar rcs bin/export/libggui.lib" + obj_files_list;
    std::cout << "Creating library: bin/export/libggui.lib" << std::endl;
#else
    std::string Command = "ar rcs bin/export/libggui.a" + obj_files_list;
    std::cout << "Creating library: bin/export/libggui.a" << std::endl;
#endif

    {
        std::string full_cmd = virtualRoot + Command;
        int rc = system(full_cmd.c_str());
        if (rc != 0) {
            std::cerr << "Warning: command failed: " << full_cmd << " (return=" << rc << ")" << std::endl;
        }
    }

#if _WIN32
    // Cross-compile for Unix if on Windows
    std::cout << "Cross-compiling for Unix..." << std::endl;
    for (size_t i = 0; i < CPP_Files.size(); ++i) {
        const std::string& cpp_file = CPP_Files[i];
        const std::string& obj_file = Object_Files[i];
        
        std::string compile_command = "x86_64-w64-mingw32-g++" + Base_Args + " -o " + obj_file + " " + cpp_file;
        
        int result = system((virtualRoot + compile_command).c_str());
        if (result != 0) {
            std::cout << "Warning: Cross-compilation failed for " << cpp_file << std::endl;
        }
    }
    
    Command = "ar rcs bin/export/libggui.a" + obj_files_list;
    {
        std::string full_cmd = virtualRoot + Command;
        int rc = system(full_cmd.c_str());
        if (rc != 0) {
            std::cerr << "Warning: command failed: " << full_cmd << " (return=" << rc << ")" << std::endl;
        }
    }
#else
    // Cross-compile for Windows if on Unix
    std::cout << "Cross-compiling for Windows..." << std::endl;
    for (size_t i = 0; i < CPP_Files.size(); ++i) {
        const std::string& cpp_file = CPP_Files[i];
        const std::string& obj_file = Object_Files[i];
        
        // If this is not found install it with: sudo apt install g++-mingw-w64-x86-64-posix
        std::string compile_command = "x86_64-w64-mingw32-g++-posix" + Base_Args + " -o " + obj_file + " " + cpp_file;
        
        int result = system((virtualRoot + compile_command).c_str());
        if (result != 0) {
            std::cout << "Warning: Cross-compilation failed for " << cpp_file << std::endl;
        }
    }
    
    Command = "x86_64-w64-mingw32-ar rcs bin/export/libggui.lib" + obj_files_list;
    {
        std::string full_cmd = virtualRoot + Command;
        int rc = system(full_cmd.c_str());
        if (rc != 0) {
            std::cerr << "Warning: command failed: " << full_cmd << " (return=" << rc << ")" << std::endl;
        }
    }
#endif

    // Clean the *.o files from the project root
#if _WIN32
    Command = std::string("del *.o");
    (void)(system((virtualRoot + Command).c_str()));
#else
    Command = std::string("rm -f *.o");
    {
        std::string full_cmd = virtualRoot + Command;
        int rc = system(full_cmd.c_str());
        if (rc != 0) {
            std::cerr << "Warning: command failed: " << full_cmd << " (return=" << rc << ")" << std::endl;
        }
    }
#endif

    std::cout << "Static library build completed!" << std::endl;
    return 0;
}