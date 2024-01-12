#include "ggui.h"

using namespace GGUI;

int main() 
{
    GGUI::GGUI([=](){    
        // GGUI::Main->Set_Title("Your App UI");
        // GGUI::Main->Set_Background_Color(COLOR::WHITE);
        // GGUI::Main->Set_Text_Color(COLOR::BLACK);
        // GGUI::Main->Show_Border(true);

        List_View* Wrapper = new List_View(
            Main,
            {
                new Button("aaa", [&](Button* self){
                    Wrapper->Display(false);
                })
            },
            Grow_Direction::COLUMN
        );
        Wrapper->Show_Border(true);
        Wrapper->Set_Dimensions(10, 10);
    });   

    GGUI::SLEEP(INT32_MAX);
    return 0;
}