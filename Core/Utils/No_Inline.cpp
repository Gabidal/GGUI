#include "../Renderer.h"

namespace GGUI{

    extern void* Stack_Start_Address;
    extern void* Heap_Start_Address;

    /**
     * @brief Initializes the Stack_Start_Address and Heap_Start_Address variables
     *
     * This function captures the nearest stack address and the nearest heap address
     * and assigns them to the Stack_Start_Address and Heap_Start_Address variables
     * respectively. This only happens if the variables are not already set.
     */
    void Init_Start_Addresses(){
        if (Stack_Start_Address == nullptr){
            // Capture the nearest stack address
            int nearest_address = 0;
            Stack_Start_Address = &nearest_address;
        }

        if (Heap_Start_Address == nullptr)
            // Now also capture the nearest heap
            Heap_Start_Address = new int;
    }

}