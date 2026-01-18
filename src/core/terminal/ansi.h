#ifndef _ANSI_H_
#define _ANSI_H_

#include "../utils/types.h"
#include "terminal.h"

/**
 * Contains ANSI sequence implementation of input interface
*/

namespace GGUI {
    namespace terminal {
        namespace ansi {

            // If TTY enabled, then will check for ANSI support.
            INTERNAL::bitMask<feature> probe() {

            }

        }
    }
}

#endif