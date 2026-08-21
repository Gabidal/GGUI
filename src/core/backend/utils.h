#ifndef _BACKEND_UTILS_H_
#define _BACKEND_UTILS_H_

#include <cstdint>
#include <algorithm>

namespace GGUI {
    namespace terminal {
        
        using cell = char32_t;

        // How many bytes/second
        enum class baudRate : int32_t {
            BYTES_50             = 50,
            BYTES_75             = 75,
            BYTES_110            = 110,
            BYTES_134            = 134,
            BYTES_150            = 150,
            BYTES_200            = 200,
            BYTES_300            = 300,
            BYTES_600            = 600,
            BYTES_1200           = 1200,
            BYTES_1800           = 1800,
            BYTES_2400           = 2400,
            BYTES_4800           = 4800,
            BYTES_9600           = 9600,
            BYTES_19200          = 19200,
            BYTES_28800          = 28800,
            BYTES_38400          = 38400,
            BYTES_57600          = 57600,
            BYTES_115200         = 115200,
            BYTES_230400         = 230400,
            BYTES_460800         = 460800,
            BYTES_576000         = 576000,
            BYTES_921600         = 921600
        };

        enum class transmissionMode : int8_t {
            DUPLEX,
            HALF_DUPLEX,
            SIMPLEX
        };

        struct transmissionInfo {
            baudRate input, output;
            int8_t bitsPerCharacter;
        };

        struct UARTFrame {
            bool hasParityBitEnabled;
            bool DualStopBitEnabled;

            constexpr int32_t getStartBitCount()  const { return 1; /* UART always has a start bit. */ }
            constexpr int32_t getParityBitCount() const { return hasParityBitEnabled ? 1 : 0; }
            constexpr int32_t getStopBitCount()   const { return DualStopBitEnabled  ? 2 : 1; } 
        };

        struct limitations {
            baudRate maximumSupportedTransmissionRate;
            transmissionMode supportedTransmissionMode;
            int32_t maximumBufferSize;
            int32_t bufferThresholdBeforeOverflowSignal;
            int32_t transmissionLatencyCharacterCount;

            /**
             * @brief Calculates the number of characters that can be transmitted before overflow signal is sent, given the current transmission conditions.
             */
            constexpr int32_t numberOfCharacterToOverflow(transmissionInfo currentTransmission) const {
                return std::max(
                    0, 
                    (int32_t)(
                        bufferThresholdBeforeOverflowSignal - (
                            transmissionLatencyCharacterCount * ((float)currentTransmission.input / (float)currentTransmission.output)
                        )
                    )
                );
            }

            /**
             * @brief Calculates the time to respond to the XOFF signal to avoid overflow, given the current transmission conditions and UART frame configuration.
             */
            constexpr float timeToRespond(transmissionInfo currentTransmission, UARTFrame frameInfo) const {
                return (float)(
                    numberOfCharacterToOverflow(currentTransmission) * 
                    (frameInfo.getStartBitCount() + currentTransmission.bitsPerCharacter + frameInfo.getParityBitCount() + frameInfo.getStopBitCount())
                ) / (float)currentTransmission.input;
            }
        };

        namespace ecma {
            enum class graphicalTextAttributes : uint8_t {
                DEFAULT,                                        // default rendition (implementation-defined), cancels the effect of any preceding occurrence of SGR in the data stream regardless of the setting of the GRAPHIC RENDITION COMBINATION MODE (GRCM)
                __min = DEFAULT,
                BOLD,                                           // bold or increased intensity
                FAINT,                                          // faint, decreased intensity or second colour
                ITALIC,                                         // italicized
                UNDERLINE,                                      // singly underlined
                SLOW_BLINK,                                     // slowly blinking (less then 150 per minute)
                RAPID_BLINK,                                    // rapidly blinking (150 per minute or more)
                REVERSE_VIDEO,                                  // negative image
                CONCEAL,                                        // concealed characters
                CROSSED_OUT,                                    // crossed-out (characters still legible but marked as to be deleted)
                PRIMARY_FONT,                                   // primary (default) font
                ALT_FONT_1,                                     // first alternative font
                ALT_FONT_2,                                     // second alternative font
                ALT_FONT_3,                                     // third alternative font
                ALT_FONT_4,                                     // fourth alternative font
                ALT_FONT_5,                                     // fifth alternative font
                ALT_FONT_6,                                     // sixth alternative font
                ALT_FONT_7,                                     // seventh alternative font
                ALT_FONT_8,                                     // eighth alternative font
                ALT_FONT_9,                                     // ninth alternative font
                FRAKTUR,                                        // Fraktur (Gothic)
                DOUBLY_UNDERLINED,                              // doubly underlined
                NORMAL_INTENSITY,                               // normal colour or normal intensity (neither bold nor faint)
                NOT_ITALIC_NOT_FRAKTUR,                         // not italicized, not fraktur
                NOT_UNDERLINED,                                 // not underlined (neither singly nor doubly)
                NOT_BLINKING,                                   // steady (not blinking)
                RESERVED_PROPORTIONAL_SPACING,                  // (reserved for proportional spacing as specified in CCITT Recommendation T.61)
                POSITIVE_IMAGE,                                 // positive image
                REVEAL,                                         // revealed characters
                NOT_CROSSED_OUT,                                // not crossed out
                FG_BLACK,                                       // black display
                FG_RED,                                         // red display
                FG_GREEN,                                       // green display
                FG_YELLOW,                                      // yellow display
                FG_BLUE,                                        // blue display
                FG_MAGENTA,                                     // magenta display
                FG_CYAN,                                        // cyan display
                FG_WHITE,                                       // white display
                FOREGROUND_COLOR,                               // intended for setting character foreground colour as specified in ISO 8613-6 [CCITT Recommendation T.416])
                FG_DEFAULT,                                     // default display colour (implementation-defined)
                BG_BLACK,                                       // black background
                BG_RED,                                         // red background
                BG_GREEN,                                       // green background
                BG_YELLOW,                                      // yellow background
                BG_BLUE,                                        // blue background
                BG_MAGENTA,                                     // magenta background
                BG_CYAN,                                        // cyan background
                BG_WHITE,                                       // white background
                BACKGROUND_COLOR,                               // intended for setting character background colour as specified in ISO 8613-6 [CCITT Recommendation T.416])
                BG_DEFAULT,                                     // default background colour (implementation-defined)
                RESERVED_CANCEL_PROPORTIONAL_SPACING,           // (reserved for cancelling the effect of the rendering aspect established by parameter value 26)
                FRAMED,                                         // framed
                ENCIRCLED,                                      // encircled
                OVERLINED,                                      // overlined
                NOT_FRAMED_NOT_ENCIRCLED,                       // not framed, not encircled
                NOT_OVERLINED,                                  // not overlined
                RESERVED_56,                                    // (reserved for future standardization)
                RESERVED_57,                                    // (reserved for future standardization)
                RESERVED_58,                                    // (reserved for future standardization)
                RESERVED_59,                                    // (reserved for future standardization)
                IDEOGRAM_UNDERLINE,                             // ideogram underline or right side line
                IDEOGRAM_DOUBLE_UNDERLINE,                      // ideogram double underline or double line on the right side
                IDEOGRAM_OVERLINE,                              // ideogram overline or left side line
                IDEOGRAM_DOUBLE_OVERLINE,                       // ideogram double overline or double line on the left side
                IDEOGRAM_STRESS_MARKING,                        // ideogram stress marking
                __max = IDEOGRAM_STRESS_MARKING,
                IDEOGRAM_ATTRIBUTES_OFF,                        // cancels the effect of the rendition aspects established by parameter values 60 to 64
            };
        }
    }
}

#endif