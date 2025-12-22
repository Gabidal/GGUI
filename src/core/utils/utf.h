#ifndef _UTF_H_
#define _UTF_H_

#include <string>
#include <variant>
#include <utility>
#include <cstdint>

#include "superString.h"
#include "color.h"
#include "types.h"
#include "constants.h"

namespace GGUI{
    class UTF : public INTERNAL::compactString {
    public:
        INTERNAL::ENCODING_FLAG flags = INTERNAL::ENCODING_FLAG::NONE;

        RGB foreground;
        RGB background;

        constexpr UTF() {}

        /**
         * @brief Copy constructor for the UTF class.
         *
         * This constructor initializes a new UTF object as a copy of another UTF object.
         *
         * @param other The UTF object to copy.
         */
        constexpr UTF(const GGUI::UTF& other)
            : INTERNAL::compactString(other),
              foreground(other.foreground),
              background(other.background) {}

        /**
         * @brief Constructs a new UTF object from a single character and a pair of foreground and background colors.
         * @param data The character to store in the UTF object.
         * @param color A pair of RGB objects representing the foreground and background colors. If not provided, defaults to {{}, {}}.
         */
        constexpr UTF(const char data, const std::pair<RGB, RGB> color = {{}, {}}) : compactString(data) {
            foreground = {color.first};
            background = {color.second};
        }

        /**
         * @brief Constructs a new UTF object from a C-style string and a pair of foreground and background colors.
         * @param data The C-style string to store in the UTF object.
         * @param color A pair of RGB objects representing the foreground and background colors. If not provided, defaults to {{}, {}}.
         */
        constexpr UTF(const char* data, const std::pair<RGB, RGB> color = {{}, {}}) : compactString(data) {
            foreground = {color.first};
            background = {color.second};
        }

        /**
         * @brief Constructs a new UTF object from a Compact_String and a pair of foreground and background colors.
         * @param CS The Compact_String to store in the UTF object.
         * @param color A pair of RGB objects representing the foreground and background colors. If not provided, defaults to {{}, {}}.
         */
        constexpr UTF(const compactString CS, const std::pair<RGB, RGB> color = {{}, {}}) : compactString(CS) {
            foreground = {color.first};
            background = {color.second};
        }

        /**
         * @brief Checks if a specific UTF flag is set.
         * @param utf_flag The UTF flag to check.
         * @return True if the flag is set, otherwise false.
         */
        constexpr bool is(const INTERNAL::ENCODING_FLAG flag) const {
            return (flags & flag) == flag;
        }

        /**
         * @brief Checks if a specific UTF flag is set.
         * @param cs_flag The UTF flag to check.
         * @return True if the flag is set, otherwise false.
         */
        constexpr bool is(unsigned char cs_flag) const {
            return compactString::is(cs_flag);
        }

        // Fast comparison of type and content
        constexpr bool is(const char* other) const {
            return compactString::is(other);
        }

        // Fast comparison of type and content
        constexpr bool is(char other) const {
            return compactString::is(other);
        }

        /**
         * @brief Sets a specific UTF flag.
         * @param utf_flag The UTF flag to set.
         */
        constexpr void setFlag(const INTERNAL::ENCODING_FLAG flag) {
            flags |= flag;
        }

        /**
         * @brief Sets the foreground color of the UTF element.
         * @param color The RGB color to set as the foreground color.
         */
        constexpr void setForeground(const RGB color) {
            foreground = color;
        }

        /**
         * @brief Sets the background color of the UTF element.
         * @param color The RGB color to set as the background color.
         */
        constexpr void setBackground(const RGB color) {
            background = color;
        }

        /**
         * @brief Sets the foreground and background color of the UTF element.
         * @param primals A pair of RGB colors. The first element is the foreground color; the second is the background color.
         */
        constexpr void setColor(const std::pair<RGB, RGB>& primals) {
            foreground = primals.first;
            background = primals.second;
        }

        /**
         * @brief Sets the text of the UTF element to a single character.
         * @param data The character to set as the text.
         */
        constexpr void setText(const char data) {
            compactString::setAscii(data);
        }

        /**
         * @brief Sets the text of the UTF element to a null-terminated string.
         * @param data The null-terminated string to set as the text.
         */
        constexpr void setText(const char* data) {
            compactString::setUnicode(data);
        }

        /**
         * @brief Sets the text of the UTF element to that of another UTF element.
         * @param other The other UTF element to copy the text from.
         */
        constexpr void setText(const UTF& other) {
            compactString::operator=(other);
        }

        // autoGen: Ignore start

        /**
         * @brief Converts the UTF character to a Super_String.
         * @param Result The result string.
         * @param Text_Overhead The foreground colour and style as a string.
         * @param Background_Overhead The background colour and style as a string.
         * @param Text_Colour The foreground colour as a string.
         * @param Background_Colour The background colour as a string.
         */
        void toSuperString(
            INTERNAL::superString<GGUI::constants::ANSI::maximumNeededPreAllocationForEncodedSuperString>* Result,
            bool TransparentBackground = false,
            INTERNAL::superString<GGUI::constants::ANSI::maximumNeededPreAllocationForColor>* TextColour = new INTERNAL::superString<GGUI::constants::ANSI::maximumNeededPreAllocationForColor>(),
            INTERNAL::superString<GGUI::constants::ANSI::maximumNeededPreAllocationForColor>* BackgroundColour = new INTERNAL::superString<GGUI::constants::ANSI::maximumNeededPreAllocationForColor>()
        ) const;

        INTERNAL::superString<GGUI::constants::ANSI::maximumNeededPreAllocationForEncodedSuperString>* toSuperString(bool TransparentBackground = false) const {
            INTERNAL::superString<GGUI::constants::ANSI::maximumNeededPreAllocationForEncodedSuperString>* Result = new INTERNAL::superString<GGUI::constants::ANSI::maximumNeededPreAllocationForEncodedSuperString>();

            toSuperString(Result, TransparentBackground);

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
        void toEncodedSuperString(
            INTERNAL::superString<GGUI::constants::ANSI::maximumNeededPreAllocationForEncodedSuperString>* Result,
            INTERNAL::superString<GGUI::constants::ANSI::maximumNeededPreAllocationForColor>* Text_Colour,
            INTERNAL::superString<GGUI::constants::ANSI::maximumNeededPreAllocationForColor>* Background_Colour
        ) const;

        // autoGen: Ignore end

        /**
         * @brief Assign a character to the UTF object.
         * @param text The character to assign.
         */
        constexpr void operator=(const char Text) {
            setText(Text);
        }

        constexpr void operator=(const char* Text) {
            setText(Text);
        }

        /**
         * @brief Assigns a UTF object to another UTF object.
         * @param other The UTF object to assign.
         * @return The assigned UTF object.
         */
        constexpr UTF& operator=(const UTF& other) {
            compactString::operator=(other);
            foreground = other.foreground;
            background = other.background;
            return *this;
        }
    };

    namespace SYMBOLS{
        inline const UTF EMPTY_UTF(' ', {COLOR::WHITE, COLOR::BLACK});
    }

    // autoGen: Ignore start
    namespace INTERNAL {
        // Precompute and store the two overhead strings at startup.
        constexpr const INTERNAL::superString<GGUI::constants::ANSI::maximumNeededPreAllocationForOverHead> makeOverhead(bool isText) {
            // Use RGB::getOverHeadAsSuperString path with a dummy color to build the header consistently.
            // Header content does not depend on the actual RGB values, only on isText flag.
            RGB dummy(0,0,0);
            return dummy.getOverHeadAsSuperString(isText);
        }

        static const INTERNAL::superString<GGUI::constants::ANSI::maximumNeededPreAllocationForOverHead> textOverheadPrecompute  = makeOverhead(true);
        static const  INTERNAL::superString<GGUI::constants::ANSI::maximumNeededPreAllocationForOverHead> backgroundOverheadPrecompute  = makeOverhead(false);
    }
    // autoGen: Ignore end
}

#endif