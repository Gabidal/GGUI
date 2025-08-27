#ifndef _TEXT_FIELD_H_
#define _TEXT_FIELD_H_

#include "element.h"

#include "../core/utils/superString.h"
#include "../core/utils/constants.h"
#include "../core/utils/color.h"
#include "../core/utils/utf.h"
#include "../core/utils/style.h"

namespace GGUI{
    class textField : public element{
    protected:
        std::string Text = "";

        // This will hold the text by lines, and does not re-allocate memory for whole text, only for indicies.
        std::vector<INTERNAL::compactString> Text_Cache; 

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
        textField(STYLING_INTERNAL::styleBase& s = STYLES::CONSTANTS::Default, bool Embed_Styles_On_Construct = false);
        
        /**
         * @brief Constructs a textField object with the specified style and optional embedding of styles.
         * 
         * @param s A rvalue reference to a styleBase object that defines the styling for the textField.
         * @param Embed_Styles_On_Construct A boolean flag indicating whether styles should be embedded during construction. 
         *        Defaults to false.
         */
        textField(STYLING_INTERNAL::styleBase&& s, bool Embed_Styles_On_Construct = false) : textField(s, Embed_Styles_On_Construct){}

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
        void input(std::function<void(textField*, char)> Then);


    protected:
        /**
         * @brief Renders the text field into the Render_Buffer.
         * @details This function processes the text field to generate a vector of UTF objects representing the current state.
         * It handles different stains such as CLASS, STRETCH, COLOR, EDGE, and DEEP to ensure the text field is rendered correctly.
         * @return A vector of UTF objects representing the rendered text field.
         */
        std::vector<GGUI::UTF>& render() override;
        
        /**
         * @brief Creates a new instance of the textField element.
         * 
         * This method overrides the base class implementation to return
         * a dynamically allocated instance of the textField class.
         * 
         * @return A pointer to a newly created textField instance.
         */
        element* createInstance() const override {
            return new textField();
        }

        // switchBox uses textField::render() so lets give it some access.
        friend class switchBox;
    };
}

#endif