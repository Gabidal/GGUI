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
            constexpr INTERNAL::compactString ESC_CODE = "\x1B[";      // Also known as \e[ or \o33
            constexpr INTERNAL::compactString SEPARATE = ';';
            constexpr INTERNAL::compactString USE_RGB = '2';
            constexpr INTERNAL::compactString END_COMMAND = 'm';
            constexpr INTERNAL::compactString RESET_COLOR = "\x1B[0m";  // Basically same as RESET_SGR but baked the end command into it for Super_String
            constexpr INTERNAL::compactString CLEAR_SCREEN = "\x1B[2J";
            constexpr INTERNAL::compactString CLEAR_SCROLLBACK = "\x1B[3J";
            constexpr INTERNAL::compactString SET_CURSOR_TO_START = "\x1B[H";
            constexpr INTERNAL::compactString RESET_CONSOLE = "\x1B[c";

            /**
             * @brief Enable or disable a private SGR feature
             * 
             * @param command The SGR command to enable or disable
             * @param Enable If true, enable the feature. Otherwise, disable it.
             * @return A Super_String object with the correct escape sequence to enable or disable the feature
             */
            constexpr INTERNAL::superString<maximumNeededPreAllocationForEnablingOrDisablingPrivateSGRFeature> Enable_Private_SGR_Feature(const INTERNAL::compactString& command, bool Enable = true) { 
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
            constexpr INTERNAL::superString<maximumNeededPreAllocationForEnablingOrDisablingSGRFeature> Enable_SGR_Feature(const INTERNAL::compactString& command) {
                INTERNAL::superString<maximumNeededPreAllocationForEnablingOrDisablingSGRFeature> Result;

                // Add the escape code
                Result.add(ESC_CODE);

                // Add the command to enable
                Result.add(command);

                // Add the end command
                Result.add(END_COMMAND);

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
            constexpr INTERNAL::compactString REPORT_MOUSE_HIGHLIGHTS = "1000";
            constexpr INTERNAL::compactString REPORT_MOUSE_BUTTON_WHILE_MOVING = "1002";
            constexpr INTERNAL::compactString REPORT_MOUSE_ALL_EVENTS = "1003";

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

            constexpr const INTERNAL::compactString toCompactTable[256] = {
                INTERNAL::compactString("0", 1), INTERNAL::compactString("1", 1), INTERNAL::compactString("2", 1), INTERNAL::compactString("3", 1), INTERNAL::compactString("4", 1), INTERNAL::compactString("5", 1), INTERNAL::compactString("6", 1), INTERNAL::compactString("7", 1), INTERNAL::compactString("8", 1), INTERNAL::compactString("9", 1),
                INTERNAL::compactString("10", 2), INTERNAL::compactString("11", 2), INTERNAL::compactString("12", 2), INTERNAL::compactString("13", 2), INTERNAL::compactString("14", 2), INTERNAL::compactString("15", 2), INTERNAL::compactString("16", 2), INTERNAL::compactString("17", 2), INTERNAL::compactString("18", 2), INTERNAL::compactString("19", 2),
                INTERNAL::compactString("20", 2), INTERNAL::compactString("21", 2), INTERNAL::compactString("22", 2), INTERNAL::compactString("23", 2), INTERNAL::compactString("24", 2), INTERNAL::compactString("25", 2), INTERNAL::compactString("26", 2), INTERNAL::compactString("27", 2), INTERNAL::compactString("28", 2), INTERNAL::compactString("29", 2),
                INTERNAL::compactString("30", 2), INTERNAL::compactString("31", 2), INTERNAL::compactString("32", 2), INTERNAL::compactString("33", 2), INTERNAL::compactString("34", 2), INTERNAL::compactString("35", 2), INTERNAL::compactString("36", 2), INTERNAL::compactString("37", 2), INTERNAL::compactString("38", 2), INTERNAL::compactString("39", 2),
                INTERNAL::compactString("40", 2), INTERNAL::compactString("41", 2), INTERNAL::compactString("42", 2), INTERNAL::compactString("43", 2), INTERNAL::compactString("44", 2), INTERNAL::compactString("45", 2), INTERNAL::compactString("46", 2), INTERNAL::compactString("47", 2), INTERNAL::compactString("48", 2), INTERNAL::compactString("49", 2),
                INTERNAL::compactString("50", 2), INTERNAL::compactString("51", 2), INTERNAL::compactString("52", 2), INTERNAL::compactString("53", 2), INTERNAL::compactString("54", 2), INTERNAL::compactString("55", 2), INTERNAL::compactString("56", 2), INTERNAL::compactString("57", 2), INTERNAL::compactString("58", 2), INTERNAL::compactString("59", 2),
                INTERNAL::compactString("60", 2), INTERNAL::compactString("61", 2), INTERNAL::compactString("62", 2), INTERNAL::compactString("63", 2), INTERNAL::compactString("64", 2), INTERNAL::compactString("65", 2), INTERNAL::compactString("66", 2), INTERNAL::compactString("67", 2), INTERNAL::compactString("68", 2), INTERNAL::compactString("69", 2),
                INTERNAL::compactString("70", 2), INTERNAL::compactString("71", 2), INTERNAL::compactString("72", 2), INTERNAL::compactString("73", 2), INTERNAL::compactString("74", 2), INTERNAL::compactString("75", 2), INTERNAL::compactString("76", 2), INTERNAL::compactString("77", 2), INTERNAL::compactString("78", 2), INTERNAL::compactString("79", 2),
                INTERNAL::compactString("80", 2), INTERNAL::compactString("81", 2), INTERNAL::compactString("82", 2), INTERNAL::compactString("83", 2), INTERNAL::compactString("84", 2), INTERNAL::compactString("85", 2), INTERNAL::compactString("86", 2), INTERNAL::compactString("87", 2), INTERNAL::compactString("88", 2), INTERNAL::compactString("89", 2),
                INTERNAL::compactString("90", 2), INTERNAL::compactString("91", 2), INTERNAL::compactString("92", 2), INTERNAL::compactString("93", 2), INTERNAL::compactString("94", 2), INTERNAL::compactString("95", 2), INTERNAL::compactString("96", 2), INTERNAL::compactString("97", 2), INTERNAL::compactString("98", 2), INTERNAL::compactString("99", 2),
                INTERNAL::compactString("100", 3), INTERNAL::compactString("101", 3), INTERNAL::compactString("102", 3), INTERNAL::compactString("103", 3), INTERNAL::compactString("104", 3), INTERNAL::compactString("105", 3), INTERNAL::compactString("106", 3), INTERNAL::compactString("107", 3), INTERNAL::compactString("108", 3), INTERNAL::compactString("109", 3),
                INTERNAL::compactString("110", 3), INTERNAL::compactString("111", 3), INTERNAL::compactString("112", 3), INTERNAL::compactString("113", 3), INTERNAL::compactString("114", 3), INTERNAL::compactString("115", 3), INTERNAL::compactString("116", 3), INTERNAL::compactString("117", 3), INTERNAL::compactString("118", 3), INTERNAL::compactString("119", 3),
                INTERNAL::compactString("120", 3), INTERNAL::compactString("121", 3), INTERNAL::compactString("122", 3), INTERNAL::compactString("123", 3), INTERNAL::compactString("124", 3), INTERNAL::compactString("125", 3), INTERNAL::compactString("126", 3), INTERNAL::compactString("127", 3), INTERNAL::compactString("128", 3), INTERNAL::compactString("129", 3),
                INTERNAL::compactString("130", 3), INTERNAL::compactString("131", 3), INTERNAL::compactString("132", 3), INTERNAL::compactString("133", 3), INTERNAL::compactString("134", 3), INTERNAL::compactString("135", 3), INTERNAL::compactString("136", 3), INTERNAL::compactString("137", 3), INTERNAL::compactString("138", 3), INTERNAL::compactString("139", 3),
                INTERNAL::compactString("140", 3), INTERNAL::compactString("141", 3), INTERNAL::compactString("142", 3), INTERNAL::compactString("143", 3), INTERNAL::compactString("144", 3), INTERNAL::compactString("145", 3), INTERNAL::compactString("146", 3), INTERNAL::compactString("147", 3), INTERNAL::compactString("148", 3), INTERNAL::compactString("149", 3),
                INTERNAL::compactString("150", 3), INTERNAL::compactString("151", 3), INTERNAL::compactString("152", 3), INTERNAL::compactString("153", 3), INTERNAL::compactString("154", 3), INTERNAL::compactString("155", 3), INTERNAL::compactString("156", 3), INTERNAL::compactString("157", 3), INTERNAL::compactString("158", 3), INTERNAL::compactString("159", 3),
                INTERNAL::compactString("160", 3), INTERNAL::compactString("161", 3), INTERNAL::compactString("162", 3), INTERNAL::compactString("163", 3), INTERNAL::compactString("164", 3), INTERNAL::compactString("165", 3), INTERNAL::compactString("166", 3), INTERNAL::compactString("167", 3), INTERNAL::compactString("168", 3), INTERNAL::compactString("169", 3),
                INTERNAL::compactString("170", 3), INTERNAL::compactString("171", 3), INTERNAL::compactString("172", 3), INTERNAL::compactString("173", 3), INTERNAL::compactString("174", 3), INTERNAL::compactString("175", 3), INTERNAL::compactString("176", 3), INTERNAL::compactString("177", 3), INTERNAL::compactString("178", 3), INTERNAL::compactString("179", 3),
                INTERNAL::compactString("180", 3), INTERNAL::compactString("181", 3), INTERNAL::compactString("182", 3), INTERNAL::compactString("183", 3), INTERNAL::compactString("184", 3), INTERNAL::compactString("185", 3), INTERNAL::compactString("186", 3), INTERNAL::compactString("187", 3), INTERNAL::compactString("188", 3), INTERNAL::compactString("189", 3),
                INTERNAL::compactString("190", 3), INTERNAL::compactString("191", 3), INTERNAL::compactString("192", 3), INTERNAL::compactString("193", 3), INTERNAL::compactString("194", 3), INTERNAL::compactString("195", 3), INTERNAL::compactString("196", 3), INTERNAL::compactString("197", 3), INTERNAL::compactString("198", 3), INTERNAL::compactString("199", 3),
                INTERNAL::compactString("200", 3), INTERNAL::compactString("201", 3), INTERNAL::compactString("202", 3), INTERNAL::compactString("203", 3), INTERNAL::compactString("204", 3), INTERNAL::compactString("205", 3), INTERNAL::compactString("206", 3), INTERNAL::compactString("207", 3), INTERNAL::compactString("208", 3), INTERNAL::compactString("209", 3),
                INTERNAL::compactString("210", 3), INTERNAL::compactString("211", 3), INTERNAL::compactString("212", 3), INTERNAL::compactString("213", 3), INTERNAL::compactString("214", 3), INTERNAL::compactString("215", 3), INTERNAL::compactString("216", 3), INTERNAL::compactString("217", 3), INTERNAL::compactString("218", 3), INTERNAL::compactString("219", 3),
                INTERNAL::compactString("220", 3), INTERNAL::compactString("221", 3), INTERNAL::compactString("222", 3), INTERNAL::compactString("223", 3), INTERNAL::compactString("224", 3), INTERNAL::compactString("225", 3), INTERNAL::compactString("226", 3), INTERNAL::compactString("227", 3), INTERNAL::compactString("228", 3), INTERNAL::compactString("229", 3),
                INTERNAL::compactString("230", 3), INTERNAL::compactString("231", 3), INTERNAL::compactString("232", 3), INTERNAL::compactString("233", 3), INTERNAL::compactString("234", 3), INTERNAL::compactString("235", 3), INTERNAL::compactString("236", 3), INTERNAL::compactString("237", 3), INTERNAL::compactString("238", 3), INTERNAL::compactString("239", 3),
                INTERNAL::compactString("240", 3), INTERNAL::compactString("241", 3), INTERNAL::compactString("242", 3), INTERNAL::compactString("243", 3), INTERNAL::compactString("244", 3), INTERNAL::compactString("245", 3), INTERNAL::compactString("246", 3), INTERNAL::compactString("247", 3), INTERNAL::compactString("248", 3), INTERNAL::compactString("249", 3),
                INTERNAL::compactString("250", 3), INTERNAL::compactString("251", 3), INTERNAL::compactString("252", 3), INTERNAL::compactString("253", 3), INTERNAL::compactString("254", 3), INTERNAL::compactString("255", 3)
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
        static const std::string DEL = "DELETE";
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

    namespace INTERNAL{
        template <typename Key, typename Value, std::size_t N>
        struct arrayMap {
            std::array<std::pair<Key, Value>, N> data;

            constexpr Value operator[](Key key) const {
                for (auto&& [k, v] : data) {
                    if (k == key) return v;
                }
                throw std::out_of_range("key not found in array_map");
            }
        };

        static const arrayMap<const std::string, unsigned long long, 37> BUTTON_STATES_TO_CONSTANTS_BRIDGE = {
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