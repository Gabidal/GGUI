#ifndef _RENDERER_H_
#define _RENDERER_H_

#undef min
#undef max

#include <functional>
#include <thread>
#include <atomic>
#include <mutex>
#include <condition_variable>

#include "../Elements/Window.h"
#include "../Elements/Text_Field.h"
#include "../Elements/List_View.h"
#include "../Elements/Canvas.h"

#include "../Core/Utils/Super_String.h"
#include "../Core/Utils/Constants.h"
#include "../Core/Utils/Color.h"
#include "../Core/Utils/Units.h"
#include "../Core/Utils/Event.h"
#include "../Core/Utils/Style.h"

//GGUI uses the ANSI escape code
//https://en.wikipedia.org/wiki/ANSI_escape_code
namespace GGUI{

    namespace INTERNAL{
        class BUFFER_CAPTURE;
    }

    extern void Report_Stack(std::string Problem);

    namespace Atomic{
        enum class Status{
            RESUMED,
            PAUSED,
            LOCKED
        };

        extern std::mutex Mutex;
        extern std::condition_variable Condition;

        extern Status Pause_Render_Thread;

        // helper to make sure all objects created by this are always treated atomically
        template<typename T>
        class Guard{
        public:
            std::mutex Shared;      // this is shared across all other threads.
            T Data;

            Guard() = default;

            void operator()(std::function<void(T&)> job){
                // check if the Shared mutex is already locked by higher/upper stack frame.
                if (Shared.try_lock()){
                    try{
                        job(Data);
                    } catch(...){
                        Report_Stack("Failed to execute the function!");
                    }

                    Shared.unlock();
                }
                else{
                    Report_Stack("Cannot double lock mutex");
                    return;
                }
            }
        };
    }

    // Inits with 'NOW()' when created
    class BUTTON_STATE {
    public:
        bool State;
        std::chrono::high_resolution_clock::time_point Capture_Time;

        BUTTON_STATE(bool state = false) : State(state), Capture_Time(std::chrono::high_resolution_clock::now()) {}
    };

    extern std::vector<UTF>& Abstract_Frame_Buffer;                 //2D clean vector without bold nor color
    extern std::string Frame_Buffer;                                //string with bold and color, this what gets drawn to console.

    extern std::vector<INTERNAL::BUFFER_CAPTURE*> Global_Buffer_Captures;

    extern unsigned int Max_Width;
    extern unsigned int Max_Height;

    extern Atomic::Guard<std::vector<Memory>> Remember;

    extern std::vector<Action*> Event_Handlers;
    extern std::vector<Input*> Inputs;
    
    extern std::unordered_map<std::string, Element*> Element_Names;

    extern Element* Focused_On;
    extern Element* Hovered_On;

    extern IVector3 Mouse;    
    extern bool Mouse_Movement_Enabled;

    extern std::unordered_map<std::string, BUTTON_STATE> KEYBOARD_STATES;

    extern time_t MAX_UPDATE_SPEED;
    extern int Inputs_Per_Second;
    extern int Inputs_Per_Query;

    extern unsigned long long Render_Delay;    // describes how long previous render cycle took in ms
    extern unsigned long long Event_Delay;    // describes how long previous memory tasks took in ms

    extern Atomic::Guard<std::unordered_map<int, Styling>> Classes;
    extern std::unordered_map<std::string, int> Class_Names;

    extern Window* Main;  

    extern std::unordered_map<GGUI::Terminal_Canvas*, bool> Multi_Frame_Canvas;

    /// @brief A platform-specific sleep function.
    /// @param mm The number of milliseconds to sleep for.
    /// @details This function is used to pause the execution of the program for a specified amount of time.
    ///          It is implemented differently for each platform, so on Windows, it calls the Sleep function,
    ///          while on Linux and macOS it calls the usleep function.
    void SLEEP(unsigned int milliseconds);

    /// @brief Checks if two rectangular areas, defined by their positions and dimensions, collide.
    /// @param A The top-left corner position of the first rectangle.
    /// @param B The top-left corner position of the second rectangle.
    /// @param A_Width Width of the first rectangle.
    /// @param A_Height Height of the first rectangle.
    /// @param B_Width Width of the second rectangle.
    /// @param B_Height Height of the second rectangle.
    /// @return True if the rectangles collide, false otherwise.
    extern bool Collides(GGUI::IVector3 A, GGUI::IVector3 B, int A_Width = 1, int A_Height = 1, int B_Width = 1, int B_Height = 1);

    /// @brief Checks if two rectangular areas, defined by their positions and dimensions, collide.
    /// @param a The first element.
    /// @param b The second element.
    /// @param Identity If a is the same as b, return this flag. Defaults into true.
    /// @return True if the rectangles collide, false otherwise.
    extern bool Collides(GGUI::Element* a, GGUI::Element* b, bool Identity = true);

    /// @brief Checks if a rectangular area, defined by an element's position and dimensions, collides with a point.
    /// @param a The element.
    /// @param b The point represented as a 3D vector.
    /// @return True if the element's area collides with the point, false otherwise.
    extern bool Collides(GGUI::Element* a, GGUI::IVector3 b);

    /// @brief Recursively searches for an element at a given position.
    /// @param c The position to search for.
    /// @param Parent The parent element to search in.
    /// @return The element at the given position, or nullptr if none was found.
    extern Element* Get_Accurate_Element_From(IVector3 c, Element* Parent);

    /// @brief Finds the element directly above the current element the mouse is hovering over.
    /// @return The position of the upper element, or the position of the current element if none is found.
    extern IVector3 Find_Upper_Element();

    /// @brief Finds the element directly below the current element the mouse is hovering over.
    /// @return The position of the lower element, or the position of the current element if none is found.
    extern IVector3 Find_Lower_Element();

    /// @brief Finds the element directly left of the current element the mouse is hovering over.
    /// @return The position of the left element, or the position of the current element if none is found.
    extern IVector3 Find_Left_Element();

    /// @brief Finds the element directly right of the current element the mouse is hovering over.
    /// @return The position of the right element, or the position of the current element if none is found.
    extern IVector3 Find_Right_Element();

    /// @brief Calculates the minimum of two signed long long integers.
    /// @param a The first value to compare.
    /// @param b The second value to compare.
    /// @return The minimum of the two values.
    extern signed long long Min(signed long long a, signed long long b);

    /// @brief Calculates the maximum of two signed long long integers.
    /// @param a The first value to compare.
    /// @param b The second value to compare.
    /// @return The maximum of the two values.
    extern signed long long Max(signed long long a, signed long long b);

    extern void ClearScreen();

    /// @brief A function to render a frame.
    /// @details This function is called from the event loop. It renders the frame by writing the Frame_Buffer data to the console.
    ///          It also moves the cursor to the top left corner of the screen.
    extern void Render_Frame();

    /// @brief Updates the maximum width and height of the console.
    /// @details This function is used to get the maximum width and height of the console.
    ///          It is called from the Query_Inputs function.
    extern void Update_Max_Width_And_Height();

    /**
     * @brief Updates the frame.
     * @details This function updates the frame. It's the main entry point for the rendering thread.
     * @note This function will return immediately if the rendering thread is paused.
     */
    void Update_Frame();

    /// @brief Waits for user input, will not translate, use Translate_Inputs for that.
    /// @details This function waits for user input and stores it in the Raw_Input array.
    ///          It is called from the event loop.
    extern void Query_Inputs();

    extern void MOUSE_API();

    // Handles also UP and DOWN buttons
    extern void SCROLL_API();

    extern void Init_Platform_Stuff();

    extern int Get_Unicode_Length(char first_char);

    extern int Get_Max_Width();

    extern int Get_Max_Height();

    //Returns a char if given ASCII, or a short if given UNICODE
    extern GGUI::UTF* Get(GGUI::IVector3 Absolute_Position);

    extern GGUI::Super_String* Liquify_UTF_Text(std::vector<GGUI::UTF>& Text, int Width, int Height);

    extern void Update_Frame();
    
    /**
     * @brief Pauses the rendering thread.
     * @details This function pauses the rendering thread. The thread will wait until the rendering thread is resumed.
     */
    extern void Pause_GGUI();

    /**
     * @brief Resumes the rendering thread.
     * @details This function resumes the rendering thread after it has been paused.
     * @param restore_render_to The status to restore the rendering thread to.
     */
    extern void Resume_GGUI(Atomic::Status restore_render_to = Atomic::Status::RESUMED);

    /**
     * @brief This function is a helper for the smart memory system to recall which tasks should be prolonged, and which should be deleted.
     * @details This function is a lambda function that is used by the Atomic::Guard class to prolong or delete memories in the smart memory system.
     *          It takes a pointer to a vector of Memory objects and prolongs or deletes the memories in the vector based on the time difference between the current time and the memory's start time.
     */
    extern void Recall_Memories();

    /**
     * @brief Checks if the given flag is set in the given flags.
     * @details This function takes two unsigned long long parameters, one for the flags and one for the flag to check. It returns true if the flag is set in the flags, otherwise it returns false.
     *
     * @param f The flags to check.
     * @param Flag The flag to check for.
     * @return True if the flag is set, otherwise false.
     */
    extern bool Is(unsigned long long f, unsigned long long Flag);

    /**
     * @brief Removes focus from the currently focused element and its children.
     * @details This function checks if there is a currently focused element.
     *          If there is, it sets the focus state on the element and its children to false.
     *          Focus is only removed if the element's current focus state differs from the desired state.
     */
    extern void Un_Focus_Element();

    /**
     * @brief Removes the hover state from the currently hovered element and its children.
     * @details This function checks if there is a currently hovered element.
     *          If there is, it sets the hover state on the element and its children to false.
     *          Hover is only removed if the element's current hover state differs from the desired state.
     */
    extern void Un_Hover_Element();

    /**
     * @brief Updates the currently focused element to a new candidate.
     * @details This function checks if the new candidate is the same as the current focused element.
     *          If not, it removes the focus from the current element and all its children.
     *          Then, it sets the focus on the new candidate element and all its children.
     * @param new_candidate The new element to focus on.
     */
    extern void Update_Focused_Element(GGUI::Element* new_candidate);

    /**
     * @brief Updates the currently hovered element to a new candidate.
     * @details This function checks if the new candidate is the same as the current hovered element.
     *          If not, it removes the hover state from the current element and all its children.
     *          Then, it sets the hover state on the new candidate element and all its children.
     * @param new_candidate The new element to hover on.
     */
    extern void Update_Hovered_Element(GGUI::Element* new_candidate);

    /**
     * @brief Handles all events in the system.
     * @details This function goes through all event handlers and checks if the event criteria matches any of the inputs.
     *          If a match is found, it calls the event handler job with the input as an argument.
     *          If the job is successful, it removes the input from the list of inputs.
     *          If the job is unsuccessful, it reports an error.
     */
    extern void Event_Handler();

    /**
     * Get the ID of a class by name, assigning a new ID if it doesn't exist.
     * 
     * @param n The name of the class.
     * @return The ID of the class.
     */
    extern int Get_Free_Class_ID(std::string n);

    /**
     * @brief Adds a new class with the specified name and styling.
     * @details This function retrieves a unique class ID for the given name.
     *          It then associates the provided styling with this class ID 
     *          in the `Classes` map.
     * 
     * @param name The name of the class.
     * @param Styling The styling to be associated with the class.
     */
    extern void Add_Class(std::string name, Styling Styling);

    /**
     * @brief Initializes the GGUI system and returns the main window.
     * 
     * @return The main window of the GGUI system.
     */
    extern GGUI::Window* Init_GGUI();

    /**
     * @brief Reports an error to the user.
     * @param Problem The error message to display.
     * @note If the main window is not created yet, the error will be printed to the console.
     * @note This function is thread safe.
     */
    extern void Report(std::string Problem);

    /**
     * @brief Nests a text buffer into a parent buffer while considering the childs position and size.
     * 
     * @param Parent The parent element which the text is being nested into.
     * @param child The child element which's text is being nested.
     * @param Text The text buffer to be nested.
     * @param Parent_Buffer The parent buffer which the text is being nested into.
     */
    extern void Nest_UTF_Text(GGUI::Element* Parent, GGUI::Element* child, std::vector<GGUI::UTF> Text, std::vector<GGUI::UTF>& Parent_Buffer);

    /**
     * @brief Pauses all other GGUI internal threads and calls the given function.
     * @details This function will pause all other GGUI internal threads and call the given function.
     * @param f The function to call.
     */
    extern void Pause_GGUI(std::function<void()> f);

    /**
     * @brief Use GGUI in a simple way.
     * @details This is a simple way to use GGUI. It will pause all other GGUI internal threads, initialize GGUI, call the given function, sleep for the given amount of milliseconds, and then exit GGUI.
     * @param DOM The function that will add all the elements to the root window.
     * @param Sleep_For The amount of milliseconds to sleep after calling the given function.
     */
    extern void GGUI(std::function<void()> DOM, unsigned long long Sleep_For = 0);

    /**
     * @brief Use GGUI in a simple way.
     * @details This is a simple way to use GGUI. It will pause all other GGUI internal threads, initialize GGUI, add all the elements to the root window, sleep for the given amount of milliseconds, and then exit GGUI.
     * @param DOM The elements to add to the root window.
     * @param Sleep_For The amount of milliseconds to sleep after calling the given function.
     */
    extern void GGUI(std::vector<Element*> DOM, unsigned long long Sleep_For = 0);

    /// @brief Cleanly exits the GGUI library.
    /// @details This function is called automatically when the application exits, or can be called manually to exit the library at any time.
    ///          It ensures that any platform-specific settings are reset before the application exits.
    /// @param signum The exit code to return to the operating system.
    extern void Exit(int Signum = 0);

    /**
     * @brief Handles the pressing of the tab key.
     * @details This function selects the next tabbed element as focused and not hovered.
     *          If the shift key is pressed, it goes backwards in the list of tabbed elements.
     */
    extern void Handle_Tabulator();

    /**
     * @brief Handles escape key press events.
     * @details This function checks if the escape key has been pressed and if the focused element is not null.
     *          If the focused element is not null, it calls the Un_Focus_Element function to remove the focus.
     *          If the focused element is null but the hovered element is not null, it calls the Un_Hover_Element
     *          function to remove the hover.
     */
    extern void Handle_Escape();

    /**
     * @brief Encodes a buffer of UTF elements by setting start and end flags based on color changes.
     * 
     * @param Buffer A vector of UTF elements to be encoded.
     * @details The function marks the beginning and end of color strips within the buffer. 
     *          It checks each UTF element's foreground and background colors with its adjacent elements
     *          to determine where encoding strips start and end.
     */
    extern void Encode_Buffer(std::vector<GGUI::UTF>& Buffer);

    /**
     * @brief Initializes the inspect tool.
     * @details This function initializes the inspect tool which is a debug tool that displays the number of elements, render time, and event time.
     * @see GGUI::Update_Stats
     */
    extern void Init_Inspect_Tool();

    /**
     * @brief Notifies all global buffer capturers about the latest data to be captured.
     *
     * This function is used to inform all global buffer capturers about the latest data to be captured.
     * It iterates over all global buffer capturers and calls their Sync() method to update their data.
     *
     * @param informer Pointer to the buffer capturer with the latest data.
     */
    extern void Inform_All_Global_BUFFER_CAPTURES(INTERNAL::BUFFER_CAPTURE* informer);

    /**
     * @brief Determines if a given pointer is likely deletable (heap-allocated).
     *
     * This function assesses whether a pointer may belong to the heap by comparing its
     * position relative to known memory sections such as the stack, heap, and data segments.
     *
     * @param ptr Pointer to be evaluated.
     * @return True if the pointer is likely deletable (heap-allocated), false otherwise.
     */
    extern bool Is_Deletable(void* ptr);
}

#endif