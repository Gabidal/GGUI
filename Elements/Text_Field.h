#ifndef _TEXT_FIELD_H_
#define _TEXT_FIELD_H_

#include "Element.h"

#include "../Core/Utils/Super_String.h"
#include "../Core/Utils/Constants.h"
#include "../Core/Utils/Color.h"
#include "../Core/Utils/Units.h"
#include "../Core/Utils/Event.h"
#include "../Core/Utils/Style.h"

namespace GGUI{
    class Text_Field : public Element{
    protected:
        std::string Text = "";

        // This will hold the text by lines, and does not re-allocate memory for whole text, only for indicies.
        std::vector<Compact_String> Text_Cache; 

        /**
         * @brief Updates the text cache list by newlines, and if no found then set the Text as the zeroth index.
         * @details This function will also determine the longest line length and store it in the class.
         * @note This function will also check if the lines can be appended to the previous line or not.
         */
        void Update_Text_Cache();
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
         */
        Text_Field(std::string text = "", Styling s = STYLES::CONSTANTS::Default) : Element(s), Text(text){

            // Since Styling Height and Width are defaulted to 1, we can use this one row to reserve for one line.
            Text_Cache.reserve(Get_Height());

            if (Get_Width() == 1 && Get_Height() == 1){
                Allow_Dynamic_Size(true);
            }

            // Update the text cache list by newlines, and if no found then set the Text as the zeroth index.
            Update_Text_Cache();
        }

        /**
         * @brief Sets the size of the text field to fill its parent element.
         * @details The function first checks if dynamic sizing is allowed for
         *          the text field and its parent. It then calculates the new
         *          width and height based on the parent's dimensions and the
         *          text size. If the parent allows dynamic sizing, it stretches
         *          to accommodate the text; otherwise, it constrains the size
         *          within the parent's boundaries.
         */
        void Set_Size_To_Fill_Parent();

        /**
         * @brief Sets the text of the text field.
         * @details This function first stops the GGUI engine, then sets the text with a space character added to the beginning, and finally updates the text field's dimensions to fit the new text. The text is then reset in the Render_Buffer nested buffer of the window.
         * @param text The new text for the text field.
         */
        void Set_Text(std::string text);

        /**
         * @brief Gets the text of the text field.
         * @details This function returns the string containing the text of the text field.
         * @return The text of the text field as a string.
         */
        std::string Get_Text(){
            return Text;
        }

        /**
         * @brief Renders the text field into the Render_Buffer.
         * @details This function processes the text field to generate a vector of UTF objects representing the current state.
         * It handles different stains such as CLASS, STRETCH, COLOR, EDGE, and DEEP to ensure the text field is rendered correctly.
         * @return A vector of UTF objects representing the rendered text field.
         */
        std::vector<GGUI::UTF>& Render() override;

        /**
         * @brief Aligns text to the left within the text field.
         * @param Result A vector of UTF objects to store the aligned text.
         * @details This function iterates over each line in the Text_Cache and aligns them to the left side 
         *          of the text field. The function respects the maximum height and width of the text field 
         *          and handles overflow according to the Style settings.
         */
        void Align_Text_Left(std::vector<UTF>& Result);
        
        /**
         * @brief Aligns text to the right within the text field.
         * @param Result A vector of UTF objects to store the aligned text.
         * @details This function iterates over each line in the Text_Cache and aligns them to the right side
         *          of the text field. The function respects the maximum height and width of the text field
         *          and handles overflow according to the Style settings.
         */
        void Align_Text_Right(std::vector<UTF>& Result);
        
        /**
         * @brief Aligns text to the center within the text field.
         * @param Result A vector of UTF objects to store the aligned text.
         * @details This function iterates over each line in the Text_Cache and aligns them to the center of the text field. The function respects the maximum height and width of the text field
         *          and handles overflow according to the Style settings.
         */
        void Align_Text_Center(std::vector<UTF>& Result);

        /**
         * @brief Listens for input and calls a function when user presses any key.
         * @param Then A function that takes a character as input and does something with it.
         * @details This function creates three actions (for key press, enter, and backspace) that listen for input when the text field is focused. If the event is a key press or enter, it
         *          calls the Then function with the character as input. If the event is a backspace, it removes the last character from the text field. In all cases, it marks the text field as
         *          dirty and updates the frame.
         */
        void Input(std::function<void(char)> Then);
    };
}

#endif