#ifndef _CONSTANTS_H_
#define _CONSTANTS_H_

#include <string>
#include <chrono>
#include <stdexcept>

#include "superString.h"

namespace GGUI{
    namespace TIME{
        constexpr  unsigned int MILLISECOND = 1; 
        constexpr  unsigned int SECOND = MILLISECOND * 1000;
        constexpr  unsigned int MINUTE = SECOND * 60;
        constexpr  unsigned int HOUR = MINUTE * 60;
    }

    namespace constants{
        namespace ANSI{
            // 1 to ESC_CODE
            // 1 to Text_Color | Background_Color
            // 1 to SEPARATE
            // 1 to USE_RGB
            // 1 to SEPARATE
            constexpr unsigned int maximumNeededPreAllocationForOverHead = 1 + 1 + 1 + 1 + 1;

            // 1 to maximumNeededPreAllocationForOverHead as compressed single compactString
            constexpr unsigned int maximumNeededPreAllocationForCompressedOverHead = 1;

            // 1 to Red
            // 1 to SEPARATE
            // 1 to Green
            // 1 to SEPARATE
            // 1 to Blue
            constexpr unsigned int maximumNeededPreAllocationForColor = 1 + 1 + 1 + 1 + 1;
            
            // 1 to Text_Overhead   (5 compressed into 1)
            // 5 to Text_Colour
            // 1 to END_COMMAND
            // 1 to Background_Overhead   (5 compressed into 1)
            // 5 to Background_Colour
            // 1 to END_COMMAND
            constexpr unsigned int maximumNeededPreAllocationForOverhead = 
                maximumNeededPreAllocationForCompressedOverHead + maximumNeededPreAllocationForColor + 1 +
                maximumNeededPreAllocationForCompressedOverHead + maximumNeededPreAllocationForColor + 1;

            // 1 to RESET_COLOR
            constexpr unsigned int maximumNeededPreAllocationForReset = 1;

            // 1 to Text_Overhead   (5 compressed into 1)
            // 5 to Text_Colour
            // 1 to END_COMMAND
            // 1 to Background_Overhead   (5 compressed into 1)
            // 5 to Background_Colour
            // 1 to END_COMMAND
            // 1 to Data
            // 1 to RESET_COLOR
            constexpr unsigned int maximumNeededPreAllocationForEncodedSuperString = maximumNeededPreAllocationForOverhead + 1 + maximumNeededPreAllocationForReset;

            // 1 to Escape code
            // 1 to private SGR telltale '?'
            // 1 to Feature to be disabled or enabled
            // 1 to Enable/Disable feature told above.
            constexpr unsigned int maximumNeededPreAllocationForEnablingOrDisablingPrivateSGRFeature = 1 + 1 + 1 + 1;
        
            // 1 to Escape code
            // 1 to feature to be enabled
            // 1 to END_COMMAND
            constexpr unsigned int maximumNeededPreAllocationForEnablingOrDisablingSGRFeature = 1 + 1 + 1;
            
            // 1 to Escape code
            // 1 to Mouse appearance
            constexpr unsigned int maximumNeededPreAllocationForSettingCursorShape = 1 + 1;

            // CSI (Control Sequence Introducer) sequences.
            constexpr INTERNAL::compactString ESC_CODE = "\x1B[";      // Also known as \e[ or \o33
            constexpr INTERNAL::compactString SEPARATE = ';';
            constexpr INTERNAL::compactString USE_RGB = '2';
            constexpr INTERNAL::compactString END_COMMAND = 'm';
            constexpr INTERNAL::compactString RESET_COLOR = "\x1B[0m";  // Basically same as RESET_SGR but baked the end command into it for Super_String
            constexpr INTERNAL::compactString CLEAR_SCREEN = "\x1B[2J";
            constexpr INTERNAL::compactString CLEAR_SCROLLBACK = "\x1B[3J";
            constexpr INTERNAL::compactString SET_CURSOR_TO_START = "\x1B[H";
            constexpr INTERNAL::compactString RESET_CONSOLE = "\x1B[c";
            // Cursor shape (DECSCUSR) fragments (value + space + 'q').
            constexpr INTERNAL::compactString CURSOR_SHAPE_BLINKING_BLOCK = "0 q";     // Also default
            constexpr INTERNAL::compactString CURSOR_SHAPE_STEADY_BLOCK = "2 q";
            constexpr INTERNAL::compactString CURSOR_SHAPE_BLINKING_UNDERLINE = "3 q";
            constexpr INTERNAL::compactString CURSOR_SHAPE_STEADY_UNDERLINE = "4 q";
            constexpr INTERNAL::compactString CURSOR_SHAPE_BLINKING_BAR = "5 q";
            constexpr INTERNAL::compactString CURSOR_SHAPE_STEADY_BAR = "6 q";

            /**
             * @brief Enable or disable a private SGR feature
             * 
             * @param command The SGR command to enable or disable
             * @param Enable If true, enable the feature. Otherwise, disable it.
             * @return A Super_String object with the correct escape sequence to enable or disable the feature
             */
            constexpr INTERNAL::superString<maximumNeededPreAllocationForEnablingOrDisablingPrivateSGRFeature> enablePrivateSGRFeature(const INTERNAL::compactString& command, bool Enable = true) { 
                INTERNAL::superString<maximumNeededPreAllocationForEnablingOrDisablingPrivateSGRFeature> Result;

                // Add the escape code
                Result.add(ESC_CODE);

                // Add the private SGR telltale '?'
                Result.add('?');

                // Add the command to enable or disable
                Result.add(command);

                // Finally, add the enable or disable code
                if (Enable)
                    Result.add('h');
                else
                    Result.add('l');

                return Result;
            }


            // SGR (Select Graphic Rendition)
            
            /**
             * @brief Enable a SGR feature
             * 
             * @param command The command to enable
             * @return A Super_String object with the correct escape sequence to enable the feature
             */
            constexpr INTERNAL::superString<maximumNeededPreAllocationForEnablingOrDisablingSGRFeature> enableSGRFeature(const INTERNAL::compactString& command) {
                INTERNAL::superString<maximumNeededPreAllocationForEnablingOrDisablingSGRFeature> Result;

                // Add the escape code
                Result.add(ESC_CODE);

                // Add the command to enable
                Result.add(command);

                // Add the end command
                Result.add(END_COMMAND);

                return Result;
            }

            // Build a cursor shape control sequence: ESC[ <Ps> q where fragment already provides "<digit> q".
            constexpr INTERNAL::superString<maximumNeededPreAllocationForSettingCursorShape> setCursorShape(const INTERNAL::compactString& fragment){
                INTERNAL::superString<maximumNeededPreAllocationForSettingCursorShape> Result;
                Result.add(ESC_CODE);
                Result.add(fragment);
                return Result;
            }

            // SGR constants
            constexpr INTERNAL::compactString RESET_SGR = "0";                                  // Removes all SGR features. 
            constexpr INTERNAL::compactString BOLD = "1";                                       // Not widely supported!
            constexpr INTERNAL::compactString FAINT = "2";                                      // Not widely supported!
            constexpr INTERNAL::compactString ITALIC = "3";                                     // Not widely supported! (Can also be same as blink)
            constexpr INTERNAL::compactString UNDERLINE = "4";              
            constexpr INTERNAL::compactString SLOW_BLINK = "5";                                 // ~150 BPM
            constexpr INTERNAL::compactString RAPID_BLINK = "6";                                // Not widely supported!
            constexpr INTERNAL::compactString INVERT_FOREGROUND_WITH_BACKGROUND = "7";          // Not widely supported!
            constexpr INTERNAL::compactString CONCEAL = "8";                                    // Not widely supported!
            constexpr INTERNAL::compactString CROSSED_OUT = "9";                                // Not widely supported!
            constexpr INTERNAL::compactString PRIMARY_FONT = "10";                              // Sets the default font.
            constexpr INTERNAL::compactString ALTERNATIVE_FONT_1 = "11";                        // Custom font slot.
            constexpr INTERNAL::compactString ALTERNATIVE_FONT_2 = "12";                        // Custom font slot.
            constexpr INTERNAL::compactString ALTERNATIVE_FONT_3 = "13";                        // Custom font slot.
            constexpr INTERNAL::compactString ALTERNATIVE_FONT_4 = "14";                        // Custom font slot.
            constexpr INTERNAL::compactString ALTERNATIVE_FONT_5 = "15";                        // Custom font slot.
            constexpr INTERNAL::compactString ALTERNATIVE_FONT_6 = "16";                        // Custom font slot.
            constexpr INTERNAL::compactString ALTERNATIVE_FONT_7 = "17";                        // Custom font slot.
            constexpr INTERNAL::compactString ALTERNATIVE_FONT_8 = "18";                        // Custom font slot.
            constexpr INTERNAL::compactString ALTERNATIVE_FONT_9 = "19";                        // Custom font slot.
            constexpr INTERNAL::compactString FRAKTUR = "20";                                   // Not widely supported! (But cool font)
            constexpr INTERNAL::compactString NOT_BOLD = "21";                                  // Removes the BOLD feature
            constexpr INTERNAL::compactString NORMAL_INTENSITY = "22";                          // Removes BOLD and ITALIC and other affixes.
            constexpr INTERNAL::compactString NOT_UNDERLINE = "23";                             // Removes UNDERLINE.
            constexpr INTERNAL::compactString NOT_BLINK = "24";                                 // Removes BLINK.
            constexpr INTERNAL::compactString INVERT_INVERT_FOREGROUND_WITH_BACKGROUND = "27";  // Inverts the INVERT_FOREGROUND_WITH_BACKGROUND.
            constexpr INTERNAL::compactString TEXT_COLOR = "38";                               // Sets the foreground color.
            constexpr INTERNAL::compactString DEFAULT_TEXT_COLOR = "39";                       // Sets the default color.
            constexpr INTERNAL::compactString BACKGROUND_COLOR = "48";                         // Sets the background color.
            constexpr INTERNAL::compactString DEFAULT_BACKGROUND_COLOR = "49";                 // Sets the default color.

            // Private SGR codes
            constexpr INTERNAL::compactString SET_X10_MOUSE = "9";
            constexpr INTERNAL::compactString SET_VT200_MOUSE = "1000";
            constexpr INTERNAL::compactString REPORT_MOUSE_BUTTON_WHILE_MOVING = "1002";
            constexpr INTERNAL::compactString REPORT_MOUSE_ALL_EVENTS = "1003";
            constexpr INTERNAL::compactString EXTEND_TO_SGR_MODE = "1006";

            constexpr INTERNAL::compactString MOUSE_CURSOR = "25";
            constexpr INTERNAL::compactString SCREEN_CAPTURE = "47"; // 47l = restores screen, 47h = saves screen
            constexpr INTERNAL::compactString ALTERNATIVE_SCREEN_BUFFER = "1049"; // 1049l = disables alternative buffer, 1049h = enables alternative buffer
            // End of enable settings for ANSI

            // ACC (ASCII Control Characters)
            constexpr char NONE = 0;
            constexpr char START_OF_HEADING = 1;
            constexpr char START_OF_TEXT = 2;
            constexpr char END_OF_TEXT = 3;
            constexpr char END_OF_TRANSMISSION = 4;
            constexpr char ENQUIRY = 5;
            constexpr char ACKNOWLEDGE = 6;
            constexpr char BELL = 7;
            constexpr char BACKSPACE = 8;
            constexpr char HORIZONTAL_TAB = 9;
            constexpr char LINE_FEED = 10;             // Also known as newline
            constexpr char VERTICAL_TAB = 11;
            constexpr char FORM_FEED = 12;
            constexpr char CARRIAGE_RETURN = 13;
            constexpr char SHIFT_OUT = 14;
            constexpr char SHIFT_IN = 15;
            constexpr char DATA_LINK_ESCAPE = 16;
            constexpr char DEVICE_CONTROL_1 = 17;
            constexpr char DEVICE_CONTROL_2 = 18;
            constexpr char DEVICE_CONTROL_3 = 19;
            constexpr char DEVICE_CONTROL_4 = 20;
            constexpr char NEGATIVE_ACKNOWLEDGE = 21;
            constexpr char SYNCHRONOUS_IDLE = 22;
            constexpr char END_OF_TRANSMISSION_BLOCK = 23;
            constexpr char CANCEL = 24;
            constexpr char END_OF_MEDIUM = 25;
            constexpr char SUBSTITUTE = 26;
            constexpr char ESCAPE = 27;
            constexpr char FILE_SEPARATOR = 28;
            constexpr char GROUP_SEPARATOR = 29;
            constexpr char RECORD_SEPARATOR = 30;
            constexpr char UNIT_SEPARATOR = 31;
            constexpr char DEL = 127;

            constexpr const INTERNAL::compactString toCompactTable[256] = {
                INTERNAL::compactString("0", 1), INTERNAL::compactString("1", 1), INTERNAL::compactString("2", 1), INTERNAL::compactString("3", 1), INTERNAL::compactString("4", 1), INTERNAL::compactString("5", 1), INTERNAL::compactString("6", 1), INTERNAL::compactString("7", 1), INTERNAL::compactString("8", 1), INTERNAL::compactString("9", 1),
                INTERNAL::compactString("10", 2, true), INTERNAL::compactString("11", 2, true), INTERNAL::compactString("12", 2, true), INTERNAL::compactString("13", 2, true), INTERNAL::compactString("14", 2, true), INTERNAL::compactString("15", 2, true), INTERNAL::compactString("16", 2, true), INTERNAL::compactString("17", 2, true), INTERNAL::compactString("18", 2, true), INTERNAL::compactString("19", 2, true),
                INTERNAL::compactString("20", 2, true), INTERNAL::compactString("21", 2, true), INTERNAL::compactString("22", 2, true), INTERNAL::compactString("23", 2, true), INTERNAL::compactString("24", 2, true), INTERNAL::compactString("25", 2, true), INTERNAL::compactString("26", 2, true), INTERNAL::compactString("27", 2, true), INTERNAL::compactString("28", 2, true), INTERNAL::compactString("29", 2, true),
                INTERNAL::compactString("30", 2, true), INTERNAL::compactString("31", 2, true), INTERNAL::compactString("32", 2, true), INTERNAL::compactString("33", 2, true), INTERNAL::compactString("34", 2, true), INTERNAL::compactString("35", 2, true), INTERNAL::compactString("36", 2, true), INTERNAL::compactString("37", 2, true), INTERNAL::compactString("38", 2, true), INTERNAL::compactString("39", 2, true),
                INTERNAL::compactString("40", 2, true), INTERNAL::compactString("41", 2, true), INTERNAL::compactString("42", 2, true), INTERNAL::compactString("43", 2, true), INTERNAL::compactString("44", 2, true), INTERNAL::compactString("45", 2, true), INTERNAL::compactString("46", 2, true), INTERNAL::compactString("47", 2, true), INTERNAL::compactString("48", 2, true), INTERNAL::compactString("49", 2, true),
                INTERNAL::compactString("50", 2, true), INTERNAL::compactString("51", 2, true), INTERNAL::compactString("52", 2, true), INTERNAL::compactString("53", 2, true), INTERNAL::compactString("54", 2, true), INTERNAL::compactString("55", 2, true), INTERNAL::compactString("56", 2, true), INTERNAL::compactString("57", 2, true), INTERNAL::compactString("58", 2, true), INTERNAL::compactString("59", 2, true),
                INTERNAL::compactString("60", 2, true), INTERNAL::compactString("61", 2, true), INTERNAL::compactString("62", 2, true), INTERNAL::compactString("63", 2, true), INTERNAL::compactString("64", 2, true), INTERNAL::compactString("65", 2, true), INTERNAL::compactString("66", 2, true), INTERNAL::compactString("67", 2, true), INTERNAL::compactString("68", 2, true), INTERNAL::compactString("69", 2, true),
                INTERNAL::compactString("70", 2, true), INTERNAL::compactString("71", 2, true), INTERNAL::compactString("72", 2, true), INTERNAL::compactString("73", 2, true), INTERNAL::compactString("74", 2, true), INTERNAL::compactString("75", 2, true), INTERNAL::compactString("76", 2, true), INTERNAL::compactString("77", 2, true), INTERNAL::compactString("78", 2, true), INTERNAL::compactString("79", 2, true),
                INTERNAL::compactString("80", 2, true), INTERNAL::compactString("81", 2, true), INTERNAL::compactString("82", 2, true), INTERNAL::compactString("83", 2, true), INTERNAL::compactString("84", 2, true), INTERNAL::compactString("85", 2, true), INTERNAL::compactString("86", 2, true), INTERNAL::compactString("87", 2, true), INTERNAL::compactString("88", 2, true), INTERNAL::compactString("89", 2, true),
                INTERNAL::compactString("90", 2, true), INTERNAL::compactString("91", 2, true), INTERNAL::compactString("92", 2, true), INTERNAL::compactString("93", 2, true), INTERNAL::compactString("94", 2, true), INTERNAL::compactString("95", 2, true), INTERNAL::compactString("96", 2, true), INTERNAL::compactString("97", 2, true), INTERNAL::compactString("98", 2, true), INTERNAL::compactString("99", 2, true),
                INTERNAL::compactString("100", 3, true), INTERNAL::compactString("101", 3, true), INTERNAL::compactString("102", 3, true), INTERNAL::compactString("103", 3, true), INTERNAL::compactString("104", 3, true), INTERNAL::compactString("105", 3, true), INTERNAL::compactString("106", 3, true), INTERNAL::compactString("107", 3, true), INTERNAL::compactString("108", 3, true), INTERNAL::compactString("109", 3, true),
                INTERNAL::compactString("110", 3, true), INTERNAL::compactString("111", 3, true), INTERNAL::compactString("112", 3, true), INTERNAL::compactString("113", 3, true), INTERNAL::compactString("114", 3, true), INTERNAL::compactString("115", 3, true), INTERNAL::compactString("116", 3, true), INTERNAL::compactString("117", 3, true), INTERNAL::compactString("118", 3, true), INTERNAL::compactString("119", 3, true),
                INTERNAL::compactString("120", 3, true), INTERNAL::compactString("121", 3, true), INTERNAL::compactString("122", 3, true), INTERNAL::compactString("123", 3, true), INTERNAL::compactString("124", 3, true), INTERNAL::compactString("125", 3, true), INTERNAL::compactString("126", 3, true), INTERNAL::compactString("127", 3, true), INTERNAL::compactString("128", 3, true), INTERNAL::compactString("129", 3, true),
                INTERNAL::compactString("130", 3, true), INTERNAL::compactString("131", 3, true), INTERNAL::compactString("132", 3, true), INTERNAL::compactString("133", 3, true), INTERNAL::compactString("134", 3, true), INTERNAL::compactString("135", 3, true), INTERNAL::compactString("136", 3, true), INTERNAL::compactString("137", 3, true), INTERNAL::compactString("138", 3, true), INTERNAL::compactString("139", 3, true),
                INTERNAL::compactString("140", 3, true), INTERNAL::compactString("141", 3, true), INTERNAL::compactString("142", 3, true), INTERNAL::compactString("143", 3, true), INTERNAL::compactString("144", 3, true), INTERNAL::compactString("145", 3, true), INTERNAL::compactString("146", 3, true), INTERNAL::compactString("147", 3, true), INTERNAL::compactString("148", 3, true), INTERNAL::compactString("149", 3, true),
                INTERNAL::compactString("150", 3, true), INTERNAL::compactString("151", 3, true), INTERNAL::compactString("152", 3, true), INTERNAL::compactString("153", 3, true), INTERNAL::compactString("154", 3, true), INTERNAL::compactString("155", 3, true), INTERNAL::compactString("156", 3, true), INTERNAL::compactString("157", 3, true), INTERNAL::compactString("158", 3, true), INTERNAL::compactString("159", 3, true),
                INTERNAL::compactString("160", 3, true), INTERNAL::compactString("161", 3, true), INTERNAL::compactString("162", 3, true), INTERNAL::compactString("163", 3, true), INTERNAL::compactString("164", 3, true), INTERNAL::compactString("165", 3, true), INTERNAL::compactString("166", 3, true), INTERNAL::compactString("167", 3, true), INTERNAL::compactString("168", 3, true), INTERNAL::compactString("169", 3, true),
                INTERNAL::compactString("170", 3, true), INTERNAL::compactString("171", 3, true), INTERNAL::compactString("172", 3, true), INTERNAL::compactString("173", 3, true), INTERNAL::compactString("174", 3, true), INTERNAL::compactString("175", 3, true), INTERNAL::compactString("176", 3, true), INTERNAL::compactString("177", 3, true), INTERNAL::compactString("178", 3, true), INTERNAL::compactString("179", 3, true),
                INTERNAL::compactString("180", 3, true), INTERNAL::compactString("181", 3, true), INTERNAL::compactString("182", 3, true), INTERNAL::compactString("183", 3, true), INTERNAL::compactString("184", 3, true), INTERNAL::compactString("185", 3, true), INTERNAL::compactString("186", 3, true), INTERNAL::compactString("187", 3, true), INTERNAL::compactString("188", 3, true), INTERNAL::compactString("189", 3, true),
                INTERNAL::compactString("190", 3, true), INTERNAL::compactString("191", 3, true), INTERNAL::compactString("192", 3, true), INTERNAL::compactString("193", 3, true), INTERNAL::compactString("194", 3, true), INTERNAL::compactString("195", 3, true), INTERNAL::compactString("196", 3, true), INTERNAL::compactString("197", 3, true), INTERNAL::compactString("198", 3, true), INTERNAL::compactString("199", 3, true),
                INTERNAL::compactString("200", 3, true), INTERNAL::compactString("201", 3, true), INTERNAL::compactString("202", 3, true), INTERNAL::compactString("203", 3, true), INTERNAL::compactString("204", 3, true), INTERNAL::compactString("205", 3, true), INTERNAL::compactString("206", 3, true), INTERNAL::compactString("207", 3, true), INTERNAL::compactString("208", 3, true), INTERNAL::compactString("209", 3, true),
                INTERNAL::compactString("210", 3, true), INTERNAL::compactString("211", 3, true), INTERNAL::compactString("212", 3, true), INTERNAL::compactString("213", 3, true), INTERNAL::compactString("214", 3, true), INTERNAL::compactString("215", 3, true), INTERNAL::compactString("216", 3, true), INTERNAL::compactString("217", 3, true), INTERNAL::compactString("218", 3, true), INTERNAL::compactString("219", 3, true),
                INTERNAL::compactString("220", 3, true), INTERNAL::compactString("221", 3, true), INTERNAL::compactString("222", 3, true), INTERNAL::compactString("223", 3, true), INTERNAL::compactString("224", 3, true), INTERNAL::compactString("225", 3, true), INTERNAL::compactString("226", 3, true), INTERNAL::compactString("227", 3, true), INTERNAL::compactString("228", 3, true), INTERNAL::compactString("229", 3, true),
                INTERNAL::compactString("230", 3, true), INTERNAL::compactString("231", 3, true), INTERNAL::compactString("232", 3, true), INTERNAL::compactString("233", 3, true), INTERNAL::compactString("234", 3, true), INTERNAL::compactString("235", 3, true), INTERNAL::compactString("236", 3, true), INTERNAL::compactString("237", 3, true), INTERNAL::compactString("238", 3, true), INTERNAL::compactString("239", 3, true),
                INTERNAL::compactString("240", 3, true), INTERNAL::compactString("241", 3, true), INTERNAL::compactString("242", 3, true), INTERNAL::compactString("243", 3, true), INTERNAL::compactString("244", 3, true), INTERNAL::compactString("245", 3, true), INTERNAL::compactString("246", 3, true), INTERNAL::compactString("247", 3, true), INTERNAL::compactString("248", 3, true), INTERNAL::compactString("249", 3, true),
                INTERNAL::compactString("250", 3, true), INTERNAL::compactString("251", 3, true), INTERNAL::compactString("252", 3, true), INTERNAL::compactString("253", 3, true), INTERNAL::compactString("254", 3, true), INTERNAL::compactString("255", 3, true)
            };
        }

        constexpr unsigned long long NONE = (unsigned long long)1 << 0;
        constexpr unsigned long long ENTER = (unsigned long long)1 << 1;
        constexpr unsigned long long ESCAPE = (unsigned long long)1 << 2;
        constexpr unsigned long long BACKSPACE = (unsigned long long)1 << 3;
        constexpr unsigned long long TAB = (unsigned long long)1 << 4;
        constexpr unsigned long long UP = (unsigned long long)1 << 5;
        constexpr unsigned long long DOWN = (unsigned long long)1 << 6;
        constexpr unsigned long long LEFT = (unsigned long long)1 << 7;
        constexpr unsigned long long RIGHT = (unsigned long long)1 << 8;
        constexpr unsigned long long SPACE = (unsigned long long)1 << 9;
        constexpr unsigned long long SHIFT = (unsigned long long)1 << 10;
        constexpr unsigned long long ALT = (unsigned long long)1 << 11;
        constexpr unsigned long long CONTROL = (unsigned long long)1 << 12;
        constexpr unsigned long long SUPER = (unsigned long long)1 << 13;
        constexpr unsigned long long HOME = (unsigned long long)1 << 14;
        constexpr unsigned long long INSERT = (unsigned long long)1 << 15;
        constexpr unsigned long long DEL = (unsigned long long)1 << 16;
        constexpr unsigned long long END = (unsigned long long)1 << 17;
        constexpr unsigned long long PAGE_UP = (unsigned long long)1 << 18;
        constexpr unsigned long long PAGE_DOWN = (unsigned long long)1 << 19;
        constexpr unsigned long long F0 = (unsigned long long)1 << 20;
        constexpr unsigned long long F1 = (unsigned long long)1 << 21;
        constexpr unsigned long long F2 = (unsigned long long)1 << 22;
        constexpr unsigned long long F3 = (unsigned long long)1 << 23;
        constexpr unsigned long long F4 = (unsigned long long)1 << 24;
        constexpr unsigned long long F5 = (unsigned long long)1 << 25;
        constexpr unsigned long long F6 = (unsigned long long)1 << 26;
        constexpr unsigned long long F7 = (unsigned long long)1 << 27;
        constexpr unsigned long long F8 = (unsigned long long)1 << 28;
        constexpr unsigned long long F9 = (unsigned long long)1 << 29;
        constexpr unsigned long long F10 = (unsigned long long)1 << 30;
        constexpr unsigned long long F11 = (unsigned long long)1 << 31;
        constexpr unsigned long long F12 = (unsigned long long)1 << 32;
        constexpr unsigned long long F13 = (unsigned long long)1 << 33;
        constexpr unsigned long long F14 = (unsigned long long)1 << 34;
        constexpr unsigned long long F15 = (unsigned long long)1 << 35;
        constexpr unsigned long long F16 = (unsigned long long)1 << 36;

        // Should not fucking exist bro!
        //constexpr unsigned long long SHIFT_TAB = (unsigned long long)1 << 37;


        //key_Press includes [a-z, A-Z] & [0-9]
        constexpr unsigned long long KEY_PRESS = (unsigned long long)1 << 38;

        // EASY MOUSE API
        constexpr unsigned long long MOUSE_LEFT_CLICKED = (unsigned long long)1 << 39;
        constexpr unsigned long long MOUSE_MIDDLE_CLICKED = (unsigned long long)1 << 40;
        constexpr unsigned long long MOUSE_RIGHT_CLICKED = (unsigned long long)1 << 41;

        // NOTE: These will be spammed until it is not pressed anymore!
        constexpr unsigned long long MOUSE_LEFT_PRESSED = (unsigned long long)1 << 42;
        constexpr unsigned long long MOUSE_MIDDLE_PRESSED = (unsigned long long)1 << 43;
        constexpr unsigned long long MOUSE_RIGHT_PRESSED = (unsigned long long)1 << 44;

        constexpr unsigned long long MOUSE_MIDDLE_SCROLL_UP = (unsigned long long)1 << 45;
        constexpr unsigned long long MOUSE_MIDDLE_SCROLL_DOWN = (unsigned long long)1 << 46;
    
    }

    namespace KEYBOARD_BUTTONS{
        static constexpr std::string_view ESC = "ECS";
        static constexpr std::string_view F1 = "F1";
        static constexpr std::string_view F2 = "F2";
        static constexpr std::string_view F3 = "F3";
        static constexpr std::string_view F4 = "F4";
        static constexpr std::string_view F5 = "F5";
        static constexpr std::string_view F6 = "F6";
        static constexpr std::string_view F7 = "F7";
        static constexpr std::string_view F8 = "F8";
        static constexpr std::string_view F9 = "F9";
        static constexpr std::string_view F10 = "F10";
        static constexpr std::string_view F11 = "F11";
        static constexpr std::string_view F12 = "F12";
        static constexpr std::string_view PRTSC = "PRTSC";
        static constexpr std::string_view SCROLL_LOCK = "SCROLL_LOCK";
        static constexpr std::string_view PAUSE = "PAUSE";
        static constexpr std::string_view SECTION = "SECTION";
        static constexpr std::string_view BACKSPACE = "BACKSPACE";
        static constexpr std::string_view TAB = "TAB";
        static constexpr std::string_view ENTER = "ENTER";
        static constexpr std::string_view CAPS = "CAPS";
        static constexpr std::string_view SHIFT = "SHIFT";
        static constexpr std::string_view CONTROL = "CTRL";
        static constexpr std::string_view SUPER = "SUPER";
        static constexpr std::string_view ALT = "ALT";
        static constexpr std::string_view SPACE = "SPACE";
        static constexpr std::string_view ALTGR = "ALTGR";
        static constexpr std::string_view FN = "FN";
        static constexpr std::string_view INS = "INS";
        static constexpr std::string_view HOME = "HOME";
        static constexpr std::string_view PAGE_UP = "PAGE_UP";
        static constexpr std::string_view DEL = "DELETE";
        static constexpr std::string_view INSERT = "INSERT";
        static constexpr std::string_view END = "END";
        static constexpr std::string_view PAGE_DOWN = "PAGE_DOWN";

        static constexpr std::string_view UP = "UP";
        static constexpr std::string_view DOWN = "DOWN";
        static constexpr std::string_view LEFT = "LEFT";
        static constexpr std::string_view RIGHT = "RIGHT";

        static constexpr std::string_view MOUSE_LEFT = "MOUSE_LEFT";
        static constexpr std::string_view MOUSE_MIDDLE = "MOUSE_MIDDLE";
        static constexpr std::string_view MOUSE_RIGHT = "MOUSE_RIGHT";
        static constexpr std::string_view MOUSE_SCROLL_UP = "MOUSE_SCROLL_UP";
        static constexpr std::string_view MOUSE_SCROLL_DOWN = "MOUSE_SCROLL_DOWN";
    }

    // autoGen: Ignore start
    namespace INTERNAL{
        template <typename Key, typename value, std::size_t N>
        struct arrayMap {
            std::array<std::pair<Key, value>, N> data;

            constexpr value operator[](Key key) const {
                for (auto&& kv : data) {
                    if (kv.first == key) return kv.second;
                }
            #if defined(__cpp_exceptions)
                throw std::out_of_range("key not found in array_map");
            #else
                return value{}; // fallback in no-exception builds
            #endif
            }
        };

        inline constexpr arrayMap<const std::string_view, unsigned long long, 37> BUTTON_STATES_TO_CONSTANTS_BRIDGE = {
            std::make_pair(KEYBOARD_BUTTONS::ESC, constants::ESCAPE),
            {KEYBOARD_BUTTONS::F1, constants::F1},
            {KEYBOARD_BUTTONS::F2, constants::F2},
            {KEYBOARD_BUTTONS::F3, constants::F3},
            {KEYBOARD_BUTTONS::F4, constants::F4},
            {KEYBOARD_BUTTONS::F5, constants::F5},
            {KEYBOARD_BUTTONS::F6, constants::F6},
            {KEYBOARD_BUTTONS::F7, constants::F7},
            {KEYBOARD_BUTTONS::F8, constants::F8},
            {KEYBOARD_BUTTONS::F9, constants::F9},
            {KEYBOARD_BUTTONS::F10, constants::F10},
            {KEYBOARD_BUTTONS::F11, constants::F11},
            {KEYBOARD_BUTTONS::F12, constants::F12},
            //{BUTTON_STATES::PRTSC, Constants::PRINT_SCREEN},
            //{BUTTON_STATES::SCROLL_LOCK, Constants::SCROLL_LOCK},
            //{BUTTON_STATES::PAUSE, Constants::PAUSE},
            //{BUTTON_STATES::SECTION, Constants::SECTION},
            {KEYBOARD_BUTTONS::BACKSPACE, constants::BACKSPACE},
            {KEYBOARD_BUTTONS::TAB, constants::TAB},
            {KEYBOARD_BUTTONS::ENTER, constants::ENTER},
            //{BUTTON_STATES::CAPS, Constants::CAPS},
            {KEYBOARD_BUTTONS::SHIFT, constants::SHIFT},
            {KEYBOARD_BUTTONS::CONTROL, constants::CONTROL},
            {KEYBOARD_BUTTONS::SUPER, constants::SUPER},
            {KEYBOARD_BUTTONS::ALT, constants::ALT},
            {KEYBOARD_BUTTONS::SPACE, constants::SPACE},
            //{BUTTON_STATES::ALTGR, Constants::ALTGR},
            //{BUTTON_STATES::FN, Constants::FN},
            {KEYBOARD_BUTTONS::INS, constants::INSERT},
            {KEYBOARD_BUTTONS::HOME, constants::HOME},
            {KEYBOARD_BUTTONS::PAGE_UP, constants::PAGE_UP},
            {KEYBOARD_BUTTONS::DEL, constants::DEL},
            {KEYBOARD_BUTTONS::INSERT, constants::INSERT},
            {KEYBOARD_BUTTONS::END, constants::END},
            {KEYBOARD_BUTTONS::PAGE_DOWN, constants::PAGE_DOWN},

            {KEYBOARD_BUTTONS::UP, constants::UP},
            {KEYBOARD_BUTTONS::DOWN, constants::DOWN},
            {KEYBOARD_BUTTONS::LEFT, constants::LEFT},
            {KEYBOARD_BUTTONS::RIGHT, constants::RIGHT},

            {KEYBOARD_BUTTONS::MOUSE_LEFT, constants::MOUSE_LEFT_CLICKED},
            {KEYBOARD_BUTTONS::MOUSE_MIDDLE, constants::MOUSE_MIDDLE_CLICKED},
            {KEYBOARD_BUTTONS::MOUSE_RIGHT, constants::MOUSE_RIGHT_CLICKED},
            {KEYBOARD_BUTTONS::MOUSE_SCROLL_UP, constants::MOUSE_MIDDLE_SCROLL_UP},
            {KEYBOARD_BUTTONS::MOUSE_SCROLL_DOWN, constants::MOUSE_MIDDLE_SCROLL_DOWN},
        };
    }
    // autoGen: Ignore end
    
    namespace SYMBOLS{
        constexpr INTERNAL::compactString TOP_LEFT_CORNER = "┌";//"\e(0\x6c\e(B";
        constexpr INTERNAL::compactString BOTTOM_LEFT_CORNER = "└";//"\e(0\x6d\e(B";
        constexpr INTERNAL::compactString TOP_RIGHT_CORNER = "┐";//"\e(0\x6b\e(B";
        constexpr INTERNAL::compactString BOTTOM_RIGHT_CORNER = "┘";//"\e(0\x6a\e(B";
        constexpr INTERNAL::compactString VERTICAL_LINE = "│";//"\e(0\x78\e(B";
        constexpr INTERNAL::compactString HORIZONTAL_LINE = "─";//"\e(0\x71\e(B";
        constexpr INTERNAL::compactString VERTICAL_RIGHT_CONNECTOR = "├";//"\e(0\x74\e(B";
        constexpr INTERNAL::compactString VERTICAL_LEFT_CONNECTOR = "┤";//"\e(0\x75\e(B";
        constexpr INTERNAL::compactString HORIZONTAL_BOTTOM_CONNECTOR = "┬";//"\e(0\x76\e(B";
        constexpr INTERNAL::compactString HORIZONTAL_TOP_CONNECTOR = "┴";//"\e(0\x77\e(B";
        constexpr INTERNAL::compactString CROSS_CONNECTOR = "┼";//"\e(0\x6e\e(B";

        constexpr INTERNAL::compactString CENTERED_HORIZONTAL_LINE = "━";//"\e(0\x2501\e(B";
        constexpr INTERNAL::compactString FULL_BLOCK = "█";//"\e(0\x2588\e(B";

        constexpr INTERNAL::compactString RADIOBUTTON_OFF = "○";
        constexpr INTERNAL::compactString RADIOBUTTON_ON = "◉";

        constexpr INTERNAL::compactString EMPTY_CHECK_BOX = "☐";
        constexpr INTERNAL::compactString CHECKED_CHECK_BOX = "☒";
    }
}

#endif