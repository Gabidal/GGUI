#include "ggui.h"

#include <vector>

using namespace std;

// IDEAS:
/*
    Put child render nesting before the parents border rendering.
    Take into consider if the child has also border combine it with parents own borders.

*/

string MENU_NAME = "menu";
string CAMPAING_NAME = "campaing";
string CANVAS_NAME = "canvas";
string TEXT_INPUT_NAME = "text_input";

GGUI::Text_Field* User_Input;
GGUI::Text_Field* Output;
GGUI::Canvas* Map_Canvas;

void Input_Handler(string input){

    Output->Set_Data(Output->Get_Data() + "\n" + input);

}

// Switches from the 'From' ID to the 'To' ID
void Switch(string From, string To, GGUI::Element* parent){

    GGUI::Element* From_Element = parent->Get_Element(From);
    GGUI::Element* To_Element = parent->Get_Element(To);

    // Run these changes in safe mode.
    GGUI::Pause_Renderer([=](){
        From_Element->Display(false);
        To_Element->Display(true);
    });

}

void Menu(){
    GGUI::Button* Campaing_Button = new GGUI::Button(
        CAMPAING_NAME,
        [](GGUI::Button* This){
            GGUI::Mouse_Movement_Enabled = false;;
            User_Input->Focus();
            Switch(MENU_NAME, CAMPAING_NAME, This->Get_Parent());
        }
    );

    GGUI::Button* Exit_Button = new GGUI::Button(
        "Exit",
        [](GGUI::Button* This){
            exit(0);
        }
    );

    GGUI::List_View* menu = new GGUI::List_View(
        GGUI::Main,
        {
            Campaing_Button,
            Exit_Button
        },
        GGUI::Grow_Direction::COLUMN
    );

    menu->Set_Name(MENU_NAME);
    menu->Set_Width(GGUI::Main->Get_Width());
    menu->Set_Height(GGUI::Main->Get_Height());
}

void Adventure_Mode(GGUI::Window* Parent){



}

void Campaing(){
    GGUI::Window* Campaing = new GGUI::Window();

    Campaing->Set_Width(GGUI::Main->Get_Width());
    Campaing->Set_Height(GGUI::Main->Get_Height());

    // Make sizes for the map to be 1/4 of the screen space.
    unsigned int Screen_Division_Width = GGUI::Main->Get_Width() / 2;
    unsigned int Screen_Division_Height = GGUI::Main->Get_Height() / 2; 

    // The user input field is on the bottom left corner.
    User_Input = new GGUI::Text_Field();
    User_Input->Set_Width(Screen_Division_Width-1);
    User_Input->Set_Height(1);
    User_Input->Set_Name(TEXT_INPUT_NAME);
    User_Input->Show_Border(true);
    User_Input->Set_Position({0, Screen_Division_Height * 2 - User_Input->Get_Height() + 1});
    User_Input->Enable_Input_Overflow();

    User_Input->Input(
        [=](char input){
            if (input == '\n'){
                string text = User_Input->Get_Data();
                User_Input->Set_Data("");
                Input_Handler(text);
            }   
            else{
                User_Input->Set_Data(User_Input->Get_Data() + input);
            }
        }
    ); 

    Output = new GGUI::Text_Field();
    Output->Set_Width(Screen_Division_Width);
    Output->Set_Height(Screen_Division_Height * 2 - User_Input->Get_Height());
    Output->Set_Position({0, 0});
    Output->Show_Border(true);
    Output->Enable_Input_Overflow();

    Campaing->Add_Child(User_Input);
    Campaing->Add_Child(Output);

    Campaing->Set_Name(CAMPAING_NAME);
    GGUI::Main->Add_Child(Campaing);
}

int main(int Argument_Count, char** Arguments){
    GGUI::GGUI([=](){
        Campaing();
    }, INT32_MAX);
    return 0;
}