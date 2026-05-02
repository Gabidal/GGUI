#ifndef _TERMINAL_H_
#define _TERMINAL_H_

#include <functional>
#include <cstdint>
#include <chrono>

#include "ascii.h"
#include "ecma.h"
#include "../utils/types.h"

namespace GGUI {
    /**
     * Contains the interface for terminal handling in terminal emulators.
    */
    namespace terminal {

        template<typename T> using bitMask = GGUI::INTERNAL::bitMask<T>;

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

                configuration();
            };
        }

        enum class features : uint16_t {
            NONE                = 0,
            READ                = 1 << 0,
            WRITE               = 1 << 1,

            TTY                 = 1 << 2,

            PIPED_IN            = 1 << 3,
            PIPED_OUT           = 1 << 4,
            
            REDIRECTED_IN       = 1 << 5,
            REDIRECTED_OUT      = 1 << 6,

            FULL_COLOR          = 1 << 7,       // If not enabled and TTY enabled, then we need to use ECMA to determine the correct color mode and clap ther RGB into it.
            EXTENDED_MOUSE      = 1 << 8,       // If enabled, then uint16_t, if not then: uint8_t - 22 range.
            // ...
        };

        extern GGUI::INTERNAL::bitMask<features> enabledFeatures;

        struct query {
            // Some compile time constants; 510, is enough. If need raise this.
            static const unsigned int capacity = UINT8_MAX * 2;

            std::array<unsigned char, capacity> buffer;
            unsigned int size = 0;
        };
        extern query inputQuery;
        
        struct device {
            // Namespace like structuring of code, because why not :)
            struct button {
                // Simple value + time snapshot
                bool state; std::chrono::steady_clock::time_point captureTime;
                // Capture time as creation
                button(bool State = false) : state(State), captureTime(std::chrono::steady_clock::now()) {}
            };

            struct wheel {
                signed char Scalar = 0;     // going up > 0 | going down < 0
            };

            struct {
                button left, right, middle;
                wheel scroll;               // Only for vertical
                IVector2 position;          // Absolute
            } mouse;

            std::array<button, (size_t)ASCII::table::MAX_VALUE> keyboard;
        };

        // Read from this to get current device states of the terminal peripherals.
        extern device currentStates;
        extern INTERNAL::configuration currentConfiguration;
        extern INTERNAL::configuration previousConfiguration;
        // extern device previousStates;    // Only accessible inside the terminal.cpp for internal use only!

        extern bitMask<features> fetchIOPermissions();

        extern bool snapshot(INTERNAL::configuration& cfg);

        extern bool apply(const INTERNAL::configuration& cfg);

        extern void init();                 // non-Platform Specific
        
        extern void startProbing();         // Terminal Specific

        extern void deinit();               // Terminal Specific

        extern void waitForInput();         // Platform Specific

        extern void parseInput();           // Terminal Specific

        // This is a general function whose job is to post GGUI input events for event handlers to catch.
        extern void postInputs();           // General
    
    }
}


#endif