#include <iostream>
#include <string>
#include <unordered_map>
#include <filesystem>
#include <vector>
#include <fstream>
#include <regex>
#include <algorithm>
#if defined(_WIN32)
#include <intrin.h>
#else
#include <cpuid.h>
#endif

using namespace std;

class Header_File{
public:
    unsigned int Use_Count = 0;
    std::string Data = "";

    Header_File(std::string Data){
        this->Data = Data;
    }

    Header_File() = default;
};

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

void Compile_Headers(){
    // This function goes through all the header files in the `../Elements/` and `../Core/` and `../Core/SIMD/` folders and then concatenates them with \n
    // The resulting buffer is then written into ./GGUI.h

    // We need to do these two things for the automatic header combination:
    // - Remove local #include "" lines.
    // Order the files before combination in the including order so that the files which are included the most are at the top. 

    std::string Destination_File_Name = "./GGUI.h";
    std::vector<std::string> Header_Source_Folders = {"../../src/core/utils/", "../../src/elements/", "../../src/core/", "../../src/core/SIMD/"};

    std::unordered_map<std::string, Header_File> Header_Files;

    for (auto current_folder : Header_Source_Folders){
        // get all the file names:
        for(const auto& file : get_all_files(current_folder)){
            // read the file
            std::string File_Path = current_folder + file;

            std::ifstream File(File_Path);
            
            std::string Data = "";

            if(File.is_open()){
                std::string Line;
                while(std::getline(File, Line)){
                    Data += Line + "\n";
                }
            }

            // add the file to the map
            Header_Files[file] = Header_File(Data);
        }
    }

    // now on each Header_Files instance we need to use regex to find the lines with local includes like: `#include "localheaderfile.h"`
    std::regex include_regex("#include\\s+\"([^\"]+/)*([^\"/]+\\.h)\"");

    // matches[1] is the file name which is included and matches[0] is the whole line.
    for(auto& Header : Header_Files){
        auto& File = Header.second;
        std::smatch matches;
        while(std::regex_search(File.Data, matches, include_regex)){
            // now that we have a match we need to do two things:
            // First increase the count of the header file which is named in the matches[1]
            // Secondly remove this include line from the File.Data
            std::string Include_File_Name = matches[2].str();

            // increase the count of the header file
            if(Header_Files.find(Include_File_Name) != Header_Files.end()){
                Header_Files[Include_File_Name].Use_Count++;
            }

            // remove the include line from the File.Data
            File.Data = matches.prefix().str() + matches.suffix().str();
        }
    }

    // Transform the map into a vector
    std::vector<std::pair<std::string, Header_File>> headers(Header_Files.begin(), Header_Files.end());

    // Sort the vector by the count, where the less count is closer to zero index.
    sort(headers.begin(), headers.end(), [](const std::pair<std::string, Header_File>& a, const std::pair<std::string, Header_File>& b){
        return a.second.Use_Count > b.second.Use_Count;
    });

    // Now write the output file with the data in the sorted order.
    std::ofstream Output(Destination_File_Name);

    if(Output.is_open()){
        for(auto& Header : headers){
            Output << Header.second.Data;
        }
    } 
}

std::string Get_Machine_SIMD_Type(){
    int cpuInfo[4] = {0};
#if defined(_WIN32)
    __cpuid(cpuInfo, 1);
    bool sseSupport = cpuInfo[3] & (1 << 25);
    bool avxSupport = cpuInfo[2] & (1 << 28);
#else
    unsigned int eax, ebx, ecx, edx;
    __cpuid(1, eax, ebx, ecx, edx);
    bool sseSupport = edx & (1 << 25);
    bool avxSupport = ecx & (1 << 28);
#endif

    if (avxSupport) return "-mavx";
    if (sseSupport) return "-msse";

    return "";
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

int main(){
    std::string Double_Command_Mark = " ; ";

#if _WIN32
	Double_Command_Mark = " && ";
#endif
    // Compile the headers:
    Compile_Headers();

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

    std::string Command = "ar rcs bin/export/GGUI" + nativeName + ".lib" + obj_files_list;
    std::cout << "Creating library: bin/export/GGUI" << nativeName << ".lib" << std::endl;
    system((virtualRoot + Command).c_str());

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
    
    Command = "ar rcs bin/export/GGUI" + alienName + ".lib" + obj_files_list;
    system((virtualRoot + Command).c_str());
#else
    // Cross-compile for Windows if on Unix
    std::cout << "Cross-compiling for Windows..." << std::endl;
    for (size_t i = 0; i < CPP_Files.size(); ++i) {
        const std::string& cpp_file = CPP_Files[i];
        const std::string& obj_file = Object_Files[i];
        
        std::string compile_command = "x86_64-w64-mingw32-g++" + Base_Args + " -o " + obj_file + " " + cpp_file;
        
        int result = system((virtualRoot + compile_command).c_str());
        if (result != 0) {
            std::cout << "Warning: Cross-compilation failed for " << cpp_file << std::endl;
        }
    }
    
    Command = "x86_64-w64-mingw32-ar rcs bin/export/GGUI" + alienName + ".lib" + obj_files_list;
    system((virtualRoot + Command).c_str());
#endif

    // Clean the *.o files from the project root
#if _WIN32
    Command = std::string("del *.o");
    system((virtualRoot + Command).c_str());
#else
    Command = std::string("rm -f *.o");
    system((virtualRoot + Command).c_str());
#endif

    std::cout << "Static library build completed!" << std::endl;
    return 0;
}