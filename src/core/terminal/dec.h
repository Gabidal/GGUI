#ifndef _DEC_H_
#define _DEC_H_

#include "../utils/types.h"
#include "terminal.h"

/**
 * Contains the DEC VTxxx input implementation of input interface and output sequencing
*/

namespace GGUI {
    namespace terminal {
        namespace dec {
        
            // If TTY enabled, then will check for ANSI support.
            INTERNAL::bitMask<type> probe() {

            }

        }
    }
}

#endif