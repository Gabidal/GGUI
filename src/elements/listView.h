#ifndef _LIST_VIEW_H_
#define _LIST_VIEW_H_

#include "element.h"

#include "../core/utils/superString.h"
#include "../core/utils/constants.h"
#include "../core/utils/color.h"
#include "../core/utils/units.h"
#include "../core/utils/event.h"
#include "../core/utils/style.h"

namespace GGUI{
    class listView : public element{
    public:
        //We can always assume that the list starts from the upper left corner, right?
        element* Last_Child = new element(styling(position(0, 0) | width(0) | height(0)));

        /**
         * @brief Default constructor for List_View.
         * 
         * This constructor calls the default constructor of Element and sets the Allow_Dynamic_Size property to true.
         */
        listView() : element(){ allowDynamicSize(true); }

        /**
         * @brief Constructor for List_View.
         * 
         * This constructor calls the Element constructor with the given style and then sets the Allow_Dynamic_Size property to true.
         * This is so that the list view can automatically resize itself based on the elements it contains.
         * 
         * @param s The style for the list view.
         */
        listView(styling s, bool Embed_Styles_On_Construct = false) : element(s, Embed_Styles_On_Construct){ allowDynamicSize(true); }

        /**
         * @brief Destructor for the List_View class.
         *
         * This destructor is responsible for properly deallocating all the memory
         * allocated by the List_View object, including its child elements.
         */
        ~listView() override {
            // Delete all child elements to avoid memory leaks.
            for (element* e : Style->Childs) {
                delete e;
            }

            // Call the base class destructor to ensure all parent class resources are cleaned up.
            element::~element();
        }

        //End of user constructors.

        IVector3 getDimensionLimit();

        /**
         * @brief Adds a child element to the list view.
         * @details This function adds a child element to the list view and manages the positioning and sizing
         *          of the child element within the list view. It takes into account the list's flow direction,
         *          border offsets, and dynamic sizing capabilities.
         * @param e The child element to be added.
         */
        void addChild(element* e) override;
        
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
        void calculateChildsHitboxes(unsigned int Starting_Offset = 0) override;

        /**
         * @brief Gets the name of the list view.
         * @details This function returns the name of the list view in the format "List_View<Name>".
         * @return The name of the list view.
         */
        std::string getName() const override;

        /**
         * @brief Removes a child element from the list view.
         * @param remove The child element to be removed.
         * @return true if the element was successfully removed, false if not.
         *
         * This function removes a child element from the list view and updates the position of all elements following the removed element.
         * It also recalculates the width and height of the list view and updates the dimensions of the list view if it is dynamically sized.
         */
        bool remove(element* e) override;

        /**
         * @brief Sets the flow direction of the list view.
         * @details This function sets the flow priority of the list view to the specified direction.
         *          The flow direction determines how the child elements are arranged within the list view.
         * @param gd The direction to set as the flow priority.
         */
        void setFlowDirection(DIRECTION gd){
            Style->Flow_Priority = gd;
        }

        /**
         * @brief Gets the current flow direction of the list view.
         * @details This function returns the current flow direction of the list view.
         * @return The flow direction of the list view.
         */
        DIRECTION getFlowDirection(){
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
        T* get(int index){
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
        element* safeMove() override {
            return new listView();
        }
    };

    class scrollView : public element{
    protected:
        unsigned int Scroll_Index = 0;  // Render based on the offset of the scroll_index by flow direction.
    public:

        /**
         * @brief Default constructor for the scrollView class.
         * 
         * This constructor initializes a scrollView object by calling the base class
         * element's default constructor.
         */
        scrollView() : element() {}

        /**
         * @brief Constructor for the Scroll_View class.
         * @details This constructor initializes a Scroll_View object with the specified styling.
         * @param s The styling to be applied to the Scroll_View.
         * @param Embed_Styles_On_Construct If true, the styling will be embedded into the Scroll_View's style. Only use if you know what you're doing!!!
         */
        scrollView(styling s, bool Embed_Styles_On_Construct = false) : element(s, Embed_Styles_On_Construct) {}

        /**
         * @brief Constructor for the Scroll_View class.
         * @details This constructor initializes a Scroll_View object with a reference to a List_View object.
         * @param container The List_View object to be used as the container for the Scroll_View.
         */
        scrollView(listView& container);

        /**
         * @brief Adds a child element to the Scroll_View.
         * @details This function adds a child element to the Scroll_View and marks the Scroll_View as dirty with the DEEP stain.
         * @param e The child element to be added.
         */
        void addChild(element* e) override;

        /**
         * @brief Enables or disables scrolling for the Scroll_View.
         * @details This function updates the scrolling capability of the Scroll_View.
         *          If scrolling is enabled, it ensures that scrolling events are registered.
         * @param allow A boolean indicating whether to enable or disable scrolling.
         */
        void allowScrolling(bool allow);
    
        /**
         * @brief Checks if the scrolling is enabled for the Scroll_View.
         * @details This function checks the value of the Allow_Scrolling property of the Scroll_View's styling.
         * @return A boolean indicating whether the scrolling is enabled for the Scroll_View.
         */
        bool isScrollingEnabled(){
            return Style->Allow_Scrolling.Value;
        }

        /**
         * @brief Scrolls the view up by one index.
         * @details Decreases the scroll index if it is greater than zero and updates the container's position based on the growth direction.
         * Marks the view as dirty for a deep update.
         */
        void scrollUp() override;

        /**
         * @brief Scrolls the view down by one index.
         * @details Increases the scroll index by one and updates the container's position based on the growth direction.
         * Marks the view as dirty for a deep update.
         */
        void scrollDown() override;

        /**
         * @brief Removes a child element from the scroll view.
         * @details This function forwards the request to the Remove(Element* remove) function of the container.
         * @param remove The element to be removed.
         * @return true if the element was successfully removed, false if not.
         */
        bool remove(element* e) override;

        /**
         * @brief Gets the name of the scroll view.
         * @details This function returns the name of the scroll view.
         * @return The name of the scroll view.
         */
        std::string getName() const override;

        /**
         * @brief Sets the growth direction of the scroll view.
         * @details This function forwards the request to the Set_Flow_Direction(DIRECTION gd) function of the container.
         * @param gd The direction value to set as the growth direction.
         */
        void setGrowthDirection(DIRECTION gd){
            ((listView*)Style->Childs[0])->setFlowDirection(gd);
        }

        /**
         * @brief Gets the current growth direction of the scroll view.
         * @details This function retrieves the current growth direction of the scroll view.
         * @return The current growth direction of the scroll view.
         */
        DIRECTION getGrowthDirection(){
            return ((listView*)Style->Childs[0])->getFlowDirection();
        }

        /**
         * @brief Gets a child element from the scroll view by its index.
         * @details This function forwards the request to the Get(int index) function of the container.
         * @param index The index of the child element to retrieve.
         * @return The child element at the specified index, or nullptr if the index is out of range.
         */
        template<typename  T>
        T* get(int index){
            return ((listView*)Style->Childs[0])->get<T>(index);
        }

        /**
         * @brief Gets the container of the scroll view.
         * @details This function retrieves the container of the scroll view, which is a List_View.
         * @return The container of the scroll view.
         */
        listView* getContainer(){
            return (listView*)Style->Childs[0];
        }
        
        /**
         * @brief Safely moves the current element to a new scrollView element.
         * 
         * This function overrides the safeMove method from the base class and 
         * creates a new instance of the scrollView element.
         * 
         * @return A pointer to the newly created scrollView element.
         */
        element* safeMove() override {
            return new scrollView();
        }
    };
}

#endif