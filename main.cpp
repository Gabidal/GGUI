#include "ggui.h"

using namespace GGUI;

int main() 
{
    GGUI::GGUI([&](){    
        GGUI::Main->Set_Title("Your App UI");
        GGUI::Main->Set_Background_Color(COLOR::WHITE);
        GGUI::Main->Set_Text_Color(COLOR::BLACK);
        GGUI::Main->Show_Border(true);

        Element* Wrapper = new Element(10, 10);

        Terminal_Canvas* map = new Terminal_Canvas(5, 5, {0, 0});

        // fill the map with random values
        for (int y = 0; y < map->Get_Width(); y++){
            for (int x = 0; x < map->Get_Height(); x++){
                Sprite tmp;

                tmp.Background_Color.R = rand() % 255;
                tmp.Background_Color.G = rand() % 255;
                tmp.Background_Color.B = rand() % 255;

                map->Set(x, y, tmp, false);
            }
        }

        map->Flush();

        Wrapper->Add_Child(map);

        Main->Add_Child(Wrapper);
    });   

    GGUI::SLEEP(INT32_MAX);
    return 0;
}