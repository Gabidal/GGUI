#include "renderer.h"
#include "../elements/fileStreamer.h"
#include "settings.h"
#include "./utils/logger.h"
#include "./utils/utils.h"
#include "./thread.h"
#include "./addons/addons.h"

#include <string>
#include <cassert>
#include <math.h>
#include <sstream>
#include <cstdio>
#include <exception>
#include <csignal>

namespace GGUI{
    namespace INTERNAL{
        std::vector<GGUI::UTF> SAFE_MIRROR;                               // Only used for references to be initalized to point at.
        std::vector<UTF>& Abstract_Frame_Buffer = SAFE_MIRROR;      // 2D clean vector without bold nor color
        std::string Frame_Buffer;                                   // string with bold and color, this what gets drawn to console.

        // For threading system
        namespace atomic{
            std::mutex Mutex;
            std::condition_variable Condition;

            status Pause_Render_Thread = status::NOT_INITIALIZED;
        }

        std::vector<std::thread> Sub_Threads;

        std::vector<INTERNAL::bufferCapture*> Global_Buffer_Captures;

        unsigned int Max_Width = 0;
        unsigned int Max_Height = 0;

        atomic::Guard<std::vector<Memory>> Remember;

        std::vector<Action*> Event_Handlers;
        std::vector<Input*> Inputs;
        std::chrono::system_clock::time_point Last_Input_Clear_Time;

        std::unordered_map<std::string, element*> Element_Names;

        element* Focused_On = nullptr;
        element* Hovered_On = nullptr;

        bool Platform_Initialized = false;

        IVector3 Mouse;
        //move 1 by 1, or element by element.
        bool Mouse_Movement_Enabled = true;

        std::unordered_map<std::string, buttonState> KEYBOARD_STATES;
        std::unordered_map<std::string, buttonState> PREVIOUS_KEYBOARD_STATES;

        // Represents the update speed of each elapsed loop of passive events, which do NOT need user as an input.
        inline time_t MAX_UPDATE_SPEED = TIME::SECOND;
        inline time_t MIN_UPDATE_SPEED = TIME::MILLISECOND * 16;    // Close approximation to 60 fps.
        inline time_t CURRENT_UPDATE_SPEED = MAX_UPDATE_SPEED;
        inline float Event_Thread_Load = 0.0f;  // Describes the load of animation and events from 0.0 to 1.0. Will reduce the event thread pause.

        std::chrono::high_resolution_clock::time_point Previous_Time;
        std::chrono::high_resolution_clock::time_point Current_Time;

        unsigned long long Render_Delay;    // describes how long previous render cycle took in ms
        unsigned long long Event_Delay;    // describes how long previous memory tasks took in ms
        unsigned long long Input_Delay;     // describes how long previous input tasks took in ms

        inline atomic::Guard<std::unordered_map<int, styling>> Classes;

        inline std::unordered_map<std::string, int> Class_Names;

        std::unordered_map<GGUI::terminalCanvas*, bool> Multi_Frame_Canvas;

        void* Stack_Start_Address = 0;
        void* Heap_Start_Address = 0;

        window* Main = nullptr;

        atomic::Guard<Carry> Carry_Flags; 

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
    }

    #if _WIN32

    namespace Constants{
        namespace ANSI{
            inline int ENABLE_UTF8_MODE_FOR_WINDOWS = 65001;
        }
    }
    
    namespace INTERNAL{
        #include <windows.h>
        #include <DbgHelp.h>

        /**
         * @brief Sleep for the specified amount of milliseconds.
         * 
         * This function pauses the execution of the current thread for the specified
         * duration in milliseconds.
         * 
         * @param mm The number of milliseconds to sleep.
         */
        void SLEEP(unsigned int mm){
            // Sleep for the specified amount of milliseconds.
            Sleep(mm);
        }

        GGUI::INTERNAL::HANDLE GLOBAL_STD_OUTPUT_HANDLE;
        GGUI::INTERNAL::HANDLE GLOBAL_STD_INPUT_HANDLE;

        DWORD PREVIOUS_CONSOLE_OUTPUT_STATE;
        DWORD PREVIOUS_CONSOLE_INPUT_STATE;

        CONSOLE_SCREEN_BUFFER_INFO Get_Console_Info();

        // This is here out from the Query_Inputs, so that we can differentiate querying and translation of said input.
        const unsigned int Raw_Input_Capacity = UINT8_MAX * 10;
        INPUT_RECORD Raw_Input[Raw_Input_Capacity];
        int Raw_Input_Size = 0;

        
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
        void De_Initialize(){
            // Clean up file stream handles
            for (auto File_Handle : File_Streamer_Handles){
                File_Handle.second->~fileStream(); // Manually call destructor to release resources
            }

            File_Streamer_Handles.clear();

            // Restore previous console modes
            SetConsoleMode(GLOBAL_STD_OUTPUT_HANDLE, PREVIOUS_CONSOLE_OUTPUT_STATE);
            SetConsoleMode(GLOBAL_STD_INPUT_HANDLE, PREVIOUS_CONSOLE_INPUT_STATE);

            // Disable specific ANSI features and restore screen
            std::cout << GGUI::Constants::ANSI::Enable_Private_SGR_Feature(GGUI::Constants::ANSI::MOUSE_CURSOR).To_String();
            std::cout << GGUI::Constants::ANSI::Enable_Private_SGR_Feature(GGUI::Constants::ANSI::REPORT_MOUSE_ALL_EVENTS, false).To_String();
            std::cout << GGUI::Constants::ANSI::Enable_Private_SGR_Feature(GGUI::Constants::ANSI::SCREEN_CAPTURE, false).To_String();
            std::cout << std::flush; // Ensure all output is flushed to console
        }

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
        void EXIT(int signum){
            if (!Carry_Flags.Read().Terminate){
                LOGGER::Log("Sending termination signals to subthreads...");

                // Gracefully shutdown event and rendering threads.
                Carry_Flags([](Carry& self){
                    self.Terminate = true;
                });

                updateFrame();

                // Join the threads
                for (auto& thread : INTERNAL::Sub_Threads){
                    if (thread.joinable()){
                        thread.join();
                    }
                }

                LOGGER::Log("Reverting to normal console mode...");

                // Clean up platform-specific resources and settings
                De_Initialize();

                LOGGER::Log("GGUI shutdown successful.");
            }

            // Exit the application with the specified exit code
            exit(signum);
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
                result += "Context (Rip): " + std::to_string(reinterpret_cast<uintptr_t>(exceptionInfo->ContextRecord->Rip)) + "\n";
            #elif defined(_M_IX86) || defined(__i386__)
                // For x86 systems, use Eip
                result += "Context (Eip): " + std::to_string(reinterpret_cast<uintptr_t>(exceptionInfo->ContextRecord->Eip)) + "\n";
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
            LOGGER::Log("Access violation or critical error occurred.");
            LOGGER::Log("Exception Code: " + std::to_string(exceptionInfo->ExceptionRecord->ExceptionCode));
            LOGGER::Log("Exception Address: " + std::to_string(reinterpret_cast<uintptr_t>(exceptionInfo->ExceptionRecord->ExceptionAddress)));
            LOGGER::Log(Exception_To_String(exceptionInfo));    // Dump
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
                LOGGER::Log("Terminated via " + signal_name + " signal.");
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
            WriteFile(GLOBAL_STD_OUTPUT_HANDLE, INTERNAL::Frame_Buffer.data(), INTERNAL::Frame_Buffer.size(), reinterpret_cast<LPDWORD>(&tmp), NULL);
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
            INTERNAL::Max_Width = info.srWindow.Right - info.srWindow.Left + 1;
            INTERNAL::Max_Height = info.srWindow.Bottom - info.srWindow.Top + 1;

            // Check if we got the console information correctly.
            if (INTERNAL::Max_Width == 0 || INTERNAL::Max_Height == 0){
                INTERNAL::reportStack("Failed to get console info!");
            }

            // Check that the main window is not active and if so, set its dimensions.
            if (INTERNAL::Main)
                INTERNAL::Main->setDimensions(INTERNAL::Max_Width, INTERNAL::Max_Height);
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

            if (INTERNAL::KEYBOARD_STATES[BUTTON_STATES::CONTROL].State && INTERNAL::KEYBOARD_STATES[BUTTON_STATES::SHIFT].State){
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
            // For appending to already existing buffered input which has not yet been processed, we can use the previous Raw_Input_Size to deduce the new starting point.
            INPUT_RECORD* Current_Starting_Address = Raw_Input + Raw_Input_Size;

            // Ceil the value so that negative numbers wont create overflows.
            unsigned int Current_Usable_Capacity = Max(Raw_Input_Capacity - Raw_Input_Size, Raw_Input_Capacity);

            // Read the console input and store it in Raw_Input.
            ReadConsoleInput(
                GLOBAL_STD_INPUT_HANDLE,
                Current_Starting_Address,
                Current_Usable_Capacity,
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
                        INTERNAL::Inputs.push_back(new GGUI::Input(0, GGUI::Constants::UP));
                        INTERNAL::KEYBOARD_STATES[BUTTON_STATES::UP] = INTERNAL::buttonState(Pressed);
                    }
                    else if (Raw_Input[i].Event.KeyEvent.wVirtualKeyCode == VK_DOWN){
                        INTERNAL::Inputs.push_back(new GGUI::Input(0, GGUI::Constants::DOWN));
                        INTERNAL::KEYBOARD_STATES[BUTTON_STATES::DOWN] = INTERNAL::buttonState(Pressed);
                    }
                    else if (Raw_Input[i].Event.KeyEvent.wVirtualKeyCode == VK_LEFT){
                        INTERNAL::Inputs.push_back(new GGUI::Input(0, GGUI::Constants::LEFT));
                        INTERNAL::KEYBOARD_STATES[BUTTON_STATES::LEFT] = INTERNAL::buttonState(Pressed);
                    }
                    else if (Raw_Input[i].Event.KeyEvent.wVirtualKeyCode == VK_RIGHT){
                        INTERNAL::Inputs.push_back(new GGUI::Input(0, GGUI::Constants::RIGHT));
                        INTERNAL::KEYBOARD_STATES[BUTTON_STATES::RIGHT] = INTERNAL::buttonState(Pressed);
                    }
                    else if (Raw_Input[i].Event.KeyEvent.wVirtualKeyCode == VK_RETURN){
                        INTERNAL::Inputs.push_back(new GGUI::Input('\n', GGUI::Constants::ENTER));
                        INTERNAL::KEYBOARD_STATES[BUTTON_STATES::ENTER] = INTERNAL::buttonState(Pressed);
                    }
                    else if (Raw_Input[i].Event.KeyEvent.wVirtualKeyCode == VK_SHIFT){
                        INTERNAL::Inputs.push_back(new GGUI::Input(' ', GGUI::Constants::SHIFT));
                        INTERNAL::KEYBOARD_STATES[BUTTON_STATES::SHIFT] = INTERNAL::buttonState(Pressed);
                    }
                    else if (Raw_Input[i].Event.KeyEvent.wVirtualKeyCode == VK_CONTROL){
                        INTERNAL::Inputs.push_back(new GGUI::Input(' ', GGUI::Constants::CONTROL));
                        INTERNAL::KEYBOARD_STATES[BUTTON_STATES::CONTROL] = INTERNAL::buttonState(Pressed);
                    }
                    else if (Raw_Input[i].Event.KeyEvent.wVirtualKeyCode == VK_BACK){
                        INTERNAL::Inputs.push_back(new GGUI::Input(' ', GGUI::Constants::BACKSPACE));
                        INTERNAL::KEYBOARD_STATES[BUTTON_STATES::BACKSPACE] = INTERNAL::buttonState(Pressed);
                    }
                    else if (Raw_Input[i].Event.KeyEvent.wVirtualKeyCode == VK_ESCAPE){
                        INTERNAL::Inputs.push_back(new GGUI::Input(' ', GGUI::Constants::ESCAPE));
                        INTERNAL::KEYBOARD_STATES[BUTTON_STATES::ESC] = INTERNAL::buttonState(Pressed);
                        handleEscape();
                    }
                    else if (Raw_Input[i].Event.KeyEvent.wVirtualKeyCode == VK_TAB){
                        INTERNAL::Inputs.push_back(new GGUI::Input(' ', GGUI::Constants::TAB));
                        INTERNAL::KEYBOARD_STATES[BUTTON_STATES::TAB] = INTERNAL::buttonState(Pressed);
                        handleTabulator();
                    }
                    else if (Raw_Input[i].Event.KeyEvent.uChar.AsciiChar != 0 && Pressed){
                        char Result = Reverse_Engineer_Keybinds(Raw_Input[i].Event.KeyEvent.uChar.AsciiChar);

                        INTERNAL::Inputs.push_back(new GGUI::Input(Result, GGUI::Constants::KEY_PRESS));
                    }
                }
                else if (Raw_Input[i].EventType == WINDOW_BUFFER_SIZE_EVENT){
                    INTERNAL::Carry_Flags([](GGUI::INTERNAL::Carry& current_carry){
                        current_carry.Resize = true;    // Tell the render thread that an resize is needed to be performed.
                    });
                }
                else if (Raw_Input[i].EventType == MOUSE_EVENT && INTERNAL::Mouse_Movement_Enabled){
                    if (Raw_Input[i].Event.MouseEvent.dwEventFlags == MOUSE_MOVED){
                        // Get mouse coordinates
                        COORD mousePos = Raw_Input[i].Event.MouseEvent.dwMousePosition;
                        // Handle cursor movement
                        INTERNAL::Mouse.X = mousePos.X;
                        INTERNAL::Mouse.Y = mousePos.Y;
                    }
                    // Handle mouse clicks
                    // TODO: Windows doesn't give release events.
                    // Yes it does you fucking moron!
                    if ((Raw_Input[i].Event.MouseEvent.dwButtonState & FROM_LEFT_1ST_BUTTON_PRESSED) != 0) {
                        //PREVIOUS_KEYBOARD_STATES[BUTTON_STATES::MOUSE_LEFT].State = KEYBOARD_STATES[BUTTON_STATES::MOUSE_LEFT].State;
                        INTERNAL::KEYBOARD_STATES[BUTTON_STATES::MOUSE_LEFT].State = true;
                        INTERNAL::KEYBOARD_STATES[BUTTON_STATES::MOUSE_LEFT].Capture_Time = std::chrono::high_resolution_clock::now();
                    }
                    else if ((Raw_Input[i].Event.MouseEvent.dwButtonState & FROM_LEFT_1ST_BUTTON_PRESSED) == 0) {
                        //PREVIOUS_KEYBOARD_STATES[BUTTON_STATES::MOUSE_LEFT].State = KEYBOARD_STATES[BUTTON_STATES::MOUSE_LEFT].State;
                        INTERNAL::KEYBOARD_STATES[BUTTON_STATES::MOUSE_LEFT].State = false;
                    }

                    if ((Raw_Input[i].Event.MouseEvent.dwButtonState & RIGHTMOST_BUTTON_PRESSED) != 0) {
                        //PREVIOUS_KEYBOARD_STATES[BUTTON_STATES::MOUSE_RIGHT].State = KEYBOARD_STATES[BUTTON_STATES::MOUSE_RIGHT].State;
                        INTERNAL::KEYBOARD_STATES[BUTTON_STATES::MOUSE_RIGHT].State = true;
                        INTERNAL::KEYBOARD_STATES[BUTTON_STATES::MOUSE_RIGHT].Capture_Time = std::chrono::high_resolution_clock::now();
                    }
                    else if ((Raw_Input[i].Event.MouseEvent.dwButtonState & RIGHTMOST_BUTTON_PRESSED) == 0) {
                        //PREVIOUS_KEYBOARD_STATES[BUTTON_STATES::MOUSE_RIGHT].State = KEYBOARD_STATES[BUTTON_STATES::MOUSE_RIGHT].State;
                        INTERNAL::KEYBOARD_STATES[BUTTON_STATES::MOUSE_RIGHT].State = false;
                    }
                
                    // mouse scroll up
                    if (Raw_Input[i].Event.MouseEvent.dwEventFlags == MOUSE_WHEELED){
                        // check if it has been wheeled up or down
                        int Scroll_Direction = GET_WHEEL_DELTA_WPARAM(Raw_Input[i].Event.MouseEvent.dwButtonState);

                        if (Scroll_Direction > 0){
                            INTERNAL::KEYBOARD_STATES[BUTTON_STATES::MOUSE_SCROLL_UP].State = true;
                            INTERNAL::KEYBOARD_STATES[BUTTON_STATES::MOUSE_SCROLL_DOWN].State = false;

                            INTERNAL::KEYBOARD_STATES[BUTTON_STATES::MOUSE_SCROLL_UP].Capture_Time = std::chrono::high_resolution_clock::now();
                        }
                        else if (Scroll_Direction < 0){
                            INTERNAL::KEYBOARD_STATES[BUTTON_STATES::MOUSE_SCROLL_DOWN].State = true;
                            INTERNAL::KEYBOARD_STATES[BUTTON_STATES::MOUSE_SCROLL_UP].State = false;

                            INTERNAL::KEYBOARD_STATES[BUTTON_STATES::MOUSE_SCROLL_DOWN].Capture_Time = std::chrono::high_resolution_clock::now();
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
            // Save the STD handles to prevent excess calls.
            GLOBAL_STD_OUTPUT_HANDLE = GetStdHandle(STD_OUTPUT_HANDLE);
            GLOBAL_STD_INPUT_HANDLE = GetStdHandle(STD_INPUT_HANDLE);

            // Retrieve and store previous console modes for restoration upon exit.
            GetConsoleMode(GLOBAL_STD_OUTPUT_HANDLE, &PREVIOUS_CONSOLE_OUTPUT_STATE);
            GetConsoleMode(GLOBAL_STD_INPUT_HANDLE, &PREVIOUS_CONSOLE_INPUT_STATE);

            // Set new console modes with extended flags, mouse, and window input enabled.
            SetConsoleMode(GLOBAL_STD_OUTPUT_HANDLE, -1);
            SetConsoleMode(GLOBAL_STD_INPUT_HANDLE, ENABLE_EXTENDED_FLAGS | ENABLE_MOUSE_INPUT | ENABLE_WINDOW_INPUT | ENABLE_PROCESSED_INPUT);

            // Enable specific ANSI features for mouse event reporting and hide the mouse cursor.
            std::cout << GGUI::Constants::ANSI::Enable_Private_SGR_Feature(GGUI::Constants::ANSI::REPORT_MOUSE_ALL_EVENTS).To_String() 
                    << GGUI::Constants::ANSI::Enable_Private_SGR_Feature(GGUI::Constants::ANSI::MOUSE_CURSOR, false).To_String();
            std::cout.flush();

            // Set the console output code page to UTF-8 mode.
            SetConsoleOutputCP(Constants::ANSI::ENABLE_UTF8_MODE_FOR_WINDOWS);

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
            INTERNAL::Platform_Initialized = true;
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

                INTERNAL::reportStack("Failed to get console info: " + std::to_string(Last_Error));
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
                DWORD cValues; 
                // Get the number of values in the key
                RegQueryInfoKey(hKey, NULL, NULL, NULL, NULL, NULL, NULL, &cValues, NULL, NULL, NULL, NULL);
                for (DWORD i = 0; i < cValues; i++) {
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

        /**
         * @brief Retrieves the module handle corresponding to a specific address.
         *
         * This function queries the loaded module that contains the provided memory address.
         * It uses the `GetModuleHandleEx` API with `GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS`.
         *
         * @param address The memory address to resolve.
         * @return HMODULE The handle to the module containing the address, or NULL if not found.
         */
        HMODULE GetModuleFromAddress(void* address) {
            HMODULE moduleHandle = NULL;
            if (!GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                                static_cast<LPCTSTR>(address), &moduleHandle)) {
                return NULL;
            }
            return moduleHandle;
        }

        /**
         * @brief Resolves the export symbol name from a module's export table for a given address.
         *
         * This function performs the following:
         * - Identifies the module that contains the provided address.
         * - Parses the module's PE headers and export directory.
         * - Matches the address to an exported function range.
         * - Returns the corresponding exported function name or ordinal.
         *
         * @param address A pointer to the code address to resolve.
         * @return std::string The resolved function name or ordinal, or an empty string if not found.
         */
        std::string ResolveSymbolFromExportTable(void* address) {
            HMODULE moduleHandle = GetModuleFromAddress(address);
            if (!moduleHandle) {
                return "";
            }

            BYTE* moduleBase = reinterpret_cast<BYTE*>(moduleHandle);
            PIMAGE_DOS_HEADER dosHeader = reinterpret_cast<PIMAGE_DOS_HEADER>(moduleBase);

            if (dosHeader->e_magic != IMAGE_DOS_SIGNATURE) {
                return "";
            }

            PIMAGE_NT_HEADERS ntHeaders = reinterpret_cast<PIMAGE_NT_HEADERS>(moduleBase + dosHeader->e_lfanew);
            if (ntHeaders->Signature != IMAGE_NT_SIGNATURE) {
                return "";
            }

            IMAGE_DATA_DIRECTORY& exportDirectoryEntry =
                ntHeaders->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT];

            if (exportDirectoryEntry.Size == 0) {
                return "";
            }

            PIMAGE_EXPORT_DIRECTORY exportDirectory =
                reinterpret_cast<PIMAGE_EXPORT_DIRECTORY>(moduleBase + exportDirectoryEntry.VirtualAddress);

            DWORD* functionAddresses = reinterpret_cast<DWORD*>(moduleBase + exportDirectory->AddressOfFunctions);
            DWORD* functionNames = reinterpret_cast<DWORD*>(moduleBase + exportDirectory->AddressOfNames);
            WORD* nameOrdinals = reinterpret_cast<WORD*>(moduleBase + exportDirectory->AddressOfNameOrdinals);

            // Pair of <function virtual address, original ordinal index>
            std::vector<std::pair<BYTE*, DWORD>> exportFunctionEntries;
            for (DWORD i = 0; i < exportDirectory->NumberOfFunctions; ++i) {
                DWORD functionRelativeAddress = functionAddresses[i];
                if (functionRelativeAddress == 0) {
                    continue; // Skip forwarded or null exports
                }
                BYTE* functionAbsoluteAddress = moduleBase + functionRelativeAddress;
                exportFunctionEntries.emplace_back(functionAbsoluteAddress, i);
            }

            // Sort exported functions by address for efficient range search
            std::sort(exportFunctionEntries.begin(), exportFunctionEntries.end(),
                    [](const auto& a, const auto& b) {
                        return a.first < b.first;
                    });

            // Find the address range the input address falls into
            for (size_t i = 0; i < exportFunctionEntries.size(); ++i) {
                BYTE* functionStartAddress = exportFunctionEntries[i].first;
                DWORD functionOrdinalIndex = exportFunctionEntries[i].second;

                // Determine the end of this function's code by checking the next function address or section boundary
                BYTE* functionEndAddress = nullptr;
                if (i + 1 < exportFunctionEntries.size()) {
                    functionEndAddress = exportFunctionEntries[i + 1].first;
                } else {
                    // Locate the section to determine the upper bound
                    DWORD relativeAddress = static_cast<DWORD>(functionStartAddress - moduleBase);
                    PIMAGE_SECTION_HEADER sectionHeader = IMAGE_FIRST_SECTION(ntHeaders);
                    for (unsigned int j = 0; j < ntHeaders->FileHeader.NumberOfSections; ++j, ++sectionHeader) {
                        DWORD sectionStart = sectionHeader->VirtualAddress;
                        DWORD sectionEnd = sectionStart + sectionHeader->Misc.VirtualSize;
                        if (relativeAddress >= sectionStart && relativeAddress < sectionEnd) {
                            functionEndAddress = moduleBase + sectionEnd;
                            break;
                        }
                    }
                    // Fallback range if section not identified
                    if (!functionEndAddress) {
                        functionEndAddress = functionStartAddress + 0x1000;
                    }
                }

                // Check if the input address lies within the current function's range
                if (reinterpret_cast<BYTE*>(address) >= functionStartAddress &&
                    reinterpret_cast<BYTE*>(address) < functionEndAddress) {

                    // Try to find the function's name using its ordinal
                    for (DWORD j = 0; j < exportDirectory->NumberOfNames; ++j) {
                        if (nameOrdinals[j] == functionOrdinalIndex) {
                            const char* functionName = reinterpret_cast<const char*>(moduleBase + functionNames[j]);
                            return std::string(functionName);
                        }
                    }

                    // If no name found, fall back to ordinal
                    DWORD exportedOrdinal = functionOrdinalIndex + exportDirectory->Base;
                    return "ExportedFunc_Ordinal_" + std::to_string(exportedOrdinal);
                }
            }

            return ""; // No match found in the export table
        }

        /**
         * @brief Performs basic demangling of Itanium C++ ABI mangled symbols.
         *
         * This function provides a simplified demangler for common mangled symbols generated by GCC and Clang
         * following the Itanium C++ ABI. It supports:
         * - Parsing simple and nested names
         * - Basic support for template argument lists
         *
         * This is *not* a full demangler and will not cover all edge cases or types. For production-grade use,
         * prefer tools like `abi::__cxa_demangle` or third-party demangling libraries.
         *
         * @param mangledSymbol A mangled C++ symbol following the Itanium ABI (typically starting with "_Z").
         * @return std::string A best-effort demangled version of the symbol.
         */
        std::string SimpleDemangle(const std::string& mangledSymbol) {
            const char* cursor = mangledSymbol.c_str();
            const char* end = cursor + mangledSymbol.size();

            // Helper function to parse a decimal number (used for name lengths)
            auto parseLength = [&]() -> int {
                int value = 0;
                if (!std::isdigit(*cursor)) return -1;

                while (cursor < end && std::isdigit(*cursor)) {
                    value = value * 10 + (*cursor - '0');
                    ++cursor;
                }
                return value;
            };

            // Helper function to parse a single name (length-prefixed)
            auto parseName = [&]() -> std::string {
                int length = parseLength();
                if (length < 0 || cursor + length > end) return "";

                std::string name(cursor, cursor + length);
                cursor += length;
                return name;
            };

            // Helper function to parse a nested name: N<names>E
            // Example: N3Foo3BarE -> Foo::Bar
            auto parseNestedName = [&]() -> std::string {
                if (*cursor != 'N') return "";
                ++cursor; // consume 'N'

                std::string fullName;
                while (cursor < end && *cursor != 'E') {
                    std::string component = parseName();
                    if (component.empty()) break;

                    if (!fullName.empty()) {
                        fullName += "::";
                    }
                    fullName += component;
                }

                if (*cursor == 'E') {
                    ++cursor; // consume 'E'
                }

                return fullName;
            };

            // Helper function to parse template arguments enclosed in I...E
            auto parseTemplateArguments = [&]() -> std::string {
                if (*cursor != 'I') return "";
                ++cursor; // consume 'I'

                std::string result = "<";
                bool firstArgument = true;

                while (cursor < end && *cursor != 'E') {
                    if (!firstArgument) {
                        result += ", ";
                    }
                    firstArgument = false;

                    if (*cursor == 'N') {
                        std::string nested = parseNestedName();
                        if (nested.empty()) break;
                        result += nested;
                    } else if (std::isdigit(*cursor)) {
                        std::string name = parseName();
                        if (name.empty()) break;
                        result += name;
                    } else {
                        // Fallback for unknown types or literals; consume single character
                        result += *cursor;
                        ++cursor;
                    }
                }

                if (*cursor == 'E') {
                    ++cursor; // consume 'E'
                }

                result += ">";
                return result;
            };

            // Begin demangling logic

            // All Itanium ABI mangled names start with "_Z"
            if (mangledSymbol.size() < 2 || mangledSymbol[0] != '_' || mangledSymbol[1] != 'Z') {
                return mangledSymbol; // Not a mangled name
            }

            cursor += 2; // skip "_Z"
            std::string demangled;

            if (*cursor == 'N') {
                // Handle nested names
                demangled = parseNestedName();

                if (cursor < end && *cursor == 'I') {
                    // Template arguments may follow the last name component
                    std::string templateArgs = parseTemplateArguments();

                    // Attach template args to last component
                    size_t lastSeparator = demangled.rfind("::");
                    if (lastSeparator == std::string::npos) {
                        demangled += templateArgs;
                    } else {
                        demangled.insert(lastSeparator + 2 + (demangled.size() - lastSeparator - 2), templateArgs);
                    }
                }
            } else if (std::isdigit(*cursor)) {
                // Single name without nesting
                demangled = parseName();

                if (cursor < end && *cursor == 'I') {
                    demangled += parseTemplateArguments();
                }
            } else {
                // Fallback for unrecognized or unsupported patterns
                demangled = mangledSymbol;
            }

            return demangled;
        }

        /**
         * @brief Captures and reports a simplified symbolic stack trace with demangled symbol names.
         *
         * This function uses Windows APIs to capture the current thread's call stack,
         * resolves each address to a symbol name (via the export table), attempts to demangle
         * the name (assuming Itanium ABI-style mangling), and formats the result into a visual stack tree.
         *
         * @param problemDescription A textual description of the issue related to the stack trace.
         */
        void reportStack(const std::string& problemDescription) {
            constexpr int MaximumStackDepth = 10;
            void* stackAddressTable[MaximumStackDepth] = {};
            int capturedFrameCount = 0;

            HANDLE currentProcess = GetCurrentProcess();

            // Set symbol options to improve resolution and enable demangling
            SymSetOptions(SymGetOptions() | SYMOPT_LOAD_LINES | SYMOPT_UNDNAME | SYMOPT_DEFERRED_LOADS);

            // Initialize DbgHelp symbol handler for the current process
            if (!SymInitialize(currentProcess, nullptr, TRUE)) {
                LOGGER::Log("Error: Failed to initialize symbol handler.");
                return;
            }

            // Capture the current call stack
            capturedFrameCount = CaptureStackBackTrace(0, MaximumStackDepth, stackAddressTable, nullptr);

            // Allocate SYMBOL_INFO structure with additional space for symbol name
            SYMBOL_INFO* symbolInfo = reinterpret_cast<SYMBOL_INFO*>(
                calloc(1, sizeof(SYMBOL_INFO) + 256 * sizeof(char))
            );

            if (!symbolInfo) {
                LOGGER::Log("Error: Memory allocation for SYMBOL_INFO failed.");
                return;
            }

            symbolInfo->MaxNameLen = 255;
            symbolInfo->SizeOfStruct = sizeof(SYMBOL_INFO);

            // Ensure any global visual layout constants are updated
            if (Max_Width == 0) {
                updateMaxWidthAndHeight(); // Presumed external function
            }

            // Start assembling the final report
            std::string formattedStackTrace = "Stack Trace:\n";
            int visualDepthIndex = 0;

            const bool enableIndentation = capturedFrameCount < (Max_Width / 2);

            // Traverse stack frames in reverse (from newest to oldest)
            for (int frameIndex = capturedFrameCount - 1; frameIndex > 0; --frameIndex) {
                void* currentAddress = stackAddressTable[frameIndex];
                if (currentAddress == nullptr) {
                    continue;
                }

                // Draw the tree-style branch symbol (main is '|' end is '\')
                std::string branchSymbol = (frameIndex == 1) ? "\\" : "|";

                // Construct indentation based on depth
                std::string indentation;
                if (enableIndentation) {
                    indentation.assign(visualDepthIndex, '-');
                }

                // Attempt to resolve and demangle the symbol for the current address
                std::string symbolName = ResolveSymbolFromExportTable(currentAddress);
                std::string readableName = SimpleDemangle(symbolName);

                if (readableName.empty()) {
                    continue;
                }

                formattedStackTrace += branchSymbol + indentation + " " + readableName + "\n";
                ++visualDepthIndex;
            }

            // Clean up symbol information memory
            free(symbolInfo);

            // Append description of the triggering problem
            formattedStackTrace += "Problem: " + problemDescription;

            // Submit the final formatted report
            report(formattedStackTrace);
        }

    }

    #else

    namespace Constants{
        namespace ANSI{
            constexpr char START_OF_CTRL = 1;
            constexpr char END_OF_CTRL = 26;
        }
    };

    namespace INTERNAL{
        #include <sys/ioctl.h>
        #include <signal.h>
        #include <termios.h>
        #include <execinfo.h>   // For stacktrace
        #include <dlfcn.h>      // For stacktrace
        #include <cxxabi.h>     // For stacktrace
        #include <elf.h>        // For stacktrace
        #include <fcntl.h>      // For stacktrace
        #include <link.h>       // For stacktrace
        #include <sys/mman.h>   // For stacktrace
        #include <sys/stat.h>   // For stacktrace

        int Previous_Flags = 0;
        struct termios Previous_Raw;

        // Stored globally, so that translation and inquiry can be separate proccess.
        const unsigned int Raw_Input_Capacity = UINT8_MAX * 2;
        unsigned char Raw_Input[Raw_Input_Capacity];
        unsigned int Raw_Input_Size = 0;

        /**
         * @brief De-initializes platform-specific settings and resources.
         * @details This function restores console modes to their previous states, cleans up file stream handles,
         *          and disables specific ANSI features. It ensures that any platform-specific settings are reset
         *          before the application exits.
         */
        void De_Initialize(){
            // Restore previous console modes
            for (auto File_Handle : File_Streamer_Handles){
                File_Handle.second->~fileStream();
            }
            
            File_Streamer_Handles.clear();

            // Restore default cursor visibility
            std::cout << GGUI::Constants::ANSI::Enable_Private_SGR_Feature(GGUI::Constants::ANSI::MOUSE_CURSOR).To_String();

            // Disable mouse event reporting
            std::cout << GGUI::Constants::ANSI::Enable_Private_SGR_Feature(GGUI::Constants::ANSI::REPORT_MOUSE_ALL_EVENTS, false).To_String();

            // Disable screen capture
            std::cout << GGUI::Constants::ANSI::Enable_Private_SGR_Feature(GGUI::Constants::ANSI::SCREEN_CAPTURE, false).To_String();  // restores the screen.
            std::cout << std::flush;

            // Restore previous file descriptor flags
            fcntl(STDIN_FILENO, F_SETFL, Previous_Flags); // set non-blocking flag

            // Restore previous terminal attributes
            tcsetattr(STDIN_FILENO, TCSAFLUSH, &Previous_Raw);
        }

        void Cleanup(){
            if (!Carry_Flags.Read().Terminate){
                LOGGER::Log("Sending termination signals to subthreads...");

                // Gracefully shutdown event and rendering threads.
                Carry_Flags([](Carry& self){
                    self.Terminate = true;
                });

                updateFrame();

                // Join the threads
                for (auto& thread : INTERNAL::Sub_Threads){
                    if (thread.joinable()){
                        thread.join();
                    }
                }

                LOGGER::Log("Reverting to normal console mode...");

                // Clean up platform-specific resources and settings
                De_Initialize();

                LOGGER::Log("GGUI shutdown successful.");
            }
        }

        /**
         * @brief De-initializes platform-specific settings and resources and exits the application.
         * @details This function is called by the Exit function to de-initialize platform-specific settings and resources.
         *          It ensures that any platform-specific settings are reset before the application exits.
         * @param signum The exit code for the application.
         */
        void EXIT(int signum){
            Cleanup();
            // Exit the application with the specified exit code
            exit(signum);
        }

        /**
         * @brief Suspends the execution of the calling thread for a specified duration.
         *
         * This function uses the nanosleep system call to suspend the execution of the calling thread
         * for the specified number of milliseconds. It repeatedly calls nanosleep until the entire
         * sleep duration has elapsed.
         *
         * @param mm The number of milliseconds to sleep.
         */
        void SLEEP(unsigned int mm){
            // Define timespec structure for the required sleep duration
            struct timespec req = {0, 0};
            // Calculate seconds from milliseconds
            time_t sec = (int)(mm / 1000);
            // Calculate remaining milliseconds
            mm = mm - (sec * 1000);
            // Set the seconds and nanoseconds in the timespec structure
            req.tv_sec = sec;
            req.tv_nsec = mm * 1000000L;
            // Repeatedly call nanosleep until the sleep duration is fully elapsed
            while(nanosleep(&req, &req) == -1)
                continue;
        }

        /**
         * @brief Renders the contents of the Frame_Buffer to the standard output (STDOUT).
         * @details This function moves the cursor to the top-left corner of the terminal, flushes the output
         *          buffer to ensure immediate writing, and writes the contents of the Frame_Buffer to STDOUT.
         *          If the write operation fails or writes fewer bytes than expected, an error message is reported.
         */
        void renderFrame() {
            // Move the cursor to the top-left corner of the terminal
            printf("%s", GGUI::Constants::ANSI::SET_CURSOR_TO_START.c_str());

            // Flush the output buffer to ensure it's written immediately
            fflush(stdout);

            // Write the contents of Frame_Buffer to STDOUT
            int Error = write(STDOUT_FILENO, Frame_Buffer.data(), Frame_Buffer.size());

            // Check for write errors or incomplete writes
            if (Error != (signed)Frame_Buffer.size()) {
                INTERNAL::reportStack("Failed to write to STDOUT: " + std::to_string(Error));
            }
        }

        /**
         * @brief Updates the maximum width and height of the terminal.
         * @details This function updates the Max_Width and Max_Height variables by calling ioctl to get the current
         *          width and height of the terminal. If the call fails, a report message is sent.
         * @note The height is reduced by 1 to account for the one line of console space taken by the GGUI status bar.
         */
        void updateMaxWidthAndHeight(){
            struct winsize w;
            if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &w) == -1){
                INTERNAL::reportStack("Failed to get console info!");
                return;
            }

            Max_Width = w.ws_col;
            Max_Height = w.ws_row - 1;

            // Convenience sake :)
            if (Main)
                Main->setDimensions(Max_Width, Max_Height);
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
                Carry_Flags([](Carry& current_carry){
                    current_carry.Resize = true;    // Tell the render thread that an resize is needed to be performed.
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
         * @brief Reverse-engineers termios raw terminal keybinds.
         * @param keybind_value The value of the key to reverse-engineer.
         * @return The reversed keybind value.
         * @details This function is used to reverse-engineer termios raw terminal keybinds. It checks if the keybind is in the known keybinding table.
         *          If it is, it returns the reversed keybind value. If not, it returns the normal value.
         */
        char Reverse_Engineer_Keybinds(char keybind_value){

            // SHIFT + TAB => \e[Z
            if (keybind_value == 'Z'){
                // Set the shift key state to true
                KEYBOARD_STATES[BUTTON_STATES::SHIFT] = buttonState(true);
                // Set the tab key state to true
                KEYBOARD_STATES[BUTTON_STATES::TAB] = buttonState(true);

                // Set the keybind value to 0, to indicate that it has been reversed
                keybind_value = 0;
            }

            return keybind_value;
        }

        /**
         * @brief Waits for user input and stores it in the Raw_Input array.
         * @details This function waits for user input and stores it in the Raw_Input array. It is called from the event loop.
         *          It is also the function that is called as soon as possible and gets stuck awaiting for the user input.
         */
        void queryInputs(){
            // Add the previous input size into the current offset for cumulative reading.
            unsigned char* Current_Input_Buffer_Location = Raw_Input + Raw_Input_Size;

            // Ceil the result, so that negative numbers wont start overflow.
            int Current_Input_Buffer_Capacity = Max(Raw_Input_Capacity - Raw_Input_Size, Raw_Input_Capacity);

            Raw_Input_Size = read(
                STDIN_FILENO,
                Current_Input_Buffer_Location,
                Current_Input_Buffer_Capacity
            );
        }

        /**
         * @brief Translate the input events stored in Raw_Input into Input objects.
         * @details This function is used to translate the input events stored in Raw_Input into Input objects. It checks if the event is a key event, and if so, it checks if the key is a special key (up, down, left, right, enter, shift, control, backspace, escape, tab) and if so, it creates an Input object with the corresponding Constants:: value. If the key is not a special key, it creates an Input object with the key's ASCII value and Constants::KEY_PRESS. If the event is not a key event, it checks if the event is a mouse event and if so, it checks if the mouse event is a movement, click or scroll event and if so, it creates an Input object with the corresponding Constants:: value. Finally, it resets the Raw_Input_Size to 0.
         */
        void Translate_Inputs(){
            // Clean the keyboard states.
            PREVIOUS_KEYBOARD_STATES = KEYBOARD_STATES;

            // Unlike in Windows we wont be getting an indication per Key information, whether it was pressed in or out.
            KEYBOARD_STATES.clear();

            // All of the if-else statements could just make into a map, where each key of combination replaces the value of the keyboard state, but you know what?
            // haha code go brrrr -- 2024 gab here, nice joke, although who asked? 
            for (unsigned int i = 0; i < Raw_Input_Size; i++) {

                // Check if SHIFT has been modifying the keys
                if ((Raw_Input[i] >= 'A' && Raw_Input[i] <= 'Z') || (Raw_Input[i] >= '!' && Raw_Input[i] <= '/')) {
                    // SHIFT key is pressed
                    Inputs.push_back(new GGUI::Input(' ', Constants::SHIFT));
                    KEYBOARD_STATES[BUTTON_STATES::SHIFT] = buttonState(true);
                }

                // We now can also check if the letter has been shifted down by CTRL key
                else if (Raw_Input[i] >= Constants::ANSI::START_OF_CTRL && Raw_Input[i] <= Constants::ANSI::END_OF_CTRL) {
                    // This is a CTRL key

                    // The CTRL domain contains multiple useful keys to check for
                    if (Raw_Input[i] == Constants::ANSI::BACKSPACE) {
                        // This is a backspace key
                        Inputs.push_back(new GGUI::Input(' ', Constants::BACKSPACE));
                        KEYBOARD_STATES[BUTTON_STATES::BACKSPACE] = buttonState(true);
                    }
                    else if (Raw_Input[i] == Constants::ANSI::HORIZONTAL_TAB) {
                        // This is a tab key
                        Inputs.push_back(new GGUI::Input(' ', Constants::TAB));
                        KEYBOARD_STATES[BUTTON_STATES::TAB] = buttonState(true);
                        handleTabulator();
                    }
                    else if (Raw_Input[i] == Constants::ANSI::LINE_FEED) {
                        // This is an enter key
                        Inputs.push_back(new GGUI::Input(' ', Constants::ENTER));
                        KEYBOARD_STATES[BUTTON_STATES::ENTER] = buttonState(true);
                    }

                    // Shift the key back up
                    char Offset = 'a' - 1; // We remove one since the CTRL characters started from 1 and not 0
                    Raw_Input[i] = Raw_Input[i] + Offset;
                    KEYBOARD_STATES[BUTTON_STATES::CONTROL] = buttonState(true);
                }

                if (Raw_Input[i] == Constants::ANSI::ESC_CODE[0]) {
                    // check if there are stuff after this escape code
                    if (i + 1 >= Raw_Input_Size) {
                        // Clearly the escape key was invoked
                        Inputs.push_back(new GGUI::Input(' ', Constants::ESCAPE));
                        KEYBOARD_STATES[BUTTON_STATES::ESC] = buttonState(true);
                        handleEscape();
                        continue;
                    }

                    // GG go next
                    i++;

                    // The current data can either be an ALT key initiative or an escape sequence followed by '['
                    if (Raw_Input[i] == Constants::ANSI::ESC_CODE[1]) {
                        // Escape sequence codes:

                        // UP, DOWN LEFT, RIGHT keys
                        if (Raw_Input[i + 1] == 'A') {
                            Inputs.push_back(new GGUI::Input(0, Constants::UP));
                            KEYBOARD_STATES[BUTTON_STATES::UP] = buttonState(true);
                            i++;
                        }
                        else if (Raw_Input[i + 1] == 'B') {
                            Inputs.push_back(new GGUI::Input(0, Constants::DOWN));
                            KEYBOARD_STATES[BUTTON_STATES::DOWN] = buttonState(true);
                            i++;
                        }
                        else if (Raw_Input[i + 1] == 'C') {
                            Inputs.push_back(new GGUI::Input(0, Constants::RIGHT));
                            KEYBOARD_STATES[BUTTON_STATES::RIGHT] = buttonState(true);
                            i++;
                        }
                        else if (Raw_Input[i + 1] == 'D') {
                            Inputs.push_back(new GGUI::Input(0, Constants::LEFT));
                            KEYBOARD_STATES[BUTTON_STATES::LEFT] = buttonState(true);
                            i++;
                        }
                        else if (Raw_Input[i + 1] == 'M') {  // Decode Mouse handling
                            // Payload structure: '\e[Mbxy' where the b is bitmask representing the buttons, x and y representing the location of the mouse. 
                            char Bit_Mask = Raw_Input[i + 2];

                            // Check if the bit 2'rd has been set, is so then the SHIFT has been pressed
                            if (Bit_Mask & 4) {
                                Inputs.push_back(new GGUI::Input(' ', Constants::SHIFT));
                                KEYBOARD_STATES[BUTTON_STATES::SHIFT] = buttonState(true);
                                // also remove the bit from the bitmask
                                Bit_Mask &= ~4;
                            }

                            // Check if the 3'th bit has been set, is so then the SUPER has been pressed
                            if (Bit_Mask & 8) {
                                Inputs.push_back(new GGUI::Input(' ', Constants::SUPER));
                                KEYBOARD_STATES[BUTTON_STATES::SUPER] = buttonState(true);
                                // also remove the bit from the bitmask
                                Bit_Mask &= ~8;
                            }

                            // Check if the 4'th bit has been set, is so then the CTRL has been pressed
                            if (Bit_Mask & 16) {
                                Inputs.push_back(new GGUI::Input(' ', Constants::CONTROL));
                                KEYBOARD_STATES[BUTTON_STATES::CONTROL] = buttonState(true);
                                // also remove the bit from the bitmask
                                Bit_Mask &= ~16;
                            }

                            // Bit 5'th is not widely supported so remove it in case.
                            Bit_Mask &= ~32;

                            // Check if the 6'th bit has been set, is so then there is a movement event.
                            if (Bit_Mask & 64) {
                                char X = Raw_Input[i + 3];
                                char Y = Raw_Input[i + 4];

                                // XTERM will normally shift its X and Y coordinates by 32, so that it skips all the control characters in ASCII.
                                Mouse.X = X - 32;
                                Mouse.Y = Y - 32;

                                Bit_Mask &= ~64;
                            }

                            // Bits 7'th are not widely supported. But clear this bit just in case
                            Bit_Mask &= ~(128);

                            if (Bit_Mask == 0) {
                                KEYBOARD_STATES[BUTTON_STATES::MOUSE_LEFT] = buttonState(true);
                                KEYBOARD_STATES[BUTTON_STATES::MOUSE_LEFT].Capture_Time = std::chrono::high_resolution_clock::now();
                            }
                            else if (Bit_Mask == 1) {
                                KEYBOARD_STATES[BUTTON_STATES::MOUSE_MIDDLE] = buttonState(true);
                                KEYBOARD_STATES[BUTTON_STATES::MOUSE_MIDDLE].Capture_Time = std::chrono::high_resolution_clock::now();
                            }
                            else if (Bit_Mask == 2) {
                                KEYBOARD_STATES[BUTTON_STATES::MOUSE_RIGHT] = buttonState(true);
                                KEYBOARD_STATES[BUTTON_STATES::MOUSE_RIGHT].Capture_Time = std::chrono::high_resolution_clock::now();
                            }
                            else if (Bit_Mask == 3) {
                                KEYBOARD_STATES[BUTTON_STATES::MOUSE_LEFT].State = false;
                                KEYBOARD_STATES[BUTTON_STATES::MOUSE_MIDDLE].State = false;
                                KEYBOARD_STATES[BUTTON_STATES::MOUSE_RIGHT].State = false;
                            }

                            i += 4;
                        }
                        else if (Raw_Input[i + 1] == 'Z') {
                            // SHIFT + TAB => Z
                            Inputs.push_back(new GGUI::Input(' ', Constants::SHIFT));
                            Inputs.push_back(new GGUI::Input(' ', Constants::TAB));

                            KEYBOARD_STATES[BUTTON_STATES::SHIFT] = buttonState(true);
                            KEYBOARD_STATES[BUTTON_STATES::TAB] = buttonState(true);

                            handleTabulator();

                            i++;
                        }

                    }
                    else {
                        // This is an ALT key
                        Inputs.push_back(new GGUI::Input(Raw_Input[i], Constants::ALT));
                        KEYBOARD_STATES[BUTTON_STATES::ALT] = buttonState(true);
                    }
                }
                else {
                    // Normal character data
                    Inputs.push_back(new GGUI::Input(Raw_Input[i], Constants::KEY_PRESS));
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
            // Initialize the console for mouse and window input, and set UTF-8 mode for output.
            std::cout << Constants::ANSI::Enable_Private_SGR_Feature(Constants::ANSI::REPORT_MOUSE_ALL_EVENTS).To_String();
            std::cout << Constants::ANSI::Enable_Private_SGR_Feature(Constants::ANSI::MOUSE_CURSOR, false).To_String();
            std::cout << Constants::ANSI::Enable_Private_SGR_Feature(Constants::ANSI::SCREEN_CAPTURE).To_String();   // for on exit to restore
            // std::cout << Constants::RESET_CONSOLE;
            // std::cout << Constants::EnableFeature(Constants::ALTERNATIVE_SCREEN_BUFFER);    // For double buffer if needed
            std::cout << std::flush;

            // Save the current flags and take an snapshot of the flags before GGUI
            Previous_Flags = fcntl(STDIN_FILENO, F_GETFL, 0);

            // Set non-blocking flag
            int flags = O_RDONLY | O_CLOEXEC;
            fcntl(STDIN_FILENO, F_SETFL, flags);

            // Save the current terminal settings
            struct termios Term_Handle;
            tcgetattr(STDIN_FILENO, &Term_Handle);

            Previous_Raw = Term_Handle;

            // Set the terminal to raw mode
            Term_Handle.c_lflag &= ~(ECHO | ICANON);
            Term_Handle.c_cc[VMIN] = 1;     // This dictates how many characters until the user input awaiting read() function will return the buffer.
            Term_Handle.c_cc[VTIME] = 0;    // Suppress automatic returning, since we want the Input_Query() to await until the user has given an input.  
            tcsetattr(STDIN_FILENO, TCSAFLUSH, &Term_Handle);

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
                LOGGER::Log("Failed to register exit handler.");
            }
        
            // Add a signal handler to automatically update the terminal size whenever a SIGWINCH signal is received.
            Add_Automatic_Terminal_Size_Update_Handler();
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
            std::string Raw_Result = Handle.Run("fc-list -v | grep file");

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

        /**
         * Resolves the symbol name corresponding to a given instruction address by inspecting the
         * ELF symbol table of the shared object or binary to which the address belongs.
         * 
         * This implementation:
         * - Works without third-party libraries
         * - Operates only on ELF64 (x86_64) binaries
         * - Uses .symtab and .strtab for symbol resolution
         * - Requires non-stripped binaries to find full symbol names
         * 
         * @param address Pointer to the instruction address to resolve.
         * @return A demangled symbol name string if found, or an empty string otherwise.
         */
        std::string ResolveSymbolFromAddress(void* address) {
            Dl_info dynamicLinkInfo;

            // Query the base module and symbol information from the address
            if (!dladdr(address, &dynamicLinkInfo) || !dynamicLinkInfo.dli_fname || !dynamicLinkInfo.dli_fbase) {
                return "";
            }

            const char* moduleFilePath = dynamicLinkInfo.dli_fname;
            uintptr_t moduleBaseAddress = reinterpret_cast<uintptr_t>(dynamicLinkInfo.dli_fbase);
            uintptr_t targetAddress = reinterpret_cast<uintptr_t>(address);

            // Open the shared object or executable for reading
            int fileDescriptor = open(moduleFilePath, O_RDONLY);
            if (fileDescriptor < 0) {
                return "";
            }

            // Retrieve file size for mmap
            struct stat fileStatus;
            if (fstat(fileDescriptor, &fileStatus) < 0) {
                close(fileDescriptor);
                return "";
            }

            // Memory-map the ELF file for reading
            void* mappedElfFile = mmap(nullptr, fileStatus.st_size, PROT_READ, MAP_PRIVATE, fileDescriptor, 0);
            close(fileDescriptor);
            if (mappedElfFile == MAP_FAILED) {
                return "";
            }

            // Verify the ELF header
            Elf64_Ehdr* elfHeader = reinterpret_cast<Elf64_Ehdr*>(mappedElfFile);
            if (memcmp(elfHeader->e_ident, ELFMAG, SELFMAG) != 0) {
                munmap(mappedElfFile, fileStatus.st_size);
                return "";
            }

            // Locate section headers
            Elf64_Shdr* sectionHeaders = reinterpret_cast<Elf64_Shdr*>(
                reinterpret_cast<char*>(mappedElfFile) + elfHeader->e_shoff
            );

            Elf64_Shdr* symbolTableSection = nullptr;
            Elf64_Shdr* stringTableSection = nullptr;

            // Iterate over all sections to find the .symtab and its associated string table (.strtab)
            for (int sectionIndex = 0; sectionIndex < elfHeader->e_shnum; ++sectionIndex) {
                if (sectionHeaders[sectionIndex].sh_type == SHT_SYMTAB) {
                    symbolTableSection = &sectionHeaders[sectionIndex];
                    stringTableSection = &sectionHeaders[symbolTableSection->sh_link];
                    break;
                }
            }

            // Ensure both sections are present
            if (!symbolTableSection || !stringTableSection) {
                munmap(mappedElfFile, fileStatus.st_size);
                return "";
            }

            const char* stringTableData = reinterpret_cast<const char*>(
                reinterpret_cast<const char*>(mappedElfFile) + stringTableSection->sh_offset
            );

            Elf64_Sym* symbolEntries = reinterpret_cast<Elf64_Sym*>(
                reinterpret_cast<char*>(mappedElfFile) + symbolTableSection->sh_offset
            );

            size_t totalSymbols = symbolTableSection->sh_size / sizeof(Elf64_Sym);

            std::string closestSymbolName;
            uintptr_t smallestOffset = static_cast<uintptr_t>(-1);

            // Iterate through all function symbols and find the closest match not exceeding the address
            for (size_t i = 0; i < totalSymbols; ++i) {
                const Elf64_Sym& symbol = symbolEntries[i];

                // Skip undefined or non-function symbols
                if (ELF64_ST_TYPE(symbol.st_info) != STT_FUNC || symbol.st_shndx == SHN_UNDEF) {
                    continue;
                }

                uintptr_t symbolAbsoluteAddress = moduleBaseAddress + symbol.st_value;

                if (targetAddress >= symbolAbsoluteAddress) {
                    uintptr_t offset = targetAddress - symbolAbsoluteAddress;
                    if (offset < smallestOffset) {
                        smallestOffset = offset;
                        closestSymbolName = std::string(stringTableData + symbol.st_name);
                    }
                }
            }

            // Clean up the memory mapping
            munmap(mappedElfFile, fileStatus.st_size);

            return closestSymbolName;
        }

        /**
         * @brief Captures and reports a formatted stack trace along with a provided error description.
         *
         * This function performs the following:
         * - Captures the current call stack up to a fixed depth.
         * - Resolves each instruction address to a function symbol using ELF symbol tables.
         * - Demangles C++ function names if applicable.
         * - Formats the stack trace into a human-readable, indented tree-like structure.
         * - Appends the original error description to the trace.
         * - Sends the result to a reporting system.
         *
         * Platform: Only supported on non-Android POSIX-compliant systems.
         *
         * @param problemDescription A descriptive message about the problem being reported.
         */
        void reportStack(const std::string& problemDescription) {
        #ifndef __ANDROID__
            constexpr int MaximumStackDepth = 10;
            void* callStackAddresses[MaximumStackDepth];

            // Capture up to MaximumStackDepth stack frames and store them in callStackAddresses
            int capturedFrameCount = backtrace(callStackAddresses, MaximumStackDepth);

            // Attempt to generate human-readable strings for each frame address (optional backup, not used directly)
            char** symbolNames = backtrace_symbols(callStackAddresses, capturedFrameCount);
            if (!symbolNames) {
                report("Error: Failed to retrieve stack trace symbols. Problem: " + problemDescription);
                return;
            }

            // Ensure UI constraints are initialized before printing
            if (Max_Width == 0) {
                updateMaxWidthAndHeight();
            }

            std::string formattedTrace = "Stack Trace:\n";
            bool useIndentation = static_cast<unsigned int>(capturedFrameCount) < (Max_Width / 2);
            int currentIndentLevel = 0;

            // Iterate backwards through the captured frames, omitting the frame that called reportStack
            for (int frameIndex = capturedFrameCount - 1; frameIndex > 0; --frameIndex) {
                std::string linePrefix = (frameIndex == 1) ? "\\" : "|";
                std::string indentation;

                // Add visual indentation for tree structure
                if (useIndentation) {
                    indentation = std::string(currentIndentLevel, '-');
                }

                std::string resolvedSymbol = ResolveSymbolFromAddress(callStackAddresses[frameIndex]);
                if (resolvedSymbol.empty()) {
                    continue; // Skip unresolved frames
                }

                // Attempt to demangle the symbol for improved readability
                int demangleStatus = 0;
                char* demangledName = abi::__cxa_demangle(resolvedSymbol.c_str(), nullptr, nullptr, &demangleStatus);
                std::string functionName = (demangleStatus == 0 && demangledName) ? demangledName : resolvedSymbol;

                // Append to the trace output
                formattedTrace += linePrefix + indentation + " " + functionName + "\n";

                if (demangledName) {
                    std::free(demangledName);
                }

                ++currentIndentLevel;
            }

            // Clean up symbol name memory allocated by backtrace_symbols
            std::free(symbolNames);

            // Attach the problem description to the trace
            formattedTrace += "Problem: " + problemDescription;

            // Output the full report
            report(formattedTrace);
        #else
            // If the platform is unsupported (e.g., Android), report the problem without stack trace
            report(problemDescription);
        #endif
        }
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
            if (Key.second.State) {

                // Skip mouse button keys
                if (BUTTON_STATES::MOUSE_LEFT == Key.first || BUTTON_STATES::MOUSE_RIGHT == Key.first || BUTTON_STATES::MOUSE_MIDDLE == Key.first)
                    continue;

                // Get the constant associated with the key
                unsigned long long Constant_Key = BUTTON_STATES_TO_CONSTANTS_BRIDGE.at(Key.first);

                // Check if the input already exists
                bool Found = false;
                for (auto input : INTERNAL::Inputs) {
                    if (input->Criteria == Constant_Key) {
                        Found = true;
                        break;
                    }
                }

                // If not found, create a new input
                if (!Found)
                    INTERNAL::Inputs.push_back(new Input((char)0, Constant_Key));
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
     */
    void mouseAPI() {
        // Get the duration the left mouse button has been pressed
        unsigned long long Mouse_Left_Pressed_For = (unsigned long long)std::chrono::duration_cast<std::chrono::milliseconds>(
            abs(INTERNAL::Current_Time - INTERNAL::KEYBOARD_STATES[BUTTON_STATES::MOUSE_LEFT].Capture_Time)).count();

        // Check if the left mouse button is pressed and for how long
        if (INTERNAL::KEYBOARD_STATES[BUTTON_STATES::MOUSE_LEFT].State && Mouse_Left_Pressed_For >= SETTINGS::Mouse_Press_Down_Cooldown) {
            INTERNAL::Inputs.push_back(new GGUI::Input(0, Constants::MOUSE_LEFT_PRESSED));
        } 
        // Check if the left mouse button was previously pressed and now released
        else if (!INTERNAL::KEYBOARD_STATES[BUTTON_STATES::MOUSE_LEFT].State && INTERNAL::PREVIOUS_KEYBOARD_STATES[BUTTON_STATES::MOUSE_LEFT].State != INTERNAL::KEYBOARD_STATES[BUTTON_STATES::MOUSE_LEFT].State) {
            INTERNAL::Inputs.push_back(new GGUI::Input(0, Constants::MOUSE_LEFT_CLICKED));
        }

        // Get the duration the right mouse button has been pressed
        unsigned long long Mouse_Right_Pressed_For = (unsigned long long)std::chrono::duration_cast<std::chrono::milliseconds>(
            abs(INTERNAL::Current_Time - INTERNAL::KEYBOARD_STATES[BUTTON_STATES::MOUSE_RIGHT].Capture_Time)).count();

        // Check if the right mouse button is pressed and for how long
        if (INTERNAL::KEYBOARD_STATES[BUTTON_STATES::MOUSE_RIGHT].State && Mouse_Right_Pressed_For >= SETTINGS::Mouse_Press_Down_Cooldown) {
            INTERNAL::Inputs.push_back(new GGUI::Input(0, Constants::MOUSE_RIGHT_PRESSED));
        }
        // Check if the right mouse button was previously pressed and now released
        else if (!INTERNAL::KEYBOARD_STATES[BUTTON_STATES::MOUSE_RIGHT].State && INTERNAL::PREVIOUS_KEYBOARD_STATES[BUTTON_STATES::MOUSE_RIGHT].State != INTERNAL::KEYBOARD_STATES[BUTTON_STATES::MOUSE_RIGHT].State) {
            INTERNAL::Inputs.push_back(new GGUI::Input(0, Constants::MOUSE_RIGHT_CLICKED));
        }

        // Get the duration the middle mouse button has been pressed
        unsigned long long Mouse_Middle_Pressed_For = (unsigned long long)std::chrono::duration_cast<std::chrono::milliseconds>(
            abs(INTERNAL::Current_Time - INTERNAL::KEYBOARD_STATES[BUTTON_STATES::MOUSE_MIDDLE].Capture_Time)).count();

        // Check if the middle mouse button is pressed and for how long
        if (INTERNAL::KEYBOARD_STATES[BUTTON_STATES::MOUSE_MIDDLE].State && Mouse_Middle_Pressed_For >= SETTINGS::Mouse_Press_Down_Cooldown) {
            INTERNAL::Inputs.push_back(new GGUI::Input(0, Constants::MOUSE_MIDDLE_PRESSED));
        }
        // Check if the middle mouse button was previously pressed and now released
        else if (!INTERNAL::KEYBOARD_STATES[BUTTON_STATES::MOUSE_MIDDLE].State && INTERNAL::PREVIOUS_KEYBOARD_STATES[BUTTON_STATES::MOUSE_MIDDLE].State != INTERNAL::KEYBOARD_STATES[BUTTON_STATES::MOUSE_MIDDLE].State) {
            INTERNAL::Inputs.push_back(new GGUI::Input(0, Constants::MOUSE_MIDDLE_CLICKED));
        }
    }

    /**
     * @brief Handles mouse scroll events.
     * @details This function checks if the mouse scroll up or down button has been pressed and if the focused element is not null.
     *          If the focused element is not null, it calls the scroll up or down function on the focused element.
     */
    void scrollAPI(){
        // Check if the mouse scroll up button has been pressed
        if (INTERNAL::KEYBOARD_STATES[BUTTON_STATES::MOUSE_SCROLL_UP].State){

            // If the focused element is not null, call the scroll up function
            if (INTERNAL::Focused_On)
                INTERNAL::Focused_On->scrollUp();
        }
        // Check if the mouse scroll down button has been pressed
        else if (INTERNAL::KEYBOARD_STATES[BUTTON_STATES::MOUSE_SCROLL_DOWN].State){

            // If the focused element is not null, call the scroll down function
            if (INTERNAL::Focused_On)
                INTERNAL::Focused_On->scrollDown();
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
        if (!INTERNAL::KEYBOARD_STATES[BUTTON_STATES::ESC].State)
            return;

        // If the focused element is not null, remove the focus
        if (INTERNAL::Focused_On){
            INTERNAL::Hovered_On = INTERNAL::Focused_On;
            unFocusElement();
        }
        // If the focused element is null but the hovered element is not null, remove the hover
        else if (INTERNAL::Hovered_On){
            unHoverElement();
        }
    }

    /**
     * @brief Handles the pressing of the tab key.
     * @details This function selects the next tabbed element as focused and not hovered.
     *          If the shift key is pressed, it goes backwards in the list of tabbed elements.
     */
    void handleTabulator(){
        // Check if the tab key has been pressed
        if (!INTERNAL::KEYBOARD_STATES[BUTTON_STATES::TAB].State)
            return;

        // Check if the shift key is pressed
        bool Shift_Is_Pressed = INTERNAL::KEYBOARD_STATES[BUTTON_STATES::SHIFT].State;

        // Get the current element from the selected element
        element* Current = INTERNAL::Focused_On;

        // If there has not been anything selected then then skip this phase and default to zero.
        if (!Current)
            Current = INTERNAL::Hovered_On;

        int Current_Index = 0;

        // Find the index of the current element in the list of event handlers
        if (Current)
            for (;(unsigned int)Current_Index < INTERNAL::Event_Handlers.size(); Current_Index++){
                if (INTERNAL::Event_Handlers[Current_Index]->Host == Current)
                    break;
            }

        // Generalize index hopping, if shift is pressed then go backwards.
        Current_Index += 1 + (-2 * Shift_Is_Pressed);

        // If the index is out of bounds, wrap it around to the other side of the list
        if (Current_Index < 0){
            Current_Index = INTERNAL::Event_Handlers.size() - 1;
        }
        else if ((unsigned int)Current_Index >= INTERNAL::Event_Handlers.size()){
            Current_Index = 0;
        }

        // Now update the focused element with the new index
        unHoverElement();
        updateFocusedElement(INTERNAL::Event_Handlers[Current_Index]->Host);
    }

    // Returns the length of a Unicode character based on the first byte.
    // @param first_char The first byte of the character.
    // @return The length of the character in bytes. Returns 1 if it is not a Unicode character.
    int getUnicodeLength(char first_char) {
        // Check if the character is an ASCII character (0xxxxxxx)
        if (!Has_Bit_At(first_char, 7)) // ASCII characters have the most significant bit as 0
            return 1;

        // Check if the character is a 2-byte Unicode character (110xxxxx)
        if (Has_Bit_At(first_char, 7) && Has_Bit_At(first_char, 6) && !Has_Bit_At(first_char, 5))
            return 2;

        // Check if the character is a 3-byte Unicode character (1110xxxx)
        if (Has_Bit_At(first_char, 7) && Has_Bit_At(first_char, 6) && Has_Bit_At(first_char, 5) && !Has_Bit_At(first_char, 4))
            return 3;

        // Check if the character is a 4-byte Unicode character (11110xxx)
        if (Has_Bit_At(first_char, 7) && Has_Bit_At(first_char, 6) && Has_Bit_At(first_char, 5) && Has_Bit_At(first_char, 4) && !Has_Bit_At(first_char, 3))
            return 4;

        // Default case: return 1 for invalid or unexpected byte patterns
        return 1;
    }

    /**
     * @brief Gets the current maximum width of the terminal.
     * @details This function returns the current maximum width of the terminal. If the width is 0, it will set the carry flag to indicate that a resize is needed to be performed.
     *
     * @return The current maximum width of the terminal.
     */
    int getMaxWidth(){
        if (INTERNAL::Max_Width == 0 && INTERNAL::Max_Height == 0){
            INTERNAL::Carry_Flags([](GGUI::INTERNAL::Carry& current_carry){
                current_carry.Resize = true;    // Tell the render thread that an resize is needed to be performed.
            });
        }
        
        return INTERNAL::Max_Width;
    }

    /**
     * @brief Gets the current maximum height of the terminal.
     * @details This function returns the current maximum height of the terminal. If the height is 0, it will set the carry flag to indicate that a resize is needed to be performed.
     *
     * @return The current maximum height of the terminal.
     */
    int getMaxHeight(){
        if (INTERNAL::Max_Width == 0 && INTERNAL::Max_Height == 0){
            INTERNAL::Carry_Flags([](GGUI::INTERNAL::Carry& current_carry){
                current_carry.Resize = true;    // Tell the render thread that an resize is needed to be performed.
            });
        }

        return INTERNAL::Max_Height;
    }

    namespace INTERNAL{
        static Super_String LIQUIFY_UTF_TEXT_RESULT_CACHE;
        static Super_String LIQUIFY_UTF_TEXT_TMP_CONTAINER(GGUI::Constants::ANSI::Maximum_Needed_Pre_Allocation_For_Encoded_Super_String);
        static Super_String LIQUIFY_UTF_TEXT_TEXT_OVERHEAD(GGUI::Constants::ANSI::Maximum_Needed_Pre_Allocation_For_Over_Head);
        static Super_String LIQUIFY_UTF_TEXT_BACKGROUND_OVERHEAD(GGUI::Constants::ANSI::Maximum_Needed_Pre_Allocation_For_Over_Head);
        static Super_String LIQUIFY_UTF_TEXT_TEXT_COLOUR(GGUI::Constants::ANSI::Maximum_Needed_Pre_Allocation_For_Color);
        static Super_String LIQUIFY_UTF_TEXT_BACKGROUND_COLOUR(GGUI::Constants::ANSI::Maximum_Needed_Pre_Allocation_For_Color);
    }

    /**
     * @brief Converts a vector of UTFs into a Super_String.
     * @details This function takes a vector of UTFs, and converts it into a Super_String. The resulting Super_String is stored in a cache, and the cache is resized if the window size has changed.
     * @param Text The vector of UTFs to convert.
     * @param Width The width of the window.
     * @param Height The height of the window.
     * @return A pointer to the resulting Super_String.
     */
    GGUI::Super_String* liquifyUTFText(std::vector<GGUI::UTF>& Text, int Width, int Height){
        const unsigned int Maximum_Needed_Pre_Allocation_For_Whole_Cache_Buffer = (Width * Height * Constants::ANSI::Maximum_Needed_Pre_Allocation_For_Encoded_Super_String + SETTINGS::Word_Wrapping * (Height - 1));
        
        // Since they are located as globals we need to remember to restart the starting offset.
        INTERNAL::LIQUIFY_UTF_TEXT_RESULT_CACHE.Clear();
        INTERNAL::LIQUIFY_UTF_TEXT_TMP_CONTAINER.Clear();
        INTERNAL::LIQUIFY_UTF_TEXT_TEXT_OVERHEAD.Clear();
        INTERNAL::LIQUIFY_UTF_TEXT_BACKGROUND_OVERHEAD.Clear();
        INTERNAL::LIQUIFY_UTF_TEXT_TEXT_COLOUR.Clear();
        INTERNAL::LIQUIFY_UTF_TEXT_BACKGROUND_COLOUR.Clear();
        
        // We need to dynamically resize this, since the window size will be potentially re-sized.
        if (INTERNAL::LIQUIFY_UTF_TEXT_RESULT_CACHE.Data.capacity() != Maximum_Needed_Pre_Allocation_For_Whole_Cache_Buffer){
            INTERNAL::LIQUIFY_UTF_TEXT_RESULT_CACHE = Super_String(Maximum_Needed_Pre_Allocation_For_Whole_Cache_Buffer);
        }

        Super_String* Result = &INTERNAL::LIQUIFY_UTF_TEXT_RESULT_CACHE;
 
        for (int y = 0; y < Height; y++){
            for (int x = 0; x < Width; x++){
                Text[y * Width + x].To_Encoded_Super_String(
                    &INTERNAL::LIQUIFY_UTF_TEXT_TMP_CONTAINER,
                    &INTERNAL::LIQUIFY_UTF_TEXT_TEXT_OVERHEAD,
                    &INTERNAL::LIQUIFY_UTF_TEXT_BACKGROUND_OVERHEAD,
                    &INTERNAL::LIQUIFY_UTF_TEXT_TEXT_COLOUR,
                    &INTERNAL::LIQUIFY_UTF_TEXT_BACKGROUND_COLOUR
                );
                
                Result->Add(INTERNAL::LIQUIFY_UTF_TEXT_TMP_CONTAINER);

                // now instead of emptying the Super_String.vector, we can reset the current index into 0 again.
                INTERNAL::LIQUIFY_UTF_TEXT_TMP_CONTAINER.Clear();
                INTERNAL::LIQUIFY_UTF_TEXT_TEXT_OVERHEAD.Clear();
                INTERNAL::LIQUIFY_UTF_TEXT_BACKGROUND_OVERHEAD.Clear();   
                INTERNAL::LIQUIFY_UTF_TEXT_TEXT_COLOUR.Clear();
                INTERNAL::LIQUIFY_UTF_TEXT_BACKGROUND_COLOUR.Clear();
            }

            // the system doesn't have word wrapping enabled then, use newlines as replacement.
            if (!SETTINGS::Word_Wrapping){
                Result->Add('\n');   // the system is word wrapped.
            }
        }

        return Result;
    }

    /**
     * @brief Updates the frame.
     * @details This function updates the frame. It's the main entry point for the rendering thread.
     * @note This function will return immediately if the rendering thread is paused.
     */
    void updateFrame(){
        std::unique_lock lock(INTERNAL::atomic::Mutex);

        // Check if the rendering thread is paused.
        if (INTERNAL::atomic::Pause_Render_Thread == INTERNAL::atomic::status::LOCKED)
            return;

        // Give the rendering thread one ticket.
        INTERNAL::atomic::Pause_Render_Thread = INTERNAL::atomic::status::RESUMED;

        // Notify all waiting threads that the frame has been updated.
        INTERNAL::atomic::Condition.notify_all();
    }

    /**
     * @brief Pauses the rendering thread.
     * @details This function pauses the rendering thread. The thread will wait until the rendering thread is resumed.
     */
    void pauseGGUI(){
        std::unique_lock lock(INTERNAL::atomic::Mutex);

        // Set the render status to locked.
        INTERNAL::atomic::Pause_Render_Thread = INTERNAL::atomic::status::LOCKED;

        // Wait for the rendering thread to become available.
        INTERNAL::atomic::Condition.wait_for(lock, GGUI::SETTINGS::Thread_Timeout, []{
            // If the rendering thread is not locked, then the wait is over.
            return INTERNAL::atomic::Pause_Render_Thread == INTERNAL::atomic::status::LOCKED;
        });
    }

    /**
     * @brief Resumes the rendering thread.
     * @details This function resumes the rendering thread after it has been paused.
     * @param restore_render_to The status to restore the rendering thread to.
     */
    void resumeGGUI(INTERNAL::atomic::status restore_render_to){
        {
            // Local scope to set the new render status.
            std::unique_lock lock(INTERNAL::atomic::Mutex);
            // Set the render status to the given status.
            INTERNAL::atomic::Pause_Render_Thread = restore_render_to;
        }

        // Check if the rendering status is anything but locked.
        if (restore_render_to < INTERNAL::atomic::status::LOCKED){
            // If it's not locked, then update the frame.
            updateFrame();
        }
    }

    /**
     * @brief This function is a helper for the smart memory system to recall which tasks should be prolonged, and which should be deleted.
     * @details This function is a lambda function that is used by the Atomic::Guard class to prolong or delete memories in the smart memory system.
     *          It takes a pointer to a vector of Memory objects and prolongs or deletes the memories in the vector based on the time difference between the current time and the memory's start time.
     */
    void recallMemories(){
        INTERNAL::Remember([](std::vector<Memory>& rememberable){
            std::chrono::high_resolution_clock::time_point Current_Time = std::chrono::high_resolution_clock::now();

            // For smart memory system to shorten the next sleep time to arrive at the perfect time for the nearest memory.
            size_t Shortest_Time = INTERNAL::MAX_UPDATE_SPEED;
            // Prolong prolongable memories.
            for (unsigned int i = 0; i < rememberable.size(); i++){
                for (unsigned int j = i + 1; j < rememberable.size(); j++){
                    if (rememberable.at(i).Is(MEMORY_FLAGS::PROLONG_MEMORY) && rememberable.at(j).Is(MEMORY_FLAGS::PROLONG_MEMORY) && i != j)
                        // Check if the Job at I is same as the one at J.
                        if (rememberable.at(i).Job.target<bool(*)(GGUI::Event*)>() == rememberable.at(j).Job.target<bool(*)(GGUI::Event*)>()){
                            // Since J will always be one later than I, J will contain the prolonging memory if there is one. 
                            rememberable.at(i).Start_Time = rememberable.at(j).Start_Time;

                            rememberable.erase(rememberable.begin() + j--);
                            break;
                        }
                }
            }

            for (unsigned int i = 0; i < rememberable.size(); i++){
                //first calculate the time difference between the start if the task and the end task
                size_t Time_Difference = std::chrono::duration_cast<std::chrono::milliseconds>(Current_Time - rememberable.at(i).Start_Time).count();

                size_t Time_Left = rememberable.at(i).End_Time - Time_Difference;

                if (Time_Left < Shortest_Time)
                    Shortest_Time = Time_Left;

                //if the time difference is greater than the time limit, then delete the memory
                if (Time_Difference > rememberable.at(i).End_Time){
                    try{
                        bool Success = rememberable.at(i).Job((Event*)&rememberable.at(i));

                        // If job is a re-trigger it will ignore whether the job was successful or not.
                        if (rememberable.at(i).Is(MEMORY_FLAGS::RETRIGGER)){

                            // May need to change this into more accurate version of time capturing.
                            rememberable.at(i).Start_Time = Current_Time;

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

            INTERNAL::Event_Thread_Load = Lerp(INTERNAL::MIN_UPDATE_SPEED, INTERNAL::MAX_UPDATE_SPEED, Shortest_Time);
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
        for (auto i : INTERNAL::Event_Handlers){
            if (i->Host == current){
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

        for (auto i : INTERNAL::Event_Handlers){
            if (i->Host == current){
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
        if (!INTERNAL::Focused_On)
            return;

        INTERNAL::Focused_On->setFocus(false);

        // Recursively remove focus from all child elements
        Recursively_Apply_Focus(INTERNAL::Focused_On, false);

        INTERNAL::Focused_On = nullptr;
    }

    /**
     * @brief Removes the hover state from the currently hovered element and its children.
     * @details This function checks if there is a currently hovered element.
     *          If there is, it sets the hover state on the element and its children to false.
     *          Hover is only removed if the element's current hover state differs from the desired state.
     */
    void unHoverElement(){
        if (!INTERNAL::Hovered_On)
            return;

        // Set the hover state to false on the currently hovered element
        INTERNAL::Hovered_On->setHoverState(false);

        // Recursively remove the hover state from all child elements
        Recursively_Apply_Hover(INTERNAL::Hovered_On, false);

        // Set the hovered element to nullptr to indicate there is no currently hovered element
        INTERNAL::Hovered_On = nullptr;
    }

    /**
     * @brief Updates the currently focused element to a new candidate.
     * @details This function checks if the new candidate is the same as the current focused element.
     *          If not, it removes the focus from the current element and all its children.
     *          Then, it sets the focus on the new candidate element and all its children.
     * @param new_candidate The new element to focus on.
     */
    void updateFocusedElement(GGUI::element* new_candidate){
        if (INTERNAL::Focused_On == new_candidate || new_candidate == INTERNAL::Main)
            return;

        // Unfocus the previous focused element and its children
        if (INTERNAL::Focused_On){
            unFocusElement();
        }

        // Set the focus on the new element and all its children
        INTERNAL::Focused_On = new_candidate;

        // Set the focus state on the new element to true
        INTERNAL::Focused_On->setFocus(true);

        // Recursively set the focus state on all child elements to true
        Recursively_Apply_Focus(INTERNAL::Focused_On, true);
    }

    /**
     * @brief Updates the currently hovered element to a new candidate.
     * @details This function checks if the new candidate is the same as the current hovered element.
     *          If not, it removes the hover state from the current element and all its children.
     *          Then, it sets the hover state on the new candidate element and all its children.
     * @param new_candidate The new element to hover on.
     */
    void updateHoveredElement(GGUI::element* new_candidate){
        if (INTERNAL::Hovered_On == new_candidate || new_candidate == INTERNAL::Main)
            return;

        // Remove the hover state from the previous hovered element and its children
        if (INTERNAL::Hovered_On){
            unHoverElement();
        }

        // Set the hover state on the new element and all its children
        INTERNAL::Hovered_On = new_candidate;

        // Set the hover state on the new element to true
        INTERNAL::Hovered_On->setHoverState(true);

        // Recursively set the hover state on all child elements to true
        Recursively_Apply_Hover(INTERNAL::Hovered_On, true);
    }

    /**
     * @brief Handles all events in the system.
     * @details This function goes through all event handlers and checks if the event criteria matches any of the inputs.
     *          If a match is found, it calls the event handler job with the input as an argument.
     *          If the job is successful, it removes the input from the list of inputs.
     *          If the job is unsuccessful, it reports an error.
     */
    void eventHandler(){
        // Disable hovered element if the mouse isn't on top of it anymore.
        if (INTERNAL::Hovered_On && !Collides(INTERNAL::Hovered_On, GGUI::INTERNAL::Mouse)){
            unHoverElement();
        }

        // Since some key events are piped to us at a different speed than others, we need to keep the older (un-used) inputs "alive" until their turn arrives.
        Populate_Inputs_For_Held_Down_Keys();

        for (auto& e : INTERNAL::Event_Handlers){

            bool Has_Select_Event = false;

            for (unsigned int i = 0; i < INTERNAL::Inputs.size(); i++){
                if (Has(INTERNAL::Inputs[i]->Criteria, Constants::MOUSE_LEFT_CLICKED | Constants::ENTER))
                    Has_Select_Event = true;

                // Criteria must be identical for more accurate criteria listing.
                if (e->Criteria == INTERNAL::Inputs[i]->Criteria){
                    try{
                        // Check if this job could be run successfully.
                        if (e->Job(INTERNAL::Inputs[i])){
                            //dont let anyone else react to this event.
                            INTERNAL::Inputs.erase(INTERNAL::Inputs.begin() + i);
                        }
                        else{
                            // TODO: report miscarried event job.
                            INTERNAL::reportStack("Job '" + e->ID + "' failed!");
                        }
                    }
                    catch(std::exception& problem){
                        INTERNAL::reportStack("In event: '" + e->ID + "' Problem: " + std::string(problem.what()));
                    }
                }
            }

            // Hosted branches
            if (e->Host){
                if (!e->Host->isDisplayed())
                    continue;

                //update the focused
                if (Collides(e->Host, GGUI::INTERNAL::Mouse)){
                    if (Has_Select_Event){
                        updateFocusedElement(e->Host);
                        unHoverElement();
                    }
                    else{
                        updateHoveredElement(e->Host);
                    }
                }
            }
            // Un-hosted branches
            else{

                // some code...

            }

            if (INTERNAL::Inputs.size() <= 1)
                continue;

            // TODO: Do better you dum!
            // GO through the inputs and check if they contain all the flags required
            unsigned long long Remaining_Flags = e->Criteria;
            std::vector<GGUI::Input *> Accepted_Inputs;

            // if an input has flags that meet the criteria, then remove the criteria from the remaining flags and continue until the remaining flags are equal to zero.
            for (auto& i : INTERNAL::Inputs){

                if (Contains(Remaining_Flags, i->Criteria)){
                    Remaining_Flags &= ~i->Criteria;
                    Accepted_Inputs.push_back(i);
                }

                if (Remaining_Flags == 0)
                    break;
            }

            if (Remaining_Flags == 0){
                // Now we need to find the information to send to the event handler.
                Input* Best_Candidate = Accepted_Inputs[0];

                for (auto i : Accepted_Inputs){
                    if (i->Data > Best_Candidate->Data){
                        Best_Candidate = i;
                    }
                }

                //check if this job could be run successfully.
                if (e->Job(Best_Candidate)){
                    // Now remove the candidates from the input
                    for (unsigned int i = 0; i < INTERNAL::Inputs.size(); i++){
                        if (INTERNAL::Inputs[i] == Best_Candidate){
                            INTERNAL::Inputs.erase(INTERNAL::Inputs.begin() + i);
                            i--;
                        }
                    }
                }
            }

        }
        //Clear_Inputs();
        INTERNAL::Inputs.clear();
    }

    /**
     * Get the ID of a class by name, assigning a new ID if it doesn't exist.
     * 
     * @param n The name of the class.
     * @return The ID of the class.
     */
    int getFreeClassID(std::string n){
        // Check if the class name is already in the map
        if (INTERNAL::Class_Names.find(n) != INTERNAL::Class_Names.end()){
            // Return the existing class ID
            return INTERNAL::Class_Names[n];
        }
        else{
            // Assign a new class ID as the current size of the map
            INTERNAL::Class_Names[n] = INTERNAL::Class_Names.size();

            // Return the newly assigned class ID
            return INTERNAL::Class_Names[n];
        }
    }

    /**
     * @brief Adds a new class with the specified name and styling.
     * @details This function retrieves a unique class ID for the given name.
     *          It then associates the provided styling with this class ID 
     *          in the `Classes` map.
     * 
     * @param name The name of the class.
     * @param Styling The styling to be associated with the class.
     */
    void addClass(std::string name, styling Styling){
        INTERNAL::Classes([name, Styling](auto& classes){
            // Obtain a unique class ID for the given class name
            int Class_ID = getFreeClassID(name);

            // Associate the styling with the obtained class ID
            classes.at(Class_ID) = Styling;
        }); 
    }

    /**
     * @brief Initializes the GGUI system and returns the main window.
     * 
     * @return The main window of the GGUI system.
     */
    GGUI::window* initGGUI(){
        INTERNAL::Read_Start_Addresses();
        SETTINGS::initSettings();
        INTERNAL::LOGGER::Init();
        INTERNAL::LOGGER::Log("Starting GGUI Core initialization...");

        INTERNAL::updateMaxWidthAndHeight();
        
        if (INTERNAL::Max_Height == 0 || INTERNAL::Max_Width == 0){
            INTERNAL::reportStack("Width/Height is zero!");
            return nullptr;
        }

        // Save the state before the init
        INTERNAL::Current_Time = std::chrono::high_resolution_clock::now();
        INTERNAL::Previous_Time = INTERNAL::Current_Time;

        INTERNAL::initPlatformStuff();

        INTERNAL::Main = new window(styling(
            width(INTERNAL::Max_Width) |
            height(INTERNAL::Max_Height)
        ), true);

        std::thread Rendering_Scheduler([&](){
            INTERNAL::renderer();
        });

        std::thread Event_Scheduler([&](){
            INTERNAL::eventThread();
        });

        std::thread Inquire_Scheduler([&](){
            INTERNAL::inputThread();
        });

        INTERNAL::Sub_Threads.push_back(std::move(Rendering_Scheduler));
        INTERNAL::Sub_Threads.push_back(std::move(Event_Scheduler));
        INTERNAL::Sub_Threads.push_back(std::move(Inquire_Scheduler));

        INTERNAL::Sub_Threads.back().detach();    // the Inquire scheduler cannot never stop and thus needs to be as an separate thread.

        INTERNAL::LOGGER::Log("GGUI Core initialization complete.");

        return INTERNAL::Main;
    }

    /**
     * @brief Reports an error to the user.
     * @param Problem The error message to display.
     * @note If the main window is not created yet, the error will be printed to the console.
     * @note This function is thread safe.
     */
    void report(std::string Problem){
        const char* ERROR_LOGGER = "_ERROR_LOGGER_";
        const char* HISTORY = "_HISTORY_";
        try{
            pauseGGUI([&Problem, &ERROR_LOGGER, &HISTORY]{
                INTERNAL::LOGGER::Log(Problem);

                // reportStack is called when the height or width is zero at init, so we dont ned to compute further.
                if (INTERNAL::Max_Height == 0 || INTERNAL::Max_Width == 0){
                    return;
                }

                Problem = " " + Problem + " ";

                // Error logger structure:
                /*
                    <Window name="_ERROR_LOGGER_">
                        <List name="_HISTORY_" type=vertical scrollable=true>
                            <List type="horizontal">
                                <TextField>Time</TextField>
                                <TextField>Problem a</TextField>
                                <TextField>[repetitions if any]</TextField>
                            </List>
                            ...
                        </List>
                    </Window>
                */

                if (INTERNAL::Main && (INTERNAL::Max_Width != 0 && INTERNAL::Max_Height != 0)){
                    bool Create_New_Line = true;

                    // First check if there already is a report log.
                    window* Error_Logger = (window*)INTERNAL::Main->getElement(ERROR_LOGGER);

                    if (Error_Logger){
                        // Get the list
                        scrollView* History = (scrollView*)Error_Logger->getElement(HISTORY);

                        // This happens, when Error logger is kidnapped!
                        if (!History){
                            // Now create the history lister
                            History = new scrollView(styling(
                                width(1.0f) | height(1.0f) |
                                text_color(GGUI::COLOR::RED) | background_color(GGUI::COLOR::BLACK) | 
                                flow_priority(DIRECTION::COLUMN) | name(HISTORY)
                            ));

                            Error_Logger->addChild(History);
                        }

                        std::vector<listView*>& Rows = (std::vector<listView*>&)(History->getContainer()->getChilds()); 

                        if (Rows.size() > 0){
                            //Text_Field* Previous_Date = Rows.back()->Get<Text_Field>(0);
                            textField* Previous_Problem = Rows.back()->get<textField>(0);
                            textField* Previous_Repetitions = Rows.back()->get<textField>(1);

                            //check if the previous problem was same problem
                            if (Previous_Problem->getText() == Problem){
                                // increase the repetition count by one
                                if (!Previous_Repetitions){
                                    Previous_Repetitions = new textField(styling(text("2")));
                                    Rows.back()->addChild(Previous_Repetitions);
                                }
                                else{
                                    // translate the string to int
                                    int Repetition = std::stoi(Previous_Repetitions->getText()) + 1;
                                    Previous_Repetitions->setText(std::to_string(Repetition));
                                }

                                // We dont need to create a new line.
                                Create_New_Line = false;
                            }
                        }
                    }
                    else{
                        // create the error logger
                        Error_Logger = new window(
                            styling(
                                width(0.25f) | height(0.5f) |

                                text_color(GGUI::COLOR::RED) | background_color(GGUI::COLOR::BLACK) |
                                border_color(GGUI::COLOR::RED) | border_background_color(GGUI::COLOR::BLACK) | 

                                title("LOG") | name(ERROR_LOGGER) | 
                                
                                position(
                                    STYLES::center + STYLES::prioritize
                                ) | 
                                
                                enable_border(true) | allow_overflow(true) | 

                                node(new scrollView(styling(
                                    width(1.0f) | height(1.0f) |
                                    text_color(GGUI::COLOR::RED) | background_color(GGUI::COLOR::BLACK) | 
                                    flow_priority(DIRECTION::COLUMN) | name(HISTORY)
                                )))
                            )
                        );

                        INTERNAL::Main->addChild(Error_Logger);
                    }

                    if (Create_New_Line){
                        // re-find the error_logger.
                        Error_Logger = (window*)INTERNAL::Main->getElement(ERROR_LOGGER);
                        scrollView* History = (scrollView*)Error_Logger->getElement(HISTORY);

                        History->addChild(new listView(styling(
                            width(1.0f) | height(4) | 
                            text_color(GGUI::COLOR::RED) | background_color(GGUI::COLOR::BLACK) | 
                            flow_priority(DIRECTION::ROW) | 

                            name(INTERNAL::now().c_str()) | 

                            // The actual reported problem text
                            node(new textField(styling(
                                text(Problem.c_str())
                            ))) |

                            // The Date field
                            node(new textField(styling(
                                text(INTERNAL::now().c_str())
                            )))
                        )));

                        listView* row = (listView*)History->getContainer()->getChilds().back();
                        row->setHeight(row->getChilds()[1]->getHeight());

                        // Calculate the new x position for the Error_Logger
                        if (Error_Logger->getParent() == INTERNAL::Main)
                            Error_Logger->setPosition({
                                (Error_Logger->getParent()->getWidth() - History->getWidth()) / 2,
                                (Error_Logger->getParent()->getHeight() - History->getHeight()) / 2,
                                POSITION::Max_Z
                            });

                        // check if the Current rows amount makes the list new rows un-visible because of the of-limits.
                        // We can assume that the singular error is at least one tall.
                        if (GGUI::Min(History->getContainer()->getHeight(), (int)History->getContainer()->getChilds().size()) >= Error_Logger->getHeight()){
                            // Since the children are added asynchronously, we can assume the the order of childs list vector represents the actual visual childs.
                            // Element* First_Child = History->Get_Childs()[0];
                            // History->Remove(First_Child);

                            // TODO: Make this into a scroll action and not a remove action, since we want to see the previous errors :)
                            History->scrollDown();
                        }
                    }

                    // If the user has disabled the Inspect_Tool then the errors appear as an popup window ,which disappears after 30s.
                    if (Error_Logger->getParent() == INTERNAL::Main){
                        Error_Logger->display(true);

                        INTERNAL::Remember([Error_Logger](std::vector<Memory>& rememberable){
                            rememberable.push_back(Memory(
                                TIME::SECOND * 30,
                                [Error_Logger](GGUI::Event*){
                                    //delete tmp;
                                    Error_Logger->display(false);
                                    //job successfully done
                                    return true;
                                },
                                MEMORY_FLAGS::PROLONG_MEMORY,
                                "Report Logger Clearer"
                            ));
                        });
                    }

                }
                else{
                    if (!INTERNAL::Platform_Initialized){
                        INTERNAL::initPlatformStuff();
                    }

                    // This is for the non GGUI space errors.
                    UTF _error__tmp_ = UTF("ERROR: ", {COLOR::RED, {}});

                    std::cout << _error__tmp_.To_String() + Problem << std::endl;
                }

            });
        }
        catch (std::exception& e){
            try{
                // First don't give up on local logging yet
                INTERNAL::LOGGER::Log("Problem: " + std::string(e.what()));
            }
            catch (std::exception& f){
                // If logger is also down
                std::cout << "Problem: " << e.what() << std::endl;
            }
        }
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
                    Parent_Y >= C.Y && Parent_X >= C.X &&
                    Parent_Y <= C.Y + (signed)child->getHeight() &&
                    Parent_X <= C.X + (signed)child->getWidth()
                )
                {
                    Parent_Buffer[Parent_Y * (signed)Parent->getWidth() + Parent_X] = Text[i++];
                }
            }
        }
    }

    /**
     * @brief Pauses all other GGUI internal threads and calls the given function.
     * @details This function will pause all other GGUI internal threads and call the given function.
     * @param f The function to call.
     */
    void pauseGGUI(std::function<void()> f){

        // Save the current render status.
        INTERNAL::atomic::status Previous_Render_Status;

        // Make an virtual local scope to temporary own the mutex.
        {
            // Lock the mutex to make sure we are the only one that can change the render status.
            std::unique_lock lock(INTERNAL::atomic::Mutex);

            // Save the current render status.
            Previous_Render_Status = INTERNAL::atomic::Pause_Render_Thread;
        }

        pauseGGUI();

        try{
            // Call the given function.
            f();
        }
        catch(std::exception& e){

            std::string Given_Function_Label_Location = Hex(reinterpret_cast<unsigned long long>(&f));

            // If an exception is thrown, report the stack trace and the exception message.
            INTERNAL::reportStack("In given function to Pause_GGUI: " + Given_Function_Label_Location + " arose problem: " + std::string(e.what()));
        }

        // Resume the render thread with the previous render status.
        resumeGGUI(
            Previous_Render_Status
        );
    }

    /**
     * @brief Use GGUI in a simple way.
     * @details This is a simple way to use GGUI. It will pause all other GGUI internal threads, initialize GGUI, call the given function, sleep for the given amount of milliseconds, and then exit GGUI.
     * @param DOM The function that will add all the elements to the root window.
     * @param Sleep_For The amount of milliseconds to sleep after calling the given function.
     */
    void GGUI(std::function<void()> DOM, unsigned long long Sleep_For){
        INTERNAL::Read_Start_Addresses();

        pauseGGUI([DOM](){
            initGGUI();

            DOM();
        });

        // Since 0.1.8 the Rendering_Paused Atomic value is initialized with PAUSED.
        resumeGGUI();

        INTERNAL::SLEEP(Sleep_For);
        // No need of un-initialization here or forced exit, since on process death the right exit codes will be initiated.
    }

    /**
     * @brief Use GGUI in a simple way.
     * @details This is a simple way to use GGUI. It will pause all other GGUI internal threads, initialize GGUI, add all the elements to the root window, sleep for the given amount of milliseconds, and then exit GGUI.
     * @param DOM The elements to add to the root window.
     * @param Sleep_For The amount of milliseconds to sleep after calling the given function.
     */
    void GGUI(styling App, unsigned long long Sleep_For){
        INTERNAL::Read_Start_Addresses();

        pauseGGUI([&App](){
            initGGUI();

            // Since the App is basically an AST Styling, we first add it to the already constructed main with its width and height set to the terminal sizes.
            INTERNAL::Main->addStyling(App);

            // Now recursively go down in the App AST nodes and Build each node.
            INTERNAL::Main->embedStyles();

            // Now we can safely insert addons while taking into notion user configured borders and other factors which may impact the usable width.
            initAddons();
        });

        // Since 0.1.8 the Rendering_Paused Atomic value is initialized with PAUSED.
        resumeGGUI();

        // TODO: remove this:
        updateFrame();

        // Sleep for the given amount of milliseconds.
        INTERNAL::SLEEP(Sleep_For);
    }

    /**
     * @brief Encodes a buffer of UTF elements by setting start and end flags based on color changes.
     * 
     * @param Buffer A vector of UTF elements to be encoded.
     * @details The function marks the beginning and end of color strips within the buffer. 
     *          It checks each UTF element's foreground and background colors with its adjacent elements
     *          to determine where encoding strips start and end.
     */
    void encodeBuffer(std::vector<GGUI::UTF>& Buffer) {
        
        // Initialize the first and last elements with start and end flags respectively.
        Buffer[0].Set_Flag(UTF_FLAG::ENCODE_START);
        Buffer[Buffer.size() - 1].Set_Flag(UTF_FLAG::ENCODE_END);

        // Iterate through the buffer to determine encoding strip positions.
        for (unsigned int Index = 1; Index < Buffer.size() - 1; Index++) {

            bool Same_Colours_As_Previous = Buffer[Index].Background == Buffer[Index - 1].Background &&
                                            Buffer[Index].Foreground == Buffer[Index - 1].Foreground;
            bool Same_Colours_As_Next = Buffer[Index].Background == Buffer[Index + 1].Background &&
                                        Buffer[Index].Foreground == Buffer[Index + 1].Foreground;

            // Set end flag if current colors differ from the next.
            if (!Same_Colours_As_Next) {
                Buffer[Index].Set_Flag(UTF_FLAG::ENCODE_END);
            }

            // Set start flag if current colors differ from the previous.
            if (!Same_Colours_As_Previous) {
                Buffer[Index].Set_Flag(UTF_FLAG::ENCODE_START);
            }
        }

        // Ensure the last element is marked correctly if the second to last was an ending node.
        if (Buffer[Buffer.size() - 2].Is(UTF_FLAG::ENCODE_END)) {
            Buffer[Buffer.size() - 1].Set_Flag(UTF_FLAG::ENCODE_START | UTF_FLAG::ENCODE_END);
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
    void informAllGlobalBufferCaptures(INTERNAL::bufferCapture* informer){

        // Iterate over all global buffer capturers
        for (auto* capturer : INTERNAL::Global_Buffer_Captures){
            if (!capturer->Is_Global)
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
     * @brief Retrieves an element by name.
     * @details This function takes a string argument representing the name of the element
     *          and returns a pointer to the element if it exists in the global Element_Names map.
     * @param name The name of the element to retrieve.
     * @return A pointer to the element if it exists; otherwise, nullptr.
     */
    element* getElement(std::string name){
        element* Result = nullptr;

        // Check if the element is in the global Element_Names map.
        if (INTERNAL::Element_Names.find(name) != INTERNAL::Element_Names.end()){
            // If the element exists, assign it to the result.
            Result = INTERNAL::Element_Names[name];
        }

        // Return the result.
        return Result;
    }
}
