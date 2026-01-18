#ifndef _ASCII_H_
#define _ASCII_H_

#include <cstdint>

namespace GGUI {
    namespace ASCII {
        // Extended ASCII table
        enum class table : uint8_t {
            NULL_TERMINATE          = 0,
            START_OF_HEADING,
            START_OF_TEXT,
            END_OF_TEXT,
            END_OF_TRANSMISSION,
            ENQUIRY,
            ACKNOWLEDGE,
            BELL,
            BACKSPACE,
            HORIZONTAL_TAB,
            LINE_FEED,
            VERTICAL_TAB,
            FORM_FEED,
            CARRIAGE_RETURN,
            SHIFT_OUT,                  // OUT /
            SHIFT_IN,                   //    / IN  is Deprecated, will send in TABLE[129]. If receives TABLE[14/15], will transform into TABLE[129] + state[ON/OFF]
            DATA_LINK_ESCAPE,
            DEVICE_CONTROL_1,
            DEVICE_CONTROL_2,
            DEVICE_CONTROL_3,
            DEVICE_CONTROL_4,
            NEGATIVE_ACKNOWLEDGE,
            SYNCHRONOUS_IDLE,
            END_OF_TRANSMISSION_BLOCK,
            CANCEL,
            END_OF_MEDIUM,
            SUBSTITUTE,
            ESCAPE,
            FILE_SEPARATOR,
            GROUP_SEPARATOR,
            RECORD_SEPARATOR,
            UNIT_SEPARATOR,

            SPACE                   = 32,

            // Visible characters
            // ...
            // - END

            DELETE                  = 127,

            // Extended UINT8 ASCII table

            INSERT,

            SHIFT,
            CTRL,
            SUPER,
            ALT,
            ALTGR,
            FN,

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

            MAX_VALUE,  // Always +1, so that arrays count zero
        };

        constexpr uint8_t get(uint8_t index) {
            return static_cast<uint8_t>(table::NULL_TERMINATE) + index;
        }

        constexpr uint8_t get(table t) {
            return static_cast<uint8_t>(t);
        }
    }
}

#endif