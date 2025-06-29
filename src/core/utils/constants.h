#ifndef _CONSTANTS_H_
#define _CONSTANTS_H_

#include <string>
#include <unordered_map>
#include <chrono>

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

            // 1 to Red
            // 1 to SEPARATE
            // 1 to Green
            // 1 to SEPARATE
            // 1 to Blue
            constexpr unsigned int maximumNeededPreAllocationForColor = 1 + 1 + 1 + 1 + 1;
            
            // 5 to Text_Overhead
            // 5 to Text_Colour
            // 1 to END_COMMAND
            // 5 to Background_Overhead
            // 5 to Background_Colour
            // 1 to END_COMMAND
            constexpr unsigned int maximumNeededPreAllocationForOverhead = 
                maximumNeededPreAllocationForOverHead + maximumNeededPreAllocationForColor + 1 +
                maximumNeededPreAllocationForOverHead + maximumNeededPreAllocationForColor + 1;

            // 1 to RESET_COLOR
            constexpr unsigned int maximumNeededPreAllocationForReset = 1;

            // 5 to Text_Overhead
            // 5 to Text_Colour
            // 1 to END_COMMAND
            // 5 to Background_Overhead
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

            // CSI (Control Sequence Introducer) sequences.
            constexpr compactString ESC_CODE = "\x1B[";      // Also known as \e[ or \o33
            constexpr compactString SEPARATE = ';';
            constexpr compactString USE_RGB = '2';
            constexpr compactString END_COMMAND = 'm';
            constexpr compactString RESET_COLOR = "\x1B[0m";  // Basically same as RESET_SGR but baked the end command into it for Super_String
            constexpr compactString CLEAR_SCREEN = "\x1B[2J";
            constexpr compactString CLEAR_SCROLLBACK = "\x1B[3J";
            constexpr compactString SET_CURSOR_TO_START = "\x1B[H";
            constexpr compactString RESET_CONSOLE = "\x1B[c";

            /**
             * @brief Enable or disable a private SGR feature
             * 
             * @param command The SGR command to enable or disable
             * @param Enable If true, enable the feature. Otherwise, disable it.
             * @return A Super_String object with the correct escape sequence to enable or disable the feature
             */
            constexpr superString<maximumNeededPreAllocationForEnablingOrDisablingPrivateSGRFeature> Enable_Private_SGR_Feature(const compactString& command, bool Enable = true) { 
                superString<maximumNeededPreAllocationForEnablingOrDisablingPrivateSGRFeature> Result;

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
            constexpr superString<maximumNeededPreAllocationForEnablingOrDisablingSGRFeature> Enable_SGR_Feature(const compactString& command) {
                superString<maximumNeededPreAllocationForEnablingOrDisablingSGRFeature> Result;

                // Add the escape code
                Result.add(ESC_CODE);

                // Add the command to enable
                Result.add(command);

                // Add the end command
                Result.add(END_COMMAND);

                return Result;
            }

            // SGR constants
            constexpr compactString RESET_SGR = "0";                                  // Removes all SGR features. 
            constexpr compactString BOLD = "1";                                       // Not widely supported!
            constexpr compactString FAINT = "2";                                      // Not widely supported!
            constexpr compactString ITALIC = "3";                                     // Not widely supported! (Can also be same as blink)
            constexpr compactString UNDERLINE = "4";              
            constexpr compactString SLOW_BLINK = "5";                                 // ~150 BPM
            constexpr compactString RAPID_BLINK = "6";                                // Not widely supported!
            constexpr compactString INVERT_FOREGROUND_WITH_BACKGROUND = "7";          // Not widely supported!
            constexpr compactString CONCEAL = "8";                                    // Not widely supported!
            constexpr compactString CROSSED_OUT = "9";                                // Not widely supported!
            constexpr compactString PRIMARY_FONT = "10";                              // Sets the default font.
            constexpr compactString ALTERNATIVE_FONT_1 = "11";                        // Custom font slot.
            constexpr compactString ALTERNATIVE_FONT_2 = "12";                        // Custom font slot.
            constexpr compactString ALTERNATIVE_FONT_3 = "13";                        // Custom font slot.
            constexpr compactString ALTERNATIVE_FONT_4 = "14";                        // Custom font slot.
            constexpr compactString ALTERNATIVE_FONT_5 = "15";                        // Custom font slot.
            constexpr compactString ALTERNATIVE_FONT_6 = "16";                        // Custom font slot.
            constexpr compactString ALTERNATIVE_FONT_7 = "17";                        // Custom font slot.
            constexpr compactString ALTERNATIVE_FONT_8 = "18";                        // Custom font slot.
            constexpr compactString ALTERNATIVE_FONT_9 = "19";                        // Custom font slot.
            constexpr compactString FRAKTUR = "20";                                   // Not widely supported! (But cool font)
            constexpr compactString NOT_BOLD = "21";                                  // Removes the BOLD feature
            constexpr compactString NORMAL_INTENSITY = "22";                          // Removes BOLD and ITALIC and other affixes.
            constexpr compactString NOT_UNDERLINE = "23";                             // Removes UNDERLINE.
            constexpr compactString NOT_BLINK = "24";                                 // Removes BLINK.
            constexpr compactString INVERT_INVERT_FOREGROUND_WITH_BACKGROUND = "27";  // Inverts the INVERT_FOREGROUND_WITH_BACKGROUND.
            constexpr compactString TEXT_COLOR = "38";                               // Sets the foreground color.
            constexpr compactString DEFAULT_TEXT_COLOR = "39";                       // Sets the default color.
            constexpr compactString BACKGROUND_COLOR = "48";                         // Sets the background color.
            constexpr compactString DEFAULT_BACKGROUND_COLOR = "49";                 // Sets the default color.

            // Private SGR codes
            constexpr compactString REPORT_MOUSE_HIGHLIGHTS = "1000";
            constexpr compactString REPORT_MOUSE_BUTTON_WHILE_MOVING = "1002";
            constexpr compactString REPORT_MOUSE_ALL_EVENTS = "1003";

            constexpr compactString MOUSE_CURSOR = "25";
            constexpr compactString SCREEN_CAPTURE = "47"; // 47l = restores screen, 47h = saves screen
            constexpr compactString ALTERNATIVE_SCREEN_BUFFER = "1049"; // 1049l = disables alternative buffer, 1049h = enables alternative buffer
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
            constexpr char DELETE = 127;
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
        constexpr unsigned long long DELETE = (unsigned long long)1 << 16;
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
    
        // At compile time generate 0-255 representations as const char* values.
        constexpr const char* toStringTable[256] = {
            "0", "1", "2", "3", "4", "5", "6", "7", "8", "9",
            "10", "11", "12", "13", "14", "15", "16", "17", "18", "19",
            "20", "21", "22", "23", "24", "25", "26", "27", "28", "29",
            "30", "31", "32", "33", "34", "35", "36", "37", "38", "39",
            "40", "41", "42", "43", "44", "45", "46", "47", "48", "49",
            "50", "51", "52", "53", "54", "55", "56", "57", "58", "59",
            "60", "61", "62", "63", "64", "65", "66", "67", "68", "69",
            "70", "71", "72", "73", "74", "75", "76", "77", "78", "79",
            "80", "81", "82", "83", "84", "85", "86", "87", "88", "89",
            "90", "91", "92", "93", "94", "95", "96", "97", "98", "99",
            "100", "101", "102", "103", "104", "105", "106", "107", "108", "109",
            "110", "111", "112", "113", "114", "115", "116", "117", "118", "119",
            "120", "121", "122", "123", "124", "125", "126", "127", "128", "129",
            "130", "131", "132", "133", "134", "135", "136", "137", "138", "139",
            "140", "141", "142", "143", "144", "145", "146", "147", "148", "149",
            "150", "151", "152", "153", "154", "155", "156", "157", "158", "159",
            "160", "161", "162", "163", "164", "165", "166", "167", "168", "169",
            "170", "171", "172", "173", "174", "175", "176", "177", "178", "179",
            "180", "181", "182", "183", "184", "185", "186", "187", "188", "189",
            "190", "191", "192", "193", "194", "195", "196", "197", "198", "199",
            "200", "201", "202", "203", "204", "205", "206", "207", "208", "209",
            "210", "211", "212", "213", "214", "215", "216", "217", "218", "219",
            "220", "221", "222", "223", "224", "225", "226", "227", "228", "229",
            "230", "231", "232", "233", "234", "235", "236", "237", "238", "239",
            "240", "241", "242", "243", "244", "245", "246", "247", "248", "249",
            "250", "251", "252", "253", "254", "255"
        };

        constexpr const compactString toCompactTable[256] = {
            compactString("0", 1), compactString("1", 1), compactString("2", 1), compactString("3", 1), compactString("4", 1), compactString("5", 1), compactString("6", 1), compactString("7", 1), compactString("8", 1), compactString("9", 1),
            compactString("10", 2), compactString("11", 2), compactString("12", 2), compactString("13", 2), compactString("14", 2), compactString("15", 2), compactString("16", 2), compactString("17", 2), compactString("18", 2), compactString("19", 2),
            compactString("20", 2), compactString("21", 2), compactString("22", 2), compactString("23", 2), compactString("24", 2), compactString("25", 2), compactString("26", 2), compactString("27", 2), compactString("28", 2), compactString("29", 2),
            compactString("30", 2), compactString("31", 2), compactString("32", 2), compactString("33", 2), compactString("34", 2), compactString("35", 2), compactString("36", 2), compactString("37", 2), compactString("38", 2), compactString("39", 2),
            compactString("40", 2), compactString("41", 2), compactString("42", 2), compactString("43", 2), compactString("44", 2), compactString("45", 2), compactString("46", 2), compactString("47", 2), compactString("48", 2), compactString("49", 2),
            compactString("50", 2), compactString("51", 2), compactString("52", 2), compactString("53", 2), compactString("54", 2), compactString("55", 2), compactString("56", 2), compactString("57", 2), compactString("58", 2), compactString("59", 2),
            compactString("60", 2), compactString("61", 2), compactString("62", 2), compactString("63", 2), compactString("64", 2), compactString("65", 2), compactString("66", 2), compactString("67", 2), compactString("68", 2), compactString("69", 2),
            compactString("70", 2), compactString("71", 2), compactString("72", 2), compactString("73", 2), compactString("74", 2), compactString("75", 2), compactString("76", 2), compactString("77", 2), compactString("78", 2), compactString("79", 2),
            compactString("80", 2), compactString("81", 2), compactString("82", 2), compactString("83", 2), compactString("84", 2), compactString("85", 2), compactString("86", 2), compactString("87", 2), compactString("88", 2), compactString("89", 2),
            compactString("90", 2), compactString("91", 2), compactString("92", 2), compactString("93", 2), compactString("94", 2), compactString("95", 2), compactString("96", 2), compactString("97", 2), compactString("98", 2), compactString("99", 2),
            compactString("100", 3), compactString("101", 3), compactString("102", 3), compactString("103", 3), compactString("104", 3), compactString("105", 3), compactString("106", 3), compactString("107", 3), compactString("108", 3), compactString("109", 3),
            compactString("110", 3), compactString("111", 3), compactString("112", 3), compactString("113", 3), compactString("114", 3), compactString("115", 3), compactString("116", 3), compactString("117", 3), compactString("118", 3), compactString("119", 3),
            compactString("120", 3), compactString("121", 3), compactString("122", 3), compactString("123", 3), compactString("124", 3), compactString("125", 3), compactString("126", 3), compactString("127", 3), compactString("128", 3), compactString("129", 3),
            compactString("130", 3), compactString("131", 3), compactString("132", 3), compactString("133", 3), compactString("134", 3), compactString("135", 3), compactString("136", 3), compactString("137", 3), compactString("138", 3), compactString("139", 3),
            compactString("140", 3), compactString("141", 3), compactString("142", 3), compactString("143", 3), compactString("144", 3), compactString("145", 3), compactString("146", 3), compactString("147", 3), compactString("148", 3), compactString("149", 3),
            compactString("150", 3), compactString("151", 3), compactString("152", 3), compactString("153", 3), compactString("154", 3), compactString("155", 3), compactString("156", 3), compactString("157", 3), compactString("158", 3), compactString("159", 3),
            compactString("160", 3), compactString("161", 3), compactString("162", 3), compactString("163", 3), compactString("164", 3), compactString("165", 3), compactString("166", 3), compactString("167", 3), compactString("168", 3), compactString("169", 3),
            compactString("170", 3), compactString("171", 3), compactString("172", 3), compactString("173", 3), compactString("174", 3), compactString("175", 3), compactString("176", 3), compactString("177", 3), compactString("178", 3), compactString("179", 3),
            compactString("180", 3), compactString("181", 3), compactString("182", 3), compactString("183", 3), compactString("184", 3), compactString("185", 3), compactString("186", 3), compactString("187", 3), compactString("188", 3), compactString("189", 3),
            compactString("190", 3), compactString("191", 3), compactString("192", 3), compactString("193", 3), compactString("194", 3), compactString("195", 3), compactString("196", 3), compactString("197", 3), compactString("198", 3), compactString("199", 3),
            compactString("200", 3), compactString("201", 3), compactString("202", 3), compactString("203", 3), compactString("204", 3), compactString("205", 3), compactString("206", 3), compactString("207", 3), compactString("208", 3), compactString("209", 3),
            compactString("210", 3), compactString("211", 3), compactString("212", 3), compactString("213", 3), compactString("214", 3), compactString("215", 3), compactString("216", 3), compactString("217", 3), compactString("218", 3), compactString("219", 3),
            compactString("220", 3), compactString("221", 3), compactString("222", 3), compactString("223", 3), compactString("224", 3), compactString("225", 3), compactString("226", 3), compactString("227", 3), compactString("228", 3), compactString("229", 3),
            compactString("230", 3), compactString("231", 3), compactString("232", 3), compactString("233", 3), compactString("234", 3), compactString("235", 3), compactString("236", 3), compactString("237", 3), compactString("238", 3), compactString("239", 3),
            compactString("240", 3), compactString("241", 3), compactString("242", 3), compactString("243", 3), compactString("244", 3), compactString("245", 3), compactString("246", 3), compactString("247", 3), compactString("248", 3), compactString("249", 3),
            compactString("250", 3), compactString("251", 3), compactString("252", 3), compactString("253", 3), compactString("254", 3), compactString("255", 3)
        };
    }

    namespace BUTTON_STATES{
        static const std::string ESC = "ECS";
        static const std::string F1 = "F1";
        static const std::string F2 = "F2";
        static const std::string F3 = "F3";
        static const std::string F4 = "F4";
        static const std::string F5 = "F5";
        static const std::string F6 = "F6";
        static const std::string F7 = "F7";
        static const std::string F8 = "F8";
        static const std::string F9 = "F9";
        static const std::string F10 = "F10";
        static const std::string F11 = "F11";
        static const std::string F12 = "F12";
        static const std::string PRTSC = "PRTSC";
        static const std::string SCROLL_LOCK = "SCROLL_LOCK";
        static const std::string PAUSE = "PAUSE";
        static const std::string SECTION = "SECTION";
        static const std::string BACKSPACE = "BACKSPACE";
        static const std::string TAB = "TAB";
        static const std::string ENTER = "ENTER";
        static const std::string CAPS = "CAPS";
        static const std::string SHIFT = "SHIFT";
        static const std::string CONTROL = "CTRL";
        static const std::string SUPER = "SUPER";
        static const std::string ALT = "ALT";
        static const std::string SPACE = "SPACE";
        static const std::string ALTGR = "ALTGR";
        static const std::string FN = "FN";
        static const std::string INS = "INS";
        static const std::string HOME = "HOME";
        static const std::string PAGE_UP = "PAGE_UP";
        static const std::string DELETE = "DELETE";
        static const std::string INSERT = "INSERT";
        static const std::string END = "END";
        static const std::string PAGE_DOWN = "PAGE_DOWN";

        static const std::string UP = "UP";
        static const std::string DOWN = "DOWN";
        static const std::string LEFT = "LEFT";
        static const std::string RIGHT = "RIGHT";

        static const std::string MOUSE_LEFT = "MOUSE_LEFT";
        static const std::string MOUSE_MIDDLE = "MOUSE_MIDDLE";
        static const std::string MOUSE_RIGHT = "MOUSE_RIGHT";
        static const std::string MOUSE_SCROLL_UP = "MOUSE_SCROLL_UP";
        static const std::string MOUSE_SCROLL_DOWN = "MOUSE_SCROLL_DOWN";
    };

    static const std::unordered_map<std::string, unsigned long long> BUTTON_STATES_TO_CONSTANTS_BRIDGE = {
        {BUTTON_STATES::ESC, constants::ESCAPE},
        {BUTTON_STATES::F1, constants::F1},
        {BUTTON_STATES::F2, constants::F2},
        {BUTTON_STATES::F3, constants::F3},
        {BUTTON_STATES::F4, constants::F4},
        {BUTTON_STATES::F5, constants::F5},
        {BUTTON_STATES::F6, constants::F6},
        {BUTTON_STATES::F7, constants::F7},
        {BUTTON_STATES::F8, constants::F8},
        {BUTTON_STATES::F9, constants::F9},
        {BUTTON_STATES::F10, constants::F10},
        {BUTTON_STATES::F11, constants::F11},
        {BUTTON_STATES::F12, constants::F12},
        //{BUTTON_STATES::PRTSC, Constants::PRINT_SCREEN},
        //{BUTTON_STATES::SCROLL_LOCK, Constants::SCROLL_LOCK},
        //{BUTTON_STATES::PAUSE, Constants::PAUSE},
        //{BUTTON_STATES::SECTION, Constants::SECTION},
        {BUTTON_STATES::BACKSPACE, constants::BACKSPACE},
        {BUTTON_STATES::TAB, constants::TAB},
        {BUTTON_STATES::ENTER, constants::ENTER},
        //{BUTTON_STATES::CAPS, Constants::CAPS},
        {BUTTON_STATES::SHIFT, constants::SHIFT},
        {BUTTON_STATES::CONTROL, constants::CONTROL},
        {BUTTON_STATES::SUPER, constants::SUPER},
        {BUTTON_STATES::ALT, constants::ALT},
        {BUTTON_STATES::SPACE, constants::SPACE},
        //{BUTTON_STATES::ALTGR, Constants::ALTGR},
        //{BUTTON_STATES::FN, Constants::FN},
        {BUTTON_STATES::INS, constants::INSERT},
        {BUTTON_STATES::HOME, constants::HOME},
        {BUTTON_STATES::PAGE_UP, constants::PAGE_UP},
        {BUTTON_STATES::DELETE, constants::DELETE},
        {BUTTON_STATES::INSERT, constants::INSERT},
        {BUTTON_STATES::END, constants::END},
        {BUTTON_STATES::PAGE_DOWN, constants::PAGE_DOWN},

        {BUTTON_STATES::UP, constants::UP},
        {BUTTON_STATES::DOWN, constants::DOWN},
        {BUTTON_STATES::LEFT, constants::LEFT},
        {BUTTON_STATES::RIGHT, constants::RIGHT},

        {BUTTON_STATES::MOUSE_LEFT, constants::MOUSE_LEFT_CLICKED},
        {BUTTON_STATES::MOUSE_MIDDLE, constants::MOUSE_MIDDLE_CLICKED},
        {BUTTON_STATES::MOUSE_RIGHT, constants::MOUSE_RIGHT_CLICKED},
        {BUTTON_STATES::MOUSE_SCROLL_UP, constants::MOUSE_MIDDLE_SCROLL_UP},
        {BUTTON_STATES::MOUSE_SCROLL_DOWN, constants::MOUSE_MIDDLE_SCROLL_DOWN},
    };

    namespace SYMBOLS{
        constexpr compactString TOP_LEFT_CORNER = "┌";//"\e(0\x6c\e(B";
        constexpr compactString BOTTOM_LEFT_CORNER = "└";//"\e(0\x6d\e(B";
        constexpr compactString TOP_RIGHT_CORNER = "┐";//"\e(0\x6b\e(B";
        constexpr compactString BOTTOM_RIGHT_CORNER = "┘";//"\e(0\x6a\e(B";
        constexpr compactString VERTICAL_LINE = "│";//"\e(0\x78\e(B";
        constexpr compactString HORIZONTAL_LINE = "─";//"\e(0\x71\e(B";
        constexpr compactString VERTICAL_RIGHT_CONNECTOR = "├";//"\e(0\x74\e(B";
        constexpr compactString VERTICAL_LEFT_CONNECTOR = "┤";//"\e(0\x75\e(B";
        constexpr compactString HORIZONTAL_BOTTOM_CONNECTOR = "┬";//"\e(0\x76\e(B";
        constexpr compactString HORIZONTAL_TOP_CONNECTOR = "┴";//"\e(0\x77\e(B";
        constexpr compactString CROSS_CONNECTOR = "┼";//"\e(0\x6e\e(B";

        constexpr compactString CENTERED_HORIZONTAL_LINE = "━";//"\e(0\x2501\e(B";
        constexpr compactString FULL_BLOCK = "█";//"\e(0\x2588\e(B";

        constexpr compactString RADIOBUTTON_OFF = "○";
        constexpr compactString RADIOBUTTON_ON = "◉";

        constexpr compactString EMPTY_CHECK_BOX = "☐";
        constexpr compactString CHECKED_CHECK_BOX = "☒";
    }
}

#endif