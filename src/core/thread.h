#ifndef _THREAD_H_
#define _THREAD_H_

#include <mutex>
#include <optional>

namespace GGUI {
    namespace thread {
        extern void renderer();
        extern void eventThread();

        extern bool identicalFrame;
        
        extern float eventThreadLoad;  // Describes the load of animation and events from 0.0 to 1.0. Will reduce the event thread pause.

        enum class status{
            PAUSED,
            REQUESTING_RENDERING,
            RENDERING,
            NOT_INITIALIZED
        };

        // These are shared with all worker threads created by initGGUI
        namespace concurrency{
            extern int LOCKED;

            extern std::mutex mutex;
            extern std::condition_variable condition;

            extern status pauseRenderThread;
        }

        template<typename T>
        class guard {
            std::mutex shared;
            std::optional<T> data;  // optionally delayed construction if needed
        public:
            guard() = default;

            /**
             * @brief Constructs a Guard object and initializes its Data member.
             * 
             * This constructor creates a unique pointer to an instance of type T
             * and assigns it to the Data member of the Guard object.
             */
            template<typename... dataConstruction>
            guard(dataConstruction&&... args) {
                std::lock_guard<std::mutex> lock(shared);
                data.emplace(std::forward<dataConstruction>(args)...);
            }

            /**
             * @brief Returns false if construction has already been done, else true.
             */
            template<typename... dataConstruction>
            bool delayConstruct(dataConstruction&&... args) {
                std::lock_guard<std::mutex> lock(shared);
                if (!data.has_value()) {
                    data.emplace(std::forward<dataConstruction>(args)...);
                    return true;
                }
                return false;  // Already constructed
            }

            /**
             * @brief Functor to execute a job with thread safety.
             * 
             * This operator() function takes a std::function that operates on a reference to a T object.
             * It ensures that the job is executed with mutual exclusion by using a std::lock_guard to lock
             * the mutex. If the job throws an exception, it catches it and reports the failure.
             * 
             * @param job A callable that takes a reference to T and performs some operation.
             * 
             * @throws Any exception thrown by the job function will be caught and reported.
             */
            template<typename callable>
            requires std::is_invocable_v<callable, T&>
            void operator()(callable&& job) {
                std::lock_guard<std::mutex> lock(shared);

                if (!data.has_value()) {
                    throw std::runtime_error("Guard data is not initialized.");
                }

                std::forward<callable>(job)(*data);
            }

            /**
             * @brief Reads the data in a thread-safe manner.
             * 
             * This function acquires a lock on the shared mutex to ensure that the data
             * is read in a thread-safe manner. It returns a copy of the data.
             * 
             * @return T A copy of the data.
             */
            const T& read() {
                std::lock_guard<std::mutex> lock(shared);
                return *data;
            }
        };
    }
}

#endif