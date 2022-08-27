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
    inline std::string TOP_LEFT_CORNER = "\e(0\x6c\e(B";
    inline std::string BOTTOM_LEFT_CORNER = "\e(0\x6d\e(B";
    inline std::string TOP_RIGHT_CORNER = "\e(0\x6b\e(B";
    inline std::string BOTTOM_RIGHT_CORNER = "\e(0\x6a\e(B";
    inline std::string VERTICAL_LINE = "\e(0\x78\e(B";
    inline std::string HORIZONTAL_LINE = "\e(0\x71\e(B";
}

namespace Constants{
    inline void Init(){
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