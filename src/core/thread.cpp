#include "thread.h"
#include "./utils/guard.h"
#include "./utils/utils.h"
#include "./renderer.h"
#include "./../elements/fileStreamer.h"

#include <thread>
#include <memory>
#include <mutex>

namespace GGUI{
    class window;
    namespace INTERNAL{

        extern window* Main;

        namespace atomic{
            enum class status;

            extern std::mutex Mutex;
            extern std::condition_variable Condition;

            extern status Pause_Render_Thread;
        }

        extern std::chrono::high_resolution_clock::time_point Previous_Time;
        extern std::chrono::high_resolution_clock::time_point Current_Time;

        extern atomic::Guard<Carry> Carry_Flags;

        extern void Translate_Inputs();

        bool Identical_Frame = false;

        // Async-Signal-Safe global flag.
        volatile sig_atomic_t Terminate = false;

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
                    std::unique_lock lock(INTERNAL::atomic::Mutex);
                    INTERNAL::atomic::Condition.wait(lock, [&](){ return INTERNAL::atomic::Pause_Render_Thread == INTERNAL::atomic::status::RESUMED; });

                    INTERNAL::atomic::Pause_Render_Thread = INTERNAL::atomic::status::LOCKED;
                }

                // Save current time, we have the right to overwrite unto the other thread, since they always run after each other and not at same time.
                INTERNAL::Previous_Time = std::chrono::high_resolution_clock::now();

                // Check for carry signals if the rendering scheduler needs to be terminated.
                // if (Terminate){
                //     break;  // Break out of the loop if the terminate flag is set
                // }

                if (INTERNAL::Main){

                    // Process the previous carry flags
                    INTERNAL::Carry_Flags([](GGUI::INTERNAL::Carry& previous_carry){
                        if (previous_carry.Resize){
                            // Clear the previous carry flag
                            previous_carry.Resize = false;

                            INTERNAL::updateMaxWidthAndHeight();
                        }
                    });

                    Identical_Frame = false;

                    INTERNAL::Abstract_Frame_Buffer = INTERNAL::Main->render();

                    if (Identical_Frame){
                        // ENCODE for optimize
                        encodeBuffer(INTERNAL::Abstract_Frame_Buffer);

                        INTERNAL::Frame_Buffer = liquifyUTFText(INTERNAL::Abstract_Frame_Buffer, INTERNAL::Main->getWidth(), INTERNAL::Main->getHeight())->To_String();
                        
                        INTERNAL::renderFrame();
                    }
                }

                // Check the difference of the time captured before render and now after render
                INTERNAL::Current_Time = std::chrono::high_resolution_clock::now();

                INTERNAL::Render_Delay = std::chrono::duration_cast<std::chrono::milliseconds>(INTERNAL::Current_Time - INTERNAL::Previous_Time).count();

                {
                    std::unique_lock lock(INTERNAL::atomic::Mutex);
                    // Now for itself set it to sleep.
                    INTERNAL::atomic::Pause_Render_Thread = INTERNAL::atomic::status::PAUSED;
                    INTERNAL::atomic::Condition.notify_all();
                }
            }
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
            for (auto i : INTERNAL::Multi_Frame_Canvas) {
                // Advance the animation to the next frame
                i.first->setNextAnimationFrame();

                // Flush the updated state of the canvas
                i.first->flush(true);
            }

            // Adjust the event thread load if there are canvases to update
            if (INTERNAL::Multi_Frame_Canvas.size() > 0) {
                INTERNAL::Event_Thread_Load = Lerp(INTERNAL::MIN_UPDATE_SPEED, INTERNAL::MAX_UPDATE_SPEED, TIME::MILLISECOND * 16);
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
                pauseGGUI([&](){
                    // Reset the thread load counter
                    INTERNAL::Event_Thread_Load = 0;
                    INTERNAL::Previous_Time = std::chrono::high_resolution_clock::now();

                    // Order independent --------------
                    recallMemories();
                    Go_Through_File_Streams();
                    Refresh_Multi_Frame_Canvas();
                });

                // Check for carry signals if the event scheduler needs to be terminated.
                // if (INTERNAL::Carry_Flags.Read().Terminate){
                //     break;  // Break out of the loop if the terminate flag is set
                // }

                /* 
                    Notice: Since the Rendering thread will use its own access to render as tickets, so every time it is "RESUMED" it will after its own run set itself to PAUSED.
                    This is what Tickets are.
                    So in other words, if there is MUST use of rendering pipeline, use Update_Frame().
                */  
                // Resume_GGUI();

                INTERNAL::Current_Time = std::chrono::high_resolution_clock::now();

                // Calculate the delta time.
                INTERNAL::Event_Delay = std::chrono::duration_cast<std::chrono::milliseconds>(INTERNAL::Current_Time - INTERNAL::Previous_Time).count();

                INTERNAL::CURRENT_UPDATE_SPEED = INTERNAL::MIN_UPDATE_SPEED + (INTERNAL::MAX_UPDATE_SPEED - INTERNAL::MIN_UPDATE_SPEED) * (1 - INTERNAL::Event_Thread_Load);

                // If ya want uncapped FPS, disable this sleep code:
                std::this_thread::sleep_for(std::chrono::milliseconds(
                    Max(
                        INTERNAL::CURRENT_UPDATE_SPEED - INTERNAL::Event_Delay, 
                        INTERNAL::MIN_UPDATE_SPEED
                    )
                ));
            }
        
            LOGGER::Log("Event thread terminated!");
        }
    
        /**
         * @brief Function that continuously handles user input in a separate thread.
         *
         * This function runs an infinite loop where it performs the following steps:
         * 1. Waits for user input by calling INTERNAL::Query_Inputs().
         * 2. Pauses the GGUI system and performs the following actions:
         *    - Records the current time as INTERNAL::Previous_Time.
         *    - Translates the queried inputs using INTERNAL::Translate_Inputs().
         *    - Processes scroll and mouse inputs using SCROLL_API() and MOUSE_API().
         *    - Calls the event handlers to react to the parsed input using Event_Handler().
         *    - Records the current time as INTERNAL::Current_Time.
         *    - Calculates the delta time (input delay) and stores it in INTERNAL::Input_Delay.
         */
        void inputThread(){
            while (true){
                // Wait for user input.
                INTERNAL::queryInputs();

                pauseGGUI([&](){
                    INTERNAL::Previous_Time = std::chrono::high_resolution_clock::now();

                    // Translate the Queried inputs.
                    INTERNAL::Translate_Inputs();

                    // Translate the movements thingies to better usable for user.
                    scrollAPI();
                    mouseAPI();

                    // Now call upon event handlers which may react to the parsed input.
                    eventHandler();

                    INTERNAL::Current_Time = std::chrono::high_resolution_clock::now();

                    // Calculate the delta time.
                    INTERNAL::Input_Delay = std::chrono::duration_cast<std::chrono::milliseconds>(INTERNAL::Current_Time - INTERNAL::Previous_Time).count();
                });
            }
        
            LOGGER::Log("Input thread terminated!");
        }
    }
}