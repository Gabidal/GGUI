#include "converter.h"
#include "core.h"
#include "thread.h"

#include "backend/terminal.h"

#include "utils/settings.h"
#include "utils/drm.h"
#include "utils/logger.h"

namespace GGUI::core {
    extern selectable focusedOn;
    extern selectable hoveredOn;
}

namespace GGUI::converter {
    namespace input {
        base::base() {
            pollingThread = std::thread([this](){
                this->inputThread();
            });
        }

        /**
        * @brief polls input from terminal or DRM backend, and then converts it into events.
        */
        void base::inputThread(){
            logger::log("Input thread starting...");

            while (true){
                {
                    std::unique_lock lock(thread::concurrency::mutex);

                    if (thread::concurrency::requestTermination){
                        break;
                    }
                }

                if (SETTINGS::enableDRM) {
                    DRM::pollInputs();
                }
                else if (!terminal::currentStates || !out || !terminal::currentStates->transmission.isConnected()) {    // platform initialization is still in progress
                    std::this_thread::sleep_for(std::chrono::seconds(1));
                    continue;
                }
                else {
                    terminal::currentStates->transmission.pollInput();

                    if (terminal::currentStates->transmission.inputSize == 0) {
                        continue;   // Do not lock or unlock rendering thread if this was just a input poll timeout!
                    }
                }

                pauseGGUI([&](){
                    if (SETTINGS::enableDRM) {
                        DRM::translateInputs();
                    }
                    else {
                        // Translate the Queried inputs.
                        terminal::currentStates->parseInput();
                    }

                    // Call the linked output registry to convert the incoming data.
                    out->transformInput();
                });
            }
        
            logger::log("Input thread terminated!");
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

            // All elements that have event handlers
            for (unsigned int i = 0; i < handlers.size(); i++){
                element* currentElement = handlers[i];
                
                if (!currentElement->getDisplay())
                    continue;   // Skip eventhandlers where their host is not active
                
                bool overlapsWithMouse = currentMouse.collides(currentElement);

                const std::vector<event::action>& currentEventhandlers = currentElement->getEventHandlers();

                // Individual event handlers from the current element
                for (unsigned int j = 0; j < currentElement->getEventHandlers().size(); j++) {
                    const event::action& currentEventHandler = currentEventhandlers[j];

                    // The reason these are held over multitude of inputs, is for scenario where this memory thread has not run in a long time and has a long query of inputs-
                    // and in this same listing of inputs at the start is the mouse click or enter and the user given inputs for that specifically activated event handler.
                    bool Has_Mouse_Left_Click_Event = false;
                    bool Has_Enter_Press_Event = false;

                    // First let's go through all inputs and see if any selector inputs are present.
                    for (size_t k = 0; k < data.size();){
                        event::base* currentInput = data[k];
    
                        Has_Mouse_Left_Click_Event = currentInput->has(input::key::types::LEFT_CLICK) && overlapsWithMouse;
                        Has_Enter_Press_Event = currentInput->has(input::key::types::ENTER);
    
                        // Check if the host is prime to be focused on
                        if ((Has_Mouse_Left_Click_Event || Has_Enter_Press_Event) && core::hoveredOn == currentElement){
                            core::focusedOn.link(currentElement);
                            core::hoveredOn.unlink();

                            // Remove the input, since it's job is used here:
                            data.erase(data.begin() + (long)k);
                            continue;
                        }

                        // Criteria must be identical for more accurate criteria listing.
                        if (currentInput->contains(static_cast<const event::base*>(&currentEventHandler)) && core::focusedOn == currentElement){
                            try{
                                event::action::responses currentResponse = currentEventHandler.job(currentInput);

                                switch (currentResponse) {
                                    case event::action::responses::DONE: {
                                        // Remove the matched criteria and push them back into data to be checked later with another handler
                                        event::selectedKeys difference = currentInput->getDifference(static_cast<const event::base*>(&currentEventHandler));
                                        data.push_back(new event::base(difference));
                                        // remove the used data
                                        data.erase(data.begin() + (long)k);
                                        break;
                                    }
                                    case event::action::responses::FAILED:
                                        logger::log("Job '" + currentEventHandler.ID + "' failed!");
                                        break;
                                    case event::action::responses::PROPAGATE:
                                        break;
                                }
                            }
                            catch(std::exception& problem){
                                logger::log("In event: '" + currentEventHandler.ID + "' Problem: " + std::string(problem.what()));
                            }
                        }

                        k++;
                    }

                }

                // If the current event handler is not focused, then we can check wether to set it on/off onHovering
                if (currentElement != core::focusedOn) {
                    if (currentMouse.state != mouse::states::ENABLE) {
                        if (overlapsWithMouse){
                            core::hoveredOn.link(currentElement);
                        }
                        else {
                            if (core::hoveredOn == currentElement)
                                core::hoveredOn.unlink();
                        }
                    }
                }
            }
            
            // If no event handler recognized these inputs, there is no need to keep them lingering for next time.
            data.clear();
        }

        void base::populateInputEventsForHeldDownKeys() {
            event::base* present = new event::base();

            for (size_t i = (size_t)input::key::types::__min; i < (size_t)input::key::types::__max; i++) {

                input::key::types currentKeyType = static_cast<input::key::types>(i);
                input::key currentKey = in->currentKeyboardState[table::getRelative(currentKeyType)];

                // Check if the key is activated
                if (currentKey.getState()) {

                    // Skip mouse button keys
                    if (input::key::types::LEFT_CLICK == currentKeyType || input::key::types::RIGHT_CLICK == currentKeyType || input::key::types::MIDDLE_CLICK == currentKeyType)
                        continue;

                    // Add the currently enabled key into the present registry
                    present->criteria.push_back(currentKeyType);
                }
            }

            // Check if the input already exists
            for (auto* input : data) {
                if (input->criteria != present->criteria) continue;

                return; // This event already exists.
            }

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
            std::chrono::steady_clock::duration mouseLeftClickPressedFor = currentTime - in->currentKeyboardState[table::getRelative(input::key::types::LEFT_CLICK)].getCaptureTime();

            // Check if the left mouse button is pressed and for how long
            if (in->currentKeyboardState[table::getRelative(input::key::types::LEFT_CLICK)].getState() && mouseLeftClickPressedFor >= SETTINGS::mousePressDownCooldown) {
                data.push_back(new event::base({input::key::types::LEFT_CLICK, input::key::types::DRAGGING}));
            }
            // Check if the left mouse button was previously pressed and now released
            else if (!in->currentKeyboardState[table::getRelative(input::key::types::LEFT_CLICK)].getState() && in->previousKeyboardState[table::getRelative(input::key::types::LEFT_CLICK)].getState() != in->currentKeyboardState[table::getRelative(input::key::types::LEFT_CLICK)].getState()) {
                data.push_back(new event::base({input::key::types::LEFT_CLICK}));
            }

            // Get the duration the right mouse button has been pressed
            std::chrono::steady_clock::duration mouseRightClickPressedFor = currentTime - in->currentKeyboardState[table::getRelative(input::key::types::RIGHT_CLICK)].getCaptureTime();

            // Check if the right mouse button is pressed and for how long
            if (in->currentKeyboardState[table::getRelative(input::key::types::RIGHT_CLICK)].getState() && mouseRightClickPressedFor >= SETTINGS::mousePressDownCooldown) {
                data.push_back(new event::base({input::key::types::RIGHT_CLICK, input::key::types::DRAGGING}));
            }
            // Check if the right mouse button was previously pressed and now released
            else if (!in->currentKeyboardState[table::getRelative(input::key::types::RIGHT_CLICK)].getState() && in->previousKeyboardState[table::getRelative(input::key::types::RIGHT_CLICK)].getState() != in->currentKeyboardState[table::getRelative(input::key::types::RIGHT_CLICK)].getState()) {
                data.push_back(new event::base({input::key::types::RIGHT_CLICK}));
            }

            // Get the duration the middle mouse button has been pressed
            std::chrono::steady_clock::duration Mouse_Middle_Pressed_For = currentTime - in->currentKeyboardState[table::getRelative(input::key::types::MIDDLE_CLICK)].getCaptureTime();

            // Check if the middle mouse button is pressed and for how long
            if (in->currentKeyboardState[table::getRelative(input::key::types::MIDDLE_CLICK)].getState() && Mouse_Middle_Pressed_For >= SETTINGS::mousePressDownCooldown) {
                data.push_back(new event::base({input::key::types::MIDDLE_CLICK, input::key::types::DRAGGING}));
            }
            // Check if the middle mouse button was previously pressed and now released
            else if (!in->currentKeyboardState[table::getRelative(input::key::types::MIDDLE_CLICK)].getState() && in->previousKeyboardState[table::getRelative(input::key::types::MIDDLE_CLICK)].getState() != in->currentKeyboardState[table::getRelative(input::key::types::MIDDLE_CLICK)].getState()) {
                data.push_back(new event::base({input::key::types::MIDDLE_CLICK}));
            }
        }

        /**
        * @brief Handles mouse scroll events.
        * @details This function checks if the mouse scroll up or down button has been pressed and if the focused element is not null.
        *          If the focused element is not null, it calls the scroll up or down function on the focused element.
        */
        void base::scrollAPI() {
            // Check if the mouse scroll up button has been pressed
            if (in->currentKeyboardState[table::getRelative(input::key::types::SCROLL_UP)].getState()){
                if (core::focusedOn != nullptr && dynamic_cast<scrollView*>(core::focusedOn.operate()))    // If the focused element is not null, call the scroll up function
                    dynamic_cast<scrollView*>(core::focusedOn.operate())->scrollUp();
            } else if (in->currentKeyboardState[table::getRelative(input::key::types::SCROLL_DOWN)].getState()){  // Check if the mouse scroll down button has been pressed
                if (core::focusedOn != nullptr && dynamic_cast<scrollView*>(core::focusedOn.operate()))    // If the focused element is not null, call the scroll down function
                    dynamic_cast<scrollView*>(core::focusedOn.operate())->scrollDown();
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
            if (!in->currentKeyboardState[table::getRelative(input::key::types::ESCAPE)].getState())
                return;

            // If the focused element is not null, remove the focus
            if (core::focusedOn != nullptr){
                core::hoveredOn.link(core::focusedOn.operate()); // Update the hovered element to be the focused element before un-focusing it.
                core::focusedOn.unlink();
            }
            else if (core::hoveredOn != nullptr){
                // If nothing is focused, ESC clears hover.
                core::hoveredOn.unlink();
            }
        }

        /**
        * @brief Handles the pressing of the tab key.
        * @details This function selects the next tabbed element as focused and not hovered.
        *          If the shift key is pressed, it goes backwards in the list of tabbed elements.
        */
        void base::handleTabulator() {
            // Check if the tab key has been pressed
            if (!in->currentKeyboardState[table::getRelative(input::key::types::TABULATOR)].getState())
                return;

            if (core::focusedOn != nullptr) return;   // Tabulator is disabled from switching if an element is focused on, this gives us the ability to insert tabs into textFields.

            if (handlers.empty())
                return;
            
            // return if there are only core::main handlers
            bool Only_Main_Handlers = true;
            for (const auto* Handler : handlers){  // yes this is kinda dumb way of doing this but it works well...
                if (Handler != core::main){
                    Only_Main_Handlers = false;
                    break;
                }
            }

            if (Only_Main_Handlers)
                return;

            // Check if the shift key is pressed
            bool Shift_Is_Pressed = in->currentKeyboardState[table::getRelative(input::key::types::SHIFT)].getState();

            int Current_Index = 0;

            // Find the index of the current element in the list of event handlers
            if (core::hoveredOn != nullptr){
                // Find the first occurrence of the event handlers with this Current being their Host.
                for (;(size_t)Current_Index < handlers.size(); Current_Index++){
                    if (handlers[(size_t)Current_Index] == core::hoveredOn)
                        break;
                }
            }

            // Skip main::* handlers.
            do {
                // Generalize index hopping, if shift is pressed then go backwards.
                Current_Index += 1 + (-2 * Shift_Is_Pressed);

                // If the index is out of bounds, wrap it around to the other side of the list
                if (Current_Index < 0){
                    Current_Index = (int)handlers.size() - 1;
                }
                else if ((size_t)Current_Index >= handlers.size()){
                    Current_Index = 0;
                }
            } while ((size_t)Current_Index < handlers.size() && handlers[(size_t)Current_Index] == core::main);

            // Now update the hovered element with the new index
            currentMouse.state = mouse::states::DISABLE;
            core::hoveredOn.link(handlers[(size_t)Current_Index]);
        }

        void base::transformInput() {
            // Translators
            populateInputEventsForHeldDownKeys();   // Read converter::input keys and create a single large event containing everything that just was polled.
            mouseAPI();
            scrollAPI();

            // Internal consumers
            handleEscape();
            handleTabulator();

            // UI event handler feeder
            eventHandler();
        }
    }
}