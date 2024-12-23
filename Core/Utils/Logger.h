#ifndef _LOGGER_H_
#define _LOGGER_H_

#include "Guard.h"

namespace GGUI{

    class FILE_STREAM;

    // Contains Logging utils.
    namespace LOGGER{
        // File handle for logging to files for Atomic access across different threads.
        extern Atomic::Guard<FILE_STREAM> Handle;

        extern void Init();

        extern void Log(std::string Text);

        extern void Close();
    };
}

#endif