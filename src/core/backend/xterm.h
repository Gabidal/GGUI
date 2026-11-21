#ifndef _XTERM_H_
#define _XTERM_H_

#include "ecma.h"
#include "utils.h"
#include "../utils/utils.h"

#include <unordered_map>

namespace GGUI::terminal::xterm {
    extern ecma::configuration::cellPatch csiPatch;

    namespace table {
        constexpr auto toInt = ecma::table::toInt;

        enum class privateModeTypes : uint16_t {
            DECCKM      = 1,                        //  Application Cursor Keys (DECCKM), VT100.
            DECANM      = 2,                        //  Designate USASCII for character sets G0-G3 (DECANM), VT100, and set VT100 mode.
            DECCOLM     = 3,                        //  132 Column Mode (DECCOLM), VT100.
            DECSCLM     = 4,                        //  Smooth (Slow) Scroll (DECSCLM), VT100.
            DECSCNM     = 5,                        //  Reverse Video (DECSCNM), VT100.
            DECOM       = 6,                        //  Origin Mode (DECOM), VT100.
            DECAWM      = 7,                        //  Auto-Wrap Mode (DECAWM), VT100.
            DECARM      = 8,                        //  Auto-Repeat Keys (DECARM), VT100.
            sendMouseXY = 9,                        //  Send Mouse X & Y on button press.  See the section Mouse Tracking.  This is the X10 xterm mouse protocol.
            showToolbar        = 10,                //  Show toolbar (rxvt).
            startBlinkingCursor = 12,               //  Start blinking cursor (AT&T 610).
            startBlinkingCursorReadOnly = 13,       //  Start blinking cursor (read-only, set only via resource or menu).
            enableXOR   = 14,                       //  Enable XOR of blinking cursor control sequence and menu (read-only, set via resource).
            DECPFF      = 18,                       //  Print Form Feed (DECPFF), VT220.
            DECPEX      = 19,                       //  Set print extent to full screen (DECPEX), VT220.
            DECTCEM     = 25,                       //  Show cursor (DECTCEM), VT220.
            showScrollbar   = 30,                   //  Show scrollbar (rxvt).
            enableFontShift = 35,                   //  Enable font-shifting functions (rxvt).
            DECTEK      = 38,                       //  Enter Tektronix mode (DECTEK), VT240, xterm.
            allow_80_and_132_modes  = 40,           //  Allow 80 ⇒  132 mode, xterm.
            moreFix = 41,                           //  more(1) fix (see curses resource).
            DECNRCM = 42,                           //  Enable National Replacement Character sets (DECNRCM), VT220.
            DECGEPM = 43,                           //  Enable Graphic Expanded Print Mode (DECGEPM), VT340.
            turnMarginBell = 44,                    //  Turn on margin bell, xterm.
            DECGPCM = 44,                           //  Enable Graphic Print Color Mode (DECGPCM), VT340.
            XTREVWRAP = 45,                         //  Reverse-wraparound mode (XTREVWRAP), xterm.
            DECGPCS = 45,                           //  Enable Graphic Print Color Syntax (DECGPCS), VT340.
            XTLOGGING = 46,                         //  Start logging (XTLOGGING), xterm.  This is normally disabled by a compile-time option.
            graphicPrintBackgroundMode = 46,        //  Graphic Print Background Mode, VT340.
            useAlternateScreenBuffer = 47,          //  Use Alternate Screen Buffer, xterm.  This may be disabled by the titeInhibit resource.
            DECGRPM = 47,                           //  Enable Graphic Rotated Print Mode (DECGRPM), VT340.
            DECNKM = 66,                            //  Application keypad mode (DECNKM), VT320.
            DECBKM = 67,                            //  Backarrow key sends backspace (DECBKM), VT340, VT420.  This sets the backarrowKey resource to "true".
            DECLRMM = 69,                            //  Enable left and right margin mode (DECLRMM), VT420 and up.
            DECSDM = 80,                            //  Enable Sixel Display Mode (DECSDM), VT330, VT340, VT382.
            DECNCSM = 95,                            //  Do not clear screen when DECCOLM is set/reset (DECNCSM), VT510 and up.
            sendMouseXYOnPressAndRelease = 1000,            //  Send Mouse X & Y on button press and release.  See the section Mouse Tracking.  This is the X11 xterm mouse protocol.
            useHiliteMouseTracking = 1001,                //  Use Hilite Mouse Tracking, xterm.
            useCellMotionMouseTracking = 1002,            //  Use Cell Motion Mouse Tracking, xterm.  See the section Button-event tracking.
            useAllMotionMouseTracking = 1003,            //  Use All Motion Mouse Tracking, xterm.  See the section Any-event tracking.
            sendFocusEvents = 1004,                    //  Send FocusIn/FocusOut events, xterm.
            enableUTF8MouseMode = 1005,                //  Enable UTF-8 Mouse Mode, xterm.
            enableSGRMouseMode = 1006,                //  Enable SGR Mouse Mode, xterm.
            enableAlternateScrollMode = 1007,            //  Enable Alternate Scroll Mode, xterm.  This corresponds to the alternateScroll resource.
            scrollToBottomOnTTYOutput = 1010,            //  Scroll to bottom on tty output (rxvt). This sets the scrollTtyOutput resource to "true".
            scrollToBottomOnKeyPress = 1011,            //  Scroll to bottom on key press (rxvt).  This sets the scrollKey resource to "true".
            enableFastScroll = 1014,                //  Enable fastScroll resource, xterm.
            enableUrxvtMouseMode = 1015,                //  Enable urxvt Mouse Mode.
            enableSGRMousePixelMode = 1016,                //  Enable SGR Mouse PixelMode, xterm.
            utf8ReadOnly = 1020,                    //  utf8 resource (read-only), xterm.
            cjkWidthReadOnly = 1021,                //  cjkWidth resource (read-only), xterm.
            emojiWidthReadOnly = 1022,                //  emojiWidth resource (read-only), xterm.
            privateWidthReadOnly = 1023,                //  privateWidth resource (read-only), xterm.
            interpretMetaKey = 1034,                //  Interpret "meta" key, xterm.  This sets the eighth bit of keyboard input (and enables the eightBitInput resource).
            enableSpecialAltAndNumLockModifiers = 1035,        //  Enable special modifiers for Alt and NumLock keys, xterm.  This enables the numLock resource.
            metaSendsEscape = 1036,                    //  Send ESC   when Meta modifies a key, xterm. This enables the metaSendsEscape resource.
            deleteKeySendsDEL = 1037,                //  Send DEL from the editing-keypad Delete key, xterm.
            altSendsEscape = 1039,                    //  Send ESC  when Alt modifies a key, xterm. This enables the altSendsEscape resource, xterm.
            keepSelection = 1040,                    //  Keep selection even if not highlighted, xterm.  This enables the keepSelection resource.
            selectToClipboard = 1041,                //  Use the CLIPBOARD selection, xterm.  This enables the selectToClipboard resource.
            bellIsUrgent = 1042,                    //  Enable Urgency window manager hint when Control-G is received, xterm.  This enables the bellIsUrgent resource.
            popOnBell = 1043,                    //  Enable raising of the window when Control-G is received, xterm.  This enables the popOnBell resource.
            keepClipboard = 1044,                    //  Reuse the most recent data copied to CLIPBOARD, xterm.  This enables the keepClipboard resource.
            XTREVWRAP2 = 1045,                    //  Extended Reverse-wraparound mode (XTREVWRAP2), xterm.
            enableAlternateScreenBufferSwitching = 1046,        //  Enable switching to/from Alternate Screen Buffer, xterm.  This works for terminfo-based systems, updating the titeInhibit resource.
            useAlternateScreenBuffer1047 = 1047,    //  Use Alternate Screen Buffer, xterm.  This may be disabled by the titeInhibit resource.
            saveCursor = 1048,                        //  Save cursor as in DECSC, xterm.  This may be disabled by the titeInhibit resource.
            saveCursorAndUseAlternateScreenBuffer = 1049,        /*  Save cursor as in DECSC, xterm.
                                                                    After saving the cursor, switch to the Alternate Screen Buffer, clearing it first.
                                                                    This may be disabled by the titeInhibit resource.
                                                                    This control combines the effects of the 1 0 4 7 and 1 0 4 8  modes.
                                                                    Use this with terminfo-based applications rather than the 4 7  mode. 
                                                                */
            setTerminfoFunctionKeyMode = 1050,            //  Set terminfo/termcap function-key mode, xterm.
            setSunFunctionKeyMode = 1051,                //  Set Sun function-key mode, xterm.
            setHPFunctionKeyMode = 1052,                //  Set HP function-key mode, xterm.
            setSCOFunctionKeyMode = 1053,                //  Set SCO function-key mode, xterm.
            setLegacyKeyboardEmulation = 1060,            //  Set legacy keyboard emulation, i.e, X11R6, xterm.
            setVT220KeyboardEmulation = 1061,            //  Set VT220 keyboard emulation, xterm.
            enableReadlineMouseButton1 = 2001,            //  Enable readline mouse button-1, xterm.
            enableReadlineMouseButton2 = 2002,            //  Enable readline mouse button-2, xterm.
            enableReadlineMouseButton3 = 2003,            //  Enable readline mouse button-3, xterm.
            setBracketedPasteMode = 2004,                //  Set bracketed paste mode, xterm.
            enableReadlineCharacterQuoting = 2005,            //  Enable readline character-quoting, xterm.
            enableReadlineNewlinePasting = 2006,            //  Enable readline newline pasting, xterm.
        };
    
        enum class finalWithoutIntermediate : uint8_t {
            __min = toInt(4, 0),  // For internal automation
    
            // Column 4                                 // Column 6
            MOUSE_PRESS     = toInt(4, 13),             MOUSE_RELEASE   = toInt(6, 13)
    
        };

    }

    namespace sequences {
        using namespace ecma::sequences;
        
        namespace miscellaneousControlFunctions {
            extern void operate_DEVICE_ATTRIBUTES(ecma::sequence::base*);
            
            /**
             * @brief override of ecma DA function, by:
             * Invoked with: `CSI > 0 c` or `CSI > c`
             * Responses with: `CSI > Pp ; Pv ; Pc c`
             * where Pp denotes the terminal type and Pv is the firmware version (for xterm, this was originally the XFree86 patch number, starting with 95).
             * In a DEC terminal, Pc indicates the ROM cartridge registration number and is always zero.
             */
            inline auto DEVICE_ATTRIBUTES = makeSequencePatch<specialTypes::HAS_INFINITE_PARAMETERS>(
                ecma::sequence::control<ecma::sequence::parameter::numeric, ecma::table::finalWithoutIntermediate>(ecma::table::finalWithoutIntermediate::DA),
                operate_DEVICE_ATTRIBUTES,
                &csiPatch,
                0
            );
        }

        namespace modeSettingFunctions {
            extern void operate_RESET_PRIVATE_MODE(ecma::sequence::base*);
            extern void operate_SET_PRIVATE_MODE(ecma::sequence::base*);

            inline auto RESET_PRIVATE_MODE = makeSequencePatch<specialTypes::HAS_INFINITE_PARAMETERS>(
                ecma::sequence::control<ecma::sequence::parameter::selectable<table::privateModeTypes>, ecma::table::finalWithoutIntermediate>(ecma::table::finalWithoutIntermediate::RM),
                operate_RESET_PRIVATE_MODE,
                &csiPatch,
                (table::privateModeTypes)0
            );

            inline auto SET_PRIVATE_MODE = makeSequencePatch<specialTypes::HAS_INFINITE_PARAMETERS>(
                ecma::sequence::control<ecma::sequence::parameter::selectable<table::privateModeTypes>, ecma::table::finalWithoutIntermediate>(ecma::table::finalWithoutIntermediate::SM),
                operate_SET_PRIVATE_MODE,
                &csiPatch,
                (table::privateModeTypes)0
            );
        }

        namespace mouseFunctions {
            extern void operate_MOUSE_PRESS(ecma::sequence::base*);
            extern void operate_MOUSE_RELEASE(ecma::sequence::base*);

            /**
             * @brief SGR (1006) mouse encoding reports button presses, motion and wheel events.
             * @example `CSI < Cb ; Cx ; Cy M`. Cb uses mouseButtonTypes bits without the legacy +32 offset. 
             * Coordinates start at 1.
             */
            inline auto MOUSE_PRESS = makeSequencePatch(
                ecma::sequence::control<ecma::sequence::parameter::numeric, table::finalWithoutIntermediate>(table::finalWithoutIntermediate::MOUSE_PRESS),
                operate_MOUSE_PRESS, &csiPatch, 0, 1, 1
            );

            /**
             * @brief SGR (1006) mouse releases retain the released button's number.
             * @example `CSI < Cb ; Cx ; Cy m`. The lowercase final byte indicates release; 
             * coordinates and modifiers have the same meaning as on press.
             */
            inline auto MOUSE_RELEASE = makeSequencePatch(
                ecma::sequence::control<ecma::sequence::parameter::numeric, table::finalWithoutIntermediate>(table::finalWithoutIntermediate::MOUSE_RELEASE),
                operate_MOUSE_RELEASE, &csiPatch, 0, 1, 1
            );
        }    
    }

    struct alignas(128) components {
        std::unordered_map<uint16_t, bool> enabledModes;
        bool enabled = false;

        bool verifyExtensions(ecma::sequence::base* parsed);
        
        void registerPatches();
    };
}

#endif