#ifndef _UTILS_H_
#define _UTILS_H_
/**
 * This is an Utils file made for the Renderer.cpp to use internally, these are just removed to clean up the source code.
 */

#include "types.h"
#include "superString.h"
#include "color.h"
#include "fastVector.h"

#include <math.h>
#include <cstring>
#include <algorithm> // std::clamp
#include <cmath>     // std::pow, std::lround

namespace GGUI{
    class element;
    class UTF;
    class RGB;

    // autoGen: Ignore start
    namespace INTERNAL{
        extern std::string constructLoggerFileName();

        extern bool Identical_Frame;

        extern void De_Initialize();

        extern int BEFORE_ENCODE_BUFFER_SIZE;
        extern int AFTER_ENCODE_BUFFER_SIZE;
        
        /**
         * @brief The Renderer function is responsible for managing the rendering loop.
         * It waits for a condition to resume rendering, processes rendering tasks, and
         * then pauses itself until the condition is met again.
         * 
         * The function performs the following steps:
         * 1. Waits for the render thread to be resumed.
         * 2. Saves the current time.
         * 3. Checks if the rendering scheduler needs to be terminated.
         * 4. Processes carry flags and updates the maximum width and height if needed.
         * 5. Renders the main frame buffer.
         * 6. Encodes the buffer for optimization.
         * 7. Converts the abstract frame buffer to a string and renders the frame.
         * 8. Calculates the render delay.
         * 9. Pauses the render thread and notifies all waiting threads.
         */
        extern void renderer();

        /**
         * @brief Event_Thread is a function that runs an infinite loop to handle various events and tasks.
         * 
         * This function performs the following tasks in each iteration of the loop:
         * - Resets the thread load counter and updates the previous time.
         * - Calls functions to recall memories, go through file streams, and refresh the multi-frame canvas.
         * - Checks for termination signals and breaks out of the loop if the terminate flag is set.
         * - Updates the current time and calculates the delta time.
         * - Adjusts the current update speed based on the event thread load.
         * - Sleeps for a calculated duration to control the update speed.
         * 
         * The function is designed to be used in a multi-threaded environment where it can be paused and resumed as needed.
         * 
         * @note If uncapped FPS is desired, the sleep code can be disabled.
         */
        extern void eventThread();

        /**
         * @brief Function that continuously handles user input in a separate thread.
         *
         * This function runs an infinite loop where it performs the following steps:
         * 1. Waits for user input by calling INTERNAL::Query_Inputs().
         * 2. Pauses the GGUI system and performs the following actions:
         *    - Records the current time as INTERNAL::Previous_Time.
         *    - Translates the queried inputs using INTERNAL::Translate_Inputs().
         *    - Processes scroll and mouse inputs using SCROLL_API() and MOUSE_API().
         *    - Calls the event handlers to react to the parsed input using Event_Handler().
         *    - Records the current time as INTERNAL::Current_Time.
         *    - Calculates the delta time (input delay) and stores it in INTERNAL::Input_Delay.
         */
        extern void inputThread();

        /**
         * @brief Converts an unsigned long long integer to its uppercase hexadecimal string representation.
         * 
         * This function takes an unsigned long long integer and formats it as a hexadecimal string
         * in uppercase. The resulting string does not include a "0x" prefix.
         * 
         * @param value The unsigned long long integer to be converted to a hexadecimal string.
         * @return A std::string containing the uppercase hexadecimal representation of the input value.
         */
        extern std::string Hex(unsigned long long value);

        /**
         * @brief Checks if two rectangles collide.
         *
         * This function determines whether two rectangles, defined by their top-left
         * corners and dimensions, overlap in a 2D space.
         *
         * @param A The top-left corner of the first rectangle as a GGUI::IVector3.
         * @param B The top-left corner of the second rectangle as a GGUI::IVector3.
         * @param A_Width The width of the first rectangle.
         * @param A_Height The height of the first rectangle.
         * @param B_Width The width of the second rectangle.
         * @param B_Height The height of the second rectangle.
         * @return true if the rectangles overlap, false otherwise.
         */
        constexpr bool Collides(GGUI::IVector3 A, GGUI::IVector3 B, int A_Width = 1, int A_Height = 1, int B_Width = 1, int B_Height = 1) noexcept {
            return (
                A.X < B.X + B_Width &&
                A.X + A_Width > B.X &&
                A.Y < B.Y + B_Height &&
                A.Y + A_Height > B.Y
            );
        }

        /**
         * @brief Checks if two GGUI elements collide.
         * 
         * This function determines whether two GGUI elements, `a` and `b`, collide with each other.
         * If the elements are the same (i.e., `a` is equal to `b`), the function returns the value of `Identity`.
         * Otherwise, it checks for collision based on the absolute positions and dimensions of the elements.
         * 
         * @param a Pointer to the first GGUI element.
         * @param b Pointer to the second GGUI element.
         * @param Identity Boolean value to return if the elements are the same.
         * @return true if the elements collide, false otherwise.
         */
        extern bool Collides(GGUI::element* a, GGUI::element* b, bool Identity = true);

        /**
         * @brief Checks if a given point collides with a specified element.
         * 
         * This function determines if the point `b` collides with the element `a` by 
         * calling another `Collides` function with the element's absolute position, 
         * width, height, and the point's assumed dimensions of 1x1.
         * 
         * @param a Pointer to the GGUI::Element to check for collision.
         * @param b The point (as GGUI::IVector3) to check for collision with the element.
         * @return true if the point collides with the element, false otherwise.
         */
        extern bool Collides(GGUI::element* a, GGUI::IVector3 b);

        /**
         * @brief Recursively finds the most accurate element that contains the given position.
         * 
         * This function checks if the given position is within the bounds of the parent element.
         * If it is, it then checks all the child elements of the parent to see if any of them
         * contain the position. If a child element contains the position, the function is called
         * recursively on that child element. If no child element contains the position, the parent
         * element is returned.
         * 
         * @param c The position to check, represented as an IVector3.
         * @param Parent The parent element to start the search from.
         * @return Element* The most accurate element that contains the given position, or nullptr if the position is not within the bounds of the parent element.
         */
        extern element* Get_Accurate_Element_From(IVector3 c, element* Parent);

        /**
         * @brief Returns the smaller of two signed long long integers.
         * 
         * This function compares two signed long long integers and returns the smaller of the two.
         * 
         * @param a The first signed long long integer to compare.
         * @param b The second signed long long integer to compare.
         * @return The smaller of the two signed long long integers.
         */
        constexpr signed long long Min(signed long long a, signed long long b) noexcept { return a < b ? a : b; }

        /**
         * @brief Returns the maximum of two signed long long integers.
         *
         * This function compares two signed long long integers and returns the greater of the two.
         *
         * @param a The first signed long long integer to compare.
         * @param b The second signed long long integer to compare.
         * @return The greater of the two signed long long integers.
         */
        constexpr signed long long Max(signed long long a, signed long long b) noexcept { return a > b ? a : b; }

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
        constexpr bool Has_Bit_At(char val, int i) noexcept { return (val & (1 << i)) != 0; }

        /**
         * @brief Gets the contents of a given position in the buffer.
         * @details This function takes a position in the buffer and returns the contents of that position. If the position is out of bounds, it will return nullptr.
         * @param Absolute_Position The position to get the contents of.
         * @return The contents of the given position, or nullptr if the position is out of bounds.
         */
        extern GGUI::UTF* Get(GGUI::IVector3 Absolute_Position);

        /**
         * @brief Calculates the current load of the GGUI thread based on the given current position.
         * @param Min The minimum value the load can have.
         * @param Max The maximum value the load can have.
         * @param Position The current position of the load.
         * @return The current load of the GGUI thread from 0 to 1.
         */
        constexpr float Lerp(int Min, int Max, int Position) noexcept {
            float Length_Of_Possible_Values = static_cast<float>(Max - Min);
            float Offset_Of_Our_Load = GGUI::INTERNAL::Max(static_cast<signed long long>(Position - Min), 0);
            return 1.0f - Offset_Of_Our_Load / Length_Of_Possible_Values;
        }

        /**
         * @brief Checks if the given flag is set in the given flags.
         * @details This function takes two unsigned long long parameters, one for the flags and one for the flag to check. It returns true if the flag is set in the flags, otherwise it returns false.
         *
         * @param f The flags to check.
         * @param Flag The flag to check for.
         * @return True if the flag is set, otherwise false.
         */
        constexpr bool Is(unsigned long long f, unsigned long long Flag) noexcept { return (f & Flag) == Flag; }

        /**
         * @brief Checks if a flag is set in a set of flags.
         * @details This function takes two unsigned long long parameters, one for the flags and one for the flag to check. It returns true if the flag is set in the flags, otherwise it returns false.
         *
         * @param f The flags to check.
         * @param flag The flag to check for.
         * @return True if the flag is set, otherwise false.
         */
        constexpr bool Has(unsigned long long f, unsigned long long flag) noexcept { return (f & flag) != 0ULL; }

        extern bool Has(ALLOCATION_TYPE f, ALLOCATION_TYPE flag);

        /**
         * @brief Checks if all flags in small are set in big.
         * @details This function takes two unsigned long long parameters, one for the flags to check and one for the flags to check against. It returns true if all flags in small are set in big, otherwise it returns false.
         *
         * @param big The flags to check against.
         * @param small The flags to check.
         * @return True if all flags in small are set in big, otherwise false.
         */
        constexpr bool Contains(unsigned long long big, unsigned long long Small) noexcept { return (Small & big) == Small; }

        extern bool Contains(ALLOCATION_TYPE big, ALLOCATION_TYPE small);

        /**
         * @brief Determines if a given pointer is likely deletable (heap-allocated).
         *
         * This function assesses whether a pointer may belong to the heap by comparing its
         * position relative to known memory sections such as the stack, heap, and data segments.
         *
         * @param ptr Pointer to be evaluated.
         * @return True if the pointer is likely deletable (heap-allocated), false otherwise.
         */
        extern ALLOCATION_TYPE getAllocationType(const void* ptr);

        /**
         * Linear interpolation function
         * @param a The start value
         * @param b The end value
         * @param t The interpolation value, between 0 and 1
         * @return The interpolated value
         */
        template<typename T>
        constexpr T lerp(T a, T b, T t) {
            // Clamp t between a and b
            return a + t * (b - a);
        }

        /**
         * @brief Performs gamma-corrected linear interpolation between two values.
         * 
         * @tparam T The type of the input values.
         * @tparam P The type of the interpolation factor.
         * @param a The start value.
         * @param b The end value.
         * @param t The interpolation factor, typically between 0 and 1.
         * @return The interpolated value, gamma-corrected and cast back to type T.
         */
        template<typename T, typename P>
        constexpr T Interpolate(T a, T b, P t) {
            // Define gamma value for correction
            constexpr float gamma = 2.2F;

            // Apply gamma correction to input values and perform linear interpolation
            const float c_f = lerp<float>(std::pow(static_cast<float>(a), gamma), std::pow(static_cast<float>(b), gamma), t);

            // Reverse gamma correction and cast back to original type
            return static_cast<T>(std::pow(c_f, 1.F / gamma));
        }

        namespace fast {
            // Initialize LUTs once (thread-safe in C++11+ for function statics)
            constexpr std::array<float, 256> make_s2l() {
                std::array<float, 256> arr{};
                for (int i = 0; i < 256; ++i) {
                    float s = static_cast<float>(i) / 255.0f;
                    // precomputed offline! Here just fill linear as placeholder
                    arr[i] = s * s; 
                }
                return arr;
            }

            constexpr std::array<unsigned char, 256> make_l2s() {
                std::array<unsigned char, 256> arr{};
                for (int i = 0; i < 256; ++i) {
                    float l = static_cast<float>(i) / 255.0f;
                    float s = l; // again: replace with offline precomputed pow(l,1/2.2)
                    int v = static_cast<int>(s * 255.0f + 0.5f);
                    arr[i] = static_cast<unsigned char>(v < 0 ? 0 : v > 255 ? 255 : v);
                }
                return arr;
            }

            struct GammaLUT {
                std::array<float, 256> s2l;
                std::array<unsigned char, 256> l2s_u8;

                constexpr GammaLUT() : s2l(make_s2l()), l2s_u8(make_l2s()) {}
            };

            constexpr GammaLUT LUT{};

            inline unsigned char Interpolate(unsigned char a, unsigned char b, float t) {
                float la = LUT.s2l[a];
                float lb = LUT.s2l[b];
                float lc = la + (lb - la) * t; // linear blend in linear space
                // Map back via LUT: index by 0..255
                int idx = static_cast<int>(std::lround(std::clamp(lc, 0.0f, 1.0f) * 255.0f));
                return LUT.l2s_u8[idx];
            };

            // Fast linear interpolate for 8-bit channels without gamma correction.
            // Uses fixed-point weights to avoid divisions and minimize float work.
            constexpr unsigned char InterpolateLinearU8(unsigned char a, unsigned char b, float t) {
                // Clamp t and convert to 0..256 fixed-point weight with rounding
                int w = static_cast<int>(t * (float)UINT8_MAX + 0.5f);   // 0..256
                int inv = UINT8_MAX - w;                             // 256..0

                // Weighted sum with rounding, then >> 8 instead of /255
                int sum = a * inv + b * w;                     // <= 255*256 + 255*256 = 130560
                return static_cast<unsigned char>((sum + UINT8_MAX/2) >> 8);
            }
        }

        /**
         * @brief Interpolates between two RGB colors using linear interpolation.
         * If SETTINGS::ENABLE_GAMMA_CORRECTION is enabled, the interpolation is done in a gamma-corrected space.
         * @param A The start RGB color.
         * @param B The end RGB color.
         * @param Distance The interpolation factor, typically between 0 and 1.
         * @return The interpolated RGB color.
         */
        extern GGUI::RGB Lerp(GGUI::RGB A, GGUI::RGB B, float Distance);

        /**
         * @brief Convert a liquefied UTF fastVector into a cached std::string.
         *
         * Re-uses a static std::string buffer between calls to avoid heap churn. The
         * function expects that Liquefied_Size equals the sum of the sizes of all
         * compactString entries in Data and will resize the cached string if the size
         * differs. Multi‑byte (unicode) entries are memcpy'd; single byte ASCII entries
         * are written directly. Entries with size == 0 are skipped.
         *
         * @param Data Contiguous collection of compactString produced by liquifyUTFText().
         * @param Liquefied_Size Pre-computed total number of bytes represented by Data.
         * @return Pointer to an internally cached std::string containing the concatenated bytes.
         * @warning The returned pointer becomes invalid after the next call to this function.
         */
        inline std::string* To_String(fastVector<compactString> Data, unsigned int Liquefied_Size) noexcept {
            static std::string result; // internal cache between renders

            if (result.size() != Liquefied_Size){
                // Resize a std::string to the total size.
                result.resize(Liquefied_Size, '\0');
            }

            // Fast-path pointer access to avoid bounds checks and replace overhead
            char* outputAddress = result.data();
            unsigned int outputIndex = 0;

            const compactString* dataAddress = Data.getData();
            const size_t cachedSize = Data.getSize();

            for (size_t i = 0; i < cachedSize; i++) {
                const compactString& data = dataAddress[i];

                // If current compact string data entry is a unicode
                if (data.size > 1) {
                    // Copy multi-byte unicode sequence directly
                    std::memcpy(outputAddress + outputIndex, data.getUnicode(), data.size);
                    outputIndex += data.size;
                } else if (data.size == 1) {    // If current compact string data entry is a unicode
                    outputAddress[outputIndex++] = data.getAscii();
                } // else data.size == 0 -> skip
            }

            return &result;
        }

        /**
         * @brief Create a std::string from a single compactString.
         *
         * Allocates a std::string sized to the compactString length and copies either the
         * multi-byte unicode sequence or the single ASCII character.
         *
         * @param cstr Source compactString.
         * @return Newly constructed std::string containing the character data (no caching).
         */
        inline std::string To_String(compactString& cstr){
            // Resize a std::string to the total size.
            std::string result;
            result.resize(cstr.size);

            // Copy the contents of the Compact_String into the std::string.
            if (cstr.size > 1){
                // Replace the current contents of the string with the contents of the Unicode data.
                result.replace(0, cstr.size, cstr.getUnicode());
            }
            else{
                // Add the single character to the string.
                result[0] = cstr.getAscii();
            }

            return result;
        }
    }
    // autoGen: Ignore end

    extern std::string toString(UTF coloredText, bool transparentBackground = false);
}

#endif