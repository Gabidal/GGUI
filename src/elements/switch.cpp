#include "switch.h"
#include "listView.h"

#include "../core/renderer.h"

#include "../core/utils/utils.h"

namespace GGUI{

    INTERNAL::STAIN_TYPE visualState::embedValue([[maybe_unused]] styling* host, element* owner){
        if (dynamic_cast<switchBox*>(owner) || dynamic_cast<radioButton*>(owner) || dynamic_cast<checkBox*>(owner))
            ((switchBox*)owner)->setStateString(Off, On);
        else
            throw std::runtime_error("The visualState attribute can only be used on switchBox, radioButton or checkBox type elements.");

        return INTERNAL::STAIN_TYPE::STATE;
    }

    INTERNAL::STAIN_TYPE singleSelect::embedValue([[maybe_unused]] styling* host, element* owner){
        if (dynamic_cast<switchBox*>(owner) || dynamic_cast<radioButton*>(owner) || dynamic_cast<checkBox*>(owner))
            ((switchBox*)owner)->enableSingleSelect();
        else 
            throw std::runtime_error("The group attribute can only be used on switchBox, radioButton or checkBox type elements.");

        return INTERNAL::STAIN_TYPE::CLEAN;
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

        // Mark the element as needing a deep state update
        Dirty.Dirty(INTERNAL::STAIN_TYPE::DEEP | INTERNAL::STAIN_TYPE::STATE);
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
    void switchBox::setStateString(const INTERNAL::compactString* off, const INTERNAL::compactString* on) {
        Off = off;
        On = on;

        // Mark the switch as needing a state update
        Dirty.Dirty(INTERNAL::STAIN_TYPE::STATE);

        updateFrame();
    }

    /**
     * @brief Sets the text of the switch element.
     * @details This function sets the text of the switch element by first pausing the GGUI engine, then setting the text with a space character added to the beginning, and finally updating the switch element's dimensions to fit the new text. The text is then reset in the Render_Buffer nested buffer of the window.
     * @param text The new text for the switch element.
     */
    void switchBox::setText(INTERNAL::compactString text) { 
        
        pauseGGUI([this, &text](){
            INTERNAL::compactString Symbol = " ";   // This is where the switchbox symbol will replace to.
            INTERNAL::compactString Space = ' ';
            
            INTERNAL::superString<3> container{Symbol, Space, text};
            
            // Mark the element as needing a deep state update
            Dirty.Dirty(INTERNAL::STAIN_TYPE::DEEP);
            
            // Set the text with a space character added to the beginning
            Text.setText(container.toString());
            setName(INTERNAL::toString(text));

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
        if (b != Style->Border_Enabled.value) {
            Style->Border_Enabled = b;

            // Adjust the width and height of the progress bar based on the border state
            if (b) {
                Style->Width.direct() += 2;
                Style->Height.direct() += 2;
            }else {
                Style->Width.direct() -= 2;
                Style->Height.direct() -= 2;
            }

            // Mark the element as dirty for border changes
            Dirty.Dirty(INTERNAL::STAIN_TYPE::EDGE);

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
    std::vector<GGUI::UTF>& switchBox::render(){
        std::vector<GGUI::UTF>& Result = renderBuffer;
        
        // Check for Dynamic attributes
        if(Style->evaluateDynamicDimensions(this))
            Dirty.Dirty(INTERNAL::STAIN_TYPE::STRETCH);

        if (Style->evaluateDynamicPosition(this))
            Dirty.Dirty(INTERNAL::STAIN_TYPE::MOVE);

        if (Style->evaluateDynamicColors(this))
            Dirty.Dirty(INTERNAL::STAIN_TYPE::COLOR);

        if (Style->evaluateDynamicBorder(this))
            Dirty.Dirty(INTERNAL::STAIN_TYPE::EDGE);

        if (Dirty.is(INTERNAL::STAIN_TYPE::CLEAN))
            return Result;

        if (Dirty.is(INTERNAL::STAIN_TYPE::RESET)){
            Dirty.Clean(INTERNAL::STAIN_TYPE::RESET);

            std::fill(renderBuffer.begin(), renderBuffer.end(), SYMBOLS::EMPTY_UTF);
            
            Dirty.Dirty(INTERNAL::STAIN_TYPE::COLOR | INTERNAL::STAIN_TYPE::EDGE | INTERNAL::STAIN_TYPE::DEEP);
        }

        // Handle the STRETCH stain by evaluating dynamic attributes and resizing the result buffer.
        if (Dirty.is(INTERNAL::STAIN_TYPE::STRETCH)){
            Result.clear();
            Result.resize(getWidth() * getHeight(), SYMBOLS::EMPTY_UTF);
            Dirty.Clean(INTERNAL::STAIN_TYPE::STRETCH);
            
            Dirty.Dirty(INTERNAL::STAIN_TYPE::COLOR | INTERNAL::STAIN_TYPE::EDGE | INTERNAL::STAIN_TYPE::DEEP | INTERNAL::STAIN_TYPE::NOT_RENDERED);
        }

        if (Dirty.is(INTERNAL::STAIN_TYPE::NOT_RENDERED)) {
            if (On_Render) On_Render(this);

            // Clean regardless of On_Render existing or not.
            Dirty.Clean(INTERNAL::STAIN_TYPE::NOT_RENDERED);
        }

        // Update the absolute position cache if the MOVE stain is detected.
        if (Dirty.is(INTERNAL::STAIN_TYPE::MOVE)) {
            Dirty.Clean(INTERNAL::STAIN_TYPE::MOVE);

            updateAbsolutePositionCache();
        }

        // Check if the text has been changed.
        if (Dirty.is(INTERNAL::STAIN_TYPE::DEEP)){
            nestElement(this, &Text, Result, Text.render());

            // Clean text update notice and state change notice.
            // NOTE: Cleaning STATE flag without checking it's existence might lead to unexpected results.
            Dirty.Clean(INTERNAL::STAIN_TYPE::DEEP);

            Dirty.Dirty(INTERNAL::STAIN_TYPE::COLOR);
        }

        // Update the state of the switch.
        if (Dirty.is(INTERNAL::STAIN_TYPE::STATE)){
            int State_Location_X = hasBorder();
            int State_Location_Y = hasBorder();
            
            if (Off && On)
                Result[State_Location_Y * getWidth() + State_Location_X] = getStateString();
            else
                INTERNAL::reportStack(getName() + " Missing visual state strings!");

            Dirty.Clean(INTERNAL::STAIN_TYPE::STATE);
            Dirty.Dirty(INTERNAL::STAIN_TYPE::COLOR);
        }

        // Apply the color system to the resized result list
        if (Dirty.is(INTERNAL::STAIN_TYPE::COLOR)){        
            // Clean the color stain after applying the color system.
            Dirty.Clean(INTERNAL::STAIN_TYPE::COLOR);

            applyColors(Result);
        }

        // Add borders and titles if the EDGE stain is detected.
        if (Dirty.is(INTERNAL::STAIN_TYPE::EDGE)){
            renderBorders(Result);
            renderTitle(Result);
        }

        return Result;
    }

    void switchBox::toggle() {
        // Flip the current state of the switch
        State = !State;

        // Mark the switch as needing a state update
        Dirty.Dirty(INTERNAL::STAIN_TYPE::STATE);

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

        Dirty.Dirty(INTERNAL::STAIN_TYPE::STATE);

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

    namespace INTERNAL{
        /**
         * @brief Disables other switches in the same group, keeping only the specified switch active.
         * 
         * This function ensures that if the provided switchBox (`keepOn`) is part of a group
         * and is marked as single-select, it will remain active while all other single-select
         * switches in the same group are disabled. If the switch is not part of a group or is
         * not single-select, it toggles the state of the provided switchBox.
         * 
         * @param keepOn Pointer to the switchBox that should remain active or be toggled.
         * 
         * @note If the `keepOn` switchBox does not have a parent or is not part of a group,
         *       the function will simply toggle its state.
         */
        void DisableOthers(switchBox* keepOn){
            // If this is in switch group, disable other grouped switches
            if (keepOn->isSingleSelect()){
                keepOn->setState(true);

                if (!keepOn->getParent())
                    return;

                for (auto* c : keepOn->getParent()->getElements<switchBox>())
                    if (c != keepOn && c->isSingleSelect())
                        c->setState(false);
            }
            else{
                keepOn->toggle();
            }
        }
    }
}