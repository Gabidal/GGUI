#include "ggui.h"

using namespace GGUI;

int main(int argc, char* argv[]){
    
    // This will enable whatever the user gave the args with GGUI
    GGUI::SETTINGS::parseCommandLineArguments(argc, argv);

    // we can also enable/disable same flags manually via the SETTINGS namespace
    //SETTINGS::enableDRM = true;   <-- example
}