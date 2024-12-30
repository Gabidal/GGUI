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
                    // Find the start of the line containing the "[stack]" entry
                    size_t line_start = maps_content.rfind("\n", stack_start);
                    line_start = (line_start == std::string::npos) ? 0 : line_start + 1; // Adjust for the beginning of the file

                    // Find the end of the line containing the "[stack]" entry
                    size_t line_end = maps_content.find("\n", stack_start);

                    // Extract the line containing the stack information
                    std::string stack_line = maps_content.substr(line_start, line_end - line_start);

                    // Extract the memory range (e.g., "7ffc90a54000-7ffc90a76000")
                    size_t dash_pos = stack_line.find("-");
                    if (dash_pos != std::string::npos) {
                        // Get the ending address of the stack area
                        std::string end_address = stack_line.substr(dash_pos + 1, stack_line.find(" ", dash_pos) - dash_pos - 1);
                        
                        // The reason we are storing the stack ending address, is because of how stack works in reverse, so the highest point in stack address space is actually the baseline.
                        Stack_Start_Address = (void*)std::stoul(end_address, nullptr, 16);
                    } else {
                        // Handle error: invalid stack line format
                        std::cerr << "Error: Failed to parse stack line: " << stack_line << std::endl;
                    }
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