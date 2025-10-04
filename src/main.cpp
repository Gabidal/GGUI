#include "ggui.h"

using namespace GGUI;

int main(int argc, char* argv[]){
    switchBox SB(
        visualState("0", "1") | onClick([](element*){ return true; }) // Enable clicking and enter functions
    );

    SB.compile();

    // For an element to get onFocus it needs to be hovered upon first:
    INTERNAL::Mouse = {0, 0};            // Let's test it on all four corners
    INTERNAL::eventHandler();            // run pipeline 
    if (SB.isHovered()) {
        std::cout << "success" << std::endl;
    }

    // Now we can create an click input ourselves and put it into the inputs list and call eventHandler to parse it
    INTERNAL::Inputs.push_back(new GGUI::input(0, constants::MOUSE_LEFT_CLICKED));
    INTERNAL::eventHandler();            // run pipeline

    // // Now we can simply check if SB has isFocused enabled
    if (SB.isFocused()) {
        std::cout << "success" << std::endl;
    }


}
