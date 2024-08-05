#include "ggui.h"

#include <vector>

using namespace std;

void Main(){
    while (true)
        for (auto i : GGUI::Main->Get_Elements<GGUI::Progress_Bar>()){
            i->Set_Progress(i->Get_Progress() + 0.01);
        }
}

int main(int Argument_Count, char** Arguments){
    GGUI::GGUI([=](){

        GGUI::Progress_Bar* arrow = new GGUI::Progress_Bar(GGUI::Main->Get_Width(), 1, GGUI::Progress_Bar_Styles::Arrow);
        GGUI::Progress_Bar* slim = new GGUI::Progress_Bar(GGUI::Main->Get_Width(), 1, GGUI::Progress_Bar_Styles::Default);
        GGUI::Progress_Bar* blocky = new GGUI::Progress_Bar(GGUI::Main->Get_Width(), 3, GGUI::Progress_Bar_Styles::Blocky);
        
        slim->Set_Position({0, arrow->Get_Height()});
        blocky->Set_Position({0, slim->Get_Height() + slim->Get_Position().Y});

        GGUI::Main->Add_Child(arrow);
        GGUI::Main->Add_Child(slim);
        GGUI::Main->Add_Child(blocky);
    });

    Main();

    GGUI::SLEEP(INT64_MAX);
    
    // Then exit properly
    GGUI::Exit();
}