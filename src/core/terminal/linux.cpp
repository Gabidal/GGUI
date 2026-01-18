#ifdef __linux__
#include <sys/ioctl.h>
#include <signal.h>
#include <termios.h>
#include <unistd.h>
#include <sys/uio.h> // Needed for writev
#include <sys/fcntl.h>
#include <errno.h>
#include <cstring>
#include <poll.h>

#include "../utils/logger.h"
#include "terminal.h"

namespace GGUI {
    namespace terminal {
        termios extendedFeatures;
        termios previousTerminalState;

        // POSIX implementation of the terminal::init
        void init() {
            // First safety zero all features:
            features.clear();

            // Check if this is an interactive terminal
            if (isatty(STDIN_FILENO) != 0) {
                features.set(feature::TTY);

                // Ensure we don't leave the descriptor in an unexpected mode; do not force non-blocking here.
                // Configure terminal to raw mode
                if (tcgetattr(STDIN_FILENO, &extendedFeatures) == 0) {
                    previousTerminalState = extendedFeatures;
                    // Keep ISIG enabled so Ctrl+C still works.
                    extendedFeatures.c_lflag &= ~(ECHO | ICANON);           // Disable echo + canonical mode so mouse packets are not echoed back into the terminal.
                    extendedFeatures.c_cc[VMIN] = 1;   // return after 1 byte
                    extendedFeatures.c_cc[VTIME] = 0;  // no timeout

                    // Check if the previous state corresponds to the current state
                    if (
                        extendedFeatures.c_lflag != previousTerminalState.c_lflag || 
                        extendedFeatures.c_cc[VMIN] != previousTerminalState.c_cc[VMIN] || 
                        extendedFeatures.c_cc[VTIME] != previousTerminalState.c_cc[VTIME]
                    ) {
                        if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &extendedFeatures) != 0) {   // Check if it succeeded
                            INTERNAL::LOGGER::log("Failed to enable raw mode (tcsetattr). Mouse reporting will remain disabled to avoid corrupting output.");
                        }
                    } 
                }
                else {
                    INTERNAL::LOGGER::log("Failed to snapshot terminal mode (tcgetattr). Mouse reporting will remain disabled to avoid corrupting output.");
                }
            }

            // Start probing regardless of (ECHO | ICANON) absence or not. 
            startProbing();
        }

        // Default deinit (NOP)
        std::function<void()> deinit = [](){};

        void waitForInput() {
            // If stdin isn't a TTY (e.g., piped/timeout), read() may return 0 (EOF) repeatedly; avoid spinning.
            if (!features.has(feature::TTY)) {
                // Use poll to wait briefly for readability; if not readable, sleep a bit to avoid busy-loop.
                struct pollfd pollFileDescriptor;
                pollFileDescriptor.fd = STDIN_FILENO;
                pollFileDescriptor.events = POLLIN;
                pollFileDescriptor.revents = 0;

                constexpr nfds_t  fileDescriptorCount = 1;

                if (poll(
                    &pollFileDescriptor,
                    fileDescriptorCount,
                    TIME::SECOND    // Max allowed wait time, could be replaced with -1, to wait as long as needed.
                ) <= 0) {
                    // No data; avoid spinning
                    inputQuery.size = 0;
                    return;
                }
            }

            inputQuery.size = read(STDIN_FILENO, inputQuery.buffer.begin(), inputQuery.capacity);
            if (inputQuery.size <= 0) {
                // EOF or error; normalize to 0 to signal no input
                inputQuery.size = 0;
            }
        }
    }
}

#endif