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
            namespace VT100 {
                namespace mode {
                    // Since so many VTxxx sequences depend on their private modes, we need to introduce bindings to enforce correct sequence for each mode.
                    namespace privates {
                        enum class types {
                            __min = 0,

                            NONE,                               // Error (ignored)
                            CURSOR_KEY_MODE,                    // Cursor key       (DECCKM)
                            ANSI_MODE,                          // ANSI/VT52        (DECANM)
                            COLUMN_MODE,                        // Column           (DECCOLM)
                            SCROLLING_MODE,                     // Scrolling        (DECSCLM)
                            SCREEN_MODE,                        // Screen           (DECSCNM)
                            ORIGIN_MODE,                        // Origin           (DECOM)
                            AUTO_WRAP_MODE,                     // Auto wrap        (DECAWM)
                            AUTO_REPEATING_MODE,                // Auto repeating   (DECARM)
                            INTERLACE_MODE,                      // Interface        (DECINLM)

                            __max = INTERLACE_MODE
                        };

                        inline const auto privateModeSetter = ecma::table::toInt(3, 15);        // '?'
                        
                        inline auto editMode(types t, ecma::table::mode::definition status) {
                            const std::vector<ecma::sequence::parameter::selectable<ecma::table::mode::types>> params = {
                                ecma::sequence::parameter::selectable<ecma::table::mode::types>(static_cast<ecma::table::mode::types>(privateModeSetter)),
                                ecma::sequence::parameter::selectable<ecma::table::mode::types>(static_cast<ecma::table::mode::types>(t))
                            };

                            if (status == ecma::table::mode::definition::SET) {
                                return ecma::sequences::modeSettings::SET_MODE.compile(params);
                            } else {
                                return ecma::sequences::modeSettings::RESET_MODE.compile(params);
                            }
                        }
                    }
                }

                struct machine {
                    ecma::table::mode::flags<mode::privates::types> modes;
                };

                enum class arrowKeysReset {
                    // UP = ecma::sequences::cursorControlFunctions::CURSOR_UP
                };

            }
        }
    }
}

#endif