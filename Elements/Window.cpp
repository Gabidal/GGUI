#include "Window.h"
#include "../Renderer.h"

GGUI::Window::Window(std::string title, Flags f){
    Title = title;
    *((Flags*)this) = f;

    GGUI::Update_Frame();
}

void GGUI::Window::Set_Title(std::string t){
    Title = t;
    GGUI::Update_Frame();
}

std::string GGUI::Window::Get_Title(){
    return Title;
}

//Returns nested buffer of AST window's
std::vector<GGUI::UTF> GGUI::Window::Render(){
    std::vector<GGUI::UTF> Result = Element::Render();

    return Result;
}

void GGUI::Window::Add_Overhead(GGUI::Element* w, std::vector<GGUI::UTF>& Result){
    if (!w->Has_Border())
        return;

    for (int y = 0; y < w->Height; y++){
        for (int x = 0; x < w->Width; x++){
            //top left corner
            if (y == 0 && x == 0){
                Result[y * w->Width + x] = GGUI::UTF(SYMBOLS::TOP_LEFT_CORNER, w->Compose_All_Border_RGB_Values());
            }
            //top right corner
            else if (y == 0 && x == w->Width - 1){
                Result[y * w->Width + x] = GGUI::UTF(SYMBOLS::TOP_RIGHT_CORNER, w->Compose_All_Border_RGB_Values());
            }
            //bottom left corner
            else if (y == w->Height - 1 && x == 0){
                Result[y * w->Width + x] = GGUI::UTF(SYMBOLS::BOTTOM_LEFT_CORNER, w->Compose_All_Border_RGB_Values());
            }
            //bottom right corner
            else if (y == w->Height - 1 && x == w->Width - 1){
                Result[y * w->Width + x] = GGUI::UTF(SYMBOLS::BOTTOM_RIGHT_CORNER, w->Compose_All_Border_RGB_Values());
            }
            //The title will only be written after the top left corner symbol until top right corner symbol and will NOT overflow
            else if (y == 0 && x < ((GGUI::Window*)w)->Get_Title().size()){
                Result[y * w->Width + x] = GGUI::UTF(((GGUI::Window*)w)->Get_Title()[x - 1], w->Compose_All_Border_RGB_Values());
            }
            //The roof border
            else if (y == 0 || y == w->Height - 1){
                Result[y * w->Width + x] = GGUI::UTF(SYMBOLS::HORIZONTAL_LINE, w->Compose_All_Border_RGB_Values());
            }
            //The left border
            else if (x == 0 || x == w->Width - 1){
                Result[y * w->Width + x] = GGUI::UTF(SYMBOLS::VERTICAL_LINE, w->Compose_All_Border_RGB_Values());
            }
        }
    }
}

std::string GGUI::Window::Get_Name(){
    return "Window";
}

GGUI::Element* GGUI::Window::Copy(){
    Window* new_element = new Window();

    *new_element = *this;

    //now also update the event handlers.
    for (auto& e : GGUI::Event_Handlers){

        if (e->Host == this){
            //copy the event and make a new one
            Action* new_action = new Action(*e);

            //update the host
            new_action->Host = new_element;

            //add the new action to the event handlers list
            GGUI::Event_Handlers.push_back(new_action);
        }

    }

    return new_element;
}



