#ifndef _GUARD_H_
#define _GUARD_H_

#include <functional>
#include <mutex>
#include <string>

namespace GGUI{

    extern void Report_Stack(std::string Problem);

    namespace Atomic{

        // Helper to make sure all objects created by this are always treated atomically
        template<typename T>
        class Guard{
        public:
            std::mutex Shared;          // This is shared across all other threads.
            T* Data = nullptr;

            Guard(): Data(new T()) {}   // Call the default constructor of the data type.

            void operator()(std::function<void(T*)> job) {
                // Check if the Shared mutex is already locked by higher/upper stack frame.
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

            /**
             * @brief Reads the value of the guarded object.
             *
             * This function acquires the lock and reads the value of the guarded object.
             * It uses a lambda function to set the result to the value of the guarded object.
             *
             * @return T* Pointer to the guarded object.
             */
            T* Read(){
                T* result = nullptr;
                (*this)([&result](T* self){
                    result = self;
                });
                return result;
            }

            // TODO: enable this code when switched T into T*
            // Automatically handle the destruction of the atomically handled object.
            ~Guard(){
                (*this)([](T* self){
                    self->~T();
                });
            }
        };   
    }
}

#endif