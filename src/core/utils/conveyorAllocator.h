#ifndef _FAST_VECTOR_H_
#define _FAST_VECTOR_H_

#include <type_traits>
#include <cstring> // std::memcpy

#include "superString.h"

namespace GGUI {
    // autoGen: Ignore start
    namespace INTERNAL {

        /**
         * @brief based on the conveyor style restaurants, where the food is given in a conveyor belt and the customer takes a plate -> eats it -> and then returns it.
         * The chef of the conveyor belt allocator will monitor how much did the customer like the serving and based on the returned plate it will give another.
         */
        template<typename T>
        class conveyorAllocator {
            T* rawBuffer = nullptr;
            std::size_t capacity = 0;
            std::size_t size = 0;   
            public:
            
            /**
             * @brief Construct a conveyorAllocator with an initial element capacity.
             * @param initialSize Number of elements memory should be reserved for immediately.
             *
             * The memory is uninitialized (raw bytes) and the logical size is set to 0.
             */
            conveyorAllocator(std::size_t initialSize) {
                rawBuffer = new T[initialSize];
                capacity = initialSize;
                clear();
            }

            conveyorAllocator() = default;

            /**
             * @brief Reset the logical size to zero without releasing capacity.
             *        Existing bytes remain untouched and will be overwritten on next append.
             */
            void clear() { size = 0; }

            /**
             * @brief Grow the underlying storage if required.
             * @param newCapacity Requested minimum capacity (in elements).
             *
             * If the requested capacity exceeds the current one a new raw buffer is allocated
             * and the active bytes (size * sizeof(T)) are memcpy'd across. Logical size is
             * preserved; newly obtained capacity is uninitialized.
             */
            void resize(std::size_t newCapacity) {
                if (newCapacity > capacity) {
                    // Allocate new mem
                    T* newBuffer = new T[newCapacity];
                    // Copy existing contents (based on current logical size)
                    if (rawBuffer && size > 0) {
                        std::memcpy(newBuffer, rawBuffer, sizeof(T) * size);
                    }
                    // Replace buffer
                    delete[] rawBuffer;
                    rawBuffer = newBuffer;
                    capacity = newCapacity;
                    // Keep size unchanged to continue appending at the correct offset
                }
            }

            /**
             * @brief Append a single element by raw copying it into the buffer tail.
             * @param unit Element to append (must be trivially copyable).
             * @warning No bounds checking is performed. Caller must ensure capacity.
             */
            void append(T unit) {
                std::memcpy(rawBuffer + size * sizeof(T), &unit, sizeof(T));
                size++;
            }

            /**
             * @brief Acquire a writable window of compile-time size appended directly after current data.
             *
             * Ensures sufficient capacity (growing if necessary) and returns a superString that writes
             * into the reserved region. After populating the window call returnPlate() with the
             * number of entries actually produced.
             *
             * @tparam mapSize Maximum number of compactString entries the returned superString may hold.
             * @return superString<mapSize> View writing into internal storage.
             */
            template<std::size_t mapSize>
            void eatPlate(superString<mapSize>& result) {
                if (size + mapSize > capacity) {
                    resize(size + mapSize);
                }

                compactString* view = reinterpret_cast<compactString*>(rawBuffer + size);

                result.remap(
                    view,
                    0,
                    0
                );
            }

            /**
             * @brief Commit a previously acquired write window.
             * @param finalSizeOfWindow Number of elements actually written into the window.
             *
             * Increases the logical size so subsequent appends occur after the committed region.
             */
            void returnPlate(std::size_t finalSizeOfWindow) { size += finalSizeOfWindow; }

            /**
             * @brief Get the number of logically stored elements.
             */
            std::size_t getSize() { return size; }

            /**
             * @brief Obtain a mutable pointer to the contiguous element data.
             * @return Pointer to first element (reinterpret_cast from raw bytes).
             */
            T* getData() { return rawBuffer; }
        };

    }
    // autoGen: Ignore end
}

#endif