#ifndef _FAST_VECTOR_H_
#define _FAST_VECTOR_H_

#include <type_traits>

#include "superString.h"

namespace GGUI {
    namespace INTERNAL {

        /**
         * Stack structured vector for fast read/write for liquefiedUTFText utility.
         */
        template<typename T>
        class fastVector {
            unsigned char* rawBuffer = nullptr;
            std::size_t capacity = 0;
            std::size_t size = 0;   
            public:
            
            fastVector(std::size_t initialSize) {
                // Initializes the fastVector with a raw buffer of size initialSize.
                rawBuffer = new unsigned char[sizeof(T) * initialSize];

                capacity = initialSize;
                clear();    // This describes the tail of the rawBuffer.
            }

            fastVector() = default;

            void clear() {
                size = 0;
            }

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

            void append(T unit) {
                // Copy the data from the other vector to this one.
                std::memcpy(rawBuffer + size * sizeof(T), &unit, sizeof(T));
                size++;
            }

            template<std::size_t mapSize>
            superString<mapSize> getWindow() {
                if (size + mapSize > capacity) {
                    resize(size + mapSize);
                }

                compactString* view = reinterpret_cast<compactString*>(rawBuffer + size * sizeof(T));

                return superString<mapSize>(view);
            }

            void releaseWindow(std::size_t finalSizeOfWindow) {
                size += finalSizeOfWindow;
            }

            std::size_t getSize() {
                return size;
            }

            T* getData() {
                return (T*)rawBuffer;
            }
        };

    }
}

#endif