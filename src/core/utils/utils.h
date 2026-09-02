#ifndef _CORE_UTILS_H_
#define _CORE_UTILS_H_
/**
 * This is an Utils file made for the core.cpp to use internally, these are just removed to clean up the source code.
 */

#include "types.h"
#include "color.h"

#include <math.h>
#include <cstring>
#include <algorithm>
#include <charconv>

namespace GGUI{
    class element;
    // class UTF;
    class RGB;

    // ===  TODO MACRO   ===
    #define GGUI_STR_IMPL(x) #x
    #define GGUI_STR(x) GGUI_STR_IMPL(x)

    #if defined(__clang__) || defined(__GNUC__)
        #define TODO(text) _Pragma(GGUI_STR(message("TODO: " text)))
    #else
        #define TODO(text)
    #endif
    // ===               ===, this is probably useless weight, but im gonna try this regardless hehe :)

    namespace utils{
        /**
         * @brief Checks if two rectangles collide.
         *
         * This function determines whether two rectangles, defined by their top-left
         * corners and dimensions, overlap in a 2D space.
         *
         * @param A The top-left corner of the first rectangle as a IVector3.
         * @param B The top-left corner of the second rectangle as a IVector3.
         * @param A_Width The width of the first rectangle.
         * @param A_Height The height of the first rectangle.
         * @param B_Width The width of the second rectangle.
         * @param B_Height The height of the second rectangle.
         * @return true if the rectangles overlap, false otherwise.
         */
        constexpr bool collides(IVector3 A, IVector3 B, int A_Width = 1, int A_Height = 1, int B_Width = 1, int B_Height = 1) noexcept {
            return (
                A.x() < B.x() + B_Width &&
                A.x() + A_Width > B.x() &&
                A.y() < B.y() + B_Height &&
                A.y() + A_Height > B.y()
            );
        }

        /**
         * @brief Checks if a bit is set in a char.
         * @details This function takes a char and an index as input and checks if the bit at the specified index is set.
         *          It returns true if the bit is set and false if it is not.
         *
         * @param val The char to check the bit in.
         * @param i The index of the bit to check.
         *
         * @return True if the bit is set, false if it is not.
         */
        constexpr bool hasBitAt(char val, int i) noexcept { return (val & (1 << i)) != 0; }

        /**
         * @brief Checks if the given flag is set in the given flags.
         * @details This function takes two unsigned long long parameters, one for the flags and one for the flag to check. It returns true if the flag is set in the flags, otherwise it returns false.
         *
         * @param f The flags to check.
         * @param Flag The flag to check for.
         * @return True if the flag is set, otherwise false.
         */
        constexpr bool is(unsigned long long f, unsigned long long Flag) noexcept { return (f & Flag) == Flag; }

        /**
         * @brief Checks if a flag is set in a set of flags.
         * @details This function takes two unsigned long long parameters, one for the flags and one for the flag to check. It returns true if the flag is set in the flags, otherwise it returns false.
         *
         * @param f The flags to check.
         * @param flag The flag to check for.
         * @return True if the flag is set, otherwise false.
         */
        constexpr bool has(unsigned long long f, unsigned long long flag) noexcept { return (f & flag) != 0ULL; }

        /**
         * @brief Checks if all flags in small are set in big.
         * @details This function takes two unsigned long long parameters, one for the flags to check and one for the flags to check against. It returns true if all flags in small are set in big, otherwise it returns false.
         *
         * @param big The flags to check against.
         * @param small The flags to check.
         * @return True if all flags in small are set in big, otherwise false.
         */
        constexpr bool contains(unsigned long long big, unsigned long long Small) noexcept { return (Small & big) == Small; }

        /**
         * Linear interpolation function
         * @param a The start value
         * @param b The end value
         * @param t The interpolation value:
         *   - decltype(t) == float, values are from [0, 1]
         *   - decltype(t) == decltype(a), values are from a to b.
         * @return The interpolated value
         */
        template<typename T, typename P>
        constexpr auto lerp(T a, T b, P t) {
            // This method is to determine where t is in relation to a and b
            if constexpr (std::is_same_v<P, T>) {
                float valueRange = static_cast<float>(b - a);
                float index = std::max(static_cast<T>(t - a), static_cast<T>(0));
                return 1.0f - index / valueRange;
            } else {    // This method is to determine where t is in relation to 0 and 1
                return a + t * (b - a);
            }
        }

        constexpr RGB lerp(RGB A, RGB B, int frameIndexRemainder, int frameDistance) {
            RGB result;
            auto extendedLerp = [frameIndexRemainder, frameDistance](int a, int b) {
                return (a * (frameDistance - frameIndexRemainder) + b * frameIndexRemainder) / frameDistance;
            };

            result.red = static_cast<unsigned char>(extendedLerp(A.red, B.red));
            result.green = static_cast<unsigned char>(extendedLerp(A.green, B.green));
            result.blue = static_cast<unsigned char>(extendedLerp(A.blue, B.blue));

            return result;
        }
    }

    // Contains useful all around utils for handling enums
    namespace table {
        template<typename enumType, typename = std::enable_if_t<std::is_enum_v<enumType> && (sizeof(enumType) == sizeof(uint8_t))>> 
        constexpr uint8_t alias(enumType value) { return static_cast<uint8_t>(value); }

        // If an enum is small enough, then it should be possible to be to stringed.
        template<typename enumType, typename = std::enable_if_t<(sizeof(enumType) == sizeof(uint8_t))>>
        constexpr char toString(enumType val) {
            return static_cast<char>(val);
        }

        template<typename T, typename P>
        requires eligibleForWriterViewType<T, P>
        constexpr void toString(writerView<T>& preAllocated, P val) {
            preAllocated.write(val);
        }

        /**
         * @brief Checks if a value falls within the range defined by an enum's __min and __max members.
         * 
         * @tparam E The enum type that defines __min and __max bounds
         * @param val The value to check against the enum's range
         * @return true if val is within the inclusive range [E::__min, E::__max]
         * @return false otherwise
         */
        template<typename E, typename V>
        constexpr bool contains(V val) {
            return static_cast<uint8_t>(val) >= static_cast<uint8_t>(E::__min) && static_cast<uint8_t>(val) <= static_cast<uint8_t>(E::__max);
        }

        template<typename V, typename E>
        constexpr bool is(V val, E selected) {
            return static_cast<uint8_t>(val) == static_cast<uint8_t>(selected);
        }

        template<typename enumType, typename = std::enable_if_t<std::is_enum_v<enumType>>>
        constexpr size_t getSize() {
            return (static_cast<uint8_t>(enumType::__max) - static_cast<uint8_t>(enumType::__min)) + 1;
        }
    }

    namespace number {
        template<typename T, typename P>
        requires eligibleForWriterViewType<T, P>
        constexpr void toString(writerView<T>& preAllocated, P val) {
            std::span<T> freeMemory = preAllocated.requestFree().getStorage();

            long long intValue = static_cast<long long>(val);

            auto [tail, errorCode] = std::to_chars(freeMemory.data(), freeMemory.data() + freeMemory.size(), intValue);

            assert(errorCode == std::errc() && "Failed to convert number to string!");

            size_t tailDistanceToBufferStart = static_cast<size_t>(tail - freeMemory.data());

            preAllocated.commit(std::span<T>{freeMemory.data(), tailDistanceToBufferStart});
        }

        template<typename T>
        requires eligibleForWriterViewType<char, T>
        constexpr void toString(std::string& appendTo, T num) {
            constexpr size_t basicLength = 32;  // long enough for large numbers
            char buffer[basicLength] = {0};
            std::span<char> bufferSpan(buffer, basicLength);

            writerView<char> writer(bufferSpan);
            toString<char, T>(writer, num);

            appendTo.append(bufferSpan.data(), writer.getSize());
        }
    }
}

#endif