#include "ggui.h"

#include <vector>

using namespace std;

int main(int Argument_Count, char** Arguments){

    GGUI::Scroll_View* scrollable;

    GGUI::GGUI([&](){
        scrollable = new GGUI::Scroll_View(GGUI::Grow_Direction::ROW);
        scrollable->Set_Background_Color(GGUI::COLOR::RED);
        scrollable->Allow_Dynamic_Size(true);

        GGUI::Element child = GGUI::Element(5, 5);

        for (int a = 0; a < 10; a++){
            child.Set_Background_Color(GGUI::RGB(rand() % 255, rand() % 255, rand() % 255));
            scrollable->Add_Child(child.Copy());
        }

        GGUI::Main->Add_Child(scrollable);
    });

    while (true){
        GGUI::SLEEP(100);

        scrollable->Scroll_Down();
    }

    GGUI::SLEEP(INT32_MAX);
    return 0;
}