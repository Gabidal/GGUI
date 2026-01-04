#ifndef _SUPER_STRING_H_
#define _SUPER_STRING_H_

#include <string>
#include <cstring>
#include <array>
#include <vector>
#include <variant>
#include <initializer_list>

namespace GGUI{
    namespace INTERNAL{
        constexpr std::array<std::array<char, 2>, 256> asciiToString = [] {
            std::array<std::array<char, 2>, 256> t{};
            for (size_t i = 0; i < 256; ++i)
                t[i] = { static_cast<char>(i), '\0' };
            return t;
        }();

        /**
         * @class compactString
         * @brief Lightweight string view optimized for tiny tokens.
         *
         * `compactString` is a small, trivially copyable wrapper around a `const char*` and a byte length.
         * It is used heavily by `superString` to store many small fragments without allocating.
         *
         * Storage model:
         * - For single-byte values (characters), it points into the static `asciiToString` lookup table.
         * - For C-strings, it points to the provided memory and computes the length once.
         * - For externally sized buffers, it can be constructed with an explicit byte length.
         *
         * Lifetime:
         * - This type does not own memory.
         * - Any pointer passed in must outlive the `compactString` and all of its consumers.
         *
         * Encoding:
         * - `size` is measured in bytes and may represent UTF-8 byte length when used with text.
         */
        class compactString{
        public:
            const char* text = nullptr;
            size_t size = 0;

            /**
             * @brief Default constructor.
             *
             * Creates an empty `compactString` (`text == nullptr`, `size == 0`).
             *
             * @note This is commonly used by containers (e.g., `std::array`) that value-initialize elements.
             */
            constexpr compactString() = default;

            /**
             * @brief Copy constructor.
             * @param other Source view.
             */
            constexpr compactString(const compactString&) = default;

            /**
             * @brief Move constructor.
             * @param other Source view.
             *
             * @note Moving is equivalent to copying because the type is non-owning.
             */
            constexpr compactString(compactString&&) = default;

            /**
             * @brief Copy assignment.
             * @param other Source view.
             * @return `*this`.
             */
            constexpr compactString& operator=(const compactString&) = default;

            /**
             * @brief Move assignment.
             * @param other Source view.
             * @return `*this`.
             *
             * @note Moving is equivalent to copying because the type is non-owning.
             */
            constexpr compactString& operator=(compactString&&) = default;

            /**
             * @brief Construct from a null-terminated C-string.
             *
             * Computes the byte length up-front.
             *
             * Special case:
             * - If the computed length is 0 (i.e., `""`), this will point at the static `"\0"` from
             *   `asciiToString[0]` and set `size = 1`.
             *
             * @param data Null-terminated string pointer. May be null.
             */
            constexpr compactString(const char* data){
                size_t tmpSize = getLength(data);

                if (tmpSize == 0) { //  If the given string is something like this: "\0", point to the global array at zero index.
                    text = asciiToString[0].data();
                    size = 1;   // {"\0", "\0"};
                }
                else {
                    text = data;
                    size = tmpSize;
                }
            }

            /**
             * @brief Construct a single-byte character view.
             * @param data Character to represent.
             *
             * Points into `asciiToString` and sets `size = 1`.
             */
            constexpr compactString(char data) : text(asciiToString[static_cast<unsigned char>(data)].data()), size(1) {}

            /**
             * @brief Construct from a pointer and size.
             *
             * Use this for buffers that are not necessarily null-terminated (e.g., UTF-8 fragments).
             *
             * @param data Pointer to character data (not owned).
             * @param Size Byte length to use when `forceUnicode` is true.
             * @param forceUnicode If true, `size` is taken from `Size`; otherwise, `size` is computed by
             *        scanning for a null terminator.
             */
            constexpr compactString(const char* data, const size_t Size, const bool forceUnicode = false){
                text = data;
                
                if (forceUnicode) size = Size;
                else size = getLength(text);
            }

            /**
             * @brief Check whether this represents the given null-terminated C-string.
             *
             * @param other Null-terminated string to compare against.
             * @return True if this is a multi-byte string view (`size > 1`) and `strcmp(text, other) == 0`.
             *
             * @note This intentionally returns false for single-character views.
             */
            constexpr bool is(const char* other) const {
                return size > 1 && text && std::strcmp(text, other) == 0;
            }

            /**
             * @brief Check whether this represents the given character.
             * @param other Character to compare against.
             * @return True if this is a single-byte view (`size == 1`) and the byte matches.
             */
            constexpr bool is(char other) const {
                return size == 1 && text && text[0] == other;
            }

            /**
             * @brief Safe byte access.
             * @param index Byte index to read.
             * @return The byte at `index`, or `'\0'` if out of bounds or `text` is null.
             *
             * @note This is byte-oriented; it does not decode UTF-8 codepoints.
             */
            constexpr char operator[](int index) const {
                return ((unsigned)index >= size || index < 0 || !text) ? 
                    '\0' : // Return null character if index is out of bounds.
                    text[index];
            }

            /**
             * @brief Replace this view with a single-byte character.
             * @param val Character to represent.
             */
            constexpr void set(char val) {
                size = 1;
                text = asciiToString[static_cast<unsigned char>(val)].data();
            }

            /**
             * @brief Replace this view with a null-terminated C-string.
             * @param val Null-terminated string pointer. May be null.
             */
            constexpr void set(const char* val) {
                text = val;
                size = getLength(val);
            }
            
            /**
             * @brief Check whether this view uses the sentinel "default text" representation.
             *
             * In this codebase, a non-empty string starting with a space (`' '`) is sometimes used as a
             * sentinel for "default"/"unset" UI text.
             *
             * @return True if non-empty and the first byte is `' '`.
             */
            constexpr bool hasDefaultText() const {
                return !empty() && text && text[0] == ' ';
            }

            /**
             * @brief Check whether this view is empty.
             * @return True when `size == 0`.
             */
            constexpr bool empty() const {
                // Check if the Compact_String is empty.
                // An empty Compact_String has a size of 0.
                return size == 0;
            }

        protected:
            /**
             * @brief Compute the byte length of a null-terminated C-string.
             *
             * @param str Pointer to a null-terminated string.
             * @return Number of bytes before the first `'\0'`. Returns 0 when `str` is null.
             */
            constexpr size_t getLength(const char* str) {
                size_t length = 0;

                if (!str) {
                    return length; // Return 0 if the input string is null.
                }

                while (str[length] != '\0') ++length;
                return length;
            }
        };


        /**
         * @class superString
         * @brief Fixed-capacity fragment buffer for efficient string assembly.
         *
         * `superString` stores up to `maxSize` non-owning string fragments (`compactString`).
         * It is designed to build output strings with minimal allocations by:
         * - storing many tiny tokens (often 1-byte) as views,
         * - keeping a running total byte length (`liquefiedSize`),
         * - optionally operating over a caller-provided fragment window.
         *
         * Lifetime/ownership:
         * - Fragment text is never owned by `superString` (it stores views).
         * - If you use external storage via `remap()` / external constructor, that memory must outlive
         *   the `superString` instance.
         *
         * Encoding:
         * - All sizes are byte lengths. When used with UTF-8, `liquefiedSize` is the UTF-8 byte count.
         */
        template<std::size_t maxSize>
        class superString{
        public:
            /**
             * @brief Active storage pointer.
             *
             * Points either to the internal inline storage (`inlineStorage`) or to an externally provided
             * contiguous window (see `superString(compactString*)` and `remap()`).
             *
             * @note This class does not own external windows.
             */
            compactString* data = nullptr;

            /**
             * @brief Number of fragments currently stored in `data`.
             *
             * Valid range: `[0, maxSize]` for inline storage.
             */
            size_t currentIndex = 0;

            /**
             * @brief Total number of bytes across all stored fragments.
             *
             * This is the sum of `data[i].size` for all added fragments.
             */
            size_t liquefiedSize = 0;

        protected:
            /**
             * @brief Inline storage to avoid dynamic allocations.
             *
             * The default constructor sets `data = inlineStorage.data()`.
             */
            std::array<compactString, maxSize> inlineStorage{};
        
        public:

            /**
             * @brief Construct an empty `superString` using inline storage.
             *
             * Initializes `data` to point at `inlineStorage` and resets indices.
             */
            constexpr superString() {
                // By default, use inline storage to avoid heap allocations.
                data = inlineStorage.data();
                currentIndex = 0;
                liquefiedSize = 0;
            }

            /**
             * @brief Construct from an initializer-list of fragments.
             *
             * Each item is appended in order via `add(const compactString&)`.
             *
             * @param Data Initial fragments to append.
             */
            constexpr superString(const std::initializer_list<compactString>& Data) : superString() {
                for (const auto& item : Data) {
                    add(item);
                }
            }

            /**
             * @brief Construct an empty `superString` over an external fragment window.
             *
             * This is used to avoid even the inline array when some higher-level allocator provides
             * a reusable `compactString[]` window.
             *
             * @param preAllocatedWindow Pointer to at least `maxSize` `compactString` slots.
             * @warning The window is not owned; it must outlive this `superString`.
             */
            constexpr superString(compactString* preAllocatedWindow) {
                // Use external window memory; caller manages its lifetime.
                data = preAllocatedWindow;
                currentIndex = 0;
                liquefiedSize = 0;
            }

            /**
             * @brief Rebind this instance to a different external window and restore state.
             *
             * This is effectively a “move” of the logical contents into a new backing window.
             *
             * @param preAllocatedWindowHead New backing array.
             * @param preAllocatedWindowCurrentIndex Fragment count to restore.
             * @param preAllocatedWindowLiquefiedSize Total byte length to restore.
             * @warning This does not copy fragments; it only repoints internal pointers.
             */
            constexpr void remap(compactString* preAllocatedWindowHead, const size_t preAllocatedWindowCurrentIndex, const size_t preAllocatedWindowLiquefiedSize) {
                // Use external window memory; caller manages its lifetime.
                data = preAllocatedWindowHead;
                currentIndex = preAllocatedWindowCurrentIndex;
                liquefiedSize = preAllocatedWindowLiquefiedSize;
            }

            /**
             * @brief Clear all stored fragments.
             *
             * Resets `currentIndex` and `liquefiedSize` to zero.
             * Does not modify the fragment storage contents.
             */
            constexpr void clear(){
                // Set the current index back to the start of the vector.
                currentIndex = 0;
                liquefiedSize = 0;
            }

            /**
             * @brief Append a fragment view.
             *
             * @param other Fragment to append.
             *
             * @warning No bounds checking is performed. Exceeding the available capacity
             *          will write out of bounds.
             */
            constexpr void add(const compactString& other){
                // Store the Compact_String in the data vector.
                data[currentIndex++] = other;
                liquefiedSize += other.size; // Update the liquefied size with the size of the new Compact_String.
            }

            /**
             * @brief Append a sized byte fragment.
             *
             * This is commonly used for UTF-8 fragments that are not null-terminated.
             *
             * @param Data Pointer to byte data (not owned).
             * @param size Byte length to append.
             *
             * @warning No bounds checking is performed.
             */
            constexpr void add(const char* Data, const int size){
                // Store the string in the Data vector.
                // `size` is an explicit byte length (often for UTF-8 fragments) and may not be null-terminated.
                compactString tmp = compactString(Data, size, true);
                add(tmp);
            }

            /**
             * @brief Append a single byte.
             * @param Data Byte/character to append.
             * @warning No bounds checking is performed.
             */
            constexpr void add(const char Data){
                // Store the character in the data vector.
                add(compactString(Data));
            }

            /**
             * @brief Append another `superString`'s fragments (pointer overload).
             *
             * @tparam OtherMaxSize Source capacity.
             * @param other Source `superString` to append.
             *
             * @warning No bounds checking is performed.
             */
            template<std::size_t OtherMaxSize>
            constexpr void add(const superString<OtherMaxSize>* other){
                // Copy the contents of the other Super_String into the Data vector.
                for (size_t i = 0; i < other->currentIndex; i++){
                    add(other->data[i]);
                }
            }
            
            /**
             * @brief Append another `superString`'s fragments (reference overload).
             *
             * @tparam OtherMaxSize Source capacity.
             * @param other Source `superString` to append.
             *
             * @warning No bounds checking is performed.
             */
            template<std::size_t OtherMaxSize>
            constexpr void add(const superString<OtherMaxSize>& other){
                // Copy the contents of the other Super_String into the Data vector.
                for (size_t i = 0; i < other.currentIndex; i++){
                    add(other.data[i]);
                }
            }

            /**
             * @brief Materialize all fragments into a single contiguous byte buffer (view return).
             *
             * Concatenates all fragments in order and returns a `compactString` view of the resulting
             * buffer.
             *
             * @return A `compactString` whose `text` points to a newly allocated byte buffer and whose
             *         `size` equals `liquefiedSize`.
             *
             * @warning Ownership/lifetime: this function allocates with `new[]` and returns a non-owning
             *          view (`compactString`) that has no way to release that allocation. As written, this
             *          is a leak unless some external convention frees the returned pointer.
             *          Prefer `toString()` if you need an owning result.
             */
            inline const compactString compress() const {
                char* header = new char[liquefiedSize];
                compactString result(header, liquefiedSize, true);

                for (size_t i = 0, pos = 0; i < currentIndex; i++) {
                    const compactString& Data = data[i];

                    if (Data.size == 0)
                        break;

                    // Replace the current contents of the string with the contents of the Unicode Data.
                    std::memcpy((char*)header + pos, Data.text, Data.size);
                    pos += Data.size;
                }

                return result;
            }

            /**
             * @brief Materialize all fragments into an owning `std::string`.
             *
             * Copies all fragments in order into a single `std::string` of size `liquefiedSize`.
             *
             * @return Owning string containing the concatenated bytes.
             */
            std::string toString() const {
                // Resize a std::string to the total size.
                std::string result;
                result.resize(liquefiedSize);

                // Copy the contents of the Data vector into the std::string.
                int currentUTFInsertIndex = 0;
                for(size_t i = 0; i < currentIndex; i++){
                    const compactString& Data = data[i];

                    if (Data.size == 0)
                        break;

                    // Replace the current contents of the string with the contents of the Unicode Data.
                    result.replace(currentUTFInsertIndex, Data.size, Data.text);
                    currentUTFInsertIndex += Data.size;
                }
                return result;
            }
        };
    }
}

#endif