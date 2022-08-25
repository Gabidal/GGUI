#ifndef _CONSTANTS_H_
#define _CONSTANTS_H_

#include <string>

using namespace std;

namespace COLOR{

    inline string RESET = "";
    inline string BLACK = "";
    inline string RED = "";
    inline string GREEN = "";
    inline string YELLOW = "";
    inline string BLUE = "";
    inline string MAGENTA = "";
    inline string CYAN = "";
    inline string WHITE = "";

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