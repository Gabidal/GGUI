#include <iostream>
#include <string>

using namespace std;

int main(){
    string Double_Command_Mark = " ; ";

#if _WIN32
	Double_Command_Mark = " && ";
#endif

    // GCC
    string Command = string("g++ -c ./GGUI_Body.cpp -c -O3 -fpermissive") + Double_Command_Mark + 
    string("ar rcs GGUI.lib ./GGUI_Body.o");
    
    // VSC++
    // string Command = string("cl /c ./GGUI_Body.cpp /O2") + Double_Command_Mark + 
    // string("lib /OUT:GGUI.lib ./GGUI_Body.obj");

    system(Command.c_str());
}