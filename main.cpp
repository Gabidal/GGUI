#include "ggui.h"

#include <vector>

using namespace std;

void progress(){
    for (auto i : GGUI::Main->Get_Elements<GGUI::Progress_Bar>()){
        while (i->Get_Progress() < 1.0){
            i->Set_Progress(i->Get_Progress() + 0.01);

            GGUI::SLEEP(16);
        }
    }
}

int main(int Argument_Count, char** Arguments){
    GGUI::GGUI([=](){

        GGUI::Progress_Bar* bar = new GGUI::Progress_Bar(
            GGUI::COLOR::CYAN,
            GGUI::COLOR::DARK_BLUE,
            GGUI::Main->Get_Width() - 2
        );
        
        // bar->Show_Border(true);
        GGUI::Main->Add_Child(bar);
    });

    progress();

    GGUI::SLEEP(INT64_MAX);
    
    // Then exit properly
    GGUI::Exit();
}