#include "Button.h"
#include "HTML.h"
#include "../Core/Renderer.h"

namespace GGUI{

    /**
     * @brief Constructs a Button element with specified text, action, and styling.
     * @param text The text to display on the button.
     * @param press The function to call when the button is pressed.
     * @param s The styling for the button.
     */
    Button::Button(std::string text, std::function<void (Button* This)> press, Styling s) : Element(s) {
        // Set default behavior for button press
        Default_Button_Behaviour(press);

        // Set the name of the button element
        Set_Name(text);

        // Allow the button to resize dynamically
        Allow_Dynamic_Size(true);

        // Enable border for the button
        Show_Border(true);

        // Add a centered text field as a child element
        Add_Child(new Text_Field(text, Styling(align(ALIGN::CENTER))));
    }

}