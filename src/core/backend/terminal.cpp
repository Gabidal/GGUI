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

        device currentStates;
        device previousStates;  // Used for to time how long buttons are held down for.
        query queue;
        bitMask<features> enabledFeatures;

        size_t device::outputCapture::getActiveIndex() const {
            return (cursor.y * dimensions.x) + cursor.x;
        }

        std::string device::outputCapture::toString() const {
            return "(" + cursor.To_String() + ")";
        }

        void init() {
            // The following code is for nominal use of GGUI via Unix/Windows terminal emulators. TODO: enable direct /dev/ ral Terminal device contact.
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

            if (!snapshot()) {     // Load checkpoint
                GGUI::INTERNAL::LOGGER::log("ERROR: Failed to snapshot terminal configuration!");
                return;
            } else if (!apply()) {  // Apply preferences
                GGUI::INTERNAL::LOGGER::log("ERROR: Failed to apply terminal configuration!");
                return;
            }
        }

        extern void platformDeinit();
        void deinit() {
            // Calls based on feature flags correct public and/or private SGR or other extension CSI's.

            if (enabledFeatures.has(features::TTY)) {

            }

            // Now cal platform specific de-initializers
            platformDeinit();
        }

        // Helper function to automatically check whether the enum does not belong to the activates set.
        template<typename T, typename... Vs> bool contains(const std::variant<Vs...>& v, T value) {
            // static_assert that T is actually one of the variant's types
            static_assert((std::is_same_v<T, Vs> || ...),  "T is not a member of this variant");
            const auto* p = std::get_if<T>(&v);
            return p && *p == value;
        }

        void parseInput() {
            // Parses input based on modular features, each brought by their own respective flag.

            // If special loaders needed to be present they better have been initialized properly at initialization phase when the handshake/probing happens.
            for (auto sequence : ecma::sequence::parse(std::string_view(queue.inputBuffer.data(), queue.inputSize))) {

                // This is likely redundant, since all operations have their own handler to process the functionality of the specific operation
                switch (sequence->getType()) {
                    default:
                        break;
                }

            }
        }

        void postInputs() {
            // Reads terminal::device::currentState and posts input events for GGUI event handlers to handle. 
        }
    }
}