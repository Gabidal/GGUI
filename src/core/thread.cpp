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

        extern element* Main;

        namespace atomic{
            enum class status;

            extern std::mutex Mutex;
            extern std::condition_variable Condition;

            extern status Pause_Render_Thread;
        }

        extern std::chrono::high_resolution_clock::time_point Previous_Time;
        extern std::chrono::high_resolution_clock::time_point Current_Time;

        extern atomic::guard<Carry> Carry_Flags;

        extern void Translate_Inputs();

        bool Identical_Frame = true;

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
                    std::unique_lock lock(atomic::Mutex);
                    atomic::Condition.wait(lock, [&](){ return atomic::Pause_Render_Thread == atomic::status::REQUESTING_RENDERING; });

                    atomic::Pause_Render_Thread = atomic::status::RENDERING;
                }

                // Save current time, we have the right to overwrite unto the other thread, since they always run after each other and not at same time.
                Previous_Time = std::chrono::high_resolution_clock::now();

                // Check for carry signals if the rendering scheduler needs to be terminated.
                if (Carry_Flags.read().Terminate){
                    break;  // Break out of the loop if the terminate flag is set
                }

                if (Main){

                    // Process the previous carry flags
                    Carry_Flags([](Carry& previous_carry){
                        if (previous_carry.Resize){
                            // Clear the previous carry flag
                            previous_carry.Resize = false;

                            updateMaxWidthAndHeight();
                        }
                    });

                    Identical_Frame = true; // Assume that the incoming frame will be identical.

                    Abstract_Frame_Buffer = &Main->render();

                    if (!Identical_Frame){
                        if (SETTINGS::enableDRM) {
                            DRM::sendBuffer(*Abstract_Frame_Buffer);
                        }
                        else {
                            // ENCODE for optimize
                            encodeBuffer(Abstract_Frame_Buffer);

                            unsigned int Liquefied_Size = 0;
                            fastVector<compactString> CS_Buffer = liquifyUTFText(Abstract_Frame_Buffer, Liquefied_Size, Main->getWidth(), Main->getHeight());
                            
                            Frame_Buffer = To_String(CS_Buffer, Liquefied_Size);
                            
                            renderFrame();
                        }
                    }
                    else{
                    #ifdef GGUI_DEBUG
                        LOGGER::Log("Saved frame");
                    #endif

                        if (SETTINGS::enableDRM) {
                            std::vector<UTF> empty;
                            DRM::sendBuffer(empty);
                        }
                    }
                }

                // Check the difference of the time captured before render and now after render
                Current_Time = std::chrono::high_resolution_clock::now();

                Render_Delay = std::chrono::duration_cast<std::chrono::milliseconds>(Current_Time - Previous_Time).count();

                {
                    std::unique_lock lock(atomic::Mutex);
                    // Now for itself set it to sleep.
                    atomic::Pause_Render_Thread = atomic::status::PAUSED;
                    atomic::Condition.notify_all();
                }
            }

            LOGGER::Log("Render thread terminated!");
        
            // Give signal to other threads this one has paused for good.
            std::unique_lock lock(atomic::Mutex);
            // Now for itself set it to sleep.
            atomic::Pause_Render_Thread = atomic::status::TERMINATED;
            atomic::Condition.notify_all();
        }

        /**
         * @brief Iterates through all file stream handles and triggers change events.
         * @details This function goes through each file stream handle in the `File_Streamer_Handles` map.
         *          It checks if the handle is not a standard output stream, and if so, calls the `Changed` method
         *          on the file stream to trigger any associated change events.
         */
        void Go_Through_File_Streams(){
            for (auto& File_Handle : File_Streamer_Handles){
                // Check if the file handle is not a standard output stream
                if (File_Handle.second->Get_type() == FILE_STREAM_TYPE::READ) {
                    // Trigger change event for the file stream
                    File_Handle.second->Changed();
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
            for (auto i : Multi_Frame_Canvas) {
                // Advance the animation to the next frame
                i.first->setNextAnimationFrame();

                // Flush the updated state of the canvas
                i.first->flush(true);
            }

            // Adjust the event thread load if there are canvases to update
            if (Multi_Frame_Canvas.size() > 0) {
                Event_Thread_Load = Lerp(MIN_UPDATE_SPEED, MAX_UPDATE_SPEED, TIME::MILLISECOND * 16);
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
                    std::unique_lock lock(atomic::Mutex);

                    atomic::Condition.wait(lock, [&](){ 
                        return atomic::Pause_Render_Thread == atomic::status::PAUSED || atomic::Pause_Render_Thread == atomic::status::TERMINATED; 
                    });

                    if (atomic::Pause_Render_Thread == atomic::status::TERMINATED){
                        break;
                    }
                }

                // Reset the thread load counter
                Event_Thread_Load = 0;
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
                Event_Delay = std::chrono::duration_cast<std::chrono::milliseconds>(Current_Time - Previous_Time).count();

                CURRENT_UPDATE_SPEED = MIN_UPDATE_SPEED + (MAX_UPDATE_SPEED - MIN_UPDATE_SPEED) * (1 - Event_Thread_Load);

                // If ya want uncapped FPS, disable this sleep code:
                std::this_thread::sleep_for(std::chrono::milliseconds(
                    Max(
                        CURRENT_UPDATE_SPEED - Event_Delay, 
                        MIN_UPDATE_SPEED
                    )
                ));
            }
        
            LOGGER::Log("Event thread terminated!");
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
        
            LOGGER::Log("Input thread terminated!");
        }
    }
}