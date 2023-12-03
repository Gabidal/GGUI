#include "ggui.h"

using namespace GGUI;

int main() 
{
    GGUI::GGUI([&](){    
        GGUI::Main->Set_Title("Your App UI");
        GGUI::Main->Set_Background_Color(COLOR::WHITE);
        GGUI::Main->Set_Text_Color(COLOR::BLACK);
        GGUI::Main->Show_Border(true);

        //GGUI::HTML* Test = new GGUI::HTML("Examples/simple.html");
    });   

    for (int i = 0; i < 1000000; i++){
        Report("aaa" + to_string(i));
    } 

    GGUI::SLEEP(INT32_MAX);
    return 0;
}