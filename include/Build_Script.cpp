#include <iostream>
#include <string>

using namespace std;

void Compile_Headers(){
    // This function goes through all the header files in the ./Elements/ folder and then concatenates them with \n
    // The resulting buffer is then written into ./include/GGUI.h
#if _WIN32 
    string Command = string("powershell -Command \"Get-Content .\\Elements\\*.h | Out-File -FilePath .\\include\\GGUI.h -Encoding ASCII\"");
#else
    string Command = string("for file in ./Elements/*.h; do cat $file; echo; done > ./include/GGUI.h");
#endif

    system(Command.c_str());
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