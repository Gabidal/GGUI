#include "Window.h"
#include "../Renderer.h"

GGUI::Window::Window(std::string title, std::vector<std::string> classes) : Element(){
    Pause_Renderer([=](){
        for (auto& c : classes){
            Add_Class(c);
        }

        Parse_Classes();

        Title = title;
        Fully_Stain();
    });
}

GGUI::Window::Window(std::map<std::string, VALUE*> css, unsigned int width, unsigned int height, Element* parent, Coordinates* position) : Element(css){
    Pause_Renderer([=](){
        if (width != 0)
            Set_Width(width);
        if (height != 0)
            Set_Height(height);

        if (parent){
            Set_Parent(parent);

            Set_Position(position);
        }
    });
}

GGUI::Window::Window(std::string title, std::map<std::string, VALUE*> css, unsigned int width, unsigned int height, Element* parent, Coordinates* position) : Element(css), Title(title){
    Pause_Renderer([=](){
        if (width != 0)
            Set_Width(width);
        if (height != 0)
            Set_Height(height);

        if (parent){
            Set_Parent(parent);

            Set_Position(position);
        }
    });
}


//These next constructors are mainly for users to more easily create elements.
GGUI::Window::Window(
    std::string title, 
    unsigned int width,
    unsigned int height
) : Element(){
    Pause_Renderer([=](){
        Title = title;
        Set_Width(width);
        Set_Height(height);

        //Because the Title will not be displayed until the border is, we will create a invisible border.
        if (Title.size() > 0){
            Show_Border(true);

            Has_Hidden_Borders = true;

            Set_Border_Color(Get_Background_Color());
            Set_Border_Background_Color(Get_Background_Color());
        }
    });
}

GGUI::Window::Window(
    std::string title, 
    unsigned int width,
    unsigned int height,
    RGB text_color,
    RGB background_color
) : Element(){
    Pause_Renderer([=](){
        Title = title;
        Set_Width(width);
        Set_Height(height);

        Set_Text_Color(text_color);
        Set_Background_Color(background_color);

        //Because the Title will not be displayed until the border is, we will create a invisible border.
        if (Title.size() > 0){

            Show_Border(true);

            Has_Hidden_Borders = true;

            Set_Border_Color(Get_Background_Color());
            Set_Border_Background_Color(Get_Background_Color());
        }
    });
}

GGUI::Window::Window(
    std::string title, 
    unsigned int width,
    unsigned int height,
    RGB text_color,
    RGB background_color,
    RGB border_color
) : Element(){
    Pause_Renderer([=](){
        Title = title;
        Set_Width(width);
        Set_Height(height);

        Set_Text_Color(text_color);
        Set_Background_Color(background_color);
        Set_Border_Color(border_color);
        Set_Border_Background_Color(background_color);
        
        Show_Border(true);
    });
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
    Pause_Renderer([=](){
        Title = title;
        Set_Width(width);
        Set_Height(height);

        Set_Text_Color(text_color);
        Set_Background_Color(background_color);
        Set_Border_Color(border_color);
        Set_Border_Background_Color(border_background_color);
        
        Show_Border(true);
    });
}


GGUI::Window::Window(
    std::string title,
    unsigned int width,
    unsigned int height,
    std::vector<Element*> Tree 
) : Element(){
    Pause_Renderer([=](){
        Title = title;
        Set_Width(width);
        Set_Height(height);

        //Because the Title will not be displayed until the border is, we will create a invisible border.
        if (Title.size() > 0){

            Show_Border(true);

            Has_Hidden_Borders = true;

            Set_Border_Color(Get_Background_Color());
            Set_Border_Background_Color(Get_Background_Color());
        }

        for (auto i : Tree)
            Add_Child(i);
    });
}

//End of user constructors.

void GGUI::Window::Set_Title(std::string t){
    Title = t;
    Dirty.Dirty(STAIN_TYPE::EDGE);
}

std::string GGUI::Window::Get_Title(){
    return Title;
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
    return "Window<" + Name + ">";
}

void GGUI::Window::Show_Border(bool b){
    
    // This means that the window has setted a title whitout the borders, so we need to reinstate them.
    if (Has_Hidden_Borders){
        Set_Border_Color(Before_Hiding_Border_Color);
        Set_Border_Background_Color(Before_Hiding_Border_Background_Color);
    }

    if (b != At<BOOL_VALUE>(STYLES::Border)->Value){
        At<BOOL_VALUE>(STYLES::Border)->Value = b;

        Dirty.Dirty(STAIN_TYPE::EDGE);
        Update_Frame();
    }
}

void GGUI::Window::Show_Border(bool b, bool Previus_State){
    
    // This means that the window has setted a title whitout the borders, so we need to reinstate them.
    if (Has_Hidden_Borders){
        Set_Border_Color(Before_Hiding_Border_Color);
        Set_Border_Background_Color(Before_Hiding_Border_Background_Color);
    }

    if (b != Previus_State){
        At<BOOL_VALUE>(STYLES::Border)->Value = b;

        Dirty.Dirty(STAIN_TYPE::EDGE);
        Update_Frame();
    }
}


