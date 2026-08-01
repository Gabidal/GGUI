#ifndef _CONVERTER_H_
#define _CONVERTER_H_

#include <cstdint>
#include <chrono>
#include <array>
#include <thread>

#include "backend/ecma.h"   TODO("Move some of the table functions into backend/utils.h so that we wount need to include ecma here.")

namespace GGUI {
    /**
     * @brief Combines the terminal and DRM backend input polling and converts them into GGUI events.
     */
    namespace converter {
        namespace output {
            struct base;
        }

        namespace input {
            struct key {
                enum class types : uint8_t {
                    __min                   = 32,

                    SPACE                   = 32,

                    // Visible characters
                    // ...
                    // - END

                    DELETE                  = 127,

                    ALL_LETTERS,     // Used when wanting to gather all visible characters.

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

                bool state;
                std::chrono::steady_clock::time_point captureTime = std::chrono::steady_clock::now();

                key(bool newState = false) : state(newState) {}
            };

            using allKeys = std::array<key, (size_t)terminal::ecma::table::getSize<key::types>()>;

            /**
             * @brief Contains the incoming key information
             * NOTE: this information is shared, so fetch this information from the correct backend interface!
             */
            struct base {
                output::base* out;

                allKeys currentKeyboardState;
                allKeys previousKeyboardState;     // used to compare changed states.

                std::thread pollingThread;

                base();

                void inputThread();
            };
        }

        namespace output {
            namespace event {
                using selectedKeys = std::vector<input::key::types>;
    
                struct base{
                    selectedKeys criteria;

                    base(std::initializer_list<input::key::types> keys = {}) : criteria(keys) {}

                    bool has(input::key::types t) const {
                        for (const auto& key : criteria){
                            if (key == t) return true;
                        }
                        return false;
                    }
                };
    
                struct action : base{
                    std::string ID;
                    std::function<bool(event::base*)> job;
                
                    action() = default;
                    action(const selectedKeys& Criteria, std::function<bool(event::base*)> Job, std::string id){
                        criteria = Criteria;
                        job = Job;
                        ID = id;
                    }
                };
    
                struct memory : action{
                    enum class types : uint8_t {
                        NONE               = 0 << 0,
                        PROLONG_MEMORY     = 1 << 0,
                        RETRIGGER          = 1 << 1
                    } flags = types::NONE;

                    std::chrono::steady_clock::time_point startTime;
                    std::chrono::steady_clock::duration endTime;
    
                    // When the job starts, job, prolong previous similar job by this time.
                    memory(std::chrono::steady_clock::duration end, std::function<bool(event::base*)> Job, types Flags = types::NONE, std::string_view id = ""){
                        startTime = std::chrono::steady_clock::now();
                        endTime = end;
                        job = Job;
                        flags = Flags;
                        ID = id;
                    }
    
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

                // Reads the incoming data and transform it into 
                void transformInput();

            protected:
                void eventHandler(); 
                void Populate_Inputs_For_Held_Down_Keys();
                void mouseAPI();
                void scrollAPI();
                void handleEscape();
                void handleTabulator();
            };
        }

        inline void link(input::base* in, output::base* out) {
            in->out = out;
            out->in = in;
        }
    }
}

#endif