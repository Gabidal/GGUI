#include "switch.h"
#include "listView.h"

#include "../core/renderer.h"

namespace GGUI{

    /**
     * @brief Constructs a Switch element with specified text, states, event handler, and styling.
     * @param text The text to display on the switch.
     * @param states A vector containing the unchecked and checked states.
     * @param event The function to call when the switch is toggled.
     * @param s The styling for the switch.
     */
    switchBox::switchBox(Compact_String text, std::vector<Compact_String> states, std::function<void (element* This)> event, styling s, bool Embed_Styles_On_Construct) : element(s, Embed_Styles_On_Construct) {
        pauseGGUI([this, text, states, event](){
            // Initialize the states for the switch
            States = states;
            
            // Enable text overflow and set initial text
            Text.allowOverflow(true);
            setText(text);

            // Define the click event behavior for the switch
            onClick([=]([[maybe_unused]] Event* e){
                this->toggle();  // Toggle the switch state

                event(this);  // Execute the provided event handler

                updateFrame();  // Update the frame to reflect changes

                return true;  // Allow event propagation
            });

            // Set dimensions based on text size
            setWidth(Text.getWidth());
            setHeight(Text.getHeight());

            // Mark the element as needing a deep state update
            Dirty.Dirty(STAIN_TYPE::DEEP | STAIN_TYPE::STATE);
        });
    }

    /**
     * @brief Sets the text of the switch element.
     * @details This function sets the text of the switch element by first pausing the GGUI engine, then setting the text with a space character added to the beginning, and finally updating the switch element's dimensions to fit the new text. The text is then reset in the Render_Buffer nested buffer of the window.
     * @param text The new text for the switch element.
     */
    void switchBox::setText(Compact_String text) { 
        pauseGGUI([this, text](){
            Compact_String Symbol = " ";   // This is where the switchbox symbol will replace to.
            Compact_String Space = ' ';

            Super_String container{Symbol, Space, text};

            // Mark the element as needing a deep state update
            Dirty.Dirty(STAIN_TYPE::DEEP);

            // Set the text with a space character added to the beginning
            Text.setText(container.To_String());

            // Update the switch element's dimensions to fit the new text
            setWidth(Text.getWidth() + hasBorder() * 2);
            setHeight(Text.getHeight() + hasBorder() * 2);
        });
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

        // Parse the classes if the CLASS stain is detected.
        if (Dirty.is(STAIN_TYPE::CLASS)){
            parseClasses();

            Dirty.Clean(STAIN_TYPE::CLASS);
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

            Result[State_Location_Y * getWidth() + State_Location_X] = States[State];

            Dirty.Clean(STAIN_TYPE::STATE);
            Dirty.Dirty(STAIN_TYPE::COLOR);
        }

        // Apply the color system to the resized result list
        if (Dirty.is(STAIN_TYPE::COLOR)){        
            // Clean the color stain after applying the color system.
            Dirty.Clean(STAIN_TYPE::COLOR);

            applyColors(this, Result);
        }

        // Add borders and titles if the EDGE stain is detected.
        if (Dirty.is(STAIN_TYPE::EDGE))
            addOverhead(this, Result);

        return Result;
    }

}