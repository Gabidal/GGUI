#include "element.h"

#include "../core/core.h"
#include "../core/utils/utils.h"

#include <algorithm>
#include <vector>

#undef min
#undef max

namespace GGUI {
    /**
    * @brief Constructor for the element class.
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
    element::element(STYLING_INTERNAL::styleBase& Style, bool Embed_Styles_On_Construct){
        fullyStain();

        flags |= (stain::types::FINALIZE);

        style = new styling(Style);

        if (Embed_Styles_On_Construct){
            style->embedStyles(this);

            check(STATE::INIT);

            // Tell the main Main->Embed_Stylings() to not call this elements On_Init, since it is already called here.
            flags ^= (stain::types::FINALIZE);
        }
        else{
            // if the styles are to be embedded later on, then we need to make an deep copy of the whole list because the stack is about to be cleared.
            style->copyUnParsedStyles();
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
    element::~element(){
        // Call handler for on destroying moment.
        check(STATE::DESTROYED);

        // Make sure this element is not listed in the parent element.
        // And if it does, then remove it from the parent element.
        if (parent) {
            // Find the element in the parent's vector of child elements and remove it.
            for (unsigned int i = 0; i < parent->style->Childs.size(); i++)
                if (parent->style->Childs[i] == this){
                    parent->style->Childs.erase(parent->style->Childs.begin() + i);

                    // This may not be enough for the parent to know where to resample the buffer where this child used to be.
                    parent->flags |= (stain::types::DEEP);

                    break;  // There should be no possibility, that there are appended two or more of this exact same element, they should be copied!!!
                }
        }

        // Fire all the childs.
        for (int i = (signed)style->Childs.size() -1; i >= 0; i--)
            if (style->Childs[i]->parent == this) 
                delete style->Childs[i];

        // Delete all the styles.
        delete style;

        style = nullptr;    // For safety, if in future some destruction system is going to need to know if this is no longer accessble

        //now also update the event handlers.
        for (size_t i = 0; i < core::inputConverter->handlers.size(); i++) {
            if (core::inputConverter->handlers[i] == this) {
                core::inputConverter->handlers.erase(core::inputConverter->handlers.begin() + i);
                // don't increment i, since elements shifted left

                break;
            }
        }

        // Now make sure that if the Focused_On element points to this element, then set it to nullptr
        if (isFocused())
            core::focusedOn = nullptr;

        // Now make sure that if the Hovered_On element points to this element, then set it to nullptr
        if (isHovered())
            core::hoveredOn = nullptr;
    }   

    /**
    * @brief Renders the element and its children into the Render_Buffer nested buffer of the window.
    * @details This function processes the element to generate a vector of UTF objects representing the current state.
    * It handles different stains such as CLASS, STRETCH, COLOR, and EDGE to ensure the element is rendered correctly.
    * @return A vector of UTF objects representing the rendered element and its children.
    */
    std::vector<terminal::cell>& element::render(){
        // Check for Dynamic attributes
        if(style->evaluateDynamicDimensions(this))
            flags |= (stain::types::STRETCH);

        if (style->evaluateDynamicPosition(this))
            flags |= (stain::types::MOVE);

        if (style->evaluateDynamicGraphics(this))
            flags |= (stain::types::GRAPHICS);

        if (style->evaluateDynamicBorder(this))
            flags |= (stain::types::EDGE);

        calculateChildsHitboxes();    // Normally elements will NOT order their content by hitbox system.

        computeDynamicSize();

        //if inned children have changed without this changing, then this will trigger.
        if (!flags.has(stain::base(stain::types::STRETCH) | stain::types::RESET)){
            bool tmp = childrenChanged();

            if (!tmp && flags.isEmpty()){
                return cellBuffer;
            }
            else if (tmp || hasTransparentChildren()){
                flags |= (stain::types::RESET);
            }
        }

        // This is to tell the rendering thread that some or no changes were made to the rendering buffer.
        if (this == getRoot() && !flags.isEmpty()){
            thread::identicalFrame = false;
        }

        if (flags.isEmpty())
            return cellBuffer;

        if (flags.has(stain::types::MOVE)){
            flags ^= stain::types::MOVE;

            updateAbsolutePositionCache();
        }

        if (flags.has(stain::types::RESET)){
            flags ^= (stain::types::RESET);

            std::fill(cellBuffer.begin(), cellBuffer.end(), ' ');
            
            flags |= (stain::base(stain::types::GRAPHICS) | stain::types::EDGE | stain::types::DEEP);
        }

        if (flags.has(stain::types::STRETCH)){
            flags ^= (stain::types::STRETCH);
            
            cellBuffer.clear();
            cellBuffer.resize(getWidth() * getHeight(), ' ');

            flags |= (stain::base(stain::types::GRAPHICS) | stain::types::EDGE | stain::types::DEEP | stain::types::NOT_RENDERED);
        }

        if (flags.has(stain::types::NOT_RENDERED)) {
            if (onRender) onRender(this);

            // Clean regardless of On_Render existing or not.
            flags ^= (stain::types::NOT_RENDERED);
        }

        bool Connect_Borders_With_Parent = hasBorder();
        unsigned int Childs_With_Borders = 0;

        //This will add the child windows to the Result buffer
        if (flags.has(stain::types::DEEP)){
            flags ^= (stain::types::DEEP);

            // clean reflection pool
            graphicalReflectionPool.clear();
            graphicalIdentityPool.clear();
            flags |= (stain::types::GRAPHICS);

            for (element* c : this->style->Childs){
                // check if the child is within the rendering area.
                if (!c || !c->isDisplayed() || !childIsShown(c))
                    continue;

                if (c->hasBorder())
                    Childs_With_Borders++;

                const std::vector<terminal::cell>& tmp = c->render();

                // compile graphical reflection pool
                graphicalReflectionPool.insert(graphicalReflectionPool.end(), c->graphicalReflectionPool.begin(), c->graphicalReflectionPool.end());

                core::nestElement(this, c, cellBuffer, tmp);
            }
        }

        if (flags.has(stain::types::GRAPHICS)) {     // Resets baked graphics and reserves for child*2+2, for the incoming deep stains.
            flags ^= (stain::types::GRAPHICS);

            compileActiveGraphics();    // compiles identifying graphics pools
        }

        if (Childs_With_Borders > 0 && Connect_Borders_With_Parent)
            flags |= (stain::types::EDGE);

        //This will add the borders if necessary and the title of the window.
        if (flags.has(stain::types::EDGE)){
            flags ^= (stain::types::EDGE);

            renderBorders(cellBuffer);
            renderTitle(cellBuffer);
        }

        // This will calculate the connecting borders.
        if (Childs_With_Borders > 0){
            for (auto A : this->style->Childs){
                for (auto B : this->style->Childs){
                    if (A == B)
                        continue;

                    if (!A->isDisplayed() || !A->hasBorder() || !B->isDisplayed() || !B->hasBorder())
                        continue;

                    postProcessBorders(A, B, cellBuffer);
                }

                postProcessBorders(this, A, cellBuffer);
            }
        }

        return cellBuffer;
    }

    /**
    * @brief Sets the opacity of the element.
    * @details This function takes a float value between 0.0f and 1.0f and sets the
    * opacity of the element to that value. If the value is greater than 1.0f, the
    * function will report an error and do nothing.
    * @param[in] Opacity The opacity value to set.
    */
    void element::setOpacity(float Opacity){
        if (Opacity > 1.0f)
            logger::log("Opacity value is too high: " + std::to_string(Opacity) + " for element: " + getName());

        style->Opacity.Set(Opacity);

        flags |= (stain::types::RESET);
        updateFrame();
    }

    /**
    * @brief Sets the opacity of the element using an integer percentage.
    * @details This function takes an unsigned integer value between 0 and 100, representing the opacity percentage,
    * and sets the element's opacity. If the value is greater than 100, it will report an error and do nothing.
    * @param[in] Opacity The opacity percentage to set.
    */
    void element::setOpacity(unsigned int Opacity) {
        constexpr unsigned int MAX_OPACITY = 100;
        constexpr unsigned int HALF_OPACITY = 50;
        // Check if the provided opacity is within valid range (0-100)
        if (Opacity > MAX_OPACITY) {
            // Report an error if the opacity value is too high
            logger::log("Opacity value is too high: " + std::to_string(Opacity) + " for element: " + getName());
        }

        // Convert the opacity percentage to an 8-bit value (0..255) and set it.
        // Keep behavior consistent with existing code: even if Opacity > 100 is reported, we still set (saturated).
        unsigned int P = Opacity;
        if (P >= MAX_OPACITY) {
            style->Opacity.Set((unsigned char)UINT8_MAX);
        } else {
            // Rounded mapping: 0..99 -> 0..252/253, 100 -> 255
            const unsigned int Byte = (P * (unsigned)UINT8_MAX + HALF_OPACITY) / MAX_OPACITY;
            style->Opacity.Set((unsigned char)Byte);
        }

        // Mark the element as dirty to trigger a visual update
        flags |= (stain::types::RESET);
        updateFrame(); // Update the frame to reflect the changes
    }

    /**
    * @brief Gets the opacity of the element.
    * @details This function returns the current opacity of the element as a float value.
    *          The opacity is a value between 0.0 and 1.0, where 0.0 is fully transparent
    *          and 1.0 is fully opaque.
    * @return The current opacity of the element.
    */
    float element::getOpacity() const {
        return (float)getOpacityByte() / (float)UINT8_MAX;
    }

    unsigned char element::getOpacityByte() const {
        return style->Opacity.Get();
    }

    /**
    * @brief Checks if the element is transparent.
    * @details This function determines whether the element is transparent by checking
    *          if the element's opacity is not equal to 1.0f. An opacity less than 1.0f
    *          indicates that the element is partially or fully transparent.
    * @return True if the element is transparent; otherwise, false.
    */
    bool element::isTransparent() const {
        return getOpacityByte() != UINT8_MAX;
    }

    /**
    * @brief Sets the parent of this element.
    * @details This function sets the parent of this element to the given element.
    *          If the given element is nullptr, it will clear the parent of this
    *          element.
    * @param[in] parent The parent element to set.
    */
    void element::setParent(element* Parent){
        if (Parent){
            parent = Parent;
        } else {
            parent = nullptr;
        }
    }

    /**
    * @brief Sets the focus state of the element.
    * @details Sets the focus state of the element to the given value.
    *          If the focus state changes, the element will be dirtied and the frame will be updated.
    * @param f The new focus state.
    */
    void element::setFocus(bool f){
        if (f != focused){
            // If the focus state has changed, dirty the element and update the frame.
            flags |= (stain::base(stain::types::GRAPHICS) | stain::types::EDGE);

            focused = f;

            updateFrame();
        }
    }

    /**
    * @brief Sets the hover state of the element.
    * @details Sets the hover state of the element to the given value.
    *          If the hover state changes, the element will be dirtied and the frame will be updated.
    * @param h The new hover state.
    */
    void element::setHoverState(bool h){
        if (h != hovered){
            // If the hover state has changed, dirty the element and update the frame.
            flags |= (stain::base(stain::types::GRAPHICS) | stain::types::EDGE);

            hovered = h;

            updateFrame();
        }
    }

    /**
    * @brief Retrieves the styling information of the element.
    * @details This function returns the current styling object associated with the element.
    *          The styling object contains various style attributes such as colors, borders, etc.
    * @return The styling object of the element.
    */
    styling element::getStyle() const {
        // Return the styling object associated with this element.
        return *style;
    }

    /**
    * @brief Retrieves the direct styling associated with this element.
    * 
    * This function returns a pointer to the styling object (`styling`) 
    * that is directly associated with the current element. The returned 
    * styling object can be used to access or modify the visual properties 
    * of the element.
    * 
    * @return styling* Pointer to the direct styling object of the element.
    */
    styling* element::getDirectStyle() const {
        return style;
    }

    /**
    * @brief Sets the styling information of the element.
    * @details This function sets the styling information of the element to the given value.
    *          If the element already has a styling object associated with it, the function will
    *          copy the given styling information to the existing object. Otherwise, the function
    *          will create a new styling object and associate it with the element.
    * @param css The new styling information to associate with the element.
    */
    void element::setStyle(styling css){
        if (style)
            style->copy(css);
        else
            style = new styling(css);

        // Update the frame after changing the styling information.
        updateFrame();
    }

    /**
    * @brief Sets the border visibility of the element.
    * @details This function takes a boolean as a parameter and sets the border visibility of the element accordingly.
    *          If the new state is different from the current state, the element will be marked as dirty with the EDGE stain.
    * @param b The new state of the border visibility.
    */
    void element::showBorder(bool b){
        if (b != style->Border_Enabled.value){
            style->Border_Enabled = b;
            flags |= (stain::types::EDGE);
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
    void element::showBorder(bool b, bool Previous_State) {
        if (b != Previous_State) {
            // Update the border enabled state
            style->Border_Enabled = b;

            // Mark the element as dirty for border changes
            flags |= (stain::types::EDGE);

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
    bool element::hasBorder() const{
        return style->Border_Enabled.value;
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
    void element::addChild(element* Child){
        // Since 0.1.8 we need to check if the given Element is Fully initialized with Style embeddings or not.
        if (Child->flags.has(stain::types::FINALIZE)){
            // Finalize flag is cleaned Style Embedding with On_Init Call.
            // Give an early access to the parent, so that parent dependant attributes work properly.
            Child->parent = this;
            
            Child->embedStyles();
        }

        int Border_Offset =  hasBorder() != Child->hasBorder() && hasBorder() ? 1 : 0;

        if (
            Child->style->Position.get().x + Child->getWidth() > (getWidth() - Border_Offset) || 
            Child->style->Position.get().y + Child->getHeight() > (getHeight() - Border_Offset)
        ){
            if (style->Allow_Dynamic_Size.value){
                // Add the border offset to the width and the height to count for the border collision and evade it. 
                unsigned int New_Width = std::max(Child->style->Position.get().x + Child->getWidth() + Border_Offset*2, getWidth());
                unsigned int New_Height = std::max(Child->style->Position.get().y + Child->getHeight() + Border_Offset*2, getHeight());

                // Resize the parent element to fit the child element
                setHeight(New_Height);
                setWidth(New_Width);
            }
            else if (Child->resizeTo(this) == false){

                // Report an error if the child element exceeds the size of the parent element and the parent element is not allowed to resize
                // logger::report(
                //     "Window exceeded static bounds\n "
                //     "Starts at: {" + std::to_string(Child->Style->Position.Get().X) + ", " + std::to_string(Child->Style->Position.Get().Y) + "}\n "
                //     "Ends at: {" + std::to_string(Child->Style->Position.Get().X + Child->getWidth()) + ", " + std::to_string(Child->Style->Position.Get().Y + Child->getHeight()) + "}\n "
                //     "Max is at: {" + std::to_string(getWidth() - hasBorder()) + ", " + std::to_string(getHeight() - hasBorder()) + "}\n "
                // );

                // return;
            }
        }

        // Mark the parent element as dirty with the DEEP stain
        flags |= (stain::types::DEEP);

        // Add the child element to the parent's child list
        core::elementNames.insert({Child->getNameAsRaw(), Child});

        style->Childs.push_back(Child);

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
    void element::setChilds(std::vector<element*> childs){
        pauseGGUI([this, childs](){
            for (auto& Child : childs){
                addChild(Child);
            }
            flags |= (stain::types::DEEP);
        });
    }

    /**
    * @brief Retrieves the list of child elements.
    * @details This function returns a reference to the vector containing all child elements
    *          associated with the current element's style.
    * @return A reference to the vector of child elements.
    */
    std::vector<element*>& element::getChilds() {
        // Return the vector of child elements from the style object.
        return style->Childs;
    }

    std::vector<element*> element::getVisibleChilds() {
        std::vector<element*> result;
        result.reserve(getChilds().size());

        for (auto* c : getChilds()) {
            if (!c->isDisplayed() || !childIsShown(c))
                    continue;

            result.push_back(c);
        }

        // sort result by z-priority, higher z is first
        std::sort(result.begin(), result.end(), [](const element* a, const element* b) {
            return a->getPosition().z > b->getPosition().z;
        });

        return result;
    }

    std::vector<IVector2> element::getDeltaPoints() {
        std::vector<IVector2> result;
        // Rough heuristic to prevent constant reallocations
        result.reserve(graphicalReflectionPool.size() * 50);    // multiply by the probable vertical length

        for (const auto* reflection : graphicalReflectionPool) {
            // no need to skip hidden since the reflection pool already filters them out.

            const std::vector<IVector2>& tmp = reflection->area.getVerticalFaces();

            // blind add
            result.insert(result.end(), tmp.begin(), tmp.end());
        }

        // sort the points
        std::sort(result.begin(), result.end());

        // remove duplicates
        result.erase(
            std::unique(result.begin(), result.end()),
            result.end()
        );

        // account only hits that are inside the dom area
        result.erase(
            std::remove_if(result.begin(), result.end(), [this](const IVector2& point) {
                return graphicalIdentityPool.back().area.hits(point) == false;
            }),
            result.end()
        );

        return result;
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
    bool element::remove(element* handle){
        for (unsigned int i = 0; i < style->Childs.size(); i++){
            if (style->Childs[i] == handle){
                // If the mouse is focused on this about to be deleted element, change mouse position into it's parent Position.
                if (core::focusedOn == style->Childs[i]){
                    currentMouse.position = style->Childs[i]->parent->style->Position.get();
                }

                delete handle;

                flags |= (stain::base(stain::types::DEEP) | stain::types::GRAPHICS);

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
    *       element as dirty with the `stain::types::DEEP` and `stain::types::COLOR` stains.
    *       This ensures that the parent element is re-rendered from scratch when the
    *       rendering thread is updated.
    */
    void element::updateParent(element* New_Element){
        // Normally elements don't do anything
        if (!New_Element->isDisplayed()){
            // Mark the parent element as dirty with the stain::types::DEEP and stain::types::COLOR stains
            fullyStain();
        }

        // When the child is unable to flag changes on parent Render(), like on removal-
        // Then ask the parent to discard the previous buffer and render from scratch.
        if (parent){
            // Mark the parent element as dirty with the stain::types::DEEP and stain::types::COLOR stains
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
    void element::display(bool f){
        // Check if the to be displayed is true and the element wasn't already displayed.
        if (f != Show){
            Show = f;
            
            if (f){
                check(STATE::SHOWN);
            }
            else{
                check(STATE::HIDDEN);
                
                // Ask the parent to flush its buffer from this.
                if (parent){
                    parent->flags |= (stain::types::RESET);
                }
            }

            // now also update all children, this is for the sake of events, since they do not obey AST structure where parental hidden would stop going deeper into AST events are linear list.
            pauseGGUI([this, f](){
                for (element* c : style->Childs){
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
    bool element::isDisplayed() const {
        bool Parent_Exists = parent;
        return (Parent_Exists && parent->isDisplayed()) || !Parent_Exists ? Show : false;
    }

    /**
    * @brief Removes the element at a given index from the list of child elements.
    * @details This function checks if the index is valid (i.e. if the index is within the bounds of the vector of child elements).
    *          If the index is valid, it removes the element at the specified index from the vector of child elements and deletes the element.
    *          If the index is invalid, the function returns false.
    * @param index The index of the element to remove.
    * @return True if the element was successfully removed, false otherwise.
    */
    bool element::remove(size_t index){
        if (index > style->Childs.size() - 1){
            return false;
        }

        element* tmp = style->Childs[index];

        // If the mouse is currently focused on the element that is about to be deleted, change the mouse position into the element's parent position.
        if (core::focusedOn == tmp){
            currentMouse.position = tmp->parent->style->Position.get();
        }

        // Delete the element at the specified index from the vector of child elements.
        delete tmp;

        // Mark the element as dirty, so that it will be re-rendered on the next frame.
        flags |= (stain::base(stain::types::DEEP) | stain::types::GRAPHICS);

        return true;
    }

    /**
    * @brief Removes the element from the parent element.
    * @details This function first checks if the element has a parent.
    *          If the element has a parent, it calls the parent's Remove() function to remove the element from the parent.
    *          If the element does not have a parent, it prints an error message to the console.
    *          The function does not update the frame, so it is the caller's responsibility to update the frame after calling this function.
    */
    void element::remove(){
        if (parent){
            // Tell the parent what is about to happen.
            // You need to update the parent before removing the child, otherwise the code cannot erase it when it is not found!
            parent->remove(this);
        }
        else{
            logger::log(
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
    void element::setDimensions(int width, int height){
        if (width != getWidth() || height != getHeight()){
            setWidth(width);
            setHeight(height);
        }
    }

    /**
    * @brief Set the width of the element.
    * @details This function sets the width of the element to the specified value.
    *          If the width is different from the current width, then the element will be resized and the STRETCH stain is set.
    *          The Update_Frame() function is also called to update the frame.
    * @param width The new width of the element.
    */
    void element::setWidth(int width){
        if (width != getWidth()){
            style->Width.Set(width);
            // Set the STRETCH stain if the width is changed
            flags |= (stain::types::STRETCH);
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
    void element::setHeight(int height){
        if (height != getHeight()){
            style->Height.Set(height);
            // Set the STRETCH stain if the height is changed
            flags |= (stain::types::STRETCH);
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
    void element::setPosition(IVector3 c) {
        // Update the element's position in the style
        style->Position.set(c);
        
        // Mark the element as dirty for movement updates
        this->flags |= (stain::types::MOVE);

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
    void element::setPosition(IVector3* c){
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
    void element::updatePosition(IVector3 v){
        style->Position += v;

        flags |= (stain::types::MOVE);

        updateFrame();
    }

    /**
    * @brief Update the absolute position cache of the element.
    * @details This function updates the cached absolute position of the element by adding the position of the element to the position of its parent.
    */
    void element::updateAbsolutePositionCache(){
        absolutePositionCache = {0, 0, 0};
        int Border_Offset = 0;

        if (parent){
            // Get the position of the parent
            absolutePositionCache = parent->getAbsolutePosition();

            Border_Offset = (parent->hasBorder() != hasBorder() && parent->hasBorder()) ? 1 : 0;

            absolutePositionCache.z += 1;   // mainly used for the compute of rectangle priority
        }

        // Add the position of the element to the position of its parent
        absolutePositionCache += getPosition() + Border_Offset;
    }

    void element::setTitle(const std::string& t){
        style->Title.value = t;
    }

    std::string_view element::getTitle() const {
        // Return the title of the element
        return style->Title.value;
    }

    /**
    * @brief Set the margin of the element.
    * @details This function sets the margin of the element to the specified margin values.
    *          The margin is stored in the element's style.
    * @param margin The new margin values for the element.
    */
    void element::setMargin(margin margin) {
        // Update the element's margin in the style
        style->Margin = margin;
    }

    /**
    * Creates a deep copy of this Element, including all its children.
    * @return A new Element object that is a copy of this one.
    */
    element* element::copy() const {
        // Compile time check
        //static_assert(std::is_same<T&, decltype(*this)>::value, "T must be the same as the type of the object");
        element* new_element = createInstance();

        // Make sure the name is also renewed to represent the memory.
        if (!hasEmptyName())
            new_element->setName(std::string(getNameAsRaw()) + "_copy");
        else 
            new_element->ID = "";

        // Ptr related members:
        // - Parent
        // - Childs
        // - Style
        // - Event Handlers
        // - Focused_On Clearance
        // - Hovered_On Clearance

        // reset the parent info.
        new_element->parent = nullptr;

        // copy the childs over.
        for (unsigned int i = 0; i < this->style->Childs.size(); i++){
            new_element->style->Childs[i] = this->style->Childs[i]->copy();
        }

        // copy the styles over.
        *new_element->style = *this->style;

        // now also update the event handlers.
        // NOTE: We don't have enough power to update the lambda captures of the this ptr value, so please use the self->host ptr instead!
        for (auto& e : this->handlers){
            //add the new action to the event handlers list
            new_element->handlers.push_back(e);
        }

        if (!new_element->handlers.empty()) {
            core::inputConverter->handlers.push_back(new_element);
        }

        // Clear the Focused on bool
        new_element->focused = false;

        // Clear the Hovered on bool
        new_element->hovered = false;

        // Call the potentially un_parsed_styles to clone them too if not initialized yet.
        if (flags.has(stain::types::FINALIZE)){
            new_element->style->copyUnParsedStyles();
        }

        return new_element;
    }

    void element::addEventhandler(const converter::output::event::action& handler) {
        handlers.push_back(handler);

        // Check if this element has been added to the INTERNAL::eventHandlers, if not, then append this into it.
        bool found = false;
        for (const auto& h : core::inputConverter->handlers){
            if (h == this){
                found = true;
                break;
            }
        }

        if (!found) core::inputConverter->handlers.push_back(this);
    }

    void element::embedStyles(){
        style->embedStyles(this);
        
        if (flags.has(stain::types::FINALIZE)){
            flags ^= (stain::types::FINALIZE);

            check(STATE::INIT);
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
    std::pair<int, int> element::getFittingDimensions(element* child) const {
        IVector3 Current_Position = child->getPosition();

        int Result_Width = 0;
        int Result_Height = 0;

        int Border_Offset = hasBorder() != child->hasBorder() && hasBorder() ? 1 * 2 : 0;

        /**
        * If there are only zero child or one and it is same as this child then give max.
        * This is because if there is no other child element, then the fitting dimensions
        * are the same as the parent element minus the border offset.
        */
        if (style->Childs.size() == 0 || style->Childs.back() == child){
            return {getWidth() - Border_Offset, getHeight() - Border_Offset};
        }

        /**
        * Start calculating the fitting dimensions from the top left corner of the child element.
        * This is done by increasing the width and height of the child element until it reaches the edge of the parent element.
        * If the child element collides with another child element, then the fitting dimensions are reduced to the point where the collision is resolved.
        */
        while (true){
            // If the width of the child element is still less than the width of this element minus the border offset
            if (Current_Position.x + (++Result_Width) < getWidth() - Border_Offset){
                // Increase the width of the child element
                Result_Width++;
            }
            else{
                break;
            }
            
            // If the height of the child element is still less than the height of this element minus the border offset
            if (Current_Position.y + (++Result_Height) < getHeight() - Border_Offset){
                // Increase the height of the child element
                Result_Height++;
            }
            else{
                break;
            }
            
            // Check if the child element is colliding with any other child elements.
            for (auto c : style->Childs) {
                // Use local positioning since this is a civil dispute :)
                if (child != c && utils::collides(c->getPosition(), Current_Position, c->getWidth(), c->getHeight(), Result_Width, Result_Height)) {
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
    * @return IVector3 The final size limit of the element.
    */
    IVector3 element::getFinalLimit() const{
        if (isOverflowAllowed()){
            return {INT16_MAX, INT16_MAX};
        }

        IVector3 End_Address = IVector3(getWidth(), getHeight());

        // We can check if the parent allows some flexibility.
        if (parent && isDynamicSizeAllowed()){
            End_Address = parent->getFinalLimit();
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
    void element::setBackgroundColor(RGB color) {
        // Set the background color in the style
        style->Background_Color = color;
        
        // If the border background color matches the current background color, update it
        if (style->Border_Background_Color.color.get<RGB>() == style->Background_Color.color.get<RGB>()) {
            style->Border_Background_Color = color;
        }
        
        // Mark the element as dirty for color updates
        flags |= (stain::types::GRAPHICS);
        
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
    void element::setBorderColor(RGB color){
        // Set the border color in the style
        style->Border_Color = color;
        
        // Mark the element as dirty for color updates
        flags |= (stain::types::GRAPHICS);
        
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
    void element::setBorderBackgroundColor(RGB color) {
        // Set the border background color in the style
        style->Border_Background_Color = color;
        
        // Mark the element as dirty for color updates
        flags |= (stain::types::GRAPHICS);
        
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
    void element::setTextColor(RGB color){
        style->Text_Color = color;
        // Mark the element as dirty for color updates
        flags |= (stain::types::GRAPHICS);
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
    void element::allowDynamicSize(bool True) {
        // Since dynamic size and percentage based size are two incompatible systems.
        if (style->Width.number.getType() != types::EVALUATION_TYPE::PERCENTAGE && style->Height.number.getType() != types::EVALUATION_TYPE::PERCENTAGE){
            style->Allow_Dynamic_Size = True; 
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
    void element::allowOverflow(bool True) {
        // Update the Allow_Overflow property in the element's style
        style->Allow_Overflow = True; 
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
    void element::setHoverBorderColor(RGB color){
        style->Hover_Border_Color = color;
        // Mark the element as dirty for color updates
        flags |= (stain::types::GRAPHICS);
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
    void element::setHoverBackgroundColor(RGB color) {
        // Set the hover background color in the style
        style->Hover_Background_Color = color;
        
        // Mark the element as dirty for color updates
        flags |= (stain::types::GRAPHICS);
        
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
    void element::setHoverTextColor(RGB color) {
        // Set the hover text color in the style
        style->Hover_Text_Color = color;
        
        // Mark the element as dirty for color updates
        flags |= (stain::types::GRAPHICS);
        
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
    void element::setHoverBorderBackgroundColor(RGB color) {
        // Set the hover border background color in the style
        style->Hover_Border_Background_Color = color;
        
        // Mark the element as dirty for color updates
        flags |= (stain::types::GRAPHICS);
        
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
    void element::setFocusBorderColor(RGB color){
        style->Focus_Border_Color = color;
        
        // Mark the element as dirty for color updates
        flags |= (stain::types::GRAPHICS);
        
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
    void element::setFocusBackgroundColor(RGB color){
        // Set the focus background color in the style
        style->Focus_Background_Color = color;
        
        // Mark the element as dirty for color updates
        flags |= (stain::types::GRAPHICS);
        
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
    void element::setFocusTextColor(RGB color){
        style->Focus_Text_Color = color;
        // Mark the element as dirty for color updates
        flags |= (stain::types::GRAPHICS);
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
    void element::setFocusBorderBackgroundColor(RGB color){
        style->Focus_Border_Background_Color = color;
        // Mark the element as dirty for color updates
        flags |= (stain::types::GRAPHICS);
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
    void element::setAnchor(ANCHOR Anchor){
        // Set the alignment in the style
        style->Align = Anchor;
    }

    /**
    * @brief Sets the flow priority of the element.
    * 
    * This function sets the flow priority of the element to the specified DIRECTION value.
    * The flow priority determines how the element will be aligned in its parent when the parent is a flow layout.
    * 
    * @param Priority The flow priority value to set for the element.
    */
    void element::setFlowPriority(DIRECTION Priority){
        style->Flow_Priority = Priority;
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
    void element::setWrap(bool Wrap){
        style->Wrap = Wrap;
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
    void element::computeDynamicSize(){
        // Go through all elements displayed.
        if (!isDisplayed())
            return;

        if (!isDynamicSizeAllowed())
            return;

        if (childrenChanged()){
            // Iterate through all the elements that are being displayed.
            for (auto c : style->Childs){
                if (!c->isDisplayed())
                    continue;

                // Check the child first if it has to stretch before this can even know if it needs to stretch.
                c->computeDynamicSize();

                int Border_Offset = hasBorder() != c->hasBorder() && hasBorder() ? 1 * 2 : 0;

                // If the width is an percentage value, then it is always smaller or equal to this's width.
                int Enable_Width_Modification = (c->getWidthType() != types::EVALUATION_TYPE::PERCENTAGE && getWidthType() != types::EVALUATION_TYPE::PERCENTAGE) ? 1 : 0;  // Enable checking width if the width attribute type is an relative one
                // Do the same for the Height attribute
                int Enable_Height_Modification = (c->getHeightType() != types::EVALUATION_TYPE::PERCENTAGE && getHeightType() != types::EVALUATION_TYPE::PERCENTAGE) ? 1 : 0; // Enable checking height if the height attribute type is an relative one

                // Add the border offset to the width and the height to count for the border collision and evade it. 
                int New_Width = std::max(
                    (c->style->Position.get().x + c->getWidth() + Border_Offset) * Enable_Width_Modification,
                    getWidth()
                );

                int New_Height = std::max(
                    (c->style->Position.get().y + c->getHeight() + Border_Offset) * Enable_Height_Modification,
                    getHeight()
                );

                // but only update those who actually allow dynamic sizing.
                if (New_Width != getWidth() || New_Height != getHeight()){
                    setHeight(New_Height);
                    setWidth(New_Width);
                    flags |= (stain::types::STRETCH);
                }
            }
        }

        return;
    }

    void element::compileActiveGraphics(){
        style->compile(this);

        std::transform(
            graphicalIdentityPool.begin(), graphicalIdentityPool.end(),
            std::back_inserter(graphicalReflectionPool),
            [](activeStyle& as) { return &as; }
        );
    }

    /**
    * @brief Add the border of the window to the rendered string.
    *
    * @param Result The string to add the border to.
    */
    void element::renderBorders(std::vector<terminal::cell>& Result){
        if (!hasBorder()) return;

        unsigned int Width  = getWidth();
        unsigned int Height = getHeight();
        const auto& Border        = style->Border_Style;

        // Corners
        Result[0] = Border.topLeftCorner;
        Result[Width - 1] = Border.topRightCorner;
        Result[(Height - 1) * Width] = Border.bottomLeftCorner;
        Result[(Height * Width) - 1] = Border.bottomRightCorner;

        // Top and Bottom horizontal borders
        for (unsigned int x = 1; x < Width - 1; ++x) {
            Result[x] = Border.horizontalLine;                          // Top row
            Result[(Height - 1) * Width + x] = Border.horizontalLine;   // Bottom row
        }

        // Left and Right vertical borders
        for (unsigned int y = 1; y < Height - 1; ++y) {
            Result[y * Width] = Border.verticalLine;            // Left column
            Result[y * Width + (Width - 1)] = Border.verticalLine; // Right column
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
    * - The title's length is determined by the `Style->Title.value.size`.
    * - The function calculates the writable length based on the element's width,
    *   taking into account borders and the space required for the ellipsis.
    * - If the title exceeds the writable length, an ellipsis is appended to indicate truncation.
    * - The function ensures that no characters are written beyond the available width.
    * 
    * @note The function assumes that the `Result` vector is pre-allocated and large enough
    *       to hold the rendered title and ellipsis.
    */
    void element::renderTitle(std::vector<terminal::cell>& Result){
        if (style->Title.empty())
            return;

        size_t Title_Length = style->Title.value.size(); // +1 for trailing, since Compact_Strings do not include trailing characters in their size.
        size_t Horizontal_Offset = (int)hasBorder();
        static constexpr std::string Ellipsis = "...";
        bool Enable_Ellipsis = false;

        size_t Writable_Length = std::min(Title_Length, (size_t)(getWidth() - Horizontal_Offset - (int)Ellipsis.size() - 1));

        if (Writable_Length < Title_Length)
            Enable_Ellipsis = true;

        // Now we'll write what we can
        for (size_t x = Horizontal_Offset; x < Writable_Length + Horizontal_Offset; x++){
            Result[x] = style->Title.value[x - Horizontal_Offset];
        }

        // And then we'll add the ellipsis
        if (Enable_Ellipsis){
            size_t Ellipsis_Offset = Writable_Length + Horizontal_Offset;
            for (size_t x = 0; x < Ellipsis.size(); x++){
                if (Ellipsis_Offset + x < (size_t)(getWidth() - Horizontal_Offset)){
                    Result[Ellipsis_Offset + x] = Ellipsis[x];
                }
            }
        }
    }

    inline bool Is_In_Bounds(IVector3 index, element* parent){
        // checks if the index is out of bounds
        if (index.x < 0 || index.y < 0 || index.x >= parent->getWidth() || index.y >= parent->getHeight())
            return false;

        return true;
    }

    inline terminal::cell* From(IVector3 index, std::vector<terminal::cell>& Parent_Buffer, element* Parent){
        return &Parent_Buffer[index.y * Parent->getWidth() + index.x];
    }

    /**
    * @brief Sets the custom border style for the element.
    * @details This function sets the custom border style for the element, marks the element's edges as dirty, and ensures that the border is visible.
    * @param style The custom border style to set.
    */
    void element::setCustomBorderStyle(styledBorder Style) {
        // Set the border style of the element
        style->Border_Style = Style;
        
        // Mark the border as needing an update
        flags |= (stain::types::EDGE);

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
    void element::postProcessBorders(element* A, element* B, std::vector<terminal::cell>& Parent_Buffer){
        // We only need to calculate the childs points in which they intersect with the parent borders.
        // At these intersecting points of border we will construct a bit mask that portraits the connections the middle point has.
        // With the calculated bit mask we can fetch from the 'SYMBOLS::Border_Identifiers' the right border string.

        // First calculate if the childs borders even touch the parents borders.
        // If not, there is no need to calculate anything.

        // First calculate if the child is outside the parent.
        if (
            B->style->Position.get().x + B->getWidth() < A->style->Position.get().x ||
            B->style->Position.get().x > A->style->Position.get().x + A->getWidth() ||
            B->style->Position.get().y + B->getHeight() < A->style->Position.get().y ||
            B->style->Position.get().y > A->style->Position.get().y + A->getHeight()
        )
            return;

        // Now calculate if the child is inside the parent.
        if (
            B->style->Position.get().x > A->style->Position.get().x &&
            B->style->Position.get().x + B->getWidth() < A->style->Position.get().x + A->getWidth() &&
            B->style->Position.get().y > A->style->Position.get().y &&
            B->style->Position.get().y + B->getHeight() < A->style->Position.get().y + A->getHeight()
        )
            return;

        // Now that we are here it means the both boxes interlace each other.
        // We will calculate the hitting points by drawing segments from corner to corner and then comparing one segments x to other segments y, and so forth.

        // two nested loops rotating the x and y usages.
        // store the line x,y into a array for the nested loops to access.
        std::vector<int> Vertical_Line_X_Coordinates = {
            
            B->style->Position.get().x,
            A->style->Position.get().x,
            B->style->Position.get().x + B->getWidth() - 1,
            A->style->Position.get().x + A->getWidth() - 1,

                    
            // A->Style->Position.Get().X,
            // B->Style->Position.Get().X,
            // A->Style->Position.Get().X + A->Width - 1,
            // B->Style->Position.Get().X + B->Width - 1

        };

        std::vector<int> Horizontal_Line_Y_Coordinates = {
            
            A->style->Position.get().y,
            B->style->Position.get().y + B->getHeight() - 1,
            A->style->Position.get().y,
            B->style->Position.get().y + B->getHeight() - 1,

            // B->Position.Y,
            // A->Position.Y + A->Height - 1,
            // B->Position.Y,
            // A->Position.Y + A->Height - 1,

        };

        std::vector<IVector3> Crossing_Indicies;

        // Go through singular box
        for (size_t Box_Index = 0; Box_Index < Horizontal_Line_Y_Coordinates.size(); Box_Index++){
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

            IVector3 Above = { c.x, c.y - 1 };
            IVector3 Below = { c.x, c.y + 1 };
            IVector3 Left = { c.x - 1, c.y };
            IVector3 Right = { c.x + 1, c.y };

            bitMask<styledBorder::connectionTypes> Current_Masks = styledBorder::connectionTypes::NONE;

            // These selected coordinates can only contain something related to the borders and if the current UTF is unicode then it is an border.
            if (Is_In_Bounds(Above, this)){
                // Since the border above can be already processed we need to check all possible border variations.
                bitMask<styledBorder::connectionTypes> borderAbove = A->getCustomBorderStyle().getBorderType(From(Above, Parent_Buffer, this)->getGlyphs()) | 
                                                         B->getCustomBorderStyle().getBorderType(From(Above, Parent_Buffer, this)->getGlyphs());
                
                if (borderAbove != styledBorder::connectionTypes::NONE)
                    Current_Masks |= styledBorder::connectionTypes::UP;
            }

            if (Is_In_Bounds(Below, this)){
                // Since the border below can be already processed we need to check all possible border variations.
                bitMask<styledBorder::connectionTypes> borderBelow = A->getCustomBorderStyle().getBorderType(From(Below, Parent_Buffer, this)->getGlyphs()) | 
                                                         B->getCustomBorderStyle().getBorderType(From(Below, Parent_Buffer, this)->getGlyphs());
                
                if (borderBelow != styledBorder::connectionTypes::NONE)
                    Current_Masks |= styledBorder::connectionTypes::DOWN;
            }

            if (Is_In_Bounds(Left, this)){
                // Since the border left can be already processed we need to check all possible border variations.
                bitMask<styledBorder::connectionTypes> borderLeft = A->getCustomBorderStyle().getBorderType(From(Left, Parent_Buffer, this)->getGlyphs()) | 
                                                        B->getCustomBorderStyle().getBorderType(From(Left, Parent_Buffer, this)->getGlyphs());
                
                if (borderLeft != styledBorder::connectionTypes::NONE)
                    Current_Masks |= styledBorder::connectionTypes::LEFT;
            }

            if (Is_In_Bounds(Right, this)){
                // Since the border right can be already processed we need to check all possible border variations.
                bitMask<styledBorder::connectionTypes> borderRight = A->getCustomBorderStyle().getBorderType(From(Right, Parent_Buffer, this)->getGlyphs()) | 
                                                         B->getCustomBorderStyle().getBorderType(From(Right, Parent_Buffer, this)->getGlyphs());
                
                if (borderRight != styledBorder::connectionTypes::NONE)
                    Current_Masks |= styledBorder::connectionTypes::RIGHT;
            }

            std::string_view finalBorder = A->getBorderStyle().getBorder(Current_Masks);

            if (finalBorder.empty()){
                continue;
            }

            *From(c, Parent_Buffer, this) = finalBorder;
        }
    }

    //End of utility functions.

    /**
    * @brief A function that registers a lambda to be executed when the element is clicked.
    * @details The lambda is given a pointer to the Event object that triggered the call.
    *          The lambda is expected to return true if it was successful and false if it failed.
    * @param action The lambda to be called when the element is clicked.
    */
    void element::onClick(std::function<bool(converter::output::event::base*)> job){
        auto wrapper = [this, job](converter::output::event::base* e){
            // As os 0.1.8 no need to check for mouse collision with current element, since mouse collision is already checked at the eventHandler scheduler.

            // Construct an Action from the Event obj
            auto* event2actionWrapper = new converter::output::event::action(e->criteria, job, getName() + "::onClick");

            //action successfully executed.
            return job(event2actionWrapper);
        };
        
        auto mouse = converter::output::event::action(
            {converter::input::key::types::LEFT_CLICK},
            wrapper,
            getName() + "::onClick::wrapper::mouse"
        );

        auto enter = converter::output::event::action(
            {converter::input::key::types::ENTER},
            wrapper,
            getName() + "::onClick::wrapper::enter"
        );

        addEventhandler(mouse);
        addEventhandler(enter);
    }

    /**
    * @brief A function that registers a lambda to be executed when the element is interacted with in any way.
    * @details The lambda is given a pointer to the Event object that triggered the call.
    *          The lambda is expected to return true if it was successful and false if it failed.
    * @param criteria The criteria to check for when deciding whether to execute the lambda.
    * @param action The lambda to be called when the element is interacted with.
    * @param GLOBAL Whether the lambda should be executed even if the element is not under the mouse.
    */
    void element::on(std::initializer_list<converter::input::key::types> criteria, std::function<bool(converter::output::event::base*)> job, bool GLOBAL){
        addEventhandler(converter::output::event::action(
            criteria,
            [this, job, GLOBAL](converter::output::event::base* e){
                if (this->isFocused() || GLOBAL){
                    // action successfully executed.
                    return job(e);
                }
                // action failed.
                return false;
            },
            getName() + "::on::"
        ));
    }

    /**
    * @brief Check if any children have changed.
    * @details This function will check if any of the children have changed, this is used to determine if the element needs to be re-drawn.
    * @return true if any children have changed, false otherwise.
    */
    bool element::childrenChanged() const {
        for (const auto* e : style->Childs){
            if (e->getDirty().is(stain::types::FINALIZE)){
                logger::log("Child element passthrough Finalization stage!");
            }

            // Not counting State machine, if element is not being drawn return always false.
            if (!e->Show)
                return false;

            if (!e->getDirty().isEmpty())
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
    bool element::hasTransparentChildren() const {
        // If the element is not visible, return false.
        if (!Show)
            return false;

        // Recursively check each child element for transparency.
        for (const auto* e : style->Childs) {
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
    * @return A std::string_view containing either the element's name or its memory address.
    */
    std::string_view element::getNameAsRaw() const {
        return ID;
    }

    /**
    * @brief Checks if the element's name is empty.
    * 
    * @return true if the name of the element has no characters, false otherwise.
    */
    bool element::hasEmptyName() const {
        return ID.empty();
    }

    /**
    * @brief Set the name of the element.
    * @details This function sets the name of the element and stores it in the global Element_Names map.
    * @param name The name of the element.
    */
    void element::setName(const std::string& Name){
        // Set the name of the element.
        ID = Name;

        // Store the element in the global Element_Names map.
        core::elementNames[ID] = this;
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
    element* element::getElement(std::string_view Name) {
        for (auto* c : getChilds()){
            if (c->getNameAsRaw() == Name)
                return c;

            element* tmp = c->getElement(Name);

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
    std::vector<element*> element::getAllNestedElements(bool Show_Hidden) {
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
    void element::reOrderChilds() {
        // Sort the child elements using a lambda function to compare the z-coordinates.
        std::sort(style->Childs.begin(), style->Childs.end(), [](element* a, element* b) {
            // Compare the z-position of the two elements.
            return a->getPosition().z < b->getPosition().z;
        });
    }

    /**
    * @brief Focuses the element.
    * @details This function updates the global focus information by setting the mouse position to the element's position and updating the focused element.
    */
    void element::focus() {
        // Set the mouse position to the element's position.
        currentMouse.position = this->style->Position.get();
        // Update the focused element.
        core::updateFocusedElement(this);
    }

    /**
    * @brief Adds a handler function to the state handlers map.
    * @details This function takes a state and a handler function as arguments.
    *          The handler function is stored in the State_Handlers map with the given state as the key.
    * @param s The state for which the handler should be executed.
    * @param job The handler function to be executed
    */
    void element::onState(STATE s, void (*job)(element* self)){
        if (s == STATE::INIT)
            onInit = job;
        else if (s == STATE::DESTROYED)
            onDestroy = job;
        else if (s == STATE::HIDDEN)
            onHide = job;
        else if (s == STATE::SHOWN)
            onShow = job;
    }

    bool Is_Signed(int x){
        return x < 0;
    }

    int Get_Sign(int x){
        return Is_Signed(x) ? -1 : 1;
    }

    // Constructs two squares one 2 steps larger on width and height, and given the different indicies.
    std::vector<IVector3> Get_Surrounding_Indicies(int Width, int Height, IVector3 start_offset, FVector2 Offset){

        std::vector<IVector3> Result;

        // First construct the first square.
        int Bigger_Square_Start_X = start_offset.x - 1;
        int Bigger_Square_Start_Y = start_offset.y - 1;

        int Bigger_Square_End_X = start_offset.x + Width + 1;
        int Bigger_Square_End_Y = start_offset.y + Height + 1;

        int Smaller_Square_Start_X = start_offset.x + (Offset.x * std::min(0, (int)Offset.x));
        int Smaller_Square_Start_Y = start_offset.y + (Offset.y * std::min(0, (int)Offset.y));

        int Smaller_Square_End_X = start_offset.x + Width - (Offset.x * std::max(0, (int)Offset.x));
        int Smaller_Square_End_Y = start_offset.y + Height - (Offset.y * std::max(0, (int)Offset.y));

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
    bool element::childIsShown(element* other){
        rectangle self = {
            getPosition(),
            { getWidth(), getHeight() }
        };

        rectangle child = {
            other->getPosition(),
            { other->getWidth(), other->getHeight() }
        };

        rectangle result = self.intersection(child);

        if (result.empty()) return false;
        else return true;
    }
}