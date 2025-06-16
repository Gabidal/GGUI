#ifndef _LOGGER_H_
#define _LOGGER_H_

#include <queue>

#include "types.h"

namespace GGUI{

    class fileStream;

    namespace INTERNAL{
        // Contains Logging utils.
        namespace LOGGER{
            // File handle for logging to files for Atomic access across different threads.
            extern atomic::Guard<fileStream> Handle;
            extern thread_local std::queue<std::string>* Queue;

            extern void Init();

            extern void Log(std::string Text);

            extern void RegisterCurrentThread();
        };
        
        extern void reportStack(const std::string& problemDescription);

        extern void loggerThread();
    }
    
    /**
     * @brief Reports an error to the user.
     * @param Problem The error message to display.
     * @note If the main window is not created yet, the error will be printed to the console.
     * @note This function is thread safe.
     */
    extern void report(const std::string& problem);
}

#endif