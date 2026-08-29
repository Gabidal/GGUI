#include "utils/utils.h"
#include "utils/settings.h"
#include "utils/settings.h"

#include "thread.h"

#include "core.h"
#include "backend/terminal.h"

#include "../elements/canvas.h"

#include <thread>
#include <mutex>
#include <csignal>

namespace GGUI{
    class element;

    namespace core {
        extern std::unordered_map<canvas*, bool> multiFrameCanvas;
    }

    namespace thread {
        namespace concurrency {
            std::mutex mutex;
            std::condition_variable condition;
            
            int LOCKED = 0;
            status pauseRenderThread = status::NOT_INITIALIZED;

            sig_atomic_t requestTermination = false;
        }

        std::chrono::steady_clock::time_point Previous_Time;
        std::chrono::steady_clock::time_point Current_Time;

        bool identicalFrame = true;

        // Represents the update speed of each elapsed loop of passive events, which do NOT need user as an input.
        std::chrono::steady_clock::duration CURRENT_UPDATE_SPEED = SETTINGS::MAX_UPDATE_SPEED;
        inline float eventThreadLoad = 0.0f;  // Describes the load of animation and events from 0.0 to 1.0. Will reduce the event thread pause.

        std::chrono::steady_clock::duration renderDelay;    // describes how long previous render cycle took in ms
        std::chrono::steady_clock::duration eventDelay;    // describes how long previous memory tasks took in ms

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
            logger::log("Render thread starting...");

            while (true){
                {
                    std::unique_lock lock(concurrency::mutex);
                    concurrency::condition.wait(lock, [&](){ return concurrency::pauseRenderThread == status::REQUESTING_RENDERING || concurrency::requestTermination; });

                    concurrency::pauseRenderThread = status::RENDERING;
                }

                // Save current time, we have the right to overwrite unto the other thread, since they always run after each other and not at same time.
                Previous_Time = std::chrono::steady_clock::now();

                // Check for carry signals if the rendering scheduler needs to be terminated.
                if (concurrency::requestTermination){
                    break;  // Break out of the loop if the terminate flag is set
                }

                if (getRoot()){
                    identicalFrame = true; // Assume that the incoming frame will be identical.

                    // Main is zero size, before the DRM sends us the correct window size.
                    bool FirstDRMRender = GGUI::SETTINGS::enableDRM && (getRoot()->getWidth() == 0 && getRoot()->getHeight() == 0);

                    // Skip rendering until DRM sends us the window size.
                    if (!FirstDRMRender) {
                        // terminal::currentStates->screen.buffer = &main->render();
                        terminal::currentStates->screen.update();

                        if (!identicalFrame){
                            if (SETTINGS::enableDRM) {
                                // DRM::sendBuffer(*terminal::currentStates->screen.buffer);
                                // DRM::sendBuffer(*terminal::currentStates->screen.dom.activeGraphicAreas);
                                TODO("Implement DRM::sendBuffer() to accept compactString instead of std::vector<UTF> for better performance and memory efficiency.")
                            }
                            else {
                                terminal::currentStates->screen.computeSGRAreas();

                                terminal::currentStates->screen.preparePresentationBuffer();
                                
                                terminal::currentStates->screen.renderBuffer();
                            }
                        }
                        else{
                        #ifdef GGUI_DEBUG
                            // logger::log("Saved frame");
                        #endif

                            if (SETTINGS::enableDRM) {
                                // std::vector<compactString> empty;
                                // DRM::sendBuffer(empty);
                            }
                        }
                    }
                }

                // Check the difference of the time captured before render and now after render
                Current_Time = std::chrono::steady_clock::now();

                renderDelay = Current_Time - Previous_Time;

                {
                    std::unique_lock lock(concurrency::mutex);
                    // Now for itself set it to sleep.
                    concurrency::pauseRenderThread = status::PAUSED;
                    concurrency::condition.notify_all();
                }
            }

            logger::log("Render thread terminated!");
        }

        /**
         * @brief Refreshes the state of all multi-frame canvases by advancing their animations and flushing their updated states.
         * 
         * This function iterates over each multi-frame canvas, advances its animation to the next frame, and flushes the updated state.
         * If there are canvases to update, it adjusts the event thread load based on the number of canvases.
         */
        void Refresh_Multi_Frame_Canvas() {
            // Iterate over each multi-frame canvas
            for (auto i : core::multiFrameCanvas) {
                // Advance the animation to the next frame
                i.first->setNextAnimationFrame();

                // Flush the updated state of the canvas
                i.first->flush(true);
            }

            // Adjust the event thread load if there are canvases to update
            if (core::multiFrameCanvas.size() > 0) {
                eventThreadLoad = std::min(1.0f, eventThreadLoad + 0.1f * core::multiFrameCanvas.size());
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
            core::remember.delayConstruct();

            logger::log("Event thread starting...");

            while (true){
                {
                    std::unique_lock lock(concurrency::mutex);

                    concurrency::condition.wait(lock, [&](){ 
                        return concurrency::pauseRenderThread == status::PAUSED || concurrency::requestTermination; 
                    });

                    if (concurrency::requestTermination){
                        break;
                    }
                }

                // Reset the thread load counter
                eventThreadLoad = 0;
                Previous_Time = std::chrono::steady_clock::now();

                // Order independent --------------
                core::recallMemories();
                Refresh_Multi_Frame_Canvas();

                /* 
                    Notice: Since the Rendering thread will use its own access to render as tickets, so every time it is "REQUESTING_PAUSE" it will after its own run set itself to PAUSED.
                    This is what Tickets are.
                    So in other words, if there is MUST use of rendering pipeline, use Update_Frame().
                */  
                // Resume_GGUI();

                Current_Time = std::chrono::steady_clock::now();

                // Calculate the delta time.
                eventDelay = Current_Time - Previous_Time;

                CURRENT_UPDATE_SPEED = std::chrono::duration_cast<std::chrono::steady_clock::duration>(
                    SETTINGS::MIN_UPDATE_SPEED + (SETTINGS::MAX_UPDATE_SPEED - SETTINGS::MIN_UPDATE_SPEED) * (1 - eventThreadLoad)
                );

                // If ya want uncapped FPS, disable this sleep code:
                std::this_thread::sleep_for(
                    std::max(
                        CURRENT_UPDATE_SPEED - eventDelay, 
                        SETTINGS::MIN_UPDATE_SPEED
                    )
                );
            }
        
            logger::log("Event thread terminated!");
        }
    }
}