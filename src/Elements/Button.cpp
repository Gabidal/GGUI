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
    button::button(styling s, bool Embed_Styles_On_Construct) : element(s, Embed_Styles_On_Construct) {
        // Allow the button to resize dynamically
        allowDynamicSize(true);

        // Enable border for the button
        showBorder(true);

        // Add a centered text field as a child element
        addChild(new textField(styling(
            align(ALIGN::CENTER) |
            // text(Text.c_str()) |     // The text is given from the Styling s.
            width(1.0f) | height(1.0f)
        )));
    }

}