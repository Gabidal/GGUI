#ifndef _FAST_VECTOR_H_
#define _FAST_VECTOR_H_

#include <type_traits>
#include <cstring> // std::memcpy

#include "superString.h"

namespace GGUI {
    // autoGen: Ignore start
    namespace INTERNAL {

        /**
         * @brief Cache-friendly, stack-like grow-only vector used in the text liquefaction pipeline.
         *
         * This container purposefully omits a large portion of the std::vector interface to
         * provide a very small & predictable set of operations that map directly to the
         * renderer's hot path requirements (sequential append + obtaining a raw writable
         * "window" for in–place construction of a batch of compactString entries).
         *
         * Design characteristics:
         * - Trivially relocatable raw byte buffer (unsigned char*) to keep type T layout opaque.
         * - Manual resize without value–initialization of new capacity.
         * - No element destruction (T must be trivially copyable / POD‑like for safe raw memcpy).
         * - getWindow()/releaseWindow() allow reserving a compile‑time sized region that a
         *   superString will write into directly, avoiding intermediate temporaries.
         *
         * @tparam T Trivially copyable element type stored contiguously.
         */
        template<typename T>
        class fastVector {
            unsigned char* rawBuffer = nullptr;
            std::size_t capacity = 0;
            std::size_t size = 0;   
            public:
            
            /**
             * @brief Construct a fastVector with an initial element capacity.
             * @param initialSize Number of elements memory should be reserved for immediately.
             *
             * The memory is uninitialized (raw bytes) and the logical size is set to 0.
             */
            fastVector(std::size_t initialSize) {
                rawBuffer = new unsigned char[sizeof(T) * initialSize];
                capacity = initialSize;
                clear();
            }

            fastVector() = default;

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
                    unsigned char* newBuffer = new unsigned char[sizeof(T) * newCapacity];
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
             * into the reserved region. After populating the window call releaseWindow() with the
             * number of entries actually produced.
             *
             * @tparam mapSize Maximum number of compactString entries the returned superString may hold.
             * @return superString<mapSize> View writing into internal storage.
             */
            template<std::size_t mapSize>
            superString<mapSize> getWindow() {
                if (size + mapSize > capacity) {
                    resize(size + mapSize);
                }

                compactString* view = reinterpret_cast<compactString*>(rawBuffer + size * sizeof(T));

                return superString<mapSize>(view);
            }

            /**
             * @brief Commit a previously acquired write window.
             * @param finalSizeOfWindow Number of elements actually written into the window.
             *
             * Increases the logical size so subsequent appends occur after the committed region.
             */
            void releaseWindow(std::size_t finalSizeOfWindow) { size += finalSizeOfWindow; }

            /**
             * @brief Get the number of logically stored elements.
             */
            std::size_t getSize() { return size; }

            /**
             * @brief Obtain a mutable pointer to the contiguous element data.
             * @return Pointer to first element (reinterpret_cast from raw bytes).
             */
            T* getData() { return (T*)rawBuffer; }
        };

    }
    // autoGen: Ignore end
}

#endif