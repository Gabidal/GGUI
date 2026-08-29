#ifdef _WIN32

#include "process.h"

#include <windows.h

namespace GGUI {
    namespace process {

        std::filesystem::path getFilePath() {
            char buffer[MAX_PATH];
            DWORD length = GetModuleFileNameA(NULL, buffer, MAX_PATH);
            if (length == 0) {
                // Handle error
                return std::filesystem::path();
            }
            return std::filesystem::path(buffer);
        }

    }
}

#endif