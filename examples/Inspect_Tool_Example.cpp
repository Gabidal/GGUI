#include "ggui.h"

using namespace GGUI;

int main() 
{
    GGUI::GGUI([&](){    
        GGUI::Main->Set_Title("Your App UI");
        GGUI::Main->Set_Background_Color(COLOR::WHITE);
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

        Window* A = new Window("A", 20, 10, COLOR::RED, COLOR::CYAN, COLOR::RED);
        Window* B = new Window("B", 20, 10, COLOR::GREEN, COLOR::MAGENTA, COLOR::GREEN);
        Window* C = new Window("C", 20, 10, COLOR::BLUE, COLOR::YELLOW, COLOR::BLUE);

        A->Set_Opacity(0.5f);
        B->Set_Opacity(0.5f);
        C->Set_Opacity(0.5f);

        GGUI::Main->Add_Child(A);
        GGUI::Main->Add_Child(B);
        GGUI::Main->Add_Child(C);

        A->Set_Position({10, 10});
        B->Set_Position({30, 10});
        C->Set_Position({20, 15});

        GGUI::Window* inspect = new GGUI::Window(
            "Inspect",
            Main->Get_Width() / 3,
            Main->Get_Height()
        );

        inspect->Show_Border(true);
        inspect->Set_Position({
            Main->Get_Width() - (Main->Get_Width() / 3),
            0,
            INT32_MAX - 1,
        });

        inspect->Display(false);
        Main->Add_Child(inspect);

        GGUI::Main->On(Constants::SHIFT | Constants::CONTROL | Constants::KEY_PRESS, [inspect](GGUI::Event* e){
            GGUI::Input* input = (GGUI::Input*)e;

            if (!KEYBOARD_STATES[BUTTON_STATES::SHIFT].State && !KEYBOARD_STATES[BUTTON_STATES::CONTROL].State && input->Data != 'i' && input->Data != 'I') 
                return false;

            inspect->Display(!inspect->Is_Displayed());

            return true;
        }, true);
    });    

    GGUI::SLEEP(INT32_MAX);
    
    // Then exit properly
    GGUI::Exit();
}