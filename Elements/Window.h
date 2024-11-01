#ifndef _WINDOW_H_
#define _WINDOW_H_

#include "Element.h"

#include "../Core/Utils/Super_String.h"
#include "../Core/Utils/Constants.h"
#include "../Core/Utils/Color.h"
#include "../Core/Utils/Units.h"
#include "../Core/Utils/Event.h"
#include "../Core/Utils/Style.h"

//GGUI uses the ANSI escape code
//https://en.wikipedia.org/wiki/ANSI_escape_code
namespace GGUI{
    class Window : public Element{
        std::string Title = "";  //if this is empty then no title

        RGB Before_Hiding_Border_Color = COLOR::WHITE;
        RGB Before_Hiding_Border_Background_Color = COLOR::BLACK;
        bool Has_Hidden_Borders = false;
    public:
    
        /**
         * @brief A window element that wraps a console window.
         * This element is capable of modifying the window's title, border visibility, and colors.
         * @param title The title string to be displayed in the window's title bar.
         * @param s The Styling object to be used for the window.
         */
        Window(std::string title = "", Styling s = STYLES::CONSTANTS::Default);

        /**
         * @brief Destructor for the Window class.
         *
         * This destructor is responsible for cleaning up the memory allocated by the Window object.
         * It calls the base class destructor to ensure all parent class resources are cleaned up.
         */
        ~Window() override{
            // call the base destructor.
            Element::~Element();
        }

        //End of user constructors.

        /**
         * @brief Updates the colors of the hidden borders for the window.
         * This function prioritizes border color variants if they are available;
         * otherwise, it falls back to text colors or default values.
         */
        void Update_Hidden_Border_Colors();

        /**
         * @brief Sets the title of the window and updates border visibility and colors accordingly.
         * 
         * This function sets the window's title and ensures that the border is shown if the title is not empty.
         * If the window previously had hidden borders, it updates the border colors based on the background color.
         * 
         * @param t The new title for the window.
         */
        void Set_Title(std::string t);

        /**
         * @brief Returns the title of the window.
         * 
         * @return The title of the window as a string.
         */
        std::string Get_Title();
        
        /**
         * @brief Adds the border of the window to the rendered string.
         * 
         * @param w The window to add the border for.
         * @param Result The string to add the border to.
         */
        void Add_Overhead(Element* w, std::vector<UTF>& Result) override;

        /**
         * @brief Gets the name of the window.
         * 
         * @return The name of the window as a string.
         */
        std::string Get_Name() const override;

        /**
         * @brief Shows or hides the window's border.
         * @details This function toggles the border visibility of the window.
         *          If the state has changed, it updates the border enabled state, marks the element as dirty for border changes, and updates the frame.
         * @param b The desired state of the border visibility.
         */
        void Show_Border(bool state) override;

        /**
         * @brief Shows or hides the window's border.
         * @details This function toggles the border visibility of the window.
         *          If the state has changed, it updates the border enabled state, marks the element as dirty for border changes, and updates the frame.
         * @param b The desired state of the border visibility.
         * @param Previous_State The current state of the border visibility.
         */
        void Show_Border(bool state, bool previus_state) override;

        /**
         * @brief Sets the background color of the window.
         * @details This function sets the background color of the window to the specified RGB value.
         *          It marks the element as dirty for color updates and triggers a frame update.
         * @param color The RGB color to set as the background color.
         */
        void Set_Background_Color(RGB color) override;

        /**
         * @brief Sets the text color of the window.
         * @details This function sets the text color of the window to the specified RGB value.
         *          It marks the element as dirty for color updates and triggers a frame update.
         * @param color The RGB color to set as the text color.
         */
        void Set_Text_Color(RGB color) override;

        /**
         * @brief Sets the background color of the window's border.
         * @details This function sets the background color of the window's border to the specified RGB value.
         *          It marks the element as dirty for color updates and triggers a frame update.
         * @param color The RGB color to set as the background color of the window's border.
         */
        void Set_Border_Background_Color(RGB color) override;

        /**
         * @brief Sets the color of the window's border.
         * @details This function sets the color of the window's border to the specified RGB value.
         *          It marks the element as dirty for color updates and triggers a frame update.
         * @param color The RGB color to set as the border color.
         */
        void Set_Border_Color(RGB color) override;

        /**
         * @brief Creates a deep copy of the Window object.
         * @details This function creates a new Window object and copies all the data from the current Window object to the new one.
         * @return A pointer to the new Window object.
         */
        Element* Safe_Move() override {
            // Allocate memory for a new Window object
            Window* new_Window = new Window();

            // Copy the data from the current Window to the new Window
            *new_Window = *(Window*)this;

            // Return a pointer to the new Window object
            return new_Window;
        }
    };
}

#endif