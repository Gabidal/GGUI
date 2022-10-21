#include "ggui.h"

#include <vector>

using namespace std;

int main(){
    GGUI::Pause_Renderer();

    GGUI::Window* Main = GGUI::Init_Renderer();
    Main->Show_Border(true);

    GGUI::List_View* l = new GGUI::List_View(
        GGUI::COLOR::WHITE,
        GGUI::COLOR::WHITE
    );

    GGUI::Window* a = new GGUI::Window(
        "Window A", 
        20,
        10,

        GGUI::COLOR::RED,
        GGUI::COLOR::BLUE
    );

    a->On_Click([=](GGUI::Event* e){
        a->Remove();
    });

    GGUI::Text_Field* b = new GGUI::Text_Field(
        "Text Field B",

        GGUI::COLOR::GREEN,
        GGUI::COLOR::BLACK,

        GGUI::COLOR::CYAN,
        GGUI::COLOR::DARK_MAGENTA 
    );

    GGUI::Check_Box* rb = new GGUI::Check_Box("Halooo?");

    GGUI::Canvas* c = new GGUI::Canvas(10, 10);

    c->Set(5, 5, {123, 221, 132});

    b->Enable_Text_Input();

    Main->Add_Child(l);

    a->Set_Name("A");
    b->Set_Name("B");

    l->Add_Child(a);

    l->Add_Child(b);

    l->Add_Child(rb);

    l->Add_Child(c);

    std::vector<GGUI::Text_Field*> r = Main->Get_Elements<GGUI::Text_Field>();

    GGUI::Element* r2 = Main->Get_Element("A");

    GGUI::Resume_Renderer();

    std::this_thread::sleep_for(std::chrono::milliseconds(GGUI::TIME::HOUR)); 

    return 0;
}