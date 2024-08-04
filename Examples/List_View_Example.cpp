#include "ggui.h"

using namespace std;

int main(){
    GGUI::Pause_GGUI();
    
    GGUI::Window* Main = GGUI::Init_GGUI();

    GGUI::List_View* l = new GGUI::List_View(); 

    Main->Add_Child(l);

    GGUI::Text_Field* Text = new GGUI::Text_Field(
        "A AA AAA AAAA AAAAA AAAAAA AAAAAAA AAAAAAAA \n"
        "B BB BBB BBBB BBBBB BBBBBB BBBBBBB BBBBBBBB \n"
        "C CC CCC CCCC CCCCC CCCCCC CCCCCCC CCCCCCCC \n"
    );

    Text->On_Click([](GGUI::Event* e){
        ((GGUI::Action*)e)->Host->Remove();

        return true;
    });
    
    l->Add_Child(Text);
    
    //Notice that the copy constructor wont set up any event handlers of text to the new owner.
    //In this case the On_Click event remembers the Original Text element memory address and not the new copyed one!
    // l->Add_Child(new GGUI::Text_Field(*Text)); <-- Illegal

    //This in turn will copy the event handlers and update their Host.
    l->Add_Child(Text->Copy());

    //You can also press SHIFT to togle element HOP mode to hop easily between elements.

    GGUI::Resume_GGUI();

    while(true){ 
        //Your programm here :D
    }

    // Then exit properly
    GGUI::Exit();
}