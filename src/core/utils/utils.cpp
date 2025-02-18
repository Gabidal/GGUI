#include <string>

#include "./utils.h"
#include "../../elements/element.h"
#include "../renderer.h"

namespace GGUI{
    namespace INTERNAL{
        extern void* Stack_Start_Address;
        extern void* Heap_Start_Address;
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
    std::string Hex(unsigned long long value) {
        char buffer[17]; // Enough to hold the largest 64-bit hexadecimal value + null terminator
        std::snprintf(buffer, sizeof(buffer), "%llX", value); // Formats the value as uppercase hex
        return std::string(buffer);
    }

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
    bool Collides(GGUI::IVector3 A, GGUI::IVector3 B, int A_Width, int A_Height, int B_Width, int B_Height) {
        // Check if the rectangles overlap in the x-axis and y-axis
        return (
            A.X < B.X + B_Width &&   // A's right edge is beyond B's left edge
            A.X + A_Width > B.X &&   // A's left edge is before B's right edge
            A.Y < B.Y + B_Height &&  // A's bottom edge is below B's top edge
            A.Y + A_Height > B.Y     // A's top edge is above B's bottom edge
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
    bool Collides(GGUI::element* a, GGUI::element* b, bool Identity) {
        if (a == b)
            return Identity;    // For custom purposes, defaults into true

        return Collides(
            a->getAbsolutePosition(), b->getAbsolutePosition(),
            a->getWidth(), a->getHeight(), b->getWidth(), b->getHeight()
        );
    }

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
    bool Collides(GGUI::element* a, GGUI::IVector3 b) {
        // Call the Collides function with the element's position and dimensions, and the point with assumed dimensions of 1x1.
        return Collides(a->getAbsolutePosition(), b, a->getWidth(), a->getHeight(), 1, 1);
    }

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
    element* Get_Accurate_Element_From(IVector3 c, element* Parent){
        //first check if the c is in bounds of Parent.
        if (!Collides(Parent, c)){
            return nullptr;
        }

        // Check all the child elements of the parent.
        for (auto child : Parent->getChilds()){
            if (Collides(child, c)){
                // If a child element contains the position, search in the child element.
                return Get_Accurate_Element_From(c, child);
            }
        }

        // If no child element contains the position, return the parent element.
        return Parent;
    }

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
    IVector3 Find_Upper_Element(){
        //first get the current element.
        element* Current_Element = Get_Accurate_Element_From(INTERNAL::Mouse, INTERNAL::Main);

        if (Current_Element == nullptr){
            return false;
        }

        IVector3 tmp_c = Current_Element->getPosition();

        tmp_c.Y--;  // Move one pixel up

        element* Upper_Element = Get_Accurate_Element_From(tmp_c, INTERNAL::Main);

        if (Upper_Element && Upper_Element != (element*)&INTERNAL::Main){
            return Upper_Element->getPosition();
        }

        return Current_Element->getPosition();
    }

    /**
     * @brief Finds the lower element relative to the current element.
     * 
     * This function retrieves the current element based on the mouse position
     * and then attempts to find an element that is positioned directly below it.
     * 
     * @return IVector3 The position of the lower element if found, otherwise the position of the current element.
     */
    IVector3 Find_Lower_Element(){
        //first get the current element.
        element* Current_Element = Get_Accurate_Element_From(INTERNAL::Mouse, INTERNAL::Main);

        if (Current_Element == nullptr){
            return false;
        }

        IVector3 tmp_c = Current_Element->getPosition();

        tmp_c.Y += Current_Element->getHeight();

        element* Lower_Element = Get_Accurate_Element_From(tmp_c, INTERNAL::Main);

        if (Lower_Element && Lower_Element != (element*)&INTERNAL::Main){
            return Lower_Element->getPosition();
        }

        return Current_Element->getPosition();
    }

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
    IVector3 Find_Left_Element(){
        //first get the current element.
        element* Current_Element = Get_Accurate_Element_From(INTERNAL::Mouse, INTERNAL::Main);

        if (Current_Element == nullptr){
            return false;
        }

        IVector3 tmp_c = Current_Element->getPosition();

        // Move one pixel to the left
        tmp_c.X--;

        element* Left_Element = Get_Accurate_Element_From(tmp_c, INTERNAL::Main);

        if (Left_Element && Left_Element != (element*)&INTERNAL::Main){
            // If a left element is found, return its position
            return Left_Element->getPosition();
        }

        // If no left element is found, return the current element's position
        return Current_Element->getPosition();
    }

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
    IVector3 Find_Right_Element(){
        //first get the current element.
        element* Current_Element = Get_Accurate_Element_From(INTERNAL::Mouse, INTERNAL::Main);

        if (Current_Element == nullptr){
            return false;
        }

        IVector3 tmp_c = Current_Element->getPosition();

        // Move one pixel to the right
        tmp_c.X += Current_Element->getWidth();

        element* Right_Element = Get_Accurate_Element_From(tmp_c, INTERNAL::Main);

        if (Right_Element && Right_Element != (element*)&INTERNAL::Main){
            // If a right element is found, return its position
            return Right_Element->getPosition();
        }

        // If no right element is found, return the current element's position
        return Current_Element->getPosition();
    }

    /**
     * @brief Finds the closest element to the given start position from a list of candidate elements.
     * 
     * This function iterates through a list of candidate elements and calculates the distance 
     * between each candidate's position and the given start position. It returns the candidate 
     * element that is closest to the start position.
     * 
     * @param start The starting position as an IVector3 object.
     * @param Candidates A vector of pointers to Element objects representing the candidate elements.
     * @return Element* A pointer to the closest candidate element. Returns nullptr if the Candidates vector is empty.
     */
    element* Find_Closest_Absolute_Element(IVector3 start, std::vector<element*> Candidates){
        // Start from the position and check if the up, down, left, right are within the bounds of the renderable window.
        // If they are, check if they collide with any element.
        // cast "rays" to each four directions, and return the lengths of each collision between the center of the rectangles and the start point.
        // return the smallest one.
        if (Candidates.size() == 0){
            INTERNAL::reportStack("Missing Candidates!");
        }

        element* Best_Candidate = nullptr;
        float Shortest_Distance = std::numeric_limits<float>::max();
        IVector3 CC; // Center of Candidate

        for (auto& candidate : Candidates){
            if (!candidate) 
                continue;   // Incase of event handlers with their stupid empty host.

            // Calculate the distance between the candidate position and the start position
            CC = candidate->getAbsolutePosition();
            float Distance = std::sqrt(std::pow(CC.X - start.X, 2) + std::pow(CC.Y - start.Y, 2));

            if (Distance < Shortest_Distance){
                Shortest_Distance = Distance;
                Best_Candidate = candidate;
            }
        }

        return Best_Candidate;
    }

    /**
     * @brief Returns the smaller of two signed long long integers.
     * 
     * This function compares two signed long long integers and returns the smaller of the two.
     * 
     * @param a The first signed long long integer to compare.
     * @param b The second signed long long integer to compare.
     * @return The smaller of the two signed long long integers.
     */
    signed long long Min(signed long long a, signed long long b){
        return a < b ? a : b;
    }

    /**
     * @brief Returns the maximum of two signed long long integers.
     *
     * This function compares two signed long long integers and returns the greater of the two.
     *
     * @param a The first signed long long integer to compare.
     * @param b The second signed long long integer to compare.
     * @return The greater of the two signed long long integers.
     */
    signed long long Max(signed long long a, signed long long b){
        return a > b ? a : b;
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
    bool Has_Bit_At(char val, int i){
        // Shift the bit to the right i times and then check if the bit is set.
        return ((val) & (1<<(i))) != 0;
    }

    /**
     * @brief Gets the contents of a given position in the buffer.
     * @details This function takes a position in the buffer and returns the contents of that position. If the position is out of bounds, it will return nullptr.
     * @param Absolute_Position The position to get the contents of.
     * @return The contents of the given position, or nullptr if the position is out of bounds.
     */
    GGUI::UTF* Get(GGUI::IVector3 Absolute_Position){
        if (Absolute_Position.X >= getMaxWidth() || 
            Absolute_Position.Y >= getMaxHeight() ||
            Absolute_Position.X < 0 || 
            Absolute_Position.Y < 0)
        {
            // The position is out of bounds, return nullptr
            return nullptr;
        }
        else{
            // The position is in bounds, return the contents of that position
            return &INTERNAL::Abstract_Frame_Buffer[Absolute_Position.Y * getMaxWidth() + Absolute_Position.X];
        }
    }

    /**
     * @brief Calculates the current load of the GGUI thread based on the given current position.
     * @param Min The minimum value the load can have.
     * @param Max The maximum value the load can have.
     * @param Position The current position of the load.
     * @return The current load of the GGUI thread from 0 to 1.
     */
    float Lerp(int Min, int Max, int Position){
        // First calculate the possible length of which our load can represent on.
        float Length_Of_Possible_Values = Max - Min;

        // Get the offset of which the wanted target fps deviates from the minimum value
        float Offset_Of_Our_Load = GGUI::Max(Position - Min, 0);

        // Calculate the simple probability.
        return 1 - Offset_Of_Our_Load / Length_Of_Possible_Values;
    }
    
    /**
     * @brief Checks if the given flag is set in the given flags.
     * @details This function takes two unsigned long long parameters, one for the flags and one for the flag to check. It returns true if the flag is set in the flags, otherwise it returns false.
     *
     * @param f The flags to check.
     * @param Flag The flag to check for.
     * @return True if the flag is set, otherwise false.
     */
    bool Is(unsigned long long f, unsigned long long Flag){
        return (f & Flag) == Flag;
    }

    /**
     * @brief Checks if a flag is set in a set of flags.
     * @details This function takes two unsigned long long parameters, one for the flags and one for the flag to check. It returns true if the flag is set in the flags, otherwise it returns false.
     *
     * @param f The flags to check.
     * @param flag The flag to check for.
     * @return True if the flag is set, otherwise false.
     */
    bool Has(unsigned long long f, unsigned long long flag){
        return (f & flag) != 0;
    }

    /**
     * @brief Checks if all flags in small are set in big.
     * @details This function takes two unsigned long long parameters, one for the flags to check and one for the flags to check against. It returns true if all flags in small are set in big, otherwise it returns false.
     *
     * @param big The flags to check against.
     * @param small The flags to check.
     * @return True if all flags in small are set in big, otherwise false.
     */
    bool Contains(unsigned long long big, unsigned long long Small) {
        return (Small & big) == Small;
    }
    
    /**
     * @brief Determines if a given pointer is likely deletable (heap-allocated).
     *
     * This function assesses whether a pointer may belong to the heap by comparing its
     * position relative to known memory sections such as the stack, heap, and data segments.
     *
     * @param ptr Pointer to be evaluated.
     * @return True if the pointer is likely deletable (heap-allocated), false otherwise.
     */
    bool isDeletable(void* ptr) {
        if (ptr == nullptr) {
            return false;  // Null pointer can't be valid
        }

        static void* Start_Of_BSS = nullptr;  // Placeholder for BSS segment start

        constexpr int MiB = 0x100000;
        static signed long long Somewhere_In_DATA = 100 * MiB;  // Arbitrary location in the data section

        // Check if ptr is above BSS, indicating potential data section location
        bool Ptr_Is_Above_BSS = ptr >= Start_Of_BSS;

        // Calculate if ptr is within range of the data section
        bool Ptr_Is_In_Range_Of_DATA_Section = ((signed long long)ptr - (signed long long)&Somewhere_In_DATA) <= Somewhere_In_DATA;

        // Check if ptr is smaller than the stack start address
        bool Lower_Than_Stack = (uintptr_t)ptr < (uintptr_t)INTERNAL::Stack_Start_Address;

        // Try to allocate memory on the heap for comparison
        size_t* new_heap = new(std::nothrow) size_t;
        if (new_heap == nullptr) {
            INTERNAL::reportStack("Failed to allocate new heap for stack pointer check!");
            exit(1);  // FATAL error if heap allocation fails
        }

        // Check if the new heap address is below the stack start address
        bool Heap_Is_Lower_Than_Stack = (uintptr_t)new_heap < (uintptr_t)INTERNAL::Stack_Start_Address;

        // Calculate distance from ptr to the stack start address
        uintptr_t ptr_distance_to_stack = (uintptr_t)INTERNAL::Stack_Start_Address - (uintptr_t)ptr;

        // Calculate distance from ptr to the closest heap address
        uintptr_t heap_min_address = Min((uintptr_t)new_heap, (uintptr_t)INTERNAL::Heap_Start_Address);
        uintptr_t ptr_distance_to_heap = heap_min_address - (uintptr_t)ptr;

        // Determine if ptr is closer to the stack than the heap and below the stack start
        bool Stack_Is_Closer = ptr_distance_to_stack < ptr_distance_to_heap && Lower_Than_Stack;

        // Clean up the heap allocation
        delete new_heap;

        // Assess likelihood of pointer being in heap, stack, or data section
        int Points_To_DATA_Section = Ptr_Is_Above_BSS + Ptr_Is_In_Range_Of_DATA_Section;  // 2pts for data section
        int Points_To_Stack = Lower_Than_Stack + Stack_Is_Closer;  // 2pts for stack
        int Points_To_Heap = !Lower_Than_Stack + !Stack_Is_Closer + Heap_Is_Lower_Than_Stack - Points_To_DATA_Section;  // 3pts for heap

        // Return true if pointer is more likely heap-allocated
        return Points_To_Heap > Points_To_Stack && Points_To_Heap > Points_To_DATA_Section;
    }

}