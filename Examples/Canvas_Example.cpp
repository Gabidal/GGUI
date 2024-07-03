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
                        {"a", {GGUI::RGB(x, y, x), GGUI::RGB(x, y, x)}}, 
                        {"b", {GGUI::RGB(y, x, y), GGUI::RGB(y, x, y)}}, 
                    },
                    0,
                    1
                );

                tem->Set(x, y, s);
            }
        }

        GGUI::Main->Add_Child(tem);
    }, UINT32_MAX);

    GGUI::Exit();
}