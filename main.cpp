#include "ggui.h"

int main(int Argument_Count, char** Arguments){
    GGUI::GGUI([=](){
        
        GGUI::Main->Set_Background_Color(GGUI::COLOR::BLUE);
        GGUI::Main->Show_Border(true);
        GGUI::Main->Set_Title("abc");

    }, GGUI::TIME::SECOND * 60);
    return 0;
}