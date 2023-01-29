#include "Renderer.h"

using namespace std;

int main(){
    GGUI::Pause_Renderer();
    
    GGUI::Window* Main = GGUI::Init_Renderer();
    
    GGUI::Text_Field* Text = new GGUI::Text_Field(
        "A AA AAA AAAA AAAAA AAAAAA AAAAAAA AAAAAAAA \n"
        "B BB BBB BBBB BBBBB BBBBBB BBBBBBB BBBBBBBB \n"
        "C CC CCC CCCC CCCCC CCCCCC CCCCCCC CCCCCCCC \n",
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