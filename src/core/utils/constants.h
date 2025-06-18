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

    namespace Constants{
        namespace ANSI{
            // 1 to ESC_CODE
            // 1 to Text_Color | Background_Color
            // 1 to SEPARATE
            // 1 to USE_RGB
            // 1 to SEPARATE
            constexpr unsigned int Maximum_Needed_Pre_Allocation_For_Over_Head = 1 + 1 + 1 + 1 + 1;

            // 1 to Red
            // 1 to SEPARATE
            // 1 to Green
            // 1 to SEPARATE
            // 1 to Blue
            constexpr unsigned int Maximum_Needed_Pre_Allocation_For_Color = 1 + 1 + 1 + 1 + 1;
            
            // 5 to Text_Overhead
            // 5 to Text_Colour
            // 1 to END_COMMAND
            // 5 to Background_Overhead
            // 5 to Background_Colour
            // 1 to END_COMMAND
            constexpr unsigned int Maximum_Needed_Pre_Allocation_For_Overhead = 
                Maximum_Needed_Pre_Allocation_For_Over_Head + Maximum_Needed_Pre_Allocation_For_Color + 1 +
                Maximum_Needed_Pre_Allocation_For_Over_Head + Maximum_Needed_Pre_Allocation_For_Color + 1;

            // 1 to RESET_COLOR
            constexpr unsigned int Maximum_Needed_Pre_Allocation_For_Reset = 1;

            // 5 to Text_Overhead
            // 5 to Text_Colour
            // 1 to END_COMMAND
            // 5 to Background_Overhead
            // 5 to Background_Colour
            // 1 to END_COMMAND
            // 1 to Data
            // 1 to RESET_COLOR
            constexpr unsigned int Maximum_Needed_Pre_Allocation_For_Encoded_Super_String = Maximum_Needed_Pre_Allocation_For_Overhead + 1 + Maximum_Needed_Pre_Allocation_For_Reset;

            // 1 to Escape code
            // 1 to private SGR telltale '?'
            // 1 to Feature to be disabled or enabled
            // 1 to Enable/Disable feature told above.
            constexpr unsigned int Maximum_Needed_Pre_Allocation_For_Enabling_Or_Disabling_Private_SGR_Feature = 1 + 1 + 1 + 1;
        
            // 1 to Escape code
            // 1 to feature to be enabled
            // 1 to END_COMMAND
            constexpr unsigned int Maximum_Needed_Pre_Allocation_For_Enabling_Or_Disabling_SGR_Feature = 1 + 1 + 1;

            // CSI (Control Sequence Introducer) sequences.
            constexpr Compact_String ESC_CODE = "\x1B[";      // Also known as \e[ or \o33
            constexpr Compact_String SEPARATE = ';';
            constexpr Compact_String USE_RGB = '2';
            constexpr Compact_String END_COMMAND = 'm';
            constexpr Compact_String RESET_COLOR = "\x1B[0m";  // Basically same as RESET_SGR but baked the end command into it for Super_String
            constexpr Compact_String CLEAR_SCREEN = "\x1B[2J";
            constexpr Compact_String CLEAR_SCROLLBACK = "\x1B[3J";
            constexpr Compact_String SET_CURSOR_TO_START = "\x1B[H";
            constexpr Compact_String RESET_CONSOLE = "\x1B[c";

            /**
             * @brief Enable or disable a private SGR feature
             * 
             * @param command The SGR command to enable or disable
             * @param Enable If true, enable the feature. Otherwise, disable it.
             * @return A Super_String object with the correct escape sequence to enable or disable the feature
             */
            constexpr Super_String<Maximum_Needed_Pre_Allocation_For_Enabling_Or_Disabling_Private_SGR_Feature> Enable_Private_SGR_Feature(const Compact_String& command, bool Enable = true) { 
                Super_String<Maximum_Needed_Pre_Allocation_For_Enabling_Or_Disabling_Private_SGR_Feature> Result;

                // Add the escape code
                Result.Add(ESC_CODE);

                // Add the private SGR telltale '?'
                Result.Add('?');

                // Add the command to enable or disable
                Result.Add(command);

                // Finally, add the enable or disable code
                if (Enable)
                    Result.Add('h');
                else
                    Result.Add('l');

                return Result;
            }


            // SGR (Select Graphic Rendition)
            
            /**
             * @brief Enable a SGR feature
             * 
             * @param command The command to enable
             * @return A Super_String object with the correct escape sequence to enable the feature
             */
            constexpr Super_String<Maximum_Needed_Pre_Allocation_For_Enabling_Or_Disabling_SGR_Feature> Enable_SGR_Feature(const Compact_String& command) {
                Super_String<Maximum_Needed_Pre_Allocation_For_Enabling_Or_Disabling_SGR_Feature> Result;

                // Add the escape code
                Result.Add(ESC_CODE);

                // Add the command to enable
                Result.Add(command);

                // Add the end command
                Result.Add(END_COMMAND);

                return Result;
            }

            // SGR constants
            constexpr Compact_String RESET_SGR = "0";                                  // Removes all SGR features. 
            constexpr Compact_String BOLD = "1";                                       // Not widely supported!
            constexpr Compact_String FAINT = "2";                                      // Not widely supported!
            constexpr Compact_String ITALIC = "3";                                     // Not widely supported! (Can also be same as blink)
            constexpr Compact_String UNDERLINE = "4";              
            constexpr Compact_String SLOW_BLINK = "5";                                 // ~150 BPM
            constexpr Compact_String RAPID_BLINK = "6";                                // Not widely supported!
            constexpr Compact_String INVERT_FOREGROUND_WITH_BACKGROUND = "7";          // Not widely supported!
            constexpr Compact_String CONCEAL = "8";                                    // Not widely supported!
            constexpr Compact_String CROSSED_OUT = "9";                                // Not widely supported!
            constexpr Compact_String PRIMARY_FONT = "10";                              // Sets the default font.
            constexpr Compact_String ALTERNATIVE_FONT_1 = "11";                        // Custom font slot.
            constexpr Compact_String ALTERNATIVE_FONT_2 = "12";                        // Custom font slot.
            constexpr Compact_String ALTERNATIVE_FONT_3 = "13";                        // Custom font slot.
            constexpr Compact_String ALTERNATIVE_FONT_4 = "14";                        // Custom font slot.
            constexpr Compact_String ALTERNATIVE_FONT_5 = "15";                        // Custom font slot.
            constexpr Compact_String ALTERNATIVE_FONT_6 = "16";                        // Custom font slot.
            constexpr Compact_String ALTERNATIVE_FONT_7 = "17";                        // Custom font slot.
            constexpr Compact_String ALTERNATIVE_FONT_8 = "18";                        // Custom font slot.
            constexpr Compact_String ALTERNATIVE_FONT_9 = "19";                        // Custom font slot.
            constexpr Compact_String FRAKTUR = "20";                                   // Not widely supported! (But cool font)
            constexpr Compact_String NOT_BOLD = "21";                                  // Removes the BOLD feature
            constexpr Compact_String NORMAL_INTENSITY = "22";                          // Removes BOLD and ITALIC and other affixes.
            constexpr Compact_String NOT_UNDERLINE = "23";                             // Removes UNDERLINE.
            constexpr Compact_String NOT_BLINK = "24";                                 // Removes BLINK.
            constexpr Compact_String INVERT_INVERT_FOREGROUND_WITH_BACKGROUND = "27";  // Inverts the INVERT_FOREGROUND_WITH_BACKGROUND.
            constexpr Compact_String TEXT_COLOR = "38";                               // Sets the foreground color.
            constexpr Compact_String DEFAULT_TEXT_COLOR = "39";                       // Sets the default color.
            constexpr Compact_String BACKGROUND_COLOR = "48";                         // Sets the background color.
            constexpr Compact_String DEFAULT_BACKGROUND_COLOR = "49";                 // Sets the default color.

            // Private SGR codes
            constexpr Compact_String REPORT_MOUSE_HIGHLIGHTS = "1000";
            constexpr Compact_String REPORT_MOUSE_BUTTON_WHILE_MOVING = "1002";
            constexpr Compact_String REPORT_MOUSE_ALL_EVENTS = "1003";

            constexpr Compact_String MOUSE_CURSOR = "25";
            constexpr Compact_String SCREEN_CAPTURE = "47"; // 47l = restores screen, 47h = saves screen
            constexpr Compact_String ALTERNATIVE_SCREEN_BUFFER = "1049"; // 1049l = disables alternative buffer, 1049h = enables alternative buffer
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
        constexpr const char* To_String[256] = {
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

        constexpr Compact_String To_Compact[256] = {
            Compact_String("0", 1), Compact_String("1", 1), Compact_String("2", 1), Compact_String("3", 1), Compact_String("4", 1), Compact_String("5", 1), Compact_String("6", 1), Compact_String("7", 1), Compact_String("8", 1), Compact_String("9", 1),
            Compact_String("10", 2), Compact_String("11", 2), Compact_String("12", 2), Compact_String("13", 2), Compact_String("14", 2), Compact_String("15", 2), Compact_String("16", 2), Compact_String("17", 2), Compact_String("18", 2), Compact_String("19", 2),
            Compact_String("20", 2), Compact_String("21", 2), Compact_String("22", 2), Compact_String("23", 2), Compact_String("24", 2), Compact_String("25", 2), Compact_String("26", 2), Compact_String("27", 2), Compact_String("28", 2), Compact_String("29", 2),
            Compact_String("30", 2), Compact_String("31", 2), Compact_String("32", 2), Compact_String("33", 2), Compact_String("34", 2), Compact_String("35", 2), Compact_String("36", 2), Compact_String("37", 2), Compact_String("38", 2), Compact_String("39", 2),
            Compact_String("40", 2), Compact_String("41", 2), Compact_String("42", 2), Compact_String("43", 2), Compact_String("44", 2), Compact_String("45", 2), Compact_String("46", 2), Compact_String("47", 2), Compact_String("48", 2), Compact_String("49", 2),
            Compact_String("50", 2), Compact_String("51", 2), Compact_String("52", 2), Compact_String("53", 2), Compact_String("54", 2), Compact_String("55", 2), Compact_String("56", 2), Compact_String("57", 2), Compact_String("58", 2), Compact_String("59", 2),
            Compact_String("60", 2), Compact_String("61", 2), Compact_String("62", 2), Compact_String("63", 2), Compact_String("64", 2), Compact_String("65", 2), Compact_String("66", 2), Compact_String("67", 2), Compact_String("68", 2), Compact_String("69", 2),
            Compact_String("70", 2), Compact_String("71", 2), Compact_String("72", 2), Compact_String("73", 2), Compact_String("74", 2), Compact_String("75", 2), Compact_String("76", 2), Compact_String("77", 2), Compact_String("78", 2), Compact_String("79", 2),
            Compact_String("80", 2), Compact_String("81", 2), Compact_String("82", 2), Compact_String("83", 2), Compact_String("84", 2), Compact_String("85", 2), Compact_String("86", 2), Compact_String("87", 2), Compact_String("88", 2), Compact_String("89", 2),
            Compact_String("90", 2), Compact_String("91", 2), Compact_String("92", 2), Compact_String("93", 2), Compact_String("94", 2), Compact_String("95", 2), Compact_String("96", 2), Compact_String("97", 2), Compact_String("98", 2), Compact_String("99", 2),
            Compact_String("100", 3), Compact_String("101", 3), Compact_String("102", 3), Compact_String("103", 3), Compact_String("104", 3), Compact_String("105", 3), Compact_String("106", 3), Compact_String("107", 3), Compact_String("108", 3), Compact_String("109", 3),
            Compact_String("110", 3), Compact_String("111", 3), Compact_String("112", 3), Compact_String("113", 3), Compact_String("114", 3), Compact_String("115", 3), Compact_String("116", 3), Compact_String("117", 3), Compact_String("118", 3), Compact_String("119", 3),
            Compact_String("120", 3), Compact_String("121", 3), Compact_String("122", 3), Compact_String("123", 3), Compact_String("124", 3), Compact_String("125", 3), Compact_String("126", 3), Compact_String("127", 3), Compact_String("128", 3), Compact_String("129", 3),
            Compact_String("130", 3), Compact_String("131", 3), Compact_String("132", 3), Compact_String("133", 3), Compact_String("134", 3), Compact_String("135", 3), Compact_String("136", 3), Compact_String("137", 3), Compact_String("138", 3), Compact_String("139", 3),
            Compact_String("140", 3), Compact_String("141", 3), Compact_String("142", 3), Compact_String("143", 3), Compact_String("144", 3), Compact_String("145", 3), Compact_String("146", 3), Compact_String("147", 3), Compact_String("148", 3), Compact_String("149", 3),
            Compact_String("150", 3), Compact_String("151", 3), Compact_String("152", 3), Compact_String("153", 3), Compact_String("154", 3), Compact_String("155", 3), Compact_String("156", 3), Compact_String("157", 3), Compact_String("158", 3), Compact_String("159", 3),
            Compact_String("160", 3), Compact_String("161", 3), Compact_String("162", 3), Compact_String("163", 3), Compact_String("164", 3), Compact_String("165", 3), Compact_String("166", 3), Compact_String("167", 3), Compact_String("168", 3), Compact_String("169", 3),
            Compact_String("170", 3), Compact_String("171", 3), Compact_String("172", 3), Compact_String("173", 3), Compact_String("174", 3), Compact_String("175", 3), Compact_String("176", 3), Compact_String("177", 3), Compact_String("178", 3), Compact_String("179", 3),
            Compact_String("180", 3), Compact_String("181", 3), Compact_String("182", 3), Compact_String("183", 3), Compact_String("184", 3), Compact_String("185", 3), Compact_String("186", 3), Compact_String("187", 3), Compact_String("188", 3), Compact_String("189", 3),
            Compact_String("190", 3), Compact_String("191", 3), Compact_String("192", 3), Compact_String("193", 3), Compact_String("194", 3), Compact_String("195", 3), Compact_String("196", 3), Compact_String("197", 3), Compact_String("198", 3), Compact_String("199", 3),
            Compact_String("200", 3), Compact_String("201", 3), Compact_String("202", 3), Compact_String("203", 3), Compact_String("204", 3), Compact_String("205", 3), Compact_String("206", 3), Compact_String("207", 3), Compact_String("208", 3), Compact_String("209", 3),
            Compact_String("210", 3), Compact_String("211", 3), Compact_String("212", 3), Compact_String("213", 3), Compact_String("214", 3), Compact_String("215", 3), Compact_String("216", 3), Compact_String("217", 3), Compact_String("218", 3), Compact_String("219", 3),
            Compact_String("220", 3), Compact_String("221", 3), Compact_String("222", 3), Compact_String("223", 3), Compact_String("224", 3), Compact_String("225", 3), Compact_String("226", 3), Compact_String("227", 3), Compact_String("228", 3), Compact_String("229", 3),
            Compact_String("230", 3), Compact_String("231", 3), Compact_String("232", 3), Compact_String("233", 3), Compact_String("234", 3), Compact_String("235", 3), Compact_String("236", 3), Compact_String("237", 3), Compact_String("238", 3), Compact_String("239", 3),
            Compact_String("240", 3), Compact_String("241", 3), Compact_String("242", 3), Compact_String("243", 3), Compact_String("244", 3), Compact_String("245", 3), Compact_String("246", 3), Compact_String("247", 3), Compact_String("248", 3), Compact_String("249", 3),
            Compact_String("250", 3), Compact_String("251", 3), Compact_String("252", 3), Compact_String("253", 3), Compact_String("254", 3), Compact_String("255", 3)
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
        {BUTTON_STATES::ESC, Constants::ESCAPE},
        {BUTTON_STATES::F1, Constants::F1},
        {BUTTON_STATES::F2, Constants::F2},
        {BUTTON_STATES::F3, Constants::F3},
        {BUTTON_STATES::F4, Constants::F4},
        {BUTTON_STATES::F5, Constants::F5},
        {BUTTON_STATES::F6, Constants::F6},
        {BUTTON_STATES::F7, Constants::F7},
        {BUTTON_STATES::F8, Constants::F8},
        {BUTTON_STATES::F9, Constants::F9},
        {BUTTON_STATES::F10, Constants::F10},
        {BUTTON_STATES::F11, Constants::F11},
        {BUTTON_STATES::F12, Constants::F12},
        //{BUTTON_STATES::PRTSC, Constants::PRINT_SCREEN},
        //{BUTTON_STATES::SCROLL_LOCK, Constants::SCROLL_LOCK},
        //{BUTTON_STATES::PAUSE, Constants::PAUSE},
        //{BUTTON_STATES::SECTION, Constants::SECTION},
        {BUTTON_STATES::BACKSPACE, Constants::BACKSPACE},
        {BUTTON_STATES::TAB, Constants::TAB},
        {BUTTON_STATES::ENTER, Constants::ENTER},
        //{BUTTON_STATES::CAPS, Constants::CAPS},
        {BUTTON_STATES::SHIFT, Constants::SHIFT},
        {BUTTON_STATES::CONTROL, Constants::CONTROL},
        {BUTTON_STATES::SUPER, Constants::SUPER},
        {BUTTON_STATES::ALT, Constants::ALT},
        {BUTTON_STATES::SPACE, Constants::SPACE},
        //{BUTTON_STATES::ALTGR, Constants::ALTGR},
        //{BUTTON_STATES::FN, Constants::FN},
        {BUTTON_STATES::INS, Constants::INSERT},
        {BUTTON_STATES::HOME, Constants::HOME},
        {BUTTON_STATES::PAGE_UP, Constants::PAGE_UP},
        {BUTTON_STATES::DELETE, Constants::DELETE},
        {BUTTON_STATES::INSERT, Constants::INSERT},
        {BUTTON_STATES::END, Constants::END},
        {BUTTON_STATES::PAGE_DOWN, Constants::PAGE_DOWN},

        {BUTTON_STATES::UP, Constants::UP},
        {BUTTON_STATES::DOWN, Constants::DOWN},
        {BUTTON_STATES::LEFT, Constants::LEFT},
        {BUTTON_STATES::RIGHT, Constants::RIGHT},

        {BUTTON_STATES::MOUSE_LEFT, Constants::MOUSE_LEFT_CLICKED},
        {BUTTON_STATES::MOUSE_MIDDLE, Constants::MOUSE_MIDDLE_CLICKED},
        {BUTTON_STATES::MOUSE_RIGHT, Constants::MOUSE_RIGHT_CLICKED},
        {BUTTON_STATES::MOUSE_SCROLL_UP, Constants::MOUSE_MIDDLE_SCROLL_UP},
        {BUTTON_STATES::MOUSE_SCROLL_DOWN, Constants::MOUSE_MIDDLE_SCROLL_DOWN},
    };

    namespace SYMBOLS{
        constexpr Compact_String TOP_LEFT_CORNER = "┌";//"\e(0\x6c\e(B";
        constexpr Compact_String BOTTOM_LEFT_CORNER = "└";//"\e(0\x6d\e(B";
        constexpr Compact_String TOP_RIGHT_CORNER = "┐";//"\e(0\x6b\e(B";
        constexpr Compact_String BOTTOM_RIGHT_CORNER = "┘";//"\e(0\x6a\e(B";
        constexpr Compact_String VERTICAL_LINE = "│";//"\e(0\x78\e(B";
        constexpr Compact_String HORIZONTAL_LINE = "─";//"\e(0\x71\e(B";
        constexpr Compact_String VERTICAL_RIGHT_CONNECTOR = "├";//"\e(0\x74\e(B";
        constexpr Compact_String VERTICAL_LEFT_CONNECTOR = "┤";//"\e(0\x75\e(B";
        constexpr Compact_String HORIZONTAL_BOTTOM_CONNECTOR = "┬";//"\e(0\x76\e(B";
        constexpr Compact_String HORIZONTAL_TOP_CONNECTOR = "┴";//"\e(0\x77\e(B";
        constexpr Compact_String CROSS_CONNECTOR = "┼";//"\e(0\x6e\e(B";

        constexpr Compact_String CENTERED_HORIZONTAL_LINE = "━";//"\e(0\x2501\e(B";
        constexpr Compact_String FULL_BLOCK = "█";//"\e(0\x2588\e(B";

        constexpr Compact_String RADIOBUTTON_OFF = "○";
        constexpr Compact_String RADIOBUTTON_ON = "◉";

        constexpr Compact_String EMPTY_CHECK_BOX = "☐";
        constexpr Compact_String CHECKED_CHECK_BOX = "☒";
    }
}

#endif