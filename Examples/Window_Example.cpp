#include "Renderer.h"

using namespace std;


int main(){
    //Pause renderer and resume renderer are only for super optimization purposes, if you dont need blazing fast TUI you can just ignore them.
    GGUI::Pause_Renderer();
    GGUI::Window* Main = GGUI::Init_Renderer();
    Main->Show_Border(true);
    
    GGUI::Window* Text = new GGUI::Window(
        "Hello World!", 
        10,
        10,

        GGUI::COLOR::CYAN,
        GGUI::COLOR::DARK_MAGENTA,

        GGUI::COLOR::WHITE,
        GGUI::COLOR::LIGHT_BLUE
    );
    
    Main->Add_Child(Text);

    GGUI::Resume_Renderer();

    while(true){ 
        //Your programm here :D
    }

}