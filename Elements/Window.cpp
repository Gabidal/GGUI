#include "Window.h"
#include "../Renderer.h"

GGUI::Window::Window(std::string title, std::vector<std::string> classes){

    for (auto& c : classes){
        Add_Class(c);
    }

    Parse_Classes();

    Title = title;
    Dirty.Stain_All();
}

GGUI::Window::Window(std::map<std::string, VALUE*> css, unsigned int width, unsigned int height, Element* parent, Coordinates* position) : Element(css){
    if (width != 0)
        Set_Width(width);
    if (height != 0)
        Set_Height(height);

    if (parent){
        Set_Parent(parent);

        Set_Position(position);
    }
}

GGUI::Window::Window(std::string title, std::map<std::string, VALUE*> css, unsigned int width, unsigned int height, Element* parent, Coordinates* position) : Element(css), Title(title){
    if (width != 0)
        Set_Width(width);
    if (height != 0)
        Set_Height(height);

    if (parent){
        Set_Parent(parent);

        Set_Position(position);
    }
}


//These next constructors are mainly for users to more easily create elements.
GGUI::Window::Window(
    std::string title, 
    unsigned int width,
    unsigned int height
) : Element(){

    Title = title;
    Set_Width(width);
    Set_Height(height);

    //Because the Title will not be displayed until the border is, we will create a invisible border.
    if (Title.size() > 0){

        Show_Border(true);

        Set_Border_Color(Get_Background_Color());
        Set_Border_Background_Color(Get_Background_Color());
    }

}

GGUI::Window::Window(
    std::string title, 
    unsigned int width,
    unsigned int height,
    RGB text_color,
    RGB background_color
) : Element(){

    Title = title;
    Set_Width(width);
    Set_Height(height);

    Set_Text_Color(text_color);
    Set_Background_Color(background_color);

    //Because the Title will not be displayed until the border is, we will create a invisible border.
    if (Title.size() > 0){

        Show_Border(true);

        Set_Border_Color(Get_Background_Color());
        Set_Border_Background_Color(Get_Background_Color());
    }
}

GGUI::Window::Window(
    std::string title, 
    unsigned int width,
    unsigned int height,
    RGB text_color,
    RGB background_color,
    RGB border_color,
    RGB border_background_color
) : Element(){

    Title = title;
    Set_Width(width);
    Set_Height(height);

    Set_Text_Color(text_color);
    Set_Background_Color(background_color);
    Set_Border_Color(border_color);
    Set_Border_Background_Color(border_background_color);
    
    Show_Border(true);
}

//End of user constructors.

void GGUI::Window::Set_Title(std::string t){
    Title = t;
    Dirty.Dirty(STAIN_TYPE::EDGE);
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

    Dirty.Clean(STAIN_TYPE::EDGE);

    for (int y = 0; y < w->Get_Height(); y++){
        for (int x = 0; x < w->Get_Width(); x++){
            //top left corner
            if (y == 0 && x == 0){
                Result[y * w->Get_Width() + x] = GGUI::UTF(SYMBOLS::TOP_LEFT_CORNER, w->Compose_All_Border_RGB_Values());
            }
            //top right corner
            else if (y == 0 && x == w->Get_Width() - 1){
                Result[y * w->Get_Width() + x] = GGUI::UTF(SYMBOLS::TOP_RIGHT_CORNER, w->Compose_All_Border_RGB_Values());
            }
            //bottom left corner
            else if (y == w->Get_Height() - 1 && x == 0){
                Result[y * w->Get_Width() + x] = GGUI::UTF(SYMBOLS::BOTTOM_LEFT_CORNER, w->Compose_All_Border_RGB_Values());
            }
            //bottom right corner
            else if (y == w->Get_Height() - 1 && x == w->Get_Width() - 1){
                Result[y * w->Get_Width() + x] = GGUI::UTF(SYMBOLS::BOTTOM_RIGHT_CORNER, w->Compose_All_Border_RGB_Values());
            }
            //The title will only be written after the top left corner symbol until top right corner symbol and will NOT overflow
            else if (y == 0 && x <= ((GGUI::Window*)w)->Get_Title().size()){
                Result[y * w->Get_Width() + x] = GGUI::UTF(((GGUI::Window*)w)->Get_Title()[x - 1], w->Compose_All_Text_RGB_Values());
            }
            //The roof border
            else if (y == 0 || y == w->Get_Height() - 1){
                Result[y * w->Get_Width() + x] = GGUI::UTF(SYMBOLS::HORIZONTAL_LINE, w->Compose_All_Border_RGB_Values());
            }
            //The left border
            else if (x == 0 || x == w->Get_Width() - 1){
                Result[y * w->Get_Width() + x] = GGUI::UTF(SYMBOLS::VERTICAL_LINE, w->Compose_All_Border_RGB_Values());
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



