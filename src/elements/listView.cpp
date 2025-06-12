#include "listView.h"
#include "../core/renderer.h"
#include "../core/utils/utils.h"
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
GGUI::scrollView::scrollView(listView& container) : element(){
    // Make the system into a Dynamic allowing parent.
    pauseGGUI([&container, this](){
        allowOverflow(true);
        element::addChild(&container);
    });
}

//End of user constructors.

GGUI::IVector3 GGUI::listView::getDimensionLimit(){
    // Overflow does not have bounds, and is thus unbounded.
    if (isOverflowAllowed()){
        return {INT16_MAX, INT16_MAX};
    }

    if (isDynamicSizeAllowed()){
        IVector3 Start_Address = Last_Child->getPosition();

        IVector3 End_Address = getFinalLimit();

        return End_Address - Start_Address;
    }

    return {getWidth(), getHeight()};
}

/**
 * @brief Adds a child element to the list view.
 * @details This function adds a child element to the list view and manages the positioning and sizing
 *          of the child element within the list view. It takes into account the list's flow direction,
 *          border offsets, and dynamic sizing capabilities.
 * @param e The child element to be added.
 */
void GGUI::listView::addChild(element* e) {
    pauseGGUI([this, e]() {
        // Since 0.1.8 we need to check if the given Element is Fully initialized with Style embeddings or not.
        STAIN& dirty = e->getDirty();
        if (dirty.is(STAIN_TYPE::FINALIZE)){
            // Finalize flag is cleaned Style Embedding with On_Init Call.
            // Give an early access to the parent, so that parent dependant attributes work properly.
            e->setParent(this);
            
            e->embedStyles();
        }

        // Get the maximum width and height limits for the list view.
        IVector3 limits = getDimensionLimit();

        // Calculate the border offset for the child element.
        unsigned Offset = (hasBorder() - e->hasBorder()) * hasBorder();
        unsigned int Child_Needs_Minimum_Height_Of = e->getHeight() + Offset * 2;
        unsigned int Child_Needs_Minimum_Width_Of = e->getWidth() + Offset * 2;

        // Check if overflow wrapping is supported.
        if (Style->Wrap.Value) {
            report("Overflow wrapping is not supported!");
            return;
        }

        // Determine the flow direction for the list view.
        if (Style->Flow_Priority.Value == DIRECTION::ROW) {
            // Adjust for minimum width needed when borders are present.
            signed int Width_Modifier = e->hasBorder() & Last_Child->hasBorder();
            if (isDynamicSizeAllowed()){
                unsigned long long Proposed_Height = Max(Child_Needs_Minimum_Height_Of, getHeight());
                unsigned long long Proposed_Width = Max(Last_Child->getPosition().X + Child_Needs_Minimum_Width_Of - Width_Modifier, getWidth());

                // Check if the parent allows stretching or overflow.
                setHeight(Min(limits.Y, Proposed_Height));
                setWidth(Min(limits.X, Proposed_Width));
                Dirty.Dirty(STAIN_TYPE::STRETCH);
            }

            // Set positions for the child and last child elements.
            e->setPosition({Last_Child->getPosition().X - Width_Modifier, e->getPosition().Y});
            Last_Child->setPosition({Last_Child->getPosition().X + e->getWidth() - Width_Modifier, Last_Child->getPosition().Y});
            Last_Child->setDimensions(e->getWidth(), e->getHeight());
        } else {
            // Adjust for minimum height needed when borders are present.
            signed int Height_Modifier = e->hasBorder() & Last_Child->hasBorder();
            if (isDynamicSizeAllowed()){
                unsigned long long Proposed_Width = Max(Child_Needs_Minimum_Width_Of, getWidth());
                unsigned long long Proposed_Height = Max(Last_Child->getPosition().Y + Child_Needs_Minimum_Height_Of - Height_Modifier, getHeight());

                // Check if the parent allows stretching or overflow.
                setWidth(Min(limits.X, Proposed_Width));
                setHeight(Min(limits.Y, Proposed_Height));
                Dirty.Dirty(STAIN_TYPE::STRETCH);
            }

            // Set positions for the child and last child elements.
            e->setPosition({e->getPosition().X, Last_Child->getPosition().Y - Height_Modifier});
            Last_Child->setPosition({Last_Child->getPosition().X, Last_Child->getPosition().Y + e->getHeight() - Height_Modifier});
            Last_Child->setDimensions(e->getWidth(), e->getHeight());
        }

        // Set border visibility for the last child.
        Last_Child->showBorder(e->hasBorder());

        // Mark the list view as deeply dirty.
        Dirty.Dirty(STAIN_TYPE::DEEP);

        // Add the child element to the internal structures.
        INTERNAL::Element_Names.insert({e->getNameAsRaw(), e});
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
void GGUI::listView::calculateChildsHitboxes(unsigned int Starting_Offset){
    // If the childs are already clean then there is nothing to do here
    if (Dirty.Type == STAIN_TYPE::CLEAN || Style->Childs.size() == 0)
        return;

    // Out mission is quite similar to the Remove(Element* c) like behaviour.
    // Since there are two different flow directions we need to slice the code into 2 separate algorithms.
    element* Current = Style->Childs[Starting_Offset];
    unsigned int Max_Width = Current->getWidth();
    unsigned int Max_Height = Current->getHeight();

    if (Style->Flow_Priority.Value == DIRECTION::ROW){

        for (unsigned int i = Starting_Offset + 1; i < Style->Childs.size(); i++){
            element* Next = Style->Childs[i];

            // Affect minimum width needed, when current child has borders as well as the previous one.
            signed int Width_Modifier = Next->hasBorder() & Current->hasBorder();

            Next->setPosition({Current->getPosition().X + Current->getWidth() - Width_Modifier, Next->getPosition().Y, Next->getPosition().Z});

            if (Next->getHeight() > Max_Height)
                Max_Height = Next->getHeight();

            Current = Next;
        }
    }
    else{
        for (unsigned int i = Starting_Offset + 1; i < Style->Childs.size(); i++){
            element* Next = Style->Childs[i];

            // Affect minimum height needed, when current child has borders as well as the previous one.
            signed int Height_Modifier = Next->hasBorder() & Current->hasBorder();

            Next->setPosition({Next->getPosition().X, Current->getPosition().Y + Current->getHeight() - Height_Modifier, Next->getPosition().Z});

            if (Next->getWidth() > Max_Width)
                Max_Width = Next->getWidth();

            Current = Next;
        }
    }

    if (
        (
        Style->Width.Value.Get_Type() != EVALUATION_TYPE::PERCENTAGE && Style->Height.Value.Get_Type() != EVALUATION_TYPE::PERCENTAGE
        ) && isDynamicSizeAllowed() && Max_Height > getHeight() && Max_Width > getWidth()
    ){
        setDimensions(Max_Width, Max_Height);
    }
}

/**
 * @brief Gets the name of the list view.
 * @details This function returns the name of the list view in the format "List_View<Name>".
 * @return The name of the list view.
 */
std::string GGUI::listView::getName() const{
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
bool GGUI::listView::remove(element* remove){
    GGUI::pauseGGUI([this, remove](){
        unsigned int Index = 0;

        //first find the removable element index.
        for (;Index < Style->Childs.size() && Style->Childs[Index] != remove; Index++);
        
        // Check if there was no element by that ptr value.
        if (Index == Style->Childs.size()){
            report("Internal: no element with ptr value: " + remove->getName() + " was found in the list view: " + getName());
            
            // Removal action failed.
            return false;
        }

        // now sadly we need to branch the code into the vertical and horizontal calculations.
        if (Style->Flow_Priority.Value == DIRECTION::ROW){
            // represents the horizontal list
            unsigned int Gap = remove->getWidth();

            unsigned int New_Stretched_Height = 0;

            // all elements after the index, need to be removed from their x position the gap value.
            for (unsigned int i = Index + 1; i < Style->Childs.size(); i++){
                // You dont need to calculate the combining borders, because they have been already been calculated when they were added to the list.
                Style->Childs[i]->setPosition({Style->Childs[i]->getPosition().X - Gap, Style->Childs[i]->getPosition().Y});

                // because if the removed element holds the stretching feature, then it means, that we dont need to check previous elements-
                // although there is a slight probability that some of the previous elements were exact same size.
                if (Style->Childs[i]->getHeight() > New_Stretched_Height)
                    New_Stretched_Height = Style->Childs[i]->getHeight();
            }

            if (isDynamicSizeAllowed()){
                setDimensions(getWidth() - Gap, New_Stretched_Height);
            }
        }   
        else{
            // represents the vertical list
            unsigned int Gap = remove->getHeight();

            unsigned int New_Stretched_Width = 0;

            // all elements after the index, need to be removed from their y position the gap value.
            for (unsigned int i = Index + 1; i < Style->Childs.size(); i++){
                // You dont need to calculate the combining borders, because they have been already been calculated when they were added to the list.
                Style->Childs[i]->setPosition({Style->Childs[i]->getPosition().X, Style->Childs[i]->getPosition().Y - Gap});

                // because if the removed element holds the stretching feature, then it means, that we dont need to check previous elements-
                // although there is a slight probability that some of the previous elements were exact same size.
                if (Style->Childs[i]->getWidth() > New_Stretched_Width)
                    New_Stretched_Width = Style->Childs[i]->getWidth();
            }

            if (isDynamicSizeAllowed()){
                setDimensions(New_Stretched_Width, getHeight() - Gap);
            }
        }

        delete remove;

        // NOTE: Last_Child is NOT an ptr to the latest child added !!!
        if (Style->Childs.size() > 0){
            element* tmp = Style->Childs[Style->Childs.size() - 1];

            Last_Child->setPosition({Last_Child->getPosition().X - tmp->getWidth(), Last_Child->getPosition().Y - tmp->getHeight()});

            Last_Child->showBorder(tmp->hasBorder());
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
void GGUI::scrollView::addChild(element* e) {
    // Mark the Scroll_View as dirty with the DEEP stain because we are adding a new child element.
    Dirty.Dirty(STAIN_TYPE::DEEP);

    // Add the child element to the List_View that is being used as the container.
    getContainer()->addChild(e);
}

/**
 * @brief Enables or disables scrolling for the Scroll_View.
 * @details This function updates the scrolling capability of the Scroll_View.
 *          If scrolling is enabled, it ensures that scrolling events are registered.
 * @param allow A boolean indicating whether to enable or disable scrolling.
 */
void GGUI::scrollView::allowScrolling(bool allow) {
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
    for (unsigned int i = 0; i < GGUI::INTERNAL::Event_Handlers.size(); i++) {
        if (GGUI::INTERNAL::Event_Handlers[i]->Host != this)
            continue;

        if (GGUI::INTERNAL::Event_Handlers[i]->Criteria == Constants::MOUSE_MIDDLE_SCROLL_UP)
            Scroll_Up_Event_Exists = true;
        else if (GGUI::INTERNAL::Event_Handlers[i]->Criteria == Constants::MOUSE_MIDDLE_SCROLL_DOWN)
            Scroll_Down_Event_Exists = true;
    }

    // Create a scroll up event if it doesn't exist
    if (!Scroll_Up_Event_Exists) {
        this->on(Constants::MOUSE_MIDDLE_SCROLL_UP, [this](GGUI::Event*) {
            this->scrollUp();
            return true;
        });
    }

    // Create a scroll down event if it doesn't exist
    if (!Scroll_Down_Event_Exists) {
        this->on(Constants::MOUSE_MIDDLE_SCROLL_DOWN, [this](GGUI::Event*) {
            this->scrollDown();
            return true;
        });
    }
}

/**
 * @brief Scrolls the view up by one index.
 * @details Decreases the scroll index if it is greater than zero and updates the container's position based on the growth direction.
 * Marks the view as dirty for a deep update.
 */
void GGUI::scrollView::scrollUp() {
    // Check if already at the topmost scroll position
    if (Scroll_Index <= 0)
        return;

    // Pause GGUI rendering to safely update the scroll index and container position
    pauseGGUI([this]() {
        // Decrease the scroll index to scroll up
        Scroll_Index--;

        // Get the container associated with this Scroll_View
        listView* Container = getContainer();

        // Update the container position based on its flow direction
        if (Container->getFlowDirection() == DIRECTION::ROW) {
            // Move the container's position left by 1 unit
            Container->setPosition({Container->getPosition().X + 1});
        } else {
            // Move the container's position up by 1 unit
            Container->setPosition({Container->getPosition().X, Container->getPosition().Y + 1});
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
void GGUI::scrollView::scrollDown() {
    // check if the scroll is too far.
    // We can assume that the container height/width always is at the same position as the last child, so that is the max scrollable amount.
    // We also want to still be able to show the last child, so get the heigh of the current child height.
    unsigned Offset = (hasBorder() - getContainer()->Last_Child->hasBorder()) * hasBorder();

    if (getContainer()->getFlowDirection() == DIRECTION::ROW) {
        if (Scroll_Index > getContainer()->getWidth() - getContainer()->Last_Child->getWidth() - Offset)
            return;
    } else {
        if (Scroll_Index > getContainer()->getHeight() - getContainer()->Last_Child->getHeight() - Offset)
            return;
    }

    pauseGGUI([this]() {
        Scroll_Index++;

        listView* Container = getContainer();

        // Now also re-set the container position dependent of the growth direction.
        if (Container->getFlowDirection() == DIRECTION::ROW)
            Container->setPosition({Container->getPosition().X - 1});
        else
            Container->setPosition({Container->getPosition().X, Container->getPosition().Y - 1});

        Dirty.Dirty(STAIN_TYPE::DEEP);
    });
}

GGUI::element* Translate_List(GGUI::HTMLNode* input){
    GGUI::listView* Result = new GGUI::listView();

    // Parse the following information given by the HTML_NODE:
    // - Childs Recursive Nesting
    // |-> Parent Linking
    // - Position written inheriting
    // - RAW ptr set to get link to origin  (no need to do anything)
    // - Type (no need to do anything)
    // - Attribute parsing: Styles, Width, Height, BG_Color, Front_Color, Border, Border color, etc.. (All CSS attributes)

    std::string Name = "";

    GGUI::translateChildsToElement(Result, input, &Name);

    GGUI::translateAttributesToElement(Result, input);

    return Result;
}

GGUIAddTranslator("ul", Translate_List);
GGUIAddTranslator("ol", Translate_List);
GGUIAddTranslator("dl", Translate_List);
GGUIAddTranslator("select", Translate_List);

/**
 * @brief Gets the name of the scroll view.
 * @details This function returns the name of the scroll view.
 * @return The name of the scroll view.
 */
std::string GGUI::scrollView::getName() const{
    return "Scroll_View<" + Name + ">";
}

/**
 * @brief Removes a child element from the scroll view.
 * @details This function forwards the request to the Remove(Element* remove) function of the container.
 * @param remove The element to be removed.
 * @return true if the element was successfully removed, false if not.
 */
bool GGUI::scrollView::remove(element* remove){
    return getContainer()->remove(remove);
}
