#include "converter.h"
#include "core.h"

#include "backend/terminal.h"

#include "utils/settings.h"
#include "utils/drm.h"
#include "utils/logger.h"

namespace GGUI {
    namespace INTERNAL {
        extern element* focusedOn;
        extern element* hoveredOn;

        extern void updateFocusedElement(GGUI::element* new_candidate);
        extern void updateHoveredElement(GGUI::element* new_candidate);

        extern void unFocusElement();
        extern void unHoverElement();
    }

    namespace converter {
        namespace input {
            base::base() {
                pollingThread = std::thread([this](){
                    INTERNAL::LOGGER::registerCurrentThread();
                    this->inputThread();
                });
            }

            /**
            * @brief polls input from terminal or DRM backend, and then converts it into events.
            */
            void base::inputThread(){
                while (true){
                    if (SETTINGS::enableDRM) {
                        INTERNAL::DRM::pollInputs();
                    }
                    else if (!terminal::currentStates || !terminal::currentStates->transmission.isConnected()) {    // platform initialization is still in progress
                        std::this_thread::sleep_for(std::chrono::seconds(1));
                        continue;
                    }
                    else {
                        terminal::currentStates->transmission.pollInput();
                    }
    
                    pauseGGUI([&](){
                        if (SETTINGS::enableDRM) {
                            INTERNAL::DRM::translateInputs();
                        }
                        else {
                            // Translate the Queried inputs.
                            terminal::currentStates->parseInput();
                        }
    
                        // Call the linked output registry to convert the incoming data.
                        out->transformInput();
                    });
                }
            
                INTERNAL::LOGGER::log("Input thread terminated!");
            }
        }

        namespace output {
            /**
            * @brief Handles all events in the system.
            * @details This function goes through all event handlers and checks if the event criteria matches any of the inputs.
            *          If a match is found, it calls the event handler job with the input as an argument.
            *          If the job is successful, it removes the input from the list of inputs.
            *          If the job is unsuccessful, it reports an error.
            */
            void base::eventHandler() {
                // For an input to belong to the current event handler, it needs to:
                // - Have identical criteria
                // - If mouse or enter is as the input:
                //      - If host present and is onHovered: Un-hover the host element and and set it to onFocused
                //      - If host present and is onFocused: Pipe input into the event handler job task

                // Since some key events are piped to us at a different speed than others, we need to keep the older (un-used) inputs "alive" until their turn arrives.
                Populate_Inputs_For_Held_Down_Keys();

                for (unsigned int i = 0; i < handlers.size(); i++){
                    element* currentElement = handlers[i];

                    const std::vector<event::action>& currentEventhandlers = currentElement->getEventHandlers();

                    for (unsigned int j = 0; j < currentElement->getEventHandlers().size(); j++) {
                        const event::action& currentEventHandler = currentEventhandlers[j];

                        // The reason these are held over multitude of inputs, is for scenario where this memory thread has not run in a long time and has a long query of inputs-
                        // and in this same listing of inputs at the start is the mouse click or enter and the user given inputs for that specifically activated event handler.
                        bool Has_Mouse_Left_Click_Event = false;
                        bool Has_Enter_Press_Event = false;
                    
                        if (!currentElement->isDisplayed())
                            continue;   // Skip eventhandlers where their host is not active

                        bool overlapsWithMouse = currentMouse.collides(currentElement);

                        // First let's go through all inputs and see if any selector inputs are present.
                        for (size_t k = 0; k < data.size();){
                            event::base* currentInput = data[k];
        
                            Has_Mouse_Left_Click_Event = currentInput->has(input::key::types::LEFT_CLICK) && overlapsWithMouse;
                            Has_Enter_Press_Event = currentInput->has(input::key::types::ENTER) && in->currentKeyboardState[(uint8_t)input::key::types::ENTER].state;
        
                            // Check if the host is prime to be focused on
                            if ((Has_Mouse_Left_Click_Event || Has_Enter_Press_Event) && currentElement->isHovered()){
                                INTERNAL::updateFocusedElement(currentElement);
                                INTERNAL::unHoverElement();

                                // Remove the input, since it's job is used here:
                                data.erase(data.begin() + k);
                                continue;
                            }

                            // Criteria must be identical for more accurate criteria listing.
                            if (currentEventHandler.criteria == currentInput->criteria && currentElement->isFocused()){
                                try{
                                    // Check if this job could be run successfully.
                                    if (currentEventHandler.job(currentInput)){
                                        //dont let anyone else react to this event.
                                        data.erase(data.begin() + k);
                                        continue;
                                    }
                                    else{
                                        INTERNAL::reportStack("Job '" + currentEventHandler.ID + "' failed!");
                                    }
                                }
                                catch(std::exception& problem){
                                    INTERNAL::reportStack("In event: '" + currentEventHandler.ID + "' Problem: " + std::string(problem.what()));
                                }
                            }

                            k++;
                        }

                        // If the current event handler is not focused, then we can check wether to set it on/off onHovering
                        if (!currentElement->isFocused()) {
                            if (currentMouse.state != mouse::states::ENABLE) {
                                if (overlapsWithMouse){
                                    INTERNAL::updateHoveredElement(currentElement);
                                }
                                else {
                                    if (INTERNAL::hoveredOn == currentElement)
                                        INTERNAL::unHoverElement();
                                }
                            }
                        }
                    }
                }
                
                // If no event handler recognized these inputs, there is no need to keep them lingering for next time.
                data.clear();
            }
            
            /**
            * @brief Populate inputs for keys that are held down.
            * @details This function iterates over the current keyboard states and creates new input objects
            *          for keys that are held down and not already present in the inputs list. It skips mouse button keys.
            */
            void base::Populate_Inputs_For_Held_Down_Keys() {
                event::base* present = new event::base();

                for (size_t i = 0; i < in->currentKeyboardState.size(); i++) {

                    input::key::types currentKeyType = static_cast<input::key::types>(i + (size_t)input::key::types::__min);
                    input::key currentKey = in->currentKeyboardState[i];

                    // Check if the key is activated
                    if (currentKey.state) {

                        // Skip mouse button keys
                        if (input::key::types::LEFT_CLICK == currentKeyType || input::key::types::RIGHT_CLICK == currentKeyType || input::key::types::MIDDLE_CLICK == currentKeyType)
                            continue;

                        // Add the currently enabled key into the present registry
                        present->criteria.push_back(currentKeyType);
                    }
                }

                // Check if the input already exists
                bool Found = false;
                for (auto* input : data) {
                    if (input->criteria == present->criteria) {
                        Found = true;
                        break;
                    }
                }

                if (Found) return;

                data.push_back(present);
            }

            /**
            * @brief Processes mouse input events and updates the input list.
            * @details This function checks the state of mouse buttons (left, right, and middle)
            *          and determines if they have been pressed or clicked. It compares the current
            *          state with the previous state and the duration the button has been pressed.
            *          Based on these checks, it creates corresponding input objects and adds them
            *          to the Inputs list.
            * @note This function related on click events when the current keyboard state for that specific key is NOT on!
            */
            void base::mouseAPI() {
                auto currentTime = std::chrono::steady_clock::now();

                // Get the duration the left mouse button has been pressed
                std::chrono::steady_clock::duration mouseLeftClickPressedFor = currentTime - in->currentKeyboardState[(uint8_t)input::key::types::LEFT_CLICK].captureTime;

                // Check if the left mouse button is pressed and for how long
                if (in->currentKeyboardState[(uint8_t)input::key::types::LEFT_CLICK].state && mouseLeftClickPressedFor >= SETTINGS::mousePressDownCooldown) {
                    data.push_back(new event::base{input::key::types::LEFT_CLICK, input::key::types::DRAGGING});
                }
                // Check if the left mouse button was previously pressed and now released
                else if (!in->currentKeyboardState[(uint8_t)input::key::types::LEFT_CLICK].state && in->previousKeyboardState[(uint8_t)input::key::types::LEFT_CLICK].state != in->currentKeyboardState[(uint8_t)input::key::types::LEFT_CLICK].state) {
                    data.push_back(new event::base{input::key::types::LEFT_CLICK});
                }

                // Get the duration the right mouse button has been pressed
                std::chrono::steady_clock::duration mouseRightClickPressedFor = currentTime - in->currentKeyboardState[(uint8_t)input::key::types::RIGHT_CLICK].captureTime;

                // Check if the right mouse button is pressed and for how long
                if (in->currentKeyboardState[(uint8_t)input::key::types::RIGHT_CLICK].state && mouseRightClickPressedFor >= SETTINGS::mousePressDownCooldown) {
                    data.push_back(new event::base{input::key::types::RIGHT_CLICK, input::key::types::DRAGGING});
                }
                // Check if the right mouse button was previously pressed and now released
                else if (!in->currentKeyboardState[(uint8_t)input::key::types::RIGHT_CLICK].state && in->previousKeyboardState[(uint8_t)input::key::types::RIGHT_CLICK].state != in->currentKeyboardState[(uint8_t)input::key::types::RIGHT_CLICK].state) {
                    data.push_back(new event::base{input::key::types::RIGHT_CLICK});
                }

                // Get the duration the middle mouse button has been pressed
                std::chrono::steady_clock::duration Mouse_Middle_Pressed_For = currentTime - in->currentKeyboardState[(uint8_t)input::key::types::MIDDLE_CLICK].captureTime;

                // Check if the middle mouse button is pressed and for how long
                if (in->currentKeyboardState[(uint8_t)input::key::types::MIDDLE_CLICK].state && Mouse_Middle_Pressed_For >= SETTINGS::mousePressDownCooldown) {
                    data.push_back(new event::base{input::key::types::MIDDLE_CLICK, input::key::types::DRAGGING});
                }
                // Check if the middle mouse button was previously pressed and now released
                else if (!in->currentKeyboardState[(uint8_t)input::key::types::MIDDLE_CLICK].state && in->previousKeyboardState[(uint8_t)input::key::types::MIDDLE_CLICK].state != in->currentKeyboardState[(uint8_t)input::key::types::MIDDLE_CLICK].state) {
                    data.push_back(new event::base{input::key::types::MIDDLE_CLICK});
                }
            }

            /**
            * @brief Handles mouse scroll events.
            * @details This function checks if the mouse scroll up or down button has been pressed and if the focused element is not null.
            *          If the focused element is not null, it calls the scroll up or down function on the focused element.
            */
            void base::scrollAPI() {
                // Check if the mouse scroll up button has been pressed
                if (in->currentKeyboardState[(uint8_t)input::key::types::SCROLL_UP].state){
                    if (INTERNAL::focusedOn)    // If the focused element is not null, call the scroll up function
                        INTERNAL::focusedOn->scrollUp();
                } else if (in->currentKeyboardState[(uint8_t)input::key::types::SCROLL_DOWN].state){  // Check if the mouse scroll down button has been pressed
                    if (INTERNAL::focusedOn)    // If the focused element is not null, call the scroll down function
                        INTERNAL::focusedOn->scrollDown();
                }
            }

            /**
            * @brief Handles escape key press events.
            * @details This function checks if the escape key has been pressed and if the focused element is not null.
            *          If the focused element is not null, it calls the Un_Focus_Element function to remove the focus.
            *          If the focused element is null but the hovered element is not null, it calls the Un_Hover_Element
            *          function to remove the hover.
            */
            void base::handleEscape() {
                // Check if the escape key has been pressed
                if (!in->currentKeyboardState[(uint8_t)input::key::types::ESCAPE].state)
                    return;

                // If the focused element is not null, remove the focus
                if (INTERNAL::focusedOn){
                    INTERNAL::updateHoveredElement(INTERNAL::focusedOn); // Update the hovered element to be the focused element before un-focusing it.
                    INTERNAL::unFocusElement();
                }
                else if (INTERNAL::hoveredOn){
                    // If nothing is focused, ESC clears hover.
                    INTERNAL::unHoverElement();
                }
            }

            /**
            * @brief Handles the pressing of the tab key.
            * @details This function selects the next tabbed element as focused and not hovered.
            *          If the shift key is pressed, it goes backwards in the list of tabbed elements.
            */
            void base::handleTabulator() {
                // Check if the tab key has been pressed
                if (!in->currentKeyboardState[(uint8_t)input::key::types::TABULATOR].state)
                    return;

                if (INTERNAL::focusedOn) return;   // Tabulator is disabled from switching if an element is focused on, this gives us the ability to insert tabs into textFields.

                if (handlers.empty())
                    return;
                
                // return if there are only INTERNAL::main handlers
                bool Only_Main_Handlers = true;
                for (const auto* Handler : handlers){  // yes this is kinda dumb way of doing this but it works well...
                    if (Handler != INTERNAL::main){
                        Only_Main_Handlers = false;
                        break;
                    }
                }

                if (Only_Main_Handlers)
                    return;

                // Check if the shift key is pressed
                bool Shift_Is_Pressed = in->currentKeyboardState[(uint8_t)input::key::types::SHIFT].state;

                // Get the current element from the selected element
                element* Current = INTERNAL::hoveredOn;
                
                int Current_Index = 0;

                // Find the index of the current element in the list of event handlers
                if (Current){
                    // Find the first occurrence of the event handlers with this Current being their Host.
                    for (;(size_t)Current_Index < handlers.size(); Current_Index++){
                        if (handlers[(size_t)Current_Index] == Current)
                            break;
                    }
                }

                // Skip main::* handlers.
                do {
                    // Generalize index hopping, if shift is pressed then go backwards.
                    Current_Index += 1 + (-2 * Shift_Is_Pressed);

                    // If the index is out of bounds, wrap it around to the other side of the list
                    if (Current_Index < 0){
                        Current_Index = handlers.size() - 1;
                    }
                    else if ((size_t)Current_Index >= handlers.size()){
                        Current_Index = 0;
                    }
                } while ((size_t)Current_Index < handlers.size() && handlers[(size_t)Current_Index] == INTERNAL::main);

                // Now update the hovered element with the new index
                currentMouse.state = mouse::states::DISABLE;
                INTERNAL::updateHoveredElement(handlers[(size_t)Current_Index]);
            }

            void base::transformInput() {

            }
        }
    }   
}

