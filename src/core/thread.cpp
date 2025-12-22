#include "./utils/utils.h"
#include "./renderer.h"
#include "./utils/fileStreamer.h"
#include "./utils/settings.h"

#include "./utils/drm.h"

#include <thread>
#include <memory>
#include <mutex>

namespace GGUI{
    class element;
    namespace INTERNAL{

        extern element* main;

        namespace atomic{
            enum class status;

            extern std::mutex mutex;
            extern std::condition_variable condition;

            extern status pauseRenderThread;
        }

        extern std::chrono::high_resolution_clock::time_point Previous_Time;
        extern std::chrono::high_resolution_clock::time_point Current_Time;

        extern atomic::guard<carry> Carry_Flags;

        extern void Translate_Inputs();

        bool identicalFrame = true;

        int BEFORE_ENCODE_BUFFER_SIZE = 0;
        int AFTER_ENCODE_BUFFER_SIZE = 0;

        /**
         * @brief The Renderer function is responsible for managing the rendering loop.
         * It waits for a condition to resume rendering, processes rendering tasks, and
         * then pauses itself until the condition is met again.
         * 
         * The function performs the following steps:
         * 1. Waits for the render thread to be resumed.
         * 2. Saves the current time.
         * 3. Checks if the rendering scheduler needs to be terminated.
         * 4. Processes carry flags and updates the maximum width and height if needed.
         * 5. Renders the main frame buffer.
         * 6. Encodes the buffer for optimization.
         * 7. Converts the abstract frame buffer to a string and renders the frame.
         * 8. Calculates the render delay.
         * 9. Pauses the render thread and notifies all waiting threads.
         */
        void renderer(){
            while (true){
                {
                    std::unique_lock lock(atomic::mutex);
                    atomic::condition.wait(lock, [&](){ return atomic::pauseRenderThread == atomic::status::REQUESTING_RENDERING; });

                    atomic::pauseRenderThread = atomic::status::RENDERING;
                }

                // Save current time, we have the right to overwrite unto the other thread, since they always run after each other and not at same time.
                Previous_Time = std::chrono::high_resolution_clock::now();

                // Check for carry signals if the rendering scheduler needs to be terminated.
                if (Carry_Flags.read().terminate){
                    break;  // Break out of the loop if the terminate flag is set
                }

                if (main){

                    // Process the previous carry flags
                    Carry_Flags([](carry& previous_carry){
                        if (previous_carry.resize){
                            // Clear the previous carry flag
                            previous_carry.resize = false;

                            updateMaxWidthAndHeight();
                        }
                    });

                    identicalFrame = true; // Assume that the incoming frame will be identical.

                    // Main is zero size, before the DRM sends us the correct window size.
                    bool FirstDRMRender = GGUI::SETTINGS::enableDRM && (main->getWidth() == 0 && main->getHeight() == 0);

                    // Skip rendering until DRM sends us the window size.
                    if (!FirstDRMRender) {
                        abstractFrameBuffer = &main->render();

                        if (!identicalFrame){
                            if (SETTINGS::enableDRM) {
                                DRM::sendBuffer(*abstractFrameBuffer);
                            }
                            else {
                                // ENCODE for optimize
                                encodeBuffer(abstractFrameBuffer);

                                unsigned int Liquefied_Size = 0;
                                conveyorAllocator<compactString> CS_Buffer = liquifyUTFText(abstractFrameBuffer, Liquefied_Size, main->getWidth(), main->getHeight());
                                
                                frameBuffer = toString(CS_Buffer, Liquefied_Size);
                                
                                renderFrame();
                            }
                        }
                        else{
                        #ifdef GGUI_DEBUG
                            LOGGER::log("Saved frame");
                        #endif

                            if (SETTINGS::enableDRM) {
                                std::vector<UTF> empty;
                                DRM::sendBuffer(empty);
                            }
                        }
                    }
                }

                // Check the difference of the time captured before render and now after render
                Current_Time = std::chrono::high_resolution_clock::now();

                renderDelay = std::chrono::duration_cast<std::chrono::milliseconds>(Current_Time - Previous_Time).count();

                {
                    std::unique_lock lock(atomic::mutex);
                    // Now for itself set it to sleep.
                    atomic::pauseRenderThread = atomic::status::PAUSED;
                    atomic::condition.notify_all();
                }
            }

            LOGGER::log("Render thread terminated!");
        
            // Give signal to other threads this one has paused for good.
            std::unique_lock lock(atomic::mutex);
            // Now for itself set it to sleep.
            atomic::pauseRenderThread = atomic::status::TERMINATED;
            atomic::condition.notify_all();
        }

        /**
         * @brief Iterates through all file stream handles and triggers change events.
         * @details This function goes through each file stream handle in the `fileStreamerHandles` map.
         *          It checks if the handle is not a standard output stream, and if so, calls the `Changed` method
         *          on the file stream to trigger any associated change events.
         */
        void Go_Through_File_Streams(){
            for (auto& pair : fileStreamerHandles){
                auto& handle = pair.second;
                if (handle && handle->getType() == FILE_STREAM_TYPE::READ){
                    handle->changed();
                }
            }
        }

        /**
         * @brief Refreshes the state of all multi-frame canvases by advancing their animations and flushing their updated states.
         * 
         * This function iterates over each multi-frame canvas, advances its animation to the next frame, and flushes the updated state.
         * If there are canvases to update, it adjusts the event thread load based on the number of canvases.
         */
        void Refresh_Multi_Frame_Canvas() {
            // Iterate over each multi-frame canvas
            for (auto i : multiFrameCanvas) {
                // Advance the animation to the next frame
                i.first->setNextAnimationFrame();

                // Flush the updated state of the canvas
                i.first->flush(true);
            }

            // Adjust the event thread load if there are canvases to update
            if (multiFrameCanvas.size() > 0) {
                eventThreadLoad = lerp(MIN_UPDATE_SPEED, MAX_UPDATE_SPEED, 0);
            }
        }

        /**
         * @brief Event_Thread is a function that runs an infinite loop to handle various events and tasks.
         * 
         * This function performs the following tasks in each iteration of the loop:
         * - Resets the thread load counter and updates the previous time.
         * - Calls functions to recall memories, go through file streams, and refresh the multi-frame canvas.
         * - Checks for termination signals and breaks out of the loop if the terminate flag is set.
         * - Updates the current time and calculates the delta time.
         * - Adjusts the current update speed based on the event thread load.
         * - Sleeps for a calculated duration to control the update speed.
         * 
         * The function is designed to be used in a multi-threaded environment where it can be paused and resumed as needed.
         * 
         * @note If uncapped FPS is desired, the sleep code can be disabled.
         */
        void eventThread(){
            while (true){
                {
                    std::unique_lock lock(atomic::mutex);

                    atomic::condition.wait(lock, [&](){ 
                        return atomic::pauseRenderThread == atomic::status::PAUSED || atomic::pauseRenderThread == atomic::status::TERMINATED; 
                    });

                    if (atomic::pauseRenderThread == atomic::status::TERMINATED){
                        break;
                    }
                }

                // Reset the thread load counter
                eventThreadLoad = 0;
                Previous_Time = std::chrono::high_resolution_clock::now();

                // Order independent --------------
                recallMemories();
                Go_Through_File_Streams();
                Refresh_Multi_Frame_Canvas();

                /* 
                    Notice: Since the Rendering thread will use its own access to render as tickets, so every time it is "REQUESTING_PAUSE" it will after its own run set itself to PAUSED.
                    This is what Tickets are.
                    So in other words, if there is MUST use of rendering pipeline, use Update_Frame().
                */  
                // Resume_GGUI();

                Current_Time = std::chrono::high_resolution_clock::now();

                // Calculate the delta time.
                eventDelay = std::chrono::duration_cast<std::chrono::milliseconds>(Current_Time - Previous_Time).count();

                CURRENT_UPDATE_SPEED = MIN_UPDATE_SPEED + (MAX_UPDATE_SPEED - MIN_UPDATE_SPEED) * (1 - eventThreadLoad);

                // If ya want uncapped FPS, disable this sleep code:
                std::this_thread::sleep_for(std::chrono::milliseconds(
                    Max(
                        CURRENT_UPDATE_SPEED - eventDelay, 
                        MIN_UPDATE_SPEED
                    )
                ));
            }
        
            LOGGER::log("Event thread terminated!");
        }
    
        /**
         * @brief Function that continuously handles user input in a separate thread.
         *
         * This function runs an infinite loop where it performs the following steps:
         * 1. Waits for user input by calling Query_Inputs().
         * 2. Pauses the GGUI system and performs the following actions:
         *    - Records the current time as Previous_Time.
         *    - Translates the queried inputs using Translate_Inputs().
         *    - Processes scroll and mouse inputs using SCROLL_API() and MOUSE_API().
         *    - Calls the event handlers to react to the parsed input using Event_Handler().
         *    - Records the current time as Current_Time.
         *    - Calculates the delta time (input delay) and stores it in Input_Delay.
         */
        void inputThread(){
            while (true){
                if (SETTINGS::enableDRM) {
                    DRM::pollInputs();
                }
                else {
                    // Wait for user input.
                    queryInputs();
                }

                pauseGGUI([&](){
                    Previous_Time = std::chrono::high_resolution_clock::now();

                    if (SETTINGS::enableDRM) {
                        DRM::translateInputs();
                    }
                    else {
                        // Translate the Queried inputs.
                        Translate_Inputs();
                    }

                    // Translate the movements thingies to better usable for user.
                    scrollAPI();
                    mouseAPI();

                    // Now call upon event handlers which may react to the parsed input.
                    eventHandler();

                    Current_Time = std::chrono::high_resolution_clock::now();

                    // Calculate the delta time.
                    Input_Delay = std::chrono::duration_cast<std::chrono::milliseconds>(Current_Time - Previous_Time).count();
                });
            }
        
            LOGGER::log("Input thread terminated!");
        }
    }
}