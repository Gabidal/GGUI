#include "ggui.h"

#include <math.h>

using namespace GGUI;

int main() 
{
    GGUI::GGUI([&](){
        GGUI::Terminal_Canvas* tem = new GGUI::Terminal_Canvas(GGUI::Max_Width, GGUI::Max_Height, {0, 0});


        for (int x = 0; x < tem->Get_Width(); x++){
            for (int y = 0; y < tem->Get_Height(); y++){
                GGUI::Sprite s(
                    {
                        {"a", {GGUI::COLOR::RED /*text color*/, GGUI::COLOR::RED /*background color*/}}, 
                        {"b", {GGUI::COLOR::BLUE, GGUI::COLOR::BLUE}}, 
                    },
                    0,  // Animation offset
                    1   // Animation speed
                );

                tem->Set(x, y, s);
            }
        }

        GGUI::Main->Add_Child(tem);
    }, 100);

    GGUI::Exit();
}