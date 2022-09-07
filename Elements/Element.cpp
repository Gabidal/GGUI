#include "Element.h"

#include "../Renderer.h"

#undef min
#undef max

#include <algorithm>

void GGUI::Element::Show_Border(bool b){
    if (b != Border){
        Dirty.Dirty(STAIN_TYPE::EDGE);
    }
    Border = b;
    Update_Frame();
}

bool GGUI::Element::Has_Border(){
    return Border;
}

void GGUI::Element::Add_Child(Element* Child){
    if (Child->Position.X + Child->Width >= this->Width || Child->Position.Y + Child->Height >= this->Height){
        if (Child->Resize_To(this) == false){

            GGUI::Report(
                "Window exeeded bounds\n "
                "Starts at: {" + std::to_string(Child->Position.X) + ", " + std::to_string(Child->Position.Y) + "}\n "
                "Ends at: {" + std::to_string(Child->Position.X + Child->Width) + ", " + std::to_string(Child->Position.Y + Child->Height) + "}\n "
                "Max is at: {" + std::to_string(this->Width) + ", " + std::to_string(this->Height) + "}\n "
            );

            return;
        }
    }

    Dirty.Dirty(STAIN_TYPE::DEEP);
    Child->Parent = this;
    Childs.push_back(Child);
    Update_Frame();
}

std::vector<GGUI::Element*>& GGUI::Element::Get_Childs(){
    return Childs;
}

bool GGUI::Element::Remove(Element* handle){
    for (int i = 0; i < Childs.size(); i++){
        if (Childs[i] == handle){
            //If the mouse if focused on this about to be deleted element, change mouse position into it's parent Position.
            if (Focused_On == Childs[i]){
                Mouse = Childs[i]->Parent->Position;
            }

            Childs.erase(Childs.begin() + i);
            Update_Parent(handle);
            return true;
        }
    }
    return false;
}

void GGUI::Element::Update_Parent(Element* New_Element){
    //normally elements dont do anything
    if (!New_Element->Is_Displayed()){
        Dirty.Stain_All();
    }

    if (Parent){
        Parent->Update_Parent(New_Element);
    }
    else{
        Update_Frame(); //the most top (Main) will not flush all the updates to render.
    }
}

void GGUI::Element::Display(bool f){
    Show = f;
    Update_Frame();
}

bool GGUI::Element::Remove(int index){
    if (index > Childs.size() - 1){
        return false;
    }
    Element* tmp = Childs[index];

    //If the mouse if focused on this about to be deleted element, change mouse position into it's parent Position.
    if (Focused_On == tmp){
        Mouse = tmp->Parent->Position;
    }

    Childs.erase(Childs.begin() + index);
    Update_Parent(tmp);
    return true;
}

void GGUI::Element::Remove(){
    if (Parent){
        //tell the parent what is about to happen.
        //you need to update the parent before removing the child, 
        //otherwise the code cannot erase it when it is not found!
        Parent->Remove(this);
    }
    else{
        Report(
            std::string("Cannot remove ") + Get_Name() + std::string(", with no parent\n")
        );
    }
}

void GGUI::Element::Set_Dimensions(int width, int height){
    if (width != Width || height != Height){    
        Width = width;
        Height = height;
        // Dirty.Dirty(STAIN_TYPE::STRECH);
        // Dirty.Dirty(STAIN_TYPE::COLOR);
        // Dirty.Dirty(STAIN_TYPE::EDGE);
        // Dirty.Dirty(STAIN_TYPE::DEEP);
        Dirty.Stain_All();
        Update_Frame();
    }
}

int GGUI::Element::Get_Width(){
    return Width;
}

int GGUI::Element::Get_Height(){
    return Height;
}

void GGUI::Element::Set_Position(Coordinates c){
    Position = c;
    if (Parent){
        Report(
            "Cannot move element who is orphan."
        );
    }
    Parent->Dirty.Dirty(STAIN_TYPE::STRECH);
    Update_Frame();
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

std::pair<unsigned int, unsigned int> GGUI::Element::Get_Fitting_Dimensions(Element* child){
    GGUI::Element tmp = *child;
    tmp.Width = 0;
    tmp.Height = 0;

    int Border_Size = Has_Border() * 2;

    while (true){
        if (tmp.Position.X + tmp.Width < Width - Border_Size){
            tmp.Width++;
        }
        else if (tmp.Position.Y + tmp.Height < Height - Border_Size){
            tmp.Height++;
        }
        else if (tmp.Position.X + tmp.Width >= Width - Border_Size && tmp.Position.Y + tmp.Height >= Height - Border_Size){
            break;
        }
        
        for (auto c : Childs){
            if (Collides(child, c)){
                //there are already other childs occupying this area so we can stop here.
                return {tmp.Width, tmp.Height};
            }
        }

    }

    return {tmp.Width, tmp.Height};
}

void GGUI::Element::Set_Back_Ground_Colour(RGB color){
    Back_Ground_Colour = color;
    if (Border_Back_Ground_Color == Back_Ground_Colour)
        Border_Back_Ground_Color = color;
    Dirty.Dirty(STAIN_TYPE::COLOR);
    Update_Frame();
}

GGUI::RGB GGUI::Element::Get_Back_Ground_Colour(){
    return Back_Ground_Colour;
}

void GGUI::Element::Set_Border_Colour(RGB color){
    Border_Colour = color;
    Dirty.Dirty(STAIN_TYPE::COLOR);
    Update_Frame();
}

GGUI::RGB GGUI::Element::Get_Border_Colour(){
    return Border_Colour;
}

void GGUI::Element::Set_Border_Back_Ground_Color(RGB color){
    Border_Back_Ground_Color = color;
    Dirty.Dirty(STAIN_TYPE::COLOR);
    Update_Frame();
}

GGUI::RGB GGUI::Element::Get_Border_Back_Ground_Color(){
    return Border_Back_Ground_Color;
}

void GGUI::Element::Set_Text_Colour(RGB color){
    Text_Colour = color;
    Dirty.Dirty(STAIN_TYPE::COLOR);
    Update_Frame();
}

GGUI::RGB GGUI::Element::Get_Text_Colour(){
    return Text_Colour;
}

//Returns nested buffer of AST window's
std::vector<GGUI::UTF> GGUI::Element::Render(){
    std::vector<GGUI::UTF> Result = Render_Buffer;

    if (Dirty.is(STAIN_TYPE::STRECH)){
        Result.clear();
        Result.resize(this->Width * this->Height);
        Dirty.Clean(STAIN_TYPE::STRECH);
    }

    //Apply the color system to the resized result list
    if (Dirty.is(STAIN_TYPE::COLOR))
        Apply_Colors(this, Result);

    //This will add the borders if nessesary and the title of the window.
    if (Dirty.is(STAIN_TYPE::EDGE))
        Add_Overhead(this, Result);

    //if inned children have changed whitout this changing, then this will trigger.
    if (Children_Changed()){
        Dirty.Dirty(STAIN_TYPE::DEEP);
    }

    //This will add the child windows to the Result buffer
    if (Dirty.is(STAIN_TYPE::DEEP)){
        Dirty.Clean(STAIN_TYPE::DEEP);
        for (auto& c : this->Get_Childs()){
                Nest_Element(this, c, Result, c->Render());
        }
    }

    Render_Buffer = Result;

    return Result;
}

//These are just utility functions for internal purposes, dont need to sed Dirty.
void GGUI::Element::Apply_Colors(Element* w, std::vector<UTF>& Result){
    Dirty.Clean(STAIN_TYPE::COLOR);

    for (auto& utf : Result){
        utf.Pre_Fix = w->Compose_All_Text_RGB_Values();
        
        if (utf.Pre_Fix != "")
            utf.Post_Fix = Constants::RESET_TEXT_COLOUR + Constants::RESET_BACK_GROUND_COLOUR;
    }
}

void GGUI::Element::Add_Overhead(GGUI::Element* w, std::vector<GGUI::UTF>& Result){
    if (!w->Has_Border())
        return;

    Dirty.Clean(STAIN_TYPE::EDGE);

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
            // else if (y == 0 && x < w->Get_Title().size()){
            //     Result[y * w->Width + x] = GGUI::UTF(w->Get_Title()[x - 1], w->Get_Text_Colour(), COLOR::RESET);
            // }
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

void GGUI::Element::Nest_Element(GGUI::Element* Parent, GGUI::Element* Child, std::vector<GGUI::UTF>& Parent_Buffer, std::vector<GGUI::UTF> Child_Buffer){
    
    unsigned int Max_Allowed_Height = Parent->Height - Parent->Has_Border();    //remove bottom borders from calculation
    unsigned int Max_Allowed_Width = Parent->Width - Parent->Has_Border();     //remove right borders from calculation

    unsigned int Min_Allowed_Height = 0 + Parent->Has_Border();                        //add top borders from calculation
    unsigned int Min_Allowed_Width = 0 + Parent->Has_Border();                         //add left borders from calculation

    unsigned int Child_Start_Y = Min_Allowed_Height + Child->Position.Y;
    unsigned int Child_Start_X = Min_Allowed_Width + Child->Position.X;

    unsigned int Child_End_Y = std::min(Child_Start_Y + Child->Height, Max_Allowed_Height);
    unsigned int Child_End_X = std::min(Child_Start_X + Child->Width, Max_Allowed_Width);

    for (int y = Child_Start_Y; y < Child_End_Y; y++){
        for (int x = Child_Start_X; x < Child_End_X; x++){
            Parent_Buffer[y * Parent->Width + x] = Child_Buffer[(y - Child_Start_Y) * Child->Width + (x - Child_Start_X)];
        }
    }

    // for (int Child_Y = 0; Child_Y < Child->Height; Child_Y++){
    //     for (int Child_X = 0; Child_X < Child->Width; Child_X++){
    //         if (Child->Position.Y + Child_Y >= Parent->Height || Child->Position.X + Child_X >= Parent->Width){
    //             break;
    //         }
    //         Parent_Buffer[(Child->Position.Y + Child_Y) * Parent->Width + Child->Position.X + Child_X] = Child_Buffer[Child_Y * Child->Width + Child_X];
    //     }
    // }
}
//End of utility functions.

void GGUI::Element::On_Click(std::function<void(GGUI::Event* e)> action){
    Action* a = new Action(
        Constants::ENTER,
        [=](GGUI::Event* e){
            if (Collides(this, Mouse)){
                action(e);

                //action succesfully executed.
                return true;
            }
            //action failed.
            return false;
        },
        this
    );
    GGUI::Event_Handlers.push_back(a);
}

GGUI::Element* GGUI::Element::Copy(){
    Element* new_element = new Element();

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

bool GGUI::Element::Children_Changed(){
    if (Dirty.Type != STAIN_TYPE::CLEAN)
        return true;

    for (auto& e : Childs){
        if (e->Children_Changed())
            return true;
    }

    return false;
}



