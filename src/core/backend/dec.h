#ifndef _DEC_H_
#define _DEC_H_

#include "terminal.h"
#include "ansi.h"

namespace GGUI {
    namespace terminal {
        /**
         * @brief Contains the DEC VTxxx input implementation of input interface and output sequencing
         * URL: https://vt100.net/docs/
        */
        namespace dec {
            using namespace ansi;       // Since the ASCII table is same as the one stated in ANSI, we will route it here, with alias and get functions for reusability.
        
            namespace VT100 {
                // Since so many VTxxx sequences depend on their private modes, we need to introduce bindings to enforce correct sequence for each mode.
                namespace binds {
                    enum class types {
                        NONE,                               // Error (ignored)
                        DEC_CURSOR_KEY_MODE,                // Cursor key       (DECCKM)
                        DEC_ANSI_MODE,                      // ANSI/VT52        (DECANM)
                        DEC_COLUMN_MODE,                    // Column           (DECCOLM)
                        DEC_SCROLLING_MODE,                 // Scrolling        (DECSCLM)
                        DEC_SCREEN_MODE,                    // Screen           (DECSCNM)
                        DEC_ORIGIN_MODE,                    // Origin           (DECOM)
                        DEC_AUTO_WRAP_MODE,                 // Auto wrap        (DECAWM)
                        DEC_AUTO_REPEATING_MODE,            // Auto repeating   (DECARM)
                        DEC_INTERLACE_MODE                  // Interface        (DECINLM)
                    };

                    inline const auto privateModeSetter = ecma::table::toInt(3, 15);        // '?'
                }

                enum class arrowKeysReset {
                    // UP = ecma::sequences::cursorControlFunctions::CURSOR_UP
                };

            }

        }
    }
}

#endif