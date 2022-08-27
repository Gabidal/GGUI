#include "Window.h"
#include "Renderer.h"

GGUI::Window::Window(std::string title, Flags f){
    Title = title;
    *((Flags*)this) = f;

    RENDERER::Update_Frame();
}

void GGUI::Window::Set_Title(std::string t){
    Title = t;
    RENDERER::Update_Frame();
}

std::string GGUI::Window::Get_Title(){
    return Title;
}

void GGUI::Element::Show_Border(bool b){
    Border = b;
    RENDERER::Update_Frame();
}

bool GGUI::Element::Has_Border(){
    return Border;
}

void GGUI::Element::Add_Child(Element* Child){
    Child->Position.X += 1;
    Child->Position.Y += 1;

    if (Child->Position.X + Child->Width >= this->Width || Child->Position.Y + Child->Height >= this->Height){
        //reset the child coordinates.
        Child->Position.X -= 1;
        Child->Position.Y -= 1;

        RENDERER::Report(
            "Window exeeded bounds\n"
            "Starts at: {" + std::to_string(Child->Position.X) + ", " + std::to_string(Child->Position.Y) + "}\n"
            "Ends at: {" + std::to_string(Child->Position.X + Child->Width) + ", " + std::to_string(Child->Position.Y + Child->Height) + "}\n"
            "Max is at: {" + std::to_string(this->Width) + ", " + std::to_string(this->Height) + "}\n"
        );

        return;
    }

    Child->Parent = this;
    Childs.push_back(Child);
    RENDERER::Update_Frame();
}

std::vector<GGUI::Element*> GGUI::Element::Get_Childs(){
    return Childs;
}

void GGUI::Element::Remove_Element(Element* handle){
    for (int i = 0; i < Childs.size(); i++){
        if (Childs[i] == handle){
            Childs.erase(Childs.begin() + i);
            RENDERER::Update_Frame();
            return;
        }
    }
}

void GGUI::Element::Remove_Element(int index){
    Childs.erase(Childs.begin() + index);
    RENDERER::Update_Frame();
}

void GGUI::Element::Set_Dimensions(int width, int height){
    Width = width;
    Height = height;
    RENDERER::Update_Frame();
}

int GGUI::Element::Get_Width(){
    return Width;
}

int GGUI::Element::Get_Height(){
    return Height;
}

void GGUI::Element::Set_Position(Coordinates c){
    Position = c;
    RENDERER::Update_Frame();
}

GGUI::Coordinates GGUI::Element::Get_Position(){
    return Position;
}

GGUI::Coordinates GGUI::Element::Get_Absolute_Position(){
    Coordinates Result = {0, 0};
    
    Element* current_element = this;
    Result = current_element->Position;
    current_element = current_element->Parent;

    while (current_element != nullptr){
        Result.X += current_element->Position.X;
        Result.Y += current_element->Position.Y;
        Result.Z += current_element->Position.Z;

        current_element = current_element->Parent;
    }

    return Result;
}

int GGUI::Element::Get_Fitting_Width(Element* child){
    int Result = 0;

    int Current_X = child->Position.X;
    for (auto& i : Childs){
        if (child->Position.Z > i->Position.Z)
            continue;
        //check when then child x hits i x.
        if (i->Position.X == Result + Current_X){
            break;
        }
        Result++;
    }

    return Result;
}

int GGUI::Element::Get_Fitting_Height(Element* child){
    int Result = 0;

    int Current_Y = child->Get_Absolute_Position().Y;
    for (auto& i : Childs){
        if (child->Get_Absolute_Position().Z > i->Get_Absolute_Position().Z)
            continue;
        //check when then child x hits i x.
        if (i->Get_Absolute_Position().Y == Result + Current_Y){
            break;
        }
        Result++;
    }
    return Result;
}

void GGUI::Element::Set_Back_Ground_Colour(std::string color){
    Back_Ground_Colour = color;
    RENDERER::Update_Frame();
}

std::string GGUI::Element::Get_Back_Ground_Colour(){
    return Back_Ground_Colour;
}

void GGUI::Element::Set_Border_Colour(std::string color){
    Border_Colour = color;
    RENDERER::Update_Frame();
}

std::string GGUI::Element::Get_Border_Colour(){
    return Border_Colour;
}

void GGUI::Element::Set_Text_Colour(std::string color){
    Text_Colour = color;
    RENDERER::Update_Frame();
}

std::string GGUI::Element::Get_Text_Colour(){
    return Text_Colour;
}

//Returns nested buffer of AST window's
std::vector<GGUI::UTF> GGUI::Window::Render(){
    std::vector<GGUI::UTF> Result;
    Result.resize(this->Width * this->Height);

    //This will add the borders if nessesary and the title of the window.
    Add_Overhead(this, Result);

    //This will add the child windows to the Result buffer
    for (auto& c : this->Get_Childs()){
        Nest_Element(this, c, Result, c->Render());
    }

    return Result;
}

//Returns nested buffer of AST window's
std::vector<GGUI::UTF> GGUI::Element::Render(){
    std::vector<GGUI::UTF> Result;
    Result.resize(this->Width * this->Height);

    //This will add the borders if nessesary and the title of the window.
    Add_Overhead(this, Result);

    //This will add the child windows to the Result buffer
    for (auto& c : this->Get_Childs()){
        Nest_Element(this, c, Result, c->Render());
    }

    return Result;
}

void GGUI::Element::Add_Overhead(GGUI::Element* w, std::vector<GGUI::UTF>& Result){
    if (!w->Has_Border())
        return;

    for (int y = 0; y < w->Height; y++){
        for (int x = 0; x < w->Width; x++){
            //top left corner
            if (y == 0 && x == 0){
                Result[y * w->Width + x] = GGUI::UTF(SYMBOLS::TOP_LEFT_CORNER, w->Get_Border_Colour(), COLOR::RESET);
            }
            //top right corner
            else if (y == 0 && x == w->Width - 1){
                Result[y * w->Width + x] = GGUI::UTF(SYMBOLS::TOP_RIGHT_CORNER, w->Get_Border_Colour(), COLOR::RESET);
            }
            //bottom left corner
            else if (y == w->Height - 1 && x == 0){
                Result[y * w->Width + x] = GGUI::UTF(SYMBOLS::BOTTOM_LEFT_CORNER, w->Get_Border_Colour(), COLOR::RESET);
            }
            //bottom right corner
            else if (y == w->Height - 1 && x == w->Width - 1){
                Result[y * w->Width + x] = GGUI::UTF(SYMBOLS::BOTTOM_RIGHT_CORNER, w->Get_Border_Colour(), COLOR::RESET);
            }
            //The title will only be written after the top left corner symbol until top right corner symbol and will NOT overflow
            // else if (y == 0 && x < w->Get_Title().size()){
            //     Result[y * w->Width + x] = GGUI::UTF(w->Get_Title()[x - 1], w->Get_Text_Colour(), COLOR::RESET);
            // }
            //The roof border
            else if (y == 0 || y == w->Height - 1){
                Result[y * w->Width + x] = GGUI::UTF(SYMBOLS::HORIZONTAL_LINE, w->Get_Border_Colour(), COLOR::RESET);
            }
            //The left border
            else if (x == 0 || x == w->Width - 1){
                Result[y * w->Width + x] = GGUI::UTF(SYMBOLS::VERTICAL_LINE, w->Get_Border_Colour(), COLOR::RESET);
            }
        }
    }
}

void GGUI::Window::Add_Overhead(GGUI::Element* w, std::vector<GGUI::UTF>& Result){
    if (!w->Has_Border())
        return;

    for (int y = 0; y < w->Height; y++){
        for (int x = 0; x < w->Width; x++){
            //top left corner
            if (y == 0 && x == 0){
                Result[y * w->Width + x] = GGUI::UTF(SYMBOLS::TOP_LEFT_CORNER, w->Get_Border_Colour(), COLOR::RESET);
            }
            //top right corner
            else if (y == 0 && x == w->Width - 1){
                Result[y * w->Width + x] = GGUI::UTF(SYMBOLS::TOP_RIGHT_CORNER, w->Get_Border_Colour(), COLOR::RESET);
            }
            //bottom left corner
            else if (y == w->Height - 1 && x == 0){
                Result[y * w->Width + x] = GGUI::UTF(SYMBOLS::BOTTOM_LEFT_CORNER, w->Get_Border_Colour(), COLOR::RESET);
            }
            //bottom right corner
            else if (y == w->Height - 1 && x == w->Width - 1){
                Result[y * w->Width + x] = GGUI::UTF(SYMBOLS::BOTTOM_RIGHT_CORNER, w->Get_Border_Colour(), COLOR::RESET);
            }
            //The title will only be written after the top left corner symbol until top right corner symbol and will NOT overflow
            else if (y == 0 && x < ((Window*)w)->Get_Title().size()){
                Result[y * w->Width + x] = GGUI::UTF(((Window*)w)->Get_Title()[x - 1], w->Get_Text_Colour(), COLOR::RESET);
            }
            //The roof border
            else if (y == 0 || y == w->Height - 1){
                Result[y * w->Width + x] = GGUI::UTF(SYMBOLS::HORIZONTAL_LINE, w->Get_Border_Colour(), COLOR::RESET);
            }
            //The left border
            else if (x == 0 || x == w->Width - 1){
                Result[y * w->Width + x] = GGUI::UTF(SYMBOLS::VERTICAL_LINE, w->Get_Border_Colour(), COLOR::RESET);
            }
        }
    }
}

void GGUI::Element::Nest_Element(GGUI::Element* Parent, GGUI::Element* Child, std::vector<GGUI::UTF>& Parent_Buffer, std::vector<GGUI::UTF> Child_Buffer){
    for (int Child_Y = 0; Child_Y < Child->Height; Child_Y++){
        for (int Child_X = 0; Child_X < Child->Width; Child_X++){
            Parent_Buffer[(Child->Position.Y + Child_Y) * Parent->Width + Child->Position.X + Child_X] = Child_Buffer[Child_Y * Child->Width + Child_X];
        }
    }
}


