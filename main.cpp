#include "ggui.h"

#include <vector>

using namespace std;

int main(int Argument_Count, char** Arguments){

    GGUI::GGUI([&](){
        GGUI::Scroll_View* vertical = new GGUI::Scroll_View(GGUI::DIRECTION::ROW);
        vertical->Set_Dimensions(1, 10);

        GGUI::Scroll_View* horizontal = new GGUI::Scroll_View(GGUI::DIRECTION::COLUMN);
        horizontal->Set_Dimensions(10, 1);

        for (int i = 0; i < 10; i++){
            GGUI::Element* tmp = new GGUI::Element(1, 1);
            tmp->Set_Background_Color(GGUI::RGB(rand() % 255, rand() % 255, rand() % 255));

            vertical->Add_Child(tmp);
            horizontal->Add_Child(tmp);
        }

        horizontal->Set_Position({1, 0});

        GGUI::Main->Add_Child(vertical);
        GGUI::Main->Add_Child(horizontal);
    });

    GGUI::Main->Set_Background_Color(GGUI::COLOR::WHITE);

    while (true){
        for (int i = 0; i < 10; i++){
            for (auto scrollable : GGUI::Main->Get_Elements<GGUI::Scroll_View>()){
                scrollable->Scroll_Up();
            }

            GGUI::SLEEP(32);
        }

        for (int i = 0; i < 10; i++){
            for (auto scrollable : GGUI::Main->Get_Elements<GGUI::Scroll_View>()){
                scrollable->Scroll_Down();
            }

            GGUI::SLEEP(32);
        }
    }

    GGUI::SLEEP(INT32_MAX);
    
    // Then exit properly
    GGUI::Exit();
}