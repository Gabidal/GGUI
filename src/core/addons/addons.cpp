#include "./addons.h"
#include "../../elements/element.h"
#include "../../elements/listView.h"
#include "../../elements/textField.h"

#include "../renderer.h"
#include "../utils/utils.h"

#include <vector>

namespace GGUI{
    std::vector<element*> addons;

    namespace INTERNAL{
        extern element* main;
    }

    /**
     * @brief Initializes all addons and adds them to the main internal structure.
     *
     * This function first calls the initializer for the inspect tool addon.
     * After all addons are loaded, it iterates through the list of addons
     * and adds each one to the main internal structure.
     */
    void initAddons(){
        // Call addon initializers.
        initInspectTool();

        // Finally after all addons are loaded
        for (auto* a : addons){
            getRoot()->addChild(a);
        }
    }

    /**
     * @brief Generates a string containing various internal statistics.
     * 
     * This function collects and formats several internal statistics into a single
     * string. The statistics include:
     * - Encoded buffer size
     * - Raw buffer size
     * - Number of elements
     * - Render delay in milliseconds
     * - Event delay in milliseconds
     * - Input delay in milliseconds
     * - Resolution (width x height)
     * - Task scheduler update speed in milliseconds
     * 
     * @return A formatted string containing the collected statistics.
     */
    std::string getStatsText(){
        std::string optimized = std::to_string((float)(INTERNAL::BEFORE_ENCODE_BUFFER_SIZE - INTERNAL::AFTER_ENCODE_BUFFER_SIZE) / (float)INTERNAL::Max(INTERNAL::BEFORE_ENCODE_BUFFER_SIZE, 1) * 100.0f);

        // cut from the decimal point
        optimized = optimized.substr(0, optimized.find('.'));

        return  "Optimized: " + optimized + "%\n" + 
                "Elements: " + std::to_string(getRoot()->getAllNestedElements().size()) + "\n" +
                "Render delay: " + std::to_string(INTERNAL::renderDelay) + "ms\n" +
                "Event delay: " + std::to_string(INTERNAL::eventDelay) + "ms\n" + 
                "Input delay: " + std::to_string(INTERNAL::Input_Delay) + "ms\n" + 
                "Resolution: " + std::to_string(INTERNAL::maxWidth) + "x" + std::to_string(INTERNAL::maxHeight) + "\n" +
                "Task scheduler: " + std::to_string(INTERNAL::CURRENT_UPDATE_SPEED) + "ms\n" + 
                "Mouse: {" + std::to_string(INTERNAL::mouse.x) + ", " + std::to_string(INTERNAL::mouse.y) + "}";
    }
    
    /**
     * @brief Updates the stats panel with the number of elements, render time, and event time.
     * @param Event The event that triggered the update.
     * @return True if the update was successful, false otherwise.
     * @details This function should be called by the main event loop to update the stats panel.
     */
    bool updateStats([[maybe_unused]] GGUI::event* Event){
        // Check if the inspect tool is displayed
        element* Inspect_Tool = getRoot()->getElement("Inspect");

        if (!Inspect_Tool || !Inspect_Tool->isDisplayed())
            return false;

        // find the stats element
        textField* Stats = (textField*)getRoot()->getElement("STATS");

        if (!Stats) // This normally should not happen, but can happen if main thread is lagging behind.
            return false;

        std::string new_stats = getStatsText();

        // Update the stats
        if (new_stats != Stats->getText()){
            Stats->setText(new_stats);
        }

        return true;
    }

    /**
     * @brief Initializes the inspect tool.
     * @details This function initializes the inspect tool which is a debug tool that displays the number of elements, render time, and event time.
     * @see GGUI::updateStats
     */
    void initInspectTool(){
        const char* ERROR_LOGGER = "_ERROR_LOGGER_";

        addons.push_back(new GGUI::listView(
            width(0.5f) | height(1.0f) | 
            textColor(1.0f) | backgroundColor(GGUI::COLOR::BLACK) |
            // Set the flow direction to column so the elements stack vertically
            flowPriority(DIRECTION::COLUMN) | 
            // Set the position of the list view to the right side of the main window
            position(
                STYLES::top + STYLES::right + STYLES::prioritize
            ) | 
            // Set the opacity of the list view to 0.8
            opacity(0.8f) |
            // Set the name of the list view to "Inspect"
            name("Inspect") |

            // enable_border(true) |     // <- will crash since the child nodes do not have borders enabled.

            // Add the error logger kidnapper:
            node(new element(
                width(1.0f) | height(0.5f) |
                enableBorder(true) | 
                title("LOG: ") | 
                // Set the name of the window to "LOG"
                name(ERROR_LOGGER) | 
                // Allow the window to overflow, so that the text can be seen even if it is longer than the window
                allowOverflow(true)
            )) | 
                        
            // Add a count for how many UTF are being streamed.
            node(new textField(
                anchor(ANCHOR::LEFT) | 
                width(1.0f) |
                height(9) |
                // Set the name of the text field to "STATS"
                name("STATS")
                // text(getStatsText().c_str())
            )) | 

            // Hide the inspect tool by default
            display(false) | 

            onInit([](element* self){
                // Register an event handler to toggle the inspect tool on and off
                GGUI::getRoot()->on(constants::SHIFT | constants::CONTROL | constants::KEY_PRESS, [self](GGUI::event* e){
                    GGUI::input* input = (GGUI::input*)e;

                    // If the shift key or control key is pressed and the 'i' key is pressed, toggle the inspect tool
                    if (!INTERNAL::KEYBOARD_STATES[KEYBOARD_BUTTONS::SHIFT].state && !INTERNAL::KEYBOARD_STATES[KEYBOARD_BUTTONS::CONTROL].state && input->data != 'i' && input->data != 'I') 
                        return false;

                    // Toggle the inspect tool, so if it is hidden, show it and if it is shown, hide it
                    self->display(!self->isDisplayed());

                    // Return true to indicate that the event was handled
                    return true;
                }, true);

                // Remember the inspect tool, so it will be updated every second
                INTERNAL::remember([](std::vector<memory>& rememberable){
                    rememberable.push_back(
                        GGUI::memory(
                            TIME::SECOND,
                            updateStats,
                            MEMORY_FLAGS::RETRIGGER,
                            "Update Stats"
                        )
                    );
                });
            })
        ));
    }

}