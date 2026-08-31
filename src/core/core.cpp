#include "core.h"
#include "utils/logger.h"
#include "utils/utils.h"
#include "addons/addons.h"
#include "utils/settings.h"
#include "utils/drm.h"
#include "backend/terminal.h"

#include "../elements/canvas.h"

#include "thread.h"

#include <string>
#include <cassert>
#include <math.h>
#include <sstream>
#include <exception>
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
    #include <termios.h>
    #include <unistd.h>
    #include <sys/uio.h> // Needed for writev
    #include <cstring>
    #include <poll.h>
#endif

namespace GGUI{
    namespace core{
        std::vector<std::function<void()>> userCleanupCallbacks;        // User defined functions to be called on cleanup

        std::vector<std::thread> Sub_Threads;

        std::vector<class bufferCapture*> globalBufferCaptures;

        thread::guard<std::vector<converter::output::event::memory>> remember;

        std::unordered_map<std::string_view, element*> elementNames;

        element* focusedOn = nullptr;
        element* hoveredOn = nullptr;

        std::unordered_map<canvas*, bool> multiFrameCanvas;

        void* Stack_Start_Address = 0;
        void* Heap_Start_Address = 0;

        element* main = nullptr;

        converter::input::base*  inputManager;
        converter::output::base* inputConverter; 

        /**
         * @brief Temporary function to return the current date and time in a string.
         * @return A string of the current date and time in the format "DD.MM.YYYY: HH.MM.SS"
         * @note This function will be replaced when the Date_Element is implemented.
         */
        std::string now(){
            // This function takes the current time and returns a string of the time.
            std::time_t now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
            std::tm* tm_ptr = std::localtime(&now);
            
            std::ostringstream oss;
            // Format: DD.MM.YYYY: HH.MM.SS
            oss << std::put_time(tm_ptr, "%d.%m.%Y: %H.%M.%S");

            return oss.str();
        }

        void SignalThreadTermination(){
            std::lock_guard<std::mutex> lock(thread::concurrency::mutex);

            // Gracefully shutdown event and rendering threads.
            thread::concurrency::requestTermination = true;

            thread::concurrency::condition.notify_all();
        }

        /**
         * @brief This function is a helper for the smart memory system to recall which tasks should be prolonged, and which should be deleted.
         * @details This function is a lambda function that is used by the thread::concurrency::Guard class to prolong or delete memories in the smart memory system.
         *          It takes a pointer to a vector of Memory objects and prolongs or deletes the memories in the vector based on the time difference between the current time and the memory's start time.
         */
        void recallMemories(){
            remember([](std::vector<converter::output::event::memory>& rememberable){
                std::chrono::steady_clock::time_point currentTime = std::chrono::steady_clock::now();

                // For smart memory system to shorten the next sleep time to arrive at the perfect time for the nearest memory.
                std::chrono::steady_clock::duration Shortest_Time = SETTINGS::MAX_UPDATE_SPEED;
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
                    std::chrono::steady_clock::duration Time_Difference = currentTime - rememberable.at(i).startTime;

                    std::chrono::steady_clock::duration Time_Left = rememberable.at(i).endTime - Time_Difference;

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
                            logger::log("In memory: '" + rememberable.at(i).ID + "' Problem: " + std::string(e.what()));
                        }
                    }

                }

                thread::eventThreadLoad = utils::lerp(SETTINGS::MIN_UPDATE_SPEED.count(), SETTINGS::MAX_UPDATE_SPEED.count(), Shortest_Time.count());
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
            if (!focusedOn)
                return;

            focusedOn->setFocus(false);

            // Recursively remove focus from all child elements
            Recursively_Apply_Focus(focusedOn, false);

            focusedOn = nullptr;
        }

        /**
         * @brief Removes the hover state from the currently hovered element and its children.
         * @details This function checks if there is a currently hovered element.
         *          If there is, it sets the hover state on the element and its children to false.
         *          Hover is only removed if the element's current hover state differs from the desired state.
         */
        void unHoverElement(){
            if (!hoveredOn)
                return;

            // Set the hover state to false on the currently hovered element
            hoveredOn->setHoverState(false);

            // Recursively remove the hover state from all child elements
            Recursively_Apply_Hover(hoveredOn, false);

            // Set the hovered element to nullptr to indicate there is no currently hovered element
            hoveredOn = nullptr;
        }

        /**
         * @brief Updates the currently focused element to a new candidate.
         * @details This function checks if the new candidate is the same as the current focused element.
         *          If not, it removes the focus from the current element and all its children.
         *          Then, it sets the focus on the new candidate element and all its children.
         * @param new_candidate The new element to focus on.
         */
        void updateFocusedElement(element* new_candidate){
            if (focusedOn == new_candidate || new_candidate == main)
                return;

            if (!new_candidate) return; // For total unselection, use unFocusElement()

            // Unfocus the previous focused element and its children
            if (focusedOn){
                unFocusElement();
            }

            // Set the focus on the new element and all its children
            focusedOn = new_candidate;

            // Update mouse location to match with keyboard given states.
            currentMouse.position = focusedOn->getAbsolutePosition();

            // Set the focus state on the new element to true
            focusedOn->setFocus(true);
            
            // Recursively set the focus state on all child elements to true
            Recursively_Apply_Focus(focusedOn, true);
        }

        /**
         * @brief Updates the currently hovered element to a new candidate.
         * @details This function checks if the new candidate is the same as the current hovered element.
         *          If not, it removes the hover state from the current element and all its children.
         *          Then, it sets the hover state on the new candidate element and all its children.
         * @param new_candidate The new element to hover on.
         */
        void updateHoveredElement(element* new_candidate){
            if (hoveredOn == new_candidate || new_candidate == main)
                return;

            if (!new_candidate) return; // For total unselection, use unHoverElement()

            // Remove the hover state from the previous hovered element and its children
            if (hoveredOn){
                unHoverElement();
            }

            // Set the hover state on the new element and all its children
            hoveredOn = new_candidate;

            // Update mouse location to match with keyboard given states.
            currentMouse.position = hoveredOn->getAbsolutePosition();

            // Set the hover state on the new element to true
            hoveredOn->setHoverState(true);

            // Recursively set the hover state on all child elements to true
            Recursively_Apply_Hover(hoveredOn, true);
        }

        /**
         * @brief Initializes the GGUI system and returns the main window.
         * 
         * @return The main window of the GGUI system.
         */
        void init(){
            std::thread Logging_Scheduler([](){
                logger::loggerThread();
            });

            logger::log("Starting GGUI Core initialization...");

            // Create the input poller pairs
            inputManager   = new converter::input::base();
            inputConverter = new converter::output::base(); 

            // link the input poller pairs
            converter::link(inputManager, inputConverter);

            main = new element(
                name("main"), 
                true
            );

            terminal::init(inputManager);   // connects with hardware I/O and resets terminal state machine

            terminal::currentStates->screen.link(main);

            std::thread renderingThread([](){
                thread::renderer();
            });
            
            std::thread eventThread([](){
                thread::eventThread();
            });

            logger::log("GGUI Core initialization complete.");

            {
                std::unique_lock lock(thread::concurrency::mutex);

                // Remove NOT_INITALIZED from the render thread flag.
                thread::concurrency::pauseRenderThread = thread::status::PAUSED;
            }
            
            renderingThread.detach();  // Let the rendering thread able to std::exit.
            eventThread.detach();  // Let the rendering thread able to std::exit.
            Logging_Scheduler.detach();
        }

        void deinit(){
            SignalThreadTermination();

            logger::log("GGUI shutdown successful.");
        }
    
        /**
         * @brief Returns the dest buffer and src buffer information for copying data between parent and child elements
         * @param Parent The parent element.
         * @param Child The child element.
         */
        std::pair<rectangle, rectangle> getFittingArea(element* Parent, element* Child){
            // If both dont have same border setup and parent has a border, then the child needs to be offsetted by one in every direction.
            int borderOffset = Parent->hasBorder() != Child->hasBorder() && Parent->hasBorder() ? 1 : 0;
            
            // Absolute bounding
            rectangle bounds = {
                {borderOffset, borderOffset},
                {Parent->getWidth() - borderOffset, Parent->getHeight() - borderOffset}
            };

            rectangle childInfo = { TODO("Give element a direct getRectangle()")
                Child->getPosition(),
                {Child->getWidth(), Child->getHeight()}
            };

            // The written output is the intersection between the child and the bounds
            rectangle dest = bounds.intersection(childInfo);

            // The childs own relative buffer intersection of clipping while partially outside or completely inside the parent element
            rectangle src = {
                {dest.position.x - childInfo.position.x, dest.position.y - childInfo.position.y},
                dest.size
            };

            return {dest, src};
        }

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
        void nestElement(element* parent, element* child, std::vector<terminal::cell>& Parent_Buffer, const std::vector<terminal::cell>& Child_Buffer){
            auto [dest, src] = getFittingArea(parent, child);

            for (int y = 0; y < dest.size.y; y++) {
                int parentRowStart = (dest.position.y + y) * parent->getWidth() + dest.position.x;
                int childRowStart  = (src.position.y  + y) * child->getWidth()  + src.position.x;

                std::copy(
                    Child_Buffer.begin() + childRowStart,
                    Child_Buffer.begin() + childRowStart + dest.size.x,
                    Parent_Buffer.begin() + parentRowStart
                );
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
        core::SignalThreadTermination();

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
        std::unique_lock lock(thread::concurrency::mutex);
        thread::concurrency::condition.wait(lock, [&](){ return thread::concurrency::requestTermination; });
    }

    element* getRoot() {
        return core::main;
    }
    
    /**
     * @brief Register cleanup functions to be called on SIGINT, SIGTERM, std::exit(), std::quick_exit(), std::termination
     */
    void registerCleanupCallback(std::function<void()> Callback) {
        core::userCleanupCallbacks.push_back(Callback);
    }

    /**
     * @brief Updates the frame.
     * @details This function updates the frame. It's the main entry point for the rendering thread.
     * @note This function will return immediately if the rendering thread is paused.
     */
    void updateFrame(){
        std::unique_lock lock(thread::concurrency::mutex);

        // The rendering thread is either locked, already rendering or already requested to render.
        if (thread::concurrency::LOCKED > 0 || thread::concurrency::pauseRenderThread == thread::status::NOT_INITIALIZED)
            return;

        // Give the rendering thread one ticket.
        thread::concurrency::pauseRenderThread = thread::status::REQUESTING_RENDERING;

        // Notify all waiting threads that the frame has been updated.
        thread::concurrency::condition.notify_all();
    }

    /**
     * @brief Pauses the rendering thread.
     * @details This function pauses the rendering thread. The thread will wait until the rendering thread is resumed.
     */
    void pauseGGUI(){
        std::unique_lock lock(thread::concurrency::mutex);
        
        // Already paused via upper scope or if the rendering system hasn't been initialized yet, just no-op.
        if (thread::concurrency::LOCKED++ > 0 || thread::concurrency::pauseRenderThread == thread::status::NOT_INITIALIZED)
            return;

        // await until the rendering thread has used it's rendering ticket.
        thread::concurrency::condition.wait(lock, []{
            return thread::concurrency::pauseRenderThread == thread::status::PAUSED;
        });
    }

    /**
     * @brief Resumes the rendering thread.
     * @details This function resumes the rendering thread after it has been paused.
     * @param restore_render_to The status to restore the rendering thread to.
     */
    void resumeGGUI(){
        // If not initialized, simply reset LOCKED counter (if needed) and no-op.
        if (thread::concurrency::pauseRenderThread == thread::status::NOT_INITIALIZED){
            thread::concurrency::LOCKED = 0; // Safety: ensure clean state for unit tests.
            return;
        }

        {
            std::unique_lock lock(thread::concurrency::mutex);

            if (--thread::concurrency::LOCKED > 0)
                return;

            thread::concurrency::condition.wait(lock, []{
                // If the rendering thread is not locked, then the wait is over.
                return thread::concurrency::pauseRenderThread == thread::status::PAUSED;
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
        if (thread::concurrency::pauseRenderThread == thread::status::NOT_INITIALIZED){
            f();
            return;
        }

        pauseGGUI();

        try{
            // Call the given function.
            f();
        }
        catch(std::exception& e){
            
            auto hex = [](unsigned long long value) {
                char buffer[17]; // Enough to hold the largest 64-bit hexadecimal value + null terminator
                std::snprintf(buffer, sizeof(buffer), "%llX", value); // Formats the value as uppercase hex
                return std::string(buffer);
            };

            std::string Given_Function_Label_Location = hex(reinterpret_cast<unsigned long long>(&f));

            // If an exception is thrown, report the stack trace and the exception message.
            logger::log("In given function to Pause_GGUI: " + Given_Function_Label_Location + " arose problem: \n" + std::string(e.what()));
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
        pauseGGUI([&App](){
            core::init();

            // Since the App is basically an AST Styling, we first add it to the already constructed main with its width and height set to the terminal sizes.
            getRoot()->addStyling(App);
            
            // Now we can safely insert addons while taking into notion user configured borders and other factors which may impact the usable width.
            initAddons();

            if (SETTINGS::enableDRM) {
                DRM::retryDRMConnect();
            }
        });
        
        // We need to call the Mains own on_init manually, since it was already called once in the initGGUI();
        getRoot()->check(STATE::INIT);

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
     * @param App An rvalue reference to a style_base object representing the application's style.
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
        if (core::elementNames.find(name) != core::elementNames.end()){
            // If the element exists, assign it to the result.
            Result = core::elementNames[name];
        }

        // Return the result.
        return Result;
    }
}
