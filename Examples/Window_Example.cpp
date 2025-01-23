#include "renderer.h"

using namespace std;


int main(){
    //Pause renderer and resume renderer are only for super optimization purposes, if you dont need blazing fast TUI you can just ignore them.
    GGUI::window* Main = GGUI::initGGUI();
    Main->showBorder(true);
    
    GGUI::window* Text = new GGUI::window(
        "Hello World!", 
        20,
        10,

        GGUI::COLOR::CYAN,
        GGUI::COLOR::DARK_MAGENTA,

        GGUI::COLOR::WHITE,
        GGUI::COLOR::LIGHT_BLUE
    );

    // Center the text element
    Text->setPosition({Main->getWidth()/2 - Text->getWidth()/2, Main->getHeight()/2 - Text->getHeight()/2});
    
    Main->addChild(Text);

    while(true){ 
        //Your programm here :D
    }

    // Then exit properly
    GGUI::EXIT();
}