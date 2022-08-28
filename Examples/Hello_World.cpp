#include "Renderer.h"

using namespace std;


int main(){
    RENDERER::Pause_Renderer();
    GGUI::Window* Main = RENDERER::Init_Renderer();
    Main->Show_Border(true);
    
    GGUI::Window* Text = new GGUI::Window("Hello World!", {{0, 0}, 20, 10, true});
    Text->Set_Back_Ground_Colour(GGUI::COLOR::DARK_MAGENTA);
    Text->Set_Border_Back_Ground_Color(GGUI::COLOR::LIGHT_BLUE);
    Text->Set_Text_Colour(GGUI::COLOR::CYAN);
    
    
    Main->Add_Child(Text);

    RENDERER::Resume_Renderer();

    while(true){ 
        //Your programm here :D
    }

}