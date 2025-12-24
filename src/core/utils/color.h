#ifndef _COLOR_H_
#define _COLOR_H_

#include <string>
#include <limits>
#include <math.h>
#include <cstdint>
#include <ostream>

#include "superString.h"
#include "constants.h"

namespace GGUI{
    class RGB{
    public:
        unsigned char red = 0;
        unsigned char green = 0;
        unsigned char blue = 0;

        constexpr RGB(unsigned char r, unsigned char g, unsigned char b) : red(r), green(g), blue(b) {}

        constexpr RGB() = default;

        /**
         * @brief Converts the RGB colour to a string.
         *
         * @param Result The result string.
         */
        // constexpr void getColourAsSuperString(INTERNAL::superString<constants::ANSI::maximumNeededPreAllocationForColor>* Result) const {
        constexpr void getColourAsSuperString(INTERNAL::superString<constants::ANSI::maximumNeededPreAllocationForEncodedSuperString>* Result) const {
            // Add the red value to the string
            Result->add(constants::ANSI::toCompactTable[red]);
            
            // Add the separator to the string
            Result->add(constants::ANSI::SEPARATE);
            
            // Add the green value to the string
            Result->add(constants::ANSI::toCompactTable[green]);
            
            // Add the separator to the string
            Result->add(constants::ANSI::SEPARATE);
            
            // Add the blue value to the string
            Result->add(constants::ANSI::toCompactTable[blue]);
        }
    
        /**
         * @brief Populates a Super_String with ANSI escape codes for text or background color.
         * 
         * This function adds the escape codes for either text or background color to the provided Super_String.
         * Ensure that the Result has been initialized with at least Maximum_Needed_Pre_Allocation_For_Over_Head.
         * 
         * @param Result A pointer to the Super_String to populate with escape codes.
         * @param Is_Text_Color A boolean to determine if the codes are for text color (true) or background color (false).
         */
        constexpr void getOverHeadAsSuperString(INTERNAL::superString<GGUI::constants::ANSI::maximumNeededPreAllocationForOverHead>* Result, const bool Is_Text_Color = true) const {
            Result->add(constants::ANSI::ESC_CODE);
            
            Is_Text_Color ? 
                Result->add(constants::ANSI::TEXT_COLOR) :
                Result->add(constants::ANSI::BACKGROUND_COLOR);

            Result->add(constants::ANSI::SEPARATE);
            Result->add(constants::ANSI::USE_RGB);
            Result->add(constants::ANSI::SEPARATE);
        }

        constexpr INTERNAL::superString<GGUI::constants::ANSI::maximumNeededPreAllocationForOverHead> getOverHeadAsSuperString(const bool Is_Text_Color = true) const {
            INTERNAL::superString<GGUI::constants::ANSI::maximumNeededPreAllocationForOverHead> Result;

            Result.add(constants::ANSI::ESC_CODE);
            
            Is_Text_Color ? 
                Result.add(constants::ANSI::TEXT_COLOR) :
                Result.add(constants::ANSI::BACKGROUND_COLOR);

            Result.add(constants::ANSI::SEPARATE);
            Result.add(constants::ANSI::USE_RGB);
            Result.add(constants::ANSI::SEPARATE);

            return Result;
        }
    
        constexpr bool operator==(const RGB& Other) const{
            // only take the bits from the first 3 unsigned chars
            return red == Other.red && green == Other.green && blue == Other.blue;
        }

        constexpr bool operator!=(const RGB& Other) const{
            return !(*this == Other);
        }

        constexpr RGB operator+(const RGB& Other) const{
            return RGB(red + Other.red, green + Other.green, blue + Other.blue);
        }

        constexpr RGB operator*(const float Scalar) const{
            return RGB((unsigned char)((float)red * Scalar), (unsigned char)((float)green * Scalar), (unsigned char)((float)blue * Scalar));
        }

        constexpr RGB operator!() const{
            return RGB(UINT8_MAX - red, UINT8_MAX - green, UINT8_MAX - blue);
        }

        // Integer-only alpha blend: opacity in [0..255] where 0 = transparent, 255 = fully opaque.
        // Uses /256 (shift) for speed, with a special-case that maps 255 -> 256 so full opacity is exact.
        constexpr void add(const RGB& other, unsigned char opacity){
            if (opacity == 0) return;
            if (opacity == UINT8_MAX){
                red = other.red;
                green = other.green;
                blue = other.blue;
                return;
            }

            red = computeAlpha(red, other.red, opacity);
            green = computeAlpha(green, other.green, opacity);
            blue = computeAlpha(blue, other.blue, opacity);
        }

    private:
        constexpr inline unsigned char computeAlpha(unsigned char A, unsigned char B, unsigned char opacity) {
            constexpr unsigned int UINT8_MAX_AS_256 = UINT8_MAX + 1;            // 256
            constexpr unsigned int UINT8_HALF = UINT8_MAX_AS_256 / 2;           // 128

            #ifndef UINT8_WIDTH // For some compilers in windows side do not have cstdint::UINT8_WIDTH defined.
            constexpr unsigned int UINT8_WIDTH = 8;
            #endif

            const unsigned int alpha = (unsigned int)opacity;                   // 1..254
            const unsigned int inv = UINT8_MAX_AS_256 - alpha;
            return (unsigned char)(((unsigned int)A * inv + (unsigned int)B * alpha + UINT8_HALF) >> UINT8_WIDTH);
        }
    };

    namespace COLOR{
        // Basic colors
        static constexpr RGB WHITE = RGB(255, 255, 255);
        static constexpr RGB BLACK = RGB(0, 0, 0);
        static constexpr RGB RED = RGB(255, 0, 0);
        static constexpr RGB GREEN = RGB(0, 255, 0);
        static constexpr RGB BLUE = RGB(0, 0, 255);
        static constexpr RGB YELLOW = RGB(255, 255, 0);
        static constexpr RGB ORANGE = RGB(255, 128, 0);
        static constexpr RGB CYAN = RGB(0, 255, 255);
        static constexpr RGB MAGENTA = RGB(255, 0, 255);
        
        // Gray scale variations
        static constexpr RGB GRAY = RGB(128, 128, 128);
        static constexpr RGB LIGHT_GRAY = RGB(211, 211, 211);
        static constexpr RGB DARK_GRAY = RGB(64, 64, 64);
        static constexpr RGB SILVER = RGB(192, 192, 192);
        static constexpr RGB DIM_GRAY = RGB(105, 105, 105);
        static constexpr RGB SLATE_GRAY = RGB(112, 128, 144);
        static constexpr RGB LIGHT_SLATE_GRAY = RGB(119, 136, 153);
        static constexpr RGB DARK_SLATE_GRAY = RGB(47, 79, 79);
        
        // Light color variations
        static constexpr RGB LIGHT_RED = RGB(255, 128, 128);
        static constexpr RGB LIGHT_GREEN = RGB(128, 255, 128);
        static constexpr RGB LIGHT_BLUE = RGB(173, 216, 230);
        static constexpr RGB LIGHT_YELLOW = RGB(255, 255, 128);
        static constexpr RGB LIGHT_CYAN = RGB(128, 255, 255);
        static constexpr RGB LIGHT_MAGENTA = RGB(255, 128, 255);
        
        // Dark color variations
        static constexpr RGB DARK_RED = RGB(128, 0, 0);
        static constexpr RGB DARK_GREEN = RGB(0, 128, 0);
        static constexpr RGB DARK_BLUE = RGB(0, 0, 128);
        static constexpr RGB DARK_YELLOW = RGB(128, 128, 0);
        static constexpr RGB DARK_CYAN = RGB(0, 128, 128);
        static constexpr RGB DARK_MAGENTA = RGB(128, 0, 128);
        
        // Extended basic colors
        static constexpr RGB TEAL = DARK_CYAN;
        static constexpr RGB NAVY = DARK_BLUE;
        static constexpr RGB MAROON = DARK_RED;
        static constexpr RGB OLIVE = DARK_YELLOW;
        static constexpr RGB LIME = GREEN;
        static constexpr RGB AQUA = CYAN;
        static constexpr RGB FUCHSIA = MAGENTA;
        
        // Purple shades
        static constexpr RGB PURPLE = DARK_MAGENTA;
        static constexpr RGB VIOLET = RGB(238, 130, 238);
        static constexpr RGB INDIGO = RGB(75, 0, 130);
        static constexpr RGB PLUM = RGB(221, 160, 221);
        static constexpr RGB ORCHID = RGB(218, 112, 214);
        static constexpr RGB MEDIUM_PURPLE = RGB(147, 112, 219);
        static constexpr RGB DARK_VIOLET = RGB(148, 0, 211);
        static constexpr RGB BLUE_VIOLET = RGB(138, 43, 226);
        
        // Pink shades
        static constexpr RGB PINK = RGB(255, 192, 203);
        static constexpr RGB LIGHT_PINK = RGB(255, 182, 193);
        static constexpr RGB HOT_PINK = RGB(255, 105, 180);
        static constexpr RGB DEEP_PINK = RGB(255, 20, 147);
        static constexpr RGB MEDIUM_VIOLET_RED = RGB(199, 21, 133);
        static constexpr RGB PALE_VIOLET_RED = RGB(219, 112, 147);
        
        // Orange shades
        static constexpr RGB ORANGE_RED = RGB(255, 69, 0);
        static constexpr RGB DARK_ORANGE = RGB(255, 140, 0);
        static constexpr RGB CORAL = RGB(255, 127, 80);
        static constexpr RGB LIGHT_SALMON = RGB(255, 160, 122);
        static constexpr RGB SALMON = RGB(250, 128, 114);
        static constexpr RGB DARK_SALMON = RGB(233, 150, 122);
        static constexpr RGB TOMATO = RGB(255, 99, 71);
        
        // Brown shades
        static constexpr RGB BROWN = RGB(165, 42, 42);
        static constexpr RGB SADDLE_BROWN = RGB(139, 69, 19);
        static constexpr RGB SIENNA = RGB(160, 82, 45);
        static constexpr RGB CHOCOLATE = RGB(210, 105, 30);
        static constexpr RGB DARK_GOLDENROD = RGB(184, 134, 11);
        static constexpr RGB PERU = RGB(205, 133, 63);
        static constexpr RGB ROSY_BROWN = RGB(188, 143, 143);
        static constexpr RGB GOLDENROD = RGB(218, 165, 32);
        static constexpr RGB SANDY_BROWN = RGB(244, 164, 96);
        static constexpr RGB TAN = RGB(210, 180, 140);
        static constexpr RGB BURLYWOOD = RGB(222, 184, 135);
        static constexpr RGB WHEAT = RGB(245, 222, 179);
        
        // Green shades
        static constexpr RGB FOREST_GREEN = RGB(34, 139, 34);
        static constexpr RGB SEA_GREEN = RGB(46, 139, 87);
        static constexpr RGB MEDIUM_SEA_GREEN = RGB(60, 179, 113);
        static constexpr RGB SPRING_GREEN = RGB(0, 255, 127);
        static constexpr RGB MEDIUM_SPRING_GREEN = RGB(0, 250, 154);
        static constexpr RGB DARK_SEA_GREEN = RGB(143, 188, 143);
        static constexpr RGB LIGHT_SEA_GREEN = RGB(32, 178, 170);
        static constexpr RGB PALE_GREEN = RGB(152, 251, 152);
        static constexpr RGB LAWN_GREEN = RGB(124, 252, 0);
        static constexpr RGB CHARTREUSE = RGB(127, 255, 0);
        static constexpr RGB GREEN_YELLOW = RGB(173, 255, 47);
        static constexpr RGB YELLOW_GREEN = RGB(154, 205, 50);
        static constexpr RGB OLIVE_DRAB = RGB(107, 142, 35);
        
        // Blue shades
        static constexpr RGB LIGHT_STEEL_BLUE = RGB(176, 196, 222);
        static constexpr RGB POWDER_BLUE = RGB(176, 224, 230);
        static constexpr RGB SKY_BLUE = RGB(135, 206, 235);
        static constexpr RGB LIGHT_SKY_BLUE = RGB(135, 206, 250);
        static constexpr RGB DEEP_SKY_BLUE = RGB(0, 191, 255);
        static constexpr RGB DODGER_BLUE = RGB(30, 144, 255);
        static constexpr RGB CORNFLOWER_BLUE = RGB(100, 149, 237);
        static constexpr RGB STEEL_BLUE = RGB(70, 130, 180);
        static constexpr RGB ROYAL_BLUE = RGB(65, 105, 225);
        static constexpr RGB MEDIUM_BLUE = RGB(0, 0, 205);
        static constexpr RGB MIDNIGHT_BLUE = RGB(25, 25, 112);
        
        // Cyan/Turquoise shades
        static constexpr RGB DARK_TURQUOISE = RGB(0, 206, 209);
        static constexpr RGB MEDIUM_TURQUOISE = RGB(72, 209, 204);
        static constexpr RGB TURQUOISE = RGB(64, 224, 208);
        static constexpr RGB PALE_TURQUOISE = RGB(175, 238, 238);
        static constexpr RGB AQUAMARINE = RGB(127, 255, 212);
        static constexpr RGB MEDIUM_AQUAMARINE = RGB(102, 205, 170);
        
        // Material Design inspired colors
        namespace MATERIAL {
            static constexpr RGB RED = RGB(244, 67, 54);
            static constexpr RGB PINK = RGB(233, 30, 99);
            static constexpr RGB PURPLE = RGB(156, 39, 176);
            static constexpr RGB DEEP_PURPLE = RGB(103, 58, 183);
            static constexpr RGB INDIGO = RGB(63, 81, 181);
            static constexpr RGB BLUE = RGB(33, 150, 243);
            static constexpr RGB LIGHT_BLUE = RGB(3, 169, 244);
            static constexpr RGB CYAN = RGB(0, 188, 212);
            static constexpr RGB TEAL = RGB(0, 150, 136);
            static constexpr RGB GREEN = RGB(76, 175, 80);
            static constexpr RGB LIGHT_GREEN = RGB(139, 195, 74);
            static constexpr RGB LIME = RGB(205, 220, 57);
            static constexpr RGB YELLOW = RGB(255, 235, 59);
            static constexpr RGB AMBER = RGB(255, 193, 7);
            static constexpr RGB ORANGE = RGB(255, 152, 0);
            static constexpr RGB DEEP_ORANGE = RGB(255, 87, 34);
            static constexpr RGB BROWN = RGB(121, 85, 72);
            static constexpr RGB GRAY = RGB(158, 158, 158);
            static constexpr RGB BLUE_GRAY = RGB(96, 125, 139);
        }
        
        // UI/Theme colors
        static constexpr RGB SUCCESS = RGB(40, 167, 69);
        static constexpr RGB WARNING = RGB(255, 193, 7);
        static constexpr RGB DANGER = RGB(220, 53, 69);
        static constexpr RGB INFO = RGB(23, 162, 184);
        static constexpr RGB PRIMARY = RGB(0, 123, 255);
        static constexpr RGB SECONDARY = RGB(108, 117, 125);
        
        // Vintage/Retro colors
        static constexpr RGB ANTIQUE_WHITE = RGB(250, 235, 215);
        static constexpr RGB BEIGE = RGB(245, 245, 220);
        static constexpr RGB BISQUE = RGB(255, 228, 196);
        static constexpr RGB BLANCHED_ALMOND = RGB(255, 235, 205);
        static constexpr RGB CORNSILK = RGB(255, 248, 220);
        static constexpr RGB EGGSHELL = RGB(240, 234, 214);
        static constexpr RGB FLORAL_WHITE = RGB(255, 250, 240);
        static constexpr RGB GAINSBORO = RGB(220, 220, 220);
        static constexpr RGB GHOST_WHITE = RGB(248, 248, 255);
        static constexpr RGB HONEYDEW = RGB(240, 255, 240);
        static constexpr RGB IVORY = RGB(255, 255, 240);
        static constexpr RGB LAVENDER = RGB(230, 230, 250);
        static constexpr RGB LAVENDER_BLUSH = RGB(255, 240, 245);
        static constexpr RGB LEMON_CHIFFON = RGB(255, 250, 205);
        static constexpr RGB LINEN = RGB(250, 240, 230);
        static constexpr RGB MINT_CREAM = RGB(245, 255, 250);
        static constexpr RGB MISTY_ROSE = RGB(255, 228, 225);
        static constexpr RGB MOCCASIN = RGB(255, 228, 181);
        static constexpr RGB NAVAJO_WHITE = RGB(255, 222, 173);
        static constexpr RGB OLD_LACE = RGB(253, 245, 230);
        static constexpr RGB PAPAYA_WHIP = RGB(255, 239, 213);
        static constexpr RGB PEACH_PUFF = RGB(255, 218, 185);
        static constexpr RGB SEASHELL = RGB(255, 245, 238);
        static constexpr RGB SNOW = RGB(255, 250, 250);
        static constexpr RGB THISTLE = RGB(216, 191, 216);
        static constexpr RGB WHITE_SMOKE = RGB(245, 245, 245);
    }

    inline std::ostream& operator<<(std::ostream& os, const GGUI::RGB& color) {
        os << "RGB(" << static_cast<int>(color.red) << ", "
           << static_cast<int>(color.green) << ", "
           << static_cast<int>(color.blue) << ")";
        return os;
    }
}

#endif