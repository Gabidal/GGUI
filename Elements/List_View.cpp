#include "List_View.h"
#include "../Renderer.h"
#include "HTML.h"

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

    Allow_Dynamic_Size(true);
}

GGUI::List_View::List_View(Element* parent, std::vector<Element*> Tree, Grow_Direction grow_direction) : Element(){
    Allow_Dynamic_Size(true);

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
    
    Allow_Dynamic_Size(true);
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
    
    Allow_Dynamic_Size(true);
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
    
    Allow_Dynamic_Size(true);
}

// Scroll_View constructors: -_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_

GGUI::Scroll_View::Scroll_View(Grow_Direction grow_direction) : Element(){
    // Make the system into a Dynamic allowing parent.

    List_View* Container = new List_View();
    Container->Set_Growth_Direction(grow_direction);
    Allow_Overflow(true);
    
    Element::Add_Child(Container);
}

GGUI::Scroll_View::Scroll_View(List_View& container) : Element(){
    // Make the system into a Dynamic allowing parent.

    Allow_Overflow(true);
    Element::Add_Child(&container);
}

GGUI::Scroll_View::Scroll_View(std::vector<Element*> Childs, Grow_Direction grow_direction) : Element(){
    // Make the system into a Dynamic allowing parent.

    List_View* Container = new List_View(this, Childs, grow_direction);

    Allow_Overflow(true);
    Element::Add_Child(Container);
}

GGUI::Scroll_View::Scroll_View(std::map<std::string, VALUE*> css, unsigned int width, unsigned int height, Element* parent, Coordinates position) : Element(css){
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

    List_View* Container = new List_View(css, width, height, this, position);

    Allow_Overflow(true);
    Element::Add_Child(Container);
}

GGUI::Scroll_View::Scroll_View(Element* parent, std::vector<Element*> Tree, Grow_Direction grow_direction) : Element(){

    List_View* Container = new List_View(this, Tree, grow_direction);

    Allow_Overflow(true);
    Element::Add_Child(Container);
}

//These next constructors are mainly for users to more easily create elements.
GGUI::Scroll_View::Scroll_View(
    RGB text_color,
    RGB background_color
) : Element(){
    Set_Text_Color(text_color);
    Set_Background_Color(background_color);
    

    List_View* Container = new List_View(text_color, background_color);
    
    Allow_Overflow(true);
    Element::Add_Child(Container);
}

GGUI::Scroll_View::Scroll_View(
    unsigned int width,
    unsigned int height,
    RGB text_color,
    RGB background_color
) : Element(){
    Set_Width(width);
    Set_Height(height);

    Set_Text_Color(text_color);
    Set_Background_Color(background_color);
    
    
    List_View* Container = new List_View(width, height, text_color, background_color);
    
    Allow_Overflow(true);
    Element::Add_Child(Container);
}

GGUI::Scroll_View::Scroll_View(
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
    

    List_View* Container = new List_View(width, height, text_color, background_color, border_color, border_background_color);
    
    Allow_Overflow(true);
    Element::Add_Child(Container);
}

//End of user constructors.

void GGUI::List_View::Add_Child(Element* e){
    Pause_Renderer([=](){

        pair<unsigned int, unsigned int> limits = Get_Limit_Dimensions();

        unsigned int max_width = limits.first;
        unsigned int max_height = limits.second;

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
                if (!Parent->At<BOOL_VALUE>(STYLES::Allow_Dynamic_Size)->Value && !Parent->At<BOOL_VALUE>(STYLES::Allow_Overflow)->Value){
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

                // Is for external users to be easily access last added data.
                Last_Child->Set_Dimensions(e->Get_Width(), e->Get_Height());
            }
            else{
                // Affect minimum height needed, when current child has borders as well as the previus one.
                signed int Height_Modifier = e->Has_Border() & Last_Child->Has_Border();

                unsigned long long Proposed_Width = Max(Child_Needs_Minimum_Width_Of, Get_Width());
                unsigned long long Proposed_Height = Max(Last_Child->Get_Position().Y + Child_Needs_Minimum_Height_Of - Height_Modifier, Get_Height());

                // Check if the parent has the capability to allow to stretch in the direction if this list elements.
                if (!Parent->At<BOOL_VALUE>(STYLES::Allow_Dynamic_Size)->Value && !Parent->At<BOOL_VALUE>(STYLES::Allow_Overflow)->Value){
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
                
                // Is for external users to be easily access last added data.
                Last_Child->Set_Dimensions(e->Get_Width(), e->Get_Height());
            }

            Last_Child->Show_Border(e->Has_Border());
            Dirty.Dirty(STAIN_TYPE::DEEP);
            Element_Names.insert({e->Get_Name(), e});
            Childs.push_back(e);
        }
    });
}

std::string GGUI::List_View::Get_Name() const{
    return "List_View<" + Name + ">";
}

bool GGUI::List_View::Remove(Element* remove){
    GGUI::Pause_Renderer([=](){
        unsigned int Index = 0;

        //first find the removable element index.
        for (;Index < Childs.size() && Childs[Index] != remove; Index++);
        
        // Check if there was no element by that ptr value.
        if (Index == Childs.size()){
            Report("Internal: no element with ptr value: " + remove->Get_Name() + " was found in the list view: " + Get_Name());
            
            // Since this removal action failed report to starter, that this failed sadge.
            return false;
        }

        // So basically this algorithm just calculates the gap which is born, when this element is removed from the middle of the list, and our task is to collapse the gap.

        // First fetch some data:
        std::pair<unsigned int, unsigned int> limits = Get_Limit_Dimensions();

        unsigned int Inner_Width = limits.first;
        unsigned int Inner_Height = limits.second;

        // represents as well as the vertical list as well the horizontal list.
        // where this checks if this element was the root cause for the elements height or width to be stretched when this was added.
        // so for an vertical list this checks if it changed the width, or for an horizontal list the height. 
        bool Is_Stretcher = remove->Get_Width() == Inner_Width || remove->Get_Height() == Inner_Height;

        // now sadly we need to branch the code into the vertical and horizontal calculations.
        if (At<NUMBER_VALUE>(STYLES::Flow_Priority)->Value == (int)Grow_Direction::ROW){
            // represents the horizontal list
            unsigned int Gap = remove->Get_Width();

            unsigned int New_Stretched_Height = 0;

            // all elements after the index, need to be removed from their x position the gap value.
            for (unsigned int i = Index + 1; i < Childs.size(); i++){
                // You dont need to calculate the combining borders, because they have been already been calculated when they were added to the list.
                Childs[i]->Set_Position({Childs[i]->Get_Position().X - Gap, Childs[i]->Get_Position().Y});

                // because if the removed element holds the stretching feature, then it means, that we dont need to check previous elements-
                // although there is a slight probability that some of the previous elements were exact same size.
                if (Is_Stretcher && Childs[i]->Get_Height() > New_Stretched_Height)
                    New_Stretched_Height = Childs[i]->Get_Height();
            }

            if (Is_Stretcher)
                Inner_Height = New_Stretched_Height;

            Inner_Width -= Gap;

            Set_Dimensions(Inner_Width, Inner_Height);
        }   
        else{
            // represents the vertical list
            unsigned int Gap = remove->Get_Height();

            unsigned int New_Stretched_Width = 0;

            // all elements after the index, need to be removed from their y position the gap value.
            for (unsigned int i = Index + 1; i < Childs.size(); i++){
                // You dont need to calculate the combining borders, because they have been already been calculated when they were added to the list.
                Childs[i]->Set_Position({Childs[i]->Get_Position().X, Childs[i]->Get_Position().Y - Gap});

                // because if the removed element holds the stretching feature, then it means, that we dont need to check previous elements-
                // although there is a slight probability that some of the previous elements were exact same size.
                if (Is_Stretcher && Childs[i]->Get_Width() > New_Stretched_Width)
                    New_Stretched_Width = Childs[i]->Get_Width();
            }

            if (Is_Stretcher)
                Inner_Width = New_Stretched_Width;

            Inner_Height -= Gap;

            Set_Dimensions(Inner_Width, Inner_Height);
        }

        delete remove;

        // NOTE: Last_Child is NOT an ptr to the latest child added !!!
        if (Childs.size() > 0){
            Element* tmp = Childs[Childs.size() - 1];

            Last_Child->Set_Position({Last_Child->Get_Position().X - tmp->Get_Width(), Last_Child->Get_Position().Y - tmp->Get_Height()});

            Last_Child->Show_Border(tmp->Has_Border());
        }

    });

    return true;
}

void GGUI::Scroll_View::Add_Child(Element* e) {
    Dirty.Dirty(STAIN_TYPE::DEEP);
    Get_Container()->Add_Child(e);
}

void GGUI::Scroll_View::Allow_Scrolling(bool allow){
    bool previous = At<BOOL_VALUE>(STYLES::Allow_Scrolling)->Value;
    if (allow != previous){
        At<BOOL_VALUE>(STYLES::Allow_Scrolling)->Value = allow;

        // no need to dirty or update frame, this feature is a non-passive change so it needs the user to do something after the enable.
    }

    bool Scroll_Up_Event_Exists = false;
    bool Scroll_Down_Event_Exists = false;

    // if this Scroll_View does not have a scrolling event then make one.
    for (int i = 0; i < GGUI::Event_Handlers.size(); i++){

        if (GGUI::Event_Handlers[i]->Host != this)
            continue;
        
        if (GGUI::Event_Handlers[i]->Criteria == Constants::MOUSE_MIDDLE_SCROLL_UP)
            Scroll_Up_Event_Exists = true;
        else if (GGUI::Event_Handlers[i]->Criteria == Constants::MOUSE_MIDDLE_SCROLL_DOWN)
            Scroll_Down_Event_Exists = true;
    }

    if (!Scroll_Up_Event_Exists){
        this->On(Constants::MOUSE_MIDDLE_SCROLL_UP, [&](GGUI::Event* e){
            this->Scroll_Up();

            return true;
        });
    }

    if (!Scroll_Down_Event_Exists){
        this->On(Constants::MOUSE_MIDDLE_SCROLL_DOWN, [&](GGUI::Event* e){
            this->Scroll_Down();

            return true;
        });
    }
}

void GGUI::Scroll_View::Scroll_Up(){
    if (Scroll_Index <= 0)
        return;

    Pause_Renderer([=](){
        Scroll_Index--;

        List_View* Container = Get_Container();

        // Now also re-set the container position dependent of the growth direction.
        if (Container->Get_Growth_Direction() == Grow_Direction::ROW)
            Container->Set_Position({Container->Get_Position().X + 1});
        else
            Container->Set_Position({Container->Get_Position().X, Container->Get_Position().Y + 1});

        Dirty.Dirty(STAIN_TYPE::DEEP);
    });
}

void GGUI::Scroll_View::Scroll_Down(){
    // check if the scroll is too far.
    // We can assume that the container height/width always is at the same position as the last child, so that is the max scrollable amount.
    // We also want to still be able to show the last child, so get the heigh of the current child height.
    unsigned Offset = (Has_Border() - Get_Container()->Last_Child->Has_Border()) * Has_Border();

    if (Get_Container()->Get_Growth_Direction() == Grow_Direction::ROW){
        if (Scroll_Index > Get_Container()->Get_Width() - Get_Container()->Last_Child->Get_Width() - Offset)
            return;
    }
    else{
        if (Scroll_Index > Get_Container()->Get_Height() - Get_Container()->Last_Child->Get_Height() - Offset)
            return;
    }

    Pause_Renderer([=](){
        Scroll_Index++;

        List_View* Container = Get_Container();

        // Now also re-set the container position dependent of the growth direction.
        if (Container->Get_Growth_Direction() == Grow_Direction::ROW)
            Container->Set_Position({Container->Get_Position().X - 1});
        else
            Container->Set_Position({Container->Get_Position().X, Container->Get_Position().Y - 1});

        Dirty.Dirty(STAIN_TYPE::DEEP);
    });
}

GGUI::Element* Translate_List(GGUI::HTML_Node* input){
    GGUI::List_View* Result = new GGUI::List_View();

    // Parse the following information given by the HTML_NODE:
    // - Childs Recursive Nesting
    // |-> Parent Linking
    // - Position written inheriting
    // - RAW ptr set to get link to origin  (no need to do anything)
    // - Type (no need to do anything)
    // - Attribute parsing: Styles, Width, Height, BG_Color, Front_Color, Border, Border color, etc.. (All CSS attributes)

    std::string Name = "";

    GGUI::Translate_Childs_To_Element(Result, input, &Name);

    GGUI::Translate_Attributes_To_Element(Result, input);

    return Result;
}

GGUI_Add_Translator("ul", Translate_List);
GGUI_Add_Translator("ol", Translate_List);
GGUI_Add_Translator("dl", Translate_List);
GGUI_Add_Translator("select", Translate_List);

std::string GGUI::Scroll_View::Get_Name() const{
    return "Scroll_View<" + Name + ">";
}

bool GGUI::Scroll_View::Remove(Element* remove){
    return Get_Container()->Remove(remove);
}
