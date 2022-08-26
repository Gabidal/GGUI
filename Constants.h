#ifndef _CONSTANTS_H_
#define _CONSTANTS_H_

#include <string>

namespace COLOR{
    inline std::string RESET = "";
    inline std::string BLACK = "";
    inline std::string RED = "";
    inline std::string GREEN = "";
    inline std::string YELLOW = "";
    inline std::string BLUE = "";
    inline std::string MAGENTA = "";
    inline std::string CYAN = "";
    inline std::string WHITE = "";
}

namespace SYMBOLS{
    inline short TOP_LEFT_CORNER = 0x23BE;
    inline short BOTTOM_LEFT_CORNER = 0x23BF;
    inline short TOP_RIGHT_CORNER = 0x23CB;
    inline short BOTTOM_RIGHT_CORNER = 0x23CC;
    inline short LEFT = 0x23B8;
    inline short RIGHT = 0x23B9;
    inline short TOP = 0x23BA;
    inline short BOTTOM = 0x23BD;
}

namespace Constants{
    void Init(){
        #if _WIN32
            COLOR::RESET = "\033[0m";
            COLOR::BLACK = "\033[30m";
            COLOR::RED = "\033[31m";
            COLOR::GREEN = "\033[32m";
            COLOR::YELLOW = "\033[33m";
            COLOR::BLUE = "\033[34m";
            COLOR::MAGENTA = "\033[35m";
            COLOR::CYAN = "\033[36m";
            COLOR::WHITE = "\033[37m";
        #else
        #endif
    }
}

#endif