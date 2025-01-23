#ifndef _GUARD_H_
#define _GUARD_H_

#include <functional>
#include <mutex>
#include <string>
#include <memory>

namespace GGUI{
    namespace INTERNAL{
        extern void reportStack(std::string Problem);

        namespace atomic{

            template<typename T>
            class Guard {
            public:
                std::mutex Shared; // Mutex to guard shared data
                std::unique_ptr<T> Data;

                /**
                 * @brief Constructs a Guard object and initializes its Data member.
                 * 
                 * This constructor creates a unique pointer to an instance of type T
                 * and assigns it to the Data member of the Guard object.
                 */
                Guard() : Data(std::make_unique<T>()) {}

                /**
                 * @brief Functor to execute a job with thread safety.
                 * 
                 * This operator() function takes a std::function that operates on a reference to a T object.
                 * It ensures that the job is executed with mutual exclusion by using a std::lock_guard to lock
                 * the mutex. If the job throws an exception, it catches it and reports the failure.
                 * 
                 * @param job A std::function that takes a reference to a T object and performs some operation.
                 * 
                 * @throws Any exception thrown by the job function will be caught and reported.
                 */
                void operator()(std::function<void(T&)> job) {
                    std::lock_guard<std::mutex> lock(Shared); // Automatically manages mutex locking and unlocking
                    try {
                        job(*Data);
                    } catch (...) {
                        reportStack("Failed to execute the function!");
                    }
                }

                /**
                 * @brief Reads the data in a thread-safe manner.
                 * 
                 * This function acquires a lock on the shared mutex to ensure that the data
                 * is read in a thread-safe manner. It returns a copy of the data.
                 * 
                 * @return T A copy of the data.
                 */
                T Read() {
                    std::lock_guard<std::mutex> lock(Shared);
                    return *Data;
                }

                /**
                 * @brief Destructor for the Guard class.
                 *
                 * This destructor ensures that the Data object is properly destroyed
                 * by acquiring a lock on the Shared mutex before resetting the Data.
                 * The use of std::lock_guard ensures that the mutex is automatically
                 * released when the destructor exits, preventing potential deadlocks.
                 */
                ~Guard() {
                    std::lock_guard<std::mutex> lock(Shared);
                    Data.reset(); // Ensures proper destruction
                }
            };   
        }
    }
}

#endif