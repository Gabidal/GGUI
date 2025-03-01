#include "./addons.h"
#include "../../elements/element.h"
#include "../../elements/listView.h"
#include "../../elements/window.h"
#include "../../elements/textField.h"

#include "../renderer.h"

#include <vector>

namespace GGUI{
    std::vector<element*> Addons;

    namespace INTERNAL{
        extern window* Main;
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
        // initInspectTool();

        // Finally after all addons are loaded
        for (auto* a : Addons){
            INTERNAL::Main->addChild(a);
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
    std::string Get_Stats_Text(){
        return  "Encoded buffer: " + std::to_string(INTERNAL::Abstract_Frame_Buffer.size()) + "\n" + 
                "Raw buffer: " + std::to_string(INTERNAL::Frame_Buffer.size()) + "\n" +
                "Elements: " + std::to_string(INTERNAL::Main->getAllNestedElements().size()) + "\n" +
                "Render delay: " + std::to_string(INTERNAL::Render_Delay) + "ms\n" +
                "Event delay: " + std::to_string(INTERNAL::Event_Delay) + "ms\n" + 
                "Input delay: " + std::to_string(INTERNAL::Input_Delay) + "ms\n" + 
                "Resolution: " + std::to_string(INTERNAL::Max_Width) + "x" + std::to_string(INTERNAL::Max_Height) + "\n" +
                "Task scheduler: " + std::to_string(INTERNAL::CURRENT_UPDATE_SPEED) + "ms\n";
    }
    
    /**
     * @brief Updates the stats panel with the number of elements, render time, and event time.
     * @param Event The event that triggered the update.
     * @return True if the update was successful, false otherwise.
     * @details This function should be called by the main event loop to update the stats panel.
     */
    bool Update_Stats([[maybe_unused]] GGUI::Event* Event){
        // Check if the inspect tool is displayed
        element* Inspect_Tool = INTERNAL::Main->getElement("Inspect");

        if (!Inspect_Tool || !Inspect_Tool->isDisplayed())
            return false;

        // find the stats element
        textField* Stats = (textField*)INTERNAL::Main->getElement("STATS");

        // Update the stats
        Stats->setText(Get_Stats_Text());

        // return success
        return true;
    }

    /**
     * @brief Initializes the inspect tool.
     * @details This function initializes the inspect tool which is a debug tool that displays the number of elements, render time, and event time.
     * @see GGUI::Update_Stats
     */
    void initInspectTool(){
        const char* ERROR_LOGGER = "_ERROR_LOGGER_";

        Addons.push_back(new GGUI::listView(styling(
            width(0.5f) | height(1.0f) | 
            text_color(1.0f) | background_color(1.0f) |
            // Set the flow direction to column so the elements stack vertically
            flow_priority(DIRECTION::COLUMN) | 
            // Set the position of the list view to the right side of the main window
            position(
                STYLES::top + STYLES::center + STYLES::prioritize
            ) | 
            // Set the opacity of the list view to 0.8
            opacity(0.8f) |
            // Set the name of the list view to "Inspect"
            name("Inspect") |

            // STYLES::border |     <- will crash since the child nodes do not have borders enabled.

            // Add the error logger kidnapper:
            node(new window(
                styling(
                    width(1.0f) | height(0.5f) |
                    text_color(GGUI::COLOR::RED) | background_color(GGUI::COLOR::BLACK) |
                    border_color(GGUI::COLOR::RED) | border_background_color(GGUI::COLOR::BLACK) | 
                    STYLES::border | 
                    title("LOG: ") | 
                    // Set the name of the window to "LOG"
                    name(ERROR_LOGGER) | 
                    // Allow the window to overflow, so that the text can be seen even if it is longer than the window
                    allow_overflow(true)
                )
            )) | 
                        
            // Add a count for how many UTF are being streamed.
            node(new textField(
                styling(
                    align(ALIGN::LEFT) | 
                    width(1.0f) |
                    height(8) |
                    // Set the name of the text field to "STATS"
                    name("STATS")
                    // text(Get_Stats_Text().c_str())
                )
            )) | 

            // Hide the inspect tool by default
            // STYLES::hide | 

            on_init([](element* self){
                // Register an event handler to toggle the inspect tool on and off
                GGUI::INTERNAL::Main->on(Constants::SHIFT | Constants::CONTROL | Constants::KEY_PRESS, [self](GGUI::Event* e){
                    GGUI::Input* input = (GGUI::Input*)e;

                    // If the shift key or control key is pressed and the 'i' key is pressed, toggle the inspect tool
                    if (!INTERNAL::KEYBOARD_STATES[BUTTON_STATES::SHIFT].State && !INTERNAL::KEYBOARD_STATES[BUTTON_STATES::CONTROL].State && input->Data != 'i' && input->Data != 'I') 
                        return false;

                    // Toggle the inspect tool, so if it is hidden, show it and if it is shown, hide it
                    self->display(!self->isDisplayed());

                    // Return true to indicate that the event was handled
                    return true;
                }, true);

                // Remember the inspect tool, so it will be updated every second
                INTERNAL::Remember([](std::vector<Memory>& rememberable){
                    rememberable.push_back(
                        GGUI::Memory(
                            TIME::SECOND,
                            Update_Stats,
                            MEMORY_FLAGS::RETRIGGER,
                            "Update Stats"
                        )
                    );
                });
            })
        )));
    }

}