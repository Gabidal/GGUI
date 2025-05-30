#include "logger.h"
#include "../settings.h"
#include "../renderer.h"

#include "../../elements/fileStreamer.h"
#include "utils.h"
#include "../renderer.h"

#include <functional>
#include <vector>
#include <string>

namespace GGUI{
    namespace SETTINGS{
        extern void initSettings();
    }

    namespace INTERNAL{
        namespace LOGGER{

            // File handle for logging to files for Atomic access across different threads.
            INTERNAL::atomic::Guard<fileStream> Handle;

            // to enable default to nullptr for Guard
            class queue{
            public:
                std::queue<std::string>* Handle;

                queue(bool init = false){
                    if (init)
                        Handle = new std::queue<std::string>();
                    else
                        Handle = nullptr;
                }

                void flushInto(queue& dest) {
                    while (!Handle->empty()) {
                        dest.Handle->push(std::move(Handle->front()));
                        Handle->pop();
                    }
                }
            };
            
            typedef INTERNAL::atomic::Guard<queue> guardedQueue;    // for tidying
            thread_local guardedQueue* localQueue = new guardedQueue();
            INTERNAL::atomic::Guard<std::vector<guardedQueue*>> AllQueues;

            /**
             * @brief Initializes the logger file stream if it is uninitialized.
             *
             * This function ensures that the logger's file stream is properly set up before use.
             * If the file stream is currently uninitialized, it checks if the logger file name is set.
             * If not, it initializes the logger settings. Then, it constructs a new fileStream object
             * for writing logs, using the configured file name.
             *
             * This function is safe to call multiple times and will only initialize the file stream
             * if it has not already been initialized.
             */
            void Init(){
                Handle([](GGUI::fileStream& self){
                    if (self.Get_type() == FILE_STREAM_TYPE::UN_INITIALIZED){    // If the Log is called before GGUI_Init, then we need to skip this in GGUI_Init
                        if (SETTINGS::LOGGER::File_Name.size() == 0){
                            SETTINGS::initSettings();
                        }
                        new (&self) fileStream(SETTINGS::LOGGER::File_Name, [](){}, FILE_STREAM_TYPE::WRITE, true);
                    }
                });
            }

            /**
             * @brief Logs a message to the log file, prepending a timestamp.
             *
             * This function formats the input text by adding the current timestamp at the beginning.
             * For multi-line messages, it indents subsequent lines to align with the timestamp.
             * If the log file stream is not initialized, it initializes it before logging.
             *
             * @param Text The message to log. Multi-line messages will be indented for readability.
             */
            void Log(std::string Text){
                Handle([&Text](GGUI::fileStream& self){
                    // If this log is called before the normal GGUI_Init is called, then we need to manually init this.
                    if (self.Get_type() == FILE_STREAM_TYPE::UN_INITIALIZED)
                        Init();

                    // Get the current time as string
                    std::string now = "[" + GGUI::INTERNAL::now() + "]: ";

                    // When the Stack_Traced report is given we need to push all the following lines after the first line by the length of the now string.
                    for (unsigned int i = 0; i < Text.size(); i++){
                        if (Text[i] == '\n'){
                            // now we can add before the newline spaces by the amount of the length of the now time.
                            for (unsigned int j = 0; j < now.size(); j++)
                                Text.insert(Text.begin() + i + 1, ' ');
                        }
                    }

                    // Write the time and the text into the log file.
                    self.Append(now + Text);
                });
            }
        
            /**
             * @brief Registers the current thread by creating a local message queue if it doesn't exist.
             *
             * This function checks if the thread-local message queue (`localQueue`) is null.
             * If so, it allocates a new queue for the current thread and registers it with the
             * global collection of all queues via the `AllQueues` function.
             * This allows the logging system to keep track of all thread-local queues for message processing.
             */
            void RegisterCurrentThread(){
                (*localQueue)([](queue& current){
                    if (current.Handle != nullptr)
                        return;

                    current.Handle = new std::queue<std::string>();
                });

                AllQueues([](std::vector<guardedQueue*>& queues){
                    queues.push_back(localQueue);
                });
            }

        }
    
    // For printing stacktrace
    #if _WIN32
    
        #include <windows.h>
        #include <DbgHelp.h>

        #ifndef GGUI_RELEASE


        /**
         * @brief Retrieves the module handle corresponding to a specific address.
         *
         * This function queries the loaded module that contains the provided memory address.
         * It uses the `GetModuleHandleEx` API with `GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS`.
         *
         * @param address The memory address to resolve.
         * @return HMODULE The handle to the module containing the address, or NULL if not found.
         */
        HMODULE GetModuleFromAddress(void* address) {
            HMODULE moduleHandle = NULL;
            if (!GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                                static_cast<LPCTSTR>(address), &moduleHandle)) {
                return NULL;
            }
            return moduleHandle;
        }

        /**
         * @brief Resolves the export symbol name from a module's export table for a given address.
         *
         * This function performs the following:
         * - Identifies the module that contains the provided address.
         * - Parses the module's PE headers and export directory.
         * - Matches the address to an exported function range.
         * - Returns the corresponding exported function name or ordinal.
         *
         * @param address A pointer to the code address to resolve.
         * @return std::string The resolved function name or ordinal, or an empty string if not found.
         */
        std::string ResolveSymbolFromExportTable(void* address) {
            HMODULE moduleHandle = GetModuleFromAddress(address);
            if (!moduleHandle) {
                return "";
            }

            BYTE* moduleBase = reinterpret_cast<BYTE*>(moduleHandle);
            PIMAGE_DOS_HEADER dosHeader = reinterpret_cast<PIMAGE_DOS_HEADER>(moduleBase);

            if (dosHeader->e_magic != IMAGE_DOS_SIGNATURE) {
                return "";
            }

            PIMAGE_NT_HEADERS ntHeaders = reinterpret_cast<PIMAGE_NT_HEADERS>(moduleBase + dosHeader->e_lfanew);
            if (ntHeaders->Signature != IMAGE_NT_SIGNATURE) {
                return "";
            }

            IMAGE_DATA_DIRECTORY& exportDirectoryEntry =
                ntHeaders->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT];

            if (exportDirectoryEntry.Size == 0) {
                return "";
            }

            PIMAGE_EXPORT_DIRECTORY exportDirectory =
                reinterpret_cast<PIMAGE_EXPORT_DIRECTORY>(moduleBase + exportDirectoryEntry.VirtualAddress);

            DWORD* functionAddresses = reinterpret_cast<DWORD*>(moduleBase + exportDirectory->AddressOfFunctions);
            DWORD* functionNames = reinterpret_cast<DWORD*>(moduleBase + exportDirectory->AddressOfNames);
            WORD* nameOrdinals = reinterpret_cast<WORD*>(moduleBase + exportDirectory->AddressOfNameOrdinals);

            // Pair of <function virtual address, original ordinal index>
            std::vector<std::pair<BYTE*, DWORD>> exportFunctionEntries;
            for (DWORD i = 0; i < exportDirectory->NumberOfFunctions; ++i) {
                DWORD functionRelativeAddress = functionAddresses[i];
                if (functionRelativeAddress == 0) {
                    continue; // Skip forwarded or null exports
                }
                BYTE* functionAbsoluteAddress = moduleBase + functionRelativeAddress;
                exportFunctionEntries.emplace_back(functionAbsoluteAddress, i);
            }

            // Sort exported functions by address for efficient range search
            std::sort(exportFunctionEntries.begin(), exportFunctionEntries.end(),
                    [](const auto& a, const auto& b) {
                        return a.first < b.first;
                    });

            // Find the address range the input address falls into
            for (size_t i = 0; i < exportFunctionEntries.size(); ++i) {
                BYTE* functionStartAddress = exportFunctionEntries[i].first;
                DWORD functionOrdinalIndex = exportFunctionEntries[i].second;

                // Determine the end of this function's code by checking the next function address or section boundary
                BYTE* functionEndAddress = nullptr;
                if (i + 1 < exportFunctionEntries.size()) {
                    functionEndAddress = exportFunctionEntries[i + 1].first;
                } else {
                    // Locate the section to determine the upper bound
                    DWORD relativeAddress = static_cast<DWORD>(functionStartAddress - moduleBase);
                    PIMAGE_SECTION_HEADER sectionHeader = IMAGE_FIRST_SECTION(ntHeaders);
                    for (unsigned int j = 0; j < ntHeaders->FileHeader.NumberOfSections; ++j, ++sectionHeader) {
                        DWORD sectionStart = sectionHeader->VirtualAddress;
                        DWORD sectionEnd = sectionStart + sectionHeader->Misc.VirtualSize;
                        if (relativeAddress >= sectionStart && relativeAddress < sectionEnd) {
                            functionEndAddress = moduleBase + sectionEnd;
                            break;
                        }
                    }
                    // Fallback range if section not identified
                    if (!functionEndAddress) {
                        functionEndAddress = functionStartAddress + 0x1000;
                    }
                }

                // Check if the input address lies within the current function's range
                if (reinterpret_cast<BYTE*>(address) >= functionStartAddress &&
                    reinterpret_cast<BYTE*>(address) < functionEndAddress) {

                    // Try to find the function's name using its ordinal
                    for (DWORD j = 0; j < exportDirectory->NumberOfNames; ++j) {
                        if (nameOrdinals[j] == functionOrdinalIndex) {
                            const char* functionName = reinterpret_cast<const char*>(moduleBase + functionNames[j]);
                            return std::string(functionName);
                        }
                    }

                    // If no name found, fall back to ordinal
                    DWORD exportedOrdinal = functionOrdinalIndex + exportDirectory->Base;
                    return "ExportedFunc_Ordinal_" + std::to_string(exportedOrdinal);
                }
            }

            return ""; // No match found in the export table
        }

        /**
         * @brief Performs basic demangling of Itanium C++ ABI mangled symbols.
         *
         * This function provides a simplified demangler for common mangled symbols generated by GCC and Clang
         * following the Itanium C++ ABI. It supports:
         * - Parsing simple and nested names
         * - Basic support for template argument lists
         *
         * This is *not* a full demangler and will not cover all edge cases or types. For production-grade use,
         * prefer tools like `abi::__cxa_demangle` or third-party demangling libraries.
         *
         * @param mangledSymbol A mangled C++ symbol following the Itanium ABI (typically starting with "_Z").
         * @return std::string A best-effort demangled version of the symbol.
         */
        std::string SimpleDemangle(const std::string& mangledSymbol) {
            const char* cursor = mangledSymbol.c_str();
            const char* end = cursor + mangledSymbol.size();

            // Helper function to parse a decimal number (used for name lengths)
            auto parseLength = [&]() -> int {
                int value = 0;
                if (!std::isdigit(*cursor)) return -1;

                while (cursor < end && std::isdigit(*cursor)) {
                    value = value * 10 + (*cursor - '0');
                    ++cursor;
                }
                return value;
            };

            // Helper function to parse a single name (length-prefixed)
            auto parseName = [&]() -> std::string {
                int length = parseLength();
                if (length < 0 || cursor + length > end) return "";

                std::string name(cursor, cursor + length);
                cursor += length;
                return name;
            };

            // Helper function to parse a nested name: N<names>E
            // Example: N3Foo3BarE -> Foo::Bar
            auto parseNestedName = [&]() -> std::string {
                if (*cursor != 'N') return "";
                ++cursor; // consume 'N'

                std::string fullName;
                while (cursor < end && *cursor != 'E') {
                    std::string component = parseName();
                    if (component.empty()) break;

                    if (!fullName.empty()) {
                        fullName += "::";
                    }
                    fullName += component;
                }

                if (*cursor == 'E') {
                    ++cursor; // consume 'E'
                }

                return fullName;
            };

            // Helper function to parse template arguments enclosed in I...E
            auto parseTemplateArguments = [&]() -> std::string {
                if (*cursor != 'I') return "";
                ++cursor; // consume 'I'

                std::string result = "<";
                bool firstArgument = true;

                while (cursor < end && *cursor != 'E') {
                    if (!firstArgument) {
                        result += ", ";
                    }
                    firstArgument = false;

                    if (*cursor == 'N') {
                        std::string nested = parseNestedName();
                        if (nested.empty()) break;
                        result += nested;
                    } else if (std::isdigit(*cursor)) {
                        std::string name = parseName();
                        if (name.empty()) break;
                        result += name;
                    } else {
                        // Fallback for unknown types or literals; consume single character
                        result += *cursor;
                        ++cursor;
                    }
                }

                if (*cursor == 'E') {
                    ++cursor; // consume 'E'
                }

                result += ">";
                return result;
            };

            // Begin demangling logic

            // All Itanium ABI mangled names start with "_Z"
            if (mangledSymbol.size() < 2 || mangledSymbol[0] != '_' || mangledSymbol[1] != 'Z') {
                return mangledSymbol; // Not a mangled name
            }

            cursor += 2; // skip "_Z"
            std::string demangled;

            if (*cursor == 'N') {
                // Handle nested names
                demangled = parseNestedName();

                if (cursor < end && *cursor == 'I') {
                    // Template arguments may follow the last name component
                    std::string templateArgs = parseTemplateArguments();

                    // Attach template args to last component
                    size_t lastSeparator = demangled.rfind("::");
                    if (lastSeparator == std::string::npos) {
                        demangled += templateArgs;
                    } else {
                        demangled.insert(lastSeparator + 2 + (demangled.size() - lastSeparator - 2), templateArgs);
                    }
                }
            } else if (std::isdigit(*cursor)) {
                // Single name without nesting
                demangled = parseName();

                if (cursor < end && *cursor == 'I') {
                    demangled += parseTemplateArguments();
                }
            } else {
                // Fallback for unrecognized or unsupported patterns
                demangled = mangledSymbol;
            }

            return demangled;
        }

        bool InitSymbolHandler() {
            static bool initialized = false;
            static HANDLE process = GetCurrentProcess();

            if (!initialized) {
                // Set symbol options to improve resolution and enable demangling
                SymSetOptions(SymGetOptions() | SYMOPT_LOAD_LINES | SYMOPT_UNDNAME | SYMOPT_DEFERRED_LOADS);
                
                if (SymInitialize(process, nullptr, TRUE)) {
                    initialized = true;
                } else {
                    return false; // Initialization failed
                }
            }

            return true;
        }

        #endif

        /**
         * @brief Captures and reports a simplified symbolic stack trace with demangled symbol names.
         *
         * This function uses Windows APIs to capture the current thread's call stack,
         * resolves each address to a symbol name (via the export table), attempts to demangle
         * the name (assuming Itanium ABI-style mangling), and formats the result into a visual stack tree.
         *
         * @param problemDescription A textual description of the issue related to the stack trace.
         */
        void reportStack(const std::string& problemDescription) {
            std::string result = "";
            
        #ifndef GGUI_RELEASE
            constexpr int MaximumStackDepth = 10;
            void* stackAddressTable[MaximumStackDepth] = {};
            int capturedFrameCount = 0;

            // Initialize DbgHelp symbol handler for the current process
            if (!InitSymbolHandler()) {
                LOGGER::Log("Error: Failed to initialize symbol handler.");
                return;
            }

            // Capture the current call stack
            capturedFrameCount = CaptureStackBackTrace(0, MaximumStackDepth, stackAddressTable, nullptr);

            // Allocate SYMBOL_INFO structure with additional space for symbol name
            SYMBOL_INFO* symbolInfo = reinterpret_cast<SYMBOL_INFO*>(
                calloc(1, sizeof(SYMBOL_INFO) + 256 * sizeof(char))
            );

            if (!symbolInfo) {
                LOGGER::Log("Error: Memory allocation for SYMBOL_INFO failed.");
                return;
            }

            symbolInfo->MaxNameLen = 255;
            symbolInfo->SizeOfStruct = sizeof(SYMBOL_INFO);

            // Ensure any global visual layout constants are updated
            if (Max_Width == 0) {
                updateMaxWidthAndHeight(); // Presumed external function
            }

            // Start assembling the final report
            std::string formattedStackTrace = "Stack Trace:\n";
            int visualDepthIndex = 0;

            const bool enableIndentation = capturedFrameCount < (Max((signed)Max_Width, 0) / 2);

            // Traverse stack frames in reverse (from newest to oldest)
            for (int frameIndex = capturedFrameCount - 1; frameIndex > 0; --frameIndex) {
                void* currentAddress = stackAddressTable[frameIndex];
                if (currentAddress == nullptr) {
                    continue;
                }

                // Draw the tree-style branch symbol (main is '|' end is '\')
                std::string branchSymbol = (frameIndex == 1) ? "\\" : "|";

                // Construct indentation based on depth
                std::string indentation;
                if (enableIndentation) {
                    indentation.assign(visualDepthIndex, '-');
                }

                // Attempt to resolve and demangle the symbol for the current address
                std::string symbolName = ResolveSymbolFromExportTable(currentAddress);
                std::string readableName = SimpleDemangle(symbolName);

                if (readableName.empty()) {
                    continue;
                }

                formattedStackTrace += branchSymbol + indentation + " " + readableName + "\n";
                ++visualDepthIndex;
            }

            // Clean up symbol information memory
            free(symbolInfo);

            // Append description of the triggering problem
            result = formattedStackTrace + "Problem: " + problemDescription;
        #else
            result = problemDescription;
        #endif
            // Submit the final formatted report
            report(result);
        }


    #else
        
        #ifndef GGUI_RELEASE

        #include <execinfo.h>   // For stacktrace
        #include <dlfcn.h>      // For stacktrace
        #include <cxxabi.h>     // For stacktrace
        #include <elf.h>        // For stacktrace
        #include <fcntl.h>      // For stacktrace
        #include <link.h>       // For stacktrace
        #include <sys/mman.h>   // For stacktrace
        #include <sys/stat.h>   // For stacktrace

        /**
         * Resolves the symbol name corresponding to a given instruction address by inspecting the
         * ELF symbol table of the shared object or binary to which the address belongs.
         * 
         * This implementation:
         * - Works without third-party libraries
         * - Operates only on ELF64 (x86_64) binaries
         * - Uses .symtab and .strtab for symbol resolution
         * - Requires non-stripped binaries to find full symbol names
         * 
         * @param address Pointer to the instruction address to resolve.
         * @return A demangled symbol name string if found, or an empty string otherwise.
         */
        std::string ResolveSymbolFromAddress(void* address) {
            Dl_info dynamicLinkInfo;

            // Query the base module and symbol information from the address
            if (!dladdr(address, &dynamicLinkInfo) || !dynamicLinkInfo.dli_fname || !dynamicLinkInfo.dli_fbase) {
                return "";
            }

            const char* moduleFilePath = dynamicLinkInfo.dli_fname;
            uintptr_t moduleBaseAddress = reinterpret_cast<uintptr_t>(dynamicLinkInfo.dli_fbase);
            uintptr_t targetAddress = reinterpret_cast<uintptr_t>(address);

            // Open the shared object or executable for reading
            int fileDescriptor = open(moduleFilePath, O_RDONLY);
            if (fileDescriptor < 0) {
                return "";
            }

            // Retrieve file size for mmap
            struct stat fileStatus;
            if (fstat(fileDescriptor, &fileStatus) < 0) {
                close(fileDescriptor);
                return "";
            }

            // Memory-map the ELF file for reading
            void* mappedElfFile = mmap(nullptr, fileStatus.st_size, PROT_READ, MAP_PRIVATE, fileDescriptor, 0);
            close(fileDescriptor);
            if (mappedElfFile == MAP_FAILED) {
                return "";
            }

            // Verify the ELF header
            Elf64_Ehdr* elfHeader = reinterpret_cast<Elf64_Ehdr*>(mappedElfFile);
            if (memcmp(elfHeader->e_ident, ELFMAG, SELFMAG) != 0) {
                munmap(mappedElfFile, fileStatus.st_size);
                return "";
            }

            // Locate section headers
            Elf64_Shdr* sectionHeaders = reinterpret_cast<Elf64_Shdr*>(
                reinterpret_cast<char*>(mappedElfFile) + elfHeader->e_shoff
            );

            Elf64_Shdr* symbolTableSection = nullptr;
            Elf64_Shdr* stringTableSection = nullptr;

            // Iterate over all sections to find the .symtab and its associated string table (.strtab)
            for (int sectionIndex = 0; sectionIndex < elfHeader->e_shnum; ++sectionIndex) {
                if (sectionHeaders[sectionIndex].sh_type == SHT_SYMTAB) {
                    symbolTableSection = &sectionHeaders[sectionIndex];
                    stringTableSection = &sectionHeaders[symbolTableSection->sh_link];
                    break;
                }
            }

            // Ensure both sections are present
            if (!symbolTableSection || !stringTableSection) {
                munmap(mappedElfFile, fileStatus.st_size);
                return "";
            }

            const char* stringTableData = reinterpret_cast<const char*>(
                reinterpret_cast<const char*>(mappedElfFile) + stringTableSection->sh_offset
            );

            Elf64_Sym* symbolEntries = reinterpret_cast<Elf64_Sym*>(
                reinterpret_cast<char*>(mappedElfFile) + symbolTableSection->sh_offset
            );

            size_t totalSymbols = symbolTableSection->sh_size / sizeof(Elf64_Sym);

            std::string closestSymbolName;
            uintptr_t smallestOffset = static_cast<uintptr_t>(-1);

            // Iterate through all function symbols and find the closest match not exceeding the address
            for (size_t i = 0; i < totalSymbols; ++i) {
                const Elf64_Sym& symbol = symbolEntries[i];

                // Skip undefined or non-function symbols
                if (ELF64_ST_TYPE(symbol.st_info) != STT_FUNC || symbol.st_shndx == SHN_UNDEF) {
                    continue;
                }

                uintptr_t symbolAbsoluteAddress = moduleBaseAddress + symbol.st_value;

                if (targetAddress >= symbolAbsoluteAddress) {
                    uintptr_t offset = targetAddress - symbolAbsoluteAddress;
                    if (offset < smallestOffset) {
                        smallestOffset = offset;
                        closestSymbolName = std::string(stringTableData + symbol.st_name);
                    }
                }
            }

            // Clean up the memory mapping
            munmap(mappedElfFile, fileStatus.st_size);

            return closestSymbolName;
        }

        #endif

        /**
         * @brief Captures and reports a formatted stack trace along with a provided error description.
         *
         * This function performs the following:
         * - Captures the current call stack up to a fixed depth.
         * - Resolves each instruction address to a function symbol using ELF symbol tables.
         * - Demangles C++ function names if applicable.
         * - Formats the stack trace into a human-readable, indented tree-like structure.
         * - Appends the original error description to the trace.
         * - Sends the result to a reporting system.
         *
         * Platform: Only supported on non-Android POSIX-compliant systems.
         *
         * @param problemDescription A descriptive message about the problem being reported.
         */
        void reportStack(const std::string& problemDescription) {
        #ifndef __ANDROID__ and GGUI_RELEASE
            constexpr int MaximumStackDepth = 10;
            void* callStackAddresses[MaximumStackDepth];

            // Capture up to MaximumStackDepth stack frames and store them in callStackAddresses
            int capturedFrameCount = backtrace(callStackAddresses, MaximumStackDepth);

            // Attempt to generate human-readable strings for each frame address (optional backup, not used directly)
            char** symbolNames = backtrace_symbols(callStackAddresses, capturedFrameCount);
            if (!symbolNames) {
                report("Error: Failed to retrieve stack trace symbols. Problem: " + problemDescription);
                return;
            }

            // Ensure UI constraints are initialized before printing
            if (Max_Width == 0) {
                updateMaxWidthAndHeight();
            }

            std::string formattedTrace = "Stack Trace:\n";
            bool useIndentation = static_cast<unsigned int>(capturedFrameCount) < (Max_Width / 2);
            int currentIndentLevel = 0;

            // Iterate backwards through the captured frames, omitting the frame that called reportStack
            for (int frameIndex = capturedFrameCount - 1; frameIndex > 0; --frameIndex) {
                std::string linePrefix = (frameIndex == 1) ? "\\" : "|";
                std::string indentation;

                // Add visual indentation for tree structure
                if (useIndentation) {
                    indentation = std::string(currentIndentLevel, '-');
                }

                std::string resolvedSymbol = ResolveSymbolFromAddress(callStackAddresses[frameIndex]);
                if (resolvedSymbol.empty()) {
                    continue; // Skip unresolved frames
                }

                // Attempt to demangle the symbol for improved readability
                int demangleStatus = 0;
                char* demangledName = abi::__cxa_demangle(resolvedSymbol.c_str(), nullptr, nullptr, &demangleStatus);
                std::string functionName = (demangleStatus == 0 && demangledName) ? demangledName : resolvedSymbol;

                // Append to the trace output
                formattedTrace += linePrefix + indentation + " " + functionName + "\n";

                if (demangledName) {
                    std::free(demangledName);
                }

                ++currentIndentLevel;
            }

            // Clean up symbol name memory allocated by backtrace_symbols
            std::free(symbolNames);

            // Attach the problem description to the trace
            formattedTrace += "Problem: " + problemDescription;

            // Output the full report
            report(formattedTrace);
        #else
            // If the platform is unsupported (e.g., Android), report the problem without stack trace
            report(problemDescription);
        #endif
        }
    #endif
    
        /**
         * @brief Reports an error to the user.
         * @param Problem The error message to display.
         * @note If the main window is not created yet, the error will be printed to the console.
         * @note This function is thread safe.
         */
        void renderLogger(const std::string& problem){
            const char* ERROR_LOGGER = "_ERROR_LOGGER_";
            const char* HISTORY = "_HISTORY_";
            try{
                pauseGGUI([&problem, &ERROR_LOGGER, &HISTORY]{
                    INTERNAL::LOGGER::Log(problem);

                    // reportStack is called when the height or width is zero at init, so we dont ned to compute further.
                    if (INTERNAL::Max_Height == 0 || INTERNAL::Max_Width == 0){
                        return;
                    }

                    std::string Problem = " " + problem + " ";

                    // Error logger structure:
                    /*
                        <Window name="_ERROR_LOGGER_">
                            <List name="_HISTORY_" type=vertical scrollable=true>
                                <List type="horizontal">
                                    <TextField>Time</TextField>
                                    <TextField>Problem a</TextField>
                                    <TextField>[repetitions if any]</TextField>
                                </List>
                                ...
                            </List>
                        </Window>
                    */

                    if (INTERNAL::Main && (INTERNAL::Max_Width != 0 && INTERNAL::Max_Height != 0)){
                        bool Create_New_Line = true;

                        // First check if there already is a report log.
                        window* Error_Logger = (window*)INTERNAL::Main->getElement(ERROR_LOGGER);

                        if (Error_Logger){
                            // Get the list
                            scrollView* History = (scrollView*)Error_Logger->getElement(HISTORY);

                            // This happens, when Error logger is kidnapped!
                            if (!History){
                                // Now create the history lister
                                History = new scrollView(styling(
                                    width(1.0f) | height(1.0f) |
                                    text_color(GGUI::COLOR::RED) | background_color(GGUI::COLOR::BLACK) | 
                                    flow_priority(DIRECTION::COLUMN) | name(HISTORY)
                                ));

                                Error_Logger->addChild(History);
                            }

                            std::vector<listView*>& Rows = (std::vector<listView*>&)(History->getContainer()->getChilds()); 

                            if (Rows.size() > 0){
                                //Text_Field* Previous_Date = Rows.back()->Get<Text_Field>(0);
                                textField* Previous_Problem = Rows.back()->get<textField>(0);
                                textField* Previous_Repetitions = Rows.back()->get<textField>(1);

                                //check if the previous problem was same problem
                                if (Previous_Problem->getText() == Problem){
                                    // increase the repetition count by one
                                    if (!Previous_Repetitions){
                                        Previous_Repetitions = new textField(styling(text("2")));
                                        Rows.back()->addChild(Previous_Repetitions);
                                    }
                                    else{
                                        // translate the string to int
                                        int Repetition = std::stoi(Previous_Repetitions->getText()) + 1;
                                        Previous_Repetitions->setText(std::to_string(Repetition));
                                    }

                                    // We dont need to create a new line.
                                    Create_New_Line = false;
                                }
                            }
                        }
                        else{
                            // create the error logger
                            Error_Logger = new window(
                                styling(
                                    width(0.25f) | height(0.5f) |

                                    text_color(GGUI::COLOR::RED) | background_color(GGUI::COLOR::BLACK) |
                                    border_color(GGUI::COLOR::RED) | border_background_color(GGUI::COLOR::BLACK) | 

                                    title("LOG") | name(ERROR_LOGGER) | 
                                    
                                    position(
                                        STYLES::center + STYLES::prioritize
                                    ) | 
                                    
                                    enable_border(true) | allow_overflow(true) | 

                                    node(new scrollView(styling(
                                        width(1.0f) | height(1.0f) |
                                        text_color(GGUI::COLOR::RED) | background_color(GGUI::COLOR::BLACK) | 
                                        flow_priority(DIRECTION::COLUMN) | name(HISTORY)
                                    )))
                                )
                            );

                            INTERNAL::Main->addChild(Error_Logger);
                        }

                        if (Create_New_Line){
                            // re-find the error_logger.
                            Error_Logger = (window*)INTERNAL::Main->getElement(ERROR_LOGGER);
                            scrollView* History = (scrollView*)Error_Logger->getElement(HISTORY);

                            History->addChild(new listView(styling(
                                text_color(GGUI::COLOR::RED) | background_color(GGUI::COLOR::BLACK) | 
                                flow_priority(DIRECTION::ROW) | 

                                name(INTERNAL::now().c_str()) | 

                                // The actual reported problem text
                                node(new textField(styling(
                                    text(Problem.c_str())
                                ))) 
                                // |

                                // // The Date field
                                // node(new textField(styling(
                                //     text(INTERNAL::now().c_str())
                                // )))
                            )));

                            // listView* row = (listView*)History->getContainer()->getChilds().back();
                            // // row->setHeight(row->getChilds()[1]->getHeight());
                            // row->setHeight(row->getChilds()[0]->getHeight());

                            // Calculate the new x position for the Error_Logger
                            if (Error_Logger->getParent() == INTERNAL::Main)
                                Error_Logger->setPosition({
                                    (Error_Logger->getParent()->getWidth() - History->getWidth()) / 2,
                                    (Error_Logger->getParent()->getHeight() - History->getHeight()) / 2,
                                    POSITION::Max_Z
                                });

                            // check if the Current rows amount makes the list new rows un-visible because of the of-limits.
                            // We can assume that the singular error is at least one tall.
                            // -1, since the border takes one.
                            if (GGUI::Min(History->getContainer()->getHeight(), (int)History->getContainer()->getChilds().size()) >= Error_Logger->getHeight() - 1){
                                // Since the children are added asynchronously, we can assume the the order of childs list vector represents the actual visual childs.
                                // Element* First_Child = History->Get_Childs()[0];
                                // History->Remove(First_Child);

                                // TODO: Make this into a scroll action and not a remove action, since we want to see the previous errors :)
                                History->scrollDown();
                            }
                        }

                        // If the user has disabled the Inspect_Tool then the errors appear as an popup window ,which disappears after 30s.
                        if (Error_Logger->getParent() == INTERNAL::Main){
                            Error_Logger->display(true);

                            INTERNAL::Remember([Error_Logger](std::vector<Memory>& rememberable){
                                rememberable.push_back(Memory(
                                    TIME::SECOND * 30,
                                    [Error_Logger](GGUI::Event*){
                                        //delete tmp;
                                        Error_Logger->display(false);
                                        //job successfully done
                                        return true;
                                    },
                                    MEMORY_FLAGS::PROLONG_MEMORY,
                                    "Report Logger Clearer"
                                ));
                            });
                        }

                    }
                    else{
                        if (!INTERNAL::Platform_Initialized){
                            INTERNAL::initPlatformStuff();
                        }

                        // This is for the non GGUI space errors.
                        UTF _error__tmp_ = UTF("ERROR: ", {COLOR::RED, {}});

                        std::cout << _error__tmp_.To_String() + Problem << std::endl;
                    }

                });
            }
            catch (std::exception& e){
                try{
                    // First don't give up on local logging yet
                    INTERNAL::LOGGER::Log("Problem: " + std::string(e.what()));
                }
                catch (std::exception& f){
                    // If logger is also down
                    std::cout << "Problem: " << e.what() << std::endl;
                }
            }
        }

        void loggerThread(){
            while (true){
                LOGGER::queue LinearQueue(true);

                // First we'll quickly do a checkup into the AllQueues and look if there is anything to log.
                LOGGER::AllQueues([&LinearQueue](std::vector<LOGGER::guardedQueue*>& self){
                    for (auto* queue : self){
                        // Pause the current queue
                        (*queue)([&LinearQueue](LOGGER::queue& self){
                            self.flushInto(LinearQueue);
                        });
                    }
                });
                
                while (!LinearQueue.Handle->empty()){
                    std::string CurrentProblem = LinearQueue.Handle->front();
                    LinearQueue.Handle->pop();

                    renderLogger(CurrentProblem);
                }

                INTERNAL::SLEEP(100);
            }
        }

    }

    void report(const std::string& problem){
        (*INTERNAL::LOGGER::localQueue)([&problem](INTERNAL::LOGGER::queue& self){
            if (self.Handle){
                self.Handle->push(problem); // Push the problem to the local queue.
            }else{
                INTERNAL::LOGGER::Log("Unregistered thread: " + std::to_string(std::hash<std::thread::id>{}(std::this_thread::get_id())) + " reported: " + problem);
            }
        });
    }

}