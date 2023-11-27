#include "List_View.h"
#include "../Renderer.h"

//undefine these before algorithm.h is included

#include <algorithm>
#undef RGB
#undef BOOL
#undef NUMBER

GGUI::List_View::List_View(std::map<std::string, VALUE*> css, unsigned int width, unsigned int height, Element* parent, Coordinates position) : Element(css){
    GGUI::Pause_Renderer([=](){
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

GGUI::List_View::List_View(Element* parent, std::vector<Element*> Tree, Grow_Direction grow_direction) : Element(){
    GGUI::Pause_Renderer([=](){
        Set_Parent(parent);
        At<NUMBER_VALUE>(STYLES::Flow_Priority)->Value = (int)grow_direction;

        for (auto i : Tree)
            Add_Child(i);
            
        Parent->Add_Child(this);
    });
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
        if ((Element*)this == (Element*)GGUI::Main){
            max_width = Max_Width;
            max_height = Max_Height;
        }
        else{
            max_width = GGUI::Main->Get_Width() - GGUI::Main->Has_Border() * 2;
            max_height = GGUI::Main->Get_Height() - GGUI::Main->Has_Border() * 2;
        }
    }


    // (This->left_border_size - child->left_border_size) * on_of_switch
    unsigned Offset = (Has_Border() - e->Has_Border()) * Has_Border();

    unsigned int Max_Inner_Space_Height = Get_Height() - Offset * 2;
    unsigned int Max_Inner_Space_Width = Get_Width() - Offset * 2;

    unsigned int Child_Needs_Minimum_Height_Of = e->Get_Height() + Offset * 2;
    unsigned int Child_Needs_Minimum_Width_Of = e->Get_Width() + Offset * 2;


    if (At<BOOL_VALUE>(STYLES::Wrap)->Value){
        Report(
            "Overflow wrapping is not supported!"
        );
    }
    else{
        e->Set_Parent(this);

        // Add reverse support for the list to grow from the end -> start.
        // Add sticky support for two child elements with borders to loan the same border space.
        if (At<NUMBER_VALUE>(STYLES::Flow_Priority)->Value == (int)Grow_Direction::ROW){

            // Affect minimum width needed, when current child has borders as well as the previus one.
            signed int Width_Modifier = e->Has_Border() & Last_Child->Has_Border();

            unsigned long long Proposed_Height = Max(Child_Needs_Minimum_Height_Of, Get_Height());
            unsigned long long Proposed_Width = Max(Last_Child->Get_Position().X + Child_Needs_Minimum_Width_Of - Width_Modifier, Get_Width());

            // Check if the parent has the capability to allow to stretch in the direction if this list elements.
            if (!Parent->At<BOOL_VALUE>(STYLES::Allow_Dynamic_Size)->Value){
                Height = Min(max_height, Proposed_Height);
                Width = Min(max_width, Proposed_Width);
            }
            else{
                Width = Proposed_Width;
                Height = Proposed_Height;

                Dirty.Dirty(STAIN_TYPE::STRECH);
            }

            e->Set_Position({Last_Child->Get_Position().X - Width_Modifier, e->Get_Position().Y});

            Last_Child->Set_Position({Last_Child->Get_Position().X + e->Get_Width() - Width_Modifier, Last_Child->Get_Position().Y});
            Last_Child->Show_Border(e->Has_Border());
        }
        else{
            // Affect minimum height needed, when current child has borders as well as the previus one.
            signed int Height_Modifier = e->Has_Border() & Last_Child->Has_Border();

            unsigned long long Proposed_Width = Max(Child_Needs_Minimum_Width_Of, Get_Width());
            unsigned long long Proposed_Height = Max(Last_Child->Get_Position().Y + Child_Needs_Minimum_Height_Of - Height_Modifier, Get_Height());

            // Check if the parent has the capability to allow to stretch in the direction if this list elements.
            if (!Parent->At<BOOL_VALUE>(STYLES::Allow_Dynamic_Size)->Value){
                Width = Min(max_width, Proposed_Width);
                Height = Min(max_height, Proposed_Height);
            }
            else{
                Width = Proposed_Width;
                Height = Proposed_Height;
                
                Dirty.Dirty(STAIN_TYPE::STRECH);
            }

            e->Set_Position({e->Get_Position().X, Last_Child->Get_Position().Y - Height_Modifier});

            Last_Child->Set_Position({Last_Child->Get_Position().X, Last_Child->Get_Position().Y + e->Get_Height() - Height_Modifier});
            Last_Child->Show_Border(e->Has_Border());
        }

        Dirty.Dirty(STAIN_TYPE::DEEP);
        Element_Names.insert({e->Get_Name(), e});
        Childs.push_back(e);

        Re_Order_Childs();

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
        Fully_Stain();
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
        if ((Element*)this == (Element*)GGUI::Main){
            max_width = Max_Width;
            max_height = Max_Height;
        }
        else{
            max_width = GGUI::Main->Get_Width() - GGUI::Main->Has_Border() * 2;
            max_height = GGUI::Main->Get_Height() - GGUI::Main->Has_Border() * 2;
        }
    }

    //first find the removable elements index.
    for (;removable_index < Childs.size() && Childs[removable_index] != remove; removable_index++);
    
    // Recalculate the new dimensions for this element.
    Set_Dimensions(0, 0);

    //now for every element past this index position needs to be altered so that this removed element didn't ever exist.
    if (At<NUMBER_VALUE>(STYLES::Flow_Priority)->Value == (int)Grow_Direction::ROW){
        // relocate the childs now that the removable child has been removed.
        for (unsigned int i = removable_index + 1; i < Childs.size(); i++){
            Childs[i]->Set_Position({Childs[i]->Get_Position().X - remove->Get_Width(), Childs[i]->Get_Position().Y});
        }

        // recalculate the new width for this list, and only update the height if the current child exceeds the height of this element.
        for (auto c : Childs){
            unsigned Offset = (Has_Border() - c->Has_Border()) * Has_Border();

            unsigned int Child_Needs_Minimum_Height_Of = c->Get_Height() + Offset * 2;
            unsigned int Child_Needs_Minimum_Width_Of = c->Get_Width() + Offset * 2;

            Width = Min(max_width, Width + Child_Needs_Minimum_Width_Of);
            Height = Min(max_height, Max(Child_Needs_Minimum_Height_Of, Height));
        }
    }
    else{        
        // relocate the childs now that the removable child has been removed.
        for (unsigned int i = removable_index + 1; i < Childs.size(); i++){
            Childs[i]->Set_Position({Childs[i]->Get_Position().X, Childs[i]->Get_Position().Y - Childs[i]->Get_Height()});
        }

        // recalculate the new height for this list, and only update the width if the current child exceeds the width of this element.
        for (auto c : Childs){
            unsigned Offset = (Has_Border() - c->Has_Border()) * Has_Border();

            unsigned int Child_Needs_Minimum_Height_Of = c->Get_Height() + Offset * 2;
            unsigned int Child_Needs_Minimum_Width_Of = c->Get_Width() + Offset * 2;

            Width = Min(max_width, Max(Child_Needs_Minimum_Width_Of, Width));
            Height = Min(max_height, Height + Child_Needs_Minimum_Height_Of);
        }
    }

    Fully_Stain();
    remove->Display(false);
    
    Element::Remove(remove);

    GGUI::Pause_Render = false;
    return true;
}

