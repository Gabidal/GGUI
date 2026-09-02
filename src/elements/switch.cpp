#include "switch.h"

#include "../core/core.h"

#include "../core/utils/utils.h"

namespace GGUI{

    stain::base visualState::embedValue([[maybe_unused]] styling* host, element* owner){
        if (dynamic_cast<switchBox*>(owner) || dynamic_cast<radioButton*>(owner) || dynamic_cast<checkBox*>(owner))
            ((switchBox*)owner)->setStateString(Off, On);
        else
            throw std::runtime_error("The visualState attribute can only be used on switchBox, radioButton or checkBox type elements.");

        return stain::types::STATE;
    }

    stain::base singleSelect::embedValue([[maybe_unused]] styling* host, element* owner){
        if (dynamic_cast<switchBox*>(owner) || dynamic_cast<radioButton*>(owner) || dynamic_cast<checkBox*>(owner))
            ((switchBox*)owner)->enableSingleSelect();
        else 
            throw std::runtime_error("The group attribute can only be used on switchBox, radioButton or checkBox type elements.");

        return {};
    }

    /**
     * @brief Constructs a Switch element with specified text, states, event handler, and styling.
     * @param text The text to display on the switch.
     * @param states A vector containing the unchecked and checked states.
     * @param event The function to call when the switch is toggled.
     * @param s The styling for the switch.
     */
    switchBox::switchBox(
        STYLING_INTERNAL::styleBase& s,
        bool Embed_Styles_On_Construct
    ) : element(s, Embed_Styles_On_Construct) {
        // Enable text overflow and set initial text
        Text.allowOverflow(true);

        // Set dimensions based on text size
        setWidth(Text.getWidth());
        setHeight(Text.getHeight());

        Text.updatePosition({2, 0});    // 1 + 1, symbol + space

        // Mark the element as needing a deep state update
        flags |= (stain::base(stain::types::DEEP) | stain::types::STATE);
    }

    /**
     * @brief Sets the state strings for the switch box.
     * 
     * This function assigns the provided strings to represent the "off" and "on" states
     * of the switch box. It also marks the switch as needing a state update and refreshes
     * its frame to reflect the changes.
     * 
     * @param off Pointer to a compactString representing the "off" state.
     * @param on Pointer to a compactString representing the "on" state.
     */
    void switchBox::setStateString(terminal::cell off, terminal::cell on) {
        Off = off;
        On = on;

        // Mark the switch as needing a state update
        flags |= (stain::types::STATE);

        updateFrame();
    }

    /**
     * @brief Sets the text of the switch element.
     * @details This function sets the text of the switch element by first pausing the GGUI engine, then setting the text with a space character added to the beginning, and finally updating the switch element's dimensions to fit the new text. The text is then reset in the Render_Buffer nested buffer of the window.
     * @param text The new text for the switch element.
     */
    void switchBox::setText(std::string_view text) { 
        pauseGGUI([this, &text](){
            // Mark the element as needing a deep state update
            flags |= (stain::types::DEEP);
            
            // Set the text with a space character added to the beginning
            Text.setText(text);

            // Update the switch element's dimensions to fit the new text
            setWidth(Text.getWidth() + hasBorder() * 2);
            setHeight(Text.getHeight() + hasBorder() * 2);
        });
    }

    /**
     * @brief Toggles the visibility of the border for the switchBox element.
     * 
     * This function enables or disables the border of the switchBox element
     * based on the provided boolean value. When the border state changes, 
     * the width and height of the element are adjusted accordingly to 
     * accommodate the border. The element is marked as dirty to reflect 
     * the border changes, and a frame update is triggered to re-render 
     * the element.
     * 
     * @param b A boolean value indicating whether the border should be 
     *          enabled (true) or disabled (false).
     */
    void switchBox::showBorder(bool b){
        if (b != style->Border_Enabled.value) {
            style->Border_Enabled = b;

            // Adjust the width and height of the progress bar based on the border state
            if (b) {
                style->Width.set(style->Width.get() + 2);
                style->Height.set(style->Height.get() + 2);
            }else {
                style->Width.set(style->Width.get() - 2);
                style->Height.set(style->Height.get() - 2);
            }

            // Mark the element as dirty for border changes
            flags |= (stain::types::EDGE);

            // Trigger a frame update to re-render the progress bar
            updateFrame();
        }
    }

    /**
     * @brief Renders the switch element and its children into the Render_Buffer nested buffer of the window.
     * @details This function processes the switch element to generate a vector of UTF objects representing the current state.
     * It handles different stains such as CLASS, STRETCH, COLOR, EDGE, and DEEP to ensure the switch element is rendered correctly.
     * @return A vector of UTF objects representing the rendered switch element.
     */
    std::vector<terminal::cell>& switchBox::render(){
        std::vector<terminal::cell>& Result = cellBuffer;
        
        // Check for Dynamic attributes
        if(style->evaluateDynamicDimensions(this))
            flags |= (stain::types::STRETCH);

        if (style->evaluateDynamicPosition(this))
            flags |= (stain::types::MOVE);

        if (style->evaluateDynamicGraphics(this))
            flags |= (stain::types::GRAPHICS);

        if (style->evaluateDynamicBorder(this))
            flags |= (stain::types::EDGE);

        if (flags.isEmpty())
            return Result;

        if (flags.has(stain::types::RESET)){
            flags ^= (stain::types::RESET);

            std::fill(cellBuffer.begin(), cellBuffer.end(), ' ');
            
            flags |= (stain::base(stain::types::GRAPHICS) | stain::types::EDGE | stain::types::DEEP);
        }

        // Handle the STRETCH stain by evaluating dynamic attributes and resizing the result buffer.
        if (flags.has(stain::types::STRETCH)){
            Result.clear();
            Result.resize(getWidth() * getHeight(), ' ');
            flags ^= (stain::types::STRETCH);
            
            flags |= (stain::base(stain::types::GRAPHICS) | stain::types::EDGE | stain::types::DEEP | stain::types::NOT_RENDERED);
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

        // Check if the text has been changed.
        if (flags.has(stain::types::DEEP)){
            core::nestElement(this, &Text, Result, Text.render());

            // Clean text update notice and state change notice.
            // NOTE: Cleaning STATE flag without checking it's existence might lead to unexpected results.
            flags ^= (stain::types::DEEP);

            flags |= (stain::types::GRAPHICS);
        }

        // Update the state of the switch.
        if (flags.has(stain::types::STATE)){
            int State_Location_X = hasBorder();
            int State_Location_Y = hasBorder();
            
            Result[State_Location_Y * getWidth() + State_Location_X] = getStateString();

            flags ^= (stain::types::STATE);
            flags |= (stain::types::GRAPHICS);
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

    void switchBox::toggle() {
        // Flip the current state of the switch
        State = !State;

        // Mark the switch as needing a state update
        flags |= (stain::types::STATE);

        updateFrame();
    }

    /**
     * @brief Sets the state of the switch box and updates its visual representation.
     * 
     * This function changes the internal state of the switch box to the specified
     * boolean value, marks the state as dirty for internal processing, and updates
     * the frame to reflect the new state.
     * 
     * @param b The new state to set for the switch box. `true` represents an active
     *          state, while `false` represents an inactive state.
     */
    void switchBox::setState(bool b){
        State = b;

        flags |= (stain::types::STATE);

        updateFrame();
    }

    /**
     * @brief Enables single selection mode for the switchBox.
     * 
     * When single selection mode is enabled, the switchBox ensures that 
     * only one option can be selected at a time.
     */
    void switchBox::enableSingleSelect(){
        SingleSelect = true;
    }

    /**
     * @brief Disables other switches in the same group, keeping only the specified switch active.
     * 
     * This function ensures that if the provided switchBox this is part of a group
     * and is marked as single-select, it will remain active while all other single-select
     * switches in the same group are disabled. If the switch is not part of a group or is
     * not single-select, it toggles the state of the provided switchBox.
     * 
     * @note If the this switchBox does not have a parent or is not part of a group,
     *       the function will simply toggle its state.
    */
    void switchBox::DisableOthers() {
        // If this is in switch group, disable other grouped switches
        if (this->isSingleSelect()){
            this->setState(true);

            if (!this->getParent())
                return;

            for (auto* c : this->getParent()->getElements<switchBox>())
                if (c != this && c->isSingleSelect())
                    c->setState(false);
        }
        else{
            this->toggle();
        }
    }
}