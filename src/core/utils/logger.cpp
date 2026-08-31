#include "logger.h"
#include "../core.h"
#include "../thread.h"

#include "../process/process.h"

#include "settings.h"
#include <unistd.h>
#include <vector>
#include <string>
#include <thread>
#include <iostream>
#include <queue>

namespace GGUI{
    namespace logger{
        std::filesystem::path logFile = process::getFilePath() / "log.txt";

        // File handle for logging to files for Atomic access across different threads.
        thread::guard<std::basic_ofstream<char>> handle;

        // to enable default to nullptr for Guard
        class queue{
        public:
            std::queue<std::string> handle;

            queue() = default;

            void push(std::string value) {
                handle.push(std::move(value));
            }

            void flushInto(thread::guard<queue>& dest) {
                dest([this](queue& destQueue){
                    while (!handle.empty()) {
                        destQueue.push(std::move(handle.front()));
                        handle.pop();
                    }
                });
            }

            void flushInto(thread::guard<std::basic_ofstream<char>>& fileHandle) {
                fileHandle([this](std::basic_ofstream<char>& out){
                    while (!handle.empty()) {
                        out << handle.front();
                        handle.pop();
                    }
                });
            }
        };

        // Holds registry of all worked localQueues, used for reading and flushing into output (log.txt)
        thread::guard<std::vector<thread::guard<queue>*>> allQueues;

        // main use is for when thread_local uninitializes this it will remove the localQueue from the AllQueues
        struct automaticRegisterer {
            thread::guard<queue> data;

            // Automatically registers upon construction
            automaticRegisterer() {
                // force allQueues to exist
                allQueues.delayConstruct();
                data.delayConstruct(); // construct and check if it was already constructed

                allQueues([this](auto& queues){
                    queues.push_back(&data);
                });
            }

            // Deleted for safety
            automaticRegisterer(const automaticRegisterer&) = delete;
            automaticRegisterer& operator=(const automaticRegisterer&) = delete;

            ~automaticRegisterer() {
                allQueues([this](auto& queues){
                    auto it = std::find(queues.begin(), queues.end(), &data);
                    if (it != queues.end()) {
                        queues.erase(it);
                    }
                });
            }
        };

        // Each thread will have one local queue which they will output into, this is automatically registered upton touch.
        thread_local automaticRegisterer localQueue;

        /**
         * @brief Logs a single-line message to the localQueue, prepending a timestamp.
         */
        void log(const std::string& Text){
            // Get current time for timestamp
            std::string now = core::now();
            
            // Format the log message with timestamp
            std::string logEntry = "[" + now + "] " + Text + "\n";

            // Push the log entry into the local queue
            localQueue.data([&logEntry](auto& queue){
                queue.push(std::move(logEntry));
            });
        }

        void loggerThread(){
            // Ensure logger file handle is created
            handle.delayConstruct(logFile, std::ios::app);

            // Register here instead of it happening inside the allQueues mutex lock.
            log("Logger thread starting...");

            while (true){
                try {
                    {
                        std::unique_lock lock(thread::concurrency::mutex);

                        if (thread::concurrency::requestTermination){
                            break;
                        }
                    }

                    // First we'll quickly do a checkup into the AllQueues and look if there is anything to log.
                    allQueues([](auto& self){
                        for (auto* queue : self){
                            if (queue == &localQueue.data) continue;  // skip the logger threads own localQueue

                            (*queue)([](logger::queue& self2){  // flush all data into logger threads own local queue
                                self2.flushInto(localQueue.data);
                            });
                        }
                    });
                    
                    // Now all possible logged data should be in our own localQueue which we will now flush into the handle logFile
                    localQueue.data([&](logger::queue& self){
                        self.flushInto(handle);
                    });

                    // Now we can flush into file
                    handle([&](std::basic_ostream<char>& out){
                        out.flush();
                    });

                    std::chrono::steady_clock::duration SleepTime = SETTINGS::MAX_UPDATE_SPEED;

                    std::this_thread::sleep_for(SleepTime);
                } 
                catch (std::exception& e){
                    std::cerr << "logger crashed because: " << e.what() << std::endl;
                }
            }
        }
    }
}