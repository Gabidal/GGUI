#include "ggui.h"

#include <vector>

using namespace std;

int main(int Argument_Count, char** Arguments){
    GGUI::Window* A;
    GGUI::Window* B;
    GGUI::Window* C;

    GGUI::GGUI([&](){

        A = new GGUI::Window("AAA", 50, 20, GGUI::COLOR::RED, GGUI::COLOR::BLUE, GGUI::COLOR::GREEN, GGUI::COLOR::RED);
        B = new GGUI::Window("BBB", 50, 20, GGUI::COLOR::GREEN, GGUI::COLOR::RED, GGUI::COLOR::BLUE, GGUI::COLOR::GREEN);
        C = new GGUI::Window("CCC", 50, 20, GGUI::COLOR::BLUE, GGUI::COLOR::GREEN, GGUI::COLOR::RED, GGUI::COLOR::BLUE);


        A->Show_Border(true);
        B->Show_Border(true);

        //B->Set_Opacity(0.5f);

        GGUI::Main->Add_Child(A);
        GGUI::Main->Add_Child(C);
        GGUI::Main->Add_Child(B);

        A->Set_Position({35, 0});
        B->Set_Position({20, 30});
        C->Set_Position({0, 0});

    });

    //B->Set_Background_Color(GGUI::COLOR::DARK_BLUE);
    B->Set_Opacity(0.8f);
    A->Set_Opacity(0.8f);
    C->Set_Opacity(0.8f);

    B->Show_Shadow({1, 1}, GGUI::COLOR::WHITE);

    while (true){

        B->Set_Position({B->Get_Position().X, B->Get_Position().Y - 1});

        GGUI::SLEEP(500);
    }

    GGUI::SLEEP(INT32_MAX);
    return 0;
}