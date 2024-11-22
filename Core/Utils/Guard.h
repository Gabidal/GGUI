#ifndef _GUARD_H_
#define _GUARD_H_

#include <functional>
#include <mutex>
#include <string>

namespace GGUI{

    extern void Report_Stack(std::string Problem);

    namespace Atomic{

        // helper to make sure all objects created by this are always treated atomically
        template<typename T>
        class Guard{
        public:
            std::mutex Shared;      // this is shared across all other threads.
            T Data;

            Guard() = default;

            void operator()(std::function<void(T&)> job) {
                // check if the Shared mutex is already locked by higher/upper stack frame.
                if (Shared.try_lock()){
                    try{
                        job(Data);
                    } catch(...){
                        Report_Stack("Failed to execute the function!");
                    }

                    Shared.unlock();
                }
                else{
                    Report_Stack("Cannot double lock mutex");
                    return;
                }
            }
        };   
    }
}

#endif