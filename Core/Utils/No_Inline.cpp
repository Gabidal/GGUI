#include "../Renderer.h"

namespace GGUI{

    extern void* Stack_Start_Address;
    extern void* Heap_Start_Address;

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