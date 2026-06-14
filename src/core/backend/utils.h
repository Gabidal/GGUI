#ifndef _UTILS_H_
#define _UTILS_H_

#include <cstdint>
#include <algorithm>

namespace GGUI {
    namespace terminal {

        enum class baudRate : int32_t {
            B50             = 50,
            B75             = 75,
            B110            = 110,
            B134            = 134,
            B150            = 150,
            B200            = 200,
            B300            = 300,
            B600            = 600,
            B1200           = 1200,
            B1800           = 1800,
            B2400           = 2400,
            B4800           = 4800,
            B9600           = 9600,
            B19200          = 19200,
            B28800          = 28800,
            B38400          = 38400,
            B57600          = 57600,
            B115200         = 115200,
            B230400         = 230400,
            B460800         = 460800,
            B576000         = 576000,
            B921600         = 921600
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

    }
}

#endif