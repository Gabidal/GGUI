#ifndef _UTF_H_
#define _UTF_H_

#include <string>
#include <variant>

#include "superString.h"
#include "color.h"
#include "types.h"

namespace GGUI{
    class UTF : public Compact_String {
    public:
        ENCODING_FLAG Flags = ENCODING_FLAG::NONE;

        RGB Foreground;
        RGB Background;

        constexpr UTF() {}

        /**
         * @brief Copy constructor for the UTF class.
         *
         * This constructor initializes a new UTF object as a copy of another UTF object.
         *
         * @param other The UTF object to copy.
         */
        constexpr UTF(const GGUI::UTF& other)
            : Compact_String(other),
              Foreground(other.Foreground),
              Background(other.Background) {}

        /**
         * @brief Constructs a new UTF object from a single character and a pair of foreground and background colors.
         * @param data The character to store in the UTF object.
         * @param color A pair of RGB objects representing the foreground and background colors. If not provided, defaults to {{}, {}}.
         */
        constexpr UTF(const char data, const std::pair<RGB, RGB> color = {{}, {}}) : Compact_String(data) {
            Foreground = {color.first};
            Background = {color.second};
        }

        /**
         * @brief Constructs a new UTF object from a C-style string and a pair of foreground and background colors.
         * @param data The C-style string to store in the UTF object.
         * @param color A pair of RGB objects representing the foreground and background colors. If not provided, defaults to {{}, {}}.
         */
        constexpr UTF(const char* data, const std::pair<RGB, RGB> color = {{}, {}}) : Compact_String(data) {
            Foreground = {color.first};
            Background = {color.second};
        }

        /**
         * @brief Constructs a new UTF object from a Compact_String and a pair of foreground and background colors.
         * @param CS The Compact_String to store in the UTF object.
         * @param color A pair of RGB objects representing the foreground and background colors. If not provided, defaults to {{}, {}}.
         */
        constexpr UTF(const Compact_String CS, const std::pair<RGB, RGB> color = {{}, {}}) : Compact_String(CS) {
            Foreground = {color.first};
            Background = {color.second};
        }

        /**
         * @brief Checks if a specific UTF flag is set.
         * @param utf_flag The UTF flag to check.
         * @return True if the flag is set, otherwise false.
         */
        constexpr bool Is(const ENCODING_FLAG flag) const {
            return (Flags & flag) == flag;
        }

        /**
         * @brief Checks if a specific UTF flag is set.
         * @param cs_flag The UTF flag to check.
         * @return True if the flag is set, otherwise false.
         */
        constexpr bool Is(unsigned char cs_flag) const {
            return Compact_String::Is(cs_flag);
        }

        // Fast comparison of type and content
        constexpr bool Is(const char* other) const {
            return Compact_String::Is(other);
        }

        // Fast comparison of type and content
        constexpr bool Is(char other) const {
            return Compact_String::Is(other);
        }

        /**
         * @brief Sets a specific UTF flag.
         * @param utf_flag The UTF flag to set.
         */
        constexpr void Set_Flag(const ENCODING_FLAG flag) {
            Flags |= flag;
        }

        /**
         * @brief Sets the foreground color of the UTF element.
         * @param color The RGB color to set as the foreground color.
         */
        constexpr void Set_Foreground(const RGB color) {
            Foreground = color;
        }

        /**
         * @brief Sets the background color of the UTF element.
         * @param color The RGB color to set as the background color.
         */
        constexpr void Set_Background(const RGB color) {
            Background = color;
        }

        /**
         * @brief Sets the foreground and background color of the UTF element.
         * @param primals A pair of RGB colors. The first element is the foreground color; the second is the background color.
         */
        constexpr void Set_Color(const std::pair<RGB, RGB>& primals) {
            Foreground = primals.first;
            Background = primals.second;
        }

        /**
         * @brief Sets the text of the UTF element.
         * @param data The std::string to set as the text.
         */
        // void Set_Text(const std::string& data) {
        //     Compact_String::Set_Unicode(data.c_str());
        // }

        /**
         * @brief Sets the text of the UTF element to a single character.
         * @param data The character to set as the text.
         */
        constexpr void Set_Text(const char data) {
            Compact_String::Set_Ascii(data);
        }

        /**
         * @brief Sets the text of the UTF element to a null-terminated string.
         * @param data The null-terminated string to set as the text.
         */
        constexpr void Set_Text(const char* data) {
            Compact_String::Set_Unicode(data);
        }

        /**
         * @brief Sets the text of the UTF element to that of another UTF element.
         * @param other The other UTF element to copy the text from.
         */
        constexpr void Set_Text(const UTF& other) {
            Compact_String::operator=(other);
        }

        /**
         * @brief Converts the UTF character to a Super_String.
         * @param Result The result string.
         * @param Text_Overhead The foreground colour and style as a string.
         * @param Background_Overhead The background colour and style as a string.
         * @param Text_Colour The foreground colour as a string.
         * @param Background_Colour The background colour as a string.
         */
        void To_Super_String(
            Super_String<GGUI::Constants::ANSI::Maximum_Needed_Pre_Allocation_For_Encoded_Super_String>* Result,
            Super_String<GGUI::Constants::ANSI::Maximum_Needed_Pre_Allocation_For_Over_Head>* Text_Overhead = new Super_String<GGUI::Constants::ANSI::Maximum_Needed_Pre_Allocation_For_Over_Head>(),
            Super_String<GGUI::Constants::ANSI::Maximum_Needed_Pre_Allocation_For_Over_Head>* Background_Overhead = new Super_String<GGUI::Constants::ANSI::Maximum_Needed_Pre_Allocation_For_Over_Head>(),
            Super_String<GGUI::Constants::ANSI::Maximum_Needed_Pre_Allocation_For_Color>* Text_Colour = new Super_String<GGUI::Constants::ANSI::Maximum_Needed_Pre_Allocation_For_Color>(),
            Super_String<GGUI::Constants::ANSI::Maximum_Needed_Pre_Allocation_For_Color>* Background_Colour = new Super_String<GGUI::Constants::ANSI::Maximum_Needed_Pre_Allocation_For_Color>()
        ) const;

        Super_String<GGUI::Constants::ANSI::Maximum_Needed_Pre_Allocation_For_Encoded_Super_String>* To_Super_String() const {
            Super_String<GGUI::Constants::ANSI::Maximum_Needed_Pre_Allocation_For_Encoded_Super_String>* Result = new Super_String<GGUI::Constants::ANSI::Maximum_Needed_Pre_Allocation_For_Encoded_Super_String>();

            To_Super_String(Result);

            return Result;
        }

        /**
         * @brief Converts the UTF character to an encoded Super_String.
         * @param Result The Super_String to which the encoded string will be added.
         * @param Text_Overhead The Super_String where the foreground colour overhead will be stored.
         * @param Background_Overhead The Super_String where the background colour overhead will be stored.
         * @param Text_Colour The Super_String where the foreground colour will be stored.
         * @param Background_Colour The Super_String where the background colour will be stored.
         */
        void To_Encoded_Super_String(
            Super_String<GGUI::Constants::ANSI::Maximum_Needed_Pre_Allocation_For_Encoded_Super_String>* Result,
            Super_String<GGUI::Constants::ANSI::Maximum_Needed_Pre_Allocation_For_Over_Head>* Text_Overhead,
            Super_String<GGUI::Constants::ANSI::Maximum_Needed_Pre_Allocation_For_Over_Head>* Background_Overhead,
            Super_String<GGUI::Constants::ANSI::Maximum_Needed_Pre_Allocation_For_Color>* Text_Colour,
            Super_String<GGUI::Constants::ANSI::Maximum_Needed_Pre_Allocation_For_Color>* Background_Colour
        ) const;

        /**
         * @brief Assign a character to the UTF object.
         * @param text The character to assign.
         */
        constexpr void operator=(const char text) {
            Set_Text(text);
        }

        constexpr void operator=(const char* text) {
            Set_Text(text);
        }

        /**
         * @brief Assigns a UTF object to another UTF object.
         * @param other The UTF object to assign.
         * @return The assigned UTF object.
         */
        constexpr UTF& operator=(const UTF& other) {
            Compact_String::operator=(other);
            Foreground = other.Foreground;
            Background = other.Background;
            return *this;
        }
    };

    namespace SYMBOLS{
        constexpr GGUI::UTF EMPTY_UTF(' ', {GGUI::COLOR::WHITE, GGUI::COLOR::BLACK});
    }
}

#endif