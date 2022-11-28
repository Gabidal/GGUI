#include "ggui.h"

#include <vector>

using namespace std;

string MENU_NAME = "menu";
string CAMPAING_NAME = "campaing";
string CANVAS_NAME = "canvas";
string TEXT_INPUT_NAME = "text_input";

void Input_Handler(string input){

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

void Menu(GGUI::Window* Main){
    GGUI::Button* Campaing_Button = new GGUI::Button(
        CAMPAING_NAME,
        [](GGUI::Button* This){
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
        Main,
        {
            Campaing_Button,
            Exit_Button
        },
        GGUI::Grow_Direction::COLUMN
    );

    menu->Set_Name(MENU_NAME);
    menu->Set_Width(Main->Get_Width());
    menu->Set_Height(Main->Get_Height());
}

void Campaing(GGUI::Window* Main){

    GGUI::Window* Campaing = new GGUI::Window();

    Campaing->Set_Width(Main->Get_Width());
    Campaing->Set_Height(Main->Get_Height());

    // Make sizes for the map to be 1/4 of the screen space.
    unsigned int Screen_Division_Width = Main->Get_Width() / 2 - 1;
    unsigned int Screen_Division_Height = Main->Get_Height() / 2; 

    // Set the map canvas to the top right corner.
    GGUI::Canvas* Map_Canvas = new GGUI::Canvas(
        Screen_Division_Width,
        Screen_Division_Height,
        GGUI::Coordinates(Screen_Division_Width, 0)
    );


    Map_Canvas->Set_Name(CANVAS_NAME);
    Map_Canvas->Show_Border(true);

    // The user input field is on the bottom left corner.
    GGUI::Text_Field* User_Input = new GGUI::Text_Field();
    User_Input->Set_Width(Screen_Division_Width);
    User_Input->Set_Height(2);
    User_Input->Set_Name(TEXT_INPUT_NAME);
    User_Input->Show_Border(true);
    User_Input->Set_Position({0, Screen_Division_Height * 2 - User_Input->Get_Height()});
    User_Input->Enable_Input_Overflow();

    Campaing->Add_Child(Map_Canvas);
    Campaing->Add_Child(User_Input);

    Campaing->Set_Name(CAMPAING_NAME);
    Campaing->Display(false);
    
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

    Main->Add_Child(Campaing);
}

int main(){
    GGUI::Window* Main = GGUI::Init_Renderer();

    GGUI::Pause_Renderer([=](){
        Menu(Main);
        Campaing(Main);
    });


    // GGUI::Window* Main = GGUI::Init_Renderer();
    // Main->Show_Border(true);

    // GGUI::List_View* l = new GGUI::List_View(
    //     GGUI::COLOR::WHITE,
    //     GGUI::COLOR::WHITE
    // );

    // GGUI::Window* a = new GGUI::Window(
    //     "Window A", 
    //     20,
    //     10,

    //     GGUI::COLOR::RED,
    //     GGUI::COLOR::BLUE
    // );

    // a->On_Click([=](GGUI::Event* e){
    //     a->Remove();
    // });

    // GGUI::Text_Field* b = new GGUI::Text_Field(
    //     "Text Field B",

    //     GGUI::COLOR::GREEN,
    //     GGUI::COLOR::BLACK,

    //     GGUI::COLOR::CYAN,
    //     GGUI::COLOR::DARK_MAGENTA 
    // );

    // GGUI::Check_Box* rb = new GGUI::Check_Box("Halooo?");

    // GGUI::Canvas* c = new GGUI::Canvas(10, 10);

    // c->Set(5, 5, {123, 221, 132});

    // b->Enable_Text_Input();

    // Main->Add_Child(l);

    // a->Set_Name("A");
    // b->Set_Name("B");

    // l->Add_Child(a);

    // l->Add_Child(b);

    // l->Add_Child(rb);

    // l->Add_Child(c);

    // std::vector<GGUI::Text_Field*> r = Main->Get_Elements<GGUI::Text_Field>();

    // GGUI::Element* r2 = Main->Get_Element("A");

    std::this_thread::sleep_for(std::chrono::milliseconds(GGUI::TIME::HOUR)); 

    return 0;
}