#ifndef _TERMINAL_H_
#define _TERMINAL_H_

#include <cstdint>

// Modules:
#include "ecma.h"
#include "dec.h"
// -----

#include "../utils/types.h"
#include "../utils/superString.h"
#include "../utils/style.h"

namespace GGUI {
    namespace INTERNAL {
        void renderer();
    }

    struct ActiveStyle;

    class element;

    // ggui::terminal's job is to be the bridge between the serial/device and platform specificity and the standard ecma/dec/xterm protocol
    namespace terminal {
        using keyListing = std::array<key, (size_t)ecma::table::getSize<key::types>()>;
        using compactString = INTERNAL::compactString;

        class outputCapture{
        protected:
            IVector2& cursor;
            IVector2& dimensions;
            std::vector<compactString>& buffer;                 // This is what the terminal::render(main) gives us, this is different from the output, because of unicode strings, which would break the activePresentationPointer, since some unicodes can be longer than one index.
            std::string* liquefiedBuffer = nullptr;             // This is what send back into the output device to be rendered into the screen.       
            std::vector<ecma::activeSGRStyle>& activeGraphicAttributes;     // this is the liquefied graphic area metadata which is ecma-48 compatible.
            element* dom;       // the primary element tree to be rendered.
        public:
            outputCapture(
                IVector2& presentationPosition,
                IVector2& presentationDimension,
                std::vector<compactString>& activeBuffer,
                std::vector<ecma::activeSGRStyle>& RGA
            ) : cursor(presentationPosition), dimensions(presentationDimension), buffer(activeBuffer), activeGraphicAttributes(RGA) {}

            size_t getActiveIndex() const;

            // === Render pipeline  ===
            void update();      // updates DOM
            void link(element* DOM);
            void computeSGRAreas();
            void preparePresentationBuffer();
        protected:
            std::pair<bool, ActiveStyle> trace(IVector2 point, element* currentContainer);
        public:
            // ===                  ===
            
            friend void renderFrame();
            friend void INTERNAL::renderer();
        };

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
        };

        class base {
        public:
            keyListing keys;
            
            ecma::components ecmaComponents;
            dec::components decComponents;

            RGB colorIndexMap[UINT8_MAX] = {};  // Used for custom color indicies for SGR.

            outputCapture screen = outputCapture(
                ecmaComponents.activePresentationPosition, 
                ecmaComponents.activeScreenDimensions,
                ecmaComponents.activePresentationBuffer, 
                ecmaComponents.registeredGraphicAttributes
            );

            query transmission;

            base() = default;
        };

        // Read from this to get current device states of the terminal peripherals.
        extern base* currentStates;

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