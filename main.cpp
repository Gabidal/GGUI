#include "ggui.h"

#include <vector>

using namespace std;

int main(){
    GGUI::Pause_Renderer();

    GGUI::Window* Main = GGUI::Init_Renderer();
    Main->Set_Back_Ground_Color({230, 50, 100});
    // Main->Set_Title("GGUI");
    // Main->Show_Border(true);

    GGUI::List_View* l = new GGUI::List_View({
        //{GGUI::STYLES::Border, new GGUI::BOOL_VALUE(true)},

        // {GGUI::STYLES::Width, new GGUI::NUMBER_VALUE(50)},
        // {GGUI::STYLES::Height, new GGUI::NUMBER_VALUE(10)},

        {GGUI::STYLES::Back_Ground_Color, new GGUI::RGB_VALUE(GGUI::COLOR::CYAN)},
    });

    GGUI::Window* a = new GGUI::Window("Window A", {
        {GGUI::STYLES::Border, new GGUI::BOOL_VALUE(true)},

        {GGUI::STYLES::Back_Ground_Color, new GGUI::RGB_VALUE(GGUI::COLOR::CYAN)},
        {GGUI::STYLES::Text_Color, new GGUI::RGB_VALUE(GGUI::COLOR::BLACK)},

        {GGUI::STYLES::Border_Back_Ground_Color, new GGUI::RGB_VALUE(GGUI::COLOR::CYAN)},
        {GGUI::STYLES::Border_Colour, new GGUI::RGB_VALUE(GGUI::COLOR::BLACK)},

        {GGUI::STYLES::Back_Ground_Focus_Color, new GGUI::RGB_VALUE(GGUI::COLOR::RED)},
        {GGUI::STYLES::Text_Focus_Color, new GGUI::RGB_VALUE(GGUI::COLOR::BLACK)},

        {GGUI::STYLES::Border_Focus_Back_Ground_Color, new GGUI::RGB_VALUE(GGUI::COLOR::RED)},
        {GGUI::STYLES::Border_Focus_Color, new GGUI::RGB_VALUE(GGUI::COLOR::BLACK)},
    }, 10, 10);

    a->On_Click([=](GGUI::Event* e){
        a->Remove();
    });

    // GGUI::Text_Field* b = new GGUI::Text_Field("Text Field B", {
    //     {0, 0},
    //     20, 10,
    //     true,
    //     GGUI::COLOR::CYAN,
    //     GGUI::COLOR::BLACK,

    //     GGUI::COLOR::CYAN,
    //     GGUI::COLOR::BLACK,

    //     GGUI::COLOR::RED,
    //     GGUI::COLOR::BLACK,

    //     GGUI::COLOR::RED,
    //     GGUI::COLOR::BLACK,
    // },
    // GGUI::TEXT_LOCATION::LEFT
    // );

    GGUI::Text_Field* b = new GGUI::Text_Field("Text Field B", {
        {GGUI::STYLES::Border, new GGUI::BOOL_VALUE(true)},

        {GGUI::STYLES::Back_Ground_Color, new GGUI::RGB_VALUE(GGUI::COLOR::CYAN)},
        {GGUI::STYLES::Text_Color, new GGUI::RGB_VALUE(GGUI::COLOR::BLACK)},

        {GGUI::STYLES::Border_Back_Ground_Color, new GGUI::RGB_VALUE(GGUI::COLOR::CYAN)},
        {GGUI::STYLES::Border_Colour, new GGUI::RGB_VALUE(GGUI::COLOR::BLACK)},

        {GGUI::STYLES::Back_Ground_Focus_Color, new GGUI::RGB_VALUE(GGUI::COLOR::RED)},
        {GGUI::STYLES::Text_Focus_Color, new GGUI::RGB_VALUE(GGUI::COLOR::BLACK)},

        {GGUI::STYLES::Border_Focus_Back_Ground_Color, new GGUI::RGB_VALUE(GGUI::COLOR::RED)},
        {GGUI::STYLES::Border_Focus_Color, new GGUI::RGB_VALUE(GGUI::COLOR::BLACK)},

        {GGUI::STYLES::Allow_Input_Overflow, new GGUI::BOOL_VALUE(true)},
    });

    b->Enable_Text_Input();

    Main->Add_Child(l);

    a->Set_Name("A");
    b->Set_Name("B");

    l->Add_Child(a);

    l->Add_Child(b);

    std::vector<GGUI::Text_Field*> r = Main->Get_Elements<GGUI::Text_Field>();

    GGUI::Element* r2 = Main->Get_Element("A");

    GGUI::Resume_Renderer();

    std::this_thread::sleep_for(std::chrono::milliseconds(GGUI::TIME::HOUR)); 

    return 0;
}