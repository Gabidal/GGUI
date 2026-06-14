#ifndef _DEC_H_
#define _DEC_H_

#include "ecma.h"
#include "utils.h"

namespace GGUI {
    namespace terminal {
        /**
         * @brief Contains the DEC VTxxx input implementation of input interface and output sequencing
         * URL: https://vt100.net/docs/
        */
        namespace dec {
            namespace VT100 {
                extern ecma::table::configuration::page G1;
                extern ecma::table::configuration::page G3;

                constexpr limitations limits{
                    baudRate::B19200,
                    transmissionMode::DUPLEX,
                    64,     // Maximum buffer capacity
                    32,     // buffer threshold before XOFF signal is sent to avoid overflow
                    3,      // The magical number given by DEC, probably a latency scaler...
                };

                namespace table {
                    constexpr auto toInt = ecma::table::toInt;

                    enum class keypadCodes {
                        /* COLUMN 4 */          /* COLUMN 5 */              /* COLUMN 6 */           /* COLUMN 7 */
                        ENTER = toInt(4, 13),   FUNCTION_1 = toInt(5, 0),   COMMA  = toInt(6, 12),  ZERO  = toInt(7, 0),
                                                FUNCTION_2 = toInt(5, 1),   DASH   = toInt(6, 13),  ONE   = toInt(7, 1),
                                                FUNCTION_3 = toInt(5, 2),   PERIOD = toInt(6, 14),  TWO   = toInt(7, 2),
                                                FUNCTION_4 = toInt(5, 3),                           THREE = toInt(7, 3),
                                                                                                    FOUR  = toInt(7, 4),
                                                                                                    FIVE  = toInt(7, 5),
                                                                                                    SIX   = toInt(7, 6),
                                                                                                    SEVEN = toInt(7, 7),
                                                                                                    EIGHT = toInt(7, 8),
                                                                                                    NINE  = toInt(7, 9),
                    };

                    // 8-bit characters, loaded into right side graphical page.
                    enum class specialGraphicCharacter {
                        /* COLUMN 8 */                  /* COLUMN 9 */                      /* COLUMN 10 */                         /* COLUMN 11 */
                                                        CARRIAGE_RETURN    = toInt(9, 0),   HORIZONTAL_LINE_3     = toInt(10, 0),   CENTERED_DOT    = toInt(11, 0),
                                                        LINE_FEED          = toInt(9, 1),   HORIZONTAL_LINE_5     = toInt(10, 1),
                                                        DEGREE_SYMBOL      = toInt(9, 2),   HORIZONTAL_LINE_7     = toInt(10, 2),
                                                        PLUS_MINUS         = toInt(9, 3),   HORIZONTAL_LINE_9     = toInt(10, 3),
                                                                                            LEFT_T_LINE_CROSS     = toInt(10, 4),
                                                                                            RIGHT_T_LINE_CROSS    = toInt(10, 5),
                                                        NEW_LINE           = toInt(9, 6),   BOTTOM_T_LINE_CROSS   = toInt(10, 6),
                                                        VERTICAL_TAB       = toInt(9, 7),   TOP_T_LINE_CROSS      = toInt(10, 7),
                                                        LOWER_RIGHT_CORNER = toInt(9, 8),   
                        BLANK          = toInt(8, 9),   UPPER_RIGHT_CORNER = toInt(9, 9),   
                                                        UPPER_LEFT_CORNER  = toInt(9, 10),  VERTICAL_BAR          = toInt(10, 10),
                                                        LOWER_LEFT_CORNER  = toInt(9, 11),  LESS_THAN_OR_EQUAL    = toInt(10, 11),
                        DIAMOND        = toInt(8, 12),  CROSSING_LINES     = toInt(9, 12),  GREATER_THAN_OR_EQUAL = toInt(10, 12),
                        CHECKERBOARD   = toInt(8, 13),  HORIZONTAL_LINE_1  = toInt(9, 13),  PI                    = toInt(10, 13),
                        HORIZONTAL_TAB = toInt(8, 14),                                      NOT_EQUAL_TO          = toInt(10, 14),
                        FORM_FEED      = toInt(8, 15),                                      UK_POUND_SIGN         = toInt(10, 15),
                    };
                }

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

                struct components {
                    ecma::table::mode::flags<mode::privates::types> modes;
                };

                namespace sequences {
                    using namespace ecma::sequences;

                    // The CSI variants are in ecma.h
                    namespace cursorControlFunctions {
                        extern void operate_CURSOR_UP(ecma::sequence::base*);
                        extern void operate_CURSOR_DOWN(ecma::sequence::base*);
                        extern void operate_CURSOR_LEFT(ecma::sequence::base*);
                        extern void operate_CURSOR_RIGHT(ecma::sequence::base*);

                        inline auto CURSOR_UP = base<ecma::sequence::prefix<ecma::table::finalWithoutIntermediate>>(ecma::table::finalWithoutIntermediate::CUU, {}, {operate_CURSOR_UP}, &G3);
                        inline auto CURSOR_DOWN = base<ecma::sequence::prefix<ecma::table::finalWithoutIntermediate>>(ecma::table::finalWithoutIntermediate::CUD, {}, {operate_CURSOR_DOWN}, &G3);
                        inline auto CURSOR_LEFT = base<ecma::sequence::prefix<ecma::table::finalWithoutIntermediate>>(ecma::table::finalWithoutIntermediate::CUF, {}, {operate_CURSOR_LEFT}, &G3);
                        inline auto CURSOR_RIGHT = base<ecma::sequence::prefix<ecma::table::finalWithoutIntermediate>>(ecma::table::finalWithoutIntermediate::CUB, {}, {operate_CURSOR_RIGHT}, &G3);
                    }

                    namespace keypadFunctions {
                        inline auto ENTER = base<ecma::sequence::prefix<table::keypadCodes>>(table::keypadCodes::ENTER, {}, {}, &G3);

                        inline auto FUNCTION_1 = base<ecma::sequence::prefix<table::keypadCodes>>(table::keypadCodes::FUNCTION_1, {}, {}, &G3);
                        inline auto FUNCTION_2 = base<ecma::sequence::prefix<table::keypadCodes>>(table::keypadCodes::FUNCTION_2, {}, {}, &G3);
                        inline auto FUNCTION_3 = base<ecma::sequence::prefix<table::keypadCodes>>(table::keypadCodes::FUNCTION_3, {}, {}, &G3);
                        inline auto FUNCTION_4 = base<ecma::sequence::prefix<table::keypadCodes>>(table::keypadCodes::FUNCTION_4, {}, {}, &G3);

                        inline auto COMMA = base<ecma::sequence::prefix<table::keypadCodes>>(table::keypadCodes::COMMA, {}, {}, &G3);
                        inline auto DASH = base<ecma::sequence::prefix<table::keypadCodes>>(table::keypadCodes::DASH, {}, {}, &G3);
                        inline auto PERIOD = base<ecma::sequence::prefix<table::keypadCodes>>(table::keypadCodes::PERIOD, {}, {}, &G3);

                        inline auto ZERO = base<ecma::sequence::prefix<table::keypadCodes>>(table::keypadCodes::ZERO, {}, {}, &G3);
                        inline auto ONE = base<ecma::sequence::prefix<table::keypadCodes>>(table::keypadCodes::ONE, {}, {}, &G3);
                        inline auto TWO = base<ecma::sequence::prefix<table::keypadCodes>>(table::keypadCodes::TWO, {}, {}, &G3);
                        inline auto THREE = base<ecma::sequence::prefix<table::keypadCodes>>(table::keypadCodes::THREE, {}, {}, &G3);
                        inline auto FOUR = base<ecma::sequence::prefix<table::keypadCodes>>(table::keypadCodes::FOUR, {}, {}, &G3);
                        inline auto FIVE = base<ecma::sequence::prefix<table::keypadCodes>>(table::keypadCodes::FIVE, {}, {}, &G3);
                        inline auto SIX = base<ecma::sequence::prefix<table::keypadCodes>>(table::keypadCodes::SIX, {}, {}, &G3);
                        inline auto SEVEN = base<ecma::sequence::prefix<table::keypadCodes>>(table::keypadCodes::SEVEN, {}, {}, &G3);
                        inline auto EIGHT = base<ecma::sequence::prefix<table::keypadCodes>>(table::keypadCodes::EIGHT, {}, {}, &G3);
                        inline auto NINE = base<ecma::sequence::prefix<table::keypadCodes>>(table::keypadCodes::NINE, {}, {}, &G3);
                    }

                    namespace graphicalCharacters {
                        inline auto BLANK = base<ecma::sequence::prefix<table::specialGraphicCharacter>>(table::specialGraphicCharacter::BLANK, {}, {}, &G1);
                        inline auto DIAMOND = base<ecma::sequence::prefix<table::specialGraphicCharacter>>(table::specialGraphicCharacter::DIAMOND, {}, {}, &G1);
                        inline auto CHECKERBOARD = base<ecma::sequence::prefix<table::specialGraphicCharacter>>(table::specialGraphicCharacter::CHECKERBOARD, {}, {}, &G1);
                        inline auto HORIZONTAL_TAB = base<ecma::sequence::prefix<table::specialGraphicCharacter>>(table::specialGraphicCharacter::HORIZONTAL_TAB, {}, {}, &G1);
                        inline auto FORM_FEED = base<ecma::sequence::prefix<table::specialGraphicCharacter>>(table::specialGraphicCharacter::FORM_FEED, {}, {}, &G1);

                        inline auto CARRIAGE_RETURN = base<ecma::sequence::prefix<table::specialGraphicCharacter>>(table::specialGraphicCharacter::CARRIAGE_RETURN, {}, {}, &G1);
                        inline auto LINE_FEED = base<ecma::sequence::prefix<table::specialGraphicCharacter>>(table::specialGraphicCharacter::LINE_FEED, {}, {}, &G1);
                        inline auto DEGREE_SYMBOL = base<ecma::sequence::prefix<table::specialGraphicCharacter>>(table::specialGraphicCharacter::DEGREE_SYMBOL, {}, {}, &G1);
                        inline auto PLUS_MINUS = base<ecma::sequence::prefix<table::specialGraphicCharacter>>(table::specialGraphicCharacter::PLUS_MINUS, {}, {}, &G1);
                        inline auto NEW_LINE = base<ecma::sequence::prefix<table::specialGraphicCharacter>>(table::specialGraphicCharacter::NEW_LINE, {}, {}, &G1);
                        inline auto VERTICAL_TAB = base<ecma::sequence::prefix<table::specialGraphicCharacter>>(table::specialGraphicCharacter::VERTICAL_TAB, {}, {}, &G1);

                        inline auto LOWER_RIGHT_CORNER = base<ecma::sequence::prefix<table::specialGraphicCharacter>>(table::specialGraphicCharacter::LOWER_RIGHT_CORNER, {}, {}, &G1);
                        inline auto UPPER_RIGHT_CORNER = base<ecma::sequence::prefix<table::specialGraphicCharacter>>(table::specialGraphicCharacter::UPPER_RIGHT_CORNER, {}, {}, &G1);
                        inline auto UPPER_LEFT_CORNER = base<ecma::sequence::prefix<table::specialGraphicCharacter>>(table::specialGraphicCharacter::UPPER_LEFT_CORNER, {}, {}, &G1);
                        inline auto LOWER_LEFT_CORNER = base<ecma::sequence::prefix<table::specialGraphicCharacter>>(table::specialGraphicCharacter::LOWER_LEFT_CORNER, {}, {}, &G1);
                        inline auto CROSSING_LINES = base<ecma::sequence::prefix<table::specialGraphicCharacter>>(table::specialGraphicCharacter::CROSSING_LINES, {}, {}, &G1);

                        inline auto HORIZONTAL_LINE_1 = base<ecma::sequence::prefix<table::specialGraphicCharacter>>(table::specialGraphicCharacter::HORIZONTAL_LINE_1, {}, {}, &G1);
                        inline auto HORIZONTAL_LINE_3 = base<ecma::sequence::prefix<table::specialGraphicCharacter>>(table::specialGraphicCharacter::HORIZONTAL_LINE_3, {}, {}, &G1);
                        inline auto HORIZONTAL_LINE_5 = base<ecma::sequence::prefix<table::specialGraphicCharacter>>(table::specialGraphicCharacter::HORIZONTAL_LINE_5, {}, {}, &G1);
                        inline auto HORIZONTAL_LINE_7 = base<ecma::sequence::prefix<table::specialGraphicCharacter>>(table::specialGraphicCharacter::HORIZONTAL_LINE_7, {}, {}, &G1);
                        inline auto HORIZONTAL_LINE_9 = base<ecma::sequence::prefix<table::specialGraphicCharacter>>(table::specialGraphicCharacter::HORIZONTAL_LINE_9, {}, {}, &G1);

                        inline auto LEFT_T_LINE_CROSS = base<ecma::sequence::prefix<table::specialGraphicCharacter>>(table::specialGraphicCharacter::LEFT_T_LINE_CROSS, {}, {}, &G1);
                        inline auto RIGHT_T_LINE_CROSS = base<ecma::sequence::prefix<table::specialGraphicCharacter>>(table::specialGraphicCharacter::RIGHT_T_LINE_CROSS, {}, {}, &G1);
                        inline auto BOTTOM_T_LINE_CROSS = base<ecma::sequence::prefix<table::specialGraphicCharacter>>(table::specialGraphicCharacter::BOTTOM_T_LINE_CROSS, {}, {}, &G1);
                        inline auto TOP_T_LINE_CROSS = base<ecma::sequence::prefix<table::specialGraphicCharacter>>(table::specialGraphicCharacter::TOP_T_LINE_CROSS, {}, {}, &G1);

                        inline auto VERTICAL_BAR = base<ecma::sequence::prefix<table::specialGraphicCharacter>>(table::specialGraphicCharacter::VERTICAL_BAR, {}, {}, &G1);

                        inline auto LESS_THAN_OR_EQUAL = base<ecma::sequence::prefix<table::specialGraphicCharacter>>(table::specialGraphicCharacter::LESS_THAN_OR_EQUAL, {}, {}, &G1);
                        inline auto GREATER_THAN_OR_EQUAL = base<ecma::sequence::prefix<table::specialGraphicCharacter>>(table::specialGraphicCharacter::GREATER_THAN_OR_EQUAL, {}, {}, &G1);
                        inline auto PI = base<ecma::sequence::prefix<table::specialGraphicCharacter>>(table::specialGraphicCharacter::PI, {}, {}, &G1);
                        inline auto NOT_EQUAL_TO = base<ecma::sequence::prefix<table::specialGraphicCharacter>>(table::specialGraphicCharacter::NOT_EQUAL_TO, {}, {}, &G1);

                        inline auto UK_POUND_SIGN = base<ecma::sequence::prefix<table::specialGraphicCharacter>>(table::specialGraphicCharacter::UK_POUND_SIGN, {}, {}, &G1);
                    }
                }

            }
        }
    }
}

#endif