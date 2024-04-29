#include "ggui.h"

#include <math.h>

using namespace GGUI;

int main() 
{
    GGUI::GGUI([&](){

        GGUI::Terminal_Canvas* tem = new GGUI::Terminal_Canvas(GGUI::Max_Width, GGUI::Max_Height, {0, 0});


        for (int x = 0; x < tem->Get_Width(); x++){
            for (int y = 0; y < tem->Get_Height(); y++){
                GGUI::Sprite s({
                    {"a", {GGUI::COLOR::ORANGE, GGUI::COLOR::DARK_BLUE}}, 
                    {"b", {GGUI::COLOR::BLUE, GGUI::COLOR::WHITE}}, 
                    {"c", {GGUI::COLOR::RED, GGUI::COLOR::YELLOW}}}, 
                    x,
                    y
                );

                tem->Set(x, y, s);
            }
        }

        GGUI::Main->Add_Child(tem);
    });  

    // Your code here...
    _sleep(UINT32_MAX);
}