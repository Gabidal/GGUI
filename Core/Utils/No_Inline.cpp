#include "../Renderer.h"
#include "../../Elements/File_Streamer.h"

namespace GGUI{
    namespace INTERNAL{
        extern void* Stack_Start_Address;
        extern void* Heap_Start_Address;

        /**
         * @brief Initializes the Stack_Start_Address and Heap_Start_Address variables
         *
         * This function captures the nearest stack address and the nearest heap address
         * and assigns them to the Stack_Start_Address and Heap_Start_Address variables
         * respectively. This only happens if the variables are not already set.
         */
        #if _WIN32
        void Read_Start_Addresses(){
            if (Stack_Start_Address == nullptr){
                // Capture the nearest stack address
                int nearest_address = 0;
                Stack_Start_Address = &nearest_address;
            }

            if (Heap_Start_Address == nullptr){
                // Now also capture the nearest heap
                Heap_Start_Address = new int;
            }
        }
        #else
        void Read_Start_Addresses(){
            if (Stack_Start_Address == nullptr){
                // Ask the kernel for /proc/self/maps to read the stack areas.
                FILE_STREAM maps("/proc/self/maps");

                // Read the contents of /proc/self/maps
                std::string maps_content = maps.Read();
                
                // Find the stack area in the maps content
                size_t stack_start = maps_content.find("[stack]");

                if (stack_start != std::string::npos){
                    // Find the start of the stack area
                    size_t start = maps_content.rfind("\n", stack_start) + 1;
                    size_t end = maps_content.find("-", start);

                    // Get the address of the stack area
                    std::string address = maps_content.substr(start, end - start);
                    Stack_Start_Address = (void*)std::stoul(address, nullptr, 16);
                }
                else{
                    // If the stack area is not found, capture the nearest stack
                    int nearest_address = 0;
                    Stack_Start_Address = &nearest_address;
                    
                    // Report an error
                    Report_Stack("Failed to find the stack area in /proc/self/maps!");
                }
            }

            if (Heap_Start_Address == nullptr){
                // Now also capture the nearest heap
                Heap_Start_Address = new int;
            }
        }
        #endif
    }
}