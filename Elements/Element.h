#ifndef _ELEMENT_H_
#define _ELEMENT_H_

#include <string>
#include <sstream>
#include <cstring>
#include <vector>
#include <unordered_map>
#include <functional>
#include <chrono>
#include <atomic>
#include <limits>

#include <iostream>

namespace GGUI{
    //GGUI uses the ANSI escape code
    //https://en.wikipedia.org/wiki/ANSI_escape_code

    class UTF;

    namespace UTF_FLAG{
        constexpr inline unsigned char IS_ASCII          = 1 << 0;
        constexpr inline unsigned char IS_UNICODE        = 1 << 1;
        constexpr inline unsigned char ENCODE_START      = 1 << 2;
        constexpr inline unsigned char ENCODE_END        = 1 << 3;
    };

    // And lighter-weight version of the UTF class. [Probably after making the RGBA use unsigned char instead of float, and thus making the overall size into 32 bits, replace this class with UTF.]
    class Compact_String{
    public:
        union{
            const char* Unicode_Data;
            char Ascii_Data;
        } Data = { nullptr };

        unsigned int Size = 0;

        // Only for resize!!!
        Compact_String() = default;

        Compact_String(const char* data){
            Size = std::strlen(data); 

            if (Size > 1)
                Data.Unicode_Data = data;
            else
                Data.Ascii_Data = data[0];
        }

        Compact_String(char data){
            Data.Ascii_Data = data;
            Size = 1;
        }

        Compact_String(const char* data, unsigned int size, bool Force_Unicode = false){
            Size = size;

            if (Size > 1 || Force_Unicode)
                Data.Unicode_Data = data;
            else
                Data.Ascii_Data = data[0];
        }

        char operator[](unsigned int index){
            if (Size > 1)
                return Data.Unicode_Data[index];
            else
                return Data.Ascii_Data;
        }
    };

    // Instead of reconstructing new strings every time, this class stores the components, and then only one time constructs the final string representation.
    class Super_String{
    public:
        std::vector<Compact_String> Data;
        unsigned int Current_Index = 0;

        Super_String(unsigned int Final_Size = 1){
            Data.resize(Final_Size);
            Current_Index = 0;
        }

        void Clear(){
            Current_Index = 0;
        }

        void Add(const char* data, int size){
            Data[Current_Index++] = Compact_String(data, size);
        }

        void Add(char data){
            Data[Current_Index++] = Compact_String(data);
        }

        void Add(const std::string& data){
            Data[Current_Index++] = Compact_String(data.data(), data.size());
        }

        void Add(Super_String* other, bool Expected = false){
            // enlarge the reservation
            if (!Expected)
                Data.resize(Current_Index + other->Current_Index);

            for (unsigned int i = 0; i < other->Current_Index; i++){

                Data[Current_Index++] = other->Data[i];
            }
        }
        
        void Add(Super_String& other, bool Expected = false){
            // enlarge the reservation
            if (!Expected)
                Data.resize(Current_Index + other.Current_Index);

            for (unsigned int i = 0; i < other.Current_Index; i++){

                Data[Current_Index++] = other.Data[i];
            }
        }

        void Add(const Compact_String& other){
            Data[Current_Index++] = other;
        }

        std::string To_String(){
            unsigned int Overall_Size = 0;

            for(unsigned int i = 0; i < Current_Index; i++){
                Overall_Size += Data[i].Size;
            }

            std::string result;
            result.resize(Overall_Size);

            int Current_UTF_Insert_Index = 0;
            for(unsigned int i = 0; i < Current_Index; i++){
                Compact_String data = Data[i];

                // Size of ones are always already loaded from memory into a char.
                if (data.Size > 1){
                    result.replace(Current_UTF_Insert_Index, data.Size, data.Data.Unicode_Data);

                    Current_UTF_Insert_Index += data.Size;
                }
                else{
                    result[Current_UTF_Insert_Index++] = data.Data.Ascii_Data;
                }
            }

            return result;
        }
    };

    namespace SYMBOLS{
        static const std::string TOP_LEFT_CORNER = "┌";//"\e(0\x6c\e(B";
        static const std::string BOTTOM_LEFT_CORNER = "└";//"\e(0\x6d\e(B";
        static const std::string TOP_RIGHT_CORNER = "┐";//"\e(0\x6b\e(B";
        static const std::string BOTTOM_RIGHT_CORNER = "┘";//"\e(0\x6a\e(B";
        static const std::string VERTICAL_LINE = "│";//"\e(0\x78\e(B";
        static const std::string HORIZONTAL_LINE = "─";//"\e(0\x71\e(B";
        static const std::string VERTICAL_RIGHT_CONNECTOR = "├";//"\e(0\x74\e(B";
        static const std::string VERTICAL_LEFT_CONNECTOR = "┤";//"\e(0\x75\e(B";
        static const std::string HORIZONTAL_BOTTOM_CONNECTOR = "┬";//"\e(0\x76\e(B";
        static const std::string HORIZONTAL_TOP_CONNECTOR = "┴";//"\e(0\x77\e(B";
        static const std::string CROSS_CONNECTOR = "┼";//"\e(0\x6e\e(B";

        static const std::string CENTERED_HORIZONTAL_LINE = "━";//"\e(0\x2501\e(B";
        static const std::string FULL_BLOCK = "█";//"\e(0\x2588\e(B";

        inline unsigned int CONNECTS_UP = 1 << 0;
        inline unsigned int CONNECTS_DOWN = 1 << 1;
        inline unsigned int CONNECTS_LEFT = 1 << 2;
        inline unsigned int CONNECTS_RIGHT = 1 << 3;

        static const std::string RADIOBUTTON_OFF = "○";
        static const std::string RADIOBUTTON_ON = "◉";

        static const std::string EMPTY_CHECK_BOX = "☐";
        static const std::string CHECKED_CHECK_BOX = "☒";

        extern GGUI::UTF EMPTY_UTF;
    }

    namespace TIME{
        inline constexpr static  unsigned int MILLISECOND = 1; 
        inline constexpr static  unsigned int SECOND = MILLISECOND * 1000;
        inline constexpr static  unsigned int MINUTE = SECOND * 60;
        inline constexpr static  unsigned int HOUR = MINUTE * 60;
    }

    // Inits with 'NOW()' when created
    class BUTTON_STATE{
    public:
        bool State = false;
        std::chrono::high_resolution_clock::time_point Capture_Time;

        BUTTON_STATE(bool state = false){
            Capture_Time = std::chrono::high_resolution_clock::now();
            State = state;
        }
    };

    namespace Constants{
        namespace ANSI{
            // 1 to ESC_CODE
            // 1 to Text_Color | Background_Color
            // 1 to SEPARATE
            // 1 to USE_RGB
            // 1 to SEPARATE
            static const unsigned int Maximum_Needed_Pre_Allocation_For_Over_Head = 1 + 1 + 1 + 1 + 1;

            // 1 to Red
            // 1 to SEPARATE
            // 1 to Green
            // 1 to SEPARATE
            // 1 to Blue
            static const unsigned int Maximum_Needed_Pre_Allocation_For_Color = 1 + 1 + 1 + 1 + 1;

            // 5 to Text_Overhead
            // 5 to Text_Colour
            // 1 to END_COMMAND
            // 5 to Background_Overhead
            // 5 to Background_Colour
            // 1 to END_COMMAND
            // 1 to Data
            // 1 to RESET_COLOR
            static const unsigned int Maximum_Needed_Pre_Allocation_For_Encoded_Super_String = 
                Maximum_Needed_Pre_Allocation_For_Over_Head + Maximum_Needed_Pre_Allocation_For_Color + 1 +
                Maximum_Needed_Pre_Allocation_For_Over_Head + Maximum_Needed_Pre_Allocation_For_Color + 1 + 1 + 1;
        
            // 1 to Escape code
            // 1 to private SGR telltale '?'
            // 1 to Feature to be disabled or enabled
            // 1 to Enable/Disable feature told above.
            static const unsigned int Maximum_Needed_Pre_Allocation_For_Enabling_Or_Disabling_Private_SGR_Feature = 1 + 1 + 1 + 1;
        
            // 1 to Escape code
            // 1 to feature to be enabled
            // 1 to END_COMMAND
            static const unsigned int Maximum_Needed_Pre_Allocation_For_Enabling_Or_Disabling_SGR_Feature = 1 + 1 + 1;

            // CSI (Control Sequence Introducer) sequences.
            static const std::string ESC_CODE = "\x1B[";      // Also known as \e[ or \o33
            static const std::string SEPARATE = ";";
            static const std::string USE_RGB = "2";
            static const std::string END_COMMAND = "m";
            static const std::string CLEAR_SCREEN = ESC_CODE + "2J";
            static const std::string CLEAR_SCROLLBACK = ESC_CODE + "3J";
            static const std::string SET_CURSOR_TO_START = ESC_CODE + "H";
            static const std::string RESET_CONSOLE = ESC_CODE + "c";
            static const std::string RESET_COLOR = ESC_CODE + '0' + END_COMMAND;  // Basically same as RESET_SGR but baked the end command into it for Super_String

            inline Super_String Enable_Private_SGR_Feature(const std::string& command, bool Enable = true) { 
                Super_String Result(Maximum_Needed_Pre_Allocation_For_Enabling_Or_Disabling_Private_SGR_Feature);

                Result.Add(ESC_CODE);
                Result.Add('?');
                Result.Add(command);

                if (Enable)
                    Result.Add('h');
                else
                    Result.Add('l');

                return Result;
            }

            // SGR (Select Graphic Rendition)
            
            // Since most of the SGR have the disable code after 20, we can make a pair of helper functions.
            // Also usually only those with pair of enable and disable codes are supported widely.
            inline Super_String Enable_SGR_Feature(const std::string& command) {
                Super_String Result(Maximum_Needed_Pre_Allocation_For_Enabling_Or_Disabling_SGR_Feature);

                Result.Add(ESC_CODE);
                Result.Add(command);
                Result.Add(END_COMMAND);

                return Result;
            }

            // SGR constants
            static const std::string RESET_SGR = "0";                                  // Removes all SGR features. 
            static const std::string BOLD = "1";                                       // Not widely supported!
            static const std::string FAINT = "2";                                      // Not widely supported!
            static const std::string ITALIC = "3";                                     // Not widely supported! (Can also be same as blink)
            static const std::string UNDERLINE = "4";              
            static const std::string SLOW_BLINK = "5";                                 // ~150 BPM
            static const std::string RAPID_BLINK = "6";                                // Not widely supported!
            static const std::string INVERT_FOREGROUND_WITH_BACKGROUND = "7";          // Not widely supported!
            static const std::string CONCEAL = "8";                                    // Not widely supported!
            static const std::string CROSSED_OUT = "9";                                // Not widely supported!
            static const std::string PRIMARY_FONT = "10";                              // Sets the default font.
            static const std::string ALTERNATIVE_FONT_1 = "11";                        // Custom font slot.
            static const std::string ALTERNATIVE_FONT_2 = "12";                        // Custom font slot.
            static const std::string ALTERNATIVE_FONT_3 = "13";                        // Custom font slot.
            static const std::string ALTERNATIVE_FONT_4 = "14";                        // Custom font slot.
            static const std::string ALTERNATIVE_FONT_5 = "15";                        // Custom font slot.
            static const std::string ALTERNATIVE_FONT_6 = "16";                        // Custom font slot.
            static const std::string ALTERNATIVE_FONT_7 = "17";                        // Custom font slot.
            static const std::string ALTERNATIVE_FONT_8 = "18";                        // Custom font slot.
            static const std::string ALTERNATIVE_FONT_9 = "19";                        // Custom font slot.
            static const std::string FRAKTUR = "20";                                   // Not widely supported! (But cool font)
            static const std::string NOT_BOLD = "21";                                  // Removes the BOLD feature
            static const std::string NORMAL_INTENSITY = "22";                          // Removes BOLD and ITALIC and other affixes.
            static const std::string NOT_UNDERLINE = "23";                             // Removes UNDERLINE.
            static const std::string NOT_BLINK = "24";                                 // Removes BLINK.
            static const std::string INVERT_INVERT_FOREGROUND_WITH_BACKGROUND = "27";  // Inverts the INVERT_FOREGROUND_WITH_BACKGROUND.
            static const std::string TEXT_COLOR = "38";                               // Sets the foreground color.
            static const std::string DEFAULT_TEXT_COLOR = "39";                       // Sets the default color.
            static const std::string BACKGROUND_COLOR = "48";                         // Sets the background color.
            static const std::string DEFAULT_BACKGROUND_COLOR = "49";                 // Sets the default color.

            // Private SGR codes
            static const std::string REPORT_MOUSE_HIGHLIGHTS = "1000";
            static const std::string REPORT_MOUSE_BUTTON_WHILE_MOVING = "1002";
            static const std::string REPORT_MOUSE_ALL_EVENTS = "1003";

            static const std::string MOUSE_CURSOR = "25";
            static const std::string SCREEN_CAPTURE = "47"; // 47l = restores screen, 47h = saves screen
            static const std::string ALTERNATIVE_SCREEN_BUFFER = "1049"; // 1049l = disables alternative buffer, 1049h = enables alternative buffer
            // End of enable settings for ANSI

            // ACC (ASCII Control Characters)
            inline char NONE = 0;
            inline char START_OF_HEADING = 1;
            inline char START_OF_TEXT = 2;
            inline char END_OF_TEXT = 3;
            inline char END_OF_TRANSMISSION = 4;
            inline char ENQUIRY = 5;
            inline char ACKNOWLEDGE = 6;
            inline char BELL = 7;
            inline char BACKSPACE = 8;
            inline char HORIZONTAL_TAB = 9;
            inline char LINE_FEED = 10;             // Also known as newline
            inline char VERTICAL_TAB = 11;
            inline char FORM_FEED = 12;
            inline char CARRIAGE_RETURN = 13;
            inline char SHIFT_OUT = 14;
            inline char SHIFT_IN = 15;
            inline char DATA_LINK_ESCAPE = 16;
            inline char DEVICE_CONTROL_1 = 17;
            inline char DEVICE_CONTROL_2 = 18;
            inline char DEVICE_CONTROL_3 = 19;
            inline char DEVICE_CONTROL_4 = 20;
            inline char NEGATIVE_ACKNOWLEDGE = 21;
            inline char SYNCHRONOUS_IDLE = 22;
            inline char END_OF_TRANSMISSION_BLOCK = 23;
            inline char CANCEL = 24;
            inline char END_OF_MEDIUM = 25;
            inline char SUBSTITUTE = 26;
            inline char ESCAPE = 27;
            inline char FILE_SEPARATOR = 28;
            inline char GROUP_SEPARATOR = 29;
            inline char RECORD_SEPARATOR = 30;
            inline char UNIT_SEPARATOR = 31;
        }

        inline unsigned long long NONE = (unsigned long long)1 << 0;
        inline unsigned long long ENTER = (unsigned long long)1 << 1;
        inline unsigned long long ESCAPE = (unsigned long long)1 << 2;
        inline unsigned long long BACKSPACE = (unsigned long long)1 << 3;
        inline unsigned long long TAB = (unsigned long long)1 << 4;
        inline unsigned long long UP = (unsigned long long)1 << 5;
        inline unsigned long long DOWN = (unsigned long long)1 << 6;
        inline unsigned long long LEFT = (unsigned long long)1 << 7;
        inline unsigned long long RIGHT = (unsigned long long)1 << 8;
        inline unsigned long long SPACE = (unsigned long long)1 << 9;
        inline unsigned long long SHIFT = (unsigned long long)1 << 10;
        inline unsigned long long ALT = (unsigned long long)1 << 11;
        inline unsigned long long CONTROL = (unsigned long long)1 << 12;
        inline unsigned long long SUPER = (unsigned long long)1 << 13;
        inline unsigned long long HOME = (unsigned long long)1 << 14;
        inline unsigned long long INSERT = (unsigned long long)1 << 15;
        inline unsigned long long DELETE = (unsigned long long)1 << 16;
        inline unsigned long long END = (unsigned long long)1 << 17;
        inline unsigned long long PAGE_UP = (unsigned long long)1 << 18;
        inline unsigned long long PAGE_DOWN = (unsigned long long)1 << 19;
        inline unsigned long long F0 = (unsigned long long)1 << 20;
        inline unsigned long long F1 = (unsigned long long)1 << 21;
        inline unsigned long long F2 = (unsigned long long)1 << 22;
        inline unsigned long long F3 = (unsigned long long)1 << 23;
        inline unsigned long long F4 = (unsigned long long)1 << 24;
        inline unsigned long long F5 = (unsigned long long)1 << 25;
        inline unsigned long long F6 = (unsigned long long)1 << 26;
        inline unsigned long long F7 = (unsigned long long)1 << 27;
        inline unsigned long long F8 = (unsigned long long)1 << 28;
        inline unsigned long long F9 = (unsigned long long)1 << 29;
        inline unsigned long long F10 = (unsigned long long)1 << 30;
        inline unsigned long long F11 = (unsigned long long)1 << 31;
        inline unsigned long long F12 = (unsigned long long)1 << 32;
        inline unsigned long long F13 = (unsigned long long)1 << 33;
        inline unsigned long long F14 = (unsigned long long)1 << 34;
        inline unsigned long long F15 = (unsigned long long)1 << 35;
        inline unsigned long long F16 = (unsigned long long)1 << 36;

        // Should not fucking exist bro!
        //inline unsigned long long SHIFT_TAB = (unsigned long long)1 << 37;


        //key_Press includes [a-z, A-Z] & [0-9]
        inline unsigned long long KEY_PRESS = (unsigned long long)1 << 38;

        // EASY MOUSE API
        inline unsigned long long MOUSE_LEFT_CLICKED = (unsigned long long)1 << 39;
        inline unsigned long long MOUSE_MIDDLE_CLICKED = (unsigned long long)1 << 40;
        inline unsigned long long MOUSE_RIGHT_CLICKED = (unsigned long long)1 << 41;

        // NOTE: These will be spammed until it is not pressed anymore!
        inline unsigned long long MOUSE_LEFT_PRESSED = (unsigned long long)1 << 42;
        inline unsigned long long MOUSE_MIDDLE_PRESSED = (unsigned long long)1 << 43;
        inline unsigned long long MOUSE_RIGHT_PRESSED = (unsigned long long)1 << 44;

        inline unsigned long long MOUSE_MIDDLE_SCROLL_UP = (unsigned long long)1 << 45;
        inline unsigned long long MOUSE_MIDDLE_SCROLL_DOWN = (unsigned long long)1 << 46;
    
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

        static const Compact_String To_Compact[256] = {
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

    inline std::unordered_map<std::string, unsigned long long> BUTTON_STATES_TO_CONSTANTS_BRIDGE = {

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

    class RGB{
    public:
        unsigned char Red = 0;
        unsigned char Green = 0;
        unsigned char Blue = 0;

        constexpr RGB(unsigned char r, unsigned char g, unsigned char b, [[maybe_unused]] bool Use_Const){
            Red = r;
            Green = g;
            Blue = b;
        }
        
        RGB(unsigned char r, unsigned char g, unsigned char b){
            Red = r;
            Green = g;
            Blue = b;
        }

        // Takes in a Hexadecimal representation of the RGB value, where 0xFFFFFF is White and 0x000000 is Black  
        RGB(unsigned short hex){
            Red = (hex >> 16) & 0xFF;
            Green = (hex >> 8) & 0xFF;
            Blue = hex & 0xFF;
        }

        RGB(){}

        std::string Get_Colour() const;

        // Needs the Result to be initialized with atleast Maximum_Needed_Pre_Allocation_For_Color
        void Get_Colour_As_Super_String(Super_String* Result) const;
    
        std::string Get_Over_Head(bool Is_Text_Color = true) const{
            if(Is_Text_Color){
                return Constants::ANSI::ESC_CODE + Constants::ANSI::TEXT_COLOR + Constants::ANSI::SEPARATE + Constants::ANSI::USE_RGB + Constants::ANSI::SEPARATE;
            }
            else{
                return Constants::ANSI::ESC_CODE + Constants::ANSI::BACKGROUND_COLOR + Constants::ANSI::SEPARATE + Constants::ANSI::USE_RGB + Constants::ANSI::SEPARATE;
            }
        }

        // Needs the Result to be initialized with atleast Maximum_Needed_Pre_Allocation_For_Over_Head
        void Get_Over_Head_As_Super_String(Super_String* Result, bool Is_Text_Color = true) const{
            if (Is_Text_Color){
                Result->Add(Constants::ANSI::ESC_CODE);
                Result->Add(Constants::ANSI::TEXT_COLOR);
                Result->Add(Constants::ANSI::SEPARATE);
                Result->Add(Constants::ANSI::USE_RGB);
                Result->Add(Constants::ANSI::SEPARATE);
            }
            else{
                Result->Add(Constants::ANSI::ESC_CODE);
                Result->Add(Constants::ANSI::BACKGROUND_COLOR);
                Result->Add(Constants::ANSI::SEPARATE);
                Result->Add(Constants::ANSI::USE_RGB);
                Result->Add(Constants::ANSI::SEPARATE);
            }
        }
    
        bool operator==(const RGB& Other) const{
            // only take the bits from the first 3 unsigned chars
            return (*(unsigned int*)this & 0xFFFFFF) == (*(unsigned int*)&Other & 0xFFFFFF);
        }
    
        RGB operator+(const RGB& Other) const{
            return RGB(Red + Other.Red, Green + Other.Green, Blue + Other.Blue);
        }

    };

    class RGBA : public RGB{
    public:
        unsigned char Alpha = std::numeric_limits<unsigned char>::max();

        RGBA(unsigned char r, unsigned char g, unsigned char b, unsigned char a = std::numeric_limits<unsigned char>::max()) : RGB(r, g, b){
            Alpha = a;
        }

        constexpr RGBA(unsigned char r, unsigned char g, unsigned char b, unsigned char a, bool Use_Const) : RGB(r, g, b, Use_Const){
            Alpha = a;
        }

        RGBA(){}

        RGBA(RGB primal) : RGB(primal){}
    
        // For float API's
        constexpr void Set_Alpha(float a){
            Alpha = (unsigned char)(a * std::numeric_limits<unsigned char>::max());
        }

        constexpr float Get_Alpha() const{
            return (float)Alpha / std::numeric_limits<unsigned char>::max();
        }

        bool operator==(const RGBA& Other){
            // only take the bits which are the 4 unsigned chars, which is same as single integer.
            // Starting from Red since Red is the first member and normally Virtual function will also allocate their own space before members.
            return *(unsigned int*)&this->Red == *(unsigned int*)&Other;
        }

        RGBA operator*(const RGBA& Other){
            float Decimal_Alpha = Other.Get_Alpha();
            // Make the reverse alpha
            float Reverse_Alpha = 1 - Decimal_Alpha;
            
            return RGBA(
                ((float)this->Red * Reverse_Alpha) * ((float)Other.Red * Decimal_Alpha), 
                ((float)this->Green * Reverse_Alpha) * ((float)Other.Green * Decimal_Alpha), 
                ((float)this->Blue * Reverse_Alpha) * ((float)Other.Blue * Decimal_Alpha),
                Alpha
            );
        }

        RGBA operator+(const RGBA& Other){
            float Decimal_Alpha = Other.Get_Alpha();
            // Make the reverse alpha
            float Reverse_Alpha = 1 - Decimal_Alpha;

            return RGBA(
                ((float)this->Red * Reverse_Alpha) + ((float)Other.Red * Decimal_Alpha), 
                ((float)this->Green * Reverse_Alpha) + ((float)Other.Green * Decimal_Alpha), 
                ((float)this->Blue * Reverse_Alpha) + ((float)Other.Blue * Decimal_Alpha),
                Alpha
            );
        }

        RGBA operator*=(const RGBA& Other){
            float Decimal_Alpha = Other.Get_Alpha();
            // Make the reverse alpha
            float Reverse_Alpha = 1 - Decimal_Alpha;

            this->Red = ((float)this->Red * Reverse_Alpha) * ((float)Other.Red * Decimal_Alpha);
            this->Green = ((float)this->Green * Reverse_Alpha) * ((float)Other.Green * Decimal_Alpha);
            this->Blue = ((float)this->Blue * Reverse_Alpha) * ((float)Other.Blue * Decimal_Alpha);

            return *this;
        }

        RGBA operator+=(const RGBA& Other){
            float Decimal_Alpha = Other.Get_Alpha();
            // Make the reverse alpha
            float Reverse_Alpha = 1 - Decimal_Alpha;

            this->Red = ((float)this->Red * Reverse_Alpha) + ((float)Other.Red * Decimal_Alpha);
            this->Green = ((float)this->Green * Reverse_Alpha) + ((float)Other.Green * Decimal_Alpha);
            this->Blue = ((float)this->Blue * Reverse_Alpha) + ((float)Other.Blue * Decimal_Alpha);

            return *this;
        }

    };

    namespace COLOR{
        static constexpr RGB WHITE = RGB(255, 255, 255, true);
        static constexpr RGB BLACK = RGB(0, 0, 0, true);
        static constexpr RGB RED = RGB(255, 0, 0, true);
        static constexpr RGB GREEN = RGB(0, 255, 0, true);
        static constexpr RGB BLUE = RGB(0, 0, 255, true);
        static constexpr RGB YELLOW = RGB(255, 255, 0, true);
        static constexpr RGB ORANGE = RGB(255, 128, 0, true);
        static constexpr RGB CYAN = RGB(0, 255, 255, true);
        static constexpr RGB TEAL = RGB(0, 128, 128, true);
        static constexpr RGB MAGENTA = RGB(255, 0, 255, true);
        static constexpr RGB GRAY = RGB(128, 128, 128, true);
        static constexpr RGB LIGHT_RED = RGB(255, 128, 128, true);
        static constexpr RGB LIGHT_GREEN = RGB(128, 255, 128, true);
        static constexpr RGB LIGHT_BLUE = RGB(128, 128, 255, true);
        static constexpr RGB LIGHT_YELLOW = RGB(255, 255, 128, true);
        static constexpr RGB LIGHT_CYAN = RGB(128, 255, 255, true);
        static constexpr RGB LIGHT_MAGENTA = RGB(255, 128, 255, true);
        static constexpr RGB LIGHT_GRAY = RGB(192, 192, 192, true);
        static constexpr RGB DARK_RED = RGB(128, 0, 0, true);
        static constexpr RGB DARK_GREEN = RGB(0, 128, 0, true);
        static constexpr RGB DARK_BLUE = RGB(0, 0, 128, true);
        static constexpr RGB DARK_YELLOW = RGB(128, 128, 0, true);
        static constexpr RGB DARK_CYAN = RGB(0, 128, 128, true);
        static constexpr RGB DARK_MAGENTA = RGB(128, 0, 128, true);
        static constexpr RGB DARK_GRAY = RGB(64, 64, 64, true);
    }

    class Vector2{
    public:
        float X = 0;
        float Y = 0;

        Vector2(float x, float y){
            X = x;
            Y = y;
        }

        Vector2(){}

        Vector2 operator+(float num){
            return Vector2(X + num, Y + num);
        }

        Vector2 operator-(float num){
            return Vector2(X - num, Y - num);
        }

        Vector2 operator*(float num){
            return Vector2(X * num, Y * num);
        }
    };

    class Vector3 : public Vector2{
    public:
        float Z = 0;

        Vector3(float x, float y, float z){
            Z = z;
            X = x;
            Y = y;
        }

        Vector3(){}
    };

    class Coordinates{
    public:
        int X = 0;  //Horizontal
        int Y = 0;  //Vertical
        int Z = 0;  //priority (the higher the more likely it will be at top).

        Coordinates(int x = 0, int y = 0, int z = 0){
            X = x;
            Y = y;
            Z = z;
        }

        void operator+=(Coordinates* other){
            X += other->X;
            Y += other->Y;
            Z += other->Z;
        }

        void operator+=(Vector2 other){
            X += other.X;
            Y += other.Y;
        }

        void operator+=(Coordinates other){
            X += other.X;
            Y += other.Y;
            Z += other.Z;
        }
    
        Coordinates operator+(Coordinates& other){
            return Coordinates(X + other.X, Y + other.Y, Z + other.Z);
        }
    
        std::string To_String(){
            return std::to_string(X) + ", " + std::to_string(Y) + ", " + std::to_string(Z);
        }
    };

    class UTF{
    public:
        unsigned char FLAGS = UTF_FLAG::IS_ASCII;

        char Ascii = ' ';
        const char* Unicode = " ";
        int Unicode_Length = 1; // Does not include the null terminator.

        RGBA Foreground; 
        RGBA Background;

        UTF(){}

        ~UTF(){}

        constexpr UTF(const GGUI::UTF& other) : FLAGS(other.FLAGS), Ascii(other.Ascii), Unicode(other.Unicode), Unicode_Length(other.Unicode_Length), Foreground(other.Foreground), Background(other.Background){}

        // {Foreground, Background}
        UTF(char data, std::pair<RGB, RGB> color = {{}, {}}){
            Ascii = data;
            Foreground = {color.first};
            Background = {color.second};
            FLAGS = UTF_FLAG::IS_ASCII;
        }

        UTF(const char* data, std::pair<RGB, RGB> color = {{}, {}}){
            Unicode = data;
            Unicode_Length = std::strlen(data);
            
            Foreground = {color.first};
            Background = {color.second};
            FLAGS = UTF_FLAG::IS_UNICODE;
        }

        UTF(const std::string& data, std::pair<RGB, RGB> color = {{}, {}}){
            Unicode = data.data();
            Unicode_Length = data.size() - 1;
            
            Foreground = {color.first};
            Background = {color.second};
            FLAGS = UTF_FLAG::IS_UNICODE;
        }

        UTF(const Compact_String CS, std::pair<RGB, RGB> color = {{}, {}}){
            if (CS.Size == 1){
                Ascii = CS.Data.Ascii_Data;
                Foreground = {color.first};
                Background = {color.second};
                FLAGS = UTF_FLAG::IS_ASCII;
            }
            else{
                Unicode = CS.Data.Unicode_Data;
                Unicode_Length = CS.Size;
                Foreground = {color.first};
                Background = {color.second};
                FLAGS = UTF_FLAG::IS_UNICODE;
            }
        }

        bool Is(unsigned char utf_flag){
            // Check if the bit mask contains the bits
            return (FLAGS & utf_flag) > 0;
        }

        void Set_Flag(unsigned char utf_flag){
            FLAGS |= utf_flag;
        }

        void Set_Foreground(RGB color){
            Foreground = color;
        }

        void Set_Background(RGB color){
            Background = color;
        }

        void Set_Color(std::pair<RGB, RGB> primals){
            Foreground = primals.first;
            Background = primals.second;
        }

        void Set_Text(std::string data){
            Unicode = data.data();
            Unicode_Length = data.size() -1;
            FLAGS = UTF_FLAG::IS_UNICODE;
        }

        void Set_Text(char data){
            Ascii = data;
            FLAGS = UTF_FLAG::IS_ASCII;
        }

        void Set_Text(const char* data){
            Unicode = data;
            Unicode_Length = std::strlen(data);
            FLAGS = UTF_FLAG::IS_UNICODE;
        }

        void Set_Text(UTF other){
            Ascii = other.Ascii;
            Unicode = other.Unicode;
            Unicode_Length = other.Unicode_Length;
            FLAGS = other.FLAGS;
        }

        std::string To_String();
        std::string To_Encoded_String();    // For UTF Strip Encoding.

        // Needs Result to be initalized with Maximum_Needed_Pre_Allocation_For_Super_String at max.
        void To_Super_String(GGUI::Super_String* Result, Super_String* Text_Overhead, Super_String* Background_Overhead, Super_String* Text_Colour, Super_String* Background_Colour);
        
        // Needs Result to be initalized with Maximum_Needed_Pre_Allocation_For_Super_String at max.
        void To_Encoded_Super_String(Super_String* Result, Super_String* Text_Overhead, Super_String* Background_Overhead, Super_String* Text_Colour, Super_String* Background_Colour);

        void operator=(char text){
            Set_Text(text);
        }

        void operator=(const std::string& text){
            Set_Text(text);
        }

        UTF& operator=(const UTF& other){
            Ascii = other.Ascii;
            Unicode = other.Unicode;
            Unicode_Length = other.Unicode_Length;
            FLAGS = other.FLAGS;
            Foreground = other.Foreground;
            Background = other.Background;

            return *this;
        }

        inline bool Has_Default_Text(){
            if (Is(UTF_FLAG::IS_ASCII))
                return Ascii == ' ';
            else
                return Unicode[0] == ' ';
        }

    };

    class Event{
    public:
        unsigned long long Criteria;
    };

    class Input : public Event{
    public:
        char Data = 0;
        unsigned int X = 0;
        unsigned int Y = 0;
        int Scale = 1;

        // The input information like the character written.
        Input(char d, unsigned long long t){
            Data = d;
            Criteria = t;
        }

        Input(Coordinates c, unsigned long long t, int s = 1){
            X = c.X;
            Y = c.Y;
            Criteria = t;
            Scale = s;
        }
    };

    class Action : public Event{
    public:
        class Element* Host = nullptr;

        std::function<bool(GGUI::Event*)> Job;
        
        std::string ID; 
    
        Action() = default;
        Action(unsigned long long criteria, std::function<bool(GGUI::Event*)> job, std::string id = ""){
            Criteria = criteria;
            Job = job;
            Host = nullptr;
            ID = id;
        }

        Action(unsigned long long criteria, std::function<bool(GGUI::Event*)> job, class Element* host, std::string id = ""){
            Criteria = criteria;
            Job = job;
            Host = host;
            ID = id;
        }
    };

    namespace MEMORY_FLAGS{
        inline unsigned char PROLONG_MEMORY     = 1 << 0;
        inline unsigned char RETRIGGER          = 1 << 1;
    };

    class Memory : public Action{
    public:
        std::chrono::high_resolution_clock::time_point Start_Time;
        size_t End_Time = 0;

        // By default all memories automatically will not prolong each other similar memories.
        unsigned char Flags = 0x0;

        // When the job starts, job, prolong previous similar job by this time.
        Memory(size_t end, std::function<bool(GGUI::Event*)>job, unsigned char flags = 0x0, std::string id = ""){
            Start_Time = std::chrono::high_resolution_clock::now();
            End_Time = end;
            Job = job;
            Flags = flags;
            ID = id;
        }

        bool Is(unsigned char f){
            return (Flags & f) > 0;
        }

        void Set(unsigned char f){
            Flags |= f;
        }
    };

    // --STYLING STUFF--

    enum class ALIGN{
        UP,
        DOWN,
        LEFT,
        RIGHT,
        CENTER
    };

    enum class VALUE_STATE{
        UNINITIALIZED,
        INITIALIZED,
        VALUE
    };

    enum class DIRECTION{
        ROW,
        COLUMN
    };

    class VALUE{
    public:
        VALUE_STATE Status = VALUE_STATE::UNINITIALIZED;

        constexpr VALUE(VALUE_STATE status, [[maybe_unused]] bool use_constexpr) : Status(status){}
        
        VALUE(VALUE_STATE status) : Status(status){}

        VALUE() = default;
    };

    class MARGIN_VALUE : public VALUE{
    public:
        unsigned int Top = 0;
        unsigned int Bottom = 0;
        unsigned int Left = 0;
        unsigned int Right = 0;

        MARGIN_VALUE(unsigned int top = 0, unsigned int bottom = 0, unsigned int left = 0, unsigned int right = 0, VALUE_STATE Default = VALUE_STATE::VALUE) : VALUE(Default){
            Top = top;
            Bottom = bottom;
            Left = left;
            Right = right;
        }

        // operator overload for copy operator
        MARGIN_VALUE& operator=(const MARGIN_VALUE& other){
            // Only copy the information if the other is enabled.
            if (other.Status >= Status){
                Top = other.Top;
                Bottom = other.Bottom;
                Left = other.Left;
                Right = other.Right;

                Status = other.Status;
            }
            return *this;
        }

        constexpr MARGIN_VALUE(const GGUI::MARGIN_VALUE& other) : VALUE(other.Status, true){
            Top = other.Top;
            Bottom = other.Bottom;
            Left = other.Left;
            Right = other.Right;
        }
    };

    class COORDINATES_VALUE : public VALUE{
    public:
        Coordinates Value = Coordinates();

        COORDINATES_VALUE(Coordinates value, VALUE_STATE Default = VALUE_STATE::VALUE) : VALUE(Default){
            Value = value;
        }

        COORDINATES_VALUE() = default;

        // operator overload for copy operator
        COORDINATES_VALUE& operator=(const COORDINATES_VALUE& other){
            // Only copy the information if the other is enabled.
            if (other.Status >= Status){
                Value = other.Value;

                Status = other.Status;
            }
            return *this;
        }

        COORDINATES_VALUE& operator=(const GGUI::Coordinates other){
            Value = other;
            Status = VALUE_STATE::VALUE;
            return *this;
        }

        constexpr COORDINATES_VALUE(const GGUI::COORDINATES_VALUE& other) : VALUE(other.Status, true), Value(other.Value){}
    };

    class SHADOW_VALUE : public VALUE{
    public:
        Vector3 Direction = {0, 0, 0.5};
        RGB Color = {};
        float Opacity = 1;
        bool Enabled = false;

        SHADOW_VALUE(Vector3 direction, RGB color, float opacity, bool enabled, VALUE_STATE Default = VALUE_STATE::VALUE) : VALUE(Default){
            Direction = direction;
            Color = color;
            Opacity = opacity;
            Enabled = enabled;
        }

        SHADOW_VALUE() : VALUE(){}

        SHADOW_VALUE& operator=(const SHADOW_VALUE& other){
            // Only copy the information if the other is enabled.
            if (other.Status >= Status){
                Direction = other.Direction;
                Color = other.Color;
                Opacity = other.Opacity;
                Enabled = other.Enabled;

                Status = other.Status;
            }
            return *this;
        }
    
        constexpr SHADOW_VALUE(const GGUI::SHADOW_VALUE& other) : VALUE(other.Status, true), Direction(other.Direction), Color(other.Color), Opacity(other.Opacity), Enabled(other.Enabled){}
    };

    class BORDER_STYLE_VALUE : public VALUE{
    public:
        const char* TOP_LEFT_CORNER             = "┌";//"\e(0\x6c\e(B";
        const char* BOTTOM_LEFT_CORNER          = "└";//"\e(0\x6d\e(B";
        const char* TOP_RIGHT_CORNER            = "┐";//"\e(0\x6b\e(B";
        const char* BOTTOM_RIGHT_CORNER         = "┘";//"\e(0\x6a\e(B";
        const char* VERTICAL_LINE               = "│";//"\e(0\x78\e(B";
        const char* HORIZONTAL_LINE             = "─";//"\e(0\x71\e(B";
        const char* VERTICAL_RIGHT_CONNECTOR    = "├";//"\e(0\x74\e(B";
        const char* VERTICAL_LEFT_CONNECTOR     = "┤";//"\e(0\x75\e(B";
        const char* HORIZONTAL_BOTTOM_CONNECTOR = "┬";//"\e(0\x76\e(B";
        const char* HORIZONTAL_TOP_CONNECTOR    = "┴";//"\e(0\x77\e(B";
        const char* CROSS_CONNECTOR             = "┼";//"\e(0\x6e\e(B";

        BORDER_STYLE_VALUE(std::vector<const char*> values, VALUE_STATE Default = VALUE_STATE::VALUE);

        // Re-import defaults:
        BORDER_STYLE_VALUE() = default; // This should also call the base class
        ~BORDER_STYLE_VALUE() = default;
        BORDER_STYLE_VALUE& operator=(const BORDER_STYLE_VALUE& other){
            if (other.Status >= Status){
                TOP_LEFT_CORNER = other.TOP_LEFT_CORNER;
                BOTTOM_LEFT_CORNER = other.BOTTOM_LEFT_CORNER;
                TOP_RIGHT_CORNER = other.TOP_RIGHT_CORNER;
                BOTTOM_RIGHT_CORNER = other.BOTTOM_RIGHT_CORNER;
                VERTICAL_LINE = other.VERTICAL_LINE;
                HORIZONTAL_LINE = other.HORIZONTAL_LINE;
                VERTICAL_RIGHT_CONNECTOR = other.VERTICAL_RIGHT_CONNECTOR;
                VERTICAL_LEFT_CONNECTOR = other.VERTICAL_LEFT_CONNECTOR;
                HORIZONTAL_BOTTOM_CONNECTOR = other.HORIZONTAL_BOTTOM_CONNECTOR;
                HORIZONTAL_TOP_CONNECTOR = other.HORIZONTAL_TOP_CONNECTOR;
                CROSS_CONNECTOR = other.CROSS_CONNECTOR;

                Status = other.Status;
            }
            return *this;
        }
    
        constexpr BORDER_STYLE_VALUE(const GGUI::BORDER_STYLE_VALUE& other) : VALUE(other.Status, true){
            TOP_LEFT_CORNER = other.TOP_LEFT_CORNER;
            BOTTOM_LEFT_CORNER = other.BOTTOM_LEFT_CORNER;
            TOP_RIGHT_CORNER = other.TOP_RIGHT_CORNER;
            BOTTOM_RIGHT_CORNER = other.BOTTOM_RIGHT_CORNER;
            VERTICAL_LINE = other.VERTICAL_LINE;
            HORIZONTAL_LINE = other.HORIZONTAL_LINE;
            VERTICAL_RIGHT_CONNECTOR = other.VERTICAL_RIGHT_CONNECTOR;
            VERTICAL_LEFT_CONNECTOR = other.VERTICAL_LEFT_CONNECTOR;
            HORIZONTAL_BOTTOM_CONNECTOR = other.HORIZONTAL_BOTTOM_CONNECTOR;
            HORIZONTAL_TOP_CONNECTOR = other.HORIZONTAL_TOP_CONNECTOR;
            CROSS_CONNECTOR = other.CROSS_CONNECTOR;
        }
    };

    class RGB_VALUE : public VALUE{
    public:
        RGB Value = RGB(0, 0, 0);

        RGB_VALUE(RGB value, VALUE_STATE Default = VALUE_STATE::VALUE) : VALUE(Default){
            Value = value;
        }

        RGB_VALUE() = default;

        // operator overload for copy operator
        RGB_VALUE& operator=(const RGB_VALUE& other){
            // Only copy the information if the other is enabled.
            if (other.Status >= Status){
                Value = other.Value;

                Status = other.Status;
            }
            return *this;
        }

        RGB_VALUE& operator=(const GGUI::RGB other){
            Value = other;
            Status = VALUE_STATE::VALUE;
            return *this;
        }
    
        constexpr RGB_VALUE(const GGUI::RGB_VALUE& other) : VALUE(other.Status, true), Value(other.Value){}
    };

    class BOOL_VALUE : public VALUE{
    public:
        bool Value = false;

        BOOL_VALUE(bool value, VALUE_STATE Default = VALUE_STATE::VALUE) : VALUE(Default){
            Value = value;
        }

        BOOL_VALUE() = default;

        // operator overload for copy operator
        BOOL_VALUE& operator=(const BOOL_VALUE& other){
            // Only copy the information if the other is enabled.
            if (other.Status >= Status){
                Value = other.Value;

                Status = other.Status;
            }
            return *this;
        }

        BOOL_VALUE& operator=(const bool other){
            Value = other;
            Status = VALUE_STATE::VALUE;
            return *this;
        }
    
        constexpr BOOL_VALUE(const GGUI::BOOL_VALUE& other) : VALUE(other.Status, true), Value(other.Value){}
    };
    
    class NUMBER_VALUE : public VALUE{
    public:
        int Value = 0;

        NUMBER_VALUE(int value, VALUE_STATE Default = VALUE_STATE::VALUE) : VALUE(Default){
            Value = value;
        }

        NUMBER_VALUE() = default;

        // operator overload for copy operator
        NUMBER_VALUE& operator=(const NUMBER_VALUE& other){
            // Only copy the information if the other is enabled.
            if (other.Status >= Status){
                Value = other.Value;

                Status = other.Status;
            }
            return *this;
        }

        NUMBER_VALUE& operator=(const int other){
            Value = other;
            Status = VALUE_STATE::VALUE;
            return *this;
        }
    
        constexpr NUMBER_VALUE(const GGUI::NUMBER_VALUE& other) : VALUE(other.Status, true), Value(other.Value){}
    };

    template<typename T>
    class ENUM_VALUE : public VALUE{
    public:
        T Value;

        ENUM_VALUE(T value, VALUE_STATE Default = VALUE_STATE::INITIALIZED) : VALUE(Default){
            Value = value;
        }

        ENUM_VALUE() = default;

        // operator overload for copy operator
        ENUM_VALUE& operator=(const ENUM_VALUE& other){
            // Only copy the information if the other is enabled.
            if (other.Status >= Status){
                Value = other.Value;

                Status = other.Status;
            }
            return *this;
        }

        ENUM_VALUE& operator=(const T other){
            Value = other;
            Status = VALUE_STATE::VALUE;
            return *this;
        }
    
        constexpr ENUM_VALUE(const GGUI::ENUM_VALUE<T>& other) : VALUE(other.Status, true), Value(other.Value){}
    };

    class Styling{
    public:
        BOOL_VALUE Border_Enabled = BOOL_VALUE(false, VALUE_STATE::INITIALIZED);
        RGB_VALUE Text_Color;
        RGB_VALUE Background_Color;
        RGB_VALUE Border_Color;
        RGB_VALUE Border_Background_Color;
        
        RGB_VALUE Hover_Border_Color;
        RGB_VALUE Hover_Text_Color;
        RGB_VALUE Hover_Background_Color;
        RGB_VALUE Hover_Border_Background_Color;

        RGB_VALUE Focus_Border_Color;
        RGB_VALUE Focus_Text_Color;
        RGB_VALUE Focus_Background_Color;
        RGB_VALUE Focus_Border_Background_Color;

        BORDER_STYLE_VALUE Border_Style;
        
        ENUM_VALUE<DIRECTION> Flow_Priority = ENUM_VALUE<DIRECTION>(DIRECTION::ROW, VALUE_STATE::INITIALIZED);
        BOOL_VALUE Wrap = BOOL_VALUE(false, VALUE_STATE::INITIALIZED);

        BOOL_VALUE Allow_Overflow = BOOL_VALUE(false, VALUE_STATE::INITIALIZED);
        BOOL_VALUE Allow_Dynamic_Size = BOOL_VALUE(false, VALUE_STATE::INITIALIZED);
        MARGIN_VALUE Margin;

        SHADOW_VALUE Shadow;
        NUMBER_VALUE Opacity = NUMBER_VALUE(100, VALUE_STATE::INITIALIZED);  // 100%

        BOOL_VALUE Allow_Scrolling = BOOL_VALUE(false, VALUE_STATE::INITIALIZED);

        // Only fetch one parent UP, and own position +, then child repeat.
        COORDINATES_VALUE Absolute_Position_Cache;

        ENUM_VALUE<ALIGN> Align = ENUM_VALUE<ALIGN>(ALIGN::LEFT, VALUE_STATE::INITIALIZED);

        Styling() = default;

        void Copy(const Styling& other);

        void Copy(const Styling* other){
            // use the reference one
            Copy(*other);
        }
    };

    namespace STYLES{
        namespace BORDER{
            const inline BORDER_STYLE_VALUE Double = std::vector<const char*>{
                "╔", "╚", "╗", "╝", "║", "═", "╠", "╣", "╦", "╩", "╬"
            };

            const inline BORDER_STYLE_VALUE Round = std::vector<const char*>{
                "╭", "╰", "╮", "╯", "│", "─", "├", "┤", "┬", "┴", "┼"
            };

            const inline BORDER_STYLE_VALUE Single = std::vector<const char*>{
                "┌", "└", "┐", "┘", "│", "─", "├", "┤", "┬", "┴", "┼"
            };

            const inline BORDER_STYLE_VALUE Bold = std::vector<const char*>{
                "▛", "▙", "▜", "▟", "█", "▅", "▉", "▉", "▉", "▉", "▉"
            };

            const inline BORDER_STYLE_VALUE Modern = std::vector<const char*>{
                "/", "\\", "\\", "/", "|", "-", "|", "|", "-", "-", "+"
            };
            
        }
    };

    // --END OF STYLING STUFF--

    enum class STAIN_TYPE{
        CLEAN = 0,              // No change
        COLOR = 1 << 0,         // BG and other color related changes
        EDGE = 1 << 1,          // Title and border changes.
        DEEP = 1 << 2,          // Children changes. Deep because the childs are connected via AST.
        STRETCH = 1 << 3,       // Width and or height changes.
        CLASS = 1 << 5,         // This is used to tell the renderer that there are still un_parsed classes.
        STATE = 1 << 6,         // This is for Switches that based on their state display one symbol differently. And also for state handlers.
        MOVE = 1 << 7,          // Enabled, to signal absolute position caching.
    };
 
    inline unsigned int operator|(STAIN_TYPE a, STAIN_TYPE b){
        return (unsigned int)a | (unsigned int)b;
    }

    inline unsigned int operator|(STAIN_TYPE a, unsigned int b){
        return (unsigned int)a | b;
    }

    inline unsigned int operator|(unsigned int a, STAIN_TYPE b){
        return a | (unsigned int)b;
    }

    class STAIN{
    public:
        STAIN_TYPE Type = STAIN_TYPE::CLEAN; //(STAIN_TYPE)(STAIN_TYPE::COLOR | STAIN_TYPE::EDGE | STAIN_TYPE::DEEP | STAIN_TYPE::STRETCH | STAIN_TYPE::CLASS | STAIN_TYPE::MOVE);


        bool is(STAIN_TYPE f){
            if (f == STAIN_TYPE::CLEAN){
                return Type <= f;
            }
            return ((unsigned int)Type & (unsigned int)f) == (unsigned int)f;
        }

        void Clean(STAIN_TYPE f){
            Type = (STAIN_TYPE)((unsigned int)Type & ~(unsigned int)f);
        }

        void Clean(unsigned int f){
            Type = (STAIN_TYPE)((unsigned int)Type & ~f);
        }

        void Dirty(STAIN_TYPE f){
            Type = (STAIN_TYPE)((unsigned int)Type | (unsigned int)f);
        }

        void Dirty(unsigned int f){
            Type = (STAIN_TYPE)((unsigned int)Type | f);
        }

        // void Stain_All(){
        //     Dirty(STAIN_TYPE::COLOR | STAIN_TYPE::EDGE | STAIN_TYPE::DEEP | STAIN_TYPE::STRETCH | STAIN_TYPE::CLASS | STAIN_TYPE::MOVE);
        // }

    };

    enum class Flags{
        Empty = 0,
        Border = 1 << 0,
        Text_Input = 1 << 1,
        Overflow = 1 << 2,
        Dynamic = 1 << 3,
        Horizontal = 1 << 4,
        Vertical = 1 << 5,
        Align_Left = 1 << 6,
        Align_Right = 1 << 7,
        Align_Center = 1 << 8,
    };
    
    inline Flags operator|(Flags a, Flags b){
        return static_cast<Flags>(static_cast<int>(a) | static_cast<int>(b));
    }

    inline bool Is(Flags a, Flags b){
        return ((int)a & (int)b) == (int)b;
    }

    inline bool Has(Flags a, Flags b){
        return ((int)a & (int)b) != 0;
    }

    enum class State{
        UNKNOWN,

        RENDERED,
        HIDDEN

    };

    namespace SETTINGS{
        // How fast for a detection of hold down situation.
        inline unsigned long long Mouse_Press_Down_Cooldown = 365;
        inline bool Word_Wrapping = true;
        inline std::chrono::milliseconds Thread_Timeout = std::chrono::milliseconds(256);
    };

    // For templates.
    extern std::vector<Action*> Event_Handlers;

    class Element{
    protected:
        Coordinates Position;

        unsigned int Width = 1;
        unsigned int Height = 1;

        unsigned int Post_Process_Width = 0;
        unsigned int Post_Process_Height = 0;

        //INTERNAL FLAGS
        class Element* Parent = nullptr;
        bool Show = true;
        
        std::vector<UTF> Render_Buffer;
        std::vector<UTF> Post_Process_Buffer;
        STAIN Dirty;
        
        std::vector<int> Classes;

        std::vector<Element*> Childs;

        bool Focused = false;
        bool Hovered = false;

        std::string Name = "";

        // NOTE: do NOT set the .VALUEs manually set each member straight with the operator= overload.
        Styling* Style = nullptr;

        std::unordered_map<State, std::function<void()>> State_Handlers;
    public:

        Element();

        Element(std::string Class, unsigned int width = 0, unsigned int height = 0, Element* parent = nullptr, Coordinates *position = nullptr);

        Element(Styling css, unsigned int width = 0, unsigned int height = 0, Element* parent = nullptr, Coordinates *position = nullptr);

        Element(
            unsigned int width,
            unsigned int height,
            Coordinates position
        );

        //These next constructors are mainly for users to more easily create elements.
        Element(
            unsigned int width,
            unsigned int height
        );

        Element(
            unsigned int width,
            unsigned int height,
            RGB text_color,
            RGB background_color
        );

        Element(
            unsigned int width,
            unsigned int height,
            RGB text_color,
            RGB background_color,
            RGB border_color,
            RGB border_background_color
        );

        // Disable Copy constructor
        Element(const Element&);

        Element& operator=(const GGUI::Element&) = default;

        //Start of destructors.
        //-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_

        virtual ~Element();

        //
        //-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_

        virtual Element* Safe_Move(){
            Element* new_element = new Element();
            *new_element = *(Element*)this;

            return new_element;
        }

        // Use this when you want to duplicate the same element with its properties safely.
        Element* Copy();

        virtual void Fully_Stain();

        // If you want to make a representing element* that isnt the same as the Abstract one.
        // Then Remember to USE THIS!
        void Inherit_States_From(Element* abstract);

        void Parse_Classes();

        STAIN& Get_Dirty(){
            return Dirty;
        }

        bool Is_Focused(){
            return Focused;
        }

        void Set_Focus(bool f);

        bool Is_Hovered(){
            return Hovered;
        }

        void Set_Hover_State(bool h);

        void Check(State s);

        Styling Get_Style();

        void Set_Style(Styling css);

        virtual void Calculate_Childs_Hitboxes([[maybe_unused]] unsigned int Starting_Offset = 0){}

        virtual Element* Handle_Or_Operator(Element* other){
            Set_Style(other->Get_Style());

            return this;
        }

        void Add_Class(std::string class_name);

        // Takes 0.0f to 1.0f
        void Set_Opacity(float Opacity);

        // RGBA - Alpha channel. 0 - 255
        void Set_Opacity(unsigned char Opacity);

        BORDER_STYLE_VALUE Get_Border_Style(){
            return Style->Border_Style;
        }

        // return int as 0 - 100
        int Get_Opacity(); 

        bool Is_Transparent();

        unsigned int Get_Processed_Width();
        unsigned int Get_Processed_Height();

        // Direction: Unsupported atm!!!
        void Show_Shadow(Vector2 Direction, RGB Shadow_Color, float Opacity = 1, float Length = 0.5);

        void Show_Shadow(RGB Shadow_Color, float Opacity = 1, float Length = 0.5);

        Element* Get_Parent(){
            return Parent;
        }

        void Set_Parent(Element* parent);

        bool Has(std::string s);

        bool Has(int s){
            for (auto i : Classes){
                if (i == s)
                    return true;
            }
            return false;
        }

        //returns the area which a new element could be fitted in.
        std::pair<unsigned int, unsigned int> Get_Fitting_Dimensions(Element* child);

        // returns the maximum area of width and height which an element could be fit in.
        // basically same as the Get_Fitting_Dimensions(), but with some extra safe checks, so use this.
        std::pair<unsigned int, unsigned int> Get_Limit_Dimensions();

        virtual void Show_Border(bool b);

        virtual void Show_Border(bool b, bool Previus_state);

        bool Has_Border();

        // NOTE: This will also HIDE ALL children in the AST beneath this element!!!
        void Display(bool f);

        bool Is_Displayed();

        virtual void Add_Child(Element* Child);

        virtual void Set_Childs(std::vector<Element*> childs);

        bool Children_Changed();
        
        bool Has_Transparent_Children();    

        virtual std::vector<Element*>& Get_Childs();

        virtual bool Remove(Element* handle);

        virtual bool Remove(unsigned int index);

        void Set_Dimensions(unsigned int width, unsigned int height);

        unsigned int Get_Width();

        unsigned int Get_Height();

        void Set_Width(unsigned int width);

        void Set_Height(unsigned int height);

        void Set_Position(Coordinates c);
       
        void Set_Position(Coordinates* c);

        Coordinates Get_Position();

        Coordinates Get_Absolute_Position();

        void Update_Absolute_Position_Cache();

        void Set_Margin(MARGIN_VALUE margin);

        MARGIN_VALUE Get_Margin();

        virtual void Set_Background_Color(RGB color);

        RGB Get_Background_Color();
        
        virtual void Set_Border_Color(RGB color);
        
        RGB Get_Border_Color();

        virtual void Set_Border_Background_Color(RGB color);
        
        RGB Get_Border_Background_Color();
        
        virtual void Set_Text_Color(RGB color);

        void Allow_Dynamic_Size(bool True);

        bool Is_Dynamic_Size_Allowed();

        // Allows by default hidden overflow, so that child elements can exceed the parent element dimension limits, whiteout resizing parent.  
        void Allow_Overflow(bool True);

        bool Is_Overflow_Allowed();
        
        RGB Get_Text_Color();

        static std::pair<std::pair<unsigned int, unsigned int> ,std::pair<std::pair<unsigned int, unsigned int>, std::pair<unsigned int, unsigned int>>> Get_Fitting_Area(GGUI::Element* Parent, GGUI::Element* Child);

        void Compute_Dynamic_Size();

        virtual std::vector<GGUI::UTF>& Render();

        // Used to update the parent when the child cannot update on itself, for an example on removal of an element.
        virtual void Update_Parent(Element* New_Element);

        virtual void Add_Overhead(Element* w, std::vector<UTF>& Result);

        virtual void Apply_Colors(Element* w, std::vector<UTF>& Result);

        virtual bool Resize_To([[maybe_unused]] Element* parent){
            return false;
        }

        void Compute_Alpha_To_Nesting(GGUI::UTF& Dest, GGUI::UTF Source);

        void Nest_Element(Element* Parent, Element* Child, std::vector<UTF>& Parent_Buffer, std::vector<UTF>& Child_Buffer);

        std::unordered_map<unsigned int, const char*> Get_Custom_Border_Map(Element* e);

        std::unordered_map<unsigned int, const char*> Get_Custom_Border_Map(GGUI::BORDER_STYLE_VALUE custom_border_style);

        void Set_Custom_Border_Style(GGUI::BORDER_STYLE_VALUE style);

        GGUI::BORDER_STYLE_VALUE Get_Custom_Border_Style();

        void Post_Process_Borders(Element* A, Element* B, std::vector<UTF>& Parent_Buffer);

        std::pair<RGB, RGB>  Compose_All_Text_RGB_Values();

        RGB  Compose_Text_RGB_Values();
        RGB  Compose_Background_RGB_Values();

        std::pair<RGB, RGB>  Compose_All_Border_RGB_Values();

        virtual std::string Get_Name() const {
            return "Element<" + Name + ">";
        }

        void Set_Name(std::string name);

        bool Has_Internal_Changes();

        //Makes suicide.
        void Remove();

        //Event handlers
        void On_Click(std::function<bool(GGUI::Event*)> action);

        void On(unsigned long long criteria, std::function<bool(GGUI::Event*)> action, bool GLOBAL = false);

        //This function returns nullptr, if the element could not be found.
        Element* Get_Element(std::string name);

        // TEMPLATES
        //-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_

        //This function returns all child elements that have the same element type.
        template<typename T>
        std::vector<T*> Get_Elements(){

            //go throgh the child AST, and check if the element in question is same type as the template T.
            std::vector<T*> result;

            if (typeid(*this) == typeid(T)){
                result.push_back((T*)this);
            }

            for (auto e : Childs){
                std::vector<T*> child_result = e->Get_Elements<T>();
                result.insert(result.end(), child_result.begin(), child_result.end());
            }

            return result;
        }

        std::vector<Element*> Get_All_Nested_Elements(bool Show_Hidden = false){
            std::vector<Element*> result;

            if (!Show && !Show_Hidden)
                return {};
            
            result.push_back(this);

            for (auto e : Childs){
                std::vector<Element*> child_result = e->Get_All_Nested_Elements(Show_Hidden);
                result.insert(result.end(), child_result.begin(), child_result.end());
            }

            return result;
        }

        //-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_

        // By default elements do not have inherent scrolling abilities.
        virtual void Scroll_Up() {}
        virtual void Scroll_Down() {}

        void Re_Order_Childs();

        void Focus();

        void On_State(State s, std::function<void()> job);

        bool Has_Postprocessing_To_Do();

        void Process_Shadow(std::vector<GGUI::UTF>& Current_Buffer);

        void Process_Opacity(std::vector<GGUI::UTF>& Current_Buffer);

        virtual std::vector<GGUI::UTF>& Postprocess();

        // Uses the post_processed widths and height values
        bool Child_Is_Shown(Element* other);
    };

    // UTILS : -_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_

    // Linear interpolation function
    template<typename T>
    constexpr T lerp(T a, T b, T t) {
        // Clamp t between a and b
        return a + t * (b - a);
    }

    constexpr GGUI::RGB Lerp(GGUI::RGB A, GGUI::RGB B, float Distance) {
        A.Red = lerp<float>(A.Red, B.Red, Distance);
        A.Green = lerp<float>(A.Green, B.Green, Distance);
        A.Blue = lerp<float>(A.Blue, B.Blue, Distance);

        return A;
    }

}

#endif
