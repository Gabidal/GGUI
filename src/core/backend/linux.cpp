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
        namespace INTERNAL {
            static constexpr size_t CC_COUNT = 32;

            enum class inputFlags : uint32_t {
                IGNORE_BREAK_CONDITION  = 1 << 0,       // Ignore BREAK condition
                BREAK_FLUSH_AND_SIGNAL_INT  = 1 << 1,       // BREAK -> flush queues + SIGINT
                DISCARD_PARITY_FRAMING_ERRORS  = 1 << 2,       // Discard parity/framing error bytes
                MARK_PARITY_ERRORS  = 1 << 3,       // Mark parity errors with 0xFF 0x00 prefix
                ENABLE_INPUT_PARITY_CHECK   = 1 << 4,       // Enable input parity checking
                STRIP_8TH_BIT  = 1 << 5,       // Strip 8th bit from input bytes
                TRANSLATE_NL_TO_CR_ON_INPUT   = 1 << 6,       // Translate NL -> CR on input
                DISCARD_CR_ON_INPUT   = 1 << 7,       // Discard CR on input
                TRANSLATE_CR_TO_NL_ON_INPUT   = 1 << 8,       // Translate CR -> NL on input
                MAP_UPPERCASE_TO_LOWERCASE = 1 << 9,       // Map uppercase -> lowercase (obsolete)
                XON_XOFF_FLOW_CONTROL_ON_OUTPUT    = 1 << 10,      // XON/XOFF flow control on output
                ANY_CHAR_RESTARTS_OUTPUT   = 1 << 11,      // Any char restarts stopped output
                XON_XOFF_FLOW_CONTROL_ON_INPUT   = 1 << 12,      // XON/XOFF flow control on input
                RING_BELL_ON_INPUT_QUEUE_FULL = 1 << 13,      // Ring bell on input queue full
                INPUT_IS_UTF8   = 1 << 14,      // Input is UTF-8 (Linux ext.)
            };

            enum class outputFlags : uint32_t {
                ENABLE_ALL_POSTPROCESSING   = 1 << 0,   // Enable all output post-processing    (OPOST)
                MAP_LOWERCASE_TO_UPPERCASE   = 1 << 1,   // Map lowercase -> uppercase (obsolete)
                TRANSLATE_NEWLINE_TO_CARRIAGE_RETURN_NEWLINE   = 1 << 2,   // Translate NL -> CR+NL on output (ONLCR)
                TRANSLATE_CARRIAGE_RETURN_TO_NEWLINE   = 1 << 3,   // Translate CR -> NL on output (OCRNL)
                SUPPRESS_CARRIAGE_RETURN_AT_COLUMN_0   = 1 << 4,   // Suppress CR at column 0 (ONOCR)
                NEWLINE_PERFORMS_CARRIAGE_RETURN  = 1 << 5,   // NL performs CR (no separate CR needed) (ONLRET)
                USE_FILL_CHARACTERS_INSTEAD_OF_DELAYS   = 1 << 6,   // Use fill chars instead of timed delays (OFILL)
                FILL_CHARACTER_IS_DEL   = 1 << 7,   // Fill char is DEL (else NUL) (OFDEL)
                // Delay masks (obsolete hardcopy terminal padding - kept for ABI compat)
                NEWLINE_DELAY_MASK   = 0x0100,   // (NLDLY)
                NEWLINE_DELAY_0     = 0x0000,   // (NL0)
                NEWLINE_DELAY_1     = 0x0100,   // (NL1)
                CARRIAGE_RETURN_DELAY_MASK   = 0x0600,   // (CRDLY)
                CARRIAGE_RETURN_DELAY_0     = 0x0000,   // (CR0)
                CARRIAGE_RETURN_DELAY_1     = 0x0200,   // (CR1)
                CARRIAGE_RETURN_DELAY_2     = 0x0400,   // (CR2)
                CARRIAGE_RETURN_DELAY_3     = 0x0600,   // (CR3)
                TAB_DELAY_MASK  = 0x1800,   // (TABDLY)
                TAB_DELAY_0    = 0x0000,   // (TAB0)
                TAB_DELAY_1    = 0x0800,   // (TAB1)
                TAB_DELAY_2    = 0x1000,   // (TAB2)
                TAB_DELAY_3_EXPAND_TABS_TO_SPACES    = 0x1800,   // Expand tabs -> spaces (TAB3)
                BACKSPACE_DELAY_MASK   = 0x2000,   // (BSDLY)
                BACKSPACE_DELAY_0     = 0x0000,   // (BS0)
                BACKSPACE_DELAY_1     = 0x2000,   // (BS1)
                VERTICAL_TAB_DELAY_MASK   = 0x4000,   // (VTDLY)
                VERTICAL_TAB_DELAY_0     = 0x0000,   // (VT0)
                VERTICAL_TAB_DELAY_1     = 0x4000,   // (VT1)
                FORM_FEED_DELAY_MASK   = 0x8000,   // (FFDLY)
                FORM_FEED_DELAY_0     = 0x0000,   // (FF0)
                FORM_FEED_DELAY_1     = 0x8000,   // (FF1)
            };

            enum class hardwareControlFlags : uint32_t {
                // Baud rate
                // B0 is special: drops DTR, disconnects the line (not a speed)
                BAUD_HANG_UP = 0,
                BAUD_50      = 1,
                BAUD_75      = 2,
                BAUD_110     = 3,    // Original ASR-33 Teletype
                BAUD_134     = 4,    // 134.5 bps, IBM Selectric
                BAUD_150     = 5,
                BAUD_200     = 6,
                BAUD_300     = 7,
                BAUD_600     = 8,
                BAUD_1200    = 9,
                BAUD_1800    = 10,
                BAUD_2400    = 11,
                BAUD_4800    = 12,
                BAUD_9600    = 13,   // Canonical default
                BAUD_19200   = 14,
                BAUD_38400   = 15,   // CBAUD ceiling, above -> CBAUDEX
                
                // Extended rates
                BAUD_57600   = (1 << 12) + BAUD_50,
                BAUD_115200  = (1 << 12) + BAUD_75,
                BAUD_230400  = (1 << 12) + BAUD_110,
                BAUD_460800  = (1 << 12) + BAUD_134,
                BAUD_500000  = (1 << 12) + BAUD_150,
                BAUD_576000  = (1 << 12) + BAUD_200,
                BAUD_921600  = (1 << 12) + BAUD_300,
                BAUD_1000000 = (1 << 12) + BAUD_600,
                BAUD_1152000 = (1 << 12) + BAUD_1200,
                BAUD_1500000 = (1 << 12) + BAUD_1800,
                BAUD_2000000 = (1 << 12) + BAUD_2400,
                BAUD_2500000 = (1 << 12) + BAUD_4800,
                BAUD_3000000 = (1 << 12) + BAUD_9600,
                BAUD_3500000 = (1 << 12) + BAUD_19200,
                BAUD_4000000 = (1 << 12) + BAUD_38400,

                // Character frame size
                CHARACTER_SIZE_MASK   = 0x30,   // mask (CSIZE)
                CHARACTER_SIZE_5_DATA_BITS     = 0,      // 5 data bits (Baudot) (CS5)
                CHARACTER_SIZE_6_DATA_BITS     = 0x10,   // 6 data bits (CS6)
                CHARACTER_SIZE_7_DATA_BITS_ASCII_CLEAN     = 0x20,   // 7 data bits (ASCII-clean) (CS7)
                CHARACTER_SIZE_8_DATA_BITS     = 0x30,   // 8 data bits (standard) (CS8)

                CHARACTER_STOP_BITS_2  = 0x40,   // 2 stop bits (else 1) (CSTOPB)
                CHARACTER_READ_ENABLE   = 0x80,   // Enable receiver (CREAD)
                PARITY_ENABLE  = 0x100,  // Enable parity generation + checking (PARENB)
                PARITY_ODD  = 0x200,  // Odd parity (else even) (PARODD)
                HANG_UP_ON_LAST_CLOSE   = 0x400,  // Hang up on last close (drop DTR) (HUPCL)
                CONTROL_LOCAL_IGNORE_MODEM_STATUS_LINES  = 0x800,  // Ignore modem status lines (CLOCAL)
                HARDWARE_RTS_CTS_FLOW_CONTROL = 0x80000000, // Hardware RTS/CTS flow control (CRTSCTS)
            };

            enum class specialCharacterIndicies : uint8_t {
                INTERRUPT_SIGNAL    = 0,   // Interrupt (Ctrl+C -> SIGINT) (VINTR)
                QUIT_SIGNAL    = 1,   // Quit      (Ctrl+\ -> SIGQUIT) (VQUIT)
                ERASE_CHARACTER_BACKWARD   = 2,   // Erase char backward (Backspace) (VERASE)
                KILL_ENTIRE_LINE    = 3,   // Erase entire line (Ctrl+U) (VKILL)
                END_OF_FILE     = 4,   // End-of-file (Ctrl+D, canonical) (VEOF)
                NON_CANONICAL_READ_TIMEOUT    = 5,   // Non-canonical read timeout ×0.1s (VTIME)
                NON_CANONICAL_MINIMUM_BYTES_BEFORE_READ     = 6,   // Non-canonical min bytes before read() (VMIN)
                SWITCH_SHELL_LAYER    = 7,   // Switch shell layer (rarely used) (VSWTC)
                START_OUTPUT_XON   = 8,   // XON  (Ctrl+Q, restart output) (VSTART)
                STOP_OUTPUT_XOFF    = 9,   // XOFF (Ctrl+S, stop output) (VSTOP)
                SUSPEND_SIGNAL  = 10,  // Suspend (Ctrl+Z -> SIGTSTP) (VSUSP)
                END_OF_LINE_ALTERNATE     = 11,  // Alternate EOL (canonical) (VEOL)
                REPRINT_UNREAD_INPUT = 12,  // Reprint unread input (Ctrl+R) (VREPRINT)
                TOGGLE_OUTPUT_DISCARD = 13,  // Toggle output discard (Ctrl+O) (VDISCARD)
                ERASE_WORD_BACKWARD  = 14,  // Erase word backward (Ctrl+W) (VWERASE)
                LITERAL_NEXT_CHARACTER   = 15,  // Literal next char (Ctrl+V) (VLNEXT)
                END_OF_LINE_SECONDARY    = 16,  // Second alternate EOL (VEOL2)
            };

            using modeFlags = GGUI::terminal::ecma::table::mode::flags<GGUI::terminal::ecma::table::mode::types>;
            using modeBase = GGUI::terminal::ecma::table::mode::base<GGUI::terminal::ecma::table::mode::types>;

            struct configuration {
                bitMask<inputFlags> iflag;                              // Input  processing flags (hardware)
                bitMask<outputFlags> oflag;                             // Output processing flags (hardware)
                bitMask<hardwareControlFlags> cflag;                    // Line   control flags    (baud, parity, framing)
                modeFlags modes;                                        // ECMA-48 mode flags (replaces c_lflag)
                uint8_t     line;                                       // Line discipline (N_TTY = 0)
                uint8_t cc_chars[CC_COUNT];                             // Special character bindings
                bitMask<hardwareControlFlags> ispeed;                   // Input  baud rate
                bitMask<hardwareControlFlags> ospeed;                   // Output baud rate

                configuration() {
                    // Hardware input: translate CR->NL, enable UTF-8
                    iflag = bitMask<inputFlags>(inputFlags::TRANSLATE_CR_TO_NL_ON_INPUT) | inputFlags::INPUT_IS_UTF8;

                    // Hardware output: translate NL->CR+NL (standard cooked output)
                    oflag = bitMask<outputFlags>(outputFlags::ENABLE_ALL_POSTPROCESSING) | outputFlags::TRANSLATE_NEWLINE_TO_CARRIAGE_RETURN_NEWLINE;

                    // Hardware line: 8-bit chars, enable receiver, ignore modem lines
                    cflag = bitMask<hardwareControlFlags>(hardwareControlFlags::CHARACTER_SIZE_8_DATA_BITS) | hardwareControlFlags::CHARACTER_READ_ENABLE | hardwareControlFlags::CONTROL_LOCAL_IGNORE_MODEM_STATUS_LINES;

                    modes.set(ecma::table::mode::presets::CRM_CONTROL);      // control chars processed (not treated as graphic)
                    modes.set(ecma::table::mode::presets::SRM_MONITOR);      // echo on (locally entered data is imaged)
                    modes.set(ecma::table::mode::presets::KAM_ENABLED);      // keyboard active (Ctrl+C still generates SIGINT)

                    cc_chars[(size_t)specialCharacterIndicies::NON_CANONICAL_MINIMUM_BYTES_BEFORE_READ]  = 1;      // return after 1 byte
                    cc_chars[(size_t)specialCharacterIndicies::NON_CANONICAL_READ_TIMEOUT] = 0;      // no timeout

                    ispeed = hardwareControlFlags::BAUD_9600;
                    ospeed = hardwareControlFlags::BAUD_9600;
                }
            };
        }

        INTERNAL::configuration currentConfiguration;
        INTERNAL::configuration previousConfiguration;

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

            // ECHO -> SRM MONITOR (RESET = echo on) / SIMULTANEOUS (SET = echo off)
            cfg.modes.set(INTERNAL::modeBase{
                ecma::table::mode::types::SEND_RECEIVE_MODE,
                (t.c_lflag & ECHO) ? ecma::table::mode::definition::RESET : ecma::table::mode::definition::SET
            });

            // ISIG -> KAM ENABLED (RESET) / DISABLED (SET)
            cfg.modes.set(INTERNAL::modeBase{
                ecma::table::mode::types::KEYBOARD_ACTION_MODE,
                (t.c_lflag & ISIG) ? ecma::table::mode::definition::RESET : ecma::table::mode::definition::SET
            });

            // ICANON -> CRM CONTROL (RESET = processed) / GRAPHIC (SET = raw)
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

        bool snapshot() {
            // save the current to previous
            previousConfiguration = currentConfiguration;

            termios t;
            if (tcgetattr(STDIN_FILENO, &t) != 0) return false;
            currentConfiguration = fromTermios(t);
            return true;
        }

        bool apply() {
            termios t = toTermios(currentConfiguration);
            return tcsetattr(STDIN_FILENO, TCSAFLUSH, &t) == 0;
        }

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