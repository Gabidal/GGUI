#include "textField.h"
#include "../core/core.h"

#include "../core/utils/utils.h"

namespace GGUI{

    textField::textField(STYLING_INTERNAL::styleBase& s, bool Embed_Styles_On_Construct) : element(s, Embed_Styles_On_Construct){

        // Since Styling Height and Width are defaulted to 1, we can use this one row to reserve for one line.
        textLineCache.reserve(getHeight());

        if (getWidth() == 1 && getHeight() == 1){
            allowDynamicSize(true);
        }

        // Update the text cache list by newlines, and if no found then set the Text as the zeroth index.
        if (Embed_Styles_On_Construct)
            updateTextCache();
    }

    /**
     * @brief Updates the text cache of the text field when the text field has a deep stain.
     * @details This function is called when the text field has a deep stain, and it will update the text cache of the text field. The text cache is a list of compact strings, where each compact string is a line of text. The text cache is used to store the text of the text field, and it is used to determine the size of the text field. The text cache is updated by splitting the text into lines based on the newline character, and then adding each line to the text cache. The text cache is also updated to remove any empty lines at the end of the text cache.
     */
    void textField::updateTextCache(){
        textLineCache.clear();
        unsigned int borderOffset = hasBorder() ? 2 : 0;
        unsigned int innerWidth = getWidth() - borderOffset;

        // NOTE: This can be potentially removed.
        // This happens when text("...") is given with percentage dimensions, leaving width as zero.
        // The textField::render() will take care of this if percentage is used.
        if (innerWidth == 0 && Style->Width.number.getType() == types::EVALUATION_TYPE::PERCENTAGE){
            return;
        }

        // Will determine the text cache list by newlines, and if no found then set the Text as the zeroth index.
        line currentLine(0, 0);
        unsigned int longestLine = 0;

        // This is for the remaining liners to determine if they can append into the previous line or not.
        enum class lineReason {
            NONE,
            NEWLINE,
            WORDWRAP
        } previousLineReason = lineReason::NONE;

        for (size_t i = 0; i < text.size(); i++){
            bool flushRow = false;

            if (text[i] == '\n'){
                // Newlines are not counted as line lengths
                flushRow = true;
                previousLineReason = lineReason::NEWLINE;
            }
            else{   // NOTE: If there is a newline character we need to TOTALLY skip it!!!
                // Since in all situations the delimeter is also wanted to be part of the current line, we need to increase the current line length before deciding if we want to add it.
                currentLine.end++;
            }
            
            // This is for the word wrapping to beautifully end at when word end and not abruptly
            if (text[i] == ' ' && !isOverflowAllowed()){
                // Check if the current line length added one more word would go over the Width
                // For this we first need to know how long is this next word if there is any
                size_t nextSpace = text.find_first_of(' ', i + 1);
                size_t wordEnd = (nextSpace == std::string::npos) ? text.size() : nextSpace;
                int newWordLength = wordEnd - i;

                if (newWordLength + currentLine.getSize() >= innerWidth){
                    flushRow = true;
                    previousLineReason = lineReason::WORDWRAP;
                }
            }

            if (flushRow){
                // If the next word would go over the Width then add the current line to the Text_Cache
                textLineCache.push_back(currentLine);

                // check if the current line is longer than the longest line
                if (currentLine.getSize() > longestLine)
                    longestLine = currentLine.getSize();

                // reset current
                currentLine = line(i + 1, i + 1);
            }
        }

        // Make sure the last line is added
        if (currentLine.getSize() > 0){

            bool Last_Line_Exceeds_Width_With_Current_Line = textLineCache.size() > 0 && textLineCache.back().getSize() >= innerWidth;

            // Add the remaining liners if: There want any previous lines OR the last line exceeds the width with the current line OR the previous line ended with a newline.
            if (
                textLineCache.size() == 0 ||
                (
                    Last_Line_Exceeds_Width_With_Current_Line &&
                    !Style->Allow_Dynamic_Size.value
                ) ||
                previousLineReason == lineReason::NEWLINE
            ){
                // If not then add the current line to the Text_Cache
                textLineCache.push_back(currentLine);
            }
            else{
                // If it can be added then add it to the last line
                textLineCache.back().end += currentLine.getSize();
            }

            longestLine = std::max(longestLine, currentLine.getSize());
        }

        // Now we can check if Dynamic size is enabled, if so then resize textField by the new sizes
        if (isDynamicSizeAllowed()){
            // Set the new size
            setWidth(std::max((size_t)longestLine + borderOffset, (size_t)getWidth()));
            setHeight(std::max(textLineCache.size() + borderOffset, (size_t)getHeight()));
        }
    }

    /**
     * @brief Renders the text field into the Render_Buffer.
     * @details This function processes the text field to generate a vector of UTF objects representing the current state.
     * It handles different stains such as CLASS, STRETCH, COLOR, EDGE, and DEEP to ensure the text field is rendered correctly.
     * @return A vector of UTF objects representing the rendered text field.
     */
    std::vector<terminal::cell>& textField::render() {
        // Get reference to the render buffer
        std::vector<terminal::cell>& Result = cellBuffer;

        // Check for Dynamic attributes
        if(Style->evaluateDynamicDimensions(this))
            Dirty.Dirty(types::STAIN_TYPE::STRETCH);

        if (Style->evaluateDynamicPosition(this))
            Dirty.Dirty(types::STAIN_TYPE::MOVE);

        if (Style->evaluateDynamicGraphics(this))
            Dirty.Dirty(types::STAIN_TYPE::GRAPHICS);

        if (Style->evaluateDynamicBorder(this))
            Dirty.Dirty(types::STAIN_TYPE::EDGE);

        // If the text field is clean, return the current render buffer
        if (Dirty.is(types::STAIN_TYPE::CLEAN))
            return Result;

        // This does not CLEAN the DEEP stain it only checks if setText has been invoked.
        if (Dirty.is(types::STAIN_TYPE::DEEP)){
            updateTextCache();
        }

        if (Dirty.is(types::STAIN_TYPE::RESET)){
            Dirty.Clean(types::STAIN_TYPE::RESET);

            std::fill(cellBuffer.begin(), cellBuffer.end(), ' ');
            
            Dirty.Dirty(types::STAIN_TYPE::GRAPHICS | types::STAIN_TYPE::EDGE | types::STAIN_TYPE::DEEP);
        }

        // Handle the STRETCH stain by evaluating dynamic attributes and resizing the result buffer
        if (Dirty.is(types::STAIN_TYPE::STRETCH)) {
            Result.clear();
            Result.resize(getWidth() * getHeight(), ' ');
            Dirty.Clean(types::STAIN_TYPE::STRETCH);
            Dirty.Dirty(types::STAIN_TYPE::GRAPHICS | types::STAIN_TYPE::EDGE | types::STAIN_TYPE::RESET | types::STAIN_TYPE::NOT_RENDERED);
        }

        if (Dirty.is(types::STAIN_TYPE::NOT_RENDERED)) {
            if (On_Render) On_Render(this);

            // Clean regardless of On_Render existing or not.
            Dirty.Clean(types::STAIN_TYPE::NOT_RENDERED);
        }

        // Update the absolute position cache if the MOVE stain is detected.
        if (Dirty.is(types::STAIN_TYPE::MOVE)) {
            Dirty.Clean(types::STAIN_TYPE::MOVE);

            updateAbsolutePositionCache();
        }

        // Align text and add child windows to the Result buffer if the DEEP stain is detected
        if (Dirty.is(types::STAIN_TYPE::DEEP)) {
            Dirty.Clean(types::STAIN_TYPE::DEEP);

            // clean reflection pool
            graphicalReflectionPool.clear();
            graphicalIdentityPool.clear();
            Dirty.Dirty(types::STAIN_TYPE::GRAPHICS);

            if (Style->Align.value == ANCHOR::LEFT)
                alignTextLeft(Result);
            else if (Style->Align.value == ANCHOR::RIGHT)
                alignTextRight(Result);
            else if (Style->Align.value == ANCHOR::CENTER)
                alignTextCenter(Result);
        }

        // Apply the color system to the resized result list
        if (Dirty.is(types::STAIN_TYPE::GRAPHICS)){        
            // Clean the color stain after applying the color system.
            Dirty.Clean(types::STAIN_TYPE::GRAPHICS);

            compileActiveGraphics();
        }

        // Add borders and titles if the EDGE stain is detected.
        if (Dirty.is(types::STAIN_TYPE::EDGE)){
            Dirty.Clean(types::STAIN_TYPE::EDGE);

            renderBorders(Result);
            renderTitle(Result);
        }

        return Result;
    }

    /**
     * @brief Sets the text of the text field.
     * @details This function first stops the GGUI engine, then sets the text with a space character added to the beginning, and finally updates the text field's dimensions to fit the new text. The text is then reset in the Render_Buffer nested buffer of the window.
     * @param text The new text for the text field.
     */
    void textField::setText(std::string_view newText){
        text = newText;
        // We don't want to accidentally start re-writing into the name when streaming input text.
        if (hasEmptyName())
            setName(text);

        Dirty.Dirty(types::STAIN_TYPE::DEEP | types::STAIN_TYPE::RESET);

        updateTextCache();

        updateFrame();
    }

    /**
     * @brief Aligns text to the left within the text field.
     * @param Result A vector of UTF objects to store the aligned text.
     * @details This function iterates over each line in the Text_Cache and aligns them to the left side 
     *          of the text field. The function respects the maximum height and width of the text field 
     *          and handles overflow according to the Style settings.
     */
    void textField::alignTextLeft(std::vector<terminal::cell>& Result) {
        size_t startY = (int)hasBorder();
        size_t startX = (int)hasBorder();
        size_t endY   = getHeight() - (int)hasBorder();
        size_t endX   = getWidth() -  (int)hasBorder();

        for (size_t lineIndex = 0; lineIndex < textLineCache.size(); lineIndex++) {
            for (size_t characterIndex = 0; characterIndex < textLineCache[lineIndex].getSize(); characterIndex++) {
                size_t outputX = startX + characterIndex;
                size_t outputY = startY + lineIndex;

                // Ensure we don't write outside the bounds of the Result buffer
                if (outputX >= endX || outputY >= endY) {
                    break;  // Stop if we reach the end of the writable area
                }

                Result[outputY * getWidth() + outputX] = text[textLineCache[lineIndex].start + characterIndex];
            }
        }
    }

    /**
     * @brief Aligns text to the right within the text field.
     * @param Result A vector of UTF objects to store the aligned text.
     * @details This function iterates over each line in the Text_Cache and aligns them to the right side
     *          of the text field. The function respects the maximum height and width of the text field
     *          and handles overflow according to the Style settings.
     */
    void textField::alignTextRight(std::vector<terminal::cell>& Result) {
        size_t startY = (int)hasBorder();
        size_t endY   = getHeight() - (int)hasBorder();
        size_t endX   = getWidth() -  (int)hasBorder();

        for (size_t lineIndex = 0; lineIndex < textLineCache.size(); lineIndex++) {
            size_t lineLength = textLineCache[lineIndex].getSize();
            size_t outputY = startY + lineIndex;

            // Calculate the starting X position for right alignment
            size_t outputXStart = endX - lineLength;

            for (size_t characterIndex = 0; characterIndex < lineLength; characterIndex++) {
                size_t outputX = outputXStart + characterIndex;

                // Ensure we don't write outside the bounds of the Result buffer
                if (outputX >= endX || outputY >= endY) {
                    break;  // Stop if we reach the end of the writable area
                }

                Result[outputY * getWidth() + outputX] = text[textLineCache[lineIndex].start + characterIndex];
            }
        }
    }

    /**
     * @brief Aligns text to the center within the text field.
     * @param Result A vector of UTF objects to store the aligned text.
     * @details This function iterates over each line in the Text_Cache and aligns them to the center of the text field. The function respects the maximum height and width of the text field
     *          and handles overflow according to the Style settings.
     */
    void textField::alignTextCenter(std::vector<terminal::cell>& Result) {
        size_t startY = (int)hasBorder();
        size_t startX = (int)hasBorder();
        size_t endY   = getHeight() - (int)hasBorder();
        size_t endX   = getWidth() -  (int)hasBorder();

        for (size_t lineIndex = 0; lineIndex < textLineCache.size(); lineIndex++) {
            size_t lineLength = textLineCache[lineIndex].getSize();
            size_t outputY = startY + lineIndex;

            // Calculate the starting X position for center alignment
            size_t outputXStart = startX + (endX - startX - lineLength) / 2;

            for (size_t characterIndex = 0; characterIndex < lineLength; characterIndex++) {
                size_t outputX = outputXStart + characterIndex;

                // Ensure we don't write outside the bounds of the Result buffer
                if (outputX >= endX || outputY >= endY) {
                    break;  // Stop if we reach the end of the writable area
                }

                Result[outputY * getWidth() + outputX] = text[textLineCache[lineIndex].start + characterIndex];
            }
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
        addEventhandler(converter::output::event::action(
            {converter::input::key::types::ALL_LETTERS},
            [this, Then](converter::output::event::base* input) {
                if (Focused) {
                    // go through all enabled keyboards between space and delete and check what letter was turned on
                    char letter = '\0';

                    for (char i = (char)converter::input::key::types::SPACE + 1; i < (char)converter::input::key::types::DELETE; i++) {
                        if (core::inputManager->currentKeyboardState[(uint8_t)i].state) {
                            letter = i;
                            break;
                        }
                    }

                    if (letter == '\0') {
                        assert(false && "No letter was pressed, but the event handler was called.");
                        return false; // No letter was pressed
                    }

                    //First call the function with the user's input
                    Then(this, letter);
                    updateFrame();

                    return true;
                }
                //action failed.
                return false;
            },
            getName() + "::input::keypress"
        ));

        addEventhandler(converter::output::event::action(
            {converter::input::key::types::ENTER},
            [this, Then](converter::output::event::base*) {
                if (Focused && core::inputManager->currentKeyboardState[(uint8_t)converter::input::key::types::ENTER].state) {
                    //First call the function with the user's input
                    Then(this, '\n');
                    updateFrame();

                    return true;
                }
                //action failed.
                return false;
            },
            getName() + "::input::enter"
        ));

        addEventhandler(converter::output::event::action(
            {converter::input::key::types::BACKSPACE},
            [this](converter::output::event::base*) {
                if (Focused && core::inputManager->currentKeyboardState[(uint8_t)converter::input::key::types::BACKSPACE].state) {
                    //If the text field is empty, there is nothing to do
                    if (text.size() > 0) {
                        text.pop_back();

                        updateTextCache();

                        Dirty.Dirty(types::STAIN_TYPE::DEEP | types::STAIN_TYPE::RESET);
                        updateFrame();
                    }

                    return true;
                }
                //action failed.
                return false;
            },
            getName() + "::input::backspace"
        ));
    }
}