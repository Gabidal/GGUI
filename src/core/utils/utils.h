#ifndef _UTILS_H_
#define _UTILS_H_
/**
 * This is an Utils file made for the Renderer.cpp to use internally, these are just removed to clean up the source code.
 */

#include "./units.h"

namespace GGUI{

    class element;
    
    namespace INTERNAL{
        // This class contains carry flags from previous cycle cross-thread, if another thread had some un-finished things when another thread was already running.
        class Carry{
        public:
            bool Resize = false;
            bool Terminate = false;     // Signals the shutdown of subthreads.

            ~Carry() = default;
        };
    }

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
    extern bool Collides(GGUI::IVector3 A, GGUI::IVector3 B, int A_Width = 1, int A_Height = 1, int B_Width = 1, int B_Height = 1);


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
     * @brief Finds the upper element relative to the current element's position.
     * 
     * This function retrieves the current element based on the mouse position and 
     * attempts to find an element directly above it by moving one pixel up. If an 
     * upper element is found and it is not the main element, the position of the 
     * upper element is returned. Otherwise, the position of the current element is returned.
     * 
     * @return IVector3 The position of the upper element if found, otherwise the position of the current element.
     */
    extern IVector3 Find_Upper_Element();

    /**
     * @brief Finds the lower element relative to the current element.
     * 
     * This function retrieves the current element based on the mouse position
     * and then attempts to find an element that is positioned directly below it.
     * 
     * @return IVector3 The position of the lower element if found, otherwise the position of the current element.
     */
    extern IVector3 Find_Lower_Element();

    /**
     * @brief Finds the element to the left of the current element.
     *
     * This function retrieves the current element based on the mouse position
     * and attempts to find an element one pixel to the left of it. If such an
     * element is found, its position is returned. If no left element is found,
     * the position of the current element is returned.
     *
     * @return IVector3 The position of the left element if found, otherwise the position of the current element.
     */
    extern IVector3 Find_Left_Element();

    /**
     * @brief Finds the element to the right of the current element.
     * 
     * This function first retrieves the current element based on the mouse position
     * and the main internal context. If the current element is found, it calculates
     * the position of the element to the right by moving one pixel to the right of
     * the current element's position. It then attempts to retrieve the element at
     * this new position.
     * 
     * @return IVector3 The position of the element to the right if found, otherwise
     *                  the position of the current element.
     */
    extern IVector3 Find_Right_Element();

    /**
     * @brief Returns the smaller of two signed long long integers.
     * 
     * This function compares two signed long long integers and returns the smaller of the two.
     * 
     * @param a The first signed long long integer to compare.
     * @param b The second signed long long integer to compare.
     * @return The smaller of the two signed long long integers.
     */
    extern signed long long Min(signed long long a, signed long long b);

    /**
     * @brief Returns the maximum of two signed long long integers.
     *
     * This function compares two signed long long integers and returns the greater of the two.
     *
     * @param a The first signed long long integer to compare.
     * @param b The second signed long long integer to compare.
     * @return The greater of the two signed long long integers.
     */
    extern signed long long Max(signed long long a, signed long long b);

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
    extern bool Has_Bit_At(char val, int i);

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
    extern float Lerp(int Min, int Max, int Position);

    /**
     * @brief Checks if the given flag is set in the given flags.
     * @details This function takes two unsigned long long parameters, one for the flags and one for the flag to check. It returns true if the flag is set in the flags, otherwise it returns false.
     *
     * @param f The flags to check.
     * @param Flag The flag to check for.
     * @return True if the flag is set, otherwise false.
     */
    extern bool Is(unsigned long long f, unsigned long long Flag);

    /**
     * @brief Checks if a flag is set in a set of flags.
     * @details This function takes two unsigned long long parameters, one for the flags and one for the flag to check. It returns true if the flag is set in the flags, otherwise it returns false.
     *
     * @param f The flags to check.
     * @param flag The flag to check for.
     * @return True if the flag is set, otherwise false.
     */
    extern bool Has(unsigned long long f, unsigned long long flag);

    /**
     * @brief Checks if all flags in small are set in big.
     * @details This function takes two unsigned long long parameters, one for the flags to check and one for the flags to check against. It returns true if all flags in small are set in big, otherwise it returns false.
     *
     * @param big The flags to check against.
     * @param small The flags to check.
     * @return True if all flags in small are set in big, otherwise false.
     */
    extern bool Contains(unsigned long long big, unsigned long long Small);

    /**
     * @brief Determines if a given pointer is likely deletable (heap-allocated).
     *
     * This function assesses whether a pointer may belong to the heap by comparing its
     * position relative to known memory sections such as the stack, heap, and data segments.
     *
     * @param ptr Pointer to be evaluated.
     * @return True if the pointer is likely deletable (heap-allocated), false otherwise.
     */
    extern bool isDeletable(void* ptr);
}

#endif