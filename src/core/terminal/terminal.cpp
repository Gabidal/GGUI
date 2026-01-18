#include "terminal.h"
#include "ansi.h"
#include "dec.h"

/**
 * Cross platform functions and containers are held here:
*/

namespace GGUI {
    namespace terminal {
        types type;
        device currentStates;
        device previousStates;  // Used for to time how long buttons are held down for.
        INTERNAL::bitMask<feature> features;
        query inputQuery;

        void startProbing() {
            features |= ansi::probe();
            features |= dec::probe();
        }

        void deinit() {
            // Calls based on feature flags correct public and/or private SGR or other extension CSI's.
        }

        void parseInput() {
            // Parses input based on modular features, each brought by their own respective flag.
        }

        void postInputs() {
            // Reads terminal::device::currentState and posts input events for GGUI event handlers to handle. 
        }
    }
}