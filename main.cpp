#include "ggui.h"

using namespace GGUI;

int main() 
{
    GGUI::GGUI([&](){

        GGUI::Terminal_Canvas* tem = new GGUI::Terminal_Canvas(20, 20, {10, 10});

        GGUI::Sprite s({{
            "a", {GGUI::COLOR::ORANGE, GGUI::COLOR::DARK_BLUE}
        }, {"b", {GGUI::COLOR::BLUE, GGUI::COLOR::WHITE}}, {"c", {GGUI::COLOR::RED, GGUI::COLOR::YELLOW}}}, 0, 5);

        tem->Set(0, 0, s);

        GGUI::Main->Add_Child(tem);
    });  

    // Your code here...
    _sleep(UINT32_MAX);
}