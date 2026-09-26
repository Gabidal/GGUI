#include "textField.h"
#include "../core/core.h"

#include "../core/utils/utils.h"

namespace GGUI{

    void textField::updateTextCache(){
        textLineCache.clear();
        size_t borderOffset = hasBorder() ? 2 : 0;
        size_t innerWidth = getWidth() - borderOffset;

        // NOTE: This can be potentially removed.
        // This happens when text("...") is given with percentage dimensions, leaving width as zero.
        // The textField::render() will take care of this if percentage is used.
        if (innerWidth == 0 && getDimensions().x().isRelative()){
            return;
        }

        // Will determine the text cache list by newlines, and if no found then set the Text as the zeroth index.
        line currentLine(0, 0);
        size_t longestLine = 0;

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
            if (text[i] == ' '){
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
                Last_Line_Exceeds_Width_With_Current_Line ||
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

        // Since 0.1.9.5 textFields are always dynamic containers
        setDimensions({
            std::max(longestLine + borderOffset, (size_t)getWidth()),
            std::max(textLineCache.size() + borderOffset, (size_t)getHeight())
        });
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

        evaluateDynamicAttributes();

        // If the text field is clean, return the current render buffer
        if (flags.isEmpty())
            return Result;

        // This does not CLEAN the DEEP stain it only checks if setText has been invoked.
        if (flags.has(stain::types::DEEP)){
            updateTextCache();
        }

        if (flags.has(stain::types::RESET)){
            flags ^= (stain::types::RESET);

            std::fill(cellBuffer.begin(), cellBuffer.end(), ' ');
            
            flags |= (stain::base(stain::types::GRAPHICS) | stain::types::EDGE | stain::types::DEEP);
        }

        // Handle the STRETCH stain by evaluating dynamic attributes and resizing the result buffer
        if (flags.has(stain::types::STRETCH)) {
            Result.clear();
            Result.resize(getWidth() * getHeight(), ' ');
            flags ^= (stain::types::STRETCH);
            flags |= (stain::base(stain::types::GRAPHICS) | stain::types::EDGE | stain::types::RESET | stain::types::NOT_RENDERED);
        }

        if (flags.has(stain::types::NOT_RENDERED)) {
            if (onRender) onRender(this);

            // Clean regardless of On_Render existing or not.
            flags ^= (stain::types::NOT_RENDERED);
        }

        // Update the absolute position cache if the MOVE stain is detected.
        if (flags.has(stain::types::MOVE)) {
            flags ^= (stain::types::MOVE);

            updateAbsolutePositionCache();
        }

        // Align text and add content windows to the Result buffer if the DEEP stain is detected
        if (flags.has(stain::types::DEEP)) {
            flags ^= (stain::types::DEEP);

            // clean reflection pool
            graphicalReflectionPool.clear();
            graphicalIdentityPool.clear();
            flags |= (stain::types::GRAPHICS);

            if (alignment == alignments::LEFT)
                alignTextLeft(Result);
            else if (alignment == alignments::RIGHT)
                alignTextRight(Result);
            else if (alignment == alignments::CENTER)
                alignTextCenter(Result);
        }

        // Apply the color system to the resized result list
        if (flags.has(stain::types::GRAPHICS)){        
            // Clean the color stain after applying the color system.
            flags ^= (stain::types::GRAPHICS);

            compileActiveGraphics();
        }

        // Add borders and titles if the EDGE stain is detected.
        if (flags.has(stain::types::EDGE)){
            flags ^= (stain::types::EDGE);

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
    textField* textField::setText(std::string& newText){
        text = newText;

        // We don't want to accidentally start re-writing into the name when streaming input text.
        if (hasEmptyName())
            setName(text);

        flags |= (stain::base(stain::types::DEEP) | stain::types::RESET);

        updateTextCache();

        updateFrame();

        return this;
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
            [this, Then](converter::output::event::base*) { TODO("Going through all pressed keys seems very inefficient!")
                if (core::focusedOn == this) {
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
            getTypedName() + "::input::keypress"
        ));

        addEventhandler(converter::output::event::action(
            {converter::input::key::types::ENTER},
            [this, Then](converter::output::event::base*) {
                if (core::focusedOn == this && core::inputManager->currentKeyboardState[(uint8_t)converter::input::key::types::ENTER].state) {
                    //First call the function with the user's input
                    Then(this, '\n');
                    updateFrame();

                    return true;
                }
                //action failed.
                return false;
            },
            getTypedName() + "::input::enter"
        ));

        addEventhandler(converter::output::event::action(
            {converter::input::key::types::BACKSPACE},
            [this](converter::output::event::base*) {
                if (core::focusedOn == this && core::inputManager->currentKeyboardState[(uint8_t)converter::input::key::types::BACKSPACE].state) {
                    //If the text field is empty, there is nothing to do
                    if (text.size() > 0) {
                        text.pop_back();

                        updateTextCache();

                        flags |= (stain::base(stain::types::DEEP) | stain::types::RESET);
                        updateFrame();
                    }

                    return true;
                }
                //action failed.
                return false;
            },
            getTypedName() + "::input::backspace"
        ));
    }
}