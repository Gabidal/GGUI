#include "List_View.h"
#include "../Renderer.h"

//undefine these before algorithm.h is included
#undef min
#undef max
#include <algorithm>

void GGUI::List_View::Add_Child(Element* e){

    unsigned int max_width = 0;
    unsigned int max_height = 0;

    if (Parent){
        std::pair<unsigned int, unsigned int> Max_Dimensions = Parent->Get_Fitting_Dimensions(this);

        max_width = Max_Dimensions.first;
        max_height = Max_Dimensions.second;
    }
    else{
        max_width = Max_Width;
        max_height = Max_Height;
    }

    unsigned int Max_Inner_Space_Height = Height - Has_Border() * 2;
    unsigned int Max_Inner_Space_Width = Width - Has_Border() * 2;

    unsigned int Child_Needs_Minimum_Height_Of = e->Height + Has_Border() * 2;
    unsigned int Child_Needs_Minimum_Width_Of = e->Width + Has_Border() * 2;


    if (Wrap_Overflow){
        Report(
            "Overflow wrapping is not supported!"
        );
    }
    else{
        if (Flow_Priority == Grow_Direction::ROW){
            Height = std::min(std::max(Child_Needs_Minimum_Height_Of, Height), max_height);
            if (Last_Child_X + Child_Needs_Minimum_Width_Of > Width){
                Width = std::min(max_width, Last_Child_X + Child_Needs_Minimum_Width_Of);
            }

            e->Position.X = Last_Child_X;
            Last_Child_X += e->Width;
        }
        else{
            Width = std::min(std::max(Child_Needs_Minimum_Width_Of, Width), max_height);
            if (Last_Child_X + Child_Needs_Minimum_Height_Of > Height){
                Height = std::min(max_width, Last_Child_Y + Child_Needs_Minimum_Height_Of);
            }

            e->Position.Y = Last_Child_Y;
            Last_Child_Y += e->Height;
        }

        Dirty.Dirty(STAIN_TYPE::DEEP);
        e->Parent = this;
        Childs.push_back(e);
        Update_Parent(this);
    }
}

GGUI::Element* GGUI::List_View::Copy(){
    List_View* new_element = new List_View();

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

std::string GGUI::List_View::Get_Name(){
    return "List_View";
}

void GGUI::List_View::Update_Parent(Element* New_Element){
    if (!New_Element->Is_Displayed()){
        Dirty.Stain_All();
    }

    if (Parent)
        Parent->Update_Parent(New_Element);
}

bool GGUI::List_View::Remove(Element* remove){

    unsigned int removable_index = 0;
    
    unsigned int max_width = 0;
    unsigned int max_height = 0;

    if (Parent){
        std::pair<unsigned int, unsigned int> Max_Dimensions = Parent->Get_Fitting_Dimensions(this);

        max_width = Max_Dimensions.first;
        max_height = Max_Dimensions.second;
    }
    else{
        max_width = Max_Width;
        max_height = Max_Height;
    }

    //first find the removable elements index.
    for (;removable_index < Childs.size() && Childs[removable_index] != remove; removable_index++);

    Width = 0;
    Height = 0;

    //now for every element past this index position needs to be altered so that this removed element didn't ever exist.
    if (Flow_Priority == Grow_Direction::ROW){
        Last_Child_X = 0;

        for (unsigned int i = removable_index + 1; i < Childs.size(); i++){
            Childs[i]->Position.X -= remove->Width;
        }

        for (auto c : Childs){
            if (c == remove)
                continue;
            
            unsigned int Child_Needs_Minimum_Height_Of = c->Height + Has_Border() * 2;
            unsigned int Child_Needs_Minimum_Width_Of = c->Width + Has_Border() * 2;

            Height += std::min(std::max(Child_Needs_Minimum_Height_Of, Height), max_height);
            if (Last_Child_X + Child_Needs_Minimum_Width_Of > Width){
                Width += std::min(max_width, Last_Child_X + Child_Needs_Minimum_Width_Of);
            }

            Last_Child_X += c->Width;
        }
    }
    else{        
        Last_Child_Y = 0;

        for (unsigned int i = removable_index + 1; i < Childs.size(); i++){
            Childs[i]->Position.Y -= remove->Height;
        }

        for (auto c : Childs){
            if (c == remove)
                continue;

            unsigned int Child_Needs_Minimum_Height_Of = c->Height + Has_Border() * 2;
            unsigned int Child_Needs_Minimum_Width_Of = c->Width + Has_Border() * 2;

            Width += std::min(std::max(Child_Needs_Minimum_Width_Of, Width), max_height);
            if (Last_Child_Y + Child_Needs_Minimum_Height_Of > Height){
                Height += std::min(max_width, Last_Child_Y + Child_Needs_Minimum_Height_Of);
            }

            Last_Child_Y += c->Height;
        }
    }

    remove->Display(false);

    Element::Remove(remove);

    return true;
}


