#include "Window.h"
#include "../Core/Renderer.h"

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


