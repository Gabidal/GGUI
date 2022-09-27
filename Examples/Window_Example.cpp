#include "Renderer.h"

using namespace std;


int main(){
    //Pause renderer and resume renderer are only for super optimization purposes, if you dont need blazing fast TUI you can just ignore them.
    GGUI::Pause_Renderer();
    GGUI::Window* Main = GGUI::Init_Renderer();
    Main->Show_Border(true);
    
    GGUI::Window* Text = new GGUI::Window("Hello World!", {{0, 0}, 20, 10, true});
    Text->Set_Back_Ground_Color(GGUI::COLOR::DARK_MAGENTA);
    Text->Set_Border_Back_Ground_Color(GGUI::COLOR::LIGHT_BLUE);
    Text->Set_Text_Color(GGUI::COLOR::CYAN);
    
    
    Main->Add_Child(Text);

    GGUI::Resume_Renderer();

    while(true){ 
        //Your programm here :D
    }

}