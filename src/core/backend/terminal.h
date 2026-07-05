#ifndef _TERMINAL_H_
#define _TERMINAL_H_

#include <cstdint>

// Modules:
#include "ecma.h"
#include "dec.h"
// -----

#include "../utils/types.h"
#include "../utils/superString.h"

namespace GGUI {
    // ggui::terminal's job is to be the bridge between the serial/device and platform specificity and the standard ecma/dec/xterm protocol
    namespace terminal {
        using keyListing = std::array<key, (size_t)ecma::table::getSize<key::types>()>;

        struct base {
            using compactString = INTERNAL::compactString;

            keyListing keys;
            
            ecma::components ecmaComponents;
            dec::components decComponents;

            RGB colorIndexMap[UINT8_MAX] = {};

            class outputCapture{
            protected:
                std::string* liquefiedBuffer = nullptr;      // This is what send back into the output device to be rendered into the screen.       
                std::vector<ecma::graphicAttributes>& registeredGraphicAttributes;   // This is the metadata of the presendted buffer.
            public:
                // These are public, since these are used by ecma and others to manipulate presentation.
                IVector2& cursor;
                IVector2 dimensions;
                std::vector<compactString>* buffer;  // This is what the terminal::render(main) gives us, this is different from the output, because of unicode strings, which would break the activePresentationPointer, since some unicodes can be longer than one index.
                
                outputCapture(IVector2& presentationPosition, std::vector<ecma::graphicAttributes>& RGA) : registeredGraphicAttributes(RGA), cursor(presentationPosition) {}

                size_t getActiveIndex() const;

                // Displays cursor position
                std::string toString() const;
            } screen = outputCapture(ecmaComponents.activePresentationPosition, ecmaComponents.registeredGraphicAttributes);

            struct query {
                // Some compile time constants; 510, is enough. If need raise this.
                static constexpr unsigned int capacity = UINT8_MAX * 2;

                std::array<char, capacity> inputBuffer;     // This is what we receive
                unsigned int inputSize = 0;
                
                std::array<char, capacity> responseBuffer;    // This is what we send (only for non-visual responses)
                unsigned int responseSize = 0;

                template<typename containerType>
                void addToQueue(containerType& input) {
                    if (input.size() + responseSize > capacity) {
                        GGUI::INTERNAL::LOGGER::log("ERROR: Output queue clogged!");
                        return;
                    }

                    std::copy(input.begin(), input.end(), responseBuffer.begin() + responseSize);
                    responseSize += input.size();
                }
            } transmission;
        };

        // Read from this to get current device states of the terminal peripherals.
        extern base currentStates;

        extern void init();                 // non-Platform Specific

        extern void platformInit();         // Platform Specific

        extern void deinit();               // Terminal Specific

        extern void queryInputs();          // Platform Specific

        extern void queryResponse();        // Platform Specific

        extern void renderFrame();          // Platform Specific
        
        extern void parseInput();           // Terminal Specific
    }
}


#endif