#include "terminal.h"
#include "dec.h"

/**
 * Cross platform functions and containers are held here:
*/

namespace GGUI {
    namespace terminal {

        namespace ecma {
            extern bitMask<features> probe();
        }

        namespace dec {
            extern bitMask<features> probe();
        }

        namespace INTERNAL {

            configuration::configuration() {
                // Hardware input: translate CR->NL, enable UTF-8
                iflag = bitMask<inputFlags>(inputFlags::TRANSLATE_CR_TO_NL_ON_INPUT) | inputFlags::INPUT_IS_UTF8;

                // Hardware output: translate NL->CR+NL (standard cooked output)
                oflag = bitMask<outputFlags>(outputFlags::ENABLE_ALL_POSTPROCESSING) | outputFlags::TRANSLATE_NEWLINE_TO_CARRIAGE_RETURN_NEWLINE;

                // Hardware line: 8-bit chars, enable receiver, ignore modem lines
                cflag = bitMask<hardwareControlFlags>(hardwareControlFlags::CHARACTER_SIZE_8_DATA_BITS) | hardwareControlFlags::CHARACTER_READ_ENABLE | hardwareControlFlags::CONTROL_LOCAL_IGNORE_MODEM_STATUS_LINES;

                modes.set(ecma::table::mode::presets::CRM_CONTROL);      // control chars processed (not treated as graphic)
                modes.set(ecma::table::mode::presets::SRM_MONITOR);      // echo on (locally entered data is imaged)
                modes.set(ecma::table::mode::presets::KAM_ENABLED);      // keyboard active (Ctrl+C still generates SIGINT)

                cc_chars[(size_t)specialCharacterIndicies::NON_CANONICAL_MINIMUM_BYTES_BEFORE_READ]  = 1;      // return after 1 byte
                cc_chars[(size_t)specialCharacterIndicies::NON_CANONICAL_READ_TIMEOUT] = 0;      // no timeout

                ispeed = hardwareControlFlags::BAUD_9600;
                ospeed = hardwareControlFlags::BAUD_9600;
            }
        }
        
        device currentStates;
        device previousStates;  // Used for to time how long buttons are held down for.
        query inputQuery;
        bitMask<features> enabledFeatures;
        INTERNAL::configuration currentConfiguration;
        INTERNAL::configuration previousConfiguration;

        void init() {
            enabledFeatures = fetchIOPermissions();

            // Check what permissions we have
            if (enabledFeatures.get() == features::NONE) {
                GGUI::INTERNAL::LOGGER::log("ERROR: No terminal features detected!");
                return;
            } 

            if (!enabledFeatures.has(features::READ)) {
                GGUI::INTERNAL::LOGGER::log("WARNING: No read permissions detected!");
            } else if (!enabledFeatures.has(features::WRITE)) {
                GGUI::INTERNAL::LOGGER::log("WARNING: No write permissions detected!");
            }

            if (!enabledFeatures.has(features::TTY)) {
                GGUI::INTERNAL::LOGGER::log("INFO: Non-interactive mode detected.");
            }

            // Clear configurations for safety:
            previousConfiguration = INTERNAL::configuration();
            currentConfiguration = INTERNAL::configuration();

            if (!snapshot(previousConfiguration)) {     // Load checkpoint
                GGUI::INTERNAL::LOGGER::log("ERROR: Failed to snapshot terminal configuration!");
                return;
            } else if (!apply(currentConfiguration)) {  // Apply preferences
                GGUI::INTERNAL::LOGGER::log("ERROR: Failed to apply terminal configuration!");
                return;
            }

            startProbing();
        }

        // Probes what ECMA and private DEC features are available
        void startProbing() {
            enabledFeatures |= ecma::probe();
            enabledFeatures |= dec::probe();
            // ...
        }

        extern void platformDeinit();
        void deinit() {
            // Calls based on feature flags correct public and/or private SGR or other extension CSI's.

            if (enabledFeatures.has(features::TTY)) {

            }

            // Now cal platform specific de-initializers
            platformDeinit();
        }

        void parseInput() {
            // Parses input based on modular features, each brought by their own respective flag.
        }

        void postInputs() {
            // Reads terminal::device::currentState and posts input events for GGUI event handlers to handle. 
        }
    }
}