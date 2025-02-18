#include "settings.h"

namespace GGUI{
    namespace INTERNAL{
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
    }

    namespace SETTINGS{

        unsigned long long Mouse_Press_Down_Cooldown = 365;

        bool Word_Wrapping = true;

        std::chrono::milliseconds Thread_Timeout = std::chrono::milliseconds(256);

        bool ENABLE_GAMMA_CORRECTION = false;

        namespace LOGGER{
            std::string File_Name = "";
        }

        /**
         * @brief Initializes the settings for the application.
         *
         * This function sets up the necessary configurations for the application
         * by initializing the logger file name using the internal logger file name
         * construction method.
         */
        void initSettings(){
            LOGGER::File_Name = INTERNAL::constructLoggerFileName();
        }
    }
}