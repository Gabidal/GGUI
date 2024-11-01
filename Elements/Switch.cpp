#include "Switch.h"
#include "List_View.h"

#include "../Core/Renderer.h"

namespace GGUI{

    /**
     * @brief Constructs a Switch element with specified text, states, event handler, and styling.
     * @param text The text to display on the switch.
     * @param states A vector containing the unchecked and checked states.
     * @param event The function to call when the switch is toggled.
     * @param s The styling for the switch.
     */
    Switch::Switch(std::string text, std::vector<std::string> states, std::function<void (Element* This)> event, Styling s) : Element(s) {
        Pause_GGUI([this, text, states, event](){
            // Initialize the states for the switch
            States = states;
            
            // Enable text overflow and set initial text
            Text.Allow_Overflow(true);
            Set_Text(text);

            // Define the click event behavior for the switch
            On_Click([=]([[maybe_unused]] Event* e){
                this->Toggle();  // Toggle the switch state

                event(this);  // Execute the provided event handler

                Update_Frame();  // Update the frame to reflect changes

                return true;  // Allow event propagation
            });

            // Set dimensions based on text size
            Set_Width(Text.Get_Width());
            Set_Height(Text.Get_Height());

            // Mark the element as needing a deep state update
            Dirty.Dirty(STAIN_TYPE::DEEP | STAIN_TYPE::STATE);
        });
    }

    /**
     * @brief Sets the text of the switch element.
     * @details This function sets the text of the switch element by first pausing the GGUI engine, then setting the text with a space character added to the beginning, and finally updating the switch element's dimensions to fit the new text. The text is then reset in the Render_Buffer nested buffer of the window.
     * @param text The new text for the switch element.
     */
    void Switch::Set_Text(std::string text) { 
        Pause_GGUI([this, text](){
            std::string Symbol = " ";
            char Space = ' ';

            // Mark the element as needing a deep state update
            Dirty.Dirty(STAIN_TYPE::DEEP);

            // Set the text with a space character added to the beginning
            Text.Set_Text(Symbol + Space + text);   // This will call the update_frame for us.

            // Update the switch element's dimensions to fit the new text
            Set_Width(Text.Get_Width() + Has_Border() * 2);
            Set_Height(Text.Get_Height() + Has_Border() * 2);
        });
    }


    /**
     * @brief Renders the switch element and its children into the Render_Buffer nested buffer of the window.
     * @details This function processes the switch element to generate a vector of UTF objects representing the current state.
     * It handles different stains such as CLASS, STRETCH, COLOR, EDGE, and DEEP to ensure the switch element is rendered correctly.
     * @return A vector of UTF objects representing the rendered switch element.
     */
    std::vector<GGUI::UTF>& Switch::Render(){
        std::vector<GGUI::UTF>& Result = Render_Buffer;

        if (Dirty.is(STAIN_TYPE::CLEAN))
            return Result;

        // Parse the classes if the CLASS stain is detected.
        if (Dirty.is(STAIN_TYPE::CLASS)){
            Parse_Classes();

            Dirty.Clean(STAIN_TYPE::CLASS);
        }

        // Handle the STRETCH stain by evaluating dynamic attributes and resizing the result buffer.
        if (Dirty.is(STAIN_TYPE::STRETCH)){
            // This needs to be called before the actual stretch, since the actual Width and Height have already been modified to the new state, and we need to make sure that is correct according to the percentile of the dynamic attributes that follow the parents diction.
            Style->Evaluate_Dynamic_Attribute_Values(this);
            
            Result.clear();
            Result.resize(Get_Width() * Get_Height(), SYMBOLS::EMPTY_UTF);
            Dirty.Clean(STAIN_TYPE::STRETCH);
            
            Dirty.Dirty(STAIN_TYPE::COLOR | STAIN_TYPE::EDGE | STAIN_TYPE::DEEP);
        }

        // Update the absolute position cache if the MOVE stain is detected.
        if (Dirty.is(STAIN_TYPE::MOVE)){
            Dirty.Clean(STAIN_TYPE::MOVE);

            Update_Absolute_Position_Cache();
        }

        // Check if the text has been changed.
        if (Dirty.is(STAIN_TYPE::DEEP)){
            Nest_Element(this, &Text, Result, Text.Render());

            // Clean text update notice and state change notice.
            // NOTE: Cleaning STATE flag without checking it's existence might lead to unexpected results.
            Dirty.Clean(STAIN_TYPE::DEEP);

            Dirty.Dirty(STAIN_TYPE::COLOR);
        }

        // Update the state of the switch.
        if (Dirty.is(STAIN_TYPE::STATE)){
            int State_Location_X = Has_Border();
            int State_Location_Y = Has_Border();

            Result[State_Location_Y * Get_Width() + State_Location_X] = States[State];

            Dirty.Clean(STAIN_TYPE::STATE);
            Dirty.Dirty(STAIN_TYPE::COLOR);
        }

        // Apply the color system to the resized result list
        if (Dirty.is(STAIN_TYPE::COLOR))
            Apply_Colors(this, Result);

        // This will add the borders if necessary and the title of the window.
        if (Dirty.is(STAIN_TYPE::EDGE))
            Add_Overhead(this, Result);

        return Result;
    }

}