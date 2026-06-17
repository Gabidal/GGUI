#ifdef __linux__
#include <sys/ioctl.h>
#include <signal.h>
#include <termios.h>
#include <unistd.h>
#include <sys/uio.h> // Needed for writev
#include <sys/fcntl.h>
#include <sys/stat.h>
#include <errno.h>
#include <cstring>
#include <poll.h>

#include "../utils/logger.h"
#include "terminal.h"

namespace GGUI {
    namespace terminal {

        // Default deinit (NOP)
        void platformDeinit() {}

        void queryInputs() {
            // If stdin isn't a TTY (e.g., piped/timeout), read() may return 0 (EOF) repeatedly; avoid spinning.
            if (!enabledFeatures.has(features::TTY)) {
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
                    queue.inputSize = 0;
                    return;
                }
            }

            queue.inputSize = read(STDIN_FILENO, queue.inputBuffer.begin(), queue.capacity);
            if (queue.inputSize <= 0) {
                // EOF or error; normalize to 0 to signal no input
                queue.inputSize = 0;
            }
        }

        void queryOutput() {
            // For output queries, we can just write to stdout directly; the caller is responsible for formatting the output buffer correctly.
            if (queue.outputSize > 0) {
                ssize_t written = write(STDOUT_FILENO, queue.outputBuffer.data(), queue.outputSize);
                if (written < 0) {
                    GGUI::INTERNAL::LOGGER::log("ERROR: Failed to write output query response: " + std::string(strerror(errno)));
                } else if (queue.outputSize != written) {   // Move the buffer
                    // NOTE: this will be potentially really slow
                    std::memmove(queue.outputBuffer.data(), queue.outputBuffer.data() + written, queue.outputSize - written);

                    queue.outputSize -= written;
                }
            }
        }
    }
}

#endif