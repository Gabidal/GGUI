#include "ggui.h"

#include <vector>

using namespace std;

int main(int Argument_Count, char** Arguments){
    GGUI::Window* A;
    GGUI::Window* B;
    GGUI::Window* C;

    GGUI::GGUI([&](){

        A = new GGUI::Window("A", 20, 10, GGUI::COLOR::MAGENTA, GGUI::COLOR::RED);
        B = new GGUI::Window("B", 20, 10, GGUI::COLOR::YELLOW, GGUI::COLOR::GREEN);
        C = new GGUI::Window("C", 20, 10, GGUI::COLOR::CYAN, GGUI::COLOR::BLUE);

        A->Set_Opacity(0.5f);
        B->Set_Opacity(0.5f);
        C->Set_Opacity(0.5f);

        GGUI::Main->Add_Child(A);
        GGUI::Main->Add_Child(B);
        GGUI::Main->Add_Child(C);

        A->Set_Position({10, 10});
        B->Set_Position({30, 10});
        C->Set_Position({20, 15});

        B->Show_Shadow(GGUI::COLOR::BLUE, 1, 3.0f);
    });

    GGUI::Coordinates A_velocity = {1, 2};
    GGUI::Coordinates B_velocity = {3, 1};
    GGUI::Coordinates C_velocity = {1, 1};

    while (true){
        GGUI::Pause_Renderer();
        // Update the position of each window
        A->Set_Position(A->Get_Position() + A_velocity);
        B->Set_Position(B->Get_Position() + B_velocity);
        C->Set_Position(C->Get_Position() + C_velocity);

        // Check if any window hits an edge and reverse its direction
        if (A->Get_Position().X <= 0 || A->Get_Position().X + A->Get_Processed_Width() >= GGUI::Main->Get_Processed_Width())
            A_velocity.X = -A_velocity.X;
        if (A->Get_Position().Y <= 0 || A->Get_Position().Y + A->Get_Processed_Height() >= GGUI::Main->Get_Processed_Height())
            A_velocity.Y = -A_velocity.Y;

        if (B->Get_Position().X <= 0 || B->Get_Position().X + B->Get_Processed_Width() >= GGUI::Main->Get_Processed_Width())
            B_velocity.X = -B_velocity.X;
        if (B->Get_Position().Y <= 0 || B->Get_Position().Y + B->Get_Processed_Height() >= GGUI::Main->Get_Processed_Height())
            B_velocity.Y = -B_velocity.Y;

        if (C->Get_Position().X <= 0 || C->Get_Position().X + C->Get_Processed_Width() >= GGUI::Main->Get_Processed_Width())
            C_velocity.X = -C_velocity.X;
        if (C->Get_Position().Y <= 0 || C->Get_Position().Y + C->Get_Processed_Height() >= GGUI::Main->Get_Processed_Height())
            C_velocity.Y = -C_velocity.Y;
        
        GGUI::Resume_Renderer();
        GGUI::Report(to_string(A->Get_Position().X));
        GGUI::SLEEP(16);
    }

    GGUI::SLEEP(INT32_MAX);
    return 0;
}