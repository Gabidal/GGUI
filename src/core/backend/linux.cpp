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
        termios extendedFeatures;
        termios previousTerminalState;

        bitMask<features> fetchIOPermissions() {
            struct stat stdinStat, stdoutStat;

            bitMask<features> result;

            // --- STDIN ---
            if (fstat(STDIN_FILENO, &stdinStat) == 0) {
                if (isatty(STDIN_FILENO)) {
                    result.set(features::TTY);
                    result.set(features::READ);   // TTY is always readable
                } else if (S_ISFIFO(stdinStat.st_mode)) {
                    result.set(features::PIPED_IN);  // stdin is a pipe
                    result.set(features::READ);
                } else if (S_ISREG(stdinStat.st_mode)) {
                    result.set(features::REDIRECTED_IN); // stdin is a plain file
                    result.set(features::READ);
                } else if (S_ISCHR(stdinStat.st_mode)) {  // S_ISCHR = character device that isn't a TTY (/dev/null etc.)
                    if (!isatty(STDIN_FILENO)) {    // Could be a character device; check if it's not a TTY
                        result.set(features::READ);     // Character devices that aren't TTYs are still readable
                    }
                }
            }

            // --- STDOUT ---
            if (fstat(STDOUT_FILENO, &stdoutStat) == 0) {
                if (S_ISFIFO(stdoutStat.st_mode)) {
                    result.set(features::PIPED_OUT);
                } 
                else if (S_ISREG(stdoutStat.st_mode)) {
                    result.set(features::REDIRECTED_OUT);
                }

                // Check actual access mode the fd was opened with
                int flags = fcntl(STDOUT_FILENO, F_GETFL);
                if (flags != -1) {
                    int mode = flags & O_ACCMODE;  // mask out everything except access bits
                    if (mode == O_WRONLY || mode == O_RDWR) {
                        result.set(features::WRITE);
                    }
                }
            }

            return result;
        }

        termios toTermios(const INTERNAL::configuration& cfg) {
            termios t = {};

            t.c_iflag = (tcflag_t)cfg.iflag.get();
            t.c_oflag = (tcflag_t)cfg.oflag.get();
            t.c_cflag = (tcflag_t)cfg.cflag.get();

            // Reconstruct c_lflag from ECMA-48 mode flags
            t.c_lflag = 0;

            // SRM: MONITOR (RESET) -> echo on
            if (cfg.modes.get(ecma::table::mode::types::SEND_RECEIVE_MODE) == ecma::table::mode::definition::RESET)
                t.c_lflag |= ECHO | ECHOE | ECHOK;

            // KAM: ENABLED (RESET) -> ISIG (Ctrl+C/Z/\ generate signals)
            if (cfg.modes.get(ecma::table::mode::types::KEYBOARD_ACTION_MODE) == ecma::table::mode::definition::RESET)
                t.c_lflag |= ISIG;

            // CRM: CONTROL (RESET) -> canonical mode + extended processing
            if (cfg.modes.get(ecma::table::mode::types::CONTROL_REPRESENTATION_MODE) == ecma::table::mode::definition::RESET)
                t.c_lflag |= ICANON | IEXTEN;

            t.c_line = cfg.line;

            static_assert(INTERNAL::CC_COUNT <= NCCS, "cc_chars larger than NCCS");
            for (size_t i = 0; i < INTERNAL::CC_COUNT; ++i)
                t.c_cc[i] = cfg.cc_chars[i];

            cfsetispeed(&t, (speed_t)cfg.ispeed.get());
            cfsetospeed(&t, (speed_t)cfg.ospeed.get());

            return t;
        }

        INTERNAL::configuration fromTermios(const ::termios& t) {
            INTERNAL::configuration cfg = {};

            cfg.iflag = t.c_iflag;
            cfg.oflag = t.c_oflag;
            cfg.cflag = t.c_cflag;

            // Reconstruct ECMA-48 modes from c_lflag

            // ECHO → SRM MONITOR (RESET = echo on) / SIMULTANEOUS (SET = echo off)
            cfg.modes.set(INTERNAL::modeBase{
                ecma::table::mode::types::SEND_RECEIVE_MODE,
                (t.c_lflag & ECHO) ? ecma::table::mode::definition::RESET : ecma::table::mode::definition::SET
            });

            // ISIG → KAM ENABLED (RESET) / DISABLED (SET)
            cfg.modes.set(INTERNAL::modeBase{
                ecma::table::mode::types::KEYBOARD_ACTION_MODE,
                (t.c_lflag & ISIG) ? ecma::table::mode::definition::RESET : ecma::table::mode::definition::SET
            });

            // ICANON → CRM CONTROL (RESET = processed) / GRAPHIC (SET = raw)
            cfg.modes.set(INTERNAL::modeBase{
                ecma::table::mode::types::CONTROL_REPRESENTATION_MODE,
                (t.c_lflag & ICANON) ? ecma::table::mode::definition::RESET : ecma::table::mode::definition::SET
            });

            cfg.line = t.c_line;

            for (size_t i = 0; i < INTERNAL::CC_COUNT; ++i)
                cfg.cc_chars[i] = t.c_cc[i];

            cfg.ispeed = cfgetispeed(&t);
            cfg.ospeed = cfgetospeed(&t);

            return cfg;
        }

        bool snapshot(INTERNAL::configuration& cfg) {
            termios t;
            if (tcgetattr(STDIN_FILENO, &t) != 0) return false;
            cfg = fromTermios(t);
            return true;
        }

        bool apply(const INTERNAL::configuration& cfg) {
            termios t = toTermios(cfg);
            return tcsetattr(STDIN_FILENO, TCSAFLUSH, &t) == 0;
        }

        // Default deinit (NOP)
        void platformDeinit() {
            
        }

        void waitForInput() {
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