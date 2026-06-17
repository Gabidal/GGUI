#ifndef _TERMINAL_H_
#define _TERMINAL_H_

#include <cstdint>

// Modules:
#include "ecma.h"
#include "dec.h"
// -----

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
            static constexpr unsigned int capacity = UINT8_MAX * 2;

            std::array<char, capacity> inputBuffer;     // This is what we receive
            unsigned int inputSize = 0;
            
            std::array<char, capacity> outputBuffer;    // This is what we send (only for input query uses, like protocol and such...)
            unsigned int outputSize = 0;

            template<typename containerType>
            void addToQueue(containerType& input) {
                if (input.size() + outputSize > capacity) {
                    GGUI::INTERNAL::LOGGER::log("ERROR: Output queue clogged!");
                    return;
                }

                std::copy(input.begin(), input.end(), outputBuffer.begin() + outputSize);
                outputSize += input.size();
            }
        };
        extern query queue;
        
        struct device {
            std::array<key, (size_t)ecma::table::getSize<key::types>()> keys;

            ecma::components ecmaComponents;
            dec::components decComponents;

            struct outputCapture{
                IVector2& cursor;
                IVector2 dimensions;
                std::vector<UTF>* cellBuffer = nullptr;

                outputCapture(IVector2& presentationPosition) : cursor(presentationPosition) {}

                size_t getActiveIndex() const;

                // Displays cursor position
                std::string toString() const;
            } screen = outputCapture(ecmaComponents.activePresentationPosition);
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

        extern void queryInputs();          // Platform Specific

        extern void queryOutput();          // Platform Specific

        extern void parseInput();           // Terminal Specific

        // This is a general function whose job is to post GGUI input events for event handlers to catch.
        extern void postInputs();           // General
    
    }
}


#endif