#ifdef _WIN32
#include "terminal.h"

#include <windows.h>
#include <dbghelp.h>
#undef min
#undef max
#undef small

/**
 * Contains Windows input implementation of input interface
*/

namespace GGUI {
    namespace terminal {
        HANDLE GLOBAL_STD_OUTPUT_HANDLE;
        HANDLE GLOBAL_STD_INPUT_HANDLE;

        DWORD PREVIOUS_CONSOLE_OUTPUT_STATE;
        DWORD PREVIOUS_CONSOLE_INPUT_STATE;

        CONSOLE_SCREEN_BUFFER_INFO Get_Console_Info();

        DWORD toInputMode(const INTERNAL::configuration& cfg) {
            DWORD mode = ENABLE_MOUSE_INPUT | ENABLE_WINDOW_INPUT;

            // KAM ENABLED (RESET) -> ENABLE_PROCESSED_INPUT (Ctrl+C handled)
            if (cfg.modes.get(mode::types::KEYBOARD_ACTION_MODE) == mode::definition::RESET)
                mode |= ENABLE_PROCESSED_INPUT;

            // CRM CONTROL (RESET) -> ENABLE_LINE_INPUT (canonical / cooked)
            if (cfg.modes.get(mode::types::CONTROL_REPRESENTATION_MODE) == mode::definition::RESET)
                mode |= ENABLE_LINE_INPUT;

            // SRM MONITOR (RESET) -> ENABLE_ECHO_INPUT
            if (cfg.modes.get(mode::types::SEND_RECEIVE_MODE) == mode::definition::RESET)
                mode |= ENABLE_ECHO_INPUT;

            return mode;
        }

        DWORD toOutputMode(const INTERNAL::configuration& cfg) {
            DWORD mode = ENABLE_PROCESSED_OUTPUT
                    | ENABLE_VIRTUAL_TERMINAL_PROCESSING;  // ANSI/VT sequences, Win10 1511+

            // ONLCR -> ENABLE_WRAP_AT_EOL_OUTPUT
            if (cfg.oflag & terminal::oflag::ONLCR)
                mode |= ENABLE_WRAP_AT_EOL_OUTPUT;

            return mode;
        }

        bool snapshot(INTERNAL::configuration& cfg) {
            DWORD inputMode = 0;
            if (!GetConsoleMode(GetStdHandle(STD_INPUT_HANDLE), &inputMode))
                return false;

            cfg.modes.set(ModeBase{
                mode::types::KEYBOARD_ACTION_MODE,
                (inputMode & ENABLE_PROCESSED_INPUT) ? mode::definition::RESET : mode::definition::SET
            });
            cfg.modes.set(ModeBase{
                mode::types::CONTROL_REPRESENTATION_MODE,
                (inputMode & ENABLE_LINE_INPUT) ? mode::definition::RESET : mode::definition::SET
            });
            cfg.modes.set(ModeBase{
                mode::types::SEND_RECEIVE_MODE,
                (inputMode & ENABLE_ECHO_INPUT) ? mode::definition::RESET : mode::definition::SET
            });

            return true;
        }

        bool apply(const INTERNAL::configuration& cfg) {
            return SetConsoleMode(GLOBAL_STD_INPUT_HANDLE,  toInputMode(cfg))
                && SetConsoleMode(GLOBAL_STD_OUTPUT_HANDLE, toOutputMode(cfg));
        }

        bitMask<features> fetchIOPermissions() {// TODO: ...}

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

        // Windows implementation of the terminal::platformInit
        void platformInit() {   // TODO: move this into the windows side terminal::INTERNAL::configuration constructor
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
            platformState.previousWindowsCodepage = GetConsoleOutputCP();
            if (platformState.previousWindowsCodepage != (unsigned long)ENABLE_UTF8_MODE_FOR_WINDOWS)
                SetConsoleOutputCP(ENABLE_UTF8_MODE_FOR_WINDOWS);

            // Enable specific ANSI features for mouse event reporting and hide the mouse cursor.
            if (STDIN_IS_TTY){
                initTerminalWithANSICodes();
            }
            
            std::cout << std::flush;
        }
        
        void platformDeinit() {
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
            if (platformState.previousWindowsCodepage != 0){
                SetConsoleOutputCP(platformState.previousWindowsCodepage);
            }
        };

        void waitForInput() {
            // Read the console input and store it in inputQuery.
            ReadConsoleInput(
                GLOBAL_STD_INPUT_HANDLE,
                inputQuery.buffer.begin(),
                inputQuery.capacity,
                (LPDWORD)&inputQuery.size
            );
        }
    }
}

#endif