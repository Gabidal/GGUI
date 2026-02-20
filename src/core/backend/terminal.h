#ifndef _TERMINAL_H_
#define _TERMINAL_H_

#include <functional>
#include <cstdint>
#include <chrono>

#include "ascii.h"
#include "../utils/types.h"

namespace GGUI {
    /**
     * Contains the interface for terminal handling in terminal emulators.
    */
    namespace terminal {
        enum class types : uint32_t {
            PLAIN       = 0 << 0,               // No sequence codes stylings or colors, just simple plain text
            VT05        = 0 << 0,               // Too limited, no support
            VT52        = 0 << 0,               // Too limited, no support
            VT55        = 0 << 0,               // Too limited, no support
            TTY         = 1 << 0,               // If zero, then probably in a file or pipe, otherwise interactive terminal
            ANSI        = 1 << 1,               // Supports sequences but not known DEC terminal, based on: X3.41-1974 and X3.64-1977
            VT100       = 1 << 2,               // Base lowest DEC terminal, with support for some private SGR sequences
            VT102       = 1 << 3,               // Insert/Delete line/character support
            VT220       = 1 << 4,               // 
        };
        extern types type; 

        extern INTERNAL::bitMask<types> features;

        struct query {
            // Some compile time constants; 510, is enough. If need raise this.
            static const unsigned int capacity = UINT8_MAX * 2;

            std::array<unsigned char, capacity> buffer;
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

            struct {
                button left, right, middle;
                wheel scroll;               // Only for vertical
                IVector2 position;          // Absolute
            } mouse;

            std::array<button, (size_t)ASCII::table::MAX_VALUE> keyboard;
        };

        // Read from this to get current device states of the terminal peripherals.
        extern device currentStates;
        // extern device previousStates;    // Only accessible inside the terminal.cpp for internal use only!

        extern void init();                 // Platform Specific
        
        extern void startProbing();         // Terminal Specific

        extern void deinit();               // Terminal Specific
        
        extern void waitForInput();         // Platform Specific

        extern void parseInput();           // Terminal Specific

        // This is a general function whose job is to post GGUI input events for event handlers to catch.
        extern void postInputs();           // General
    
    }
}


#endif