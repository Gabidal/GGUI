#include "terminal.h"
#include "../../elements/element.h"

/**
 * Cross platform functions and containers are held here:
*/

namespace GGUI {
    namespace terminal {
        base* currentStates = nullptr;

        size_t outputCapture::getActiveIndex() const {
            return (cursor.y * dimensions.x) + cursor.x;
        }

        void outputCapture::link(element* DOM) {
            dom = DOM;
        }

        void outputCapture::update() {
            buffer = dom->render();
        }

        void init() {
            currentStates = new base();

            platformInit();
        }

        extern void platformDeinit();
        void deinit() {
            // Calls based on feature flags correct public and/or private SGR or other extension CSI's.

            // Now cal platform specific de-initializers
            platformDeinit();
        }

        void parseInput() {
            // Parses input based on modular features, each brought by their own respective flag.

            // If special loaders needed to be present they better have been initialized properly at initialization phase when the handshake/probing happens.
            for (auto sequence : ecma::sequence::parse(std::string_view(currentStates->transmission.inputBuffer.data(), currentStates->transmission.inputSize))) {

                // This is likely redundant, since all operations have their own handler to process the functionality of the specific operation
                switch (sequence->getType()) {
                    default:
                        break;
                }

            }
        }
    }
}