#include "List_View.h"
#include "../Renderer.h"

//undefine these before algorithm.h is included

#include <algorithm>
#undef RGB
#undef BOOL
#undef NUMBER

GGUI::List_View::List_View(std::map<std::string, VALUE*> css, unsigned int width, unsigned int height, Element* parent, Coordinates position) : Element(css){

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
GGUI::List_View::List_View(
    RGB text_color,
    RGB background_color
) : Element(){
    Set_Text_Color(text_color);
    Set_Background_Color(background_color);
}

GGUI::List_View::List_View(
    unsigned int width,
    unsigned int height,
    RGB text_color,
    RGB background_color
) : Element(){
    Set_Width(width);
    Set_Height(height);

    Set_Text_Color(text_color);
    Set_Background_Color(background_color);
}

GGUI::List_View::List_View(
    unsigned int width,
    unsigned int height,
    RGB text_color,
    RGB background_color,
    RGB border_color,
    RGB border_background_color
) : Element(){
    Set_Width(width);
    Set_Height(height);

    Set_Text_Color(text_color);
    Set_Background_Color(background_color);
    Set_Border_Color(border_color);
    Set_Border_Background_Color(border_background_color);
    
    Show_Border(true);
}

GGUI::List_View::List_View(
    Grow_Direction grow_direction
){
    At<NUMBER_VALUE>(STYLES::Flow_Priority)->Value = (int)grow_direction;
}

//End of user constructors.

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

    


    unsigned int Max_Inner_Space_Height = Get_Height() - Has_Border() * 2;
    unsigned int Max_Inner_Space_Width = Get_Width() - Has_Border() * 2;

    unsigned int Child_Needs_Minimum_Height_Of = e->Get_Height() + Has_Border() * 2;
    unsigned int Child_Needs_Minimum_Width_Of = e->Get_Width() + Has_Border() * 2;


    if (At<BOOL_VALUE>(STYLES::Wrap)->Value){
        Report(
            "Overflow wrapping is not supported!"
        );
    }
    else{
        e->Set_Parent(this);

        if (At<NUMBER_VALUE>(STYLES::Flow_Priority)->Value == (int)Grow_Direction::ROW){
            Height = Min(Max(Child_Needs_Minimum_Height_Of, Get_Height()), max_height);
            Width = Min(max_width, Max(Last_Child_X + Child_Needs_Minimum_Width_Of, Get_Width()));

            e->Set_Position({Last_Child_X, e->Get_Position().Y});
            Last_Child_X += e->Get_Width();
        }
        else{
            Width = Min(Max(Child_Needs_Minimum_Width_Of, Get_Width()), max_width);
            Height = Min(max_width, Max(Last_Child_Y + Child_Needs_Minimum_Height_Of, Get_Height()));

            e->Set_Position({e->Get_Position().X, Last_Child_Y});
            Last_Child_Y += e->Get_Height();
        }

        Dirty.Dirty(STAIN_TYPE::DEEP);
        Element_Names.insert({e->Get_Name(), e});
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

    GGUI::Pause_Render = true;
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

    Set_Dimensions(0, 0);

    //now for every element past this index position needs to be altered so that this removed element didn't ever exist.
    if (At<NUMBER_VALUE>(STYLES::Flow_Priority)->Value == (int)Grow_Direction::ROW){
        Last_Child_X = 0;

        for (unsigned int i = removable_index + 1; i < Childs.size(); i++){
            Childs[i]->Set_Position({Childs[i]->Get_Position().X - remove->Get_Width(), Childs[i]->Get_Position().Y});
        }

        for (auto c : Childs){
            if (c == remove)
                continue;
            
            unsigned int Child_Needs_Minimum_Height_Of = c->Get_Height() + Has_Border() * 2;
            unsigned int Child_Needs_Minimum_Width_Of = c->Get_Width() + Has_Border() * 2;

            Set_Height(Get_Height() + Min(Max(Child_Needs_Minimum_Height_Of, Get_Height()), max_height));
            if (Last_Child_X + Child_Needs_Minimum_Width_Of > Get_Width()){
                Set_Width(Get_Width() + Min(max_width, Last_Child_X + Child_Needs_Minimum_Width_Of));
            }

            Last_Child_X += c->Get_Width();
        }
    }
    else{        
        Last_Child_Y = 0;

        for (unsigned int i = removable_index + 1; i < Childs.size(); i++){
            Childs[i]->Set_Position({Childs[i]->Get_Position().X, Childs[i]->Get_Position().Y - remove->Get_Height()});
        }

        for (auto c : Childs){
            if (c == remove)
                continue;

            unsigned int Child_Needs_Minimum_Height_Of = c->Get_Height() + Has_Border() * 2;
            unsigned int Child_Needs_Minimum_Width_Of = c->Get_Width() + Has_Border() * 2;

            Set_Width(Get_Width() + Min(Max(Child_Needs_Minimum_Width_Of, Get_Width()), max_height));
            if (Last_Child_Y + Child_Needs_Minimum_Height_Of > Get_Height()){
                Set_Height(Get_Height() + Min(max_width, Last_Child_Y + Child_Needs_Minimum_Height_Of));
            }

            Last_Child_Y += c->Get_Height();
        }
    }

    remove->Display(false);
    
    GGUI::Pause_Render = false;

    Element::Remove(remove);

    return true;
}


