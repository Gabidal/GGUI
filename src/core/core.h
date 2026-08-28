#ifndef _CORE_H_
#define _CORE_H_

#undef min
#undef max

#include <functional>
#include <mutex>
#include <condition_variable>

#include "utils/style.h"
#include "utils/utils.h"

#include "thread.h"

#include "../elements/element.h"

#include "converter.h"

namespace GGUI{
    static struct mouse {
        enum class states : uint8_t {
            DISABLE,
            ENABLE
        } state = states::DISABLE;
        
        IVector2 position = {};

        constexpr bool collides(element* other) {
            IVector3 tmp(position);
            return utils::collides(other, tmp);
        }
    } currentMouse;

    namespace core{
        class bufferCapture;

        extern thread::guard<std::vector<converter::output::event::memory>> remember;
        
        extern std::unordered_map<std::string_view, element*> elementNames;

        extern element* focusedOn;
        extern element* hoveredOn;

        extern std::chrono::steady_clock::duration CURRENT_UPDATE_SPEED; // dynamic depending on load

        extern converter::input::base*  inputManager;
        extern converter::output::base* inputConverter; 

        extern element* main;

        extern std::chrono::steady_clock::duration renderDelay;    // describes how long previous render cycle took in ms

        extern std::string now();

        extern std::string constructLoggerFileName();

        extern void Cleanup();

        extern void SignalThreadTermination();

        /**
         * @brief This function is a helper for the smart memory system to recall which tasks should be prolonged, and which should be deleted.
         * @details This function is a lambda function that is used by the concurrency::Guard class to prolong or delete memories in the smart memory system.
         *          It takes a pointer to a vector of Memory objects and prolongs or deletes the memories in the vector based on the time difference between the current time and the memory's start time.
         */
        extern void recallMemories();

        /**
         * @brief Removes focus from the currently focused element and its children.
         * @details This function checks if there is a currently focused element.
         *          If there is, it sets the focus state on the element and its children to false.
         *          Focus is only removed if the element's current focus state differs from the desired state.
         */
        extern void unFocusElement();

        /**
         * @brief Removes the hover state from the currently hovered element and its children.
         * @details This function checks if there is a currently hovered element.
         *          If there is, it sets the hover state on the element and its children to false.
         *          Hover is only removed if the element's current hover state differs from the desired state.
         */
        extern void unHoverElement();

        /**
         * @brief Updates the currently focused element to a new candidate.
         * @details This function checks if the new candidate is the same as the current focused element.
         *          If not, it removes the focus from the current element and all its children.
         *          Then, it sets the focus on the new candidate element and all its children.
         * @param new_candidate The new element to focus on.
         */
        extern void updateFocusedElement(GGUI::element* new_candidate);

        /**
         * @brief Updates the currently hovered element to a new candidate.
         * @details This function checks if the new candidate is the same as the current hovered element.
         *          If not, it removes the hover state from the current element and all its children.
         *          Then, it sets the hover state on the new candidate element and all its children.
         * @param new_candidate The new element to hover on.
         */
        extern void updateHoveredElement(GGUI::element* new_candidate);

        /**
         * @brief Handles all events in the system.
         * @details This function goes through all event handlers and checks if the event criteria matches any of the inputs.
         *          If a match is found, it calls the event handler job with the input as an argument.
         *          If the job is successful, it removes the input from the list of inputs.
         *          If the job is unsuccessful, it reports an error.
         */
        extern void eventHandler();

        /**
         * Get the ID of a class by name, assigning a new ID if it doesn't exist.
         * 
         * @param n The name of the class.
         * @return The ID of the class.
         */
        extern int getFreeClassID(std::string n);

        /**
         * @brief Initializes the GGUI system and returns the main window.
         * 
         * @return The main window of the GGUI system.
         */
        extern GGUI::element* initGGUI();

        /**
         * @brief Nests a text buffer into a parent buffer while considering the childs position and size.
         * 
         * @param Parent The parent element which the text is being nested into.
         * @param child The child element which's text is being nested.
         * @param Text The text buffer to be nested.
         * @param Parent_Buffer The parent buffer which the text is being nested into.
         */
        // extern void nestUTFText(GGUI::element* Parent, GGUI::element* child, std::vector<GGUI::UTF> Text, std::vector<GGUI::UTF>& Parent_Buffer);

        /**
         * @brief Handles the pressing of the tab key.
         * @details This function selects the next tabbed element as focused and not hovered.
         *          If the shift key is pressed, it goes backwards in the list of tabbed elements.
         */
        extern void handleTabulator();

        /**
         * @brief Handles escape key press events.
         * @details This function checks if the escape key has been pressed and if the focused element is not null.
         *          If the focused element is not null, it calls the Un_Focus_Element function to remove the focus.
         *          If the focused element is null but the hovered element is not null, it calls the Un_Hover_Element
         *          function to remove the hover.
         */
        extern void handleEscape();

        /**
         * @brief Gets the fitting area for a child element in its parent.
         * @details This function calculates the area where the child element should be rendered within the parent element.
         *          It takes into account the border offsets of both the parent and the child element as well as their positions.
         *          The function returns a pair of pairs, where the first pair contains the negative offset of the child element from the parent element,
         *          the second pair contains the starting offset of the child element within the parent element and the third pair contains the ending offset of the child element within the parent element.
         * @param Parent The parent element.
         * @param Child The child element.
         * @return A pair of pairs containing the fitting area for the child element within the parent element.
         */
        types::fittingArea getFittingArea(GGUI::element* Parent, GGUI::element* Child);

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
        void nestElement(element* parent, element* child, std::vector<terminal::cell>& Parent_Buffer, const std::vector<terminal::cell>& Child_Buffer);
    }
    
    /**
     * @brief Register a user defined cleanup callback to be invoked during de-initialization.
     * @details Callbacks are executed after internal threads are joined but before terminal state is restored.
     */
    extern void registerCleanupCallback(std::function<void()> Callback);

    /**
     * @brief Gracefully shuts down the application.
     *
     * This function performs a series of steps to gracefully shut down the application:
     * 1. Logs the initiation of the termination process.
     * 2. Signals subthreads to terminate.
     * 3. Waits for all subthreads to join.
     * 4. Reverts the console to its normal mode.
     * 5. Cleans up platform-specific resources and settings.
     * 6. Logs the successful shutdown of the application.
     * 7. Exits the application with the specified exit code.
     *
     * @param signum The exit code to be used when terminating the application.
     */
    extern void EXIT(int Signum = 0);
    
    /**
     * @brief Blocks the calling thread until a termination request is signaled.
     * 
     * It is typically used to keep the main thread alive until the application is
     * requested to terminate (e.g., via signal or internal shutdown logic).
     */
    extern void waitForTermination();

    /**
     * @brief Returns the Main element of the GGUI system.
     * @details This function returns the main element of the GGUI system, which is the root element of the GUI.
     * @return A pointer to the main element of the GGUI system.
     */
    extern element* getRoot();
    
    /**
     * @brief Register cleanup functions to be called on SIGINT, SIGTERM, std::exit(), std::quick_exit(), std::termination
     */
    extern void registerCleanupCallback(std::function<void()> Callback);

    /**
     * @brief Updates the frame.
     * @details This function updates the frame. It's the main entry point for the rendering thread.
     * @note This function will return immediately if the rendering thread is paused.
     */
    extern void updateFrame();
    
    /**
     * @brief Pauses the rendering thread.
     * @details This function pauses the rendering thread. The thread will wait until the rendering thread is resumed.
     */
    extern void pauseGGUI();

    /**
     * @brief Resumes the rendering thread.
     * @details This function resumes the rendering thread after it has been paused.
     * @param restore_render_to The status to restore the rendering thread to.
     */
    extern void resumeGGUI();

    /**
     * @brief Pauses all other GGUI internal threads and calls the given function.
     * @details This function will pause all other GGUI internal threads and call the given function.
     * @param f The function to call.
     */
    extern void pauseGGUI(std::function<void()> f);

    /**
     * @brief Use GGUI in a simple way.
     * @details This is a simple way to use GGUI. It will pause all other GGUI internal threads, initialize GGUI, add all the elements to the root window, sleep for the given amount of milliseconds, and then exit GGUI.
     * @param App The whole GGUI Application that GGUI holds.
     * @param Sleep_For The amount of milliseconds to sleep after calling the given function.
     */
    extern void GGUI(STYLING_INTERNAL::styleBase& App, unsigned long long Sleep_For = 0);

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
    extern void GGUI(STYLING_INTERNAL::styleBase&& App, unsigned long long Sleep_For = 0);

    /**
    * @brief Retrieves an element by name.
    * @details This function takes a string argument representing the name of the element
    *          and returns a pointer to the element if it exists in the global Element_Names map.
    * @param name The name of the element to retrieve.
    * @return A pointer to the element if it exists; otherwise, nullptr.
    */
    extern element* getElement(std::string name);

    /**
     * @brief Retrieves a vector of pointers to elements of type T.
     * 
     * This template function delegates the retrieval of elements to the INTERNAL::Main object.
     * It returns a std::vector containing pointers to elements of the specified type T.
     * 
     * @tparam T The type of elements to retrieve.
     * @return std::vector<T*> A vector of pointers to elements of type T.
     */
    template<typename T>
    std::vector<T*> getElements(){
        return getRoot()->getElements<T>();
    }
}

#endif