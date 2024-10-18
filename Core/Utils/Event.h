#ifndef _EVENT_H_
#define _EVENT_H_

#include <functional>
#include <chrono>

#include "Units.h"

namespace GGUI{
    
    class Event{
    public:
        unsigned long long Criteria;
    };

    class Input : public Event{
    public:
        char Data = 0;
        unsigned int X = 0;
        unsigned int Y = 0;
        int Scale = 1;

        // The input information like the character written.
        Input(char d, unsigned long long t){
            Data = d;
            Criteria = t;
        }

        Input(IVector3 c, unsigned long long t, int s = 1){
            X = c.X;
            Y = c.Y;
            Criteria = t;
            Scale = s;
        }
    };

    class Action : public Event{
    public:
        class Element* Host = nullptr;

        std::function<bool(GGUI::Event*)> Job;
        
        std::string ID; 
    
        Action() = default;
        Action(unsigned long long criteria, std::function<bool(GGUI::Event*)> job, std::string id = ""){
            Criteria = criteria;
            Job = job;
            Host = nullptr;
            ID = id;
        }

        Action(unsigned long long criteria, std::function<bool(GGUI::Event*)> job, class Element* host, std::string id = ""){
            Criteria = criteria;
            Job = job;
            Host = host;
            ID = id;
        }
    };

    namespace MEMORY_FLAGS{
        inline unsigned char PROLONG_MEMORY     = 1 << 0;
        inline unsigned char RETRIGGER          = 1 << 1;
    };

    class Memory : public Action{
    public:
        std::chrono::high_resolution_clock::time_point Start_Time;
        size_t End_Time = 0;

        // By default all memories automatically will not prolong each other similar memories.
        unsigned char Flags = 0x0;

        // When the job starts, job, prolong previous similar job by this time.
        Memory(size_t end, std::function<bool(GGUI::Event*)>job, unsigned char flags = 0x0, std::string id = ""){
            Start_Time = std::chrono::high_resolution_clock::now();
            End_Time = end;
            Job = job;
            Flags = flags;
            ID = id;
        }

        bool Is(unsigned char f){
            return (Flags & f) > 0;
        }

        void Set(unsigned char f){
            Flags |= f;
        }
    };

}

#endif