#include <iostream>
#include <string>

using namespace std;

int main(){
    string Double_Command_Mark = " ; ";

#if _WIN32
	Double_Command_Mark = " && ";
#endif

    string Command = string("g++ -c ./GGUI_Body.cpp -c -O3 -fpermissive") + Double_Command_Mark + 
    string("ar rcs ./libGGUI.lib ./GGUI_Body.o");

    system(Command.c_str());

}