#include "List_View.h"
#include "../Core/Renderer.h"
#include "HTML.h"

//undefine these before algorithm.h is included

#include <algorithm>
#undef RGB
#undef BOOL
#undef NUMBER

// Scroll_View constructors: -_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_

/**
 * @brief Constructor for the Scroll_View class.
 * @details This constructor initializes a Scroll_View object with a reference to a List_View object.
 * @param container The List_View object to be used as the container for the Scroll_View.
 */
GGUI::Scroll_View::Scroll_View(List_View& container) : Element(){
    // Make the system into a Dynamic allowing parent.
    Pause_GGUI([&container, this](){
        Allow_Overflow(true);
        Element::Add_Child(&container);
    });
}

//End of user constructors.

/**
 * @brief Adds a child element to the list view.
 * @details This function adds a child element to the list view and manages the positioning and sizing
 *          of the child element within the list view. It takes into account the list's flow direction,
 *          border offsets, and dynamic sizing capabilities.
 * @param e The child element to be added.
 */
void GGUI::List_View::Add_Child(Element* e) {
    Pause_GGUI([this, e]() {
        // Get the maximum width and height limits for the list view.
        std::pair<unsigned int, unsigned int> limits = Get_Limit_Dimensions();
        unsigned int max_width = limits.first;
        unsigned int max_height = limits.second;

        // Calculate the border offset for the child element.
        unsigned Offset = (Has_Border() - e->Has_Border()) * Has_Border();
        unsigned int Child_Needs_Minimum_Height_Of = e->Get_Height() + Offset * 2;
        unsigned int Child_Needs_Minimum_Width_Of = e->Get_Width() + Offset * 2;

        // Check if overflow wrapping is supported.
        if (Style->Wrap.Value) {
            Report("Overflow wrapping is not supported!");
            return;
        }

        e->Set_Parent(this);

        // Determine the flow direction for the list view.
        if (Style->Flow_Priority.Value == DIRECTION::ROW) {
            // Adjust for minimum width needed when borders are present.
            signed int Width_Modifier = e->Has_Border() & Last_Child->Has_Border();
            unsigned long long Proposed_Height = Max(Child_Needs_Minimum_Height_Of, Get_Height());
            unsigned long long Proposed_Width = Max(Last_Child->Get_Position().X + Child_Needs_Minimum_Width_Of - Width_Modifier, Get_Width());

            // Check if the parent allows stretching or overflow.
            if (!Parent->Is_Dynamic_Size_Allowed() && !Parent->Is_Overflow_Allowed()) {
                Set_Height(Min(max_height, Proposed_Height));
                Set_Width(Min(max_width, Proposed_Width));
            } else {
                Set_Width(Proposed_Width);
                Set_Height(Proposed_Height);
                Dirty.Dirty(STAIN_TYPE::STRETCH);
            }

            // Set positions for the child and last child elements.
            e->Set_Position({Last_Child->Get_Position().X - Width_Modifier, e->Get_Position().Y});
            Last_Child->Set_Position({Last_Child->Get_Position().X + e->Get_Width() - Width_Modifier, Last_Child->Get_Position().Y});
            Last_Child->Set_Dimensions(e->Get_Width(), e->Get_Height());
        } else {
            // Adjust for minimum height needed when borders are present.
            signed int Height_Modifier = e->Has_Border() & Last_Child->Has_Border();
            unsigned long long Proposed_Width = Max(Child_Needs_Minimum_Width_Of, Get_Width());
            unsigned long long Proposed_Height = Max(Last_Child->Get_Position().Y + Child_Needs_Minimum_Height_Of - Height_Modifier, Get_Height());

            // Check if the parent allows stretching or overflow.
            if (!Parent->Is_Dynamic_Size_Allowed() && !Parent->Is_Overflow_Allowed()) {
                Set_Width(Min(max_width, Proposed_Width));
                Set_Height(Min(max_height, Proposed_Height));
            } else {
                Set_Width(Proposed_Width);
                Set_Height(Proposed_Height);
                Dirty.Dirty(STAIN_TYPE::STRETCH);
            }

            // Set positions for the child and last child elements.
            e->Set_Position({e->Get_Position().X, Last_Child->Get_Position().Y - Height_Modifier});
            Last_Child->Set_Position({Last_Child->Get_Position().X, Last_Child->Get_Position().Y + e->Get_Height() - Height_Modifier});
            Last_Child->Set_Dimensions(e->Get_Width(), e->Get_Height());
        }

        // Set border visibility for the last child.
        Last_Child->Show_Border(e->Has_Border());

        // Mark the list view as deeply dirty.
        Dirty.Dirty(STAIN_TYPE::DEEP);

        // Add the child element to the internal structures.
        Element_Names.insert({e->Get_Name(), e});
        Style->Childs.push_back(e);
    });
}

/**
 * @brief Calculates the hitboxes of all child elements of the list view.
 * @details This function is similar to the Remove(Element* c) like behaviour.
 *          It takes into account the border offsets of both the current and the next element as well as their positions.
 *          For an horizontal list, it checks if the next element's width is greater than the current element's width.
 *          For a vertical list, it checks if the next element's height is greater than the current element's height.
 *          If the next element is greater in size than the current element, it sets the maximum width/height to the next element's width/height.
 *          It finally sets the dimensions of the list view to the maximum width and height if the list view is dynamically sized and the maximum width/height is greater than the current width/height.
 * @param Starting_Offset The starting offset into the child array.
 */
void GGUI::List_View::Calculate_Childs_Hitboxes(unsigned int Starting_Offset){
    // If the childs are already clean then there is nothing to do here
    if (Dirty.Type == STAIN_TYPE::CLEAN || Style->Childs.size() == 0)
        return;

    // Out mission is quite similar to the Remove(Element* c) like behaviour.
    // Since there are two different flow directions we need to slice the code into 2 separate algorithms.
    Element* Current = Style->Childs[Starting_Offset];
    unsigned int Max_Width = Current->Get_Width();
    unsigned int Max_Height = Current->Get_Height();

    if (Style->Flow_Priority.Value == DIRECTION::ROW){

        for (unsigned int i = Starting_Offset + 1; i < Style->Childs.size(); i++){
            Element* Next = Style->Childs[i];

            // Affect minimum width needed, when current child has borders as well as the previous one.
            signed int Width_Modifier = Next->Has_Border() & Current->Has_Border();

            Next->Set_Position({Current->Get_Position().X + Current->Get_Width() - Width_Modifier, Next->Get_Position().Y, Next->Get_Position().Z});

            if (Next->Get_Height() > Max_Height)
                Max_Height = Next->Get_Height();

            Current = Next;
        }
    }
    else{
        for (unsigned int i = Starting_Offset + 1; i < Style->Childs.size(); i++){
            Element* Next = Style->Childs[i];

            // Affect minimum height needed, when current child has borders as well as the previous one.
            signed int Height_Modifier = Next->Has_Border() & Current->Has_Border();

            Next->Set_Position({Next->Get_Position().X, Current->Get_Position().Y + Current->Get_Height() - Height_Modifier, Next->Get_Position().Z});

            if (Next->Get_Width() > Max_Width)
                Max_Width = Next->Get_Width();

            Current = Next;
        }
    }

    if (Is_Dynamic_Size_Allowed() && Max_Height > Get_Height() && Max_Width > Get_Width()){
        Set_Dimensions(Max_Width, Max_Height);
    }
}

/**
 * @brief Gets the name of the list view.
 * @details This function returns the name of the list view in the format "List_View<Name>".
 * @return The name of the list view.
 */
std::string GGUI::List_View::Get_Name() const{
    return "List_View<" + Name + ">";
}

/**
 * @brief Removes a child element from the list view.
 * @param remove The child element to be removed.
 * @return true if the element was successfully removed, false if not.
 *
 * This function removes a child element from the list view and updates the position of all elements following the removed element.
 * It also recalculates the width and height of the list view and updates the dimensions of the list view if it is dynamically sized.
 */
bool GGUI::List_View::Remove(Element* remove){
    GGUI::Pause_GGUI([this, remove](){
        unsigned int Index = 0;

        //first find the removable element index.
        for (;Index < Style->Childs.size() && Style->Childs[Index] != remove; Index++);
        
        // Check if there was no element by that ptr value.
        if (Index == Style->Childs.size()){
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
        if (Style->Flow_Priority.Value == DIRECTION::ROW){
            // represents the horizontal list
            unsigned int Gap = remove->Get_Width();

            unsigned int New_Stretched_Height = 0;

            // all elements after the index, need to be removed from their x position the gap value.
            for (unsigned int i = Index + 1; i < Style->Childs.size(); i++){
                // You dont need to calculate the combining borders, because they have been already been calculated when they were added to the list.
                Style->Childs[i]->Set_Position({Style->Childs[i]->Get_Position().X - Gap, Style->Childs[i]->Get_Position().Y});

                // because if the removed element holds the stretching feature, then it means, that we dont need to check previous elements-
                // although there is a slight probability that some of the previous elements were exact same size.
                if (Is_Stretcher && Style->Childs[i]->Get_Height() > New_Stretched_Height)
                    New_Stretched_Height = Style->Childs[i]->Get_Height();
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
            for (unsigned int i = Index + 1; i < Style->Childs.size(); i++){
                // You dont need to calculate the combining borders, because they have been already been calculated when they were added to the list.
                Style->Childs[i]->Set_Position({Style->Childs[i]->Get_Position().X, Style->Childs[i]->Get_Position().Y - Gap});

                // because if the removed element holds the stretching feature, then it means, that we dont need to check previous elements-
                // although there is a slight probability that some of the previous elements were exact same size.
                if (Is_Stretcher && Style->Childs[i]->Get_Width() > New_Stretched_Width)
                    New_Stretched_Width = Style->Childs[i]->Get_Width();
            }

            if (Is_Stretcher)
                Inner_Width = New_Stretched_Width;

            Inner_Height -= Gap;

            Set_Dimensions(Inner_Width, Inner_Height);
        }

        delete remove;

        // NOTE: Last_Child is NOT an ptr to the latest child added !!!
        if (Style->Childs.size() > 0){
            Element* tmp = Style->Childs[Style->Childs.size() - 1];

            Last_Child->Set_Position({Last_Child->Get_Position().X - tmp->Get_Width(), Last_Child->Get_Position().Y - tmp->Get_Height()});

            Last_Child->Show_Border(tmp->Has_Border());
        }

        return true;
    });

    return true;
}

/**
 * @brief Adds a child element to the Scroll_View.
 * @details This function adds a child element to the Scroll_View and marks the Scroll_View as dirty with the DEEP stain.
 * @param e The child element to be added.
 */
void GGUI::Scroll_View::Add_Child(Element* e) {
    // Mark the Scroll_View as dirty with the DEEP stain because we are adding a new child element.
    Dirty.Dirty(STAIN_TYPE::DEEP);

    // Add the child element to the List_View that is being used as the container.
    Get_Container()->Add_Child(e);
}

/**
 * @brief Enables or disables scrolling for the Scroll_View.
 * @details This function updates the scrolling capability of the Scroll_View.
 *          If scrolling is enabled, it ensures that scrolling events are registered.
 * @param allow A boolean indicating whether to enable or disable scrolling.
 */
void GGUI::Scroll_View::Allow_Scrolling(bool allow) {
    // Check the previous scrolling state
    bool previous = Style->Allow_Scrolling.Value;
    
    // Update the scrolling state if it has changed
    if (allow != previous) {
        Style->Allow_Scrolling = allow;
        // No need to dirty or update frame, this feature is a non-passive change
        // so it needs the user to do something after enabling.
    }

    bool Scroll_Up_Event_Exists = false;
    bool Scroll_Down_Event_Exists = false;

    // Check if scrolling events already exist for this Scroll_View
    for (unsigned int i = 0; i < GGUI::Event_Handlers.size(); i++) {
        if (GGUI::Event_Handlers[i]->Host != this)
            continue;

        if (GGUI::Event_Handlers[i]->Criteria == Constants::MOUSE_MIDDLE_SCROLL_UP)
            Scroll_Up_Event_Exists = true;
        else if (GGUI::Event_Handlers[i]->Criteria == Constants::MOUSE_MIDDLE_SCROLL_DOWN)
            Scroll_Down_Event_Exists = true;
    }

    // Create a scroll up event if it doesn't exist
    if (!Scroll_Up_Event_Exists) {
        this->On(Constants::MOUSE_MIDDLE_SCROLL_UP, [this](GGUI::Event*) {
            this->Scroll_Up();
            return true;
        });
    }

    // Create a scroll down event if it doesn't exist
    if (!Scroll_Down_Event_Exists) {
        this->On(Constants::MOUSE_MIDDLE_SCROLL_DOWN, [this](GGUI::Event*) {
            this->Scroll_Down();
            return true;
        });
    }
}

/**
 * @brief Scrolls the view up by one index.
 * @details Decreases the scroll index if it is greater than zero and updates the container's position based on the growth direction.
 * Marks the view as dirty for a deep update.
 */
void GGUI::Scroll_View::Scroll_Up() {
    // Check if already at the topmost scroll position
    if (Scroll_Index <= 0)
        return;

    // Pause GGUI rendering to safely update the scroll index and container position
    Pause_GGUI([this]() {
        // Decrease the scroll index to scroll up
        Scroll_Index--;

        // Get the container associated with this Scroll_View
        List_View* Container = Get_Container();

        // Update the container position based on its flow direction
        if (Container->Get_Flow_Direction() == DIRECTION::ROW) {
            // Move the container's position left by 1 unit
            Container->Set_Position({Container->Get_Position().X + 1});
        } else {
            // Move the container's position up by 1 unit
            Container->Set_Position({Container->Get_Position().X, Container->Get_Position().Y + 1});
        }

        // Mark the Scroll_View as dirty to trigger a deep update
        Dirty.Dirty(STAIN_TYPE::DEEP);
    });
}

/**
 * @brief Scrolls the view down by one index.
 * @details Increases the scroll index by one and updates the container's position based on the growth direction.
 * Marks the view as dirty for a deep update.
 */
void GGUI::Scroll_View::Scroll_Down() {
    // check if the scroll is too far.
    // We can assume that the container height/width always is at the same position as the last child, so that is the max scrollable amount.
    // We also want to still be able to show the last child, so get the heigh of the current child height.
    unsigned Offset = (Has_Border() - Get_Container()->Last_Child->Has_Border()) * Has_Border();

    if (Get_Container()->Get_Flow_Direction() == DIRECTION::ROW) {
        if (Scroll_Index > Get_Container()->Get_Width() - Get_Container()->Last_Child->Get_Width() - Offset)
            return;
    } else {
        if (Scroll_Index > Get_Container()->Get_Height() - Get_Container()->Last_Child->Get_Height() - Offset)
            return;
    }

    Pause_GGUI([this]() {
        Scroll_Index++;

        List_View* Container = Get_Container();

        // Now also re-set the container position dependent of the growth direction.
        if (Container->Get_Flow_Direction() == DIRECTION::ROW)
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

/**
 * @brief Gets the name of the scroll view.
 * @details This function returns the name of the scroll view.
 * @return The name of the scroll view.
 */
std::string GGUI::Scroll_View::Get_Name() const{
    return "Scroll_View<" + Name + ">";
}

/**
 * @brief Removes a child element from the scroll view.
 * @details This function forwards the request to the Remove(Element* remove) function of the container.
 * @param remove The element to be removed.
 * @return true if the element was successfully removed, false if not.
 */
bool GGUI::Scroll_View::Remove(Element* remove){
    return Get_Container()->Remove(remove);
}
