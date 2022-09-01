#include "ggui.h"

using namespace std;

int main(){
    GGUI::Pause_Renderer();
    
    GGUI::Window* Main = GGUI::Init_Renderer();

    GGUI::List_View* l = new GGUI::List_View({}, GGUI::Grow_Direction::ROW, false); 

    Main->Add_Child(l);
    
    GGUI::Text_Field* Text = new GGUI::Text_Field(
        "A AA AAA AAAA AAAAA AAAAAA AAAAAAA AAAAAAAA \n"
        "B BB BBB BBBB BBBBB BBBBBB BBBBBBB BBBBBBBB \n"
        "C CC CCC CCCC CCCCC CCCCCC CCCCCCC CCCCCCCC \n"
    );
    Text->Set_Back_Ground_Colour(GGUI::COLOR::DARK_MAGENTA);
    Text->Set_Border_Back_Ground_Color(GGUI::COLOR::LIGHT_BLUE);
    Text->Set_Text_Colour(GGUI::COLOR::CYAN);
    //Text->Show_Border(true);

    Text->On_Click([=](GGUI::Event* e){
        Text->Remove();
    });
    
    l->Add_Child(Text);
    l->Add_Child(new GGUI::Text_Field(*Text));
    l->Add_Child(new GGUI::Text_Field(*Text));

    GGUI::Resume_Renderer();

    while(true){ 
        //Your programm here :D
    }

}