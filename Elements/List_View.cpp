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
            Width = std::min(Child_Needs_Minimum_Width_Of + Width, max_width);

            e->Position.X = Last_Child_X;
            Last_Child_X += e->Width;
        }
        else{
            Width = std::min(std::max(Child_Needs_Minimum_Width_Of, Width), max_height);
            Height = std::min(Child_Needs_Minimum_Height_Of + Height, max_width);

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

void GGUI::List_View::Update_Parent(Element* deleted){
    if (deleted == this){
        if (Parent)
            Parent->Update_Parent(this);
        return;
    }

    if (Flow_Priority == Grow_Direction::ROW){    
        if (deleted->Height == Height){
            Height = 0;
        }
        //First find the nodes that are after the deleted
        int i = 0;
        for (; i < Childs.size(); i++){
            if (Childs[i] == deleted){
                break;
            }
            //check the first half for new max width
            else {
                if (Childs[i]->Height > Height){
                    Height = Childs[i]->Height;
                }
            }
        }
        for (int j = i + 1; j < Childs.size(); j++){

            Childs[j]->Position.X -= deleted->Width;

            //check on second half for max width
            if (Childs[j]->Height > Height){
                Height = Childs[j]->Height;
            }

        }
    }
    else{
        if (deleted->Width == Width){
            Width = 0;
        }
        //First find the nodes that are after the deleted
        int i = 0;
        for (; i < Childs.size(); i++){
            if (Childs[i] == deleted){
                break;
            }
            //check the first half for new max width
            else {
                if (Childs[i]->Width > Width){
                    Width = Childs[i]->Width;
                }
            }
        }
        for (int j = i + 1; j < Childs.size(); j++){

            Childs[j]->Position.Y -= deleted->Height;

            //check on second half for max width
            if (Childs[j]->Width > Width){
                Width = Childs[j]->Width;
            }

        }
    }
}

