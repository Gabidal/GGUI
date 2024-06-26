#include "Renderer.h"

using namespace std;


int main(){
    //Pause renderer and resume renderer are only for super optimization purposes, if you dont need blazing fast TUI you can just ignore them.
    GGUI::Window* Main = GGUI::Init_Renderer();
    Main->Show_Border(true);
    
    GGUI::Window* Text = new GGUI::Window(
        "Hello World!", 
        20,
        10,

        GGUI::COLOR::CYAN,
        GGUI::COLOR::DARK_MAGENTA,

        GGUI::COLOR::WHITE,
        GGUI::COLOR::LIGHT_BLUE
    );

    // Center the text element
    Text->Set_Position({Main->Get_Width()/2 - Text->Get_Width()/2, Main->Get_Height()/2 - Text->Get_Height()/2});
    
    Main->Add_Child(Text);

    while(true){ 
        //Your programm here :D
    }

    // Then exit properly
    GGUI::Exit();
}