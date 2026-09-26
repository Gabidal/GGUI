#include "switch.h"

#include "../core/core.h"

namespace GGUI{
    switchBox* switchBox::setStateString(terminal::cell off, terminal::cell on) {
        Off = off;
        On = on;

        // Mark the switch as needing a state update
        flags |= (stain::types::STATE);

        updateFrame();

        return this;
    }

    switchBox* switchBox::setText(std::string& text) { 
        pauseGGUI([this, &text](){
            // Mark the element as needing a deep state update
            flags |= (stain::types::DEEP);
            
            // Set the text with a space character added to the beginning
            Text.setText(text);

            // Update the switch element's dimensions to fit the new text
            setDimensions({
                Text.getWidth() + hasBorder() * 2,
                Text.getHeight() + hasBorder() * 2
            });
        });
        
        return this;
    }

    std::vector<terminal::cell>& switchBox::render(){
        std::vector<terminal::cell>& Result = cellBuffer;
        
        // Check for Dynamic attributes
        evaluateDynamicAttributes();

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

    switchBox* switchBox::setState(bool b){
        State = b;

        flags |= (stain::types::STATE);

        updateFrame();
        
        return this;
    }

    void switchBox::enableSingleSelect(){
        SingleSelect = true;
    }

    void switchBox::DisableOthers() {
        // If this is in switch group, disable other grouped switches
        if (this->isSingleSelect()){
            this->setState(true);

            if (!this->container)
                return;

            for (auto* c : this->container->getElements<switchBox>())
                if (c != this && c->isSingleSelect())
                    c->setState(false);
        }
        else{
            this->toggle();
        }
    }
}