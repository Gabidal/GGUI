#include "renderer.h"
#include "./utils/fileStreamer.h"
#include "./utils/logger.h"
#include "./utils/utils.h"
#include "./utils/constants.h"
#include "./addons/addons.h"
#include "./utils/settings.h"
#include "./utils/drm.h"
#include "./utils/fastVector.h"

#include <string>
#include <cassert>
#include <math.h>
#include <sstream>
#include <cstdio>
#include <exception>
#include <csignal>

#if _WIN32
    #include <windows.h>
    #include <dbghelp.h>
#else
    #include <sys/ioctl.h>
    #include <signal.h>
    #include <termios.h>
    #include <unistd.h>
    #include <sys/uio.h> // Needed for writev
    #include <errno.h>
    #include <cstring>
    #include <poll.h>
#endif

namespace GGUI{

    // User provided cleanup callbacks.
    static std::vector<std::function<void()>> User_Cleanup_Callbacks;
    void registerCleanupCallback(std::function<void()> Callback) {
        User_Cleanup_Callbacks.push_back(Callback);
    }

    namespace INTERNAL{
        std::vector<UTF>* abstractFrameBuffer = nullptr;              // 2D clean vector without bold nor color
        struct platformState {
            bool Screen_Capture_Enabled = false;
            bool Alternative_Screen_Enabled = false;
            bool Mouse_Reporting_Enabled = false;
            bool Cursor_Hidden = false;
            bool Raw_Mode_Enabled = false;          // POSIX only
            bool Initialized = false;
#if _WIN32
            unsigned long Previous_Windows_Codepage = 0;
#endif
            bool De_Initialized = false;
        } Platform_State;

        std::string* frameBuffer;                                      // string with bold and color, this what gets drawn to console.

        // For threading system
        namespace atomic{
            std::mutex mutex;
            std::condition_variable condition;
            
            int LOCKED = 0;
            status pauseRenderThread = status::NOT_INITIALIZED;
        }

        std::vector<std::thread> Sub_Threads;

        std::vector<bufferCapture*> globalBufferCaptures;

        unsigned int maxWidth = 0;
        unsigned int maxHeight = 0;

        atomic::guard<std::vector<memory>> remember;

        std::vector<action*> eventHandlers;
        std::vector<input*> inputs;
        std::chrono::system_clock::time_point Last_Input_Clear_Time;

        std::unordered_map<std::string, element*> elementNames;

        element* focusedOn = nullptr;
        element* hoveredOn = nullptr;

        bool platformInitialized = false;

        // Whether STDIN is an interactive terminal (TTY). Used to decide input setup/teardown and whether to spawn the input thread.
        static bool STDIN_IS_TTY = false;

        IVector3 mouse;
        //move 1 by 1, or element by element.
        bool mouseMovementEnabled = true;

        std::unordered_map<std::string_view, buttonState> KEYBOARD_STATES;
        std::unordered_map<std::string_view, buttonState> PREVIOUS_KEYBOARD_STATES;

        // Represents the update speed of each elapsed loop of passive events, which do NOT need user as an input.
        time_t CURRENT_UPDATE_SPEED = MAX_UPDATE_SPEED;
        inline float eventThreadLoad = 0.0f;  // Describes the load of animation and events from 0.0 to 1.0. Will reduce the event thread pause.

        std::chrono::high_resolution_clock::time_point Previous_Time;
        std::chrono::high_resolution_clock::time_point Current_Time;

        unsigned long long renderDelay;    // describes how long previous render cycle took in ms
        unsigned long long eventDelay;    // describes how long previous memory tasks took in ms
        unsigned long long Input_Delay;     // describes how long previous input tasks took in ms

        inline atomic::guard<std::unordered_map<int, styling>> classes;

        inline std::unordered_map<std::string, int> classNames;

        std::unordered_map<GGUI::canvas*, bool> multiFrameCanvas;

        void* Stack_Start_Address = 0;
        void* Heap_Start_Address = 0;

        element* main = nullptr;

        atomic::guard<carry> Carry_Flags; 

        /**
         * @brief Temporary function to return the current date and time in a string.
         * @return A string of the current date and time in the format "DD.MM.YYYY: SS.MM.HH"
         * @note This function will be replaced when the Date_Element is implemented.
         */
        std::string now(){
            // This function takes the current time and returns a string of the time.
            // Format: DD.MM.YYYY: SS.MM.HH
            std::time_t now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());

            std::string Result = std::ctime(&now);

            // Remove the newline at the end of the string
            return Result.substr(0, Result.size() - 1);
        }
        
        /**
         * @brief Initializes the start addresses for stack and heap.
         * 
         * This function is made extern to prevent inlining. It is responsible
         * for capturing and initializing the nearest stack and heap addresses 
         * and assigning them to the respective global variables.
         */

        extern void Read_Start_Addresses();

        #if _WIN32

        HANDLE GLOBAL_STD_OUTPUT_HANDLE;
        HANDLE GLOBAL_STD_INPUT_HANDLE;

        DWORD PREVIOUS_CONSOLE_OUTPUT_STATE;
        DWORD PREVIOUS_CONSOLE_INPUT_STATE;

        CONSOLE_SCREEN_BUFFER_INFO Get_Console_Info();

        // This is here out from the Query_Inputs, so that we can differentiate querying and translation of said input.
        const unsigned int Raw_Input_Capacity = UINT8_MAX * 10;
        INPUT_RECORD Raw_Input[Raw_Input_Capacity];
        int Raw_Input_Size = 0;

        /**
         * @brief Checks whether STDIN is connected to an interactive Windows Console.
         * @details When input is redirected (piped / file), GetConsoleMode will fail.
         * @return true if STDIN is a console, otherwise false.
         */
        bool Is_Stdin_TTY(){
            if (GLOBAL_STD_INPUT_HANDLE == 0)
                GLOBAL_STD_INPUT_HANDLE = GetStdHandle(STD_INPUT_HANDLE);

            if (GLOBAL_STD_INPUT_HANDLE == NULL || GLOBAL_STD_INPUT_HANDLE == INVALID_HANDLE_VALUE)
                return false;

            DWORD Mode = 0;
            return GetConsoleMode(GLOBAL_STD_INPUT_HANDLE, &Mode) != 0;
        }

        
        /**
         * @brief De-initializes the renderer by cleaning up resources and restoring console states.
         * 
         * This function performs the following tasks:
         * - Cleans up file stream handles by manually calling their destructors.
         * - Clears the list of file stream handles.
         * - Restores the previous console modes for both standard output and input.
         * - Disables specific ANSI features and restores the screen state.
         * - Flushes the output to ensure all data is written to the console.
         */
        void deInitialize(){
            if (Platform_State.De_Initialized)
                return; // Guard against double execution.

            // Execute user-provided cleanup callbacks first (they may rely on live terminal state).
            for (auto& Callback : User_Cleanup_Callbacks){
                if (Callback) Callback();
            }
            User_Cleanup_Callbacks.clear();

            // fileStreamerHandles now holds unique_ptr; clearing map will destroy streams.
            fileStreamerHandles.clear();

            // Restore previous console modes
            {
                DWORD Mode = 0;
                if (GetConsoleMode(GLOBAL_STD_OUTPUT_HANDLE, &Mode))
                    SetConsoleMode(GLOBAL_STD_OUTPUT_HANDLE, PREVIOUS_CONSOLE_OUTPUT_STATE);
            }
            {
                DWORD Mode = 0;
                if (GetConsoleMode(GLOBAL_STD_INPUT_HANDLE, &Mode))
                    SetConsoleMode(GLOBAL_STD_INPUT_HANDLE, PREVIOUS_CONSOLE_INPUT_STATE);
            }

            // Restore code page if it was changed
            if (Platform_State.Previous_Windows_Codepage != 0){
                SetConsoleOutputCP(Platform_State.Previous_Windows_Codepage);
            }

            // Symmetric disabling of features we enabled.
            if (Platform_State.Cursor_Hidden)
                std::cout << GGUI::constants::ANSI::enablePrivateSGRFeature(GGUI::constants::ANSI::MOUSE_CURSOR).toString();
            if (Platform_State.Mouse_Reporting_Enabled)
                std::cout << GGUI::constants::ANSI::enablePrivateSGRFeature(GGUI::constants::ANSI::REPORT_MOUSE_ALL_EVENTS, false).toString();
            if (Platform_State.Screen_Capture_Enabled)
                std::cout << GGUI::constants::ANSI::enablePrivateSGRFeature(GGUI::constants::ANSI::SCREEN_CAPTURE, false).toString();
            if (Platform_State.Alternative_Screen_Enabled)
                std::cout << GGUI::constants::ANSI::enablePrivateSGRFeature(GGUI::constants::ANSI::ALTERNATIVE_SCREEN_BUFFER, false).toString();

            // Reset all SGR (text style / color) attributes so the parent shell is not left with altered colors.
            std::cout << GGUI::constants::ANSI::enableSGRFeature(GGUI::constants::ANSI::RESET_SGR).toString();
            // We don't want to touch the cursor shape.
            std::cout << std::flush; // Ensure all output is flushed to console

            Platform_State.De_Initialized = true;
        }

        void Cleanup(){
            if (!Carry_Flags.read().terminate){
                waitForThreadTermination();

                // Join the threads
                for (auto& thread : Sub_Threads){
                    if (thread.joinable()) thread.join();
                }

                LOGGER::log("Reverting to normal console mode...");

                // Clean up platform-specific resources and settings (idempotent)
                deInitialize();

                LOGGER::log("GGUI shutdown successful.");
            }
        }

        /**
         * @brief Converts exception information to a human-readable string.
         *
         * This function takes a pointer to an EXCEPTION_POINTERS structure and converts
         * the exception information into a formatted string using std::string. The string
         * includes the exception code, exception address, and context information (Rip for
         * x64 systems or Eip for x86 systems).
         *
         * @param exceptionInfo A pointer to an EXCEPTION_POINTERS structure containing the
         *                      exception information.
         * @return A std::string containing the formatted exception information.
         */
        std::string Exception_To_String(EXCEPTION_POINTERS* exceptionInfo) {
            // Convert exception info to a string using only std::string
            std::string result;

            result += "Exception Code: " + std::to_string(exceptionInfo->ExceptionRecord->ExceptionCode) + "\n";
            result += "Exception Address: " + std::to_string(reinterpret_cast<uintptr_t>(exceptionInfo->ExceptionRecord->ExceptionAddress)) + "\n";

            // Handle different architectures (x86 vs x64)
            #if defined(_M_X64) || defined(__x86_64__)
                // For x64 systems, use Rip
                result += "Context (Rip): " + std::to_string((exceptionInfo->ContextRecord->Rip)) + "\n";
            #elif defined(_M_IX86) || defined(__i386__)
                // For x86 systems, use Eip
                result += "Context (Eip): " + std::to_string((exceptionInfo->ContextRecord->Eip)) + "\n";
            #else
                result += "Context: Architecture not supported\n";
            #endif

            return result;
        }

        /**
         * @brief Handles critical errors and access violations.
         * 
         * This function is a custom exception handler that logs critical errors and access violations.
         * It logs the exception code and the address where the exception occurred, then terminates the program gracefully.
         * 
         * @param exceptionInfo A pointer to an EXCEPTION_POINTERS structure that contains the exception information.
         * @return This function does not return a value.
         */
        LONG WINAPI Critical_Error_Handler(EXCEPTION_POINTERS* exceptionInfo) {
            LOGGER::log("Access violation or critical error occurred.");
            LOGGER::log("Exception Code: " + std::to_string(exceptionInfo->ExceptionRecord->ExceptionCode));
            LOGGER::log("Exception Address: " + std::to_string(reinterpret_cast<uintptr_t>(exceptionInfo->ExceptionRecord->ExceptionAddress)));
            LOGGER::log(Exception_To_String(exceptionInfo));    // Dump
            EXIT(EXIT_FAILURE); // Graceful termination
            return EXCEPTION_EXECUTE_HANDLER;   // For warning fillers, since the execution should not extend to this line.
        }

        BOOL WINAPI Console_Handler(DWORD signal) {
            std::string signal_name = "";
            switch (signal)
            {
            case CTRL_C_EVENT:
                signal_name = "CTRL+C";
                break;
            case CTRL_BREAK_EVENT:
                signal_name = "CTRL+BREAK";
                break;
            case CTRL_CLOSE_EVENT:
                signal_name = "CTRL+CLOSE";
                break;
            case CTRL_LOGOFF_EVENT:
                signal_name = "CTRL+LOGOFF";
                break;
            case CTRL_SHUTDOWN_EVENT:
                signal_name = "CTRL+SHUTDOWN";
                break;
            default:
                break;
            }

            if (signal_name.size() != 0) {
                LOGGER::log("Terminated via " + signal_name + " signal.");
                EXIT(EXIT_SUCCESS); 
                return TRUE;
            }
            return FALSE;
        }

        /**
         * @brief Renders the current frame to the console.
         * 
         * This function moves the console cursor to the top left corner of the screen
         * and writes the contents of the Frame_Buffer to the console.
         * 
         * @note The number of bytes written to the console is stored in a temporary
         * variable but is not used elsewhere in the function.
         */
        void renderFrame(){
            // The number of bytes written to the console, not used anywhere else.
            unsigned long long tmp = 0;
            // Move the cursor to the top left corner of the screen.
            SetConsoleCursorPosition(GLOBAL_STD_OUTPUT_HANDLE, {0, 0});
            // Write the Frame_Buffer data to the console.
            WriteFile(GLOBAL_STD_OUTPUT_HANDLE, frameBuffer->data(), frameBuffer->size(), reinterpret_cast<LPDWORD>(&tmp), NULL);
        }

        /**
         * @brief Updates the maximum width and height of the console window.
         * 
         * This function retrieves the current console screen buffer information and updates
         * the maximum width and height based on the console window dimensions. If the console
         * information is not retrieved correctly, an error message is reported. Additionally,
         * if the main window is active, its dimensions are set to the updated maximum width
         * and height.
         */
        void updateMaxWidthAndHeight(){
            // Get the console information.
            CONSOLE_SCREEN_BUFFER_INFO info = Get_Console_Info();

            // Update the maximum width and height.
            maxWidth = info.dwSize.X;
            maxHeight = info.dwSize.Y;

            // Check if we got the console information correctly.
            if (maxWidth == 0 || maxHeight == 0){
                reportStack("Failed to get console info!");
            }

            // Check that the main window is not active and if so, set its dimensions.
            if (main)
                main->setDimensions(maxWidth, maxHeight);
        }

        /**
         * @brief Reverse engineers keybinds based on the provided keybind value.
         *
         * This function checks the current state of specific key combinations and returns
         * the corresponding character if a known keybind is detected. If no keybind is detected,
         * it returns the original keybind value.
         *
         * @param keybind_value The value of the keybind to be checked.
         * @return The character corresponding to the detected keybind, or the original keybind value if no keybind is detected.
         *
         * @note The current known keybinding table:
         *       CTRL+SHIFT+I => TAB
         *       // TODO: Add more keybinds to the table
         */
        char Reverse_Engineer_Keybinds(char keybind_value){
            // The current known keybinding table:

            /*
                CTRL+SHIFT+I => TAB
                // TODO: Add more keybinds to the table
            */

            if (KEYBOARD_STATES[KEYBOARD_BUTTONS::CONTROL].state && KEYBOARD_STATES[KEYBOARD_BUTTONS::SHIFT].state){
                if (keybind_value == VK_TAB){
                    return 'i';
                }
            }

            // return the normal value, if there is no key-binds detected.
            return keybind_value;
        }

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
        void queryInputs(){
            // Read the console input and store it in Raw_Input.
            ReadConsoleInput(
                GLOBAL_STD_INPUT_HANDLE,
                Raw_Input,
                Raw_Input_Capacity,
                (LPDWORD)&Raw_Input_Size
            );
        }

        /**
         * @brief Translates raw input events into internal input states and actions.
         * 
         * This function processes raw input events, such as keyboard and mouse events, and updates the internal input states accordingly.
         * It handles various types of input events including key presses, mouse movements, and window buffer size changes.
         * 
         * The function performs the following tasks:
         * - Cleans the previous keyboard states.
         * - Iterates through the raw input events and processes each event based on its type.
         * - For key events, it updates the internal input states and pushes corresponding inputs to the internal input list.
         * - For window buffer size events, it sets a flag indicating that a resize is needed.
         * - For mouse events, it updates the mouse coordinates and handles mouse button states and scroll events.
         * 
         * @note This function assumes that the raw input buffer will be fully translated by the end of its execution.
         * 
         * @param None
         * @return void
         */
        void Translate_Inputs(){
            // Clean the keyboard states.
            INTERNAL::PREVIOUS_KEYBOARD_STATES = INTERNAL::KEYBOARD_STATES;

            for (int i = 0; i < Raw_Input_Size; i++){
                if (Raw_Input[i].EventType == KEY_EVENT){

                    bool Pressed = Raw_Input[i].Event.KeyEvent.bKeyDown;

                    if (Raw_Input[i].Event.KeyEvent.wVirtualKeyCode == VK_UP){
                        INTERNAL::inputs.push_back(new GGUI::input(0, GGUI::constants::UP));
                        INTERNAL::KEYBOARD_STATES[KEYBOARD_BUTTONS::UP] = INTERNAL::buttonState(Pressed);
                    }
                    else if (Raw_Input[i].Event.KeyEvent.wVirtualKeyCode == VK_DOWN){
                        INTERNAL::inputs.push_back(new GGUI::input(0, GGUI::constants::DOWN));
                        INTERNAL::KEYBOARD_STATES[KEYBOARD_BUTTONS::DOWN] = INTERNAL::buttonState(Pressed);
                    }
                    else if (Raw_Input[i].Event.KeyEvent.wVirtualKeyCode == VK_LEFT){
                        INTERNAL::inputs.push_back(new GGUI::input(0, GGUI::constants::LEFT));
                        INTERNAL::KEYBOARD_STATES[KEYBOARD_BUTTONS::LEFT] = INTERNAL::buttonState(Pressed);
                    }
                    else if (Raw_Input[i].Event.KeyEvent.wVirtualKeyCode == VK_RIGHT){
                        INTERNAL::inputs.push_back(new GGUI::input(0, GGUI::constants::RIGHT));
                        INTERNAL::KEYBOARD_STATES[KEYBOARD_BUTTONS::RIGHT] = INTERNAL::buttonState(Pressed);
                    }
                    else if (Raw_Input[i].Event.KeyEvent.wVirtualKeyCode == VK_RETURN){
                        INTERNAL::inputs.push_back(new GGUI::input('\n', GGUI::constants::ENTER));
                        INTERNAL::KEYBOARD_STATES[KEYBOARD_BUTTONS::ENTER] = INTERNAL::buttonState(Pressed);
                    }
                    else if (Raw_Input[i].Event.KeyEvent.wVirtualKeyCode == VK_SHIFT){
                        INTERNAL::inputs.push_back(new GGUI::input(' ', GGUI::constants::SHIFT));
                        INTERNAL::KEYBOARD_STATES[KEYBOARD_BUTTONS::SHIFT] = INTERNAL::buttonState(Pressed);
                    }
                    else if (Raw_Input[i].Event.KeyEvent.wVirtualKeyCode == VK_CONTROL){
                        INTERNAL::inputs.push_back(new GGUI::input(' ', GGUI::constants::CONTROL));
                        INTERNAL::KEYBOARD_STATES[KEYBOARD_BUTTONS::CONTROL] = INTERNAL::buttonState(Pressed);
                    }
                    else if (Raw_Input[i].Event.KeyEvent.wVirtualKeyCode == VK_BACK){
                        INTERNAL::inputs.push_back(new GGUI::input(' ', GGUI::constants::BACKSPACE));
                        INTERNAL::KEYBOARD_STATES[KEYBOARD_BUTTONS::BACKSPACE] = INTERNAL::buttonState(Pressed);
                    }
                    else if (Raw_Input[i].Event.KeyEvent.wVirtualKeyCode == VK_ESCAPE){
                        INTERNAL::inputs.push_back(new GGUI::input(' ', GGUI::constants::ESCAPE));
                        INTERNAL::KEYBOARD_STATES[KEYBOARD_BUTTONS::ESC] = INTERNAL::buttonState(Pressed);
                        handleEscape();
                    }
                    else if (Raw_Input[i].Event.KeyEvent.wVirtualKeyCode == VK_TAB){
                        INTERNAL::inputs.push_back(new GGUI::input(' ', GGUI::constants::TAB));
                        INTERNAL::KEYBOARD_STATES[KEYBOARD_BUTTONS::TAB] = INTERNAL::buttonState(Pressed);
                        handleTabulator();
                    }
                    else if (Raw_Input[i].Event.KeyEvent.uChar.AsciiChar != 0 && Pressed){
                        char Result = Reverse_Engineer_Keybinds(Raw_Input[i].Event.KeyEvent.uChar.AsciiChar);
                        INTERNAL::inputs.push_back(new GGUI::input(Result, GGUI::constants::KEY_PRESS));
                    }
                }
                else if (Raw_Input[i].EventType == WINDOW_BUFFER_SIZE_EVENT){
                    INTERNAL::Carry_Flags([](GGUI::INTERNAL::carry& current_carry){
                        current_carry.resize = true;    // Tell the render thread that an resize is needed to be performed.
                    });
                }
                else if (Raw_Input[i].EventType == MOUSE_EVENT && INTERNAL::mouseMovementEnabled){
                    if (Raw_Input[i].Event.MouseEvent.dwEventFlags == MOUSE_MOVED){
                        // Get mouse coordinates
                        COORD mousePos = Raw_Input[i].Event.MouseEvent.dwMousePosition;
                        // Handle cursor movement
                        INTERNAL::mouse.x = mousePos.X;
                        INTERNAL::mouse.y = mousePos.Y;
                    }
                    // Handle mouse clicks
                    if ((Raw_Input[i].Event.MouseEvent.dwButtonState & FROM_LEFT_1ST_BUTTON_PRESSED) != 0) {
                        //PREVIOUS_KEYBOARD_STATES[KEYBOARD_BUTTONS::MOUSE_LEFT].State = KEYBOARD_STATES[KEYBOARD_BUTTONS::MOUSE_LEFT].State;
                        INTERNAL::KEYBOARD_STATES[KEYBOARD_BUTTONS::MOUSE_LEFT].state = true;
                        INTERNAL::KEYBOARD_STATES[KEYBOARD_BUTTONS::MOUSE_LEFT].captureTime = std::chrono::high_resolution_clock::now();
                    }
                    else if ((Raw_Input[i].Event.MouseEvent.dwButtonState & FROM_LEFT_1ST_BUTTON_PRESSED) == 0) {
                        //PREVIOUS_KEYBOARD_STATES[KEYBOARD_BUTTONS::MOUSE_LEFT].State = KEYBOARD_STATES[KEYBOARD_BUTTONS::MOUSE_LEFT].State;
                        INTERNAL::KEYBOARD_STATES[KEYBOARD_BUTTONS::MOUSE_LEFT].state = false;
                    }

                    if ((Raw_Input[i].Event.MouseEvent.dwButtonState & RIGHTMOST_BUTTON_PRESSED) != 0) {
                        //PREVIOUS_KEYBOARD_STATES[KEYBOARD_BUTTONS::MOUSE_RIGHT].State = KEYBOARD_STATES[KEYBOARD_BUTTONS::MOUSE_RIGHT].State;
                        INTERNAL::KEYBOARD_STATES[KEYBOARD_BUTTONS::MOUSE_RIGHT].state = true;
                        INTERNAL::KEYBOARD_STATES[KEYBOARD_BUTTONS::MOUSE_RIGHT].captureTime = std::chrono::high_resolution_clock::now();
                    }
                    else if ((Raw_Input[i].Event.MouseEvent.dwButtonState & RIGHTMOST_BUTTON_PRESSED) == 0) {
                        //PREVIOUS_KEYBOARD_STATES[KEYBOARD_BUTTONS::MOUSE_RIGHT].State = KEYBOARD_STATES[KEYBOARD_BUTTONS::MOUSE_RIGHT].State;
                        INTERNAL::KEYBOARD_STATES[KEYBOARD_BUTTONS::MOUSE_RIGHT].state = false;
                    }
                
                    // mouse scroll up
                    if (Raw_Input[i].Event.MouseEvent.dwEventFlags == MOUSE_WHEELED){
                        // check if it has been wheeled up or down
                        int Scroll_Direction = GET_WHEEL_DELTA_WPARAM(Raw_Input[i].Event.MouseEvent.dwButtonState);

                        if (Scroll_Direction > 0){
                            INTERNAL::KEYBOARD_STATES[KEYBOARD_BUTTONS::MOUSE_SCROLL_UP].state = true;
                            INTERNAL::KEYBOARD_STATES[KEYBOARD_BUTTONS::MOUSE_SCROLL_DOWN].state = false;

                            INTERNAL::KEYBOARD_STATES[KEYBOARD_BUTTONS::MOUSE_SCROLL_UP].captureTime = std::chrono::high_resolution_clock::now();
                        }
                        else if (Scroll_Direction < 0){
                            INTERNAL::KEYBOARD_STATES[KEYBOARD_BUTTONS::MOUSE_SCROLL_DOWN].state = true;
                            INTERNAL::KEYBOARD_STATES[KEYBOARD_BUTTONS::MOUSE_SCROLL_UP].state = false;

                            INTERNAL::KEYBOARD_STATES[KEYBOARD_BUTTONS::MOUSE_SCROLL_DOWN].captureTime = std::chrono::high_resolution_clock::now();
                        }
                    }
                }
            }
    
            // We can assume that the Raw_Input buffer will be fully translated by this point, if not, then something is wrong!!!
            // We can now also restart the Raw_Input_Size.
            Raw_Input_Size = 0;
        }

        /**
         * @brief Initializes platform-specific settings for the console.
         * 
         * This function performs the following tasks:
         * - Saves the standard input and output handles to global variables to minimize redundant calls.
         * - Retrieves and stores the previous console modes for both input and output to allow restoration upon exit.
         * - Sets new console modes with extended flags, enabling mouse and window input.
         * - Enables specific ANSI features for mouse event reporting and hides the mouse cursor.
         * - Sets the console output code page to UTF-8 mode.
         * - Marks the platform as initialized.
         */
        void initPlatformStuff(){
            constexpr int ENABLE_UTF8_MODE_FOR_WINDOWS = 65001;

            // Save the STD handles to prevent excess calls.
            GLOBAL_STD_OUTPUT_HANDLE = GetStdHandle(STD_OUTPUT_HANDLE);
            GLOBAL_STD_INPUT_HANDLE = GetStdHandle(STD_INPUT_HANDLE);

            // Detect whether STDIN is a TTY. When not a TTY (e.g. piped/timeout), avoid input-specific console mode setup.
            STDIN_IS_TTY = Is_Stdin_TTY();

            // Retrieve and store previous console modes for restoration upon exit.
            GetConsoleMode(GLOBAL_STD_OUTPUT_HANDLE, &PREVIOUS_CONSOLE_OUTPUT_STATE);
            if (STDIN_IS_TTY)
                GetConsoleMode(GLOBAL_STD_INPUT_HANDLE, &PREVIOUS_CONSOLE_INPUT_STATE);

            // Set new console modes with extended flags, mouse, and window input enabled (preserve existing output mode flags).
            DWORD Current_Output_Mode = 0;
            GetConsoleMode(GLOBAL_STD_OUTPUT_HANDLE, &Current_Output_Mode);
            // We do not forcibly overwrite output mode with -1. Input mode is extended for mouse + window events.
            if (STDIN_IS_TTY)
                SetConsoleMode(GLOBAL_STD_INPUT_HANDLE, ENABLE_EXTENDED_FLAGS | ENABLE_MOUSE_INPUT | ENABLE_WINDOW_INPUT | ENABLE_PROCESSED_INPUT);

            // Capture previous code page and switch to UTF-8 if different.
            Platform_State.Previous_Windows_Codepage = GetConsoleOutputCP();
            if (Platform_State.Previous_Windows_Codepage != (unsigned long)ENABLE_UTF8_MODE_FOR_WINDOWS)
                SetConsoleOutputCP(ENABLE_UTF8_MODE_FOR_WINDOWS);

            // Enable specific ANSI features for mouse event reporting and hide the mouse cursor.
            if (STDIN_IS_TTY){
                std::cout << GGUI::constants::ANSI::enablePrivateSGRFeature(GGUI::constants::ANSI::REPORT_MOUSE_ALL_EVENTS).toString();
                Platform_State.Mouse_Reporting_Enabled = true;
                std::cout << GGUI::constants::ANSI::enablePrivateSGRFeature(GGUI::constants::ANSI::MOUSE_CURSOR, false).toString();
                Platform_State.Cursor_Hidden = true;
            }
            std::cout.flush();

            // Critical error handlers.
            SetUnhandledExceptionFilter(Critical_Error_Handler);

            if (!SetConsoleCtrlHandler(Console_Handler, TRUE)){
                reportStack("Failed to set console handler!");
            }

            // These are for most generic cases not needed, but could be useful if GGUI is called inside another utility, like windows version of timeout utility.
            for (
                auto i : {
                    SIGINT,
                    SIGILL,
                    SIGABRT,
                    SIGFPE,
                    SIGSEGV,
                    SIGTERM
                }){
                std::signal(i, []([[maybe_unused]] int dummy){
                    GenerateConsoleCtrlEvent(CTRL_C_EVENT, 0);  // trigger the Console_Handler
                });
            }

            // Mark the platform as initialized.
            platformInitialized = true;
            Platform_State.Initialized = true;
        }

        /**
         * @brief Retrieves information about the console screen buffer.
         *
         * This function obtains the handle to the standard output console if it is not already set.
         * It then retrieves the console screen buffer information, including the size and cursor position.
         * If the function fails to retrieve the information, it reports the error.
         *
         * @return CONSOLE_SCREEN_BUFFER_INFO structure containing the console screen buffer information.
         */
        CONSOLE_SCREEN_BUFFER_INFO Get_Console_Info() {
            if (GLOBAL_STD_OUTPUT_HANDLE == 0) {
                GLOBAL_STD_OUTPUT_HANDLE = GetStdHandle(STD_OUTPUT_HANDLE);
            }

            CONSOLE_SCREEN_BUFFER_INFO Result;

            // first get the size of the file
            if (!GetConsoleScreenBufferInfo(GLOBAL_STD_OUTPUT_HANDLE, &Result)){
                int Last_Error = GetLastError();

                reportStack("Failed to get console info: " + std::to_string(Last_Error));
            }

            return Result;
        }

        /**
         * @brief Reads the current console screen buffer and returns its contents as a vector of characters.
         * 
         * This function retrieves the console screen buffer information, allocates a vector of CHAR_INFO structures
         * with the same size as the screen buffer, and reads the console screen buffer into this vector. It then
         * extracts the ASCII characters from the CHAR_INFO structures and stores them in a vector of characters,
         * which is returned.
         * 
         * @return std::vector<char> A vector containing the ASCII characters from the console screen buffer.
         */
        std::vector<char> Read_Console(){
            // Get the console screen buffer information
            CONSOLE_SCREEN_BUFFER_INFO Info = Get_Console_Info();

            // Allocate a vector of CHAR_INFO structures with the same size as the screen buffer
            std::vector<char> Buffer(Info.dwSize.X * Info.dwSize.Y);
            std::vector<CHAR_INFO> Fake_Buffer(Info.dwSize.X * Info.dwSize.Y);

            // The area to read
            SMALL_RECT rect {0,0, Info.dwSize.X-1, Info.dwSize.Y} ; // the console screen (buffer) region to read from (120x4)

            // Read the console screen buffer into the vector of CHAR_INFO structures
            ReadConsoleOutput( GLOBAL_STD_OUTPUT_HANDLE, Fake_Buffer.data(), {Info.dwSize.X, Info.dwSize.Y} /*buffer size cols x rows*/, {0,0} /*buffer top,left*/, &rect );

            // Copy the AsciiChar member of each CHAR_INFO structure into the corresponding position in the output vector
            for (unsigned int i = 0; i < Fake_Buffer.size(); i++){
                Buffer[i] = Fake_Buffer[i].Char.AsciiChar;
            }

            return Buffer;
        }

        /**
         * @brief Retrieves a list of font files installed on the system.
         * 
         * This function opens the Windows registry key "Software\\Microsoft\\Windows NT\\CurrentVersion\\Fonts"
         * and enumerates all the values within that key. Each value represents a font file installed on the system.
         * The function collects the file names of these fonts and returns them in a vector of strings.
         * 
         * @return std::vector<std::string> A vector containing the names of the font files installed on the system.
         */
        std::vector<std::string> Get_List_Of_Font_Files() {
            std::vector<std::string> Result;

            // Open the "Software\\Microsoft\\Windows NT\\CurrentVersion\\Fonts" key in the registry
            HKEY hKey;
            if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, "Software\\Microsoft\\Windows NT\\CurrentVersion\\Fonts", 0, KEY_READ, &hKey) == ERROR_SUCCESS) {
                DWORD cvalues; 
                // Get the number of values in the key
                RegQueryInfoKey(hKey, NULL, NULL, NULL, NULL, NULL, NULL, &cvalues, NULL, NULL, NULL, NULL);
                for (DWORD i = 0; i < cvalues; i++) {
                    char valueName[1024];
                    DWORD valueNameSize = 1024; 
                    BYTE valueData[1024];
                    DWORD valueDataSize = 1024;
                    DWORD valueType;
                    // Enumerate the values in the key
                    if (RegEnumValue(hKey, i, valueName, &valueNameSize, NULL, &valueType, valueData, &valueDataSize) == ERROR_SUCCESS) {
                        // Add the file to the Result list
                        Result.push_back((char*)valueData);
                    }
                }
                // Close the key
                RegCloseKey(hKey);
            }

            return Result;
        }

        #else

        int Previous_Flags = 0;
        struct termios Previous_Raw;

        // Stored globally, so that translation and inquiry can be separate proccess.
        const unsigned int Raw_Input_Capacity = UINT8_MAX * 2;
        unsigned char Raw_Input[Raw_Input_Capacity];
        ssize_t Raw_Input_Size = 0;

        /**
         * @brief Checks whether STDIN is connected to an interactive terminal (TTY).
         * @return true if STDIN is a TTY, otherwise false.
         */
        bool Is_Stdin_TTY(){ return (isatty(STDIN_FILENO) == 1); }

        /**
         * @brief De-initializes platform-specific settings and resources.
         * @details This function restores console modes to their previous states, cleans up file stream handles,
         *          and disables specific ANSI features. It ensures that any platform-specific settings are reset
         *          before the application exits.
         */
        void deInitialize(){
            if (Platform_State.De_Initialized)
                return; // Guard against double execution.

            // Execute user cleanup callbacks.
            for (auto& Callback : User_Cleanup_Callbacks){
                if (Callback) Callback();
            }
            User_Cleanup_Callbacks.clear();

            // Unique_ptr map cleanup.
            fileStreamerHandles.clear();

            if (!SETTINGS::enableDRM) {
                if (Platform_State.Cursor_Hidden)
                    std::cout << GGUI::constants::ANSI::enablePrivateSGRFeature(GGUI::constants::ANSI::MOUSE_CURSOR).toString();
                if (Platform_State.Mouse_Reporting_Enabled)
                    std::cout << GGUI::constants::ANSI::enablePrivateSGRFeature(GGUI::constants::ANSI::REPORT_MOUSE_ALL_EVENTS, false).toString();
                if (Platform_State.Screen_Capture_Enabled)
                    std::cout << GGUI::constants::ANSI::enablePrivateSGRFeature(GGUI::constants::ANSI::SCREEN_CAPTURE, false).toString();
                if (Platform_State.Alternative_Screen_Enabled)
                    std::cout << GGUI::constants::ANSI::enablePrivateSGRFeature(GGUI::constants::ANSI::ALTERNATIVE_SCREEN_BUFFER, false).toString();
                std::cout << GGUI::constants::ANSI::enableSGRFeature(GGUI::constants::ANSI::RESET_SGR).toString();

                // Do NOT force a cursor shape; leave user / outer environment shape intact (we never changed shape explicitly).
                std::cout << std::flush;

                if (STDIN_IS_TTY && Platform_State.Raw_Mode_Enabled) {
                    fcntl(STDIN_FILENO, F_SETFL, Previous_Flags);
                    tcsetattr(STDIN_FILENO, TCSAFLUSH, &Previous_Raw);
                    LOGGER::log("Restored previous terminal raw mode configuration.");
                } else if (!STDIN_IS_TTY) {
                    LOGGER::log("STDIN was not a TTY; no terminal mode restoration needed.");
                }
            }

            Platform_State.De_Initialized = true;
        }

        void Cleanup(){
            if (!Carry_Flags.read().terminate){
                waitForThreadTermination();
                
                for (auto& thread : Sub_Threads){
                    if (thread.joinable()) thread.join();
                }

                LOGGER::log("Reverting to normal console mode...");

                if (SETTINGS::enableDRM)
                    DRM::close();

                deInitialize();

                LOGGER::log("GGUI shutdown successful.");
            }
        }

        /**
         * @brief Renders the contents of the Frame_Buffer to the standard output (STDOUT).
         * @details This function moves the cursor to the top-left corner of the terminal, flushes the output
         *          buffer to ensure immediate writing, and writes the contents of the Frame_Buffer to STDOUT.
         *          If the write operation fails or writes fewer bytes than expected, an error message is reported.
         */
        void renderFrame() {
            // Write cursor-home, then the frame buffer. Avoid stdio printf/fflush.
            const char* cursorReset = GGUI::constants::ANSI::SET_CURSOR_TO_START.getUnicode();
            size_t cursorResetLength = strlen(cursorReset);

            iovec vec[2] = {
                { (void*)cursorReset,           cursorResetLength },
                { (void*)frameBuffer->data(),  frameBuffer->size() }
            };

            ssize_t wrote = writev(STDOUT_FILENO, vec, 2);
            if (wrote != (ssize_t)cursorResetLength + (ssize_t)frameBuffer->size()) {
                LOGGER::log("Failed to write to STDOUT (home): " + std::to_string((int)wrote));
            }
        }

        /**
         * @brief Updates the maximum width and height of the terminal.
         * @details This function updates the maxWidth and Max_Height variables by calling ioctl to get the current
         *          width and height of the terminal. If the call fails, a report message is sent.
         * @note The height is reduced by 1 to account for the one line of console space taken by the GGUI status bar.
         */
        void updateMaxWidthAndHeight(){
            if (!SETTINGS::enableDRM){  // window size comes from DRM is DRM is enabled.
                struct winsize w;
                if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &w) == -1){
                    INTERNAL::reportStack("Failed to get console info!");
                    return;
                }

                maxWidth = w.ws_col;
                maxHeight = w.ws_row - 1;

                // Convenience sake :)
                if (main)
                    main->setDimensions(maxWidth, maxHeight);
            }
        }

        /**
         * @brief Adds a signal handler to automatically update the terminal size whenever a SIGWINCH signal is received.
         * @details This function sets up a signal handler to detect when the terminal size changes and updates the
         *          Carry_Flags to indicate that a resize is needed. This is necessary because the render thread needs
         *          to update the maximum width and height of the terminal whenever a resize occurs.
         */
        void Add_Automatic_Terminal_Size_Update_Handler(){
            // To automatically update GGUI max dimensions, we need to make an WINCH Signal handler.
            struct sigaction Handler;
            
            // Setup the function handler with a lambda
            Handler.sa_handler = []([[maybe_unused]] int signum){
                // When the signal is received, update the carry flags to indicate that a resize is needed
                Carry_Flags([](carry& current_carry){
                    current_carry.resize = true;    // Tell the render thread that an resize is needed to be performed.
                });
            };

            // Clears any other handler which could potentially hinder this handler.
            sigemptyset(&Handler.sa_mask);

            // Since sigaction flags does not get auto constructed, we need to clean it.
            Handler.sa_flags = 0;

            // Now set this handler up.
            sigaction(SIGWINCH, &Handler, nullptr);
        }

        /**
         * @brief Takes in an buffer with hex and octal values and transforms them into printable strings
         * @details This function takes in a buffer with hex and octal values and transforms them into printable strings
         *          for better visualization of the buffer contents.
         *
         * @param buffer The buffer to transform into a printable string
         * @param length The length of the buffer
         * @param Obfuscate A boolean flag to control whether the buffer contents should be obfuscated or not. If set to true,
         *                  any non-printable characters in the buffer will be replaced with a space.
         *
         * @return A string containing the transformed buffer contents
         */
        std::string To_String(char* buffer, int length, bool Obfuscate = false){
            std::string Result = "";

            for (int i = 0; i < length; i++){
                // This is used as an precession to avoid accidental ANSI escape sequences to trigger from printing the buffer contents.
                if (Obfuscate)
                    Result += " ";

                if (isprint(buffer[i])){
                    Result += buffer[i];
                    continue;
                }

                // add base
                Result += std::to_string((unsigned char)buffer[i]);
            }

            return Result;
        }

        /**
         * @brief Waits for user input and stores it in the Raw_Input array.
         * @details This function waits for user input and stores it in the Raw_Input array. It is called from the event loop.
         *          It is also the function that is called as soon as possible and gets stuck awaiting for the user input.
         */
        void queryInputs(){
            // If stdin isn't a TTY (e.g., piped/timeout), read() may return 0 (EOF) repeatedly; avoid spinning.
            if (!STDIN_IS_TTY) {
                // Use poll to wait briefly for readability; if not readable, sleep a bit to avoid busy-loop.
                struct pollfd pollFileDescriptor;
                pollFileDescriptor.fd = STDIN_FILENO;
                pollFileDescriptor.events = POLLIN;
                pollFileDescriptor.revents = 0;

                constexpr nfds_t  fileDescriptorCount = 1;

                if (poll(
                    &pollFileDescriptor,
                    fileDescriptorCount,
                    TIME::SECOND    // Max allowed wait time, could be replaced with -1, to wait as long as needed.
                ) <= 0) {
                    // No data; avoid spinning
                    Raw_Input_Size = 0;
                    return;
                }
            }

            Raw_Input_Size = read(STDIN_FILENO, Raw_Input, Raw_Input_Capacity);
            if (Raw_Input_Size <= 0) {
                // EOF or error; normalize to 0 to signal no input
                Raw_Input_Size = 0;
            }
        }

        enum class VTTermModifiers{
            SHIFT       = 1 << 0,
            ALT         = 1 << 1,
            CONTROL     = 1 << 2,
            SUPER      = 1 << 3,
        };

        /**
         * @brief Translate the input events stored in Raw_Input into Input objects.
         * @details This function is used to translate the input events stored in Raw_Input into Input objects. 
         * It checks if the event is a key event, and if so, it checks if the key is a special key (up, down, left, right, enter, shift, control, backspace, escape, tab) and if so, it creates an Input object with the corresponding constants:: value. 
         * If the key is not a special key, it creates an Input object with the key's ASCII value and constants::KEY_PRESS. 
         * If the event is not a key event, it checks if the event is a mouse event and if so, it checks if the mouse event is a movement, click or scroll event and if so, it creates an Input object with the corresponding constants:: value. 
         * Finally, it resets the Raw_Input_Size to 0.
         */
        void Translate_Inputs(){
            constexpr char START_OF_CTRL = 1;
            constexpr char END_OF_CTRL = 26;
            // Clean the keyboard states.
            PREVIOUS_KEYBOARD_STATES = KEYBOARD_STATES;

            // Unlike in Windows we wont be getting an indication per Key information, whether it was pressed in or out.
            KEYBOARD_STATES.clear();

            if (Raw_Input_Size <= 0) {
                return; // nothing to translate
            }

            for (ssize_t i = 0; i < Raw_Input_Size; i++) {                // Check if SHIFT has been modifying the keys
                if ((Raw_Input[i] >= 'A' && Raw_Input[i] <= 'Z') || (Raw_Input[i] >= '!' && Raw_Input[i] <= '/')) {
                    // SHIFT key is pressed
                    inputs.push_back(new GGUI::input(' ', constants::SHIFT));
                    KEYBOARD_STATES[KEYBOARD_BUTTONS::SHIFT] = buttonState(true);
                }

                // ACC ASCII character handlers.
                else if (Raw_Input[i] >= START_OF_CTRL && Raw_Input[i] <= END_OF_CTRL) {
                    // This is a CTRL key                    // The CTRL domain contains multiple useful keys to check for
                    if (Raw_Input[i] == constants::ANSI::BACKSPACE) {
                        // This is a backspace key
                        inputs.push_back(new GGUI::input(' ', constants::BACKSPACE));
                        KEYBOARD_STATES[KEYBOARD_BUTTONS::BACKSPACE] = buttonState(true);
                    }
                    else if (Raw_Input[i] == constants::ANSI::HORIZONTAL_TAB) {
                        // This is a tab key
                        inputs.push_back(new GGUI::input(' ', constants::TAB));
                        KEYBOARD_STATES[KEYBOARD_BUTTONS::TAB] = buttonState(true);
                        handleTabulator();
                    }
                    else if (Raw_Input[i] == constants::ANSI::LINE_FEED) {
                        // This is an enter key
                        inputs.push_back(new GGUI::input('\n', constants::ENTER));
                        KEYBOARD_STATES[KEYBOARD_BUTTONS::ENTER] = buttonState(true);
                    }
                    else{
                        // Since we cannot discern between ACC and ctrl+characters, we'll just yolo it for now and assume it works.
                        Raw_Input[i] += 'A'-1;  // Since A is encoded as 1, we need to subtract 1 to get the correct ASCII value.
                        // This is an ctrl key
                        inputs.push_back(new GGUI::input(' ', constants::CONTROL));
                        KEYBOARD_STATES[KEYBOARD_BUTTONS::CONTROL] = buttonState(true);
                    }
                }                if (Raw_Input[i] == constants::ANSI::ESC_CODE[0]) {
                    // check if there are stuff after this escape code
                    if (i + 1 >= Raw_Input_Size) {
                        // Clearly the escape key was invoked
                        inputs.push_back(new GGUI::input(' ', constants::ESCAPE));
                        KEYBOARD_STATES[KEYBOARD_BUTTONS::ESC] = buttonState(true);
                        handleEscape();
                        continue;
                    }

                    // GG go next
                    i++;

                    // The current data can either be an ALT key initiative or an escape sequence followed by '['
                    if (Raw_Input[i] == constants::ANSI::ESC_CODE[1]) {
                        // Escape sequence codes:

                        // Check for modifiers with base [1;
                        if (Raw_Input[i+1] == '1' && Raw_Input[i+2] == ';'){
                            i += 2;

                            unsigned char Modifier = (Raw_Input[i + 1] - '0') - 1;

                            switch (Modifier) {
                                case (unsigned char)VTTermModifiers::SHIFT:
                                    inputs.push_back(new GGUI::input(' ', constants::SHIFT));
                                    KEYBOARD_STATES[KEYBOARD_BUTTONS::SHIFT] = buttonState(true);
                                    break;

                                case (unsigned char)VTTermModifiers::ALT:
                                    inputs.push_back(new GGUI::input(' ', constants::ALT));
                                    KEYBOARD_STATES[KEYBOARD_BUTTONS::ALT] = buttonState(true);
                                    break;

                                case (unsigned char)VTTermModifiers::CONTROL:
                                    inputs.push_back(new GGUI::input(' ', constants::CONTROL));
                                    KEYBOARD_STATES[KEYBOARD_BUTTONS::CONTROL] = buttonState(true);
                                    break;

                                case (unsigned char)VTTermModifiers::SUPER:
                                    inputs.push_back(new GGUI::input(' ', constants::SUPER));
                                    KEYBOARD_STATES[KEYBOARD_BUTTONS::SUPER] = buttonState(true);
                                    break;

                                default:
                                    // Unknown modifier, ignore
                                    break;
                            }

                            i += 2; // Skip the modifier and the semicolon
                        }

                        // UP, DOWN LEFT, RIGHT keys
                        if (Raw_Input[i + 1] == 'A') {
                            inputs.push_back(new GGUI::input(0, constants::UP));
                            KEYBOARD_STATES[KEYBOARD_BUTTONS::UP] = buttonState(true);
                            i++;
                        }
                        else if (Raw_Input[i + 1] == 'B') {
                            inputs.push_back(new GGUI::input(0, constants::DOWN));
                            KEYBOARD_STATES[KEYBOARD_BUTTONS::DOWN] = buttonState(true);
                            i++;
                        }
                        else if (Raw_Input[i + 1] == 'C') {
                            inputs.push_back(new GGUI::input(0, constants::RIGHT));
                            KEYBOARD_STATES[KEYBOARD_BUTTONS::RIGHT] = buttonState(true);
                            i++;
                        }
                        else if (Raw_Input[i + 1] == 'D') {
                            inputs.push_back(new GGUI::input(0, constants::LEFT));
                            KEYBOARD_STATES[KEYBOARD_BUTTONS::LEFT] = buttonState(true);
                            i++;
                        }
                        else if (Raw_Input[i + 1] == 'M') {  // Decode X10 Mouse handling
                            // Payload structure: '\e[Mbxy' where the b is bitmask representing the buttons, x and y representing the location of the mouse. 
                            char Bit_Mask = Raw_Input[i + 2];

                            // Check if the bit 2'rd has been set, is so then the SHIFT has been pressed
                            if (Bit_Mask & 4) {
                                inputs.push_back(new GGUI::input(' ', constants::SHIFT));
                                KEYBOARD_STATES[KEYBOARD_BUTTONS::SHIFT] = buttonState(true);
                                // also remove the bit from the bitmask
                                Bit_Mask &= ~4;
                            }

                            // Check if the 3'th bit has been set, is so then the SUPER has been pressed
                            if (Bit_Mask & 8) {
                                inputs.push_back(new GGUI::input(' ', constants::SUPER));
                                KEYBOARD_STATES[KEYBOARD_BUTTONS::SUPER] = buttonState(true);
                                // also remove the bit from the bitmask
                                Bit_Mask &= ~8;
                            }

                            // Check if the 4'th bit has been set, is so then the CTRL has been pressed
                            if (Bit_Mask & 16) {
                                inputs.push_back(new GGUI::input(' ', constants::CONTROL));
                                KEYBOARD_STATES[KEYBOARD_BUTTONS::CONTROL] = buttonState(true);
                                // also remove the bit from the bitmask
                                Bit_Mask &= ~16;
                            }

                            // Bit 5'th is not widely supported so remove it in case.
                            Bit_Mask &= ~32;

                            // Check if the 6'th bit has been set, is so then there is a movement event.
                            if (Bit_Mask & 64) {
                                unsigned char X = Raw_Input[i + 3];
                                unsigned char Y = Raw_Input[i + 4];

                                // XTERM will normally shift its X and Y coordinates by 32, so that it skips all the control characters in ASCII.
                                mouse.x = Max(X - 32 - 1, 0);   // The additional -1 is so that the mouse cursor top left point works as the actual focus point of the mouse.
                                mouse.y = Max(Y - 32 - 1, 0);

                                Bit_Mask &= ~64;
                            }

                            // Bits 7'th are not widely supported. But clear this bit just in case
                            Bit_Mask &= ~(128);

                            if (Bit_Mask == 0) {
                                KEYBOARD_STATES[KEYBOARD_BUTTONS::MOUSE_LEFT] = buttonState(true);
                                KEYBOARD_STATES[KEYBOARD_BUTTONS::MOUSE_LEFT].captureTime = std::chrono::high_resolution_clock::now();
                            }
                            else if (Bit_Mask == 1) {
                                KEYBOARD_STATES[KEYBOARD_BUTTONS::MOUSE_MIDDLE] = buttonState(true);
                                KEYBOARD_STATES[KEYBOARD_BUTTONS::MOUSE_MIDDLE].captureTime = std::chrono::high_resolution_clock::now();
                            }
                            else if (Bit_Mask == 2) {
                                KEYBOARD_STATES[KEYBOARD_BUTTONS::MOUSE_RIGHT] = buttonState(true);
                                KEYBOARD_STATES[KEYBOARD_BUTTONS::MOUSE_RIGHT].captureTime = std::chrono::high_resolution_clock::now();
                            }
                            else if (Bit_Mask == 3) {
                                KEYBOARD_STATES[KEYBOARD_BUTTONS::MOUSE_LEFT].state = false;
                                KEYBOARD_STATES[KEYBOARD_BUTTONS::MOUSE_MIDDLE].state = false;
                                KEYBOARD_STATES[KEYBOARD_BUTTONS::MOUSE_RIGHT].state = false;
                            }

                            i += 4;
                        }
                        else if (Raw_Input[i + 1] == 'Z') {
                            // SHIFT + TAB => Z
                            inputs.push_back(new GGUI::input(' ', constants::SHIFT));
                            inputs.push_back(new GGUI::input(' ', constants::TAB));

                            KEYBOARD_STATES[KEYBOARD_BUTTONS::SHIFT] = buttonState(true);
                            KEYBOARD_STATES[KEYBOARD_BUTTONS::TAB] = buttonState(true);

                            handleTabulator();

                            i++;
                        }
                        else if (Raw_Input[i + 1] == '<' && Raw_Input[i + 2] == 'b'){   // Decode SGR Mouse handling
                            // SGR mouse: ESC [ < b ; x ; y ( M | m )
                            // ---------------------------------------
                            // layout: '[' '<' b ';' x ';' y ('M' = press | 'm' = release)
                            ssize_t k = i + 3;

                            // helper to parse a decimal integer
                            auto parseNum = [&](int &out){
                                out = 0;
                                while (k < Raw_Input_Size && isdigit(Raw_Input[k])) {
                                    out = out * 10 + (Raw_Input[k] - '0');
                                    ++k;
                                }
                            };

                            int mask, mx, my;
                            parseNum(mask);
                            if (Raw_Input[k] == ';') ++k;
                            parseNum(mx);
                            if (Raw_Input[k] == ';') ++k;
                            parseNum(my);

                            // final char tells us press vs. release
                            char action = (k < Raw_Input_Size ? Raw_Input[k] : '\0');

                            // advance i to the end of this sequence
                            i = k;

                            // Map reported coords directly
                            INTERNAL::mouse.x = mx;
                            INTERNAL::mouse.y = my;

                            // Extract modifiers
                            bool shift   = (mask & 4) != 0;
                            bool alt     = (mask & 8) != 0;
                            bool control = (mask & 16) != 0;

                            if (shift) {
                                INTERNAL::KEYBOARD_STATES[KEYBOARD_BUTTONS::SHIFT] = buttonState(true);
                                INTERNAL::inputs.push_back(new GGUI::input(' ', GGUI::constants::SHIFT));
                            }
                            if (alt) {
                                INTERNAL::KEYBOARD_STATES[KEYBOARD_BUTTONS::ALT] = buttonState(true);
                                INTERNAL::inputs.push_back(new GGUI::input(' ', GGUI::constants::ALT));
                            }
                            if (control) {
                                INTERNAL::KEYBOARD_STATES[KEYBOARD_BUTTONS::CONTROL] = buttonState(true);
                                INTERNAL::inputs.push_back(new GGUI::input(' ', GGUI::constants::CONTROL));
                            }

                            // Button ID: low two bits
                            int btn = mask & 0x03;
                            bool pressed = (action == 'M');

                            switch (btn) {
                                case 0: // left
                                    KEYBOARD_STATES[KEYBOARD_BUTTONS::MOUSE_LEFT] = buttonState(pressed);
                                    KEYBOARD_STATES[KEYBOARD_BUTTONS::MOUSE_LEFT].captureTime = std::chrono::high_resolution_clock::now();
                                    break;
                                case 1: // middle
                                    KEYBOARD_STATES[KEYBOARD_BUTTONS::MOUSE_MIDDLE] = buttonState(pressed);
                                    KEYBOARD_STATES[KEYBOARD_BUTTONS::MOUSE_MIDDLE].captureTime = std::chrono::high_resolution_clock::now();
                                    break;
                                case 2: // right
                                    KEYBOARD_STATES[KEYBOARD_BUTTONS::MOUSE_RIGHT] = buttonState(pressed);
                                    KEYBOARD_STATES[KEYBOARD_BUTTONS::MOUSE_RIGHT].captureTime = std::chrono::high_resolution_clock::now();
                                    break;
                                case 3: // release all buttons
                                    // you may want to clear all three
                                    KEYBOARD_STATES[KEYBOARD_BUTTONS::MOUSE_LEFT]   = buttonState(false);
                                    KEYBOARD_STATES[KEYBOARD_BUTTONS::MOUSE_MIDDLE] = buttonState(false);
                                    KEYBOARD_STATES[KEYBOARD_BUTTONS::MOUSE_RIGHT]  = buttonState(false);
                                    break;
                            }

                            continue;
                        }
                        
                        
                    
                    }
                    else {
                        // This is an ALT key
                        inputs.push_back(new GGUI::input(Raw_Input[i], constants::ALT));
                        KEYBOARD_STATES[KEYBOARD_BUTTONS::ALT] = buttonState(true);
                    }
                }
                else if (Raw_Input[i] >= ' ' && Raw_Input[i] <= '~') {
                    // Normal character data
                    inputs.push_back(new GGUI::input(Raw_Input[i], constants::KEY_PRESS));
                }
                else if (Raw_Input[i] == constants::ANSI::DEL){
                    inputs.push_back(new GGUI::input(' ', constants::BACKSPACE));
                    KEYBOARD_STATES[KEYBOARD_BUTTONS::BACKSPACE] = buttonState(true);
                }
            }

            // We can assume that at the end of user input translation, all buffered inputs are hereby translate and no need to store, so reset offset.
            Raw_Input_Size = 0;
        }

        /**
         * @brief Initializes platform-specific settings for console handling.
         * @details This function sets up the console handles and modes required for input and output operations.
         *          It enables mouse and window input, sets UTF-8 mode for output, and prepares the console for
         *          handling specific ANSI features.
         */
        void initPlatformStuff(){
            if (!SETTINGS::enableDRM) {
                // Initialize the console for mouse and window input, and set UTF-8 mode for output.
                std::cout << constants::ANSI::enablePrivateSGRFeature(constants::ANSI::REPORT_MOUSE_ALL_EVENTS).toString();
                Platform_State.Mouse_Reporting_Enabled = true;
                std::cout << constants::ANSI::enablePrivateSGRFeature(constants::ANSI::MOUSE_CURSOR, false).toString();
                Platform_State.Cursor_Hidden = true;
                std::cout << constants::ANSI::enablePrivateSGRFeature(constants::ANSI::SCREEN_CAPTURE).toString();   // for on exit to restore
                Platform_State.Screen_Capture_Enabled = true;
                // std::cout << constants::RESET_CONSOLE;
                // std::cout << constants::EnableFeature(constants::ALTERNATIVE_SCREEN_BUFFER);    // For double buffer if needed
                std::cout << std::flush;

                // Detect whether STDIN is a TTY. When not a TTY (e.g. piped/timeout), avoid raw mode and polling setup.
                STDIN_IS_TTY = Is_Stdin_TTY();

                if (STDIN_IS_TTY) {
                    // Save the current flags and take a snapshot of the flags before GGUI
                    Previous_Flags = fcntl(STDIN_FILENO, F_GETFL, 0);

                    // Ensure we don't leave the descriptor in an unexpected mode; do not force non-blocking here.
                    // Configure terminal to raw mode
                    struct termios Term_Handle;
                    if (tcgetattr(STDIN_FILENO, &Term_Handle) == 0) {
                        Previous_Raw = Term_Handle;
                        Term_Handle.c_lflag &= ~(ECHO | ICANON);
                        Term_Handle.c_cc[VMIN] = 1;   // return after 1 byte
                        Term_Handle.c_cc[VTIME] = 0;  // no timeout
                        tcsetattr(STDIN_FILENO, TCSAFLUSH, &Term_Handle);
                        Platform_State.Raw_Mode_Enabled = true;
                    }

                    // Add a signal handler to automatically update the terminal size whenever a SIGWINCH signal is received.
                    Add_Automatic_Terminal_Size_Update_Handler();
                } else {
                    LOGGER::log("STDIN is not a TTY; input thread will be disabled unless DRM is enabled.");
                }
            }

            /*
            SIGINT -> This is on by default, if then user makes an element to be focused with capabilities to capture this, then well remove it from the Exit codes.
            SIGILL
            SIGABRT
            SIGFPE
            SIGSEGV
            SIGTERM
            */

            // Register the exit handler for the following signals
            struct sigaction normal_exit = {};
            normal_exit.sa_handler = []([[maybe_unused]] int dummy){
                exit(EXIT_SUCCESS);  // Ensures `atexit()` is triggered
            };
            sigemptyset(&normal_exit.sa_mask);
            normal_exit.sa_flags = 0;

            for (
                auto i : {
                    SIGINT,
                    SIGILL,
                    SIGABRT,
                    SIGFPE,
                    SIGSEGV,
                    SIGTERM
                }){
                sigaction(i, &normal_exit, NULL);
            }

            if (atexit([](){Cleanup();})){
                LOGGER::log("Failed to register exit handler.");
            }

            Platform_State.Initialized = true;
        }


        /**
         * @brief Get a list of all font files in the system.
         *        This function uses the "fc-list" command to get a list of all font files in the system.
         *        The output is then processed to extract the file names and return them as a vector.
         * @return A vector of strings containing all the font file names.
         */
        std::vector<std::string> Get_List_Of_Font_Files() {
            CMD Handle;

            // Run the command and store the output in Raw_Result
            std::string Raw_Result = Handle.run("fc-list -v | grep file");

            std::vector<std::string> File_Names;
            std::stringstream ss(Raw_Result);
            std::string temp;

            // Process the output line by line
            while (std::getline(ss, temp, '\n')) {
                // Extract the file name from each line and add it to the vector
                std::size_t pos = temp.find(": ");
                if (pos != std::string::npos) {
                    std::string file_name = temp.substr(pos + 2);
                    File_Names.push_back(file_name);
                }
            }

            return File_Names;
        }

        #endif

        /**
         * @brief Populate inputs for keys that are held down.
         * @details This function iterates over the current keyboard states and creates new input objects
         *          for keys that are held down and not already present in the inputs list. It skips mouse button keys.
         */
        void Populate_Inputs_For_Held_Down_Keys() {
            for (auto Key : INTERNAL::KEYBOARD_STATES) {

                // Check if the key is activated
                if (Key.second.state) {

                    // Skip mouse button keys
                    if (KEYBOARD_BUTTONS::MOUSE_LEFT == Key.first || KEYBOARD_BUTTONS::MOUSE_RIGHT == Key.first || KEYBOARD_BUTTONS::MOUSE_MIDDLE == Key.first)
                        continue;

                    // Get the constant associated with the key
                    unsigned long long Constant_Key = BUTTON_STATES_TO_CONSTANTS_BRIDGE[Key.first];

                    // Check if the input already exists
                    bool Found = false;
                    for (auto input : INTERNAL::inputs) {
                        if (input->criteria == Constant_Key) {
                            Found = true;
                            break;
                        }
                    }

                    // If not found, create a new input
                    if (!Found)
                        INTERNAL::inputs.push_back(new input((char)0, Constant_Key));
                }
            }
        }

        /**
         * @brief Processes mouse input events and updates the input list.
         * @details This function checks the state of mouse buttons (left, right, and middle)
         *          and determines if they have been pressed or clicked. It compares the current
         *          state with the previous state and the duration the button has been pressed.
         *          Based on these checks, it creates corresponding input objects and adds them
         *          to the Inputs list.
         * @note This function related on click events when the current keyboard state for that specific key is NOT on!
         */
        void mouseAPI() {
            // Get the duration the left mouse button has been pressed
            unsigned long long Mouse_Left_Pressed_For = (unsigned long long)std::chrono::duration_cast<std::chrono::milliseconds>(abs(Current_Time - INTERNAL::KEYBOARD_STATES[KEYBOARD_BUTTONS::MOUSE_LEFT].captureTime)).count();

            // Check if the left mouse button is pressed and for how long
            if (INTERNAL::KEYBOARD_STATES[KEYBOARD_BUTTONS::MOUSE_LEFT].state && Mouse_Left_Pressed_For >= SETTINGS::mousePressDownCooldown) {
                INTERNAL::inputs.push_back(new GGUI::input(0, constants::MOUSE_LEFT_PRESSED));
            } 
            // Check if the left mouse button was previously pressed and now released
            else if (!INTERNAL::KEYBOARD_STATES[KEYBOARD_BUTTONS::MOUSE_LEFT].state && INTERNAL::PREVIOUS_KEYBOARD_STATES[KEYBOARD_BUTTONS::MOUSE_LEFT].state != INTERNAL::KEYBOARD_STATES[KEYBOARD_BUTTONS::MOUSE_LEFT].state) {
                INTERNAL::inputs.push_back(new GGUI::input(0, constants::MOUSE_LEFT_CLICKED));
            }

            // Get the duration the right mouse button has been pressed
            unsigned long long Mouse_Right_Pressed_For = (unsigned long long)std::chrono::duration_cast<std::chrono::milliseconds>(abs(Current_Time - INTERNAL::KEYBOARD_STATES[KEYBOARD_BUTTONS::MOUSE_RIGHT].captureTime)).count();

            // Check if the right mouse button is pressed and for how long
            if (INTERNAL::KEYBOARD_STATES[KEYBOARD_BUTTONS::MOUSE_RIGHT].state && Mouse_Right_Pressed_For >= SETTINGS::mousePressDownCooldown) {
                INTERNAL::inputs.push_back(new GGUI::input(0, constants::MOUSE_RIGHT_PRESSED));
            }
            // Check if the right mouse button was previously pressed and now released
            else if (!INTERNAL::KEYBOARD_STATES[KEYBOARD_BUTTONS::MOUSE_RIGHT].state && INTERNAL::PREVIOUS_KEYBOARD_STATES[KEYBOARD_BUTTONS::MOUSE_RIGHT].state != INTERNAL::KEYBOARD_STATES[KEYBOARD_BUTTONS::MOUSE_RIGHT].state) {
                INTERNAL::inputs.push_back(new GGUI::input(0, constants::MOUSE_RIGHT_CLICKED));
            }

            // Get the duration the middle mouse button has been pressed
            unsigned long long Mouse_Middle_Pressed_For = (unsigned long long)std::chrono::duration_cast<std::chrono::milliseconds>(abs(Current_Time - INTERNAL::KEYBOARD_STATES[KEYBOARD_BUTTONS::MOUSE_MIDDLE].captureTime)).count();

            // Check if the middle mouse button is pressed and for how long
            if (INTERNAL::KEYBOARD_STATES[KEYBOARD_BUTTONS::MOUSE_MIDDLE].state && Mouse_Middle_Pressed_For >= SETTINGS::mousePressDownCooldown) {
                INTERNAL::inputs.push_back(new GGUI::input(0, constants::MOUSE_MIDDLE_PRESSED));
            }
            // Check if the middle mouse button was previously pressed and now released
            else if (!INTERNAL::KEYBOARD_STATES[KEYBOARD_BUTTONS::MOUSE_MIDDLE].state && INTERNAL::PREVIOUS_KEYBOARD_STATES[KEYBOARD_BUTTONS::MOUSE_MIDDLE].state != INTERNAL::KEYBOARD_STATES[KEYBOARD_BUTTONS::MOUSE_MIDDLE].state) {
                INTERNAL::inputs.push_back(new GGUI::input(0, constants::MOUSE_MIDDLE_CLICKED));
            }
        }

        /**
         * @brief Handles mouse scroll events.
         * @details This function checks if the mouse scroll up or down button has been pressed and if the focused element is not null.
         *          If the focused element is not null, it calls the scroll up or down function on the focused element.
         */
        void scrollAPI(){
            // Check if the mouse scroll up button has been pressed
            if (KEYBOARD_STATES[KEYBOARD_BUTTONS::MOUSE_SCROLL_UP].state){

                // If the focused element is not null, call the scroll up function
                if (focusedOn)
                    focusedOn->scrollUp();
            }
            // Check if the mouse scroll down button has been pressed
            else if (KEYBOARD_STATES[KEYBOARD_BUTTONS::MOUSE_SCROLL_DOWN].state){

                // If the focused element is not null, call the scroll down function
                if (focusedOn)
                    focusedOn->scrollDown();
            }
        }

        /**
         * @brief Handles escape key press events.
         * @details This function checks if the escape key has been pressed and if the focused element is not null.
         *          If the focused element is not null, it calls the Un_Focus_Element function to remove the focus.
         *          If the focused element is null but the hovered element is not null, it calls the Un_Hover_Element
         *          function to remove the hover.
         */
        void handleEscape(){
            // Check if the escape key has been pressed
            if (!KEYBOARD_STATES[KEYBOARD_BUTTONS::ESC].state)
                return;

            // If the focused element is not null, remove the focus
            if (focusedOn){
                updateHoveredElement(focusedOn); // Update the hovered element to be the focused element before un-focusing it.
                unFocusElement();
            }
        }

        /**
         * @brief Handles the pressing of the tab key.
         * @details This function selects the next tabbed element as focused and not hovered.
         *          If the shift key is pressed, it goes backwards in the list of tabbed elements.
         */
        void handleTabulator(){
            // Check if the tab key has been pressed
            if (!KEYBOARD_STATES[KEYBOARD_BUTTONS::TAB].state)
                return;

            if (focusedOn) return;   // Tabulator is disabled from switching if an element is focused on, this gives us the ability to insert tabs into textFields.
            
            // Check if the shift key is pressed
            bool Shift_Is_Pressed = KEYBOARD_STATES[KEYBOARD_BUTTONS::SHIFT].state;

            // Get the current element from the selected element
            element* Current = hoveredOn;
            
            int Current_Index = 0;

            // Find the index of the current element in the list of event handlers
            if (Current){
                // Find the first occurrence of the event handlers with this Current being their Host.
                for (;(unsigned int)Current_Index < eventHandlers.size(); Current_Index++){
                    if (eventHandlers[Current_Index]->host == Current)
                        break;
                }

                // Now, we need to find the last occurrence of this Current in the event handlers
                for (int i = Current_Index + 1; i < (int)eventHandlers.size(); i++){
                    if (eventHandlers[i]->host == Current){
                        Current_Index = i; // Update the index to the last occurrence
                    }
                }
            }

            // Generalize index hopping, if shift is pressed then go backwards.
            Current_Index += 1 + (-2 * Shift_Is_Pressed);

            // If the index is out of bounds, wrap it around to the other side of the list
            if (Current_Index < 0){
                Current_Index = eventHandlers.size() - 1;
            }
            else if ((size_t)Current_Index >= eventHandlers.size() - 1){
                Current_Index = 0;
            }

            // Now update the hovered element with the new index
            updateHoveredElement(eventHandlers[Current_Index]->host);
        }

        /**
         * @brief Gets the current maximum width of the terminal.
         * @details This function returns the current maximum width of the terminal. If the width is 0, it will set the carry flag to indicate that a resize is needed to be performed.
         *
         * @return The current maximum width of the terminal.
         */
        int getMaxWidth(){
            if (maxWidth == 0 && maxHeight == 0){
                Carry_Flags([](carry& current_carry){
                    current_carry.resize = true;    // Tell the render thread that an resize is needed to be performed.
                });
            }
            
            return maxWidth;
        }

        /**
         * @brief Gets the current maximum height of the terminal.
         * @details This function returns the current maximum height of the terminal. If the height is 0, it will set the carry flag to indicate that a resize is needed to be performed.
         *
         * @return The current maximum height of the terminal.
         */
        int getMaxHeight(){
            if (maxWidth == 0 && maxHeight == 0){
                Carry_Flags([](carry& current_carry){
                    current_carry.resize = true;    // Tell the render thread that an resize is needed to be performed.
                });
            }

            return maxHeight;
        }

        static fastVector<compactString> LIQUIFY_UTF_TEXT_RESULT_CACHE(1000*1000);
        static superString<GGUI::constants::ANSI::maximumNeededPreAllocationForEncodedSuperString> LIQUIFY_UTF_TEXT_TMP_CONTAINER;
        static superString<GGUI::constants::ANSI::maximumNeededPreAllocationForOverHead> LIQUIFY_UTF_TEXT_TEXT_OVERHEAD;
        static superString<GGUI::constants::ANSI::maximumNeededPreAllocationForOverHead> LIQUIFY_UTF_TEXT_BACKGROUND_OVERHEAD;
        static superString<GGUI::constants::ANSI::maximumNeededPreAllocationForColor> LIQUIFY_UTF_TEXT_TEXT_COLOUR;
        static superString<GGUI::constants::ANSI::maximumNeededPreAllocationForColor> LIQUIFY_UTF_TEXT_BACKGROUND_COLOUR;

        /**
         * @brief Converts a vector of UTFs into a Super_String.
         * @details This function takes a vector of UTFs, and converts it into a Super_String. The resulting Super_String is stored in a cache, and the cache is resized if the window size has changed.
         * @param Text The vector of UTFs to convert.
         * @param Width The width of the window.
         * @param Height The height of the window.
         * @return A pointer to the resulting Super_String.
         */
        fastVector<compactString> liquifyUTFText(const std::vector<GGUI::UTF>* Text, unsigned int& Liquefied_Size, int Width, int Height){
            const unsigned int Maximum_Needed_Pre_Allocation_For_Whole_Cache_Buffer = (Width * Height * constants::ANSI::maximumNeededPreAllocationForEncodedSuperString + !SETTINGS::wordWrapping * (Height - 1));

            // Since they are located as globals we need to remember to restart the starting offset.
            Liquefied_Size = 0;

            // Ensure previous frame contents are not read again
            LIQUIFY_UTF_TEXT_RESULT_CACHE.clear();
            LIQUIFY_UTF_TEXT_TMP_CONTAINER.clear();
            LIQUIFY_UTF_TEXT_TEXT_COLOUR.clear();
            LIQUIFY_UTF_TEXT_BACKGROUND_COLOUR.clear();
            
            // We need to dynamically resize this, since the window size will be potentially re-sized.
            LIQUIFY_UTF_TEXT_RESULT_CACHE.resize(Maximum_Needed_Pre_Allocation_For_Whole_Cache_Buffer);
    
            for (int y = 0; y < Height; y++){
                for (int x = 0; x < Width; x++){
                    LIQUIFY_UTF_TEXT_TMP_CONTAINER = LIQUIFY_UTF_TEXT_RESULT_CACHE.getWindow<GGUI::constants::ANSI::maximumNeededPreAllocationForEncodedSuperString>();

                    Text->at(y * Width + x).toEncodedSuperString(
                        &LIQUIFY_UTF_TEXT_TMP_CONTAINER,
                        &LIQUIFY_UTF_TEXT_TEXT_COLOUR,
                        &LIQUIFY_UTF_TEXT_BACKGROUND_COLOUR
                    );

                    // Tell the fastVector the actual used size of the window.
                    LIQUIFY_UTF_TEXT_RESULT_CACHE.releaseWindow(LIQUIFY_UTF_TEXT_TMP_CONTAINER.currentIndex);

                    Liquefied_Size += LIQUIFY_UTF_TEXT_TMP_CONTAINER.liquefiedSize;

                    // now instead of emptying the Super_String.vector, we can reset the current index into 0 again.
                    LIQUIFY_UTF_TEXT_TMP_CONTAINER.clear();
                    LIQUIFY_UTF_TEXT_TEXT_COLOUR.clear();
                    LIQUIFY_UTF_TEXT_BACKGROUND_COLOUR.clear();
                }

                // the system doesn't have word wrapping enabled then, use newlines as replacement.
                if (!SETTINGS::wordWrapping){
                    LIQUIFY_UTF_TEXT_RESULT_CACHE.append(compactString('\n')); // the system is word wrapped.
                    Liquefied_Size += 1;
                }
            }

            return LIQUIFY_UTF_TEXT_RESULT_CACHE;
        }

        void waitForThreadTermination(){
            LOGGER::log("Sending termination signals to subthreads...");

            std::unique_lock lock(atomic::mutex);
            
            // Gracefully shutdown event and rendering threads.
            Carry_Flags([](carry& self){
                self.terminate = true;
            });
            
            // Give the rendering thread one ticket.
            atomic::pauseRenderThread = atomic::status::REQUESTING_RENDERING;

            // Notify all waiting threads that the frame has been updated.
            atomic::condition.notify_all();

            // await until the rendering thread has used it's rendering ticket.
            atomic::condition.wait(lock, []{
                return atomic::pauseRenderThread == atomic::status::TERMINATED;
            });
        }

        /**
         * @brief This function is a helper for the smart memory system to recall which tasks should be prolonged, and which should be deleted.
         * @details This function is a lambda function that is used by the Atomic::Guard class to prolong or delete memories in the smart memory system.
         *          It takes a pointer to a vector of Memory objects and prolongs or deletes the memories in the vector based on the time difference between the current time and the memory's start time.
         */
        void recallMemories(){
            INTERNAL::remember([](std::vector<memory>& rememberable){
                std::chrono::high_resolution_clock::time_point currentTime = std::chrono::high_resolution_clock::now();

                // For smart memory system to shorten the next sleep time to arrive at the perfect time for the nearest memory.
                size_t Shortest_Time = INTERNAL::MAX_UPDATE_SPEED;
                // Prolong prolongable memories.
                for (unsigned int i = 0; i < rememberable.size(); i++){
                    for (unsigned int j = i + 1; j < rememberable.size(); j++){
                        if (rememberable.at(i).is(MEMORY_FLAGS::PROLONG_MEMORY) && rememberable.at(j).is(MEMORY_FLAGS::PROLONG_MEMORY) && i != j)
                            // Check if the Job at I is same as the one at J.
                            if (rememberable.at(i).Job.target<bool(*)(GGUI::event*)>() == rememberable.at(j).Job.target<bool(*)(GGUI::event*)>()){
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
                            bool Success = rememberable.at(i).Job((event*)&rememberable.at(i));

                            // If job is a re-trigger it will ignore whether the job was successful or not.
                            if (rememberable.at(i).is(MEMORY_FLAGS::RETRIGGER)){

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
            // Flag to determine if the current element is an event handler
            bool Is_An_Event_handler = false;

            // Check if the current element is an event handler
            for (auto i : INTERNAL::eventHandlers){
                if (i->host == current){
                    Is_An_Event_handler = true;
                    break;
                }
            } 

            // If the element is an event handler and the focus state is unchanged, return
            if (Is_An_Event_handler && current->isFocused() != Focus)
                return;

            // Set the focus state on the current element
            current->setFocus(Focus);

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
            // check if the current element is one of the Event handlers, if not, then apply the focus buff.
            bool Is_An_Event_handler = false;

            for (auto i : INTERNAL::eventHandlers){
                if (i->host == current){
                    Is_An_Event_handler = true;
                    break;
                }
            }

            if (Is_An_Event_handler && current->isHovered() != Hover)
                return;

            current->setHoverState(Hover);

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
            mouse = INTERNAL::focusedOn->getAbsolutePosition();

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
            mouse = INTERNAL::hoveredOn->getAbsolutePosition();

            // Set the hover state on the new element to true
            INTERNAL::hoveredOn->setHoverState(true);

            // Recursively set the hover state on all child elements to true
            Recursively_Apply_Hover(INTERNAL::hoveredOn, true);
        }

        /**
         * @brief Handles all events in the system.
         * @details This function goes through all event handlers and checks if the event criteria matches any of the inputs.
         *          If a match is found, it calls the event handler job with the input as an argument.
         *          If the job is successful, it removes the input from the list of inputs.
         *          If the job is unsuccessful, it reports an error.
         */
        void eventHandler(){
            // For an input to belong to the current event handler, it needs to:
            // - Have identical criteria
            // - If mouse or enter is as the input:
            //      - If host present and is onHovered: Un-hover the host element and and set it to onFocused
            //      - If host present and is onFocused: Pipe input into the event handler job task

            // Since some key events are piped to us at a different speed than others, we need to keep the older (un-used) inputs "alive" until their turn arrives.
            Populate_Inputs_For_Held_Down_Keys();

            for (unsigned int i = 0; i < INTERNAL::eventHandlers.size(); i++){
                action* currentEventHandler = INTERNAL::eventHandlers[i];

                // The reason these are held over multitude of inputs, is for scenario where this memory thread has not run in a long time and has a long query of inputs-
                // and in this same listing of inputs at the start is the mouse click or enter and the user given inputs for that specifically activated event handler.
                bool Has_Mouse_Left_Click_Event = false;
                bool Has_Enter_Press_Event = false;
                
                // If the current event handler has an host element present
                if (currentEventHandler->host){
                    if (!currentEventHandler->host->isDisplayed())
                        continue;   // Skip eventhandlers where their host is not active

                    bool overlapsWithMouse = INTERNAL::collides(currentEventHandler->host, INTERNAL::mouse);

                    // First let's go through all inputs and see if any selector inputs are present.
                    for (size_t j = 0; j < INTERNAL::inputs.size();){
                        input* currentInput = INTERNAL::inputs[j];      
    
                        Has_Mouse_Left_Click_Event = has(currentInput->criteria, constants::MOUSE_LEFT_CLICKED) && overlapsWithMouse;
                        Has_Enter_Press_Event = has(currentInput->criteria, constants::ENTER) && INTERNAL::KEYBOARD_STATES[KEYBOARD_BUTTONS::ENTER].state == true;
    
                        // Check if the host is prime to be focused on
                        if ((Has_Mouse_Left_Click_Event || Has_Enter_Press_Event) && currentEventHandler->host->isHovered()){
                            updateFocusedElement(currentEventHandler->host);
                            unHoverElement();

                            // Remove the input, since it's job is used here:
                            INTERNAL::inputs.erase(INTERNAL::inputs.begin() + j);
                            continue;
                        }

                        // Criteria must be identical for more accurate criteria listing.
                        if (currentEventHandler->criteria == currentInput->criteria && currentEventHandler->host->isFocused()){
                            try{
                                // Check if this job could be run successfully.
                                if (currentEventHandler->Job(currentInput)){
                                    //dont let anyone else react to this event.
                                    INTERNAL::inputs.erase(INTERNAL::inputs.begin() + j);
                                    continue;
                                }
                                else{
                                    // TODO: report miscarried event job.
                                    INTERNAL::reportStack("Job '" + currentEventHandler->ID + "' failed!");
                                }
                            }
                            catch(std::exception& problem){
                                INTERNAL::reportStack("In event: '" + currentEventHandler->ID + "' Problem: " + std::string(problem.what()));
                            }
                        }

                        j++;
                    }

                    // If the current event handler is not focused, then we can check wether to set it on/off onHovering
                    if (!currentEventHandler->host->isFocused()) {
                        if (overlapsWithMouse){
                            updateHoveredElement(currentEventHandler->host);
                        }
                        else {
                            if (INTERNAL::hoveredOn == currentEventHandler->host)
                                unHoverElement();
                        }
                    }
                }

                // if (INTERNAL::Inputs.size() <= 1)
                //     continue;

                // // TODO: Do better you dum!
                // // GO through the inputs and check if they contain all the flags required
                // unsigned long long Remaining_Flags = currentEventHandler->criteria;
                // std::vector<GGUI::input *> Accepted_Inputs;

                // // if an input has flags that meet the criteria, then remove the criteria from the remaining flags and continue until the remaining flags are equal to zero.
                // for (auto* j : INTERNAL::Inputs){

                //     if (Contains(Remaining_Flags, j->criteria)){
                //         Remaining_Flags &= ~j->criteria;
                //         Accepted_Inputs.push_back(j);
                //     }

                //     if (Remaining_Flags == 0)
                //         break;
                // }

                // if (Remaining_Flags == 0){
                //     // Now we need to find the information to send to the event handler.
                //     input* Best_Candidate = Accepted_Inputs[0];

                //     for (auto* j : Accepted_Inputs){
                //         if (j->data > Best_Candidate->data){
                //             Best_Candidate = j;
                //         }
                //     }

                //     //check if this job could be run successfully.
                //     if (currentEventHandler->Job(Best_Candidate)){
                //         // Now remove the candidates from the input
                //         for (unsigned int j = 0; j < INTERNAL::Inputs.size(); j++){
                //             if (INTERNAL::Inputs[j] == Best_Candidate){
                //                 INTERNAL::Inputs.erase(INTERNAL::Inputs.begin() + j);
                //                 j--;
                //             }
                //         }
                //     }
                // }

            }
            
            // If no event handler recognized these inputs, there is no need to keep them lingering for next time.
            INTERNAL::inputs.clear();
        }

        /**
         * Get the ID of a class by name, assigning a new ID if it doesn't exist.
         * 
         * @param n The name of the class.
         * @return The ID of the class.
         */
        int getFreeClassID(std::string n){
            // Check if the class name is already in the map
            if (INTERNAL::classNames.find(n) != INTERNAL::classNames.end()){
                // Return the existing class ID
                return INTERNAL::classNames[n];
            }
            else{
                // Assign a new class ID as the current size of the map
                INTERNAL::classNames[n] = INTERNAL::classNames.size();

                // Return the newly assigned class ID
                return INTERNAL::classNames[n];
            }
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

            INTERNAL::updateMaxWidthAndHeight();
            
            if (!SETTINGS::enableDRM){
                if (INTERNAL::maxHeight == 0 || INTERNAL::maxWidth == 0){
                    INTERNAL::LOGGER::log("Width/Height is zero!");
                    return nullptr;
                }
            }

            // Save the state before the init
            INTERNAL::Current_Time = std::chrono::high_resolution_clock::now();
            INTERNAL::Previous_Time = INTERNAL::Current_Time;

            INTERNAL::initPlatformStuff();

            INTERNAL::main = new element(
                width(INTERNAL::maxWidth) |
                height(INTERNAL::maxHeight) | 
                name("Main")
            , true);

            INTERNAL::Sub_Threads.emplace_back([](){
                INTERNAL::LOGGER::registerCurrentThread();
                INTERNAL::renderer();
            });
            
            INTERNAL::Sub_Threads.emplace_back([](){
                INTERNAL::LOGGER::registerCurrentThread();
                INTERNAL::eventThread();
            });
            
            // Start input thread only if DRM is enabled or STDIN is a TTY (interactive).
            std::unique_ptr<std::thread> Inquire_Scheduler_ptr;
            if (SETTINGS::enableDRM || STDIN_IS_TTY){
                Inquire_Scheduler_ptr = std::make_unique<std::thread>([](){
                    INTERNAL::LOGGER::registerCurrentThread();
                    INTERNAL::inputThread();
                });
            }

            std::thread Logging_Scheduler([](){
                INTERNAL::LOGGER::registerCurrentThread();
                INTERNAL::loggerThread();
            });
            
            // INTERNAL::Sub_Threads.push_back(std::move(Inquire_Scheduler));
            // INTERNAL::Sub_Threads.back().detach();    // the Inquire scheduler cannot never stop and thus needs to be as an separate thread.
            
            if (Inquire_Scheduler_ptr) Inquire_Scheduler_ptr->detach();
            Logging_Scheduler.detach();

            // INTERNAL::Sub_Threads.push_back(std::move(Logging_Scheduler));
            // INTERNAL::Sub_Threads.back().detach();    // the Logging scheduler cannot never stop and thus needs to be as an separate thread.
            
            INTERNAL::LOGGER::log("GGUI Core initialization complete.");

            {
                std::unique_lock lock(INTERNAL::atomic::mutex);

                // Remove NOT_INITALIZED from the render thread flag.
                INTERNAL::atomic::pauseRenderThread = INTERNAL::atomic::status::PAUSED;
            }

            return INTERNAL::main;
        }

        /**
         * @brief Nests a text buffer into a parent buffer while considering the childs position and size.
         * 
         * @param Parent The parent element which the text is being nested into.
         * @param child The child element which's text is being nested.
         * @param Text The text buffer to be nested.
         * @param Parent_Buffer The parent buffer which the text is being nested into.
         */
        void nestUTFText(GGUI::element* Parent, GGUI::element* child, std::vector<GGUI::UTF> Text, std::vector<GGUI::UTF>& Parent_Buffer)
        {
            if (Parent == child)
            {
                std::string R = 
                    std::string("Cannot nest element to it self\n") +
                    std::string("Element name: ") + Parent->getName();

                if (Parent->getParent())
                {
                    R += std::string("\n") + 
                    std::string("Inside of: ") + Parent->getParent()->getName();
                }

                INTERNAL::reportStack(
                    R
                );
            }

            // Get the position of the child element in the parent buffer.
            GGUI::IVector3 C = child->getPosition();

            int i = 0;
            // Iterate over the parent buffer and copy the text buffer into the parent buffer at the correct position.
            for (int Parent_Y = 0; Parent_Y < (signed)Parent->getHeight(); Parent_Y++)
            {
                for (int Parent_X = 0; Parent_X < (signed)Parent->getWidth(); Parent_X++)
                {
                    if (
                        Parent_Y >= C.y && Parent_X >= C.x &&
                        Parent_Y <= C.y + (signed)child->getHeight() &&
                        Parent_X <= C.x + (signed)child->getWidth()
                    )
                    {
                        Parent_Buffer[Parent_Y * (signed)Parent->getWidth() + Parent_X] = Text[i++];
                    }
                }
            }
        }

        /**
         * @brief Encodes a buffer of UTF elements by setting start and end flags based on color changes.
         * 
         * @param Buffer A vector of UTF elements to be encoded.
         * @details The function marks the beginning and end of color strips within the buffer. 
         *          It checks each UTF element's foreground and background colors with its adjacent elements
         *          to determine where encoding strips start and end.
         */
        void encodeBuffer(std::vector<GGUI::UTF>* Buffer) {
            const size_t Count = Buffer->size();
            if (Count == 0) return;

            // Set START flag for the first element
            Buffer->front().setFlag(ENCODING_FLAG::START);

            // If only one element, also mark as END
            if (Count == 1) {
                Buffer->front().setFlag(ENCODING_FLAG::END);
                return;
            }

            // Calculate the relative size difference between the non-encoded and the encoded buffers.
            INTERNAL::BEFORE_ENCODE_BUFFER_SIZE = Buffer->size() *  constants::ANSI::maximumNeededPreAllocationForEncodedSuperString;
            INTERNAL::AFTER_ENCODE_BUFFER_SIZE = 0;

            // Cache previous colors (start with the very first element)
            auto PrevFg = Buffer->front().foreground;
            auto PrevBg = Buffer->front().background;

            unsigned int StartOffset = 1;

            // Align pointers so Curr points to the current index i, and Next to i+1
            auto* Curr = Buffer->data() + StartOffset;      // i = 1
            auto* Next = Curr + 1;                          // i + 1

            // Process interior elements [StartOffset, Count-2]
            for (size_t i = StartOffset; i < Count - 1; i++) {
                bool SameAsPrev = (Curr->foreground == PrevFg) && (Curr->background == PrevBg);
                bool SameAsNext = (Curr->foreground == Next->foreground) && (Curr->background == Next->background);

                if (!SameAsPrev) {
                    Curr->setFlag(ENCODING_FLAG::START);
                    // for logging:
                    INTERNAL::AFTER_ENCODE_BUFFER_SIZE += constants::ANSI::maximumNeededPreAllocationForOverhead;
                }

                if (!SameAsNext) {
                    Curr->setFlag(ENCODING_FLAG::END);
                    // for logging:
                    INTERNAL::AFTER_ENCODE_BUFFER_SIZE += constants::ANSI::maximumNeededPreAllocationForReset;
                }

                PrevFg = Curr->foreground;
                PrevBg = Curr->background;

                // for logging:
                INTERNAL::AFTER_ENCODE_BUFFER_SIZE++;

                Curr++;
                Next++;
            }

            // Handle the last element
            auto& Last = Buffer->back();
            Last.setFlag(ENCODING_FLAG::END);
            // for logging:
            INTERNAL::AFTER_ENCODE_BUFFER_SIZE++;

            // Compare last with second-last for possible START flag
            const auto& SecondLast = Buffer->at(Count - 2);
            if (!(Last.foreground == SecondLast.foreground) || !(Last.background == SecondLast.background)) {
                Last.setFlag(ENCODING_FLAG::START);
                // for logging:
                INTERNAL::AFTER_ENCODE_BUFFER_SIZE += constants::ANSI::maximumNeededPreAllocationForOverhead;
            }
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
                GGUI::INTERNAL::Max(Child->getPosition().x, 0),
                GGUI::INTERNAL::Max(Child->getPosition().y, 0)
            } + parentStart;

            // Drawable box end, within the bounding box.
            IVector2 childEnd = {
                GGUI::INTERNAL::Min(childStart.x + Child->getWidth() - negativeOffset.x, parentEnd.x),
                GGUI::INTERNAL::Min(childStart.y + Child->getHeight() - negativeOffset.y, parentEnd.y)
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
        void computeAlphaToNesting(GGUI::UTF& Dest, const GGUI::UTF& Source, float childOpacity){
            // If the Source element has full opacity, then the destination gets fully rewritten over.
            if (childOpacity == 1.0f){
                Dest = Source;
                return;
            }
            else if (childOpacity == 0.0f) return;         // Dont need to do anything.

            // Color the Destination UTF by the Source UTF background color.
            Dest.background.add(Source.background, childOpacity);
            Dest.foreground.add(Source.background, childOpacity);

            // Check if source has text
            if (!Source.hasDefaultText()){
                Dest.setText(Source);
                Dest.foreground.add(Source.foreground, childOpacity); 
            }
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
        void nestElement(GGUI::element* parent, GGUI::element* child, std::vector<GGUI::UTF>& Parent_Buffer, std::vector<GGUI::UTF>& Child_Buffer){
            INTERNAL::fittingArea Limits = getFittingArea(parent, child);

            for (int y = Limits.start.y; y < Limits.end.y; y++){
                for (int x = Limits.start.x; x < Limits.end.x; x++){
                    // Calculate the position of the child element in its own buffer.
                    int Child_Buffer_Y = (y - Limits.start.y + Limits.negativeOffset.y) * child->getWidth();
                    int Child_Buffer_X = (x - Limits.start.x + Limits.negativeOffset.x); 
                    computeAlphaToNesting(Parent_Buffer[y * parent->getWidth() + x], Child_Buffer[Child_Buffer_Y + Child_Buffer_X], child->getOpacity());
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
        INTERNAL::Cleanup();

        // Exit the application with the specified exit code
        exit(signum);
    }

    element* getRoot() {
        return INTERNAL::main;
    }

    /**
     * @brief Updates the frame.
     * @details This function updates the frame. It's the main entry point for the rendering thread.
     * @note This function will return immediately if the rendering thread is paused.
     */
    void updateFrame(){
        std::unique_lock lock(INTERNAL::atomic::mutex);

        // The rendering thread is either locked, already rendering or already requested to render.
        if (INTERNAL::atomic::LOCKED > 0 || INTERNAL::atomic::pauseRenderThread == INTERNAL::atomic::status::NOT_INITIALIZED)
            return;

        // Give the rendering thread one ticket.
        INTERNAL::atomic::pauseRenderThread = INTERNAL::atomic::status::REQUESTING_RENDERING;

        // Notify all waiting threads that the frame has been updated.
        INTERNAL::atomic::condition.notify_all();
    }

    /**
     * @brief Pauses the rendering thread.
     * @details This function pauses the rendering thread. The thread will wait until the rendering thread is resumed.
     */
    void pauseGGUI(){
        std::unique_lock lock(INTERNAL::atomic::mutex);
        
        // Already paused via upper scope or if the rendering system hasn't been initialized yet, just no-op.
        if (INTERNAL::atomic::LOCKED++ > 0 || INTERNAL::atomic::pauseRenderThread == INTERNAL::atomic::status::NOT_INITIALIZED)
            return;

        // await until the rendering thread has used it's rendering ticket.
        INTERNAL::atomic::condition.wait(lock, []{
            return INTERNAL::atomic::pauseRenderThread == INTERNAL::atomic::status::PAUSED;
        });
    }

    /**
     * @brief Resumes the rendering thread.
     * @details This function resumes the rendering thread after it has been paused.
     * @param restore_render_to The status to restore the rendering thread to.
     */
    void resumeGGUI(){
        // If not initialized, simply reset LOCKED counter (if needed) and no-op.
        if (INTERNAL::atomic::pauseRenderThread == INTERNAL::atomic::status::NOT_INITIALIZED){
            INTERNAL::atomic::LOCKED = 0; // Safety: ensure clean state for unit tests.
            return;
        }

        {
            std::unique_lock lock(INTERNAL::atomic::mutex);

            if (--INTERNAL::atomic::LOCKED > 0)
                return;

            INTERNAL::atomic::condition.wait(lock, []{
                // If the rendering thread is not locked, then the wait is over.
                return INTERNAL::atomic::pauseRenderThread == INTERNAL::atomic::status::PAUSED;
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
        if (INTERNAL::atomic::pauseRenderThread == INTERNAL::atomic::status::NOT_INITIALIZED){
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
