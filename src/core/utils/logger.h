#ifndef _logger_H_
#define _logger_H_

#include "../thread.h"

#include <fstream>
#include <filesystem>

namespace GGUI{

    // Contains Logging utils.
    namespace logger{
        extern std::filesystem::path logFile;

        // File handle for logging to files for Atomic access across different threads.
        extern thread::guard<std::basic_ofstream<char>> handle;

        extern void log(const std::string& Text);
    
        extern void loggerThread();
    }
}

#endif