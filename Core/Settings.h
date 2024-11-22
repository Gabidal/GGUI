#ifndef _SETTINGS_H_
#define _SETTINGS_H_

#include <chrono>
#include <string>

namespace GGUI{
    namespace SETTINGS{
        // How fast for a detection of hold down situation.
        extern unsigned long long Mouse_Press_Down_Cooldown;
        
        extern bool Word_Wrapping;

        extern std::chrono::milliseconds Thread_Timeout;

        extern bool ENABLE_GAMMA_CORRECTION;

        namespace LOGGER{

            extern std::string File_Name;

        }

        extern void Init_Settings();
    };
}

#endif