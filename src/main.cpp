#include "ggui.h"

using namespace GGUI;

int main(int argc, char* argv[]){
    
    // This will enable whatever the user gave the args with GGUI
    GGUI::SETTINGS::parseCommandLineArguments(argc, argv);

    GGUI::GGUI(
        backgroundColor(COLOR::YELLOW) | 
        enableBorder(true) |
        position(STYLES::bottom + STYLES::left)
    );

    std::this_thread::sleep_for(std::chrono::milliseconds(UINT32_MAX));
    EXIT();
}
