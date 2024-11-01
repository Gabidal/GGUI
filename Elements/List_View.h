#ifndef _LIST_VIEW_H_
#define _LIST_VIEW_H_

#include "Element.h"

#include "../Core/Utils/Super_String.h"
#include "../Core/Utils/Constants.h"
#include "../Core/Utils/Color.h"
#include "../Core/Utils/Units.h"
#include "../Core/Utils/Event.h"
#include "../Core/Utils/Style.h"

namespace GGUI{
    class List_View : public Element{
    public:
        //We can always assume that the list starts from the upper left corner, right?
        Element* Last_Child = new Element(Styling(position(0, 0) | width(0) | height(0)));

        /**
         * @brief Default constructor for List_View.
         * 
         * This constructor calls the default constructor of Element and sets the Allow_Dynamic_Size property to true.
         */
        List_View() : Element(){ Allow_Dynamic_Size(true); }

        /**
         * @brief Constructor for List_View.
         * 
         * This constructor calls the Element constructor with the given style and then sets the Allow_Dynamic_Size property to true.
         * This is so that the list view can automatically resize itself based on the elements it contains.
         * 
         * @param s The style for the list view.
         */
        List_View(Styling s) : Element(s){ Allow_Dynamic_Size(true); }

        /**
         * @brief Destructor for the List_View class.
         *
         * This destructor is responsible for properly deallocating all the memory
         * allocated by the List_View object, including its child elements.
         */
        ~List_View() override {
            // Delete all child elements to avoid memory leaks.
            for (Element* e : Style->Childs) {
                delete e;
            }

            // Call the base class destructor to ensure all parent class resources are cleaned up.
            Element::~Element();
        }

        /**
         * @brief Handles the OR operator for List_View.
         * @details This function is called when the OR operator is used between a List_View and another Element.
         *          It simply adds the other element to the list view.
         * @param other The other element to handle the OR operator with.
         * @return The current element (i.e. the List_View).
         */
        Element* Handle_Or_Operator(Element* other) override{
            Add_Child(other);
            return this;
        }

        //End of user constructors.

        /**
         * @brief Adds a child element to the list view.
         * @details This function adds a child element to the list view and manages the positioning and sizing
         *          of the child element within the list view. It takes into account the list's flow direction,
         *          border offsets, and dynamic sizing capabilities.
         * @param e The child element to be added.
         */
        void Add_Child(Element* e) override;
        
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
        void Calculate_Childs_Hitboxes(unsigned int Starting_Offset = 0) override;

        /**
         * @brief Gets the name of the list view.
         * @details This function returns the name of the list view in the format "List_View<Name>".
         * @return The name of the list view.
         */
        std::string Get_Name() const override;

        /**
         * @brief Removes a child element from the list view.
         * @param remove The child element to be removed.
         * @return true if the element was successfully removed, false if not.
         *
         * This function removes a child element from the list view and updates the position of all elements following the removed element.
         * It also recalculates the width and height of the list view and updates the dimensions of the list view if it is dynamically sized.
         */
        bool Remove(Element* e) override;

        /**
         * @brief Sets the flow direction of the list view.
         * @details This function sets the flow priority of the list view to the specified direction.
         *          The flow direction determines how the child elements are arranged within the list view.
         * @param gd The direction to set as the flow priority.
         */
        void Set_Flow_Direction(DIRECTION gd){
            Style->Flow_Priority = gd;
        }

        /**
         * @brief Gets the current flow direction of the list view.
         * @details This function returns the current flow direction of the list view.
         * @return The flow direction of the list view.
         */
        DIRECTION Get_Flow_Direction(){
            return (DIRECTION)Style->Flow_Priority.Value;
        }

        /**
         * @brief Gets a child element from the list view by its index.
         * @details This function returns a pointer to the child element at the specified index.
         *          The index is checked to be within the range of the child array.
         * @param index The index of the child element to retrieve.
         * @return The child element at the specified index, or nullptr if the index is out of range.
         */
        template<typename  T>
        T* Get(int index){
            if (index > (signed)Style->Childs.size() - 1)
                return nullptr;

            if (index < 0)
                index = (signed)Style->Childs.size() + index - 1;

            return (T*)this->Style->Childs[index];
        }

        /**
         * @brief Creates a deep copy of the List_View object.
         * @details This function creates a new List_View object and copies all the data from the current List_View object to the new one.
         * @return A pointer to the new List_View object.
         */
        Element* Safe_Move() override {
            List_View* new_List_View = new List_View();
            *new_List_View = *(List_View*)this;

            return new_List_View;
        }
    };

    class Scroll_View : public Element{
    protected:
        unsigned int Scroll_Index = 0;  // Render based on the offset of the scroll_index by flow direction.
    public:

        /**
         * @brief Constructor for the Scroll_View class.
         * @details This constructor initializes a Scroll_View object with the specified styling.
         * @param s The styling to be applied to the Scroll_View.
         */
        Scroll_View(Styling s) : Element(s) {}

        /**
         * @brief Constructor for the Scroll_View class.
         * @details This constructor initializes a Scroll_View object with a reference to a List_View object.
         * @param container The List_View object to be used as the container for the Scroll_View.
         */
        Scroll_View(List_View& container);

        /**
         * @brief Adds a child element to the Scroll_View.
         * @details This function adds a child element to the Scroll_View and marks the Scroll_View as dirty with the DEEP stain.
         * @param e The child element to be added.
         */
        void Add_Child(Element* e) override;

        /**
         * @brief Enables or disables scrolling for the Scroll_View.
         * @details This function updates the scrolling capability of the Scroll_View.
         *          If scrolling is enabled, it ensures that scrolling events are registered.
         * @param allow A boolean indicating whether to enable or disable scrolling.
         */
        void Allow_Scrolling(bool allow);
    
        /**
         * @brief Checks if the scrolling is enabled for the Scroll_View.
         * @details This function checks the value of the Allow_Scrolling property of the Scroll_View's styling.
         * @return A boolean indicating whether the scrolling is enabled for the Scroll_View.
         */
        bool Is_Scrolling_Enabled(){
            return Style->Allow_Scrolling.Value;
        }

        /**
         * @brief Scrolls the view up by one index.
         * @details Decreases the scroll index if it is greater than zero and updates the container's position based on the growth direction.
         * Marks the view as dirty for a deep update.
         */
        void Scroll_Up() override;

        /**
         * @brief Scrolls the view down by one index.
         * @details Increases the scroll index by one and updates the container's position based on the growth direction.
         * Marks the view as dirty for a deep update.
         */
        void Scroll_Down() override;

        /**
         * @brief Removes a child element from the scroll view.
         * @details This function forwards the request to the Remove(Element* remove) function of the container.
         * @param remove The element to be removed.
         * @return true if the element was successfully removed, false if not.
         */
        bool Remove(Element* e) override;

        /**
         * @brief Gets the name of the scroll view.
         * @details This function returns the name of the scroll view.
         * @return The name of the scroll view.
         */
        std::string Get_Name() const override;

        /**
         * @brief Sets the growth direction of the scroll view.
         * @details This function forwards the request to the Set_Flow_Direction(DIRECTION gd) function of the container.
         * @param gd The direction value to set as the growth direction.
         */
        void Set_Growth_Direction(DIRECTION gd){
            ((List_View*)Style->Childs[0])->Set_Flow_Direction(gd);
        }

        /**
         * @brief Gets the current growth direction of the scroll view.
         * @details This function retrieves the current growth direction of the scroll view.
         * @return The current growth direction of the scroll view.
         */
        DIRECTION Get_Growth_Direction(){
            return ((List_View*)Style->Childs[0])->Get_Flow_Direction();
        }

        /**
         * @brief Gets a child element from the scroll view by its index.
         * @details This function forwards the request to the Get(int index) function of the container.
         * @param index The index of the child element to retrieve.
         * @return The child element at the specified index, or nullptr if the index is out of range.
         */
        template<typename  T>
        T* Get(int index){
            return ((List_View*)Style->Childs[0])->Get<T>(index);
        }

        /**
         * @brief Gets the container of the scroll view.
         * @details This function retrieves the container of the scroll view, which is a List_View.
         * @return The container of the scroll view.
         */
        List_View* Get_Container(){
            return (List_View*)Style->Childs[0];
        }
    
    };
}

#endif