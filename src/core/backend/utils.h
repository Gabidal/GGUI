#ifndef _BACKEND_UTILS_H_
#define _BACKEND_UTILS_H_

#include <cstdint>
#include <algorithm>
#include <chrono>

namespace GGUI {
    namespace terminal {

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

        // This is a simplification struct, made for the user to interface with.
        struct key {
            enum class types : uint8_t {
                __min                   = 32,

                SPACE                   = 32,

                // Visible characters
                // ...
                // - END

                DELETE                  = 127,

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

                LEFT_CLICK,
                MIDDLE_CLICK,
                RIGHT_CLICK,

                __max,  // Always +1, so that arrays count zero
            };
            
            // Simple value + time snapshot
            bool state; std::chrono::steady_clock::time_point captureTime;

            // Capture time as creation
            key(bool State = false) : state(State), captureTime(std::chrono::steady_clock::now()) {}
        };

    }
}

#endif