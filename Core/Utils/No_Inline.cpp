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
        #include <windows.h>
        #include <winternl.h>

        /**
         * @typedef NtQueryInformationThread_t
         * @brief A typedef for a function pointer to the NtQueryInformationThread function.
         * 
         * This function retrieves information about the specified thread.
         * 
         * @param ThreadHandle A handle to the thread for which information is to be retrieved.
         * @param ThreadInformationClass The class of information to be retrieved.
         * @param ThreadInformation A pointer to a buffer that receives the requested information.
         * @param ThreadInformationLength The size of the buffer pointed to by ThreadInformation.
         * @param ReturnLength A pointer to a variable that receives the size of the data returned in the buffer.
         * 
         * @return NTSTATUS The status code returned by the function.
         */
        typedef NTSTATUS(WINAPI* NtQueryInformationThread_t)(
            HANDLE ThreadHandle,
            THREADINFOCLASS ThreadInformationClass,
            PVOID ThreadInformation,
            ULONG ThreadInformationLength,
            PULONG ReturnLength
        );

        /**
         * @typedef PNT_TIB
         * @brief A typedef for a pointer to the NT_TIB structure.
         * 
         * The NT_TIB structure is the Thread Information Block for a thread.
         * It contains information about the thread's stack base and limit.
         */
        typedef struct _THREAD_BASIC_INFORMATION {
            NTSTATUS ExitStatus;
            PVOID TebBaseAddress;
            CLIENT_ID ClientId;
            PVOID AffinityMask;
            LONG Priority;
            LONG BasePriority;
        } THREAD_BASIC_INFORMATION;

        /**
         * @brief Reads and initializes the start addresses for the stack and heap.
         *
         * This function dynamically loads the NtQueryInformationThread function from ntdll.dll
         * to retrieve the Thread Environment Block (TEB) for the current thread. It then uses
         * the TEB to determine the stack base and limit, and calculates the stack start address.
         * Additionally, it initializes the heap start address by allocating a new integer.
         *
         * @note This function should be called only once to initialize the addresses.
         *       Subsequent calls will have no effect if the addresses are already initialized.
         *
         * @warning If the function fails to load ntdll.dll or retrieve NtQueryInformationThread,
         *          it will print an error message and return without initializing the addresses.
         */
        void Read_Start_Addresses(){
            if (Stack_Start_Address == nullptr){

                // Load NtQueryInformationThread dynamically
                HMODULE ntdll = LoadLibraryA("ntdll.dll");
                if (!ntdll) {
                    std::cerr << "Failed to load ntdll.dll" << std::endl;
                    return;
                }

                // We have to cast first to void* so that we can elude warnings.
                auto NtQueryInformationThread = (NtQueryInformationThread_t)((void*)GetProcAddress(ntdll, "NtQueryInformationThread"));
                if (!NtQueryInformationThread) {
                    std::cerr << "Failed to get NtQueryInformationThread" << std::endl;
                    return;
                }

                THREAD_BASIC_INFORMATION tbi;
                NTSTATUS status = NtQueryInformationThread(
                    GetCurrentThread(),
                    (THREADINFOCLASS)0,  // ThreadBasicInformation
                    &tbi,
                    sizeof(tbi),
                    nullptr
                );

                if (status != 0) {
                    std::cerr << "NtQueryInformationThread failed with status: " << std::hex << status << std::endl;
                    return;
                }

                // The TEB (Thread Environment Block) contains stack base/limit
                PNT_TIB teb = (PNT_TIB)tbi.TebBaseAddress;

                Stack_Start_Address = (void*)((unsigned long long)teb->StackBase + (unsigned long long)teb->StackLimit);
            }

            if (Heap_Start_Address == nullptr){
                // Now also capture the nearest heap
                Heap_Start_Address = new int;
            }
        }
        #else
        /**
         * @brief Reads the start addresses of the stack and heap memory areas.
         * 
         * This function attempts to read the start address of the stack memory area
         * by parsing the contents of the `/proc/self/maps` file. If the stack area
         * is found, it extracts the ending address of the stack and stores it in
         * `Stack_Start_Address`. If the stack area is not found, it captures the
         * nearest stack address and reports an error.
         * 
         * Additionally, this function captures the nearest heap address and stores
         * it in `Heap_Start_Address` if it is not already set.
         * 
         * @note This function is intended to be used on systems that provide the
         *       `/proc/self/maps` file, such as Linux.
         */
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