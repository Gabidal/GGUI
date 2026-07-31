#include "core.h"
#include "utils/fileStreamer.h"
#include "utils/logger.h"
#include "utils/utils.h"
#include "addons/addons.h"
#include "utils/settings.h"
#include "utils/drm.h"
#include "backend/terminal.h"

#include "../elements/canvas.h"

#include <string>
#include <cassert>
#include <math.h>
#include <sstream>
#include <cstdio>
#include <exception>
#include <csignal>
#include <iomanip>
#include <thread>

#if _WIN32
    #include <windows.h>
    #include <dbghelp.h>
    #undef min
    #undef max
    #undef small
#else
    #include <sys/ioctl.h>
    #include <signal.h>
    #include <termios.h>
    #include <unistd.h>
    #include <sys/uio.h> // Needed for writev
    #include <cstring>
    #include <poll.h>
#endif

namespace GGUI{
    namespace INTERNAL{
        std::vector<std::function<void()>> userCleanupCallbacks;        // User defined functions to be called on cleanup

        std::vector<std::thread> Sub_Threads;

        std::vector<bufferCapture*> globalBufferCaptures;

        concurrency::guard<std::vector<converter::output::event::memory>> remember;

        std::unordered_map<std::string, element*> elementNames;

        element* focusedOn = nullptr;
        element* hoveredOn = nullptr;

        std::unordered_map<GGUI::canvas*, bool> multiFrameCanvas;

        void* Stack_Start_Address = 0;
        void* Heap_Start_Address = 0;

        element* main = nullptr;

        converter::input::base*  inputManager;
        converter::output::base* inputConverter; 

        extern sig_atomic_t requestTermination;

        /**
         * @brief Temporary function to return the current date and time in a string.
         * @return A string of the current date and time in the format "DD.MM.YYYY: SS.MM.HH"
         * @note This function will be replaced when the Date_Element is implemented.
         */
        std::string now(){
            // This function takes the current time and returns a string of the time.
            std::time_t now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
            std::tm* tm_ptr = std::localtime(&now);
            
            std::ostringstream oss;
            // Format: DD.MM.YYYY: SS.MM.HH
            oss << std::put_time(tm_ptr, "%d.%m.%Y: %S.%M.%H");

            return oss.str();
        }
        
        /**
         * @brief Initializes the start addresses for stack and heap.
         * 
         * This function is made extern to prevent inlining. It is responsible
         * for capturing and initializing the nearest stack and heap addresses 
         * and assigning them to the respective global variables.
         */
        extern void Read_Start_Addresses();

        void Cleanup(){
            SignalThreadTermination();

            LOGGER::log("Reverting to normal console mode...");

            // Clean up platform-specific resources and settings (idempotent)
            deInitialize();

            LOGGER::log("GGUI shutdown successful.");
        }

        void SignalThreadTermination(){
            // Gracefully shutdown event and rendering threads.
            requestTermination = true;

            concurrency::condition.notify_all();
        }

        /**
         * @brief This function is a helper for the smart memory system to recall which tasks should be prolonged, and which should be deleted.
         * @details This function is a lambda function that is used by the concurrency::Guard class to prolong or delete memories in the smart memory system.
         *          It takes a pointer to a vector of Memory objects and prolongs or deletes the memories in the vector based on the time difference between the current time and the memory's start time.
         */
        void recallMemories(){
            INTERNAL::remember([](std::vector<converter::output::event::memory>& rememberable){
                std::chrono::steady_clock::time_point currentTime = std::chrono::steady_clock::now();

                // For smart memory system to shorten the next sleep time to arrive at the perfect time for the nearest memory.
                size_t Shortest_Time = INTERNAL::MAX_UPDATE_SPEED;
                // Prolong prolongable memories.
                for (unsigned int i = 0; i < rememberable.size(); i++){
                    for (unsigned int j = i + 1; j < rememberable.size(); j++){
                        if (rememberable.at(i).is(converter::output::event::memory::types::PROLONG_MEMORY) && rememberable.at(j).is(converter::output::event::memory::types::PROLONG_MEMORY) && i != j)
                            // Check if the Job at I is same as the one at J.
                            if (rememberable.at(i).job.target<bool(*)(converter::output::event::base*)>() == rememberable.at(j).job.target<bool(*)(converter::output::event::base*)>()){
                                // Since J will always be one later than I, J will contain the prolonging memory if there is one. 
                                rememberable.at(i).startTime = rememberable.at(j).startTime;

                                rememberable.erase(rememberable.begin() + j--);
                                break;
                            }
                    }
                }

                for (unsigned int i = 0; i < rememberable.size(); i++){
                    //first calculate the time difference between the start if the task and the end task
                    size_t Time_Difference = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - rememberable.at(i).startTime).count();

                    size_t Time_Left = rememberable.at(i).endTime - Time_Difference;

                    if (Time_Left < Shortest_Time)
                        Shortest_Time = Time_Left;

                    //if the time difference is greater than the time limit, then delete the memory
                    if (Time_Difference > rememberable.at(i).endTime){
                        try{
                            bool Success = rememberable.at(i).job((converter::output::event::base*)&rememberable.at(i));

                            // If job is a re-trigger it will ignore whether the job was successful or not.
                            if (rememberable.at(i).is(converter::output::event::memory::types::RETRIGGER)){

                                // May need to change this into more accurate version of time capturing.
                                rememberable.at(i).startTime = currentTime;

                            }
                            else if (Success){
                                rememberable.erase(rememberable.begin() + i);

                                i--;
                            }
                        }
                        catch (std::exception& e){
                            INTERNAL::reportStack("In memory: '" + rememberable.at(i).ID + "' Problem: " + std::string(e.what()));
                        }
                    }

                }

                INTERNAL::eventThreadLoad = lerp(INTERNAL::MIN_UPDATE_SPEED, INTERNAL::MAX_UPDATE_SPEED, Shortest_Time);
            });
        }

        /**
         * @brief Recursively applies or removes focus on an element and its children.
         * @details This function checks if the current element is an event handler.
         *          If not, it sets the focus state on the element and recurses on its children.
         *          Focus is only applied if the element's current focus state differs from the desired state.
         * 
         * @param current The current element to apply or remove focus.
         * @param Focus The desired focus state.
         */
        void Recursively_Apply_Focus(element* current, bool Focus){
            
            // Prior function calls would have set this into the correct Focus state.
            if (current->isFocused() != Focus) {    // if not, then this means this is the child of an Focus setted element.
                if (!current->getEventHandlers().empty())
                    current->setFocus(Focus);
            }

            // Recurse on all child elements
            for (auto c : current->getChilds()){
                Recursively_Apply_Focus(c, Focus);
            }
        }

        /**
         * @brief Recursively applies or removes hover state on an element and its children.
         * @details This function checks if the current element is an event handler.
         *          If not, it sets the hover state on the element and recurses on its children.
         *          Hover is only applied if the element's current hover state differs from the desired state.
         * 
         * @param current The current element to apply or remove hover.
         * @param Hover The desired hover state.
         */
        void Recursively_Apply_Hover(element* current, bool Hover){

            // Prior function calls would have set this into the correct hover state.
            if (current->isHovered() != Hover) {    // if not, then this means this is the child of an hover setted element.

                if (!current->getEventHandlers().empty())
                    current->setHoverState(Hover);
            }

            // Recurse on all child elements
            for (auto c : current->getChilds()){
                Recursively_Apply_Hover(c, Hover);
            }
        }

        /**
         * @brief Removes focus from the currently focused element and its children.
         * @details This function checks if there is a currently focused element.
         *          If there is, it sets the focus state on the element and its children to false.
         *          Focus is only removed if the element's current focus state differs from the desired state.
         */
        void unFocusElement(){
            if (!INTERNAL::focusedOn)
                return;

            INTERNAL::focusedOn->setFocus(false);

            // Recursively remove focus from all child elements
            Recursively_Apply_Focus(INTERNAL::focusedOn, false);

            INTERNAL::focusedOn = nullptr;
        }

        /**
         * @brief Removes the hover state from the currently hovered element and its children.
         * @details This function checks if there is a currently hovered element.
         *          If there is, it sets the hover state on the element and its children to false.
         *          Hover is only removed if the element's current hover state differs from the desired state.
         */
        void unHoverElement(){
            if (!INTERNAL::hoveredOn)
                return;

            // Set the hover state to false on the currently hovered element
            INTERNAL::hoveredOn->setHoverState(false);

            // Recursively remove the hover state from all child elements
            Recursively_Apply_Hover(INTERNAL::hoveredOn, false);

            // Set the hovered element to nullptr to indicate there is no currently hovered element
            INTERNAL::hoveredOn = nullptr;
        }

        /**
         * @brief Updates the currently focused element to a new candidate.
         * @details This function checks if the new candidate is the same as the current focused element.
         *          If not, it removes the focus from the current element and all its children.
         *          Then, it sets the focus on the new candidate element and all its children.
         * @param new_candidate The new element to focus on.
         */
        void updateFocusedElement(GGUI::element* new_candidate){
            if (INTERNAL::focusedOn == new_candidate || new_candidate == INTERNAL::main)
                return;

            if (!new_candidate) return; // For total unselection, use unFocusElement()

            // Unfocus the previous focused element and its children
            if (INTERNAL::focusedOn){
                unFocusElement();
            }

            // Set the focus on the new element and all its children
            INTERNAL::focusedOn = new_candidate;

            // Update mouse location to match with keyboard given states.
            currentMouse.position = INTERNAL::focusedOn->getAbsolutePosition();

            // Set the focus state on the new element to true
            INTERNAL::focusedOn->setFocus(true);
            
            // Recursively set the focus state on all child elements to true
            Recursively_Apply_Focus(INTERNAL::focusedOn, true);
        }

        /**
         * @brief Updates the currently hovered element to a new candidate.
         * @details This function checks if the new candidate is the same as the current hovered element.
         *          If not, it removes the hover state from the current element and all its children.
         *          Then, it sets the hover state on the new candidate element and all its children.
         * @param new_candidate The new element to hover on.
         */
        void updateHoveredElement(GGUI::element* new_candidate){
            if (INTERNAL::hoveredOn == new_candidate || new_candidate == INTERNAL::main)
                return;

            if (!new_candidate) return; // For total unselection, use unHoverElement()

            // Remove the hover state from the previous hovered element and its children
            if (INTERNAL::hoveredOn){
                unHoverElement();
            }

            // Set the hover state on the new element and all its children
            INTERNAL::hoveredOn = new_candidate;

            // Update mouse location to match with keyboard given states.
            currentMouse.position = INTERNAL::hoveredOn->getAbsolutePosition();

            // Set the hover state on the new element to true
            INTERNAL::hoveredOn->setHoverState(true);

            // Recursively set the hover state on all child elements to true
            Recursively_Apply_Hover(INTERNAL::hoveredOn, true);
        }

        /**
         * @brief Initializes the GGUI system and returns the main window.
         * 
         * @return The main window of the GGUI system.
         */
        GGUI::element* initGGUI(){
            INTERNAL::Read_Start_Addresses();
            SETTINGS::initSettings();
            INTERNAL::LOGGER::init();
            INTERNAL::LOGGER::registerCurrentThread();
            INTERNAL::LOGGER::log("Starting GGUI Core initialization...");

            // Create the input poller pairs
            inputManager   = new converter::input::base();
            inputConverter = new converter::output::base(); 

            // link the input poller pairs
            converter::link(inputManager, inputConverter);

            terminal::init(inputManager);   // connects with hardware I/O and resets terminal state machine

            INTERNAL::initPlatformStuff();

            INTERNAL::main = new element(
                width(100) |
                height(100) | 
                name("Main")
            , true);

            terminal::currentStates->screen.link(main);

            std::thread renderingThread([](){
                INTERNAL::LOGGER::registerCurrentThread();
                INTERNAL::renderer();
            });
            renderingThread.detach();  // Let the rendering thread able to std::exit.
            
            std::thread eventThread([](){
                INTERNAL::LOGGER::registerCurrentThread();
                INTERNAL::eventThread();
            });
            eventThread.detach();  // Let the rendering thread able to std::exit.

            std::thread Logging_Scheduler([](){
                INTERNAL::LOGGER::registerCurrentThread();
                INTERNAL::loggerThread();
            });
            
            Logging_Scheduler.detach();

            INTERNAL::LOGGER::log("GGUI Core initialization complete.");

            {
                std::unique_lock lock(INTERNAL::concurrency::mutex);

                // Remove NOT_INITALIZED from the render thread flag.
                INTERNAL::concurrency::pauseRenderThread = INTERNAL::concurrency::status::PAUSED;
            }

            return INTERNAL::main;
        }

        /**
         * @brief Notifies all global buffer capturers about the latest data to be captured.
         *
         * This function is used to inform all global buffer capturers about the latest data to be captured.
         * It iterates over all global buffer capturers and calls their Sync() method to update their data.
         *
         * @param informer Pointer to the buffer capturer with the latest data.
         */
        void informAllGlobalBufferCaptures(bufferCapture* informer){
            // Iterate over all global buffer capturers
            for (auto* capturer : globalBufferCaptures){
                if (!capturer->isGlobal)
                    continue;

                // Give the capturers the latest row of captured buffer data
                if (capturer->sync(informer)){
                    // success
                }
                else{
                    // fail, maybe try merge?
                }
            }
        }
    
        /**
         * @brief Gets the fitting area for a child element in its parent.
         * @details This function calculates the area where the child element should be rendered within the parent element.
         *          It takes into account the border offsets of both the parent and the child element as well as their positions.
         * @param Parent The parent element.
         * @param Child The child element.
         */
        GGUI::INTERNAL::fittingArea getFittingArea(GGUI::element* Parent, GGUI::element* Child){
            // If both dont have same border setup and parent has a border, then the child needs to be offsetted by one in every direction.
            int Border_Offset = Parent->hasBorder() != Child->hasBorder() && Parent->hasBorder() ? 1 : 0;
            
            // Absolute bounding
            IVector2 parentStart = {Border_Offset, Border_Offset};
            IVector2 parentEnd = {Parent->getWidth() - Border_Offset, Parent->getHeight() - Border_Offset};

            // This only contains value if the position of the child element has any negative positioning in it.
            IVector2 negativeOffset = {
                Child->getPosition().x < 0 ? -Child->getPosition().x : 0,
                Child->getPosition().y < 0 ? -Child->getPosition().y : 0
            };

            // Drawable box start, within the bounding box.
            IVector2 childStart = IVector2{
               std::max(Child->getPosition().x, 0),
               std::max(Child->getPosition().y, 0)
            } + parentStart;

            // Drawable box end, within the bounding box.
            IVector2 childEnd = {
                std::min(childStart.x + Child->getWidth() - negativeOffset.x, parentEnd.x),
                std::min(childStart.y + Child->getHeight() - negativeOffset.y, parentEnd.y)
            };

            return {negativeOffset, childStart, childEnd };
        }

        /**
         * @brief Compute the alpha blending of the source element to the destination element.
         * @details This function takes two UTF elements as arguments, the source element and the destination element.
         *          It calculates the alpha blending of the source element to the destination element, by adding the
         *          background color of the source element to the destination element, but only if the source element has
         *          a non-zero alpha value. If the source element has full opacity, then the destination gets fully rewritten
         *          over. If the source element has full transparency, then nothing is done.
         * @param Dest The destination element to which the source element will be blended.
         * @param Source The source element which will be blended to the destination element.
         */
        // void computeAlphaToNesting(GGUI::UTF& Dest, const GGUI::UTF& Source, unsigned char childOpacity){
        //     // If the Source element has full opacity, then the destination gets fully rewritten over.
        //     if (childOpacity == UINT8_MAX){
        //         Dest = Source;
        //         return;
        //     }
        //     else if (childOpacity == 0) return;         // Dont need to do anything.

        //     // Color the Destination UTF by the Source UTF background color.
        //     Dest.background.add(Source.background, childOpacity);
        //     Dest.foreground.add(Source.background, childOpacity);

        //     // Check if source has text
        //     if (!Source.hasDefaultText()){
        //         Dest.setText(Source);
        //         Dest.foreground.add(Source.foreground, childOpacity); 
        //     }
        // }

        /**
         * @brief Nests a child element into a parent element.
         * @details This function calculates the area where the child element should be rendered within the parent element.
         *          It takes into account the border offsets of both the parent and the child element as well as their positions.
         *          The function then copies the contents of the child element's buffer into the parent element's buffer at the calculated position.
         * @param Parent The parent element.
         * @param Child The child element.
         * @param Parent_Buffer The parent element's buffer.
         * @param Child_Buffer The child element's buffer.
         */
        void nestElement(GGUI::element* parent, GGUI::element* child, std::vector<compactString>& Parent_Buffer, std::vector<compactString>& Child_Buffer){
            INTERNAL::fittingArea Limits = getFittingArea(parent, child);

            for (int y = Limits.start.y; y < Limits.end.y; y++){
                for (int x = Limits.start.x; x < Limits.end.x; x++){
                    // Calculate the position of the child element in its own buffer.
                    int Child_Buffer_Y = (y - Limits.start.y + Limits.negativeOffset.y) * child->getWidth();
                    int Child_Buffer_X = (x - Limits.start.x + Limits.negativeOffset.x); 
                    // computeAlphaToNesting(Parent_Buffer[y * parent->getWidth() + x], Child_Buffer[Child_Buffer_Y + Child_Buffer_X], child->getOpacityByte());
                    Parent_Buffer[y * parent->getWidth() + x] = Child_Buffer[Child_Buffer_Y + Child_Buffer_X];
                }
            }
        }
    }

    /**
     * @brief De-initializes platform-specific settings and resources and exits the application.
     * @details This function is called by the Exit function to de-initialize platform-specific settings and resources.
     *          It ensures that any platform-specific settings are reset before the application exits.
     * @param signum The exit code for the application.
     */
    void EXIT(int signum){
        INTERNAL::SignalThreadTermination();

        // Exit the application with the specified exit code
        exit(signum);
    }

    /**
     * @brief Blocks the calling thread until a termination request is signaled.
     * 
     * It is typically used to keep the main thread alive until the application is
     * requested to terminate (e.g., via signal or internal shutdown logic).
     */
    void waitForTermination() {
        std::unique_lock lock(INTERNAL::concurrency::mutex);
        INTERNAL::concurrency::condition.wait(lock, [&](){ return INTERNAL::requestTermination; });
    }

    element* getRoot() {
        return INTERNAL::main;
    }
    
    /**
     * @brief Register cleanup functions to be called on SIGINT, SIGTERM, std::exit(), std::quick_exit(), std::termination
     */
    void registerCleanupCallback(std::function<void()> Callback) {
        INTERNAL::userCleanupCallbacks.push_back(Callback);
    }

    /**
     * @brief Updates the frame.
     * @details This function updates the frame. It's the main entry point for the rendering thread.
     * @note This function will return immediately if the rendering thread is paused.
     */
    void updateFrame(){
        std::unique_lock lock(INTERNAL::concurrency::mutex);

        // The rendering thread is either locked, already rendering or already requested to render.
        if (INTERNAL::concurrency::LOCKED > 0 || INTERNAL::concurrency::pauseRenderThread == INTERNAL::concurrency::status::NOT_INITIALIZED)
            return;

        // Give the rendering thread one ticket.
        INTERNAL::concurrency::pauseRenderThread = INTERNAL::concurrency::status::REQUESTING_RENDERING;

        // Notify all waiting threads that the frame has been updated.
        INTERNAL::concurrency::condition.notify_all();
    }

    /**
     * @brief Pauses the rendering thread.
     * @details This function pauses the rendering thread. The thread will wait until the rendering thread is resumed.
     */
    void pauseGGUI(){
        std::unique_lock lock(INTERNAL::concurrency::mutex);
        
        // Already paused via upper scope or if the rendering system hasn't been initialized yet, just no-op.
        if (INTERNAL::concurrency::LOCKED++ > 0 || INTERNAL::concurrency::pauseRenderThread == INTERNAL::concurrency::status::NOT_INITIALIZED)
            return;

        // await until the rendering thread has used it's rendering ticket.
        INTERNAL::concurrency::condition.wait(lock, []{
            return INTERNAL::concurrency::pauseRenderThread == INTERNAL::concurrency::status::PAUSED;
        });
    }

    /**
     * @brief Resumes the rendering thread.
     * @details This function resumes the rendering thread after it has been paused.
     * @param restore_render_to The status to restore the rendering thread to.
     */
    void resumeGGUI(){
        // If not initialized, simply reset LOCKED counter (if needed) and no-op.
        if (INTERNAL::concurrency::pauseRenderThread == INTERNAL::concurrency::status::NOT_INITIALIZED){
            INTERNAL::concurrency::LOCKED = 0; // Safety: ensure clean state for unit tests.
            return;
        }

        {
            std::unique_lock lock(INTERNAL::concurrency::mutex);

            if (--INTERNAL::concurrency::LOCKED > 0)
                return;

            INTERNAL::concurrency::condition.wait(lock, []{
                // If the rendering thread is not locked, then the wait is over.
                return INTERNAL::concurrency::pauseRenderThread == INTERNAL::concurrency::status::PAUSED;
            });
        }

        updateFrame();
    }

    /**
     * @brief Pauses all other GGUI internal threads and calls the given function.
     * @details This function will pause all other GGUI internal threads and call the given function.
     * @param f The function to call.
     */
    void pauseGGUI(std::function<void()> f){
        // Fast path: if rendering thread not initialized (e.g. unit tests constructing elements only), just execute.
        if (INTERNAL::concurrency::pauseRenderThread == INTERNAL::concurrency::status::NOT_INITIALIZED){
            f();
            return;
        }

        pauseGGUI();

        try{
            // Call the given function.
            f();
        }
        catch(std::exception& e){

            std::string Given_Function_Label_Location = INTERNAL::hex(reinterpret_cast<unsigned long long>(&f));

            // If an exception is thrown, report the stack trace and the exception message.
            INTERNAL::reportStack("In given function to Pause_GGUI: " + Given_Function_Label_Location + " arose problem: \n" + std::string(e.what()));
        }

        // Resume the render thread with the previous render status.
        resumeGGUI();
    }

    /**
     * @brief Use GGUI in a simple way.
     * @details This is a simple way to use GGUI. It will pause all other GGUI internal threads, initialize GGUI, add all the elements to the root window, sleep for the given amount of milliseconds, and then exit GGUI.
     * @param DOM The elements to add to the root window.
     * @param Sleep_For The amount of milliseconds to sleep after calling the given function.
     */
    void GGUI(STYLING_INTERNAL::styleBase& App, unsigned long long Sleep_For){
        INTERNAL::Read_Start_Addresses();

        pauseGGUI([&App](){
            INTERNAL::initGGUI();

            // Since the App is basically an AST Styling, we first add it to the already constructed main with its width and height set to the terminal sizes.
            getRoot()->addStyling(App);
            
            // Now we can safely insert addons while taking into notion user configured borders and other factors which may impact the usable width.
            initAddons();

            if (SETTINGS::enableDRM) {
                INTERNAL::DRM::retryDRMConnect();
            }
        });
        
        // We need to call the Mains own on_init manually, since it was already called once in the initGGUI();
        getRoot()->check(INTERNAL::STATE::INIT);

        // Sleep for the given amount of milliseconds.
        std::this_thread::sleep_for(std::chrono::milliseconds(Sleep_For));
    }

    /**
     * @brief Calls the GGUI function with the provided style and sleep duration.
     *
     * This function forwards the given style object and sleep duration to another
     * overload of the GGUI function. It is typically used to initialize or update
     * the graphical user interface with specific styling and timing parameters.
     *
     * @param App An rvalue reference to a STYLING_INTERNAL::style_base object representing the application's style.
     * @param Sleep_For The duration, in microseconds, for which the function should sleep or delay execution.
     */
    void GGUI(STYLING_INTERNAL::styleBase&& App, unsigned long long Sleep_For) { GGUI(App, Sleep_For); }

    /**
     * @brief Retrieves an element by name.
     * @details This function takes a string argument representing the name of the element
     *          and returns a pointer to the element if it exists in the global Element_Names map.
     * @param name The name of the element to retrieve.
     * @return A pointer to the element if it exists; otherwise, nullptr.
     */
    element* getElement(std::string name){
        element* Result = nullptr;

        // Check if the element is in the global Element_Names map.
        if (INTERNAL::elementNames.find(name) != INTERNAL::elementNames.end()){
            // If the element exists, assign it to the result.
            Result = INTERNAL::elementNames[name];
        }

        // Return the result.
        return Result;
    }
}
