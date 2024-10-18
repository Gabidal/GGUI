#include "Window.h"
#include "../Core/Renderer.h"

GGUI::Window::Window(std::string title, std::vector<std::string> classes) : Element(){
    Pause_GGUI([this, title, classes](){
        for (auto& c : classes){
            Add_Class(c);
        }

        Parse_Classes();

        Title = title;

        Set_Name(title);

        Fully_Stain();

        Update_Hidden_Border_Colors();
    });
}

GGUI::Window::Window(Styling css, unsigned int width, unsigned int height, Element* parent, IVector3* position) : Element(css){
    Pause_GGUI([this, width, height, parent, position](){
        if (width != 0)
            Set_Width(width);
        if (height != 0)
            Set_Height(height);

        if (parent){
            Set_Parent(parent);

            Set_Position(position);
        }

        
        Update_Hidden_Border_Colors();
    });
}

GGUI::Window::Window(std::string title, Styling css, unsigned int width, unsigned int height, Element* parent, IVector3* position) : Element(css), Title(title){
    Pause_GGUI([this, title, css, width, height, parent, position](){
        if (width != 0)
            Set_Width(width);
        if (height != 0)
            Set_Height(height);

        if (parent){
            Set_Parent(parent);

            Set_Position(position);
        }

        Set_Name(title);

        
        Update_Hidden_Border_Colors();
    });
}


//These next constructors are mainly for users to more easily create elements.
GGUI::Window::Window(
    std::string title, 
    unsigned int width,
    unsigned int height
) : Element(){
    Pause_GGUI([this, title, width, height](){
        Title = title;
        Set_Width(width);
        Set_Height(height);

        //Because the Title will not be displayed until the border is, we will create a invisible border.
        if (Title.size() > 0){
            Show_Border(true);

            Has_Hidden_Borders = true;

            Update_Hidden_Border_Colors();

            Set_Border_Color(Get_Background_Color());
            Set_Border_Background_Color(Get_Background_Color());

            Set_Name(title);
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
    Pause_GGUI([this, title, width, height, text_color, background_color](){
        Title = title;
        Set_Width(width);
        Set_Height(height);

        Set_Text_Color(text_color);
        Set_Background_Color(background_color);

        //Because the Title will not be displayed until the border is, we will create a invisible border.
        if (Title.size() > 0){

            Show_Border(true);

            Has_Hidden_Borders = true;

            Update_Hidden_Border_Colors();

            Set_Border_Color(Get_Background_Color());
            Set_Border_Background_Color(Get_Background_Color());

            Set_Name(title);
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
    Pause_GGUI([this, title, width, height, text_color, background_color, border_color](){
        Title = title;
        Set_Width(width);
        Set_Height(height);

        Set_Text_Color(text_color);
        Set_Background_Color(background_color);
        Set_Border_Color(border_color);
        Set_Border_Background_Color(background_color);
        
        Show_Border(true);

        Set_Name(title);

        Update_Hidden_Border_Colors();
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
    Pause_GGUI([this, title, width, height, text_color, background_color, border_color, border_background_color](){
        Title = title;
        Set_Width(width);
        Set_Height(height);

        Set_Text_Color(text_color);
        Set_Background_Color(background_color);
        Set_Border_Color(border_color);
        Set_Border_Background_Color(border_background_color);
        
        Show_Border(true);

        Set_Name(title);

        Update_Hidden_Border_Colors();
    });
}


GGUI::Window::Window(
    std::string title,
    unsigned int width,
    unsigned int height,
    std::vector<Element*> Tree 
) : Element(){
    Pause_GGUI([this, title, width, height, Tree](){
        Title = title;
        Set_Width(width);
        Set_Height(height);

        //Because the Title will not be displayed until the border is, we will create a invisible border.
        if (Title.size() > 0){

            Show_Border(true);

            Has_Hidden_Borders = true;

            Update_Hidden_Border_Colors();

            Set_Border_Color(Get_Background_Color());
            Set_Border_Background_Color(Get_Background_Color());

            Set_Name(title);
        }

        for (auto i : Tree)
            Add_Child(i);
    });
}

//End of user constructors.

void GGUI::Window::Update_Hidden_Border_Colors(){
    // prioritizes the border variants if they are available
    if (Style->Border_Color.Status >= VALUE_STATE::INITIALIZED)
        Before_Hiding_Border_Color = Style->Border_Color.Value.Get<RGB>();
    else if (Style->Text_Color.Status >= VALUE_STATE::INITIALIZED)
        Before_Hiding_Border_Color = Style->Text_Color.Value.Get<RGB>();
    else
        this->Before_Hiding_Border_Color = STYLES::CONSTANTS::Default.Border_Color.Value.Get<RGB>();

    if (Style->Border_Background_Color.Status >= VALUE_STATE::INITIALIZED)
        Before_Hiding_Border_Background_Color = Style->Border_Background_Color.Value.Get<RGB>();
    else if (Style->Background_Color.Status >= VALUE_STATE::INITIALIZED)
        Before_Hiding_Border_Background_Color = Style->Background_Color.Value.Get<RGB>();
    else
        this->Before_Hiding_Border_Background_Color = STYLES::CONSTANTS::Default.Border_Background_Color.Value.Get<RGB>();
}

void GGUI::Window::Set_Title(std::string t){
    Pause_GGUI([this, t](){
        Title = t;

        if (!Has_Border() && t.size() > 0){
            Show_Border(true);
            
            Has_Hidden_Borders = true;

            Before_Hiding_Border_Color = Get_Background_Color();
            Before_Hiding_Border_Background_Color = Get_Background_Color();
            
            Set_Border_Color(Get_Background_Color());
            Set_Border_Background_Color(Get_Background_Color());
        }

        Set_Name(t);

        Dirty.Dirty(STAIN_TYPE::EDGE);
    });
}

std::string GGUI::Window::Get_Title(){
    return Title;
}

void GGUI::Window::Add_Overhead(GGUI::Element* w, std::vector<GGUI::UTF>& Result){
    Dirty.Clean(STAIN_TYPE::EDGE);

    if (!w->Has_Border())
        return;
    
    GGUI::styled_border* custom_border = &Style->Border_Style;

    for (unsigned int y = 0; y < w->Get_Height(); y++){
        for (unsigned int x = 0; x < w->Get_Width(); x++){
            //top left corner
            if (y == 0 && x == 0){
                Result[y * w->Get_Width() + x] = GGUI::UTF(custom_border->TOP_LEFT_CORNER, w->Compose_All_Border_RGB_Values());
            }
            //top right corner
            else if (y == 0 && x == w->Get_Width() - 1){
                Result[y * w->Get_Width() + x] = GGUI::UTF(custom_border->TOP_RIGHT_CORNER, w->Compose_All_Border_RGB_Values());
            }
            //bottom left corner
            else if (y == w->Get_Height() - 1 && x == 0){
                Result[y * w->Get_Width() + x] = GGUI::UTF(custom_border->BOTTOM_LEFT_CORNER, w->Compose_All_Border_RGB_Values());
            }
            //bottom right corner
            else if (y == w->Get_Height() - 1 && x == w->Get_Width() - 1){
                Result[y * w->Get_Width() + x] = GGUI::UTF(custom_border->BOTTOM_RIGHT_CORNER, w->Compose_All_Border_RGB_Values());
            }
            //The title will only be written after the top left corner symbol until top right corner symbol and will NOT overflow
            else if (y == 0 && x <= ((GGUI::Window*)w)->Get_Title().size()){
                Result[y * w->Get_Width() + x] = GGUI::UTF(((GGUI::Window*)w)->Get_Title()[x - 1], w->Compose_All_Text_RGB_Values());
            }
            //The roof border
            else if (y == 0 || y == w->Get_Height() - 1){
                Result[y * w->Get_Width() + x] = GGUI::UTF(custom_border->HORIZONTAL_LINE, w->Compose_All_Border_RGB_Values());
            }
            //The left border
            else if (x == 0 || x == w->Get_Width() - 1){
                Result[y * w->Get_Width() + x] = GGUI::UTF(custom_border->VERTICAL_LINE, w->Compose_All_Border_RGB_Values());
            }
        }
    }
}

std::string GGUI::Window::Get_Name() const{
    return "Window<" + Name + ">";
}

void GGUI::Window::Show_Border(bool b){
    
    // This means that the window has setted a title whitout the borders, so we need to reinstate them.
    if (Has_Hidden_Borders){
        Set_Border_Color(Before_Hiding_Border_Color);
        Set_Border_Background_Color(Before_Hiding_Border_Background_Color);
    }

    if (b != Style->Border_Enabled.Value){
        Style->Border_Enabled = b;

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
        Style->Border_Enabled = b;

        Dirty.Dirty(STAIN_TYPE::EDGE);
        Update_Frame();
    }
}

void GGUI::Window::Set_Background_Color(RGB color){
    Style->Background_Color = color;

    Before_Hiding_Border_Background_Color = color;

    Dirty.Dirty(STAIN_TYPE::COLOR);
    Update_Frame();
}

void GGUI::Window::Set_Text_Color(RGB color){
    Style->Text_Color = color;
    
    Before_Hiding_Border_Color = color;
    
    Dirty.Dirty(STAIN_TYPE::COLOR);
    Update_Frame();
}

void GGUI::Window::Set_Border_Background_Color(RGB color){
    Style->Border_Background_Color = color;

    Before_Hiding_Border_Background_Color = color;

    Dirty.Dirty(STAIN_TYPE::COLOR);
    Update_Frame();
}

void GGUI::Window::Set_Border_Color(RGB color){
    Style->Border_Color = color;

    Before_Hiding_Border_Color = color;

    Dirty.Dirty(STAIN_TYPE::COLOR);
    Update_Frame();
}


