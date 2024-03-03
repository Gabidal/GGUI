#include "ggui.h"

#include <vector>

using namespace std;

int main(int Argument_Count, char** Arguments){
    GGUI::Scroll_View* scrollable;
    GGUI::Element* child;

    GGUI::GGUI([&](){
        scrollable = new GGUI::Scroll_View(GGUI::Grow_Direction::ROW);
        scrollable->Set_Background_Color(GGUI::COLOR::RED);
        scrollable->Set_Dimensions(10, 10);

        child = new GGUI::Element(1, 1);

        for (int a = 0; a < 10; a++){
            child->Set_Background_Color(GGUI::RGB(rand() % 255, rand() % 255, rand() % 255));
            scrollable->Add_Child(child->Copy());
        }

        GGUI::Main->Add_Child(scrollable);
    });

    // Now add the one child over the parent area.
    child->Set_Background_Color(GGUI::COLOR::BLUE);
    scrollable->Add_Child(child->Copy());

    scrollable->Scroll_Down();

    GGUI::SLEEP(INT32_MAX);
    return 0;
}