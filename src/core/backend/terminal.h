#ifndef _TERMINAL_H_
#define _TERMINAL_H_

#include <functional>
#include <cstdint>
#include <chrono>

#include "ascii.h"
#include "ecma.h"
#include "../utils/types.h"
#include "../utils/utf.h"

namespace GGUI {
    /**
     * Contains the interface for terminal handling in terminal emulators.
    */
    namespace terminal {

        template<typename T> using bitMask = GGUI::INTERNAL::bitMask<T>;

        enum class features : uint16_t {
            NONE                = 0,
            READ                = 1 << 0,
            WRITE               = 1 << 1,

            TTY                 = 1 << 2,

            PIPED_IN            = 1 << 3,
            PIPED_OUT           = 1 << 4,
            
            REDIRECTED_IN       = 1 << 5,
            REDIRECTED_OUT      = 1 << 6,
        };

        extern GGUI::INTERNAL::bitMask<features> enabledFeatures;

        struct query {
            // Some compile time constants; 510, is enough. If need raise this.
            static const unsigned int capacity = UINT8_MAX * 2;

            std::array<char, capacity> buffer;
            unsigned int size = 0;
        };
        extern query inputQuery;
        
        struct device {
            // Namespace like structuring of code, because why not :)
            struct button {
                // Simple value + time snapshot
                bool state; std::chrono::steady_clock::time_point captureTime;
                // Capture time as creation
                button(bool State = false) : state(State), captureTime(std::chrono::steady_clock::now()) {}
            };

            struct wheel {
                signed char Scalar = 0;     // going up > 0 | going down < 0
            };

            /**
             * NOTE: This component is not part of the normal ECMA-48 nor DEC-VTxxx.
             * Mouse position reporting comes from XTerm, the xterm.cpp module will be responsible to maintain these values of this struct.
             * The reason why this struct is here in terminal.h instead of xterm.cpp, is because of GGDirect to be able to also send and maintain mouse position regardless of xterm support.
             */
            struct {
                button left, right, middle;
                wheel scroll;               // Only for vertical
                IVector2 position;          // Absolute
            } mouse;

            std::array<button, (size_t)ASCII::table::MAX_VALUE> keyboard;

            struct {
                IVector2 cursor;
                IVector2 dimensions;
                std::vector<UTF>* cellBuffer = nullptr;
            } screen;

            ecma::components ecmaComponents;
        };

        // Read from this to get current device states of the terminal peripherals.
        extern device currentStates;
        // extern device previousStates;    // Only accessible inside the terminal.cpp for internal use only!

        extern bitMask<features> fetchIOPermissions();

        // Represents a platform specific terminal driver configuration fetcher into the specific platform state variables.
        extern bool snapshot();             // Platform specific
        
        // Represents a platform specific terminal driver configuration saver from the specific platform state variables.
        extern bool apply();                // Platform specific

        extern void init();                 // non-Platform Specific

        extern void deinit();               // Terminal Specific

        extern void queryInputs();         // Platform Specific

        extern void parseInput();           // Terminal Specific

        // This is a general function whose job is to post GGUI input events for event handlers to catch.
        extern void postInputs();           // General
    
    }
}


#endif