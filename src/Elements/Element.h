#ifndef _ELEMENT_H_
#define _ELEMENT_H_

#include <string>
#include <sstream>
#include <cstring>
#include <vector>
#include <unordered_map>
#include <functional>
#include <chrono>
#include <atomic>
#include <limits>

#include <iostream>

#include "../core/utils/superString.h"
#include "../core/utils/constants.h"
#include "../core/utils/color.h"
#include "../core/utils/units.h"
#include "../core/utils/event.h"
#include "../core/utils/style.h"

namespace GGUI{
    class element{
    protected:
        // Shadows and some other postprocessing effects can have an impact on the final width and height of the element.
        unsigned int Post_Process_Width = 0;
        unsigned int Post_Process_Height = 0;

        // Only fetch one parent UP, and own position +, then child repeat in Render pipeline.
        IVector3 Absolute_Position_Cache;

        class element* Parent = nullptr;

        // Determines if the element is rendered or not.
        bool Show = true;
        
        std::vector<UTF> Render_Buffer;
        std::vector<UTF> Post_Process_Buffer;

        // State machine for render pipeline only focus on changed aspects.
        STAIN Dirty;
        
        // For styling to have shared styles.
        std::vector<int> Classes;

        bool Focused = false;
        bool Hovered = false;

        // Human readable ID.
        std::string Name = "";

        // For long term support made this a pointer to avoid size mismatch.
        styling* Style = nullptr;

        void (*On_Init)(element*) = nullptr;
        void (*On_Destroy)(element*) = nullptr;
        void (*On_Hide)(element*) = nullptr;
        void (*On_Show)(element*) = nullptr;
    public:

        /**
         * @brief The constructor for the Element class that accepts a Styling object.
         *
         * This constructor is used when an Element is created without a parent.
         * In this case, the Element is created as a root object, and it will be
         * automatically added to the list of root objects.
         *
         * @param s The Styling object to use for the Element.
         * @param Embed_Styles_On_Construct A flag indicating whether to embed the styles on construction. Only use if you know what you're doing!!!
         */
        element(styling s = STYLES::CONSTANTS::Default, bool Embed_Styles_On_Construct = false);

        /**
         * @brief For correctly copying data between elements, try the Copy() function.
         * Copying is removed, so that Slicing doesn't happen for the VTable
         */
        element(const element&) = delete;
        element& operator=(const GGUI::element&) = delete;

        element& operator=(element&&) = default;
        element(element&&) = default;

        /**
         * @brief The destructor for the Element class.
         *
         * This destructor is responsible for properly deallocating all the memory
         * allocated by the Element object.
         *
         * @note This destructor is also responsible for cleaning up the parent
         * element's vector of child elements and the event handlers list.
         */
        virtual ~element();

        /**
         * @brief Safely moves the current Element object to a new memory location.
         * 
         * This function creates a new Element object, copies the contents of the 
         * current Element object to the new object, and returns a pointer to the 
         * newly created object.
         * 
         * @return Element* Pointer to the newly created Element object.
         */
        virtual element* safeMove(){
            return new element();
        }

        /**
         * @brief Creates a deep copy of this Element, including all its children.
         * 
         * @return A new Element object that is a copy of this one.
         */
        element* copy();

        /**
         * @brief Embeds styles into the current element and its child elements.
         * 
         * This function calls the Embed_Styles method on the current element's style,
         * passing the current element as a parameter. It then recursively calls the 
         * Embed_Styles method on each child element's style.
         */
        void embedStyles();

        void addStyling(styling& s){
            Style->Copy(s);
        }

        /** 
         * @brief Marks the Element as fully dirty by setting all stain types.
         * 
         * This function sets each stain type on the Dirty object, indicating
         * that the Element needs to be reprocessed for all attributes.
         */
        void fullyStain();

        /**
         * @brief Accumulates all the classes and their styles.
         * @details This method accumulates all the classes and their styles to the
         *          current element.
         */
        void parseClasses();

        /**
         * @brief Returns the Dirty object for the Element.
         * @details This function returns the Dirty object, which is a bitfield
         *          that keeps track of what needs to be reprocessed on the
         *          Element when it is asked to render.
         * @return A reference to the Dirty object.
         */
        STAIN& getDirty(){
            return Dirty;
        }


        /**
         * @brief Returns true if the element is currently focused.
         * @return A boolean indicating whether the element is focused.
         */
        bool isFocused() const{
            return Focused;
        }

        /**
         * @brief Sets the focus state of the element.
         * @details Sets the focus state of the element to the given value.
         *          If the focus state changes, the element will be dirtied and the frame will be updated.
         * @param f The new focus state.
         */
        void setFocus(bool f);

        /**
         * @brief Returns true if the element is currently hovered.
         * @return A boolean indicating whether the element is hovered.
         */
        bool isHovered() const{
            return Hovered;
        }

        /**
         * @brief Sets the hover state of the element.
         * @details Sets the hover state of the element to the given value.
         *          If the hover state changes, the element will be dirtied and the frame will be updated.
         * @param h The new hover state.
         */
        void setHoverState(bool h);

        /**
         * @brief Executes the handler function associated with a given state.
         * @details This function checks if there is a registered handler for the specified state.
         *          If a handler exists, it invokes the handler function.
         * @param s The state for which the handler should be executed.
         */
        inline void check(STATE s){
            if (s == STATE::INIT && On_Init){
                // Since the rendering hasn't yet started and the function here may be reliant on some relative information, we need to evaluate the the dynamic values.
                Style->Evaluate_Dynamic_Attribute_Values(this);

                On_Init(this);
            }
            else if (s == STATE::DESTROYED && On_Destroy)
                On_Destroy(this);
            else if (s == STATE::HIDDEN && On_Hide)
                On_Hide(this);
            else if (s == STATE::SHOWN && On_Show)
                On_Show(this);
        }

        /**
         * @brief Retrieves the styling information of the element.
         * @details This function returns the current styling object associated with the element.
         *          The styling object contains various style attributes such as colors, borders, etc.
         * @return The styling object of the element.
         */
        styling getStyle() const;

        styling* getDirectStyle();

        /**
         * @brief Sets the styling information of the element.
         * @details This function sets the styling information of the element to the given value.
         *          If the element already has a styling object associated with it, the function will
         *          copy the given styling information to the existing object. Otherwise, the function
         *          will create a new styling object and associate it with the element.
         * @param css The new styling information to associate with the element.
         */
        void setStyle(styling css);

        /**
         * @brief Calculates the hitboxes of all child elements of the element.
         * @details This function calculates the hitboxes of all child elements of the element.
         *          The hitbox of a child element is the area of the element that is actually visible
         *          on the screen. The function takes the starting offset into the child array as an
         *          argument. If no argument is provided, the function starts at the beginning of the
         *          child array.
         * @param Starting_Offset The starting offset into the child array. If no argument is provided,
         *                         the function starts at the beginning of the child array.
         */
        virtual void calculateChildsHitboxes([[maybe_unused]] unsigned int Starting_Offset = 0) {}

        /**
         * @brief Adds a class to the element.
         * @details This function adds the given class to the element's class list.
         *          If the class does not exist in the global class map, a new ID is assigned to the class.
         *          The element is then marked as dirty, which will trigger a re-render of the element.
         * @param class_name The name of the class to add.
         */
        void addClass(std::string class_name);

        /**
         * @brief Sets the opacity of the element.
         * @details This function takes a float value between 0.0f and 1.0f and sets the
         * opacity of the element to that value. If the value is greater than 1.0f, the
         * function will report an error and do nothing.
         * @param[in] Opacity The opacity value to set.
         */
        void setOpacity(float Opacity);
        
        /**
         * @brief Sets the opacity of the element using an integer percentage.
         * @details This function takes an unsigned integer value between 0 and 100, representing the opacity percentage,
         * and sets the element's opacity. If the value is greater than 100, it will report an error and do nothing.
         * @param[in] Opacity The opacity percentage to set.
         */
        void setOpacity(unsigned int Opacity);

        /**
         * @brief Gets the current border style of the element.
         * @details This function returns the current border style of the element.
         *          The border style is a structure that contains the strings for
         *          the top left corner, top right corner, bottom left corner, bottom
         *          right corner, horizontal line, vertical line, vertical right connector,
         *          vertical left connector, horizontal bottom connector, horizontal top
         *          connector, and cross connector.
         * @return The current border style of the element.
         */
        styled_border getBorderStyle() const {
            return Style->Border_Style;
        }

        /**
         * @brief Gets the opacity of the element.
         * @details This function returns the current opacity of the element as a float value.
         *          The opacity is a value between 0.0 and 1.0, where 0.0 is fully transparent
         *          and 1.0 is fully opaque.
         * @return The current opacity of the element.
         */
        float getOpacity() const; 

        /**
         * @brief Checks if the element is transparent.
         * @details This function determines whether the element is transparent by checking
         *          if the element's opacity is not equal to 1.0f. An opacity less than 1.0f
         *          indicates that the element is partially or fully transparent.
         * @return True if the element is transparent; otherwise, false.
         */
        bool isTransparent() const;

        /**
         * @brief Gets the processed width of the element.
         * @details This function returns the width of the element after any post-processing
         *          has been applied. If the element has not been post-processed, the
         *          original width of the element is returned.
         * @return The processed width of the element.
         */
        unsigned int getProcessedWidth();

        /**
         * @brief Gets the processed height of the element.
         * @details This function returns the height of the element after any post-processing
         *          has been applied. If the element has not been post-processed, the
         *          original height of the element is returned.
         * @return The processed height of the element.
         */
        unsigned int getProcessedHeight();

        /**
         * @brief Configures and displays the shadow for the element.
         * @details This function sets the shadow properties such as direction, color, opacity, 
         *          and length, and applies the shadow effect to the element. It adjusts the 
         *          element's position to account for the shadow and marks the element as dirty 
         *          for a visual update.
         * @param Direction The direction vector of the shadow.
         * @param Shadow_Color The color of the shadow.
         * @param Opacity The opacity of the shadow, between 0.0f (fully transparent) and 1.0f (fully opaque).
         * @param Length The length of the shadow.
         */
        void showShadow(FVector2 Direction, RGB Shadow_Color, float Opacity = 1, float Length = 0.5);

        /**
         * @brief Displays the shadow for the element.
         * @details This function sets the shadow properties such as direction, color, opacity, and length, and applies the shadow effect to the element. It adjusts the element's position to account for the shadow and marks the element as dirty for a visual update. The direction vector of the shadow is set to (0, 0) by default, which means the shadow will appear directly below the element.
         * @param Shadow_Color The color of the shadow.
         * @param Opacity The opacity of the shadow, between 0.0f (fully transparent) and 1.0f (fully opaque).
         * @param Length The length of the shadow.
         */
        void showShadow(RGB Shadow_Color, float Opacity = 1, float Length = 0.5);

        /**
         * @brief Sets the shadow properties for the element.
         * @details This function sets the shadow properties such as direction, color, opacity, and length, and applies the shadow effect to the element. It also marks the element as dirty for a visual update.
         * @param s The shadow properties to set.
         */
        void setShadow(shadow s);

        /**
         * @brief Retrieves the parent element.
         * @details This function returns the parent element of the current element.
         *          If the element has no parent, it will return nullptr.
         * @return A pointer to the parent element.
         */
        element* getParent() const{
            return Parent;
        }

        /**
         * @brief Sets the parent of this element.
         * @details This function sets the parent of this element to the given element.
         *          If the given element is nullptr, it will clear the parent of this
         *          element.
         * @param parent The parent element to set.
         */
        void setParent(element* parent);

        /**
         * @brief Checks if the element has the given class.
         * @details This function takes a class name and checks if the element has the class in its class list.
         *          If the class does not exist in the global class map, the function will return false.
         *          If the class exists, the function will return true if the element has the class in its list.
         * @param s The name of the class to check.
         * @return True if the element has the class, false otherwise.
         */
        bool has(std::string s) const;

        /**
         * @brief Checks if the element has the given class ID.
         * @details This function takes a class ID and checks if the element has the class in its class list.
         *          If the class does not exist in the global class map, the function will return false.
         *          If the class exists, the function will return true if the element has the class in its list.
         * @param s The ID of the class to check.
         * @return True if the element has the class, false otherwise.
         */
        bool has(int s) const{
            // Iterate through the class list of the element
            for (auto i : Classes){
                // If the class ID matches the given ID, return true
                if (i == s)
                    return true;
            }
            // If no match is found, return false
            return false;
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
        std::pair<unsigned int, unsigned int> getFittingDimensions(element* child);

        /**
         * @brief Returns the maximum dimensions of the element without exceeding the parent element's dimensions.
         * @return A pair containing the maximum width and height of the element.
         */
        std::pair<unsigned int, unsigned int> getLimitDimensions();

        /**
         * @brief Sets the border visibility of the element.
         * @details This function takes a boolean as a parameter and sets the border visibility of the element accordingly.
         *          If the new state is different from the current state, the element will be marked as dirty with the EDGE stain.
         * @param b The new state of the border visibility.
         */
        virtual void showBorder(bool b);

        /**
         * @brief Sets the border visibility of the element.
         * @details This function toggles the border visibility based on the provided state.
         *          If the state has changed, it updates the border visibility, marks the 
         *          element as dirty with the EDGE stain, and updates the frame.
         * @param b The desired state of the border visibility.
         * @param Previous_State The current state of the border visibility.
         */
        virtual void showBorder(bool b, bool Previous_state);

        /**
         * @brief Checks if the element has a border.
         * @details This function checks if the element has a border.
         *          It returns true if the element has a border, false otherwise.
         * @return True if the element has a border, false otherwise.
         */
        bool hasBorder();

        /**
         * @brief Displays or hides the element and all its children.
         * @details This function changes the display status of the element and all its children.
         *          If the element is displayed, all its children are also displayed. If the element is hidden,
         *          all its children are also hidden.
         * @param f A boolean indicating whether to display (true) or hide (false) the element and its children.
         */
        void display(bool f);

        /**
         * @brief Returns whether the element is currently displayed.
         * @details This function is used to check whether the element is currently displayed or hidden.
         *          It returns true if the element is displayed and false if the element is hidden.
         * @return A boolean indicating whether the element is displayed (true) or hidden (false).
         */
        bool isDisplayed();

        /**
         * @brief Adds a child element to the element.
         * @details This function adds a child element to the element. If the element has a border, it will
         *          be taken into account when calculating the size of the parent element. If the child element
         *          exceeds the size of the parent element, the parent element will be resized to fit the child
         *          element. If the parent element is not allowed to resize, the child element will be resized to
         *          fit the parent element.
         * @param Child The child element to add.
         */
        virtual void addChild(element* Child);

        /**
         * @brief Adds a vector of child elements to the current element.
         * @param childs The vector of child elements to add.
         *
         * This function adds all the child elements to the current element by calling the Add_Child function for each element in the vector.
         * It also marks the current element as dirty with the DEEP stain after adding all the elements.
         */
        virtual void setChilds(std::vector<element*> childs);

        /**
         * @brief Check if any children have changed.
         * @details This function will check if any of the children have changed, this is used to determine if the element needs to be re-drawn.
         * @return true if any children have changed, false otherwise.
         */
        bool childrenChanged();
        
        /**
         * @brief Check if there are any transparent children.
         * @details This function determines if the current element or any of its children
         *          are transparent and require redrawing.
         * @return True if any child is transparent and not clean; otherwise, false.
         */
        bool hasTransparentChildren();    

        /**
         * @brief Retrieves the list of child elements.
         * @details This function returns a reference to the vector containing all child elements
         *          associated with the current element's style.
         * @return A reference to the vector of child elements.
         */
        virtual std::vector<element*>& getChilds();

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
        virtual bool remove(element* handle);

        /**
         * @brief Removes the element at a given index from the list of child elements.
         * @details This function checks if the index is valid (i.e. if the index is within the bounds of the vector of child elements).
         *          If the index is valid, it removes the element at the specified index from the vector of child elements and deletes the element.
         *          If the index is invalid, the function returns false.
         * @param index The index of the element to remove.
         * @return True if the element was successfully removed, false otherwise.
         */
        virtual bool remove(unsigned int index);

        /**
         * @brief Set the width and height of the element.
         * @details This function sets the width and height of the element to the specified values.
         *          If the width or height is different from the current width or height, then the element will be resized and the STRETCH stain is set.
         *          The Update_Frame() function is also called to update the frame.
         * @param width The new width of the element.
         * @param height The new height of the element.
         */
        void setDimensions(unsigned int width, unsigned int height);

        /**
         * @brief Get the width of the element.
         * @details This function returns the width of the element.
         * @return The width of the element.
         */
        constexpr unsigned int getWidth(){ return Style->Width.Get(); }

        /**
         * @brief Get the height of the element.
         * @details This function returns the height of the element.
         * @return The height of the element.
         */
        constexpr unsigned int getHeight() { return Style->Height.Get(); }

        /**
         * @brief Set the width of the element.
         * @details This function sets the width of the element to the specified value.
         *          If the width is different from the current width, then the element will be resized and the STRETCH stain is set.
         *          The Update_Frame() function is also called to update the frame.
         * @param width The new width of the element.
         */
        void setWidth(unsigned int width);

        /**
         * @brief Set the height of the element.
         * @details This function sets the height of the element to the specified value.
         *          If the height is different from the current height, then the element will be resized and the STRETCH stain is set.
         *          The Update_Frame() function is also called to update the frame.
         * @param height The new height of the element.
         */
        void setHeight(unsigned int height);

        /**
         * @brief Retrieves the evaluation type of the width property.
         * 
         * This function returns the evaluation type of the width property
         * from the style's width value.
         * 
         * @return EVALUATION_TYPE The evaluation type of the width property.
         */
        EVALUATION_TYPE getWidthType() { return Style->Width.Value.Get_Type(); }

        /**
         * @brief Retrieves the evaluation type of the height value.
         * 
         * This function returns the evaluation type of the height value from the style's height property.
         * 
         * @return EVALUATION_TYPE The evaluation type of the height value.
         */
        EVALUATION_TYPE getHeightType() { return Style->Height.Value.Get_Type(); }

        /**
         * @brief Set the position of the element.
         * @details This function sets the position of the element to the specified coordinates.
         *          If the position changes, the element will be marked as dirty for movement
         *          and the frame will be updated.
         * @param c The new position of the element.
         */
        void setPosition(IVector3 c);
       
        /**
         * @brief Set the position of the element.
         * @details This function sets the position of the element to the specified coordinates.
         *          If the position changes, the element will be marked as dirty for movement
         *          and the frame will be updated.
         * @param c The new position of the element.
         */
        void setPosition(IVector3* c);

        /**
         * @brief Get the position of the element.
         * @details This function retrieves the position of the element from its style.
         * @return The position of the element as an IVector3 object.
         */
        constexpr IVector3 getPosition() { return Style->Position.Get(); }

        /**
         * @brief Get the absolute position of the element.
         * @details This function returns the cached absolute position of the element.
         *          The absolute position is the position of the element in the context of the entire document or window.
         * @return The absolute position of the element as an IVector3 object.
         */
        constexpr IVector3 getAbsolutePosition() { return Absolute_Position_Cache; }

        /**
         * @brief Update the absolute position cache of the element.
         * @details This function updates the cached absolute position of the element by adding the position of the element to the position of its parent.
         */
        void updateAbsolutePositionCache();

        /**
         * @brief Set the margin of the element.
         * @details This function sets the margin of the element to the specified margin values.
         *          The margin is stored in the element's style.
         * @param margin The new margin values for the element.
         */
        void setMargin(margin margin);

        /**
         * @brief Get the margin of the element.
         * @details This function retrieves the margin of the element from its style.
         * @return The margin of the element as a GGUI::margin object.
         */
        margin getMargin() { return Style->Margin; }
        
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
        virtual void setBackgroundColor(RGB color);

        /**
         * @brief Retrieves the background color of the element.
         * 
         * This function returns the RGB value of the background color 
         * from the element's style.
         * 
         * @return The RGB color of the element's background.
         */
        constexpr RGB getBackgroundColor() { return Style->Background_Color.Value.Get<RGB>(); }
        
        /**
         * @brief Sets the border color of the element.
         * 
         * This function sets the border color of the element to the specified RGB value. Marks the element as dirty for color updates and triggers a frame update.
         * 
         * @param color The RGB color to set as the border color.
         */
        virtual void setBorderColor(RGB color);
        
        /**
         * @brief Retrieves the border color of the element.
         * 
         * This function returns the RGB value of the border color 
         * from the element's style.
         * 
         * @return The RGB color of the element's border.
         */
        constexpr RGB getBorderColor(){ return Style->Border_Color.Value.Get<RGB>(); }

        /**
         * @brief Sets the border background color of the element.
         * 
         * This function sets the border background color of the element to the specified RGB value.
         * It marks the element as dirty for color updates and triggers a frame update.
         * 
         * @param color The RGB color to set as the border background color.
         */
        virtual void setBorderBackgroundColor(RGB color);
        
        /**
         * @brief Retrieves the border background color of the element.
         * 
         * This function returns the RGB value of the border background color
         * from the element's style.
         * 
         * @return The RGB color of the element's border background.
         */
        constexpr RGB getBorderBackgroundColor(){ return Style->Border_Background_Color.Value.Get<RGB>(); }
        
        /**
         * @brief Sets the text color of the element.
         * 
         * This function sets the text color of the element to the specified RGB value. 
         * It marks the element as dirty for color updates and triggers a frame update.
         * 
         * @param color The RGB color to set as the text color.
         */
        virtual void setTextColor(RGB color);

        /**
         * @brief Retrieves the text color of the element.
         * 
         * This function returns the RGB value of the text color
         * from the element's style.
         * 
         * @return The RGB color of the element's text.
         */
        constexpr RGB getTextColor(){ return Style->Text_Color.Value.Get<RGB>(); }

        /**
         * @brief Sets the hover border color of the element.
         * 
         * This function sets the border color of the element when the mouse hovers over it
         * to the specified RGB value. Marks the element as dirty for color updates and
         * triggers a frame update.
         * 
         * @param color The RGB color to set as the hover border color.
         */
        void setHoverBorderColor(RGB color);

        /**
         * @brief Retrieves the hover border color of the element.
         * 
         * This function returns the RGB value of the border color when the mouse hovers over the element
         * from the element's style.
         * 
         * @return The RGB color of the element's hover border.
         */
        constexpr RGB getHoverBorderColor(){ return Style->Hover_Border_Color.Value.Get<RGB>(); }

        /**
         * @brief Sets the hover background color of the element.
         * 
         * This function sets the background color of the element when the mouse hovers over it
         * to the specified RGB value. Marks the element as dirty for color updates and triggers
         * a frame update.
         * 
         * @param color The RGB color to set as the hover background color.
         */
        void setHoverBackgroundColor(RGB color);

        /**
         * @brief Retrieves the hover background color of the element.
         * 
         * This function returns the RGB value of the background color when the mouse hovers over the element
         * from the element's style.
         * 
         * @return The RGB color of the element's hover background.
         */
        constexpr RGB getHoverBackgroundColor(){ return Style->Hover_Background_Color.Value.Get<RGB>(); }

        /**
         * @brief Sets the hover text color of the element.
         * 
         * This function sets the text color of the element when the mouse hovers over it
         * to the specified RGB value. Marks the element as dirty for color updates and triggers
         * a frame update.
         * 
         * @param color The RGB color to set as the hover text color.
         */
        void setHoverTextColor(RGB color);

        /**
         * @brief Retrieves the hover text color of the element.
         * 
         * This function returns the RGB value of the text color when the mouse hovers over the element
         * from the element's style.
         * 
         * @return The RGB color of the element's hover text.
         */
        constexpr RGB getHoverTextColor(){ return Style->Hover_Text_Color.Value.Get<RGB>(); }

        /**
         * @brief Sets the hover border background color of the element.
         * 
         * This function sets the background color of the element's border 
         * when the mouse hovers over it to the specified RGB value. It marks 
         * the element as dirty for color updates and triggers a frame update.
         * 
         * @param color The RGB color to set as the hover border background color.
         */
        void setHoverBorderBackgroundColor(RGB color);

        /**
         * @brief Retrieves the hover border background color of the element.
         * 
         * This function returns the RGB value of the background color of the element's border
         * when the mouse hovers over it from the element's style.
         * 
         * @return The RGB color of the element's hover border background.
         */
        constexpr RGB getHoverBorderBackgroundColor(){ return Style->Hover_Border_Background_Color.Value.Get<RGB>(); }

        /**
         * @brief Sets the focus border color of the element.
         * 
         * This function sets the color of the element's border when it is focused to the specified RGB value. It marks the element as dirty for color updates and triggers a frame update.
         * 
         * @param color The RGB color to set as the focus border color.
         */
        void setFocusBorderColor(RGB color);

        /**
         * @brief Retrieves the focus border color of the element.
         * 
         * This function returns the RGB value of the border color when the element is focused
         * from the element's style.
         * 
         * @return The RGB color of the element's focus border.
         */
        constexpr RGB getFocusBorderColor(){ return Style->Focus_Border_Color.Value.Get<RGB>(); }

        /**
         * @brief Sets the focus background color of the element.
         * 
         * This function sets the background color of the element when it is focused to the specified RGB value. It marks the element as dirty for color updates and triggers a frame update.
         * 
         * @param color The RGB color to set as the focus background color.
         */
        void setFocusBackgroundColor(RGB color);

        /**
         * @brief Retrieves the focus background color of the element.
         * 
         * This function returns the RGB value of the background color when the element is focused
         * from the element's style.
         * 
         * @return The RGB color of the element's focus background.
         */
        constexpr RGB getFocusBackgroundColor(){ return Style->Focus_Background_Color.Value.Get<RGB>(); }

        /**
         * @brief Sets the focus text color of the element.
         * 
         * This function sets the text color of the element when it is focused to the specified RGB value. It marks the element as dirty for color updates and triggers a frame update.
         * 
         * @param color The RGB color to set as the focus text color.
         */
        void setFocusTextColor(RGB color);

        /**
         * @brief Retrieves the focus text color of the element.
         * 
         * This function returns the RGB value of the text color when the element is focused
         * from the element's style.
         * 
         * @return The RGB color of the element's focus text.
         */
        constexpr RGB getFocusTextColor(){ return Style->Focus_Text_Color.Value.Get<RGB>(); }

        /**
         * @brief Sets the focus border background color of the element.
         * 
         * This function sets the focus border background color of the element to the specified RGB value.
         * It marks the element as dirty for color updates and triggers a frame update.
         * 
         * @param color The RGB color to set as the focus border background color.
         */
        void setFocusBorderBackgroundColor(RGB color);

        /**
         * @brief Retrieves the focus border background color of the element.
         * 
         * This function returns the RGB value of the focus border background color
         * from the element's style.
         * 
         * @return The RGB color of the element's focus border background.
         */
        constexpr RGB getFocusBorderBackgroundColor(){ return Style->Focus_Border_Background_Color.Value.Get<RGB>(); }

        /**
         * @brief Sets the alignment of the element.
         * 
         * This function sets the alignment of the element to the specified ALIGN value.
         * 
         * @param Align The alignment value to set for the element.
         */
        void setAlign(ALIGN a);

        /**
         * @brief Sets the alignment of the element.
         * 
         * This function sets the alignment of the element to the specified ALIGN value.
         * 
         * @param Align The alignment value to set for the element.
         */
        constexpr ALIGN getAlign(){ return Style->Align.Value; }

        /**
         * @brief Sets the flow priority of the element.
         * 
         * This function sets the flow priority of the element to the specified DIRECTION value.
         * The flow priority determines how the element will be aligned in its parent when the parent is a flow layout.
         * 
         * @param Priority The flow priority value to set for the element.
         */
        void setFlowPriority(DIRECTION d);

        /**
         * @brief Retrieves the flow priority of the element.
         * 
         * This function returns the DIRECTION value that was previously set with Set_Flow_Priority.
         * The flow priority determines how the element will be aligned in its parent when the parent is a flow layout.
         * 
         * @return The flow priority value of the element.
         */
        constexpr DIRECTION getFlowPriority(){ return Style->Flow_Priority.Value; }

        /**
         * @brief Sets whether the element will wrap its contents to the next line when it hits the edge of the screen.
         * 
         * This function sets whether the element will wrap its contents to the next line when it hits the edge of the screen.
         * If true, the element will wrap its contents to the next line when it hits the edge of the screen.
         * If false, the element will not wrap its contents to the next line when it hits the edge of the screen.
         * 
         * @param Wrap The value to set for whether the element will wrap its contents to the next line.
         */
        void setWrap(bool w);

        /**
         * @brief Retrieves the wrap setting of the element.
         * 
         * This function returns whether the element will wrap its contents to the next line
         * when it reaches the edge of the screen.
         * 
         * @return True if the element will wrap its contents, false otherwise.
         */
        constexpr bool getWrap(){ return Style->Wrap.Value; }

        /**
         * @brief Sets whether the element is allowed to dynamically resize.
         * 
         * This function enables or disables the ability of the element to 
         * adjust its size based on its content.
         * 
         * @param True A boolean indicating whether dynamic resizing is allowed.
         */
        void allowDynamicSize(bool True);

        /**
         * @brief Checks whether the element is allowed to dynamically resize.
         * 
         * This function checks the Allow_Dynamic_Size property in the element's style
         * and returns its value.
         * 
         * @return True if the element is allowed to dynamically resize, false otherwise.
         */
        constexpr bool isDynamicSizeAllowed(){ return Style->Allow_Dynamic_Size.Value; }

        /**
         * @brief Sets whether the element allows overflow.
         * 
         * This function enables or disables the overflow property of the element,
         * allowing child elements to exceed the parent's dimensions without resizing it.
         * 
         * @param True A boolean indicating whether overflow is allowed.
         */ 
        void allowOverflow(bool True);

        /**
         * @brief Checks whether the element allows overflow.
         * 
         * This function checks the Allow_Overflow property in the element's style
         * and returns its value.
         * 
         * @return True if the element allows overflow, false otherwise.
         */
        constexpr bool isOverflowAllowed(){ return Style->Allow_Overflow.Value; }
        
        /**
         * @brief Gets the fitting area for a child element in its parent.
         * @details This function calculates the area where the child element should be rendered within the parent element.
         *          It takes into account the border offsets of both the parent and the child element as well as their positions.
         *          The function returns a pair of pairs, where the first pair contains the negative offset of the child element from the parent element,
         *          the second pair contains the starting offset of the child element within the parent element and the third pair contains the ending offset of the child element within the parent element.
         * @param Parent The parent element.
         * @param Child The child element.
         * @return A pair of pairs containing the fitting area for the child element within the parent element.
         */
        static std::pair<std::pair<unsigned int, unsigned int> ,std::pair<std::pair<unsigned int, unsigned int>, std::pair<unsigned int, unsigned int>>> getFittingArea(GGUI::element* Parent, GGUI::element* Child);
                
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
        void computeDynamicSize();

        /**
         * @brief Renders the element and its children into the Render_Buffer nested buffer of the window.
         * @details This function processes the element to generate a vector of UTF objects representing the current state.
         * It handles different stains such as CLASS, STRETCH, COLOR, and EDGE to ensure the element is rendered correctly.
         * @return A vector of UTF objects representing the rendered element and its children.
         */
        virtual std::vector<GGUI::UTF>& render();

        /**
         * @brief Updates the parent element of the current element.
         * @details This function is called when the current element is added, removed, or moved
         *          to a different parent element. It marks the parent element as dirty and
         *          requests a render update.
         * @param New_Element The new parent element.
         *
         * @note If the parent element does not have a valid render buffer (i.e., its
         *       `Is_Displayed()` function returns false), this function marks the parent
         *       element as dirty with the `STAIN_TYPE::DEEP` and `STAIN_TYPE::COLOR` stains.
         *       This ensures that the parent element is re-rendered from scratch when the
         *       rendering thread is updated.
         */
        virtual void updateParent(element* New_Element);

        /**
         * @brief Add the border of the window to the rendered string.
         *
         * @param w The window to add the border for.
         * @param Result The string to add the border to.
         */
        virtual void addOverhead(element* w, std::vector<UTF>& Result);

        /**
         * @brief Apply the color system to the rendered string.
         *
         * This function applies the color system set by the style to the rendered string.
         * It is called after the element has been rendered and the result is stored in the
         * Result vector.
         *
         * @param w The window to apply the color system to.
         * @param Result The vector containing the rendered string.
         */
        void applyColors(element* w, std::vector<UTF>& Result);

        /**
         * @brief Resizes the element to fit the size of its parent element.
         * @details This function is called when the parent element is resized and the
         *          current element is a child of the parent element. It resizes the
         *          current element to fit the size of its parent element. If the parent
         *          element does not have a valid render buffer (i.e., its
         *          `Is_Displayed()` function returns false), this function does
         *          nothing.
         * @param parent The parent element to resize to.
         * @return true if the resize was successful, false otherwise.
         */
        virtual bool resizeTo([[maybe_unused]] element* parent){
            return false;
        }

        /**
         * @brief Compute the alpha blending of the source element to the destination element.
         * @details This function takes two UTF elements as arguments, the source element and the destination element.
         *          It calculates the alpha blending of the source element to the destination element, by adding the
         *          background color of the source element to the destination element, but only if the source element has
         *          a non-zero alpha value. If the source element has full opacity, then the destination gets fully rewritten
         *          over. If the source element has full transparency, then nothing is done.
         * @param Dest The destination element to which the source element will be blended.
         * @param Source The source element which will be blended to the destination element.
         */
        void computeAlphaToNesting(GGUI::UTF& Dest, GGUI::UTF Source);

        /**
         * @brief Nests a child element into a parent element.
         * @details This function calculates the area where the child element should be rendered within the parent element.
         *          It takes into account the border offsets of both the parent and the child element as well as their positions.
         *          The function then copies the contents of the child element's buffer into the parent element's buffer at the calculated position.
         * @param Parent The parent element.
         * @param Child The child element.
         * @param Parent_Buffer The parent element's buffer.
         * @param Child_Buffer The child element's buffer.
         */
        void nestElement(element* Parent, element* Child, std::vector<UTF>& Parent_Buffer, std::vector<UTF>& Child_Buffer);

        /**
         * @brief Returns a map of the custom border symbols for the given element.
         * @param e The element to get the custom border map for.
         * @return A map of the custom border symbols where the key is the bit mask of the border and the value is the corresponding symbol.
         */
        std::unordered_map<unsigned int, const char*> getCustomBorderMap(element* e);

        /**
         * @brief Returns a map of the custom border symbols for the given border style.
         * The map key is the bit mask of the border and the value is the corresponding symbol.
         * @param custom_border_style The custom border style to get the map for.
         * @return A map of the custom border symbols.
         */
        std::unordered_map<unsigned int, const char*> getCustomBorderMap(GGUI::styled_border custom_border_style);

        /**
         * @brief Sets the custom border style for the element.
         * @details This function sets the custom border style for the element, marks the element's edges as dirty, and ensures that the border is visible.
         * @param style The custom border style to set.
         */
        void setCustomBorderStyle(GGUI::styled_border style);

        /**
         * @brief Gets the custom border style of the element.
         * @return The custom border style of the element.
         */
        GGUI::styled_border getCustomBorderStyle(){ return Style->Border_Style; }

        /**
         * @brief Posts a process that handles the intersection of borders between two elements and their parent.
         * @details This function posts a process that handles the intersection of borders between two elements and their parent.
         *          The process calculates the intersection points of the borders and then constructs a bit mask that portraits the connections the middle point has.
         *          With the calculated bit mask it can fetch from the 'SYMBOLS::Border_Identifiers' the right border string.
         * @param A The first element.
         * @param B The second element.
         * @param Parent_Buffer The buffer of the parent element.
         */
        void postProcessBorders(element* A, element* B, std::vector<UTF>& Parent_Buffer);

        /**
         * @brief Composes the RGB values of the text color and background color of the element.
         * 
         * This function will return a pair of RGB values, where the first element is the
         * color of the text and the second element is the color of the background.
         * 
         * If the element is focused, the function will return the RGB values of the focused
         * text color and background color. If the element is hovered, the function will
         * return the RGB values of the hovered text color and background color. Otherwise,
         * the function will return the RGB values of the normal text color and background
         * color.
         * 
         * @return A pair of RGB values representing the text color and background color of the element.
         */
        std::pair<RGB, RGB>  composeAllTextRGBValues();

        /**
         * @brief Composes the RGB values of the text color of the element.
         * 
         * This function will return the RGB values of the text color of the element.
         * If the element is focused, the function will return the RGB values of the focused
         * text color. If the element is hovered, the function will return the RGB values of the hovered
         * text color. Otherwise, the function will return the RGB values of the normal text color.
         * 
         * @return The RGB color of the element's text.
         */
        RGB  composeTextRGBValues();
        
        /**
         * @brief Composes the RGB values of the background color of the element.
         * 
         * This function will return the RGB values of the background color of the element.
         * If the element is focused, the function will return the RGB values of the focused
         * background color. If the element is hovered, the function will return the RGB values of the hovered
         * background color. Otherwise, the function will return the RGB values of the normal background color.
         * 
         * @return The RGB color of the element's background.
         */
        RGB  composeBackgroundRGBValues();

        /**
         * @brief Composes the RGB values of the border color and background color of the element.
         * @details This function will return the RGB values of the border color and background color of the element.
         * If the element is focused, the function will return the RGB values of the focused border color and background color.
         * If the element is hovered, the function will return the RGB values of the hovered border color and background color.
         * Otherwise, the function will return the RGB values of the normal border color and background color.
         * @return A pair of RGB values representing the border color and background color of the element.
         */
        std::pair<RGB, RGB>  composeAllBorderRGBValues();

        /**
         * @brief Returns the name of the element.
         * @details This function returns a string that represents the name of the element.
         *          The name is constructed by concatenating the name of the element with the 
         *          class name of the element, separated by a "<" and a ">".
         * @return The name of the element.
         */
        virtual std::string getName() const {
            return "Element<" + Name + ">";
        }

        /**
         * @brief Set the name of the element.
         * @details This function sets the name of the element and stores it in the global Element_Names map.
         * @param name The name of the element.
         */
        void setName(std::string name);

        /**
         * @brief Removes the element from the parent element.
         * @details This function first checks if the element has a parent.
         *          If the element has a parent, it calls the parent's Remove() function to remove the element from the parent.
         *          If the element does not have a parent, it prints an error message to the console.
         *          The function does not update the frame, so it is the caller's responsibility to update the frame after calling this function.
         */
        void remove();

        /**
         * @brief A function that registers a lambda to be executed when the element is clicked.
         * @details The lambda is given a pointer to the Event object that triggered the call.
         *          The lambda is expected to return true if it was successful and false if it failed.
         * @param action The lambda to be called when the element is clicked.
         */
        void onClick(std::function<bool(GGUI::Event*)> action);

        /**
         * @brief A function that registers a lambda to be executed when the element is interacted with in any way.
         * @details The lambda is given a pointer to the Event object that triggered the call.
         *          The lambda is expected to return true if it was successful and false if it failed.
         * @param criteria The criteria to check for when deciding whether to execute the lambda.
         * @param action The lambda to be called when the element is interacted with.
         * @param GLOBAL Whether the lambda should be executed even if the element is not under the mouse.
         */
        void on(unsigned long long criteria, std::function<bool(GGUI::Event*)> action, bool GLOBAL = false);

        /**
         * @brief Retrieves an element by name.
         * @details This function takes a string argument representing the name of the element
         *          and returns a pointer to the element if it exists in the global Element_Names map.
         * @param name The name of the element to retrieve.
         * @return A pointer to the element if it exists; otherwise, nullptr.
         */
        element* getElement(std::string name);

        // TEMPLATES
        //-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_

        /**
         * @brief Retrieves all child elements that have the same type as the given template.
         * @details This function takes a template argument representing the type of the elements to retrieve.
         *          It goes through the child AST, and checks if the element in question is the same type as the template T.
         *          If the element in question is of the same type as the template T, it adds it to the result vector.
         *          The function then recursively goes through the child AST, and checks if any of the child elements are of the same type as the template T.
         *          If any of the child elements are of the same type as the template T, it adds them to the result vector.
         *          The function then returns the result vector, which contains all child elements that have the same type as the given template.
         * @param T The type of the elements to retrieve.
         * @return A vector of pointers to the elements that have the same type as the given template.
         */
        template<typename T>
        std::vector<T*> getElements(){
            std::vector<T*> result;

            // Check if the element in question is of the same type as the template T.
            if (typeid(*this) == typeid(T)){
                result.push_back((T*)this);
            }

            // Go through the child AST, and check if any of the child elements are of the same type as the template T.
            for (auto e : Style->Childs){
                // Recursively go through the child AST, and check if any of the child elements are of the same type as the template T.
                std::vector<T*> child_result = e->getElements<T>();

                // Add the results of the recursive call to the result vector.
                result.insert(result.end(), child_result.begin(), child_result.end());
            }

            // Return the result vector, which contains all child elements that have the same type as the given template.
            return result;
        }

        /**
         * @brief Retrieves all nested elements, including this element.
         * @details This function collects all nested elements recursively, starting from this element.
         *          If 'Show_Hidden' is false, hidden elements are excluded from the result.
         * @param Show_Hidden Flag to determine whether to include hidden elements in the result.
         * @return A vector of pointers to all nested elements.
         */
        std::vector<element*> getAllNestedElements(bool Show_Hidden = false) {
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

        //-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_

        /**
         * @brief Default virtual function for scrolling up.
         * @details By default, elements do not have inherent scrolling abilities.
         *          This function is used as a base for other elements to implement their own scrolling.
         */
        virtual void scrollUp() {}

        /**
         * @brief Default virtual function for scrolling down.
         * @details By default, elements do not have inherent scrolling abilities.
         *          This function is used as a base for other elements to implement their own scrolling.
         */
        virtual void scrollDown() {}

        /**
         * @brief Reorders child elements based on their z-position.
         * @details This function sorts the child elements of the current element by their z-coordinate
         *          in ascending order, so that elements with a higher z-coordinate appear later in the list.
         */
        void reOrderChilds();

        /**
         * @brief Focuses the element.
         * @details This function updates the global focus information by setting the mouse position to the element's position and updating the focused element.
         */
        void focus();

        /**
         * @brief Adds a handler function to the state handlers map.
         * @details This function takes a state and a handler function as arguments.
         *          The handler function is stored in the State_Handlers map with the given state as the key.
         * @param s The state for which the handler should be executed.
         * @param job The handler function to be executed
         */
        void onState(STATE s, void (*job)(element* self));

        /**
         * @brief Checks if the element needs postprocessing.
         * @details This function checks if the element needs postprocessing by checking if the element has a shadow or is transparent.
         * @return True if the element needs postprocessing; otherwise, false.
         */
        bool hasPostprocessingToDo();

        /**
         * @brief Process the shadow of the element.
         * @details This function processes the shadow of the element by calculating the new buffer size and creating a new buffer with the shadow.
         *          It then offsets the shadow box buffer by the direction and blends it with the original buffer.
         * @param Current_Buffer The buffer to be processed.
         */
        void processShadow(std::vector<GGUI::UTF>& Current_Buffer);

        /**
         * @brief Applies the opacity of the element to the given buffer.
         * @details This function will iterate over the given buffer and apply the opacity of the element to the background and foreground of each UTF character.
         * @param Current_Buffer The buffer to be processed.
         */
        void processOpacity(std::vector<GGUI::UTF>& Current_Buffer);

        /**
         * @brief
         * This function performs postprocessing on the rendered buffer of the element.
         * It applies the shadow, and then the opacity to the rendered buffer.
         * @return The postprocessed buffer.
         */
        virtual std::vector<GGUI::UTF>& postprocess();

        // Customization helper function
        //-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_
        
        /**
         * @brief Adds a stain to the element.
         * @details This function adds the given stain to the element's stains list.
         *          The stains list is used to keep track of which properties of the element need to be re-evaluated.
         *          The function takes a STAIN_TYPE as a parameter and adds it to the list of stains.
         * @param s The stain to be added.
         */
        void addStain(STAIN_TYPE s){
            Dirty.Dirty(s);
        }

        /**
         * @brief
         * This function determines if the given element is a direct child of this element (in the DOM tree),
         * and if it is visible on the screen (does not go out of bounds of the parent element).
         * @param other Child element to check
         * @return True if the child element is visible within the bounds of the parent.
         */
        bool childIsShown(element* other);

        inline void setOnInit(void (*func)(element* self)){
            On_Init = func;
        }

        inline void setOnDestroy(void (*func)(element* self)){
            On_Destroy = func;
        }

        inline void setOnHide(void (*func)(element* self)){
            On_Hide = func;
        }

        inline void setOnShow(void (*func)(element* self)){
            On_Show = func;
        }

        inline void forceStyleEvaluation(){
            if (Style)
                Style->Evaluate_Dynamic_Attribute_Values(this);
        }
    };
}

#endif
