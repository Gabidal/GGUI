#ifndef _TERMINAL_H_
#define _TERMINAL_H_

#include <cstdint>
#include <condition_variable>
#include <mutex>
#include <array>
#include <vector>
#include <string>

// Modules:
#include "ecma.h"
#include "dec.h"
// -----

#include "../converter.h"
#include "../utils/types.h"
#include "../utils/style.h"

namespace GGUI {
    namespace thread {
        void renderer();
    }

    struct activeStyle;

    class element;

    // ggui::terminal's job is to be the bridge between the serial/device and platform specificity and the standard ecma/dec/xterm protocol
    namespace terminal {
        extern void init(converter::input::base* inputPoller);                 // non-Platform Specific
        
        extern void deinit();               // non-platform Specific

        extern void setGlyphWidth(cell&);

        // Links into different components and their properties to update and keep track of them while rendering
        class outputCapture {
        protected:
            IVector2& cursor;
            IVector2& dimensions;
            std::vector<terminal::cell>& buffer;                 // This is what the terminal::render(main) gives us, this is different from the output, because of unicode strings, which would break the activePresentationPointer, since some unicodes can be longer than one index.
            std::string* liquefiedBuffer = nullptr;             // This is what send back into the output device to be rendered into the screen.       
            std::vector<ecma::activeSGRStyle>& activeGraphicAttributes;     // this is the liquefied graphic area metadata which is ecma-48 compatible.
            element* dom = nullptr;       // the primary element tree to be rendered.
        public:
            outputCapture(
                IVector2& presentationPosition,
                IVector2& presentationDimension,
                std::vector<terminal::cell>& activeBuffer,
                std::vector<ecma::activeSGRStyle>& RGA
            ) : cursor(presentationPosition), dimensions(presentationDimension), buffer(activeBuffer), activeGraphicAttributes(RGA) {}

            // === Render pipeline  ===
            
            // Calls getRoot()->render()
            void update();

            // Setups what element to read from
            void link(element* DOM);

            // Compute SGR areas from linked element
            void computeSGRAreas();

            // Combines computed traced SGR areas into buffer with text from linked element::render()
            void preparePresentationBuffer();

            // Sends the combined presentation buffer into cross-platform outputing device
            void renderBuffer();

            // === Helpers ===
            IVector2 getDimensions() const;
            IVector2 getCursor() const;
            
            // Returns position as buffer index
            size_t getIndexOf(IVector2) const;
        protected:
            // Ray-tracing inspired SGR baker
            std::pair<bool, activeStyle> trace(IVector2 point, element* currentContainer);
        public:
            // ===                  ===
            
            friend void thread::renderer();
            friend void updateScreenDimensions();
        };

        struct query {
            // Some compile time constants; 510, is enough. If need raise this.
            static constexpr unsigned int capacity = UINT8_MAX * 2;

            std::array<char, capacity> inputBuffer;     // This is what we receive
            unsigned int inputSize = 0;

            // Outside reads from here
            std::vector<ecma::sequence::base*> parsedInputBuffer;

            // mutex for waiting input
            std::mutex mutex;
            std::condition_variable condition;
            enum class status : uint8_t {
                NONE,
                CONNECTED,      // Check if above this state for approved polling/writing to I/O
                SENDING,
                RECEIVING
            } state = status::NONE;

            query() = default;

            // Use this to check connection status
            bool isConnected();

            // Used internally on initialization to confirm cross-platform acknowledgement of connection success
            bool acknowledgeConnection();

            // Cross-platform output writer
            void addToQueue(std::string_view input);

            // Cross-platform input poller
            void pollInput();

            // Simple helper called from base::parseInput
            void informParsedInput();

            // Activates when the polled is received and parsed
            bool waitForInput();
        };

        class base {
        public:
            ecma::components ecmaComponents;
            dec::components decComponents;

            converter::input::base* keyRegistry = nullptr;    // This is where we will be outputting polled data into.

            RGB colorIndexMap[UINT8_MAX] = {};  // Used for custom color indicies for SGR.

            outputCapture screen = outputCapture(
                ecmaComponents.activePresentationPosition, 
                ecmaComponents.activeScreenDimensions,
                ecmaComponents.activePresentationBuffer, 
                ecmaComponents.registeredGraphicAttributes
            );

            query transmission;

            base(converter::input::base* reg) : keyRegistry(reg) {}

            // Used in converter::input thread to read incoming buffer and parses them. Parsed sequences are stored inside base::transmission.
            void parseInput();

            // Checks for terminal extensions and enables them if available.
            void enableExtensions();
        };

        // Read from this to get current device states of the terminal peripherals.
        extern base* currentStates;
    }
}


#endif