#ifndef _CONVERTER_H_
#define _CONVERTER_H_

#include <cstdint>
#include <chrono>
#include <array>
#include <thread>
#include <functional>

#include "utils/utils.h"

/**
 * @brief Combines the terminal and DRM backend input polling and converts them into GGUI events.
 */
namespace GGUI::converter {
    namespace output {
        struct base;
    }

    namespace input {
        class alignas(16) key {
            bool state = false;
            std::chrono::steady_clock::time_point captureTime = std::chrono::steady_clock::now();
            
        public:
            enum class types : uint8_t {
                __min                   = 32,

                SPACE                   = 32,
                __min_LETTERS           = 32,

                // Visible characters
                // ...
                // - END
                
                __max_LETTERS           = 126,
                DELETE                  = 127,

                ANY_LETTERS,     // Used when wanting to gather all visible characters.

                INSERT,

                SHIFT,
                CTRL,
                SUPER,
                ALT,
                ALTGR,
                FN,
                ESCAPE,
                TABULATOR,
                ENTER,
                BACKSPACE,

                F1,
                F2,
                F3,
                F4,
                F5,
                F6,
                F7,
                F8,
                F9,
                F10,
                F11,
                F12,

                ARROW_UP,
                ARROW_DOWN,
                ARROW_LEFT,
                ARROW_RIGHT,

                HOME,
                END,

                PAGE_UP,
                PAGE_DOWN,

                LEFT_CLICK,
                MIDDLE_CLICK,
                RIGHT_CLICK,

                DRAGGING,

                SCROLL_UP,
                SCROLL_DOWN,

                __max,
            };
        
            bool getState() const { return state; }
            std::chrono::steady_clock::time_point getCaptureTime() const { return captureTime; }

            // Force capture time
            void setState(bool newState) {
                state = newState;
                captureTime = std::chrono::steady_clock::now();
            }
        };

        using allKeys = std::array<key, table::getSize<key::types>()>;

        /**
            * @brief Contains the incoming key information
            * NOTE: this information is shared, so fetch this information from the correct backend interface!
            */
        struct alignas(128) base {
            output::base* out = nullptr;

            // Terminal modules will write into these. Afterwards read from output converter.
            allKeys currentKeyboardState = {};
            allKeys previousKeyboardState = {};     // used to compare changed states.

            std::thread pollingThread;

            base();

            void inputThread();
        };
    }

    namespace output {
        namespace event {
            using selectedKeys = std::vector<input::key::types>;

            struct alignas(32) base {
                selectedKeys criteria;

                base(selectedKeys keys = {}) : criteria(std::move(keys)) {}

                bool has(input::key::types t) const {
                    for (const auto& key : criteria){
                        if (key == t) return true;
                    }
                    return false;
                }

                bool contains(const base* other) const {
                    for (const auto key : other->criteria){
                        // When this accepts any letters, as long as the incoming key is within the letter space we can accept
                        if (has(input::key::types::ANY_LETTERS) && table::contains<input::key::types, input::key::types::__min_LETTERS, input::key::types::__max_LETTERS>(key)) continue;

                        if (!has(key)) return false;
                    }
                    return true;
                }

                selectedKeys getDifference(const base* other) const {
                    selectedKeys difference;

                    for (const auto& key : criteria){
                        if (!other->has(key)) difference.push_back(key);
                    }

                    // filter out any letters if we have ANY_LETTERS capture enabled
                    if (has(input::key::types::ANY_LETTERS)) {
                        difference.erase(std::remove_if(difference.begin(), difference.end(), [](input::key::types key) {
                            return table::contains<input::key::types, input::key::types::__min_LETTERS, input::key::types::__max_LETTERS>(key);
                        }), difference.end());
                    }

                    return difference;
                }
            };

            struct alignas(32) action : public base {
                enum class responses : uint8_t {
                    FAILED,     // Failed job, will not remove the event data from others to have
                    DONE,       // job completed and owning of data, will be removed, other handlers will not get this data
                    PROPAGATE,  // job completed not owning data will give data to next recipient
                };

                std::string ID;
                std::function<responses(event::base*)> job;
            
                action(const selectedKeys& Criteria, std::function<responses(event::base*)> Job, std::string id) : ID(std::move(id)), job(std::move(Job)) {
                    criteria = Criteria;
                }
            };

            struct alignas(64) memory : public action {
                enum class types : uint8_t {
                    NONE               = 0 << 0,
                    PROLONG_MEMORY     = 1 << 0,
                    RETRIGGER          = 1 << 1
                } flags = types::NONE;

                std::chrono::steady_clock::time_point startTime;
                std::chrono::steady_clock::duration endTime;

                // When the job starts, job, prolong previous similar job by this time.
                memory(std::chrono::steady_clock::duration end, std::function<responses(event::base*)> Job, types Flags = types::NONE, std::string id = "") : 
                    action({}, std::move(Job), std::move(id)), 
                    flags(Flags), 
                    startTime(std::chrono::steady_clock::now()), 
                    endTime(end) 
                {}

                bool is(types f) const {
                    return ((uint8_t)flags & (uint8_t)f) > 0;
                }

                void add(types f){
                    flags = (types)((uint8_t)flags | (uint8_t)f);
                }

                void remove(types f){
                    flags = (types)((uint8_t)flags & ~(uint8_t)f);
                }
            };
        }
        
        struct base {
            input::base* in;   // helper to more easily reference the incoming data and transform it.

            std::vector<element*> handlers;     // A simple registry of elements that have registered themselves as event handlers
            std::vector<event::base*> data;     // Converted data

            base() = default;

            // Reads the input key data and transform it into output events
            void transformInput();

        private:
            void populateInputEventsForHeldDownKeys();  // Called from eventHandler()
            void mouseAPI();            // Reads input::keys and creates user friendly events based on those
            void scrollAPI();           // same as mouseAPI but for scrolling
            void handleEscape();        // Consumes escape key events into UI actions
            void handleTabulator();     // same as handleEscape but for tabulator
            void eventHandler(); 
        };
    }

    /**  
     This function represents philosophy more than functionality, let me explain:
     The idea is that all parts in the core::backend are as separated as possible, but as state machines with interconnected behaviour they still need connections to each other.
     - converter::input represents the machine level inputs, like a driver state machine
     - converter::output represents the conversion of these low level inputs as more user friendly format
     converter::input owns the input thread since that is the direct polling connection at the hardware level, but converter::input still needs to call the state machine of converter::output to parse the inputs it has polled.
     This interconnactivity and separation is not a must, but i have decided to use this philosophy in this projects new backend.
     */
    inline void link(input::base* in, output::base* out) {
        in->out = out;
        out->in = in;
    }

}

namespace GGUI::action {
    using responses = converter::output::event::action::responses;
}

#endif