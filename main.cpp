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
                        {"a", {GGUI::RGB(x, y, 0), GGUI::RGB(x, y, 0)}}, 
                        {"b", {GGUI::RGB(y, x, 255), GGUI::RGB(y, x, 255)}}, 
                    },
                    0,
                    2
                );

                tem->Set(x, y, s);
            }
        }

        GGUI::Main->Add_Child(tem);
    });

    GGUI::SLEEP(1000000);
    GGUI::Exit(); 
}