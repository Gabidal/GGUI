#ifndef _LOGGER_H_
#define _LOGGER_H_

#include "guard.h"

namespace GGUI{

    class fileStream;

    namespace INTERNAL{
        // Contains Logging utils.
        namespace LOGGER{
            // File handle for logging to files for Atomic access across different threads.
            extern atomic::Guard<fileStream> Handle;

            extern void Init();

            extern void Log(std::string Text);
        };
    }
}

#endif