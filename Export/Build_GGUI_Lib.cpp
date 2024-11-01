#include <iostream>
#include <string>
#include <unordered_map>
#include <filesystem>
#include <vector>
#include <fstream>
#include <regex>
#include <intrin.h>

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
    std::vector<std::string> Header_Source_Folders = {"../Core/Utils/", "../Elements/", "../Core/", "../Core/SIMD/"};

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

    // close all the files
    Output.close();
}

std::string Get_Machine_SIMD_Type(){
    // Uses the CMD utility to prone 'lscpu' command
    int cpuInfo[4];
    __cpuid(cpuInfo, 1);

    bool sseSupport = cpuInfo[3] & (1 << 25);
    bool avxSupport = cpuInfo[2] & (1 << 28);

    if (avxSupport) return "-mavx";
    if (sseSupport) return "-msse";

    return "";
}

int main(){
    std::string Double_Command_Mark = " ; ";

#if _WIN32
	Double_Command_Mark = " && ";
#endif
    // Compile the headers:
    Compile_Headers();

    // generate the .o file for window and linux

    std::string SIMD_Support = Get_Machine_SIMD_Type();

    if (SIMD_Support.size() > 0)
        std::cout << "Using SIMD type: " << SIMD_Support << std::endl;

    std::string Universal_Args = " -c ./GGUI_Body.cpp -c -O3 -fpermissive -Wno-narrowing " + SIMD_Support + " --std=c++17 ";

    // generate for the main platform this script is run from with gcc
    std::string Command = std::string("g++" + Universal_Args) + Double_Command_Mark + 
    std::string("ar rcs GGUI_Win.lib ./GGUI_Body.o");

#if _WIN32
    // if we are in windows, then generate for unix
    Command += Double_Command_Mark + std::string("x86_64-w64-mingw32-g++" + Universal_Args) + Double_Command_Mark +
    std::string("ar rcs GGUI_Unix.lib ./GGUI_Body.o");
#else
    // if we are on Unix, then generate for windows too
    Command += Double_Command_Mark + std::string("x86_64-w64-mingw32-g++" + Universal_Args) + Double_Command_Mark +
    std::string("x86_64-w64-mingw32-ar rcs GGUI_Win.lib ./GGUI_Body.o");
#endif

    system(Command.c_str());

#if _WIN32
    // Clean the *.o file
    Command = std::string("del GGUI_Body.o");
    system(Command.c_str());
#else
    // Clean the *.o file
    Command = std::string("rm ./GGUI_Body.o");
    system(Command.c_str());
#endif
}