#include <string>
#include <cmath>
#include <algorithm>

#include "utils.h"
#include "../../elements/element.h"
#include "fileStreamer.h"
#include "../renderer.h"
#include "settings.h"

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
                fileStream maps("/proc/self/maps");

                // Read the contents of /proc/self/maps
                std::string maps_content = maps.read();
                
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
                    reportStack("Failed to find the stack area in /proc/self/maps!");
                }
            }

            if (Heap_Start_Address == nullptr){
                // Now also capture the nearest heap
                Heap_Start_Address = new int;
            }
        }
        #endif
    
    /**
         * @brief Extracts the directory path from a full executable path.
         *
         * This function takes a full path to an executable and returns the directory
         * portion of the path. It handles both Windows and Unix-style path separators.
         *
         * @param fullPath The full path to the executable as a C-style string.
         * @return A std::string containing the directory path. If the separator is not found,
         *         an empty string is returned.
         */
        std::string Get_Executable_Directory(const char* fullPath) {
            std::string path(fullPath);
        #if defined(_WIN32) || defined(_WIN64)
            const char separator = '\\';
        #else
            const char separator = '/';
        #endif
            size_t pos = path.find_last_of(separator);
            if (pos != std::string::npos) {
                return path.substr(0, pos); // Extract everything before the last separator
            }
            return ""; // Fallback if separator not found
        }

        #if defined(_WIN32) || defined(_WIN64)
        #include <windows.h>

        /**
         * @brief Retrieves the path of the executable file of the current process.
         * 
         * This function uses the GetModuleFileName function to obtain the full path 
         * of the executable file of the current process and returns it as a C-style string.
         * 
         * @return const char* A pointer to a null-terminated string that contains the 
         * path of the executable file. If the function fails, the string will be empty.
         */
        const char* Get_Executable_Path() {
            static char path[MAX_PATH];
            DWORD len = GetModuleFileName(nullptr, path, MAX_PATH);
            if (len == 0) {
                path[0] = '\0';
            }
            return path;
        }

        #elif defined(__linux__) || defined(__unix__) || defined(__APPLE__)
        #include <unistd.h>
        #include <limits.h>

        /**
         * @brief Retrieves the executable path of the current process.
         *
         * This function uses the `readlink` system call to obtain the path of the
         * executable file of the current process. The path is stored in a static
         * character array and returned as a C-string.
         *
         * @return A C-string containing the path of the executable file. If the
         *         path cannot be determined, an empty string is returned.
         */
        const char* Get_Executable_Path() {
            static char path[PATH_MAX];
            ssize_t len = readlink("/proc/self/exe", path, sizeof(path) - 1);
            if (len != -1) {
                path[len] = '\0';
            } else {
                path[0] = '\0';
            }
            return path;
        }

        #else
        /**
         * @brief Retrieves the path of the executable.
         * 
         * @return A constant character pointer to the executable path.
         */
        const char* Get_Executable_Path() {
            return "";
        }
        #endif

        /**
         * @brief Constructs the file name for the logger.
         *
         * This function constructs the file name for the logger by appending "/log.txt" 
         * to the directory path of the executable.
         *
         * @return A string representing the full path to the log file.
         */
        std::string constructLoggerFileName(){
            return GGUI::INTERNAL::Get_Executable_Directory(GGUI::INTERNAL::Get_Executable_Path()) + std::string("/log.txt");
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
        std::string hex(unsigned long long value) {
            char buffer[17]; // Enough to hold the largest 64-bit hexadecimal value + null terminator
            std::snprintf(buffer, sizeof(buffer), "%llX", value); // Formats the value as uppercase hex
            return std::string(buffer);
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
        bool collides(GGUI::element* a, GGUI::element* b, bool Identity) {
            if (a == b)
                return Identity;    // For custom purposes, defaults into true

            return collides(
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
        bool collides(GGUI::element* a, GGUI::IVector3 b) {
            if (!a) return false;   // Safe guard
            // Call the Collides function with the element's position and dimensions, and the point with assumed dimensions of 1x1.
            return collides(a->getAbsolutePosition(), b, a->getWidth(), a->getHeight(), 1, 1);
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
        element* getAccurateElementFrom(IVector3 c, element* Parent){
            //first check if the c is in bounds of Parent.
            if (!collides(Parent, c)){
                return nullptr;
            }

            // Check all the child elements of the parent.
            for (auto child : Parent->getChilds()){
                if (collides(child, c)){
                    // If a child element contains the position, search in the child element.
                    return getAccurateElementFrom(c, child);
                }
            }

            // If no child element contains the position, return the parent element.
            return Parent;
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
                float Distance = std::sqrt(std::pow(CC.x - start.x, 2) + std::pow(CC.y - start.y, 2));

                if (Distance < Shortest_Distance){
                    Shortest_Distance = Distance;
                    Best_Candidate = candidate;
                }
            }

            return Best_Candidate;
        }

        /**
         * @brief Gets the contents of a given position in the buffer.
         * @details This function takes a position in the buffer and returns the contents of that position. If the position is out of bounds, it will return nullptr.
         * @param Absolute_Position The position to get the contents of.
         * @return The contents of the given position, or nullptr if the position is out of bounds.
         */
        GGUI::UTF* get(GGUI::IVector3 Absolute_Position){
            if (Absolute_Position.x >= INTERNAL::getMaxWidth() || 
                Absolute_Position.y >= INTERNAL::getMaxHeight() ||
                Absolute_Position.x < 0 || 
                Absolute_Position.y < 0)
            {
                // The position is out of bounds, return nullptr
                return nullptr;
            }
            else{
                // The position is in bounds, return the contents of that position
                return &INTERNAL::abstractFrameBuffer->at(Absolute_Position.y * INTERNAL::getMaxWidth() + Absolute_Position.x);
            }
        }

        bool has(ALLOCATION_TYPE f, ALLOCATION_TYPE flag){
            return has((unsigned long long)f, (unsigned long long)flag);
        }

        bool contains(ALLOCATION_TYPE big, ALLOCATION_TYPE small){
            return contains((unsigned long long)big, (unsigned long long)small);
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
        ALLOCATION_TYPE getAllocationType(const void* ptr) {
            if (ptr == nullptr) {
                return ALLOCATION_TYPE::UNKNOWN; // Null pointer can't be valid
            }

            constexpr void* Start_Of_BSS = nullptr;  // Placeholder for BSS segment start

            constexpr int MiB = 0x100000;
            constexpr uintptr_t Somewhere_In_DATA = 100 * MiB;  // Arbitrary location in the data section

            // Check if ptr is above BSS, indicating potential data section location
            bool Ptr_Is_Above_BSS = ptr >= Start_Of_BSS;

            uintptr_t ptr_distance_to_DATA_section = abs((signed long long)ptr - (signed long long)Somewhere_In_DATA);

            // Calculate if ptr is within range of the data section
            bool Ptr_Is_In_Range_Of_DATA_Section = ptr_distance_to_DATA_section <= Somewhere_In_DATA;

            // Try to allocate memory on the heap for comparison
            size_t* new_heap = new(std::nothrow) size_t;
            if (new_heap == nullptr) {
                INTERNAL::reportStack("Failed to allocate new heap for stack pointer check!");
                exit(1);  // FATAL error if heap allocation fails
            }

            // Allocate an closeby stack variable
            char Local_Stack_Variable = 0;

            // Calculate distance from ptr to the stack start address
            uintptr_t ptr_distance_to_stack = Min(
                abs((signed long long)ptr - (signed long long)INTERNAL::Stack_Start_Address),
                abs((signed long long)ptr - (signed long long)&Local_Stack_Variable)
            );

            // Calculate distance from ptr to the closest heap address, heap grows accumulatively
            // uintptr_t heap_max_address = Max((uintptr_t)new_heap, (uintptr_t)INTERNAL::Heap_Start_Address);
            uintptr_t ptr_distance_to_heap = Min(
                abs((signed long long)ptr - (signed long long)new_heap),
                abs((signed long long)ptr - (signed long long)INTERNAL::Heap_Start_Address)
            );

            // Clean up the heap allocation
            delete new_heap;

            bool Points_To_DATA_Section = ((ptr_distance_to_DATA_section < ptr_distance_to_stack && ptr_distance_to_DATA_section < ptr_distance_to_heap) || Ptr_Is_Above_BSS) && Ptr_Is_In_Range_Of_DATA_Section;
            bool Points_To_Stack = ptr_distance_to_stack < ptr_distance_to_heap && ptr_distance_to_stack < ptr_distance_to_DATA_section;
            bool Points_To_Heap = ptr_distance_to_heap < ptr_distance_to_stack && ptr_distance_to_heap < ptr_distance_to_DATA_section;

            uintptr_t Result = 0;

            if (Points_To_Heap)
                Result |= (uintptr_t)ALLOCATION_TYPE::HEAP;
            if (Points_To_Stack)
                Result |= (uintptr_t)ALLOCATION_TYPE::STACK;
            if (Points_To_DATA_Section)
                Result |= (uintptr_t)ALLOCATION_TYPE::DATA;

            return (ALLOCATION_TYPE)Result;
        }

        GGUI::RGB lerp(GGUI::RGB A, GGUI::RGB B, float Distance) {
            if (SETTINGS::enableGammaCorrection) {
                A.red   = fast::interpolate(A.red,   B.red, Distance);
                A.green = fast::interpolate(A.green, B.green, Distance);
                A.blue  = fast::interpolate(A.blue,  B.blue, Distance);
            } else {
                // Fast integer-based linear interpolation on 8-bit channels
                A.red   = fast::interpolateLinearU8(A.red,   B.red, Distance);
                A.green = fast::interpolateLinearU8(A.green, B.green, Distance);
                A.blue  = fast::interpolateLinearU8(A.blue,  B.blue, Distance);
            }
            return A;
        }

    }

    std::string toString(UTF coloredText, bool transparentBackground) {
        return coloredText.toSuperString(transparentBackground)->toString();
    }
}