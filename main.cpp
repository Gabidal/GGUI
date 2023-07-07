#include "GGUI.h"

using namespace GGUI;

int main() 
{
    GGUI::GGUI([&](){    
        GGUI::Main->Set_Title("Your App UI");
        GGUI::Main->Set_Background_Color(COLOR::BLACK);
        GGUI::Main->Set_Text_Color(COLOR::BLACK);
        GGUI::Main->Show_Border(true);

        List_View* bar = new List_View(
            GGUI::Main,
            {
                new Button("File"),
                new Button("Edit"),
                new Button("View"),
                new Button("Help")
            }
        );

        bar->Show_Border(true);

        // for (auto i : bar->Get_Childs()){
        //     i->Show_Border(false);
        // }

        Window* A = new Window("A", 20, 10, COLOR::RED, COLOR::CYAN);
        Window* B = new Window("B", 20, 10, COLOR::GREEN, COLOR::MAGENTA);
        Window* C = new Window("C", 20, 10, COLOR::BLUE, COLOR::YELLOW);

        A->Set_Opacity(0.5f);
        B->Set_Opacity(0.5f);
        C->Set_Opacity(0.5f);

        GGUI::Main->Add_Child(A);
        GGUI::Main->Add_Child(B);
        GGUI::Main->Add_Child(C);

        A->Set_Position({10, 10});
        B->Set_Position({30, 10});
        C->Set_Position({20, 15});
    });    
    
    // GGUI::RGB Increaser = GGUI::RGB(1, 1, 1);
    // while (true){
    //     vector<Window*> Result = GGUI::Main->Get_Elements<Window>();

    //     for (auto& w : Result){

    //         if (w->Get_Opacity() > 100.0f)
    //             w->Set_Opacity(0.0f);
    //         else
    //             w->Set_Opacity((w->Get_Opacity() / 100.0f) + 0.011f);
    //     }
    // }

    GGUI::SLEEP(INT32_MAX);
    return 0;
}