#ifndef _LOGGER_H_
#define _LOGGER_H_

#include <queue>

#include "types.h"

namespace GGUI{

    class fileStream;

    // autoGen: Ignore start
    namespace INTERNAL{
        // Contains Logging utils.
        namespace LOGGER{
            // File handle for logging to files for Atomic access across different threads.
            extern atomic::guard<fileStream> handle;

            extern void init(fileStream* pre_pausedSelf = nullptr);

            extern void log(std::string Text);

            extern void registerCurrentThread();
        }
        
        extern void reportStack(const std::string& problemDescription);

        extern void loggerThread();
    }
    // autoGen: Ignore end
    
    /**
     * @brief Reports an error to the user.
     * @param Problem The error message to display.
     * @note If the main window is not created yet, the error will be printed to the console.
     * @note This function is thread safe.
     */
    extern void report(const std::string& problem);
}

#endif