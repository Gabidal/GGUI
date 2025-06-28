#ifndef _COLOR_H_
#define _COLOR_H_

#include <string>
#include <limits>
#include <math.h>

#include "superString.h"
#include "constants.h"

namespace GGUI{
    class RGB{
    public:
        unsigned char Red = 0;
        unsigned char Green = 0;
        unsigned char Blue = 0;

        constexpr RGB(unsigned char r, unsigned char g, unsigned char b) : Red(r), Green(g), Blue(b) {}

        constexpr RGB() = default;

        /**
         * @brief Converts the RGB colour to a string.
         *
         * @param Result The result string.
         */
        constexpr void Get_Colour_As_Super_String(Super_String<Constants::ANSI::Maximum_Needed_Pre_Allocation_For_Color>* Result) const {
            // Add the red value to the string
            Result->Add(Constants::To_Compact[Red]);
            
            // Add the separator to the string
            Result->Add(Constants::ANSI::SEPARATE);
            
            // Add the green value to the string
            Result->Add(Constants::To_Compact[Green]);
            
            // Add the separator to the string
            Result->Add(Constants::ANSI::SEPARATE);
            
            // Add the blue value to the string
            Result->Add(Constants::To_Compact[Blue]);
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
        constexpr void Get_Over_Head_As_Super_String(Super_String<GGUI::Constants::ANSI::Maximum_Needed_Pre_Allocation_For_Over_Head>* Result, const bool Is_Text_Color = true) const {
            Result->Add(Constants::ANSI::ESC_CODE);
            
            Is_Text_Color ? 
                Result->Add(Constants::ANSI::TEXT_COLOR) :
                Result->Add(Constants::ANSI::BACKGROUND_COLOR);

            Result->Add(Constants::ANSI::SEPARATE);
            Result->Add(Constants::ANSI::USE_RGB);
            Result->Add(Constants::ANSI::SEPARATE);
        }
    
        constexpr bool operator==(const RGB& Other) const{
            // only take the bits from the first 3 unsigned chars
            //return (*(unsigned int*)this & 0xFFFFFF) == (*(unsigned int*)&Other & 0xFFFFFF);
            return Red == Other.Red && Green == Other.Green && Blue == Other.Blue;
        }

        constexpr bool operator!=(const RGB& Other) const{
            return !(*this == Other);
        }

        constexpr RGB operator+(const RGB& Other) const{
            return RGB(Red + Other.Red, Green + Other.Green, Blue + Other.Blue);
        }

        constexpr RGB operator*(const float Scalar) const{
            return RGB((unsigned char)((float)Red * Scalar), (unsigned char)((float)Green * Scalar), (unsigned char)((float)Blue * Scalar));
        }

        constexpr RGB operator!() const{
            return RGB(UINT8_MAX - Red, UINT8_MAX - Green, UINT8_MAX - Blue);
        }

        constexpr void Add(const RGB& other, float opacity){
            // Calculate the reverse alpha
            float Reverse_Alpha = 1.0f - opacity;

            // Blend the colors based on the opacity
            Red = (unsigned char)((float)Red * Reverse_Alpha + (float)other.Red * opacity);
            Green = (unsigned char)((float)Green * Reverse_Alpha + (float)other.Green * opacity);
            Blue = (unsigned char)((float)Blue * Reverse_Alpha + (float)other.Blue * opacity);
        }
    };

    namespace COLOR{
        static constexpr RGB WHITE = RGB(255, 255, 255);
        static constexpr RGB BLACK = RGB(0, 0, 0);
        static constexpr RGB RED = RGB(255, 0, 0);
        static constexpr RGB GREEN = RGB(0, 255, 0);
        static constexpr RGB BLUE = RGB(0, 0, 255);
        static constexpr RGB YELLOW = RGB(255, 255, 0);
        static constexpr RGB ORANGE = RGB(255, 128, 0);
        static constexpr RGB CYAN = RGB(0, 255, 255);
        static constexpr RGB TEAL = RGB(0, 128, 128);
        static constexpr RGB MAGENTA = RGB(255, 0, 255);
        static constexpr RGB GRAY = RGB(128, 128, 128);
        static constexpr RGB LIGHT_RED = RGB(255, 128, 128);
        static constexpr RGB LIGHT_GREEN = RGB(128, 255, 128);
        static constexpr RGB LIGHT_BLUE = RGB(128, 128, 255);
        static constexpr RGB LIGHT_YELLOW = RGB(255, 255, 128);
        static constexpr RGB LIGHT_CYAN = RGB(128, 255, 255);
        static constexpr RGB LIGHT_MAGENTA = RGB(255, 128, 255);
        static constexpr RGB LIGHT_GRAY = RGB(192, 192, 192);
        static constexpr RGB DARK_RED = RGB(128, 0, 0);
        static constexpr RGB DARK_GREEN = RGB(0, 128, 0);
        static constexpr RGB DARK_BLUE = RGB(0, 0, 128);
        static constexpr RGB DARK_YELLOW = RGB(128, 128, 0);
        static constexpr RGB DARK_CYAN = RGB(0, 128, 128);
        static constexpr RGB DARK_MAGENTA = RGB(128, 0, 128);
        static constexpr RGB DARK_GRAY = RGB(64, 64, 64);
    }

}

#endif