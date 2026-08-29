#ifdef __linux__

#include "process.h"

#include <unistd.h>
#include <limits.h>

namespace GGUI {
    namespace process {

        std::filesystem::path getFilePath() {
            std::filesystem::path path;
            char buffer[PATH_MAX];
            ssize_t length = readlink("/proc/self/exe", buffer, sizeof(buffer) - 1);
            if (length != -1) {
                buffer[length] = '\0';  // Null-terminate the string
                path = std::filesystem::path(buffer);
            }

            // remove filename
            path = path.parent_path();

            return path;
        }

    }
}

#endif