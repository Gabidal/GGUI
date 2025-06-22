#include "switch.h"
#include "listView.h"

#include "../core/renderer.h"

#include "../core/utils/utils.h"

namespace GGUI{

    STAIN_TYPE visualState::Embed_Value([[maybe_unused]] styling* host, element* owner){
        if (dynamic_cast<switchBox*>(owner) || dynamic_cast<radioButton*>(owner) || dynamic_cast<checkBox*>(owner))
            ((switchBox*)owner)->setStateString(Off, On);
        else
            throw std::runtime_error("The visualState attribute can only be used on switchBox, radioButton or checkBox type elements.");

        return STAIN_TYPE::STATE;
    }

    STAIN_TYPE singleSelect::Embed_Value([[maybe_unused]] styling* host, element* owner){
        if (dynamic_cast<switchBox*>(owner) || dynamic_cast<radioButton*>(owner) || dynamic_cast<checkBox*>(owner))
            ((switchBox*)owner)->enableSingleSelect();
        else 
            throw std::runtime_error("The group attribute can only be used on switchBox, radioButton or checkBox type elements.");

        return STAIN_TYPE::CLEAN;
    }

    /**
     * @brief Constructs a Switch element with specified text, states, event handler, and styling.
     * @param text The text to display on the switch.
     * @param states A vector containing the unchecked and checked states.
     * @param event The function to call when the switch is toggled.
     * @param s The styling for the switch.
     */
    switchBox::switchBox(
        STYLING_INTERNAL::style_base& s,
        bool Embed_Styles_On_Construct
    ) : element(s, Embed_Styles_On_Construct) {
        // Enable text overflow and set initial text
        Text.allowOverflow(true);

        // Set dimensions based on text size
        setWidth(Text.getWidth());
        setHeight(Text.getHeight());

        // Mark the element as needing a deep state update
        Dirty.Dirty(STAIN_TYPE::DEEP | STAIN_TYPE::STATE);
    }

    void switchBox::setStateString(const Compact_String* off, const Compact_String* on) {
        Off = off;
        On = on;

        // Mark the switch as needing a state update
        Dirty.Dirty(STAIN_TYPE::STATE);

        updateFrame();
    }

    /**
     * @brief Sets the text of the switch element.
     * @details This function sets the text of the switch element by first pausing the GGUI engine, then setting the text with a space character added to the beginning, and finally updating the switch element's dimensions to fit the new text. The text is then reset in the Render_Buffer nested buffer of the window.
     * @param text The new text for the switch element.
     */
    void switchBox::setText(Compact_String text) { 
        
        pauseGGUI([this, &text](){
            Compact_String Symbol = " ";   // This is where the switchbox symbol will replace to.
            Compact_String Space = ' ';
            
            Super_String<3> container{Symbol, Space, text};
            
            // Mark the element as needing a deep state update
            Dirty.Dirty(STAIN_TYPE::DEEP);
            
            // Set the text with a space character added to the beginning
            Text.setText(container.To_String());
            setName(To_String(text));

            // Update the switch element's dimensions to fit the new text
            setWidth(Text.getWidth() + hasBorder() * 2);
            setHeight(Text.getHeight() + hasBorder() * 2);
        });
    }

    void switchBox::showBorder(bool b){
        if (b != Style->Border_Enabled.Value) {
            Style->Border_Enabled = b;

            // Adjust the width and height of the progress bar based on the border state
            if (b) {
                Style->Width.Direct() += 2;
                Style->Height.Direct() += 2;
            }else {
                Style->Width.Direct() -= 2;
                Style->Height.Direct() -= 2;
            }

            // Mark the element as dirty for border changes
            Dirty.Dirty(STAIN_TYPE::EDGE);

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

        if (Dirty.is(STAIN_TYPE::CLEAN))
            return Result;

        if (Dirty.is(STAIN_TYPE::RESET)){
            Dirty.Clean(STAIN_TYPE::RESET);

            std::fill(Render_Buffer.begin(), Render_Buffer.end(), SYMBOLS::EMPTY_UTF);
            
            Dirty.Dirty(STAIN_TYPE::COLOR | STAIN_TYPE::EDGE | STAIN_TYPE::DEEP);
        }

        // Handle the STRETCH stain by evaluating dynamic attributes and resizing the result buffer.
        if (Dirty.is(STAIN_TYPE::STRETCH)){
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

        // Check if the text has been changed.
        if (Dirty.is(STAIN_TYPE::DEEP)){
            nestElement(this, &Text, Result, Text.render());

            // Clean text update notice and state change notice.
            // NOTE: Cleaning STATE flag without checking it's existence might lead to unexpected results.
            Dirty.Clean(STAIN_TYPE::DEEP);

            Dirty.Dirty(STAIN_TYPE::COLOR);
        }

        // Update the state of the switch.
        if (Dirty.is(STAIN_TYPE::STATE)){
            int State_Location_X = hasBorder();
            int State_Location_Y = hasBorder();
            
            if (Off && On)
                Result[State_Location_Y * getWidth() + State_Location_X] = getStateString();
            else
                INTERNAL::reportStack(getName() + " Missing visual state strings!");

            Dirty.Clean(STAIN_TYPE::STATE);
            Dirty.Dirty(STAIN_TYPE::COLOR);
        }

        // Apply the color system to the resized result list
        if (Dirty.is(STAIN_TYPE::COLOR)){        
            // Clean the color stain after applying the color system.
            Dirty.Clean(STAIN_TYPE::COLOR);

            applyColors(Result);
        }

        // Add borders and titles if the EDGE stain is detected.
        if (Dirty.is(STAIN_TYPE::EDGE)){
            renderBorders(Result);
            renderTitle(Result);
        }

        return Result;
    }

    void switchBox::toggle() {
        // Flip the current state of the switch
        State = !State;

        // Mark the switch as needing a state update
        Dirty.Dirty(STAIN_TYPE::STATE);

        updateFrame();
    }

    void switchBox::setState(bool b){
        State = b;

        Dirty.Dirty(STAIN_TYPE::STATE);

        updateFrame();
    }

    void switchBox::enableSingleSelect(){
        SingleSelect = true;
    }

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