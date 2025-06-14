#include "textField.h"
#include "../core/renderer.h"

#include "../core/utils/utils.h"

namespace GGUI{

    /**
     * @brief Updates the text cache of the text field when the text field has a deep stain.
     * @details This function is called when the text field has a deep stain, and it will update the text cache of the text field. The text cache is a list of compact strings, where each compact string is a line of text. The text cache is used to store the text of the text field, and it is used to determine the size of the text field. The text cache is updated by splitting the text into lines based on the newline character, and then adding each line to the text cache. The text cache is also updated to remove any empty lines at the end of the text cache.
     */
    void textField::updateTextCache(){
        Text_Cache.clear();

        // Will determine the text cache list by newlines, and if no found then set the Text as the zeroth index.
        Compact_String current_line(Text.data(), 0, true);
        unsigned int Longest_Line = 0;

        // This is for the remaining liners to determine if they can append into the previous line or not.
        enum class Line_Reason {
            NONE,
            NEWLINE,
            WORDWRAP
        } Previous_Line_Reason = Line_Reason::NONE;

        for (unsigned int i = 0; i < Text.size(); i++){
            bool flush_row = false;

            if (Text[i] == '\n'){
                // Newlines are not counted as line lengths
                flush_row = true;
                Previous_Line_Reason = Line_Reason::NEWLINE;
            }
            else{   // NOTE: If there is a newline character we need to TOTALLY skip it!!!
                // Since in all situations the delimeter is also wanted to be part of the current line, we need to increase the current line length before deciding if we want to add it.
                current_line.Size++;
            }
            
            // This is for the word wrapping to beautifully end at when word end and not abruptly
            if (Text[i] == ' ' && !Style->Allow_Dynamic_Size.Value){    
                // Check if the current line length added one more word would go over the Width
                // For this we first need to know how long is this next word if there is any
                int New_Word_Length = Text.find_first_of(' ', i + 1) - i;

                if (New_Word_Length + current_line.Size >= getWidth()){
                    flush_row = true;
                    Previous_Line_Reason = Line_Reason::WORDWRAP;
                }
            }

            if (flush_row){
                // If the next word would go over the Width then add the current line to the Text_Cache
                Text_Cache.push_back(current_line);

                // check if the current line is longer than the longest line
                if (current_line.Size > Longest_Line)
                    Longest_Line = current_line.Size;

                // reset current
                current_line = Compact_String(Text.data() + i + 1, 0, true);
            }
        }

        // Make sure the last line is added
        if (current_line.Size > 0){

            bool Last_Line_Exceeds_Width_With_Current_Line = Text_Cache.size() > 0 && Text_Cache.back().Size >= getWidth();

            // Add the remaining liners if: There want any previous lines OR the last line exceeds the width with the current line OR the previous line ended with a newline.
            if (
                Text_Cache.size() == 0 ||
                (
                    Last_Line_Exceeds_Width_With_Current_Line &&
                    !Style->Allow_Dynamic_Size.Value
                ) ||
                Previous_Line_Reason == Line_Reason::NEWLINE
            ){
                // If not then add the current line to the Text_Cache
                Text_Cache.push_back(current_line);
            }
            else{
                // If it can be added then add it to the last line
                Text_Cache.back().Size += current_line.Size;
            }

            Longest_Line = Max(Longest_Line, Text_Cache.back().Size);
        }

        // now we need to go through each compact string and make sure that those that are enforced as unicode's but are still 1 long, need to be transformed into the char bearing.
        for (Compact_String& line : Text_Cache){
            // We need to take care of the "Force Unicode" shenanigans before we add it to this list.
            if (line.Size == 1){
                line.Set_Ascii(line.Get_Unicode(true)[0]);
            }
        }

        // Now we can check if Dynamic size is enabled, if so then resize Text_Field by the new sizes
        if (isDynamicSizeAllowed()){
            // Set the new size
            setWidth(Max(Longest_Line, getWidth()));
            setHeight(Max(Text_Cache.size(), getHeight()));
        }
    }

    /**
     * @brief Renders the text field into the Render_Buffer.
     * @details This function processes the text field to generate a vector of UTF objects representing the current state.
     * It handles different stains such as CLASS, STRETCH, COLOR, EDGE, and DEEP to ensure the text field is rendered correctly.
     * @return A vector of UTF objects representing the rendered text field.
     */
    std::vector<GGUI::UTF>& textField::render() {
        // Get reference to the render buffer
        std::vector<GGUI::UTF>& Result = Render_Buffer;

        // Check for Dynamic attributes
        if(Style->Evaluate_Dynamic_Dimensions(this))
            Dirty.Dirty(STAIN_TYPE::STRETCH);

        if (Style->Evaluate_Dynamic_Position(this))
            Dirty.Dirty(STAIN_TYPE::MOVE);

        if (Style->Evaluate_Dynamic_Colors(this))
            Dirty.Dirty(STAIN_TYPE::COLOR);

        if (Style->Evaluate_Dynamic_Border(this))
            Dirty.Dirty(STAIN_TYPE::EDGE);

        // If the text field is clean, return the current render buffer
        if (Dirty.is(STAIN_TYPE::CLEAN))
            return Result;

        // Handle the STRETCH stain by evaluating dynamic attributes and resizing the result buffer
        if (Dirty.is(STAIN_TYPE::STRETCH)) {
            Result.clear();
            Result.resize(getWidth() * getHeight(), SYMBOLS::EMPTY_UTF);
            Dirty.Clean(STAIN_TYPE::STRETCH);
            Dirty.Dirty(STAIN_TYPE::COLOR | STAIN_TYPE::EDGE | STAIN_TYPE::DEEP);
        }

        // Update the absolute position cache if the MOVE stain is detected.
        if (Dirty.is(STAIN_TYPE::MOVE)) {
            Dirty.Clean(STAIN_TYPE::MOVE);

            updateAbsolutePositionCache();
        }

        // Apply the color system to the resized result list
        if (Dirty.is(STAIN_TYPE::COLOR)){        
            // Clean the color stain after applying the color system.
            Dirty.Clean(STAIN_TYPE::COLOR);

            applyColors(Result);
        }

        // Align text and add child windows to the Result buffer if the DEEP stain is detected
        if (Dirty.is(STAIN_TYPE::DEEP)) {
            Dirty.Clean(STAIN_TYPE::DEEP);

            if (Style->Align.Value == ANCHOR::LEFT)
                alignTextLeft(Result);
            else if (Style->Align.Value == ANCHOR::RIGHT)
                alignTextRight(Result);
            else if (Style->Align.Value == ANCHOR::CENTER)
                alignTextCenter(Result);
        }

        // Add borders and titles if the EDGE stain is detected.
        if (Dirty.is(STAIN_TYPE::EDGE)){
            renderBorders(Result);
            renderTitle(Result);
        }

        return Result;
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
    void textField::setSizeToFillParent(){
        if (!isDynamicSizeAllowed())
            return;

        int New_Width, New_Height;

        if (Parent->isDynamicSizeAllowed()){
            // If the parent can stretch, set the maximum width and a height of 1.
            New_Width = Text.size();
            New_Height = 1;
        }
        else{
            // Constrain the size within the parent's dimensions.
            New_Width = Min(Parent->getWidth() - getPosition().X, Text.size());
            updateTextCache();    // Recalculate the height based on the new width.
            New_Height = Min(Parent->getHeight() - getPosition().Y, Text_Cache.size());
        }
        
        // Apply the calculated dimensions to the text field.
        setDimensions(New_Width, New_Height);
    }

    /**
     * @brief Sets the text of the text field.
     * @details This function first stops the GGUI engine, then sets the text with a space character added to the beginning, and finally updates the text field's dimensions to fit the new text. The text is then reset in the Render_Buffer nested buffer of the window.
     * @param text The new text for the text field.
     */
    void textField::setText(std::string text){
        Text = text;
        setName(text);
        updateTextCache();

        Dirty.Dirty(STAIN_TYPE::DEEP | STAIN_TYPE::RESET);

        updateFrame();
    }

    /**
     * @brief Aligns text to the left within the text field.
     * @param Result A vector of UTF objects to store the aligned text.
     * @details This function iterates over each line in the Text_Cache and aligns them to the left side 
     *          of the text field. The function respects the maximum height and width of the text field 
     *          and handles overflow according to the Style settings.
     */
    void textField::alignTextLeft(std::vector<UTF>& Result) {
        unsigned int Line_Index = 0;  // To keep track of the inter-line positioning.

        for (Compact_String line : Text_Cache) {
            unsigned int Row_Index = 0;  // To track characters within the line.

            if (Line_Index >= getHeight())
                break;  // Stop if all available lines are filled.

            for (unsigned int Y = 0; Y < getHeight(); Y++) {
                for (unsigned int X = 0; X < getWidth(); X++) {

                    // Stop if the end of the current line is reached.
                    if (Y * getWidth() + X >= line.Size)
                        goto Next_Line;

                    // Write the current character to the Result buffer.
                    Result[(Y + Line_Index) * getWidth() + X] = line[Row_Index++];
                }

                // Handle line overflow based on the style settings.
                if (Style->Allow_Overflow.Value)
                    goto Next_Line;
            }

            Next_Line:;
            Line_Index++;  // Move to the next line.
        }
    }

    /**
     * @brief Aligns text to the right within the text field.
     * @param Result A vector of UTF objects to store the aligned text.
     * @details This function iterates over each line in the Text_Cache and aligns them to the right side
     *          of the text field. The function respects the maximum height and width of the text field
     *          and handles overflow according to the Style settings.
     */
    void textField::alignTextRight(std::vector<UTF>& Result) {
        unsigned int Line_Index = 0;    // To keep track of the inter-line rowing.

        for (Compact_String line : Text_Cache) {
            int Row_Index = line.Size - 1;  // Start from the end of the line

            if (Line_Index >= getHeight())
                break;  // All possible usable lines filled.

            for (unsigned int Y = 0; Y < getHeight(); Y++) {
                for (int X = (signed)getWidth() - 1; X >= 0; X--) {

                    if (Row_Index < 0)  // If there are no more characters in the line
                        goto Next_Line;

                    // write to the Result
                    Result[(Y + Line_Index) * getWidth() + (unsigned)X] = line[Row_Index--];  // Decrement Line_Index
                }

                // If current line has ended and the text is not word wrapped
                if (Style->Allow_Overflow.Value)
                    goto Next_Line; // An break would suffice but use goto for more readability
            }

            Next_Line:;
            Line_Index++;
        }
    }

    /**
     * @brief Aligns text to the center within the text field.
     * @param Result A vector of UTF objects to store the aligned text.
     * @details This function iterates over each line in the Text_Cache and aligns them to the center of the text field. The function respects the maximum height and width of the text field
     *          and handles overflow according to the Style settings.
     */
    void textField::alignTextCenter(std::vector<UTF>& Result) {
        unsigned int Line_Index = 0;    // To keep track of the inter-line rowing.

        for (Compact_String line : Text_Cache) {
            unsigned int Row_Index = 0;  // Start from the beginning of the line
            unsigned int Start_Pos = (getWidth() - line.Size) / 2;  // Calculate the starting position

            if (Line_Index >= getHeight())
                break;  // All possible usable lines filled.

            for (unsigned int Y = 0; Y < getHeight(); Y++) {
                for (unsigned int X = 0; X < getWidth(); X++) {

                    // If the current character is outside the line's range, skip it
                    if (X < Start_Pos || X > Start_Pos + line.Size)
                        continue;

                    if (Y * getWidth() + X >= line.Size)
                        goto Next_Line;

                    // write to the Result
                    Result[(Y + Line_Index) * getWidth() + X] = line[Row_Index++];
                }

                // If current line has ended and the text is not word wrapped
                if (Style->Allow_Overflow.Value)
                    goto Next_Line; // An break would suffice but use goto for more readability
            }

            Next_Line:;
            Line_Index++;
        }
    }

    /**
     * @brief Listens for input and calls a function when user presses any key.
     * @param Then A function that takes a character as input and does something with it.
     * @details This function creates three actions (for key press, enter, and backspace) that listen for input when the text field is focused. If the event is a key press or enter, it
     *          calls the Then function with the character as input. If the event is a backspace, it removes the last character from the text field. In all cases, it marks the text field as
     *          dirty and updates the frame.
     */
    void textField::input(std::function<void(textField*, char)> Then) {
        Action* key_press = new Action(
            Constants::KEY_PRESS,
            [this, Then](GGUI::Event* e) {
                if (Focused) {
                    //We know the event was gifted as Input*
                    GGUI::Input* input = (GGUI::Input*)e;

                    //First call the function with the user's input
                    Then(this, input->Data);
                    updateFrame();

                    return true;
                }
                //action failed.
                return false;
            },
            this,
            getName() + "::input::keypress"
        );
        GGUI::INTERNAL::Event_Handlers.push_back(key_press);

        Action* enter = new Action(
            Constants::ENTER,
            [this, Then](GGUI::Event* e) {
                if (Focused) {
                    //We know the event was gifted as Input*
                    GGUI::Input* input = (GGUI::Input*)e;

                    //First call the function with the user's input
                    Then(this, input->Data);
                    updateFrame();

                    return true;
                }
                //action failed.
                return false;
            },
            this,
            getName() + "::input::enter"
        );
        GGUI::INTERNAL::Event_Handlers.push_back(enter);

        Action* back_space = new Action(
            Constants::BACKSPACE,
            [this](GGUI::Event*) {
                if (Focused) {
                    //If the text field is empty, there is nothing to do
                    if (Text.size() > 0) {
                        Text.pop_back();
                        Dirty.Dirty(STAIN_TYPE::DEEP);
                        updateFrame();
                    }

                    return true;
                }
                //action failed.
                return false;
            },
            this,
            getName() + "::input::backspace"
        );
        GGUI::INTERNAL::Event_Handlers.push_back(back_space);
    }

}