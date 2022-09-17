#include "ggui.h"

using namespace std;

int main(){
    GGUI::Pause_Renderer();

    GGUI::Window* Main = GGUI::Init_Renderer();
    Main->Set_Back_Ground_Colour({230, 50, 100});
    Main->Set_Title("GGUI");
    Main->Show_Border(true);

    GGUI::List_View* l = new GGUI::List_View({
    {0, 0},
    50, 10
    }, GGUI::Grow_Direction::ROW, false);
    l->Show_Border(true);

    GGUI::Window* a = new GGUI::Window("Window A", {
        {0, 0},
        20, 10,
        true,
        GGUI::COLOR::CYAN,
        GGUI::COLOR::BLACK,

        GGUI::COLOR::CYAN,
        GGUI::COLOR::BLACK,

        GGUI::COLOR::RED,
        GGUI::COLOR::BLACK,

        GGUI::COLOR::RED,
        GGUI::COLOR::BLACK,
    });

    a->On_Click([=](GGUI::Event* e){
        a->Remove();
    });

    GGUI::Text_Field* b = new GGUI::Text_Field("Text Field B", {
        {0, 0},
        20, 10,
        true,
        GGUI::COLOR::CYAN,
        GGUI::COLOR::BLACK,

        GGUI::COLOR::CYAN,
        GGUI::COLOR::BLACK,

        GGUI::COLOR::RED,
        GGUI::COLOR::BLACK,

        GGUI::COLOR::RED,
        GGUI::COLOR::BLACK,
    },
    GGUI::TEXT_LOCATION::LEFT
    );

    b->Enable_Text_Input();
    b->Enable_Input_Overflow();

    Main->Add_Child(l);

    l->Add_Child(a);
    l->Add_Child(b);

    GGUI::Resume_Renderer();

    std::this_thread::sleep_for(std::chrono::milliseconds(GGUI::TIME::HOUR)); 

    return 0;
}