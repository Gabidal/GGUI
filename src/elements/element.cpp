#include "element.h"
#include "HTML.h"

#include "../core/renderer.h"
#include "../core/utils/utils.h"

#include <algorithm>
#include <vector>
#include <cmath>

#undef min
#undef max

/**
 * @brief Converts the UTF character to a string.
 *
 * This function converts the UTF character to a string by combining the foreground and background colour
 * strings with the character itself.
 *
 * @param Result The result string.
 * @param Text_Overhead The foreground colour and style as a string.
 * @param Background_Overhead The background colour and style as a string.
 * @param Text_Colour The foreground colour as a string.
 * @param Background_Colour The background colour as a string.
 */
void GGUI::UTF::toSuperString(
    INTERNAL::superString<GGUI::constants::ANSI::maximumNeededPreAllocationForEncodedSuperString>* Result,
    bool TransparentBackground,
    INTERNAL::superString<GGUI::constants::ANSI::maximumNeededPreAllocationForOverHead>* Text_Overhead,
    INTERNAL::superString<GGUI::constants::ANSI::maximumNeededPreAllocationForOverHead>* Background_Overhead,
    INTERNAL::superString<GGUI::constants::ANSI::maximumNeededPreAllocationForColor>* Text_Colour,
    INTERNAL::superString<GGUI::constants::ANSI::maximumNeededPreAllocationForColor>* Background_Colour
) const{
    // Get the foreground colour and style as a string
    foreground.getOverHeadAsSuperString(Text_Overhead, true);
    // Get the foreground colour as a string
    foreground.getColourAsSuperString(Text_Colour);

    if (!TransparentBackground) {
        // Get the background colour and style as a string
        background.getOverHeadAsSuperString(Background_Overhead, false);
        // Get the background colour as a string
        background.getColourAsSuperString(Background_Colour);
    }

    Result->add(Text_Overhead);
    Result->add(Text_Colour);
    Result->add(constants::ANSI::END_COMMAND);
    
    if (!TransparentBackground) {
        Result->add(Background_Overhead);
        Result->add(Background_Colour);
        Result->add(constants::ANSI::END_COMMAND);
    }

    if (is(INTERNAL::COMPACT_STRING_FLAG::IS_UNICODE)){
        // Add the const char* to the Result
        Result->add(std::get<const char*>(text), size);
    }
    else{
        Result->add(std::get<char>(text));
    }

    // Add the reset ANSI code to the end of the string
    Result->add(constants::ANSI::RESET_COLOR);
}

/**
 * @brief Converts the UTF character to an encoded Super_String.
 *
 * This function converts the UTF character to an encoded Super_String by applying
 * encoding flags and combining the foreground and background colour strings
 * with the character itself.
 *
 * @param Result The Super_String to which the encoded string will be added.
 * @param Text_Overhead The Super_String where the foreground colour overhead will be stored.
 * @param Background_Overhead The Super_String where the background colour overhead will be stored.
 * @param Text_Colour The Super_String where the foreground colour will be stored.
 * @param Background_Colour The Super_String where the background colour will be stored.
 */
void GGUI::UTF::toEncodedSuperString(
    INTERNAL::superString<GGUI::constants::ANSI::maximumNeededPreAllocationForEncodedSuperString>* Result,
    INTERNAL::superString<GGUI::constants::ANSI::maximumNeededPreAllocationForColor>* Text_Colour,
    INTERNAL::superString<GGUI::constants::ANSI::maximumNeededPreAllocationForColor>* Background_Colour
) const{

    if (is(INTERNAL::ENCODING_FLAG::START)) {
        // Add the foreground and background colour and style to the result
        foreground.getColourAsSuperString(Text_Colour);
        background.getColourAsSuperString(Background_Colour);

        Result->add(INTERNAL::textOverheadPrecompute);
        Result->add(Text_Colour);
        Result->add(constants::ANSI::END_COMMAND);
        Result->add(INTERNAL::backgroundOverheadPrecompute);
        Result->add(Background_Colour);
        Result->add(constants::ANSI::END_COMMAND);
    }

    if (is(INTERNAL::COMPACT_STRING_FLAG::IS_UNICODE)) {
        // Append the Unicode character to the result
        Result->add(getUnicode(), size);
    } else {
        // Append the ASCII character to the result
        Result->add(getAscii());
    }

    if (is(INTERNAL::ENCODING_FLAG::END)) {
        // Add the reset ANSI code to the end of the string
        Result->add(constants::ANSI::RESET_COLOR);
    }
}

/**
 * @brief Constructor for the GGUI::element class.
 * 
 * This constructor initializes an element with a given style and optionally embeds styles during construction.
 * 
 * @param style A reference to a STYLING_INTERNAL::styleBase object that defines the base styling for the element.
 * @param Embed_Styles_On_Construct A boolean flag indicating whether styles should be embedded during construction.
 *        - If true, styles are embedded immediately, and the element's initialization state is finalized.
 *        - If false, styles are not embedded, and a deep copy of unparsed styles is created for later use.
 * 
 * @note If styles are embedded during construction, the element's `On_Init` method will not be called again by 
 *       the main embedding process.
 * 
 * @todo Implement the deep copy functionality for unparsed styles in `Style->copyUnParsedStyles()`.
 */
GGUI::element::element(STYLING_INTERNAL::styleBase& style, bool Embed_Styles_On_Construct){
    fullyStain();

    Dirty.Dirty(INTERNAL::STAIN_TYPE::FINALIZE);

    Style = new styling(style);

    if (Embed_Styles_On_Construct){
        Style->embedStyles(this);

        check(INTERNAL::STATE::INIT);

        // Tell the main Main->Embed_Stylings() to not call this elements On_Init, since it is already called here.
        Dirty.Clean(INTERNAL::STAIN_TYPE::FINALIZE);
    }
    else{
        // if the styles are to be embedded later on, then we need to make an deep copy of the whole list because the stack is about to be cleared.
        // TODO:
        Style->copyUnParsedStyles();
    }
}

/**
 * @brief The destructor for the Element class.
 *
 * This destructor is responsible for properly deallocating all the memory
 * allocated by the Element object.
 *
 * @note This destructor is also responsible for cleaning up the parent
 * element's vector of child elements and the event handlers list.
 */
GGUI::element::~element(){
    // Call handler for on destroying moment.
    check(INTERNAL::STATE::DESTROYED);

    // Make sure this element is not listed in the parent element.
    // And if it does, then remove it from the parent element.
    if (Parent) {
        // Find the element in the parent's vector of child elements and remove it.
        for (unsigned int i = 0; i < Parent->Style->Childs.size(); i++)
            if (Parent->Style->Childs[i] == this){
                Parent->Style->Childs.erase(Parent->Style->Childs.begin() + i);

                // This may not be enough for the parent to know where to resample the buffer where this child used to be.
                Parent->Dirty.Dirty(INTERNAL::STAIN_TYPE::DEEP);

                break;  // There should be no possibility, that there are appended two or more of this exact same element, they should be copied!!!
            }
    }

    // Fire all the childs.
    for (int i = (signed)Style->Childs.size() -1; i >= 0; i--)
        if (Style->Childs[i]->Parent == this) 
            delete Style->Childs[i];

    // Delete all the styles.
    delete Style;

    //now also update the event handlers.
    for (unsigned int i = 0; i < INTERNAL::Event_Handlers.size(); i++)
        if (INTERNAL::Event_Handlers[i]->host == this){
            
            //delete the event
            delete INTERNAL::Event_Handlers[i];

            //remove the event from the list
            INTERNAL::Event_Handlers.erase(INTERNAL::Event_Handlers.begin() + i);
        }

    // Now make sure that if the Focused_On element points to this element, then set it to nullptr
    if (isFocused())
        GGUI::INTERNAL::Focused_On = nullptr;

    // Now make sure that if the Hovered_On element points to this element, then set it to nullptr
    if (isHovered())
        GGUI::INTERNAL::Hovered_On = nullptr;
}   

/**
 * @brief Renders the element and its children into the Render_Buffer nested buffer of the window.
 * @details This function processes the element to generate a vector of UTF objects representing the current state.
 * It handles different stains such as CLASS, STRETCH, COLOR, and EDGE to ensure the element is rendered correctly.
 * @return A vector of UTF objects representing the rendered element and its children.
 */
std::vector<GGUI::UTF>& GGUI::element::render(){
    // Check for Dynamic attributes
    if(Style->evaluateDynamicDimensions(this))
        Dirty.Dirty(INTERNAL::STAIN_TYPE::STRETCH);

    if (Style->evaluateDynamicPosition(this))
        Dirty.Dirty(INTERNAL::STAIN_TYPE::MOVE);

    if (Style->evaluateDynamicColors(this))
        Dirty.Dirty(INTERNAL::STAIN_TYPE::COLOR);

    if (Style->evaluateDynamicBorder(this))
        Dirty.Dirty(INTERNAL::STAIN_TYPE::EDGE);

    calculateChildsHitboxes();    // Normally elements will NOT order their content by hitbox system.

    computeDynamicSize();

    //if inned children have changed without this changing, then this will trigger.
    if (!Dirty.has(INTERNAL::STAIN_TYPE::STRETCH | INTERNAL::STAIN_TYPE::RESET)){
        bool tmp = childrenChanged();

        if (!tmp && Dirty.is(INTERNAL::STAIN_TYPE::CLEAN)){
            return renderBuffer;
        }
        else if (tmp || hasTransparentChildren()){
            Dirty.Dirty(INTERNAL::STAIN_TYPE::RESET);
        }
    }

    // This is to tell the rendering thread that some or no changes were made to the rendering buffer.
    if (this == GGUI::INTERNAL::Main && !Dirty.is(INTERNAL::STAIN_TYPE::CLEAN)){
        GGUI::INTERNAL::Identical_Frame = false;
    }

    if (Dirty.is(INTERNAL::STAIN_TYPE::CLEAN))
        return renderBuffer;

    if (Dirty.is(INTERNAL::STAIN_TYPE::MOVE)){
        Dirty.Clean(INTERNAL::STAIN_TYPE::MOVE);

        updateAbsolutePositionCache();
    }

    if (Dirty.is(INTERNAL::STAIN_TYPE::RESET)){
        Dirty.Clean(INTERNAL::STAIN_TYPE::RESET);

        std::fill(renderBuffer.begin(), renderBuffer.end(), SYMBOLS::EMPTY_UTF);
        
        Dirty.Dirty(INTERNAL::STAIN_TYPE::COLOR | INTERNAL::STAIN_TYPE::EDGE | INTERNAL::STAIN_TYPE::DEEP);
    }

    if (Dirty.is(INTERNAL::STAIN_TYPE::STRETCH)){
        Dirty.Clean(INTERNAL::STAIN_TYPE::STRETCH);
        
        renderBuffer.clear();
        renderBuffer.resize(getWidth() * getHeight(), SYMBOLS::EMPTY_UTF);

        Dirty.Dirty(INTERNAL::STAIN_TYPE::COLOR | INTERNAL::STAIN_TYPE::EDGE | INTERNAL::STAIN_TYPE::DEEP);
    }

    // Apply the color system to the resized result list
    if (Dirty.is(INTERNAL::STAIN_TYPE::COLOR)){
        // Clean the color stain after applying the color system.
        Dirty.Clean(INTERNAL::STAIN_TYPE::COLOR);

        applyColors(renderBuffer);
    }

    bool Connect_Borders_With_Parent = hasBorder();
    unsigned int Childs_With_Borders = 0;

    //This will add the child windows to the Result buffer
    if (Dirty.is(INTERNAL::STAIN_TYPE::DEEP)){
        Dirty.Clean(INTERNAL::STAIN_TYPE::DEEP);

        for (auto c : this->Style->Childs){
            if (!c->isDisplayed())
                continue;

            // check if the child is within the renderable borders.
            if (!childIsShown(c))
                continue;

            if (c->hasBorder())
                Childs_With_Borders++;

            std::vector<UTF>* tmp = &c->render();

            nestElement(this, c, renderBuffer, *tmp);
        }
    }

    if (Childs_With_Borders > 0 && Connect_Borders_With_Parent)
        Dirty.Dirty(INTERNAL::STAIN_TYPE::EDGE);

    //This will add the borders if necessary and the title of the window.
    if (Dirty.is(INTERNAL::STAIN_TYPE::EDGE)){
        renderBorders(renderBuffer);
        renderTitle(renderBuffer);
    }

    // This will calculate the connecting borders.
    if (Childs_With_Borders > 0){
        for (auto A : this->Style->Childs){
            for (auto B : this->Style->Childs){
                if (A == B)
                    continue;

                if (!A->isDisplayed() || !A->hasBorder() || !B->isDisplayed() || !B->hasBorder())
                    continue;

                postProcessBorders(A, B, renderBuffer);
            }

            postProcessBorders(this, A, renderBuffer);
        }
    }

    return renderBuffer;
}

/**
 * @brief Sets the opacity of the element.
 * @details This function takes a float value between 0.0f and 1.0f and sets the
 * opacity of the element to that value. If the value is greater than 1.0f, the
 * function will report an error and do nothing.
 * @param[in] Opacity The opacity value to set.
 */
void GGUI::element::setOpacity(float Opacity){
    if (Opacity > 1.0f)
        INTERNAL::reportStack("Opacity value is too high: " + std::to_string(Opacity) + " for element: " + getName());

    Style->Opacity.Set(Opacity);

    Dirty.Dirty(INTERNAL::STAIN_TYPE::RESET);
    updateFrame();
}

/**
 * @brief Sets the opacity of the element using an integer percentage.
 * @details This function takes an unsigned integer value between 0 and 100, representing the opacity percentage,
 * and sets the element's opacity. If the value is greater than 100, it will report an error and do nothing.
 * @param[in] Opacity The opacity percentage to set.
 */
void GGUI::element::setOpacity(unsigned int Opacity) {
    // Check if the provided opacity is within valid range (0-100)
    if (Opacity > 100) {
        // Report an error if the opacity value is too high
        INTERNAL::reportStack("Opacity value is too high: " + std::to_string(Opacity) + " for element: " + getName());
    }

    // Convert the opacity percentage to a float value between 0.0 and 1.0 and set it
    Style->Opacity.Set((float)Opacity / 100.f);

    // Mark the element as dirty to trigger a visual update
    Dirty.Dirty(INTERNAL::STAIN_TYPE::RESET);
    updateFrame(); // Update the frame to reflect the changes
}

/**
 * @brief Gets the opacity of the element.
 * @details This function returns the current opacity of the element as a float value.
 *          The opacity is a value between 0.0 and 1.0, where 0.0 is fully transparent
 *          and 1.0 is fully opaque.
 * @return The current opacity of the element.
 */
float GGUI::element::getOpacity() const {
    return Style->Opacity.Get();
}

/**
 * @brief Checks if the element is transparent.
 * @details This function determines whether the element is transparent by checking
 *          if the element's opacity is not equal to 1.0f. An opacity less than 1.0f
 *          indicates that the element is partially or fully transparent.
 * @return True if the element is transparent; otherwise, false.
 */
bool GGUI::element::isTransparent() const {
    // Get the current opacity of the element and compare it with 1.0f
    return getOpacity() != 1.0f;
}

/**
 * @brief Sets the parent of this element.
 * @details This function sets the parent of this element to the given element.
 *          If the given element is nullptr, it will clear the parent of this
 *          element.
 * @param[in] parent The parent element to set.
 */
void GGUI::element::setParent(element* parent){
    if (parent){
        Parent = parent;
    } else {
        Parent = nullptr;
    }
}

/**
 * @brief Sets the focus state of the element.
 * @details Sets the focus state of the element to the given value.
 *          If the focus state changes, the element will be dirtied and the frame will be updated.
 * @param f The new focus state.
 */
void GGUI::element::setFocus(bool f){
    if (f != Focused){
        // If the focus state has changed, dirty the element and update the frame.
        Dirty.Dirty(INTERNAL::STAIN_TYPE::COLOR | INTERNAL::STAIN_TYPE::EDGE);

        Focused = f;

        updateFrame();
    }
}

/**
 * @brief Sets the hover state of the element.
 * @details Sets the hover state of the element to the given value.
 *          If the hover state changes, the element will be dirtied and the frame will be updated.
 * @param h The new hover state.
 */
void GGUI::element::setHoverState(bool h){
    if (h != Hovered){
        // If the hover state has changed, dirty the element and update the frame.
        Dirty.Dirty(INTERNAL::STAIN_TYPE::COLOR | INTERNAL::STAIN_TYPE::EDGE);

        Hovered = h;

        updateFrame();
    }
}

/**
 * @brief Retrieves the styling information of the element.
 * @details This function returns the current styling object associated with the element.
 *          The styling object contains various style attributes such as colors, borders, etc.
 * @return The styling object of the element.
 */
GGUI::styling GGUI::element::getStyle() const {
    // Return the styling object associated with this element.
    return *Style;
}

/**
 * @brief Retrieves the direct styling associated with this element.
 * 
 * This function returns a pointer to the styling object (`GGUI::styling`) 
 * that is directly associated with the current element. The returned 
 * styling object can be used to access or modify the visual properties 
 * of the element.
 * 
 * @return GGUI::styling* Pointer to the direct styling object of the element.
 */
GGUI::styling* GGUI::element::getDirectStyle() {
    return Style;
}

/**
 * @brief Sets the styling information of the element.
 * @details This function sets the styling information of the element to the given value.
 *          If the element already has a styling object associated with it, the function will
 *          copy the given styling information to the existing object. Otherwise, the function
 *          will create a new styling object and associate it with the element.
 * @param css The new styling information to associate with the element.
 */
void GGUI::element::setStyle(styling css){
    if (Style)
        Style->copy(css);
    else
        Style = new styling(css);

    // Update the frame after changing the styling information.
    updateFrame();
}

/**
 * @brief Sets the border visibility of the element.
 * @details This function takes a boolean as a parameter and sets the border visibility of the element accordingly.
 *          If the new state is different from the current state, the element will be marked as dirty with the EDGE stain.
 * @param b The new state of the border visibility.
 */
void GGUI::element::showBorder(bool b){
    if (b != Style->Border_Enabled.value){
        Style->Border_Enabled = b;
        Dirty.Dirty(INTERNAL::STAIN_TYPE::EDGE);
        updateFrame();
    }
}

/**
 * @brief Sets the border visibility of the element.
 * @details This function toggles the border visibility based on the provided state.
 *          If the state has changed, it updates the border visibility, marks the 
 *          element as dirty with the EDGE stain, and updates the frame.
 * @param b The desired state of the border visibility.
 * @param Previous_State The current state of the border visibility.
 */
void GGUI::element::showBorder(bool b, bool Previous_State) {
    if (b != Previous_State) {
        // Update the border enabled state
        Style->Border_Enabled = b;

        // Mark the element as dirty for border changes
        Dirty.Dirty(INTERNAL::STAIN_TYPE::EDGE);

        // Refresh the element's frame to reflect changes
        updateFrame();
    }
}

/**
 * @brief Checks if the element has a border.
 * @details This function checks if the element has a border.
 *          It returns true if the element has a border, false otherwise.
 * @return True if the element has a border, false otherwise.
 */
bool GGUI::element::hasBorder(){
    return Style->Border_Enabled.value;
}

/**
 * @brief Adds a child element to the element.
 * @details This function adds a child element to the element. If the element has a border, it will
 *          be taken into account when calculating the size of the parent element. If the child element
 *          exceeds the size of the parent element, the parent element will be resized to fit the child
 *          element. If the parent element is not allowed to resize, the child element will be resized to
 *          fit the parent element.
 * @param Child The child element to add.
 */
void GGUI::element::addChild(element* Child){
    // Since 0.1.8 we need to check if the given Element is Fully initialized with Style embeddings or not.
    if (Child->Dirty.is(INTERNAL::STAIN_TYPE::FINALIZE)){
        // Finalize flag is cleaned Style Embedding with On_Init Call.
        // Give an early access to the parent, so that parent dependant attributes work properly.
        Child->Parent = this;
        
        Child->embedStyles();
    }

    int Border_Offset =  hasBorder() != Child->hasBorder() && hasBorder() ? 1 : 0;

    if (
        Child->Style->Position.get().X + Child->getWidth() > (getWidth() - Border_Offset) || 
        Child->Style->Position.get().Y + Child->getHeight() > (getHeight() - Border_Offset)
    ){
        if (Style->Allow_Dynamic_Size.value){
            // Add the border offset to the width and the height to count for the border collision and evade it. 
            unsigned int New_Width = GGUI::INTERNAL::Max(Child->Style->Position.get().X + Child->getWidth() + Border_Offset*2, getWidth());
            unsigned int New_Height = GGUI::INTERNAL::Max(Child->Style->Position.get().Y + Child->getHeight() + Border_Offset*2, getHeight());

            // Resize the parent element to fit the child element
            setHeight(New_Height);
            setWidth(New_Width);
        }
        else if (Child->resizeTo(this) == false){

            // Report an error if the child element exceeds the size of the parent element and the parent element is not allowed to resize
            // GGUI::report(
            //     "Window exceeded static bounds\n "
            //     "Starts at: {" + std::to_string(Child->Style->Position.Get().X) + ", " + std::to_string(Child->Style->Position.Get().Y) + "}\n "
            //     "Ends at: {" + std::to_string(Child->Style->Position.Get().X + Child->getWidth()) + ", " + std::to_string(Child->Style->Position.Get().Y + Child->getHeight()) + "}\n "
            //     "Max is at: {" + std::to_string(getWidth() - hasBorder()) + ", " + std::to_string(getHeight() - hasBorder()) + "}\n "
            // );

            // return;
        }
    }

    // Mark the parent element as dirty with the DEEP stain
    Dirty.Dirty(INTERNAL::STAIN_TYPE::DEEP);

    // Add the child element to the parent's child list
    INTERNAL::Element_Names.insert({Child->getNameAsRaw(), Child});

    Style->Childs.push_back(Child);

    // Make sure that elements with higher Z, are rendered later, making them visible as on top.
    reOrderChilds();

    // Refresh the parent element's frame to reflect changes
    updateFrame();
}

/**
 * @brief Adds a vector of child elements to the current element.
 * @param childs The vector of child elements to add.
 *
 * This function adds all the child elements to the current element by calling the Add_Child function for each element in the vector.
 * It also marks the current element as dirty with the DEEP stain after adding all the elements.
 */
void GGUI::element::setChilds(std::vector<element*> childs){
    pauseGGUI([this, childs](){
        for (auto& Child : childs){
            addChild(Child);
        }
        Dirty.Dirty(INTERNAL::STAIN_TYPE::DEEP);
    });
}

/**
 * @brief Retrieves the list of child elements.
 * @details This function returns a reference to the vector containing all child elements
 *          associated with the current element's style.
 * @return A reference to the vector of child elements.
 */
std::vector<GGUI::element*>& GGUI::element::getChilds() {
    // Return the vector of child elements from the style object.
    return Style->Childs;
}

/**
 * @brief Removes a child element from the current element.
 * @param handle The pointer to the child element to be removed.
 * @return true if the element was successfully removed, false if not.
 *
 * This function iterates through the vector of child elements and checks
 * if the element at the current index is equal to the handle passed as an argument.
 * If it is, the element is deleted and the parent element is marked as dirty with the DEEP and COLOR stains.
 * If the currently focused element is the one being removed, the mouse position is set to the parent element's position.
 */
bool GGUI::element::remove(element* handle){
    for (unsigned int i = 0; i < Style->Childs.size(); i++){
        if (Style->Childs[i] == handle){
            // If the mouse is focused on this about to be deleted element, change mouse position into it's parent Position.
            if (INTERNAL::Focused_On == Style->Childs[i]){
                INTERNAL::Mouse = Style->Childs[i]->Parent->Style->Position.get();
            }

            delete handle;

            Dirty.Dirty(INTERNAL::STAIN_TYPE::DEEP | INTERNAL::STAIN_TYPE::COLOR);

            return true;
        }
    }
    return false;
}

/**
 * @brief Updates the parent element of the current element.
 * @details This function is called when the current element is added, removed, or moved
 *          to a different parent element. It marks the parent element as dirty and
 *          requests a render update.
 * @param New_Element The new parent element.
 *
 * @note If the parent element does not have a valid render buffer (i.e., its
 *       `Is_Displayed()` function returns false), this function marks the parent
 *       element as dirty with the `INTERNAL::STAIN_TYPE::DEEP` and `INTERNAL::STAIN_TYPE::COLOR` stains.
 *       This ensures that the parent element is re-rendered from scratch when the
 *       rendering thread is updated.
 */
void GGUI::element::updateParent(element* New_Element){
    // Normally elements don't do anything
    if (!New_Element->isDisplayed()){
        // Mark the parent element as dirty with the INTERNAL::STAIN_TYPE::DEEP and INTERNAL::STAIN_TYPE::COLOR stains
        fullyStain();
    }

    // When the child is unable to flag changes on parent Render(), like on removal-
    // Then ask the parent to discard the previous buffer and render from scratch.
    if (Parent){
        // Mark the parent element as dirty with the INTERNAL::STAIN_TYPE::DEEP and INTERNAL::STAIN_TYPE::COLOR stains
        fullyStain();
        // Request a render update
        updateFrame();
    }
}

/**
 * @brief Displays or hides the element and all its children.
 * @details This function changes the display status of the element and all its children.
 *          If the element is displayed, all its children are also displayed. If the element is hidden,
 *          all its children are also hidden.
 * @param f A boolean indicating whether to display (true) or hide (false) the element and its children.
 */
void GGUI::element::display(bool f){
    // Check if the to be displayed is true and the element wasn't already displayed.
    if (f != Show){
        Show = f;
        
        if (f){
            check(INTERNAL::STATE::SHOWN);
        }
        else{
            check(INTERNAL::STATE::HIDDEN);
            
            // Ask the parent to flush its buffer from this.
            if (Parent){
                Parent->Dirty.Dirty(INTERNAL::STAIN_TYPE::RESET);
            }
        }

        // now also update all children, this is for the sake of events, since they do not obey AST structure where parental hidden would stop going deeper into AST events are linear list.
        pauseGGUI([this, f](){
            for (element* c : Style->Childs){
                c->display(f);
            }
        });
    }
}

/**
 * @brief Returns whether the element is currently displayed.
 * @details This function is used to check whether the element is currently displayed or hidden.
 *          It returns true if the element is displayed and false if the element is hidden.
 * @return A boolean indicating whether the element is displayed (true) or hidden (false).
 */
bool GGUI::element::isDisplayed(){
    bool Parent_Exists = Parent;
    return (Parent_Exists && Parent->isDisplayed()) || !Parent_Exists ? Show : false;
}

/**
 * @brief Removes the element at a given index from the list of child elements.
 * @details This function checks if the index is valid (i.e. if the index is within the bounds of the vector of child elements).
 *          If the index is valid, it removes the element at the specified index from the vector of child elements and deletes the element.
 *          If the index is invalid, the function returns false.
 * @param index The index of the element to remove.
 * @return True if the element was successfully removed, false otherwise.
 */
bool GGUI::element::remove(unsigned int index){
    if (index > Style->Childs.size() - 1){
        return false;
    }

    element* tmp = Style->Childs[index];

    // If the mouse is currently focused on the element that is about to be deleted, change the mouse position into the element's parent position.
    if (INTERNAL::Focused_On == tmp){
        INTERNAL::Mouse = tmp->Parent->Style->Position.get();
    }

    // Delete the element at the specified index from the vector of child elements.
    delete tmp;

    // Mark the element as dirty, so that it will be re-rendered on the next frame.
    Dirty.Dirty(INTERNAL::STAIN_TYPE::DEEP | INTERNAL::STAIN_TYPE::COLOR);

    return true;
}

/**
 * @brief Removes the element from the parent element.
 * @details This function first checks if the element has a parent.
 *          If the element has a parent, it calls the parent's Remove() function to remove the element from the parent.
 *          If the element does not have a parent, it prints an error message to the console.
 *          The function does not update the frame, so it is the caller's responsibility to update the frame after calling this function.
 */
void GGUI::element::remove(){
    if (Parent){
        // Tell the parent what is about to happen.
        // You need to update the parent before removing the child, otherwise the code cannot erase it when it is not found!
        Parent->remove(this);
    }
    else{
        report(
            std::string("Cannot remove ") + getName() + std::string(", with no parent\n")
        );
    }
}

/**
 * @brief Set the width and height of the element.
 * @details This function sets the width and height of the element to the specified values.
 *          If the width or height is different from the current width or height, then the element will be resized and the STRETCH stain is set.
 *          The Update_Frame() function is also called to update the frame.
 * @param width The new width of the element.
 * @param height The new height of the element.
 */
void GGUI::element::setDimensions(unsigned int width, unsigned int height){
    if (width != getWidth() || height != getHeight()){
        setWidth(width);
        setHeight(height);
        //Fully_Stain();
        Dirty.Dirty(INTERNAL::STAIN_TYPE::STRETCH);
        updateFrame();
    }
}

/**
 * @brief Set the width of the element.
 * @details This function sets the width of the element to the specified value.
 *          If the width is different from the current width, then the element will be resized and the STRETCH stain is set.
 *          The Update_Frame() function is also called to update the frame.
 * @param width The new width of the element.
 */
void GGUI::element::setWidth(unsigned int width){
    if (width != getWidth()){
        Style->Width.Set(width);
        // Set the STRETCH stain if the width is changed
        fullyStain();
        // Update the frame after resizing
        updateFrame();
    }
}

/**
 * @brief Set the height of the element.
 * @details This function sets the height of the element to the specified value.
 *          If the height is different from the current height, then the element will be resized and the STRETCH stain is set.
 *          The Update_Frame() function is also called to update the frame.
 * @param height The new height of the element.
 */
void GGUI::element::setHeight(unsigned int height){
    if (height != getHeight()){
        Style->Height.Set(height);
        // Set the STRETCH stain if the height is changed
        fullyStain();
        // Update the frame after resizing
        updateFrame();
    }
}

/**
 * @brief Set the position of the element.
 * @details This function sets the position of the element to the specified coordinates.
 *          If the position changes, the element will be marked as dirty for movement
 *          and the frame will be updated.
 * @param c The new position of the element.
 */
void GGUI::element::setPosition(IVector3 c) {
    // Update the element's position in the style
    Style->Position.set(c);
    
    // Mark the element as dirty for movement updates
    this->Dirty.Dirty(INTERNAL::STAIN_TYPE::MOVE);

    // Update the frame to reflect the position change
    updateFrame();
}

/**
 * @brief Set the position of the element.
 * @details This function sets the position of the element to the specified coordinates.
 *          If the position changes, the element will be marked as dirty for movement
 *          and the frame will be updated.
 * @param c The new position of the element.
 */
void GGUI::element::setPosition(IVector3* c){
    if (c){
        // Set the position of the element to the specified coordinates
        setPosition(*c);
    }
}

/**
 * @brief Updates the position of the element by adding the given vector.
 *
 * This function increments the current position of the element by the specified vector `v`.
 *
 * @param v The vector to add to the element's current position.
 */
void GGUI::element::updatePosition(IVector3 v){
    Style->Position += v;

    Dirty.Dirty(INTERNAL::STAIN_TYPE::MOVE);

    updateFrame();
}

/**
 * @brief Update the absolute position cache of the element.
 * @details This function updates the cached absolute position of the element by adding the position of the element to the position of its parent.
 */
void GGUI::element::updateAbsolutePositionCache(){
    absolutePositionCache = {0, 0, 0};
    int Border_Offset = 0;

    if (Parent){
        // Get the position of the parent
        absolutePositionCache = Parent->getAbsolutePosition();

        Border_Offset = (Parent->hasBorder() != hasBorder() && Parent->hasBorder()) ? 1 : 0;
    }

    // Add the position of the element to the position of its parent
    absolutePositionCache += getPosition() + Border_Offset;
}

void GGUI::element::setTitle(INTERNAL::compactString t){
    Style->Title.Value = t;
}

GGUI::INTERNAL::compactString GGUI::element::getTitle(){
    // Return the title of the element
    return Style->Title.Value;
}

/**
 * @brief Set the margin of the element.
 * @details This function sets the margin of the element to the specified margin values.
 *          The margin is stored in the element's style.
 * @param margin The new margin values for the element.
 */
void GGUI::element::setMargin(margin margin) {
    // Update the element's margin in the style
    Style->Margin = margin;
}

/**
 * Creates a deep copy of this Element, including all its children.
 * @return A new Element object that is a copy of this one.
 */
GGUI::element* GGUI::element::copy() const {
    // Compile time check
    //static_assert(std::is_same<T&, decltype(*this)>::value, "T must be the same as the type of the object");
    element* new_element = createInstance();

    // Make sure the name is also renewed to represent the memory.
    new_element->setName(std::to_string((unsigned long long)new_element));

    // Ptr related members:
    // - Parent
    // - Childs
    // - Style
    // - Event Handlers
    // - Focused_On Clearance
    // - Hovered_On Clearance

    // reset the parent info.
    new_element->Parent = nullptr;

    // copy the childs over.
    for (unsigned int i = 0; i < this->Style->Childs.size(); i++){
        new_element->Style->Childs[i] = this->Style->Childs[i]->copy();
    }

    // copy the styles over.
    *new_element->Style = *this->Style;

    //now also update the event handlers.
    // NOTE: We don't have enough power to update the lambda captures of the this ptr value, so please use the self->host ptr instead!
    for (auto& e : INTERNAL::Event_Handlers){

        if (e->host == this){
            //copy the event and make a new one
            action* new_action = new action(*e);

            //update the host
            new_action->host = new_element;

            //add the new action to the event handlers list
            INTERNAL::Event_Handlers.push_back(new_action);
        }
    }

    // Clear the Focused on bool
    new_element->Focused = false;

    // Clear the Hovered on bool
    new_element->Hovered = false;

    // Call the potentially un_parsed_styles to clone them too if not initialized yet.
    if (Dirty.is(INTERNAL::STAIN_TYPE::FINALIZE)){
        new_element->Style->copyUnParsedStyles();
    }

    return new_element;
}

void GGUI::element::embedStyles(){
    if (Parent == nullptr && GGUI::INTERNAL::Main == nullptr){
        INTERNAL::reportStack("OUTBOX not supported, cannot anchor: " + getName());
    }

    Style->embedStyles(this);
    
    if (Dirty.is(INTERNAL::STAIN_TYPE::FINALIZE)){
        Dirty.Clean(INTERNAL::STAIN_TYPE::FINALIZE);

        check(INTERNAL::STATE::INIT);
    }
}

/**
 * @brief Get the fitting dimensions for the given child element.
 *
 * This function takes a child element and calculates the fitting dimensions for it.
 * The fitting dimensions are the width and height of the child element that does not exceed the
 * bounds of the parent element. If the child element is colliding with another child element
 * then the fitting dimensions are reduced to the point where the collision is resolved.
 *
 * @param child The child element for which the fitting dimensions are calculated.
 * @return A pair containing the width and height of the fitting dimensions.
 */
std::pair<unsigned int, unsigned int> GGUI::element::getFittingDimensions(element* child) {
    IVector3 Current_Position = child->getPosition();

    unsigned int Result_Width = 0;
    unsigned int Result_Height = 0;

    int Border_Offset = hasBorder() != child->hasBorder() && hasBorder() ? 1 * 2 : 0;

    /**
     * If there are only zero child or one and it is same as this child then give max.
     * This is because if there is no other child element, then the fitting dimensions
     * are the same as the parent element minus the border offset.
     */
    if (Style->Childs.size() == 0 || Style->Childs.back() == child){
        return {getWidth() - Border_Offset, getHeight() - Border_Offset};
    }

    /**
     * Start calculating the fitting dimensions from the top left corner of the child element.
     * This is done by increasing the width and height of the child element until it reaches the edge of the parent element.
     * If the child element collides with another child element, then the fitting dimensions are reduced to the point where the collision is resolved.
     */
    while (true){
        // If the width of the child element is still less than the width of this element minus the border offset
        if (Current_Position.X + (++Result_Width) < getWidth() - Border_Offset){
            // Increase the width of the child element
            Result_Width++;
        }
        else{
            break;
        }
        
        // If the height of the child element is still less than the height of this element minus the border offset
        if (Current_Position.Y + (++Result_Height) < getHeight() - Border_Offset){
            // Increase the height of the child element
            Result_Height++;
        }
        else{
            break;
        }
        
        // Check if the child element is colliding with any other child elements.
        for (auto c : Style->Childs) {
            // Use local positioning since this is a civil dispute :)
            if (child != c && INTERNAL::Collides(c->getPosition(), Current_Position, c->getWidth(), c->getHeight(), Result_Width, Result_Height)) {
                // If the child element is colliding with another child element, then we can stop here.
                return {Result_Width, Result_Height};
            }
        }
    }

    return {Result_Width, Result_Height};
}

/**
 * @brief Retrieves the final size limit of the element.
 *
 * This function calculates the final size limit of the element based on its
 * properties and its parent's constraints. If overflow is allowed, the size
 * limit is set to the maximum possible value. Otherwise, the size is determined
 * by the element's dimensions or inherited from the parent if dynamic sizing
 * is allowed.
 *
 * @return GGUI::IVector3 The final size limit of the element.
 */
GGUI::IVector3 GGUI::element::getFinalLimit(){
    if (isOverflowAllowed()){
        return {INT16_MAX, INT16_MAX};
    }

    IVector3 End_Address = IVector3(getWidth(), getHeight());

    // We can check if the parent allows some flexibility.
    if (Parent && isDynamicSizeAllowed()){
        End_Address = Parent->getFinalLimit();
    }

    return End_Address;
}

/**
 * @brief Sets the background color of the element.
 * 
 * This function sets the background color of the element to the specified RGB value. 
 * If the border background color is the same as the current background color, 
 * it updates the border background color as well. Marks the element as dirty for 
 * color updates and triggers a frame update.
 * 
 * @param color The RGB color to set as the background color.
 */
void GGUI::element::setBackgroundColor(RGB color) {
    // Set the background color in the style
    Style->Background_Color = color;
    
    // If the border background color matches the current background color, update it
    if (Style->Border_Background_Color.color.get<RGB>() == Style->Background_Color.color.get<RGB>()) {
        Style->Border_Background_Color = color;
    }
    
    // Mark the element as dirty for color updates
    Dirty.Dirty(INTERNAL::STAIN_TYPE::COLOR);
    
    // Update the frame to reflect the new color
    updateFrame();
}

/**
 * @brief Sets the border color of the element.
 * 
 * This function sets the border color of the element to the specified RGB value. Marks the element as dirty for color updates and triggers a frame update.
 * 
 * @param color The RGB color to set as the border color.
 */
void GGUI::element::setBorderColor(RGB color){
    // Set the border color in the style
    Style->Border_Color = color;
    
    // Mark the element as dirty for color updates
    Dirty.Dirty(INTERNAL::STAIN_TYPE::COLOR);
    
    // Update the frame to reflect the new color
    updateFrame();
}

/**
 * @brief Sets the border background color of the element.
 * 
 * This function sets the border background color of the element to the specified RGB value.
 * It marks the element as dirty for color updates and triggers a frame update.
 * 
 * @param color The RGB color to set as the border background color.
 */
void GGUI::element::setBorderBackgroundColor(RGB color) {
    // Set the border background color in the style
    Style->Border_Background_Color = color;
    
    // Mark the element as dirty for color updates
    Dirty.Dirty(INTERNAL::STAIN_TYPE::COLOR);
    
    // Update the frame to reflect the new color
    updateFrame();
}

/**
 * @brief Sets the text color of the element.
 * 
 * This function sets the text color of the element to the specified RGB value. 
 * It marks the element as dirty for color updates and triggers a frame update.
 * 
 * @param color The RGB color to set as the text color.
 */
void GGUI::element::setTextColor(RGB color){
    Style->Text_Color = color;
    // Mark the element as dirty for color updates
    Dirty.Dirty(INTERNAL::STAIN_TYPE::COLOR);
    // Update the frame to reflect the new color
    updateFrame();
}

/**
 * @brief Sets whether the element is allowed to dynamically resize.
 * 
 * This function enables or disables the ability of the element to 
 * adjust its size based on its content.
 * 
 * @param True A boolean indicating whether dynamic resizing is allowed.
 */
void GGUI::element::allowDynamicSize(bool True) {
    // Since dynamic size and percentage based size are two incompatible systems.
    if (Style->Width.number.Get_Type() != INTERNAL::EVALUATION_TYPE::PERCENTAGE && Style->Height.number.Get_Type() != INTERNAL::EVALUATION_TYPE::PERCENTAGE){
        Style->Allow_Dynamic_Size = True; 
    }
    // Set the Allow_Dynamic_Size property in the element's style
    // No need to update the frame, as this is used only on content change which triggers a frame update
}

/**
 * @brief Sets whether the element allows overflow.
 * 
 * This function enables or disables the overflow property of the element,
 * allowing child elements to exceed the parent's dimensions without resizing it.
 * 
 * @param True A boolean indicating whether overflow is allowed.
 */
void GGUI::element::allowOverflow(bool True) {
    // Update the Allow_Overflow property in the element's style
    Style->Allow_Overflow = True; 
    // No need to update the frame, as this is used only on content change which triggers a frame update
}

/**
 * @brief Sets the hover border color of the element.
 * 
 * This function sets the border color of the element when the mouse hovers over it
 * to the specified RGB value. Marks the element as dirty for color updates and
 * triggers a frame update.
 * 
 * @param color The RGB color to set as the hover border color.
 */
void GGUI::element::setHoverBorderColor(RGB color){
    Style->Hover_Border_Color = color;
    // Mark the element as dirty for color updates
    Dirty.Dirty(INTERNAL::STAIN_TYPE::COLOR);
    // Update the frame to reflect the new color
    updateFrame();
}

/**
 * @brief Sets the hover background color of the element.
 * 
 * This function sets the background color of the element when the mouse hovers over it
 * to the specified RGB value. Marks the element as dirty for color updates and triggers
 * a frame update.
 * 
 * @param color The RGB color to set as the hover background color.
 */
void GGUI::element::setHoverBackgroundColor(RGB color) {
    // Set the hover background color in the style
    Style->Hover_Background_Color = color;
    
    // Mark the element as dirty for color updates
    Dirty.Dirty(INTERNAL::STAIN_TYPE::COLOR);
    
    // Update the frame to reflect the new color
    updateFrame();
}

/**
 * @brief Sets the hover text color of the element.
 * 
 * This function sets the text color of the element when the mouse hovers over it
 * to the specified RGB value. Marks the element as dirty for color updates and triggers
 * a frame update.
 * 
 * @param color The RGB color to set as the hover text color.
 */
void GGUI::element::setHoverTextColor(RGB color) {
    // Set the hover text color in the style
    Style->Hover_Text_Color = color;
    
    // Mark the element as dirty for color updates
    Dirty.Dirty(INTERNAL::STAIN_TYPE::COLOR);
    
    // Update the frame to reflect the new color
    updateFrame();
}

/**
 * @brief Sets the hover border background color of the element.
 * 
 * This function sets the background color of the element's border 
 * when the mouse hovers over it to the specified RGB value. It marks 
 * the element as dirty for color updates and triggers a frame update.
 * 
 * @param color The RGB color to set as the hover border background color.
 */
void GGUI::element::setHoverBorderBackgroundColor(RGB color) {
    // Set the hover border background color in the style
    Style->Hover_Border_Background_Color = color;
    
    // Mark the element as dirty for color updates
    Dirty.Dirty(INTERNAL::STAIN_TYPE::COLOR);
    
    // Update the frame to reflect the new color
    updateFrame();
}

/**
 * @brief Sets the focus border color of the element.
 * 
 * This function sets the color of the element's border when it is focused to the specified RGB value. It marks the element as dirty for color updates and triggers a frame update.
 * 
 * @param color The RGB color to set as the focus border color.
 */
void GGUI::element::setFocusBorderColor(RGB color){
    Style->Focus_Border_Color = color;
    
    // Mark the element as dirty for color updates
    Dirty.Dirty(INTERNAL::STAIN_TYPE::COLOR);
    
    // Update the frame to reflect the new color
    updateFrame();
}

/**
 * @brief Sets the focus background color of the element.
 * 
 * This function sets the background color of the element when it is focused to the specified RGB value. It marks the element as dirty for color updates and triggers a frame update.
 * 
 * @param color The RGB color to set as the focus background color.
 */
void GGUI::element::setFocusBackgroundColor(RGB color){
    // Set the focus background color in the style
    Style->Focus_Background_Color = color;
    
    // Mark the element as dirty for color updates
    Dirty.Dirty(INTERNAL::STAIN_TYPE::COLOR);
    
    // Update the frame to reflect the new color
    updateFrame();
}

/**
 * @brief Sets the focus text color of the element.
 * 
 * This function sets the text color of the element when it is focused to the specified RGB value. It marks the element as dirty for color updates and triggers a frame update.
 * 
 * @param color The RGB color to set as the focus text color.
 */
void GGUI::element::setFocusTextColor(RGB color){
    Style->Focus_Text_Color = color;
    // Mark the element as dirty for color updates
    Dirty.Dirty(INTERNAL::STAIN_TYPE::COLOR);
    // Update the frame to reflect the new color
    updateFrame();
}

/**
 * @brief Sets the focus border background color of the element.
 * 
 * This function sets the focus border background color of the element to the specified RGB value.
 * It marks the element as dirty for color updates and triggers a frame update.
 * 
 * @param color The RGB color to set as the focus border background color.
 */
void GGUI::element::setFocusBorderBackgroundColor(RGB color){
    Style->Focus_Border_Background_Color = color;
    // Mark the element as dirty for color updates
    Dirty.Dirty(INTERNAL::STAIN_TYPE::COLOR);
    // Update the frame to reflect the new color
    updateFrame();
}

/**
 * @brief Sets the alignment of the element.
 * 
 * This function sets the alignment of the element to the specified ALIGN value.
 * 
 * @param Align The alignment value to set for the element.
 */
void GGUI::element::setAnchor(GGUI::ANCHOR Anchor){
    // Set the alignment in the style
    Style->Align = Anchor;
}

/**
 * @brief Sets the flow priority of the element.
 * 
 * This function sets the flow priority of the element to the specified DIRECTION value.
 * The flow priority determines how the element will be aligned in its parent when the parent is a flow layout.
 * 
 * @param Priority The flow priority value to set for the element.
 */
void GGUI::element::setFlowPriority(GGUI::DIRECTION Priority){
    Style->Flow_Priority = Priority;
}

/**
 * @brief Sets whether the element will wrap its contents to the next line when it hits the edge of the screen.
 * 
 * This function sets whether the element will wrap its contents to the next line when it hits the edge of the screen.
 * If true, the element will wrap its contents to the next line when it hits the edge of the screen.
 * If false, the element will not wrap its contents to the next line when it hits the edge of the screen.
 * 
 * @param Wrap The value to set for whether the element will wrap its contents to the next line.
 */
void GGUI::element::setWrap(bool Wrap){
    Style->Wrap = Wrap;
}

/**
 * @brief Recursively computes the size of the element based on its children.
 * 
 * This function will go through all the elements that are being displayed and
 * compute their size based on the size of their children. If the element has
 * children and the children have changed, then the element will be resized
 * to fit the children. If the element does not have children, then the function
 * will not do anything.
 * 
 * @note This function is called automatically by the framework when the
 *       elements are being rendered. It is not necessary to call this function
 *       manually.
 */
void GGUI::element::computeDynamicSize(){
    // Go through all elements displayed.
    if (!isDisplayed())
        return;

    if (!isDynamicSizeAllowed())
        return;

    if (childrenChanged()){
        // Iterate through all the elements that are being displayed.
        for (auto c : Style->Childs){
            if (!c->isDisplayed())
                continue;

            // Check the child first if it has to stretch before this can even know if it needs to stretch.
            c->computeDynamicSize();

            int Border_Offset = hasBorder() != c->hasBorder() && hasBorder() ? 1 * 2 : 0;

            // If the width is an percentage value, then it is always smaller or equal to this's width.
            int Enable_Width_Modification = (c->getWidthType() != INTERNAL::EVALUATION_TYPE::PERCENTAGE && getWidthType() != INTERNAL::EVALUATION_TYPE::PERCENTAGE) ? 1 : 0;  // Enable checking width if the width attribute type is an relative one
            // Do the same for the Height attribute
            int Enable_Height_Modification = (c->getHeightType() != INTERNAL::EVALUATION_TYPE::PERCENTAGE && getHeightType() != INTERNAL::EVALUATION_TYPE::PERCENTAGE) ? 1 : 0; // Enable checking height if the height attribute type is an relative one

            // Add the border offset to the width and the height to count for the border collision and evade it. 
            unsigned int New_Width = (unsigned int)GGUI::INTERNAL::Max(
                (c->Style->Position.get().X + (signed int)c->getWidth() + Border_Offset) * Enable_Width_Modification,
                (signed int)getWidth()
            );

            unsigned int New_Height = (unsigned int)GGUI::INTERNAL::Max(
                (c->Style->Position.get().Y + (signed int)c->getHeight() + Border_Offset) * Enable_Height_Modification,
                (signed int)getHeight()
            );

            // but only update those who actually allow dynamic sizing.
            if (New_Width != getWidth() || New_Height != getHeight()){
                setHeight(New_Height);
                setWidth(New_Width);
                Dirty.Dirty(INTERNAL::STAIN_TYPE::STRETCH);
            }
        }
    }

    return;
}

/**
 * @brief Apply the color system to the rendered string.
 *
 * This function applies the color system set by the style to the rendered string.
 * It is called after the element has been rendered and the result is stored in the
 * Result vector.
 *
 * @param Result The vector containing the rendered string.
 */
void GGUI::element::applyColors(std::vector<UTF>& Result){
    // Loop over each UTF-8 character in the rendered string and set its color to the
    // color specified in the style.
    const auto composedRGB = composeAllTextRGBValues();

    for (auto& utf : Result){
        utf.setColor(composedRGB);
    }
}

/**
 * @brief Add the border of the window to the rendered string.
 *
 * @param Result The string to add the border to.
 */
void GGUI::element::renderBorders(std::vector<UTF>& Result){
    Dirty.Clean(INTERNAL::STAIN_TYPE::EDGE);
    if (!hasBorder()) return;

    const unsigned int Width  = getWidth();
    const unsigned int Height = getHeight();
    const auto composedRGB    = composeAllBorderRGBValues();
    const auto& Border        = Style->Border_Style;

    // Corners
    Result[0] = GGUI::UTF(Border.topLeftCorner, composedRGB);
    Result[Width - 1] = GGUI::UTF(Border.topRightCorner, composedRGB);
    Result[(Height - 1) * Width] = GGUI::UTF(Border.bottomLeftCorner, composedRGB);
    Result[(Height * Width) - 1] = GGUI::UTF(Border.bottomRightCorner, composedRGB);

    // Top and Bottom horizontal borders
    for (unsigned int x = 1; x < Width - 1; ++x) {
        Result[x] = GGUI::UTF(Border.horizontalLine, composedRGB);                          // Top row
        Result[(Height - 1) * Width + x] = GGUI::UTF(Border.horizontalLine, composedRGB);   // Bottom row
    }

    // Left and Right vertical borders
    for (unsigned int y = 1; y < Height - 1; ++y) {
        Result[y * Width] = GGUI::UTF(Border.verticalLine, composedRGB);            // Left column
        Result[y * Width + (Width - 1)] = GGUI::UTF(Border.verticalLine, composedRGB); // Right column
    }
}


/**
 * @brief Renders the title of the element into the provided result buffer.
 * 
 * This function writes the title text of the element into the `Result` vector,
 * taking into account the available width and applying an ellipsis ("...") if
 * the title is too long to fit within the allocated space. The title is styled
 * using the element's text RGB values.
 * 
 * @param Result A vector of UTF objects where the rendered title will be stored.
 *               The vector should have sufficient size to accommodate the rendered text.
 * 
 * @details
 * - If the title is empty, the function returns immediately without modifying `Result`.
 * - The title's length is determined by the `Style->Title.Value.size`.
 * - The function calculates the writable length based on the element's width,
 *   taking into account borders and the space required for the ellipsis.
 * - If the title exceeds the writable length, an ellipsis is appended to indicate truncation.
 * - The function ensures that no characters are written beyond the available width.
 * 
 * @note The function assumes that the `Result` vector is pre-allocated and large enough
 *       to hold the rendered title and ellipsis.
 */
void GGUI::element::renderTitle(std::vector<UTF>& Result){
    if (Style->Title.empty())
        return;

    unsigned int Title_Length = Style->Title.Value.size; // +1 for trailing, since Compact_Strings do not include trailing characters in their size.
    unsigned int Horizontal_Offset = hasBorder();
    INTERNAL::compactString Ellipsis = "...";
    bool Enable_Ellipsis = false;

    unsigned int Writable_Length = INTERNAL::Min(Title_Length, getWidth() - Horizontal_Offset - Ellipsis.size - 1);

    if (Writable_Length < Title_Length)
        Enable_Ellipsis = true;

    // Now we'll write what we can
    for (unsigned int x = Horizontal_Offset; x < Writable_Length + Horizontal_Offset; x++){
        Result[x] = UTF(Style->Title.Value[x - Horizontal_Offset], composeAllTextRGBValues());
    }

    // And then we'll add the ellipsis
    if (Enable_Ellipsis){
        unsigned int Ellipsis_Offset = Writable_Length + Horizontal_Offset;
        for (unsigned int x = 0; x < Ellipsis.size; x++){
            if (Ellipsis_Offset + x < getWidth()){
                Result[Ellipsis_Offset + x] = UTF(Ellipsis[x], composeAllTextRGBValues());
            }
        }
    }
}

inline bool Is_In_Bounds(GGUI::IVector3 index, GGUI::element* parent){
    // checks if the index is out of bounds
    if (index.X < 0 || index.Y < 0 || index.X >= (signed)parent->getWidth() || index.Y >= (signed)parent->getHeight())
        return false;

    return true;
}

inline GGUI::UTF* From(GGUI::IVector3 index, std::vector<GGUI::UTF>& Parent_Buffer, GGUI::element* Parent){
    return &Parent_Buffer[index.Y * Parent->getWidth() + index.X];
}

/**
 * @brief Sets the custom border style for the element.
 * @details This function sets the custom border style for the element, marks the element's edges as dirty, and ensures that the border is visible.
 * @param style The custom border style to set.
 */
void GGUI::element::setCustomBorderStyle(GGUI::styledBorder style) {
    // Set the border style of the element
    Style->Border_Style = style;
    
    // Mark the border as needing an update
    Dirty.Dirty(INTERNAL::STAIN_TYPE::EDGE);

    // Ensure the border is visible
    showBorder(true);
}

/**
 * @brief Posts a process that handles the intersection of borders between two elements and their parent.
 * @details This function posts a process that handles the intersection of borders between two elements and their parent.
 *          The process calculates the intersection points of the borders and then constructs a bit mask that portraits the connections the middle point has.
 *          With the calculated bit mask it can fetch from the 'SYMBOLS::Border_Identifiers' the right border string.
 * @param A The first element.
 * @param B The second element.
 * @param Parent_Buffer The buffer of the parent element.
 */
void GGUI::element::postProcessBorders(element* A, element* B, std::vector<UTF>& Parent_Buffer){
    // We only need to calculate the childs points in which they intersect with the parent borders.
    // At these intersecting points of border we will construct a bit mask that portraits the connections the middle point has.
    // With the calculated bit mask we can fetch from the 'SYMBOLS::Border_Identifiers' the right border string.

    // First calculate if the childs borders even touch the parents borders.
    // If not, there is no need to calculate anything.

    // First calculate if the child is outside the parent.
    if (
        B->Style->Position.get().X + (signed)B->getWidth() < A->Style->Position.get().X ||
        B->Style->Position.get().X > A->Style->Position.get().X + (signed)A->getWidth() ||
        B->Style->Position.get().Y + (signed)B->getHeight() < A->Style->Position.get().Y ||
        B->Style->Position.get().Y > A->Style->Position.get().Y + (signed)A->getHeight()
    )
        return;

    // Now calculate if the child is inside the parent.
    if (
        B->Style->Position.get().X > A->Style->Position.get().X &&
        B->Style->Position.get().X + (signed)B->getWidth() < A->Style->Position.get().X + (signed)A->getWidth() &&
        B->Style->Position.get().Y > A->Style->Position.get().Y &&
        B->Style->Position.get().Y + (signed)B->getHeight() < A->Style->Position.get().Y + (signed)A->getHeight()
    )
        return;

    // Now that we are here it means the both boxes interlace each other.
    // We will calculate the hitting points by drawing segments from corner to corner and then comparing one segments x to other segments y, and so forth.

    // two nested loops rotating the x and y usages.
    // store the line x,y into a array for the nested loops to access.
    std::vector<int> Vertical_Line_X_Coordinates = {
        
        B->Style->Position.get().X,
        A->Style->Position.get().X,
        B->Style->Position.get().X + (int)B->getWidth() - 1,
        A->Style->Position.get().X + (int)A->getWidth() - 1,

                
        // A->Style->Position.Get().X,
        // B->Style->Position.Get().X,
        // A->Style->Position.Get().X + A->Width - 1,
        // B->Style->Position.Get().X + B->Width - 1

    };

    std::vector<int> Horizontal_Line_Y_Coordinates = {
        
        A->Style->Position.get().Y,
        B->Style->Position.get().Y + (int)B->getHeight() - 1,
        A->Style->Position.get().Y,
        B->Style->Position.get().Y + (int)B->getHeight() - 1,

        // B->Position.Y,
        // A->Position.Y + A->Height - 1,
        // B->Position.Y,
        // A->Position.Y + A->Height - 1,

    };

    std::vector<IVector3> Crossing_Indicies;

    // Go through singular box
    for (unsigned int Box_Index = 0; Box_Index < Horizontal_Line_Y_Coordinates.size(); Box_Index++){
        // Now just pair the indicies from the two lists.
        Crossing_Indicies.push_back(
            // First pair
            IVector3(
                Vertical_Line_X_Coordinates[Box_Index],
                Horizontal_Line_Y_Coordinates[Box_Index]
            )
        );
    }

    // Now that we have the crossing points we can start analyzing the ways they connect to construct the bit masks.
    for (auto c : Crossing_Indicies){

        IVector3 Above = { c.X, c.Y - 1 };
        IVector3 Below = { c.X, c.Y + 1 };
        IVector3 Left = { c.X - 1, c.Y };
        IVector3 Right = { c.X + 1, c.Y };

        INTERNAL::borderConnection Current_Masks = INTERNAL::borderConnection::NONE;

        // These selected coordinates can only contain something related to the borders and if the current UTF is unicode then it is an border.
        if (Is_In_Bounds(Above, this)){
            // Since the border above can be already processed we need to check all possible border variations.
            INTERNAL::borderConnection borderAbove = A->getCustomBorderStyle().getBorderType(From(Above, Parent_Buffer, this)->getUnicode()) | 
                                            B->getCustomBorderStyle().getBorderType(From(Above, Parent_Buffer, this)->getUnicode());
            
            if (borderAbove != INTERNAL::borderConnection::NONE)
                Current_Masks |= INTERNAL::borderConnection::UP;
        }

        if (Is_In_Bounds(Below, this)){
            // Since the border below can be already processed we need to check all possible border variations.
            INTERNAL::borderConnection borderBelow = A->getCustomBorderStyle().getBorderType(From(Below, Parent_Buffer, this)->getUnicode()) | 
                                            B->getCustomBorderStyle().getBorderType(From(Below, Parent_Buffer, this)->getUnicode());
            
            if (borderBelow != INTERNAL::borderConnection::NONE)
                Current_Masks |= INTERNAL::borderConnection::DOWN;
        }

        if (Is_In_Bounds(Left, this)){
            // Since the border left can be already processed we need to check all possible border variations.
            INTERNAL::borderConnection borderLeft = A->getCustomBorderStyle().getBorderType(From(Left, Parent_Buffer, this)->getUnicode()) | 
                                           B->getCustomBorderStyle().getBorderType(From(Left, Parent_Buffer, this)->getUnicode());
            
            if (borderLeft != INTERNAL::borderConnection::NONE)
                Current_Masks |= INTERNAL::borderConnection::LEFT;
        }

        if (Is_In_Bounds(Right, this)){
            // Since the border right can be already processed we need to check all possible border variations.
            INTERNAL::borderConnection borderRight = A->getCustomBorderStyle().getBorderType(From(Right, Parent_Buffer, this)->getUnicode()) | 
                                            B->getCustomBorderStyle().getBorderType(From(Right, Parent_Buffer, this)->getUnicode());
            
            if (borderRight != INTERNAL::borderConnection::NONE)
                Current_Masks |= INTERNAL::borderConnection::RIGHT;
        }

        const char* finalBorder = A->getBorderStyle().getBorder(Current_Masks);

        if (!finalBorder){
            continue;
        }

        From(c, Parent_Buffer, this)->setText(finalBorder);
    }
}

//End of utility functions.

/**
 * @brief A function that registers a lambda to be executed when the element is clicked.
 * @details The lambda is given a pointer to the Event object that triggered the call.
 *          The lambda is expected to return true if it was successful and false if it failed.
 * @param action The lambda to be called when the element is clicked.
 */
void GGUI::element::onClick(std::function<bool(GGUI::event*)> job){
    auto wrapper = [this, job](GGUI::event* e){
        // As os 0.1.8 no need to check for mouse collision with current element, since mouse collision is already checked at the eventHandler scheduler.

        // Construct an Action from the Event obj
        GGUI::action* event2actionWrapper = new GGUI::action(e->criteria, job, this, getName() + "::onClick");

        //action successfully executed.
        return job(event2actionWrapper);
    };
    
    action* mouse = new action(
        constants::MOUSE_LEFT_CLICKED,
        wrapper,
        this,
        getName() + "::onClick::wrapper::mouse"
    );

    action* enter = new action(
        constants::ENTER,
        wrapper,
        this,
        getName() + "::onClick::wrapper::enter"
    );

    GGUI::INTERNAL::Event_Handlers.push_back(mouse);
    GGUI::INTERNAL::Event_Handlers.push_back(enter);
}

/**
 * @brief A function that registers a lambda to be executed when the element is interacted with in any way.
 * @details The lambda is given a pointer to the Event object that triggered the call.
 *          The lambda is expected to return true if it was successful and false if it failed.
 * @param criteria The criteria to check for when deciding whether to execute the lambda.
 * @param action The lambda to be called when the element is interacted with.
 * @param GLOBAL Whether the lambda should be executed even if the element is not under the mouse.
 */
void GGUI::element::on(unsigned long long criteria, std::function<bool(GGUI::event*)> job, bool GLOBAL){
    action* a = new action(
        criteria,
        [this, job, GLOBAL](GGUI::event* e){
            if (INTERNAL::Collides(this, INTERNAL::Mouse) || GLOBAL){
                // action successfully executed.
                return job(e);
            }
            // action failed.
            return false;
        },
        this,
        getName() + "::on::" + std::to_string(criteria)
    );
    GGUI::INTERNAL::Event_Handlers.push_back(a);
}

/**
 * @brief Check if any children have changed.
 * @details This function will check if any of the children have changed, this is used to determine if the element needs to be re-drawn.
 * @return true if any children have changed, false otherwise.
 */
bool GGUI::element::childrenChanged(){
    for (auto* e : Style->Childs){
        if (e->getDirty().is(INTERNAL::STAIN_TYPE::FINALIZE)){
            GGUI::INTERNAL::reportStack("Child element passthrough Finalization stage!");
        }

        // Not counting State machine, if element is not being drawn return always false.
        if (!e->Show)
            return false;

        if (e->getDirty().Type != INTERNAL::STAIN_TYPE::CLEAN)
            return true;

        if (e->childrenChanged())
            return true;
    }

    return false;
}

/**
 * @brief Check if there are any transparent children.
 * @details This function determines if the current element or any of its children
 *          are transparent and require redrawing.
 * @return True if any child is transparent and not clean; otherwise, false.
 */
bool GGUI::element::hasTransparentChildren() {
    // If the element is not visible, return false.
    if (!Show)
        return false;

    // Recursively check each child element for transparency.
    for (auto e : Style->Childs) {
        if (e->isTransparent())
            return true;

        if (e->hasTransparentChildren())
            return true;
    }

    // No transparent children found.
    return false;
}

/**
 * @brief Retrieves the name of the element as a raw string.
 * 
 * If the element's name is not set (i.e., the Name string is empty),
 * this function returns the memory address of the element as a string.
 * Otherwise, it returns the Name string.
 * 
 * @return A std::string containing either the element's name or its memory address.
 */
std::string GGUI::element::getNameAsRaw() const {
    if (Name.size() == 0) return std::to_string((unsigned long long)this);
    else return Name;
}

/**
 * @brief Checks if the element's name is empty.
 * 
 * @return true if the name of the element has no characters, false otherwise.
 */
bool GGUI::element::hasEmptyName() const {
    return Name.size() == 0;
}

/**
 * @brief Set the name of the element.
 * @details This function sets the name of the element and stores it in the global Element_Names map.
 * @param name The name of the element.
 */
void GGUI::element::setName(std::string name){
    // Set the name of the element.
    Name = name;

    // Store the element in the global Element_Names map.
    INTERNAL::Element_Names[name] = this;
}

/**
 * @brief Retrieves an element by its name.
 * 
 * This function searches through the child elements of the current element
 * to find an element with the specified name. It performs a recursive search
 * through all descendants.
 * 
 * @param name The name of the element to search for.
 * @return A pointer to the element with the specified name, or nullptr if no such element is found.
 */
GGUI::element* GGUI::element::getElement(std::string name){
    for (auto* c : getChilds()){
        if (c->getNameAsRaw() == name)
            return c;

        element* tmp = c->getElement(name);

        if (tmp){
            return tmp;
        }
    }

    return nullptr;
}

/**
 * @brief Retrieves all nested elements, including this element.
 * @details This function collects all nested elements recursively, starting from this element.
 *          If 'Show_Hidden' is false, hidden elements are excluded from the result.
 * @param Show_Hidden Flag to determine whether to include hidden elements in the result.
 * @return A vector of pointers to all nested elements.
 */
std::vector<GGUI::element*> GGUI::element::getAllNestedElements(bool Show_Hidden) {
    std::vector<element*> result;

    // If the element is not visible and hidden elements should not be shown, return an empty vector.
    if (!Show && !Show_Hidden)
        return {};

    // Add the current element to the result vector.
    result.push_back(this);

    // Recursively retrieve all nested elements from child elements.
    for (auto e : getChilds()) {
        std::vector<element*> child_result = e->getAllNestedElements(Show_Hidden);
        result.insert(result.end(), child_result.begin(), child_result.end());
    }

    // Return the result vector containing all nested elements.
    return result;
}

/**
 * @brief Reorders child elements based on their z-position.
 * @details This function sorts the child elements of the current element by their z-coordinate
 *          in ascending order, so that elements with a higher z-coordinate appear later in the list.
 */
void GGUI::element::reOrderChilds() {
    // Sort the child elements using a lambda function to compare the z-coordinates.
    std::sort(Style->Childs.begin(), Style->Childs.end(), [](element* a, element* b) {
        // Compare the z-position of the two elements.
        return a->getPosition().Z < b->getPosition().Z;
    });
}

/**
 * @brief Focuses the element.
 * @details This function updates the global focus information by setting the mouse position to the element's position and updating the focused element.
 */
void GGUI::element::focus() {
    // Set the mouse position to the element's position.
    GGUI::INTERNAL::Mouse = this->Style->Position.get();
    // Update the focused element.
    GGUI::INTERNAL::updateFocusedElement(this);
}

/**
 * @brief Adds a handler function to the state handlers map.
 * @details This function takes a state and a handler function as arguments.
 *          The handler function is stored in the State_Handlers map with the given state as the key.
 * @param s The state for which the handler should be executed.
 * @param job The handler function to be executed
 */
void GGUI::element::onState(INTERNAL::STATE s, void (*job)(element* self)){
    if (s == INTERNAL::STATE::INIT)
        On_Init = job;
    else if (s == INTERNAL::STATE::DESTROYED)
        On_Destroy = job;
    else if (s == INTERNAL::STATE::HIDDEN)
        On_Hide = job;
    else if (s == INTERNAL::STATE::SHOWN)
        On_Show = job;
}

bool Is_Signed(int x){
    return x < 0;
}

int Get_Sign(int x){
    return Is_Signed(x) ? -1 : 1;
}

// Constructs two squares one 2 steps larger on width and height, and given the different indicies.
std::vector<GGUI::IVector3> Get_Surrounding_Indicies(int Width, int Height, GGUI::IVector3 start_offset, GGUI::FVector2 Offset){

    std::vector<GGUI::IVector3> Result;

    // First construct the first square.
    int Bigger_Square_Start_X = start_offset.X - 1;
    int Bigger_Square_Start_Y = start_offset.Y - 1;

    int Bigger_Square_End_X = start_offset.X + Width + 1;
    int Bigger_Square_End_Y = start_offset.Y + Height + 1;

    int Smaller_Square_Start_X = start_offset.X + (Offset.X * GGUI::INTERNAL::Min(0, (int)Offset.X));
    int Smaller_Square_Start_Y = start_offset.Y + (Offset.Y * GGUI::INTERNAL::Min(0, (int)Offset.Y));

    int Smaller_Square_End_X = start_offset.X + Width - (Offset.X * GGUI::INTERNAL::Max(0, (int)Offset.X));
    int Smaller_Square_End_Y = start_offset.Y + Height - (Offset.Y * GGUI::INTERNAL::Max(0, (int)Offset.Y));

    for (int y = Bigger_Square_Start_Y; y < Bigger_Square_End_Y; y++){
        for (int x = Bigger_Square_Start_X; x < Bigger_Square_End_X; x++){

            bool Is_Inside_Smaller_Square = x >= Smaller_Square_Start_X && x < Smaller_Square_End_X && y >= Smaller_Square_Start_Y && y < Smaller_Square_End_Y;

            // Check if the current coordinates are outside the smaller square.
            if (!Is_Inside_Smaller_Square)
                Result.push_back({ x, y });
        }
    }

    return Result;

}

/**
 * @brief
 * This function determines if the given element is a direct child of this element (in the DOM tree),
 * and if it is visible on the screen (does not go out of bounds of the parent element).
 * @param other Child element to check
 * @return True if the child element is visible within the bounds of the parent.
 */
bool GGUI::element::childIsShown(element* other){

    // Check if the element has a border
    bool Border_Modifier = hasBorder() != other->hasBorder() && hasBorder() ? 1 : 0;

    // Calculate the minimum and maximum coordinates of the child element
    int Minimum_X = other->Style->Position.get().X + other->getWidth();
    int Minimum_Y = other->Style->Position.get().Y + other->getHeight();
    int Maximum_X = other->Style->Position.get().X;
    int Maximum_Y = other->Style->Position.get().Y;

    // Check if the child element is visible within the bounds of the parent
    bool X_Is_Inside = Minimum_X >= Border_Modifier && Maximum_X < (signed)getWidth() - Border_Modifier;
    bool Y_Is_Inside = Minimum_Y >= Border_Modifier && Maximum_Y < (signed)getHeight() - Border_Modifier;

    // Return true if the child element is visible within the bounds of the parent
    return X_Is_Inside && Y_Is_Inside;
}