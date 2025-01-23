#include "button.h"
#include "HTML.h"
#include "../core/renderer.h"

namespace GGUI{

    /**
     * @brief Constructs a Button element with specified text, action, and styling.
     * @param text The text to display on the button.
     * @param press The function to call when the button is pressed.
     * @param s The styling for the button.
     */
    Button::Button(Styling s, bool Embed_Styles_On_Construct) : Element(s, Embed_Styles_On_Construct) {
        // Allow the button to resize dynamically
        Allow_Dynamic_Size(true);

        // Enable border for the button
        Show_Border(true);

        // Add a centered text field as a child element
        Add_Child(new Text_Field(Styling(
            align(ALIGN::CENTER) |
            // text(Text.c_str()) |     // The text is given from the Styling s.
            width(1.0f) | height(1.0f)
        )));
    }

}