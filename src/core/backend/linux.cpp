#ifdef __linux__
#include <sys/ioctl.h>
#include <termios.h>
#include <unistd.h>
#include <sys/uio.h> // Needed for writev
#include <sys/fcntl.h>
#include <sys/stat.h>
#include <errno.h>
#include <cstring>
#include <poll.h>

#include "../utils/logger.h"
#include "../utils/constants.h"
#include "terminal.h"

namespace GGUI {
    namespace terminal {
        // These are re-routable, set these to master/slave handles for PTY or anything really.
        static struct device {
            static constexpr int32_t CLOSED_HANDLE = -1;

            int32_t handle;
            termios state;

            device() {
                handle = CLOSED_HANDLE;
                state = {};
            }

            ~device() {
                if (handle != CLOSED_HANDLE) close(handle); // Clean up handle on destruction
            }

            void update(int32_t newHandle) {
                if (handle != CLOSED_HANDLE) close(handle); // Close previous handle if open

                handle = newHandle;

                // Only do termios checks and binds if the given route is a tty valid, if not we dont need termios and are probably buffering into a file or a pipe.
                if (isatty(handle)) {
                    if (tcgetattr(handle, &state) < 0) {    // fetch tty attributes from kernel
                        GGUI::INTERNAL::LOGGER::log("ERROR: Failed to get terminal attributes: " + std::string(strerror(errno)));
                    }

                    // since we have our own control sequence parsing we need the line discipline to be raw
                    cfmakeraw(&state);
                }
            }
        } 
            input,          // Used to read input from the incoming transmission
            output;         // Used to write into the presentation buffer, which is shown

        // Simple helper struct for packaging custom flags and device serial locations to be opened and routed into. 
        struct routable {
            std::string_view AbsolutePath;
            int32_t flags;
        };

        void routeTo(routable in, routable out) {
            // Open the input and output files with the specified flags
            int32_t inHandle = open(in.AbsolutePath.data(), in.flags | O_RDONLY);
            if (inHandle == device::CLOSED_HANDLE) {
                GGUI::INTERNAL::LOGGER::log("ERROR: Failed to open input route: " + std::string(strerror(errno)));
                return;
            }

            int32_t outHandle = open(out.AbsolutePath.data(), out.flags | O_WRONLY);
            if (outHandle == device::CLOSED_HANDLE) {
                GGUI::INTERNAL::LOGGER::log("ERROR: Failed to open output route: " + std::string(strerror(errno)));
                return;
            }

            // Update the input and output handles and states
            input.update(inHandle);
            output.update(outHandle);
        }

        void platformInit() {
            // By default we route to the standard streams, but this can be changed by calling routeTo with custom paths and flags.
            routeTo(
                { "/dev/stdin",  0 },   // Input route
                { "/dev/stdout", 0 }    // Output route
            );
        }

        // Default deinit (NOP)
        void platformDeinit() {}

        // Here we translate the linux specific termios and API it back via the terminal::transmission data
        void queryInput() {
            // If stdin isn't a TTY (e.g., piped/timeout), read() may return 0 (EOF) repeatedly; avoid spinning.
            if (!isatty(input.handle)) {
                // Use poll to wait briefly for readability; if not readable, sleep a bit to avoid busy-loop.
                struct pollfd pollFileDescriptor = {
                    input.handle,
                    POLLIN,
                    0
                };

                constexpr nfds_t  fileDescriptorCount = 1;

                if (poll(
                    &pollFileDescriptor,
                    fileDescriptorCount,
                    TIME::SECOND    // Max allowed wait time, could be replaced with -1, to wait as long as needed.
                ) <= 0) {
                    // No data; avoid spinning
                    currentStates->transmission.inputSize = 0;
                    return;
                }
            }

            currentStates->transmission.inputSize = read(input.handle, currentStates->transmission.inputBuffer.begin(), currentStates->transmission.capacity);
            if (currentStates->transmission.inputSize <= 0) {
                // EOF or error; normalize to 0 to signal no input
                currentStates->transmission.inputSize = 0;
            }
        }

        void queryOutput(std::vector<std::string_view>&& queue) {
            // transforms the incoming queued vector of strings into iovec
            std::vector<iovec> vec;
            vec.reserve(queue.size());

            ssize_t totalSize = 0;

            for (const auto& str : queue) {
                vec.push_back({ (void*)str.data(), str.size() });
                totalSize += str.size();
            }

            ssize_t wrote = writev(output.handle, vec.data(), vec.size());
            if (wrote != totalSize) {
                GGUI::INTERNAL::LOGGER::log("Failed to fully write to: '" + std::to_string(output.handle) + "' (wrote " + std::to_string(wrote) + " of " + std::to_string(totalSize) + ")");
            }
        }
    }
}

#endif