#ifndef _TEXT_FIELD_H_
#define _TEXT_FIELD_H_

#include "element.h"

#include "../core/utils/superString.h"
#include "../core/utils/constants.h"
#include "../core/utils/color.h"
#include "../core/utils/units.h"
#include "../core/utils/event.h"
#include "../core/utils/style.h"

namespace GGUI{
    class textField : public element{
    protected:
        std::string Text = "";

        // This will hold the text by lines, and does not re-allocate memory for whole text, only for indicies.
        std::vector<Compact_String> Text_Cache; 

        /**
         * @brief Updates the text cache list by newlines, and if no found then set the Text as the zeroth index.
         * @details This function will also determine the longest line length and store it in the class.
         * @note This function will also check if the lines can be appended to the previous line or not.
         */
        void updateTextCache();
    public:

        /**
         * @brief Constructor for the Text_Field class.
         * @details This constructor takes an optional Styling parameter, and an optional string parameter.
         *          If the string parameter is not given, it defaults to an empty string.
         *          If the Styling parameter is not given, it defaults to a Styling object with
         *          the default values for the Height and Width properties.
         *          The Styling parameter is used to set the style of the Text_Field object.
         * @param text The text to be displayed in the Text_Field object.
         * @param s The Styling object to use for the Text_Field object.
         * @param Embed_Styles_On_Construct If true, the styling will be embedded into the Text_Field's style. Only use if you know what you're doing!!!
         */
        textField(STYLING_INTERNAL::style_base& s = STYLES::CONSTANTS::Default, bool Embed_Styles_On_Construct = false) : element(s, Embed_Styles_On_Construct){

            // Since Styling Height and Width are defaulted to 1, we can use this one row to reserve for one line.
            Text_Cache.reserve(getHeight());

            if (getWidth() == 1 && getHeight() == 1){
                allowDynamicSize(true);
            }

            // Update the text cache list by newlines, and if no found then set the Text as the zeroth index.
            if (Embed_Styles_On_Construct)
                updateTextCache();
        }
        
        textField(STYLING_INTERNAL::style_base&& s, bool Embed_Styles_On_Construct = false) : textField(s, Embed_Styles_On_Construct){}

        /**
         * @brief Sets the size of the text field to fill its parent element.
         * @details The function first checks if dynamic sizing is allowed for
         *          the text field and its parent. It then calculates the new
         *          width and height based on the parent's dimensions and the
         *          text size. If the parent allows dynamic sizing, it stretches
         *          to accommodate the text; otherwise, it constrains the size
         *          within the parent's boundaries.
         */
        void setSizeToFillParent();

        /**
         * @brief Sets the text of the text field.
         * @details This function first stops the GGUI engine, then sets the text with a space character added to the beginning, and finally updates the text field's dimensions to fit the new text. The text is then reset in the Render_Buffer nested buffer of the window.
         * @param text The new text for the text field.
         */
        void setText(std::string text);

        /**
         * @brief Gets the text of the text field.
         * @details This function returns the string containing the text of the text field.
         * @return The text of the text field as a string.
         */
        std::string getText(){
            return Text;
        }

        /**
         * @brief Renders the text field into the Render_Buffer.
         * @details This function processes the text field to generate a vector of UTF objects representing the current state.
         * It handles different stains such as CLASS, STRETCH, COLOR, EDGE, and DEEP to ensure the text field is rendered correctly.
         * @return A vector of UTF objects representing the rendered text field.
         */
        std::vector<GGUI::UTF>& render() override;

        /**
         * @brief Aligns text to the left within the text field.
         * @param Result A vector of UTF objects to store the aligned text.
         * @details This function iterates over each line in the Text_Cache and aligns them to the left side 
         *          of the text field. The function respects the maximum height and width of the text field 
         *          and handles overflow according to the Style settings.
         */
        void alignTextLeft(std::vector<UTF>& Result);
        
        /**
         * @brief Aligns text to the right within the text field.
         * @param Result A vector of UTF objects to store the aligned text.
         * @details This function iterates over each line in the Text_Cache and aligns them to the right side
         *          of the text field. The function respects the maximum height and width of the text field
         *          and handles overflow according to the Style settings.
         */
        void alignTextRight(std::vector<UTF>& Result);
        
        /**
         * @brief Aligns text to the center within the text field.
         * @param Result A vector of UTF objects to store the aligned text.
         * @details This function iterates over each line in the Text_Cache and aligns them to the center of the text field. The function respects the maximum height and width of the text field
         *          and handles overflow according to the Style settings.
         */
        void alignTextCenter(std::vector<UTF>& Result);

        /**
         * @brief Listens for input and calls a function when user presses any key.
         * @param Then A function that takes a character as input and does something with it.
         * @details This function creates three actions (for key press, enter, and backspace) that listen for input when the text field is focused. If the event is a key press or enter, it
         *          calls the Then function with the character as input. If the event is a backspace, it removes the last character from the text field. In all cases, it marks the text field as
         *          dirty and updates the frame.
         */
        void input(std::function<void(char)> Then);

        element* safeMove() const override {
            return new textField();
        }
    };
}

#endif