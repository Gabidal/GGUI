#include <iostream>
#include <string>
#include <unordered_map>
#include <filesystem>
#include <vector>
#include <fstream>
#include <regex>

using namespace std;

class Header_File{
public:
    unsigned int Use_Count = 0;
    string Data = "";

    Header_File(string Data){
        this->Data = Data;
    }

    Header_File() = default;
};

// returns all the file names in the dir.
vector<string> get_all_files(const string& directory) {
    vector<string> files;
    for (const auto & entry : std::filesystem::directory_iterator(directory)) {

        // Make sure that only header filers are selected:
        if(entry.path().extension() != ".h") continue;

        files.push_back(entry.path().filename().string());
    }

    // Also remember to add the Renderer.h
    files.push_back("Renderer.h");

    return files;
}

void Compile_Headers(){
    // This function goes through all the header files in the ./Elements/ folder and then concatenates them with \n
    // The resulting buffer is then written into ./include/GGUI.h

    // We need to do these two things for the automatic header combination:
    // - Remove local #include "" lines.
    // Order the files before combination in the including order so that the files which are included the most are at the top. 

    string Destination_File_Name = "./include/GGUI.h";
    string Header_Source_Folder = "./Elements/";

    unordered_map<string, Header_File> Header_Files;

    // get all the file names:
    for(const auto& file : get_all_files(Header_Source_Folder)){
        // read the file
        string File_Path = Header_Source_Folder + file;

        // Special case for Renderer.h since it is not located aat the same place as the elements.
        if (file == "Renderer.h") File_Path = "./Renderer.h";

        ifstream File(File_Path);
        
        string Data = "";

        if(File.is_open()){
            string Line;
            while(getline(File, Line)){
                Data += Line + "\n";
            }
        }

        // add the file to the map
        Header_Files[file] = Header_File(Data);
    }

    // now on each Header_Files instance we need to use regex to find the lines with local includes like: `#include "localheaderfile.h"`
    regex include_regex(R"(#include \"(.*)\")");


    // matches[1] is the file name which is included and matches[0] is the whole line.
    for(auto& Header : Header_Files){
        auto& File = Header.second;
        smatch matches;
        while(regex_search(File.Data, matches, include_regex)){
            // now that we have a match we need to do two things:
            // First increase the count of the header file which is named in the matches[1]
            // Secondly remove this include line from the File.Data
            string Include_File_Name = matches[1].str();

            // increase the count of the header file
            if(Header_Files.find(Include_File_Name) != Header_Files.end()){
                Header_Files[Include_File_Name].Use_Count++;
            }

            // remove the include line from the File.Data
            File.Data = matches.prefix().str() + matches.suffix().str();
        }
    }

    // Transform the map into a vector
    vector<pair<string, Header_File>> headers(Header_Files.begin(), Header_Files.end());

    // Sort the vector by the count, where the less count is closer to zero index.
    sort(headers.begin(), headers.end(), [](const pair<string, Header_File>& a, const pair<string, Header_File>& b){
        return a.second.Use_Count > b.second.Use_Count;
    });

    // Now write the output file with the data in the sorted order.
    ofstream Output(Destination_File_Name);

    if(Output.is_open()){
        for(auto& Header : headers){
            Output << Header.second.Data;
        }
    } 

    // close all the files
    Output.close();
}

int main(){
    string Double_Command_Mark = " ; ";

#if _WIN32
	Double_Command_Mark = " && ";
#endif
    // Compile the headers:
    Compile_Headers();

    // generate the .o file for window and linux

    // generate for the main platform this script is run from with gcc
    string Command = string("g++ -c ./include/GGUI_Body.cpp -c -O3 -fpermissive") + Double_Command_Mark + 
    string("ar rcs GGUI_Win.lib ./GGUI_Body.o");

#if _WIN32
    // if we are in windows, then generate for unix
    Command += Double_Command_Mark + string("x86_64-w64-mingw32-g++ -c ./include/GGUI_Body.cpp -c -O3 -fpermissive") + Double_Command_Mark + 
    string("ar rcs GGUI_Unix.lib ./GGUI_Body.o");
#else
    // if we are on Unix, then generate for windows too
    Command += Double_Command_Mark + string("x86_64-w64-mingw32-g++ -c ./include/GGUI_Body.cpp -c -O3 -fpermissive") + Double_Command_Mark + 
    string("x86_64-w64-mingw32-ar rcs GGUI_Win.lib ./GGUI_Body.o");
#endif

    system(Command.c_str());

#if _WIN32
    // Clean the *.o file
    Command = string("del GGUI_Body.o");
    system(Command.c_str());
#else
    // Clean the *.o file
    Command = string("rm ./GGUI_Body.o");
    system(Command.c_str());
#endif
}