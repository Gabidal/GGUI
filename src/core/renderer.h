#ifndef _RENDERER_H_
#define _RENDERER_H_

#undef min
#undef max

#include <functional>
#include <thread>
#include <atomic>
#include <mutex>
#include <condition_variable>

#include "../elements/textField.h"
#include "../elements/listView.h"
#include "../elements/canvas.h"

#include "../core/utils/superString.h"
#include "../core/utils/constants.h"
#include "../core/utils/color.h"
#include "../core/utils/units.h"
#include "../core/utils/event.h"
#include "../core/utils/style.h"
#include "../core/utils/guard.h"

//GGUI uses the ANSI escape code
//https://en.wikipedia.org/wiki/ANSI_escape_code
namespace GGUI{

    namespace INTERNAL{
        class bufferCapture;

        namespace atomic{
            enum class status{
                PAUSED,
                REQUESTING_RENDERING,
                RENDERING,
                TERMINATED,
                NOT_INITIALIZED
            };

            extern int LOCKED;

            extern std::mutex Mutex;
            extern std::condition_variable Condition;

            extern status Pause_Render_Thread;
        }

        // Inits with 'NOW()' when created
        class buttonState {
        public:
            bool State;
            std::chrono::high_resolution_clock::time_point Capture_Time;

            buttonState(bool state = false) : State(state), Capture_Time(std::chrono::high_resolution_clock::now()) {}
        };

        extern std::vector<UTF>* Abstract_Frame_Buffer;                 //2D clean vector without bold nor color
        extern std::string* Frame_Buffer;                                //string with bold and color, this what gets drawn to console.

        extern std::vector<INTERNAL::bufferCapture*> Global_Buffer_Captures;

        extern unsigned int Max_Width;
        extern unsigned int Max_Height;

        extern atomic::Guard<std::vector<Memory>> Remember;

        extern std::vector<Action*> Event_Handlers;
        extern std::vector<Input*> Inputs;
        
        extern std::unordered_map<std::string, element*> Element_Names;

        extern element* Focused_On;
        extern element* Hovered_On;

        extern bool Platform_Initialized;

        extern IVector3 Mouse;    
        extern bool Mouse_Movement_Enabled;

        extern std::unordered_map<std::string, buttonState> KEYBOARD_STATES;

        extern time_t MAX_UPDATE_SPEED;
        extern int Inputs_Per_Second;
        extern int Inputs_Per_Query;

        extern unsigned long long Render_Delay;    // describes how long previous render cycle took in ms
        extern unsigned long long Event_Delay;    // describes how long previous memory tasks took in ms

        extern atomic::Guard<std::unordered_map<int, styling>> Classes;
        extern std::unordered_map<std::string, int> Class_Names;

        extern element* Main;  

        extern std::unordered_map<GGUI::terminalCanvas*, bool> Multi_Frame_Canvas;

        // Represents the update speed of each elapsed loop of passive events, which do NOT need user as an input.
        extern time_t MAX_UPDATE_SPEED;
        extern time_t MIN_UPDATE_SPEED;    // Close approximation to 60 fps.
        extern time_t CURRENT_UPDATE_SPEED;
        extern float Event_Thread_Load;  // Describes the load of animation and events from 0.0 to 1.0. Will reduce the event thread pause.

        extern unsigned long long Render_Delay;    // describes how long previous render cycle took in ms
        extern unsigned long long Event_Delay;    // describes how long previous memory tasks took in ms
        extern unsigned long long Input_Delay;     // describes how long previous input tasks took in ms

        extern std::string now();

        extern std::string constructLoggerFileName();

        /**
         * @brief Initializes platform-specific settings for console handling.
         * @details This function sets up the console handles and modes required for input and output operations.
         *          It enables mouse and window input, sets UTF-8 mode for output, and prepares the console for
         *          handling specific ANSI features.
         */
        extern void initPlatformStuff();
        
        
        /**
         * @brief Sleep for the specified amount of milliseconds.
         * @details This function is used to pause the execution of the program for a specified amount of time.
         *          It is implemented differently for each platform, so on Windows, it calls the Sleep function,
         *          while on Linux and macOS it calls the usleep function.
         * @param mm The number of milliseconds to sleep.
         */
        void SLEEP(unsigned int milliseconds);
        
        /**
         * @brief Renders the current frame to the console.
         * 
         * This function moves the console cursor to the top left corner of the screen
         * and writes the contents of the Frame_Buffer to the console.
         * 
         * @note The number of bytes written to the console is stored in a temporary
         * variable but is not used elsewhere in the function.
         */
        extern void renderFrame();

        /**
         * @brief Updates the maximum width and height of the console window.
         * 
         * This function retrieves the current console screen buffer information and updates
         * the maximum width and height based on the console window dimensions. If the console
         * information is not retrieved correctly, an error message is reported. Additionally,
         * if the main window is active, its dimensions are set to the updated maximum width
         * and height.
         */
        extern void updateMaxWidthAndHeight();
        
        /**
         * @brief Queries and appends new input records to the existing buffered input.
         *
         * This function reads input records from the console and appends them to the 
         * existing buffered input which has not yet been processed. It uses the previous 
         * size of the raw input buffer to determine the starting point for new input records.
         *
         * @note The function ensures that negative numbers do not create overflows by 
         *       using the maximum of the remaining capacity and the total capacity.
         *
         * @param None
         * @return None
         */
        extern void queryInputs();
        
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

        extern void waitForThreadTermination();
    }

    /**
     * @brief Exits the application with the given signal number.
     * 
     * This function calls the INTERNAL::Exit function with the provided signal number.
     * 
     * @param Signum The signal number to exit with. Default is 0.
     */
    inline void EXIT(int Signum = 0) { INTERNAL::EXIT(Signum); }

    /**
     * @brief Processes mouse input events and updates the input list.
     * @details This function checks the state of mouse buttons (left, right, and middle)
     *          and determines if they have been pressed or clicked. It compares the current
     *          state with the previous state and the duration the button has been pressed.
     *          Based on these checks, it creates corresponding input objects and adds them
     *          to the Inputs list.
     */
    extern void mouseAPI();

    /**
     * @brief Handles mouse scroll events.
     * @details This function checks if the mouse scroll up or down button has been pressed and if the focused element is not null.
     *          If the focused element is not null, it calls the scroll up or down function on the focused element.
     */
    extern void scrollAPI();

    /**
     * @brief Returns the length of a Unicode character based on the first byte.
     * @details This function takes the first byte of a Unicode character and returns its length in bytes.
     *          If the character is not a Unicode character, it returns 1.
     * @param first_char The first byte of the character.
     * @return The length of the character in bytes.
     */
    extern int getUnicodeLength(char first_char);

    /**
     * @brief Gets the current maximum width of the terminal.
     * @details This function returns the current maximum width of the terminal. If the width is 0, it will set the carry flag to indicate that a resize is needed to be performed.
     *
     * @return The current maximum width of the terminal.
     */
    extern int getMaxWidth();

    /**
     * @brief Gets the current maximum height of the terminal.
     * @details This function returns the current maximum height of the terminal. If the height is 0, it will set the carry flag to indicate that a resize is needed to be performed.
     *
     * @return The current maximum height of the terminal.
     */
    extern int getMaxHeight();

    /**
     * @brief Converts a vector of UTFs into a Super_String.
     * @details This function takes a vector of UTFs, and converts it into a Super_String. The resulting Super_String is stored in a cache, and the cache is resized if the window size has changed.
     * @param Text The vector of UTFs to convert.
     * @param Width The width of the window.
     * @param Height The height of the window.
     * @return A pointer to the resulting Super_String.
     */
    extern std::vector<Compact_String>* liquifyUTFText(std::vector<GGUI::UTF>* Text, unsigned int& Liquefied_Size, int Width, int Height);

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
     * @brief This function is a helper for the smart memory system to recall which tasks should be prolonged, and which should be deleted.
     * @details This function is a lambda function that is used by the Atomic::Guard class to prolong or delete memories in the smart memory system.
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
    extern void nestUTFText(GGUI::element* Parent, GGUI::element* child, std::vector<GGUI::UTF> Text, std::vector<GGUI::UTF>& Parent_Buffer);

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
    extern void GGUI(STYLING_INTERNAL::style_base& App, unsigned long long Sleep_For = 0);

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
    extern void GGUI(STYLING_INTERNAL::style_base&& App, unsigned long long Sleep_For = 0);

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
     * @brief Encodes a buffer of UTF elements by setting start and end flags based on color changes.
     * 
     * @param Buffer A vector of UTF elements to be encoded.
     * @details The function marks the beginning and end of color strips within the buffer. 
     *          It checks each UTF element's foreground and background colors with its adjacent elements
     *          to determine where encoding strips start and end.
     */
    extern void encodeBuffer(std::vector<GGUI::UTF>* Buffer);

    /**
     * @brief Notifies all global buffer capturers about the latest data to be captured.
     *
     * This function is used to inform all global buffer capturers about the latest data to be captured.
     * It iterates over all global buffer capturers and calls their Sync() method to update their data.
     *
     * @param informer Pointer to the buffer capturer with the latest data.
     */
    extern void informAllGlobalBufferCaptures(INTERNAL::bufferCapture* informer);

    /**
    * @brief Retrieves an element by name.
    * @details This function takes a string argument representing the name of the element
    *          and returns a pointer to the element if it exists in the global Element_Names map.
    * @param name The name of the element to retrieve.
    * @return A pointer to the element if it exists; otherwise, nullptr.
    */
    extern element* getElement(std::string name);
}

#endif