#include "ggui.h"

using namespace GGUI;

int main() 
{
    GGUI::GGUI([&](){

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

        Window* A = new Window("A", 20, 10, COLOR::MAGENTA, COLOR::RED);
        Window* B = new Window("B", 20, 10, COLOR::YELLOW, COLOR::GREEN);
        Window* C = new Window("C", 20, 10, COLOR::CYAN, COLOR::BLUE);

        A->Set_Opacity(0.5f);
        B->Set_Opacity(0.5f);
        C->Set_Opacity(0.5f);

        GGUI::Main->Add_Child(A);
        GGUI::Main->Add_Child(B);
        GGUI::Main->Add_Child(C);

        A->Set_Position({10, 10});
        B->Set_Position({30, 10});
        C->Set_Position({20, 15});

        A->Set_Custom_Border_Style(GGUI::STYLES::BORDER::Round);
        B->Set_Custom_Border_Style(GGUI::STYLES::BORDER::Round);
        C->Set_Custom_Border_Style(GGUI::STYLES::BORDER::Round);
    });
    
    GGUI::Main->Set_Title("Your App UI");
    GGUI::Main->Set_Background_Color(COLOR::WHITE);
    GGUI::Main->Set_Text_Color(COLOR::BLACK);
    GGUI::Main->Show_Border(true);    

    // Your code here...
    _sleep(UINT32_MAX);
}