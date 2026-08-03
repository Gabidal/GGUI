#ifdef __linux__
#include <sys/ioctl.h>
#include <termios.h>
#include <unistd.h>
#include <sys/uio.h> // Needed for writev
#include <sys/fcntl.h>
#include <sys/stat.h>
#include <signal.h>
#include <errno.h>
#include <cstring>
#include <poll.h>

#include "../utils/logger.h"

#include "terminal.h"

#include "../core.h"

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

                    // Since we have our own control sequence parsing we need the line discipline to be raw
                    cfmakeraw(&state);

                    // Since cfmakeraw does not push the update, we need to push it manually:
                    if (tcsetattr(handle, TCSANOW, &state) < 0) {
                        GGUI::INTERNAL::LOGGER::log("ERROR: Failed to set terminal attributes: " + std::string(strerror(errno)));
                    }
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

        IVector2 getScreenDimensions() {
            struct winsize w;
            if (ioctl(output.handle, TIOCGWINSZ, &w) == -1) {
                GGUI::INTERNAL::LOGGER::log("ERROR: Failed to get window size: " + std::string(strerror(errno)));
                return { 0, 0 }; // Return a default size on error
            }
            return { w.ws_col, w.ws_row };
        }

        // Updates main dom and terminal.screen.dimensions
        void updateScreenDimensions() {
            IVector2 newSize = getScreenDimensions();
            
            // Update terminal backend
            currentStates->screen.dimensions = newSize;

            // Notify the renderer of change
            getRoot()->setDimensions(newSize.x, newSize.y);
        }

        void setAutomaticScreenResizeHandler() {
            // Create a sigaction for window resize
            struct sigaction resizeHandler;

            resizeHandler.sa_handler = [](int) {
                updateScreenDimensions();
            };

            sigemptyset(&resizeHandler.sa_mask);    // Clears any other handler which could potentially hinder this handler.

            resizeHandler.sa_flags = 0;              // Since sigaction flags does not get auto constructed, we need to clean it.

            if (sigaction(SIGWINCH, &resizeHandler, nullptr) == -1) {
                GGUI::INTERNAL::LOGGER::log("ERROR: Failed to set SIGWINCH handler: " + std::string(strerror(errno)));
            }
        }

        void platformInit() {
            // By default we route to the standard streams, but this can be changed by calling routeTo with custom paths and flags.
            routeTo(
                { "/dev/stdin",  0 },   // Input route
                { "/dev/stdout", 0 }    // Output route
            );

            // Set up automatic screen resize handling
            setAutomaticScreenResizeHandler();
        }

        // Used from outside linux.cpp to ask regardless of platform type, whether the connection was established correctly
        bool isConnected() {
            return input.handle != device::CLOSED_HANDLE && output.handle != device::CLOSED_HANDLE;
        }

        // Default deinit (NOP)
        void platformDeinit() {}

        // Here we translate the linux specific termios and API it back via the terminal::transmission data
        void queryInput() {
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
                std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::seconds(10)).count()
            ) <= 0) {
                // No data; avoid spinning
                currentStates->transmission.inputSize = 0;
                
                GGUI::INTERNAL::LOGGER::log("poll timeout!");
                return;
            }

            currentStates->transmission.inputSize = read(input.handle, currentStates->transmission.inputBuffer.data(), currentStates->transmission.capacity);
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

            GGUI::INTERNAL::LOGGER::log("sending bytes...");
            ssize_t wrote = writev(output.handle, vec.data(), vec.size());
            if (wrote != totalSize) {
                GGUI::INTERNAL::LOGGER::log("Failed to fully write to: '" + std::to_string(output.handle) + "' (wrote " + std::to_string(wrote) + " of " + std::to_string(totalSize) + ")");
            }

            // force the PTY to flush our bytes
            if (tcdrain(output.handle) < 0) {
                GGUI::INTERNAL::LOGGER::log("ERROR: Failed to drain terminal output: " + std::string(strerror(errno)));
            }
        }
    }
}

#endif