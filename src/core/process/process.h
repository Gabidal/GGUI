#ifndef _PROCESS_H_
#define _PROCESS_H_

#include <filesystem>

namespace GGUI {

    // Contains cross-platform information about the environment this process was spawned in
    namespace process {
        extern std::filesystem::path getFilePath();
    }
}

#endif