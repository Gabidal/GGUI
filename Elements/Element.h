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

#include "../Core/Utils/Super_String.h"
#include "../Core/Utils/Constants.h"
#include "../Core/Utils/Color.h"
#include "../Core/Utils/Units.h"
#include "../Core/Utils/Event.h"
#include "../Core/Utils/Style.h"

namespace GGUI{
    class Element{
    protected:
        unsigned int Post_Process_Width = 0;
        unsigned int Post_Process_Height = 0;

        // Only fetch one parent UP, and own position +, then child repeat.
        IVector3 Absolute_Position_Cache;

        //INTERNAL FLAGS
        class Element* Parent = nullptr;
        bool Show = true;
        
        std::vector<UTF> Render_Buffer;
        std::vector<UTF> Post_Process_Buffer;
        STAIN Dirty;
        
        std::vector<int> Classes;

        bool Focused = false;
        bool Hovered = false;

        std::string Name = "";

        // NOTE: do NOT set the .VALUEs manually set each member straight with the operator= overload.
        Styling* Style = nullptr;

        std::unordered_map<State, std::function<void()>> State_Handlers;
    public:

        /**
         * The constructor for the Element class.
         *
         * This constructor is used when an Element is created without a parent.
         * In this case, the Element is created as a root object, and it will be
         * automatically added to the list of root objects.
         *
         * @param None
         */
        Element();

        /**
         * The constructor for the Element class that accepts a Styling object.
         *
         * This constructor is used when an Element is created without a parent.
         * In this case, the Element is created as a root object, and it will be
         * automatically added to the list of root objects.
         *
         * @param s The Styling object to use for the Element.
         */
        Element(Styling s);

        
        /**
         * @brief Copy constructor for the Element class.
         *
         * This constructor is disabled and should not be used.
         * Instead, use the Copy() method to create a copy of an Element.
         *
         * @param copyable The Element object to be copied.
         */
        Element(const Element&);

        /**
         * @brief Assignment operator for the Element class.
         *
         * This operator is used to assign the values from one Element object
         * to another. The default implementation is used, which performs
         * a member-wise copy of the element's properties.
         *
         * @param other The Element object to assign from.
         * @return A reference to the assigned Element object.
         */
        Element& operator=(const GGUI::Element&) = default;

        //Start of destructors.
        //-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_

        /**
         * @brief The destructor for the Element class.
         *
         * This destructor is responsible for properly deallocating all the memory
         * allocated by the Element object.
         *
         * @note This destructor is also responsible for cleaning up the parent
         * element's vector of child elements and the event handlers list.
         */
        virtual ~Element();

        //
        //-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_

        virtual Element* Safe_Move(){
            Element* new_element = new Element();
            *new_element = *(Element*)this;

            return new_element;
        }

        /**
         * Creates a deep copy of this Element, including all its children.
         * @return A new Element object that is a copy of this one.
         */
        Element* Copy();

        // @brief Marks the Element as fully dirty by setting all stain types.
        // 
        // This function sets each stain type on the Dirty object, indicating
        // that the Element needs to be reprocessed for all attributes.
        virtual void Fully_Stain();

        
        /**
         * @brief Copies the state of the abstract element to the current element.
         *
         * This function will copy the state of the abstract element to the current element.
         * It will copy the following states: focused and show.
         *
         * @param abstract The abstract element to copy the state from.
         */
        void Inherit_States_From(Element* abstract);

        /**
         * @brief Accumulates all the classes and their styles.
         * @details This method accumulates all the classes and their styles to the
         *          current element.
         */
        void Parse_Classes();

        /**
         * @brief Returns the Dirty object for the Element.
         * @details This function returns the Dirty object, which is a bitfield
         *          that keeps track of what needs to be reprocessed on the
         *          Element when it is asked to render.
         * @return A reference to the Dirty object.
         */
        STAIN& Get_Dirty(){
            return Dirty;
        }


        /**
         * @brief Returns true if the element is currently focused.
         * @return A boolean indicating whether the element is focused.
         */
        bool Is_Focused(){
            return Focused;
        }

        /**
         * @brief Sets the focus state of the element.
         * @details Sets the focus state of the element to the given value.
         *          If the focus state changes, the element will be dirtied and the frame will be updated.
         * @param f The new focus state.
         */
        void Set_Focus(bool f);

        /**
         * @brief Returns true if the element is currently hovered.
         * @return A boolean indicating whether the element is hovered.
         */
        bool Is_Hovered(){
            return Hovered;
        }

        /**
         * @brief Sets the hover state of the element.
         * @details Sets the hover state of the element to the given value.
         *          If the hover state changes, the element will be dirtied and the frame will be updated.
         * @param h The new hover state.
         */
        void Set_Hover_State(bool h);

        /**
         * @brief Executes the handler function associated with a given state.
         * @details This function checks if there is a registered handler for the specified state.
         *          If a handler exists, it invokes the handler function.
         * @param s The state for which the handler should be executed.
         */
        void Check(State s);

        /**
         * @brief Retrieves the styling information of the element.
         * @details This function returns the current styling object associated with the element.
         *          The styling object contains various style attributes such as colors, borders, etc.
         * @return The styling object of the element.
         */
        Styling Get_Style();

        /**
         * @brief Sets the styling information of the element.
         * @details This function sets the styling information of the element to the given value.
         *          If the element already has a styling object associated with it, the function will
         *          copy the given styling information to the existing object. Otherwise, the function
         *          will create a new styling object and associate it with the element.
         * @param css The new styling information to associate with the element.
         */
        void Set_Style(Styling css);

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
        virtual void Calculate_Childs_Hitboxes([[maybe_unused]] unsigned int Starting_Offset = 0) {}

        /**
         * @brief Handles the OR operator for two elements.
         * @details This function is called when the OR operator is used between two elements.
         *          It sets the style of the current element to the style of the other element.
         * @param other The other element to handle the OR operator with.
         * @return The current element.
         */
        virtual Element* Handle_Or_Operator(Element* other){
            // Set the style of the current element to the style of the other element.
            Set_Style(other->Get_Style());

            return this;
        }

        /**
         * @brief Adds a class to the element.
         * @details This function adds the given class to the element's class list.
         *          If the class does not exist in the global class map, a new ID is assigned to the class.
         *          The element is then marked as dirty, which will trigger a re-render of the element.
         * @param class_name The name of the class to add.
         */
        void Add_Class(std::string class_name);

        
        /**
         * @brief Sets the opacity of the element.
         * @details This function takes a float value between 0.0f and 1.0f and sets the
         * opacity of the element to that value. If the value is greater than 1.0f, the
         * function will report an error and do nothing.
         * @param[in] Opacity The opacity value to set.
         */
        void Set_Opacity(float Opacity);

        
        /**
         * @brief Sets the opacity of the element using an integer percentage.
         * @details This function takes an unsigned integer value between 0 and 100, representing the opacity percentage,
         * and sets the element's opacity. If the value is greater than 100, it will report an error and do nothing.
         * @param[in] Opacity The opacity percentage to set.
         */
        void Set_Opacity(unsigned int Opacity);

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
        styled_border Get_Border_Style(){
            return Style->Border_Style;
        }

        /**
         * @brief Gets the opacity of the element.
         * @details This function returns the current opacity of the element as a float value.
         *          The opacity is a value between 0.0 and 1.0, where 0.0 is fully transparent
         *          and 1.0 is fully opaque.
         * @return The current opacity of the element.
         */
        float Get_Opacity(); 

        /**
         * @brief Checks if the element is transparent.
         * @details This function determines whether the element is transparent by checking
         *          if the element's opacity is not equal to 1.0f. An opacity less than 1.0f
         *          indicates that the element is partially or fully transparent.
         * @return True if the element is transparent; otherwise, false.
         */
        bool Is_Transparent();

        /**
         * @brief Gets the processed width of the element.
         * @details This function returns the width of the element after any post-processing
         *          has been applied. If the element has not been post-processed, the
         *          original width of the element is returned.
         * @return The processed width of the element.
         */
        unsigned int Get_Processed_Width();

        /**
         * @brief Gets the processed height of the element.
         * @details This function returns the height of the element after any post-processing
         *          has been applied. If the element has not been post-processed, the
         *          original height of the element is returned.
         * @return The processed height of the element.
         */
        unsigned int Get_Processed_Height();

        /**
         * @brief Configures and displays the shadow for the element.
         * @details This function sets the shadow properties such as direction, color, opacity, 
         *          and length, and applies the shadow effect to the element. It adjusts the 
         *          element's position to account for the shadow and marks the element as dirty 
         *          for a visual update.
         * @param[in] Direction The direction vector of the shadow.
         * @param[in] Shadow_Color The color of the shadow.
         * @param[in] Opacity The opacity of the shadow, between 0.0f (fully transparent) and 1.0f (fully opaque).
         * @param[in] Length The length of the shadow.
         */
        void Show_Shadow(FVector2 Direction, RGB Shadow_Color, float Opacity = 1, float Length = 0.5);

        /**
         * @brief Displays the shadow for the element.
         * @details This function sets the shadow properties such as direction, color, opacity, and length, and applies the shadow effect to the element. It adjusts the element's position to account for the shadow and marks the element as dirty for a visual update. The direction vector of the shadow is set to (0, 0) by default, which means the shadow will appear directly below the element.
         * @param[in] Shadow_Color The color of the shadow.
         * @param[in] Opacity The opacity of the shadow, between 0.0f (fully transparent) and 1.0f (fully opaque).
         * @param[in] Length The length of the shadow.
         */
        void Show_Shadow(RGB Shadow_Color, float Opacity = 1, float Length = 0.5);

        /**
         * @brief Sets the shadow properties for the element.
         * @details This function sets the shadow properties such as direction, color, opacity, and length, and applies the shadow effect to the element. It also marks the element as dirty for a visual update.
         * @param[in] s The shadow properties to set.
         */
        void Set_Shadow(shadow s);

        /**
         * @brief Retrieves the parent element.
         * @details This function returns the parent element of the current element.
         *          If the element has no parent, it will return nullptr.
         * @return A pointer to the parent element.
         */
        Element* Get_Parent(){
            return Parent;
        }

        /**
         * @brief Sets the parent of this element.
         * @details This function sets the parent of this element to the given element.
         *          If the given element is nullptr, it will clear the parent of this
         *          element.
         * @param[in] parent The parent element to set.
         */
        void Set_Parent(Element* parent);

        /**
         * @brief Checks if the element has the given class.
         * @details This function takes a class name and checks if the element has the class in its class list.
         *          If the class does not exist in the global class map, the function will return false.
         *          If the class exists, the function will return true if the element has the class in its list.
         * @param s The name of the class to check.
         * @return True if the element has the class, false otherwise.
         */
        bool Has(std::string s);

        /**
         * @brief Checks if the element has the given class ID.
         * @details This function takes a class ID and checks if the element has the class in its class list.
         *          If the class does not exist in the global class map, the function will return false.
         *          If the class exists, the function will return true if the element has the class in its list.
         * @param s The ID of the class to check.
         * @return True if the element has the class, false otherwise.
         */
        bool Has(int s){
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
        std::pair<unsigned int, unsigned int> Get_Fitting_Dimensions(Element* child);


        /**
         * @brief Returns the maximum dimensions of the element without exceeding the parent element's dimensions.
         * @return A pair containing the maximum width and height of the element.
         */
        std::pair<unsigned int, unsigned int> Get_Limit_Dimensions();

        /**
         * @brief Sets the border visibility of the element.
         * @details This function takes a boolean as a parameter and sets the border visibility of the element accordingly.
         *          If the new state is different from the current state, the element will be marked as dirty with the EDGE stain.
         * @param b The new state of the border visibility.
         */
        virtual void Show_Border(bool b);

        /**
         * @brief Sets the border visibility of the element.
         * @details This function toggles the border visibility based on the provided state.
         *          If the state has changed, it updates the border visibility, marks the 
         *          element as dirty with the EDGE stain, and updates the frame.
         * @param b The desired state of the border visibility.
         * @param Previous_State The current state of the border visibility.
         */
        virtual void Show_Border(bool b, bool Previous_state);

        /**
         * @brief Checks if the element has a border.
         * @details This function checks if the element has a border.
         *          It returns true if the element has a border, false otherwise.
         * @return True if the element has a border, false otherwise.
         */
        bool Has_Border();

        /**
         * @brief Displays or hides the element and all its children.
         * @details This function changes the display status of the element and all its children.
         *          If the element is displayed, all its children are also displayed. If the element is hidden,
         *          all its children are also hidden.
         * @param f A boolean indicating whether to display (true) or hide (false) the element and its children.
         */
        void Display(bool f);

        /**
         * @brief Returns whether the element is currently displayed.
         * @details This function is used to check whether the element is currently displayed or hidden.
         *          It returns true if the element is displayed and false if the element is hidden.
         * @return A boolean indicating whether the element is displayed (true) or hidden (false).
         */
        bool Is_Displayed();

        /**
         * @brief Adds a child element to the element.
         * @details This function adds a child element to the element. If the element has a border, it will
         *          be taken into account when calculating the size of the parent element. If the child element
         *          exceeds the size of the parent element, the parent element will be resized to fit the child
         *          element. If the parent element is not allowed to resize, the child element will be resized to
         *          fit the parent element.
         * @param Child The child element to add.
         */
        virtual void Add_Child(Element* Child);

        /**
         * @brief Adds a vector of child elements to the current element.
         * @param childs The vector of child elements to add.
         *
         * This function adds all the child elements to the current element by calling the Add_Child function for each element in the vector.
         * It also marks the current element as dirty with the DEEP stain after adding all the elements.
         */
        virtual void Set_Childs(std::vector<Element*> childs);

        /**
         * @brief Check if any children have changed.
         * @details This function will check if any of the children have changed, this is used to determine if the element needs to be re-drawn.
         * @return true if any children have changed, false otherwise.
         */
        bool Children_Changed();
        
        /**
         * @brief Check if there are any transparent children.
         * @details This function determines if the current element or any of its children
         *          are transparent and require redrawing.
         * @return True if any child is transparent and not clean; otherwise, false.
         */
        bool Has_Transparent_Children();    

        /**
         * @brief Retrieves the list of child elements.
         * @details This function returns a reference to the vector containing all child elements
         *          associated with the current element's style.
         * @return A reference to the vector of child elements.
         */
        virtual std::vector<Element*>& Get_Childs();

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
        virtual bool Remove(Element* handle);

        /**
         * @brief Removes the element at a given index from the list of child elements.
         * @details This function checks if the index is valid (i.e. if the index is within the bounds of the vector of child elements).
         *          If the index is valid, it removes the element at the specified index from the vector of child elements and deletes the element.
         *          If the index is invalid, the function returns false.
         * @param index The index of the element to remove.
         * @return True if the element was successfully removed, false otherwise.
         */
        virtual bool Remove(unsigned int index);

        /**
         * @brief Set the width and height of the element.
         * @details This function sets the width and height of the element to the specified values.
         *          If the width or height is different from the current width or height, then the element will be resized and the STRETCH stain is set.
         *          The Update_Frame() function is also called to update the frame.
         * @param width The new width of the element.
         * @param height The new height of the element.
         */
        void Set_Dimensions(unsigned int width, unsigned int height);

        /**
         * @brief Get the width of the element.
         * @details This function returns the width of the element.
         * @return The width of the element.
         */
        unsigned int Get_Width();

        /**
         * @brief Get the height of the element.
         * @details This function returns the height of the element.
         * @return The height of the element.
         */
        unsigned int Get_Height();

        /**
         * @brief Set the width of the element.
         * @details This function sets the width of the element to the specified value.
         *          If the width is different from the current width, then the element will be resized and the STRETCH stain is set.
         *          The Update_Frame() function is also called to update the frame.
         * @param width The new width of the element.
         */
        void Set_Width(unsigned int width);

        /**
         * @brief Set the height of the element.
         * @details This function sets the height of the element to the specified value.
         *          If the height is different from the current height, then the element will be resized and the STRETCH stain is set.
         *          The Update_Frame() function is also called to update the frame.
         * @param height The new height of the element.
         */
        void Set_Height(unsigned int height);

        /**
         * @brief Set the position of the element.
         * @details This function sets the position of the element to the specified coordinates.
         *          If the position changes, the element will be marked as dirty for movement
         *          and the frame will be updated.
         * @param c The new position of the element.
         */
        void Set_Position(IVector3 c);
       
        /**
         * @brief Set the position of the element.
         * @details This function sets the position of the element to the specified coordinates.
         *          If the position changes, the element will be marked as dirty for movement
         *          and the frame will be updated.
         * @param c The new position of the element.
         */
        void Set_Position(IVector3* c);

        /**
         * @brief Get the position of the element.
         * @details This function retrieves the position of the element from its style.
         * @return The position of the element as an IVector3 object.
         */
        IVector3 Get_Position();

        /**
         * @brief Get the absolute position of the element.
         * @details This function returns the cached absolute position of the element.
         *          The absolute position is the position of the element in the context of the entire document or window.
         * @return The absolute position of the element as an IVector3 object.
         */
        IVector3 Get_Absolute_Position();

        /**
         * @brief Update the absolute position cache of the element.
         * @details This function updates the cached absolute position of the element by adding the position of the element to the position of its parent.
         */
        void Update_Absolute_Position_Cache();

        /**
         * @brief Set the margin of the element.
         * @details This function sets the margin of the element to the specified margin values.
         *          The margin is stored in the element's style.
         * @param margin The new margin values for the element.
         */
        void Set_Margin(margin margin);

        /**
         * @brief Get the margin of the element.
         * @details This function retrieves the margin of the element from its style.
         * @return The margin of the element as a GGUI::margin object.
         */
        margin Get_Margin();
        
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
        virtual void Set_Background_Color(RGB color);

        /**
         * @brief Retrieves the background color of the element.
         * 
         * This function returns the RGB value of the background color 
         * from the element's style.
         * 
         * @return The RGB color of the element's background.
         */
        RGB Get_Background_Color();
        
        /**
         * @brief Sets the border color of the element.
         * 
         * This function sets the border color of the element to the specified RGB value. Marks the element as dirty for color updates and triggers a frame update.
         * 
         * @param color The RGB color to set as the border color.
         */
        virtual void Set_Border_Color(RGB color);
        
        /**
         * @brief Retrieves the border color of the element.
         * 
         * This function returns the RGB value of the border color 
         * from the element's style.
         * 
         * @return The RGB color of the element's border.
         */
        RGB Get_Border_Color();

        /**
         * @brief Sets the border background color of the element.
         * 
         * This function sets the border background color of the element to the specified RGB value.
         * It marks the element as dirty for color updates and triggers a frame update.
         * 
         * @param color The RGB color to set as the border background color.
         */
        virtual void Set_Border_Background_Color(RGB color);
        
        /**
         * @brief Retrieves the border background color of the element.
         * 
         * This function returns the RGB value of the border background color
         * from the element's style.
         * 
         * @return The RGB color of the element's border background.
         */
        RGB Get_Border_Background_Color();
        
        /**
         * @brief Sets the text color of the element.
         * 
         * This function sets the text color of the element to the specified RGB value. 
         * It marks the element as dirty for color updates and triggers a frame update.
         * 
         * @param color The RGB color to set as the text color.
         */
        virtual void Set_Text_Color(RGB color);

        /**
         * @brief Retrieves the text color of the element.
         * 
         * This function returns the RGB value of the text color
         * from the element's style.
         * 
         * @return The RGB color of the element's text.
         */
        RGB Get_Text_Color();

        /**
         * @brief Sets the hover border color of the element.
         * 
         * This function sets the border color of the element when the mouse hovers over it
         * to the specified RGB value. Marks the element as dirty for color updates and
         * triggers a frame update.
         * 
         * @param color The RGB color to set as the hover border color.
         */
        void Set_Hover_Border_Color(RGB color);

        /**
         * @brief Retrieves the hover border color of the element.
         * 
         * This function returns the RGB value of the border color when the mouse hovers over the element
         * from the element's style.
         * 
         * @return The RGB color of the element's hover border.
         */
        RGB Get_Hover_Border_Color();

        /**
         * @brief Sets the hover background color of the element.
         * 
         * This function sets the background color of the element when the mouse hovers over it
         * to the specified RGB value. Marks the element as dirty for color updates and triggers
         * a frame update.
         * 
         * @param color The RGB color to set as the hover background color.
         */
        void Set_Hover_Background_Color(RGB color);

        /**
         * @brief Retrieves the hover background color of the element.
         * 
         * This function returns the RGB value of the background color when the mouse hovers over the element
         * from the element's style.
         * 
         * @return The RGB color of the element's hover background.
         */
        RGB Get_Hover_Background_Color();

        /**
         * @brief Sets the hover text color of the element.
         * 
         * This function sets the text color of the element when the mouse hovers over it
         * to the specified RGB value. Marks the element as dirty for color updates and triggers
         * a frame update.
         * 
         * @param color The RGB color to set as the hover text color.
         */
        void Set_Hover_Text_Color(RGB color);

        /**
         * @brief Retrieves the hover text color of the element.
         * 
         * This function returns the RGB value of the text color when the mouse hovers over the element
         * from the element's style.
         * 
         * @return The RGB color of the element's hover text.
         */
        RGB Get_Hover_Text_Color();

        /**
         * @brief Sets the hover border background color of the element.
         * 
         * This function sets the background color of the element's border 
         * when the mouse hovers over it to the specified RGB value. It marks 
         * the element as dirty for color updates and triggers a frame update.
         * 
         * @param color The RGB color to set as the hover border background color.
         */
        void Set_Hover_Border_Background_Color(RGB color);

        /**
         * @brief Retrieves the hover border background color of the element.
         * 
         * This function returns the RGB value of the background color of the element's border
         * when the mouse hovers over it from the element's style.
         * 
         * @return The RGB color of the element's hover border background.
         */
        RGB Get_Hover_Border_Background_Color();

        /**
         * @brief Sets the focus border color of the element.
         * 
         * This function sets the color of the element's border when it is focused to the specified RGB value. It marks the element as dirty for color updates and triggers a frame update.
         * 
         * @param color The RGB color to set as the focus border color.
         */
        void Set_Focus_Border_Color(RGB color);

        /**
         * @brief Retrieves the focus border color of the element.
         * 
         * This function returns the RGB value of the border color when the element is focused
         * from the element's style.
         * 
         * @return The RGB color of the element's focus border.
         */
        RGB Get_Focus_Border_Color();

        /**
         * @brief Sets the focus background color of the element.
         * 
         * This function sets the background color of the element when it is focused to the specified RGB value. It marks the element as dirty for color updates and triggers a frame update.
         * 
         * @param color The RGB color to set as the focus background color.
         */
        void Set_Focus_Background_Color(RGB color);

        /**
         * @brief Retrieves the focus background color of the element.
         * 
         * This function returns the RGB value of the background color when the element is focused
         * from the element's style.
         * 
         * @return The RGB color of the element's focus background.
         */
        RGB Get_Focus_Background_Color();

        /**
         * @brief Sets the focus text color of the element.
         * 
         * This function sets the text color of the element when it is focused to the specified RGB value. It marks the element as dirty for color updates and triggers a frame update.
         * 
         * @param color The RGB color to set as the focus text color.
         */
        void Set_Focus_Text_Color(RGB color);

        /**
         * @brief Retrieves the focus text color of the element.
         * 
         * This function returns the RGB value of the text color when the element is focused
         * from the element's style.
         * 
         * @return The RGB color of the element's focus text.
         */
        RGB Get_Focus_Text_Color();

        /**
         * @brief Sets the focus border background color of the element.
         * 
         * This function sets the focus border background color of the element to the specified RGB value.
         * It marks the element as dirty for color updates and triggers a frame update.
         * 
         * @param color The RGB color to set as the focus border background color.
         */
        void Set_Focus_Border_Background_Color(RGB color);

        /**
         * @brief Retrieves the focus border background color of the element.
         * 
         * This function returns the RGB value of the focus border background color
         * from the element's style.
         * 
         * @return The RGB color of the element's focus border background.
         */
        RGB Get_Focus_Border_Background_Color();

        /**
         * @brief Sets the alignment of the element.
         * 
         * This function sets the alignment of the element to the specified ALIGN value.
         * 
         * @param Align The alignment value to set for the element.
         */
        void Set_Align(ALIGN a);

        /**
         * @brief Sets the alignment of the element.
         * 
         * This function sets the alignment of the element to the specified ALIGN value.
         * 
         * @param Align The alignment value to set for the element.
         */
        ALIGN Get_Align();

        /**
         * @brief Sets the flow priority of the element.
         * 
         * This function sets the flow priority of the element to the specified DIRECTION value.
         * The flow priority determines how the element will be aligned in its parent when the parent is a flow layout.
         * 
         * @param Priority The flow priority value to set for the element.
         */
        void Set_Flow_Priority(DIRECTION d);

        /**
         * @brief Retrieves the flow priority of the element.
         * 
         * This function returns the DIRECTION value that was previously set with Set_Flow_Priority.
         * The flow priority determines how the element will be aligned in its parent when the parent is a flow layout.
         * 
         * @return The flow priority value of the element.
         */
        DIRECTION Get_Flow_Priority();

        /**
         * @brief Sets whether the element will wrap its contents to the next line when it hits the edge of the screen.
         * 
         * This function sets whether the element will wrap its contents to the next line when it hits the edge of the screen.
         * If true, the element will wrap its contents to the next line when it hits the edge of the screen.
         * If false, the element will not wrap its contents to the next line when it hits the edge of the screen.
         * 
         * @param Wrap The value to set for whether the element will wrap its contents to the next line.
         */
        void Set_Wrap(bool w);

        /**
         * @brief Retrieves the wrap setting of the element.
         * 
         * This function returns whether the element will wrap its contents to the next line
         * when it reaches the edge of the screen.
         * 
         * @return True if the element will wrap its contents, false otherwise.
         */
        bool Get_Wrap();

        /**
         * @brief Sets whether the element is allowed to dynamically resize.
         * 
         * This function enables or disables the ability of the element to 
         * adjust its size based on its content.
         * 
         * @param True A boolean indicating whether dynamic resizing is allowed.
         */
        void Allow_Dynamic_Size(bool True);

        /**
         * @brief Checks whether the element is allowed to dynamically resize.
         * 
         * This function checks the Allow_Dynamic_Size property in the element's style
         * and returns its value.
         * 
         * @return True if the element is allowed to dynamically resize, false otherwise.
         */
        bool Is_Dynamic_Size_Allowed();

        /**
         * @brief Sets whether the element allows overflow.
         * 
         * This function enables or disables the overflow property of the element,
         * allowing child elements to exceed the parent's dimensions without resizing it.
         * 
         * @param True A boolean indicating whether overflow is allowed.
         */ 
        void Allow_Overflow(bool True);

        /**
         * @brief Checks whether the element allows overflow.
         * 
         * This function checks the Allow_Overflow property in the element's style
         * and returns its value.
         * 
         * @return True if the element allows overflow, false otherwise.
         */
        bool Is_Overflow_Allowed();
        
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
        static std::pair<std::pair<unsigned int, unsigned int> ,std::pair<std::pair<unsigned int, unsigned int>, std::pair<unsigned int, unsigned int>>> Get_Fitting_Area(GGUI::Element* Parent, GGUI::Element* Child);
                
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
        void Compute_Dynamic_Size();

        /**
         * @brief Renders the element and its children into the Render_Buffer nested buffer of the window.
         * @details This function processes the element to generate a vector of UTF objects representing the current state.
         * It handles different stains such as CLASS, STRETCH, COLOR, and EDGE to ensure the element is rendered correctly.
         * @return A vector of UTF objects representing the rendered element and its children.
         */
        virtual std::vector<GGUI::UTF>& Render();

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
        virtual void Update_Parent(Element* New_Element);

        /**
         * @brief Add the border of the window to the rendered string.
         *
         * @param w The window to add the border for.
         * @param Result The string to add the border to.
         */
        virtual void Add_Overhead(Element* w, std::vector<UTF>& Result);

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
        virtual void Apply_Colors(Element* w, std::vector<UTF>& Result);

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
        virtual bool Resize_To([[maybe_unused]] Element* parent){
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
        void Compute_Alpha_To_Nesting(GGUI::UTF& Dest, GGUI::UTF Source);

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
        void Nest_Element(Element* Parent, Element* Child, std::vector<UTF>& Parent_Buffer, std::vector<UTF>& Child_Buffer);

        /**
         * @brief Returns a map of the custom border symbols for the given element.
         * @param e The element to get the custom border map for.
         * @return A map of the custom border symbols where the key is the bit mask of the border and the value is the corresponding symbol.
         */
        std::unordered_map<unsigned int, const char*> Get_Custom_Border_Map(Element* e);

        /**
         * @brief Returns a map of the custom border symbols for the given border style.
         * The map key is the bit mask of the border and the value is the corresponding symbol.
         * @param custom_border_style The custom border style to get the map for.
         * @return A map of the custom border symbols.
         */
        std::unordered_map<unsigned int, const char*> Get_Custom_Border_Map(GGUI::styled_border custom_border_style);

        /**
         * @brief Sets the custom border style for the element.
         * @details This function sets the custom border style for the element, marks the element's edges as dirty, and ensures that the border is visible.
         * @param style The custom border style to set.
         */
        void Set_Custom_Border_Style(GGUI::styled_border style);

        /**
         * @brief Gets the custom border style of the element.
         * @return The custom border style of the element.
         */
        GGUI::styled_border Get_Custom_Border_Style();

        /**
         * @brief Posts a process that handles the intersection of borders between two elements and their parent.
         * @details This function posts a process that handles the intersection of borders between two elements and their parent.
         *          The process calculates the intersection points of the borders and then constructs a bit mask that portraits the connections the middle point has.
         *          With the calculated bit mask it can fetch from the 'SYMBOLS::Border_Identifiers' the right border string.
         * @param A The first element.
         * @param B The second element.
         * @param Parent_Buffer The buffer of the parent element.
         */
        void Post_Process_Borders(Element* A, Element* B, std::vector<UTF>& Parent_Buffer);

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
        std::pair<RGB, RGB>  Compose_All_Text_RGB_Values();

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
        RGB  Compose_Text_RGB_Values();
        
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
        RGB  Compose_Background_RGB_Values();

        /**
         * @brief Composes the RGB values of the border color and background color of the element.
         * @details This function will return the RGB values of the border color and background color of the element.
         * If the element is focused, the function will return the RGB values of the focused border color and background color.
         * If the element is hovered, the function will return the RGB values of the hovered border color and background color.
         * Otherwise, the function will return the RGB values of the normal border color and background color.
         * @return A pair of RGB values representing the border color and background color of the element.
         */
        std::pair<RGB, RGB>  Compose_All_Border_RGB_Values();

        /**
         * @brief Returns the name of the element.
         * @details This function returns a string that represents the name of the element.
         *          The name is constructed by concatenating the name of the element with the 
         *          class name of the element, separated by a "<" and a ">".
         * @return The name of the element.
         */
        virtual std::string Get_Name() const {
            return "Element<" + Name + ">";
        }

        /**
         * @brief Set the name of the element.
         * @details This function sets the name of the element and stores it in the global Element_Names map.
         * @param name The name of the element.
         */
        void Set_Name(std::string name);

        /**
         * @brief Removes the element from the parent element.
         * @details This function first checks if the element has a parent.
         *          If the element has a parent, it calls the parent's Remove() function to remove the element from the parent.
         *          If the element does not have a parent, it prints an error message to the console.
         *          The function does not update the frame, so it is the caller's responsibility to update the frame after calling this function.
         */
        void Remove();

        /**
         * @brief A function that registers a lambda to be executed when the element is clicked.
         * @details The lambda is given a pointer to the Event object that triggered the call.
         *          The lambda is expected to return true if it was successful and false if it failed.
         * @param action The lambda to be called when the element is clicked.
         */
        void On_Click(std::function<bool(GGUI::Event*)> action);

        /**
         * @brief A function that registers a lambda to be executed when the element is interacted with in any way.
         * @details The lambda is given a pointer to the Event object that triggered the call.
         *          The lambda is expected to return true if it was successful and false if it failed.
         * @param criteria The criteria to check for when deciding whether to execute the lambda.
         * @param action The lambda to be called when the element is interacted with.
         * @param GLOBAL Whether the lambda should be executed even if the element is not under the mouse.
         */
        void On(unsigned long long criteria, std::function<bool(GGUI::Event*)> action, bool GLOBAL = false);

        /**
         * @brief Retrieves an element by name.
         * @details This function takes a string argument representing the name of the element
         *          and returns a pointer to the element if it exists in the global Element_Names map.
         * @param name The name of the element to retrieve.
         * @return A pointer to the element if it exists; otherwise, nullptr.
         */
        Element* Get_Element(std::string name);

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
        std::vector<T*> Get_Elements(){
            std::vector<T*> result;

            // Check if the element in question is of the same type as the template T.
            if (typeid(*this) == typeid(T)){
                result.push_back((T*)this);
            }

            // Go through the child AST, and check if any of the child elements are of the same type as the template T.
            for (auto e : Style->Childs){
                // Recursively go through the child AST, and check if any of the child elements are of the same type as the template T.
                std::vector<T*> child_result = e->Get_Elements<T>();

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
        std::vector<Element*> Get_All_Nested_Elements(bool Show_Hidden = false) {
            std::vector<Element*> result;

            // If the element is not visible and hidden elements should not be shown, return an empty vector.
            if (!Show && !Show_Hidden)
                return {};

            // Add the current element to the result vector.
            result.push_back(this);

            // Recursively retrieve all nested elements from child elements.
            for (auto e : Get_Childs()) {
                std::vector<Element*> child_result = e->Get_All_Nested_Elements(Show_Hidden);
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
        virtual void Scroll_Up() {}

        /**
         * @brief Default virtual function for scrolling down.
         * @details By default, elements do not have inherent scrolling abilities.
         *          This function is used as a base for other elements to implement their own scrolling.
         */
        virtual void Scroll_Down() {}

        /**
         * @brief Reorders child elements based on their z-position.
         * @details This function sorts the child elements of the current element by their z-coordinate
         *          in ascending order, so that elements with a higher z-coordinate appear later in the list.
         */
        void Re_Order_Childs();

        /**
         * @brief Focuses the element.
         * @details This function updates the global focus information by setting the mouse position to the element's position and updating the focused element.
         */
        void Focus();

        /**
         * @brief Adds a handler function to the state handlers map.
         * @details This function takes a state and a handler function as arguments.
         *          The handler function is stored in the State_Handlers map with the given state as the key.
         * @param s The state for which the handler should be executed.
         * @param job The handler function to be executed when the given state is triggered.
         */
        void On_State(State s, std::function<void()> job);

        /**
         * @brief Checks if the element needs postprocessing.
         * @details This function checks if the element needs postprocessing by checking if the element has a shadow or is transparent.
         * @return True if the element needs postprocessing; otherwise, false.
         */
        bool Has_Postprocessing_To_Do();

        /**
         * @brief Process the shadow of the element.
         * @details This function processes the shadow of the element by calculating the new buffer size and creating a new buffer with the shadow.
         *          It then offsets the shadow box buffer by the direction and blends it with the original buffer.
         * @param Current_Buffer The buffer to be processed.
         */
        void Process_Shadow(std::vector<GGUI::UTF>& Current_Buffer);

        /**
         * @brief Applies the opacity of the element to the given buffer.
         * @details This function will iterate over the given buffer and apply the opacity of the element to the background and foreground of each UTF character.
         * @param Current_Buffer The buffer to be processed.
         */
        void Process_Opacity(std::vector<GGUI::UTF>& Current_Buffer);

        /**
         * @brief
         * This function performs postprocessing on the rendered buffer of the element.
         * It applies the shadow, and then the opacity to the rendered buffer.
         * @return The postprocessed buffer.
         */
        virtual std::vector<GGUI::UTF>& Postprocess();

        // Customization helper function
        //-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_
        
        /**
         * @brief Adds a stain to the element.
         * @details This function adds the given stain to the element's stains list.
         *          The stains list is used to keep track of which properties of the element need to be re-evaluated.
         *          The function takes a STAIN_TYPE as a parameter and adds it to the list of stains.
         * @param s The stain to be added.
         */
        void Add_Stain(STAIN_TYPE s){
            Dirty.Dirty(s);
        }

        /**
         * @brief
         * This function determines if the given element is a direct child of this element (in the DOM tree),
         * and if it is visible on the screen (does not go out of bounds of the parent element).
         * @param other Child element to check
         * @return True if the child element is visible within the bounds of the parent.
         */
        bool Child_Is_Shown(Element* other);
    };
}

#endif
