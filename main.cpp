#include "ggui.h"

#include <vector>

using namespace std;

int main(int Argument_Count, char** Arguments){
    GGUI::GGUI([&](){

        GGUI::Window* A = new GGUI::Window("A", 20, 10, GGUI::COLOR::MAGENTA, GGUI::COLOR::RED);
        GGUI::Window* B = new GGUI::Window("B", 20, 10, GGUI::COLOR::YELLOW, GGUI::COLOR::GREEN);
        GGUI::Window* C = new GGUI::Window("C", 20, 10, GGUI::COLOR::CYAN, GGUI::COLOR::BLUE);

        A->Set_Opacity(0.5f);
        B->Set_Opacity(0.5f);
        C->Set_Opacity(0.5f);

        GGUI::Main->Add_Child(A);
        GGUI::Main->Add_Child(B);
        GGUI::Main->Add_Child(C);

        A->Set_Position({10, 10});
        B->Set_Position({30, 10});
        C->Set_Position({20, 15});

        GGUI::Main->Set_Background_Color(GGUI::COLOR::WHITE);

        GGUI::Main->Set_Title(to_string(GGUI::Frame_Buffer.size()));
        GGUI::Main->Set_Border_Color(GGUI::COLOR::RED);
        GGUI::Main->Set_Text_Color(GGUI::COLOR::BLUE);

        B->Show_Shadow(GGUI::COLOR::BLUE, 1, 10.0f);
    });

    GGUI::SLEEP(INT32_MAX);
    return 0;
}