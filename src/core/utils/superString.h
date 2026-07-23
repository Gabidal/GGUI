#ifndef _SUPER_STRING_H_
#define _SUPER_STRING_H_

#include <string>
#include <cstring>
#include <array>
#include <initializer_list>

namespace GGUI{
    namespace INTERNAL{
        constexpr std::array<std::array<char, 2>, UINT8_MAX + 1> asciiToString = [] {
            std::array<std::array<char, 2>, UINT8_MAX + 1> t{};
            for (size_t i = 0; i < UINT8_MAX + 1; i++)
                t[i] = { static_cast<char>(i), '\0' };
            return t;
        }();

        constexpr std::array<std::array<char, 6>, UINT16_MAX + 1> numberToString = [] {
            std::array<std::array<char, 6>, UINT16_MAX + 1> t{};
            for (size_t i = 0; i <= UINT16_MAX; i++) {
                size_t currentValue = i;

                // find the length
                int length = (
                    (currentValue < 10)     ? 1 :
                    (currentValue < 100)    ? 2 :
                    (currentValue < 1000)   ? 3 :
                    (currentValue < 10000)  ? 4 : 5
                );

                // add null termination
                t[i][length] = '\0';

                // fill the digits
                for (int j = length - 1; j >= 0; j--) {
                    t[i][j] = '0' + (currentValue % 10);
                    currentValue /= 10;
                }
            }
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

            constexpr compactString(uint16_t longNumber) : text(numberToString[longNumber].data()), size(getLength(text)) {}

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
            constexpr compactString(const char* data, size_t Size, bool forceUnicode = false){
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
            constexpr void remap(compactString* preAllocatedWindowHead, size_t preAllocatedWindowCurrentIndex, size_t preAllocatedWindowLiquefiedSize) {
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
            constexpr void add(const char* Data, int size){
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
            constexpr void add(char Data){
                // Store the character in the data vector.
                add(compactString(Data));
            }

            constexpr void add(uint16_t longNumber) {
                add(compactString(longNumber));
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

            compactString toString(char* preAllocated) const {
                for (size_t i = 0, pos = 0; i < currentIndex; i++) {
                    const compactString& Data = data[i];

                    if (Data.size == 0)
                        break;

                    // Replace the current contents of the string with the contents of the Unicode Data.
                    std::memcpy(preAllocated + pos, Data.text, Data.size);
                    pos += Data.size;
                }

                return compactString(preAllocated, liquefiedSize, true);
            }
        };
    }

    
    constexpr const INTERNAL::compactString toCompactTable[256] = {
        INTERNAL::compactString("0", 1), INTERNAL::compactString("1", 1), INTERNAL::compactString("2", 1), INTERNAL::compactString("3", 1), INTERNAL::compactString("4", 1), INTERNAL::compactString("5", 1), INTERNAL::compactString("6", 1), INTERNAL::compactString("7", 1), INTERNAL::compactString("8", 1), INTERNAL::compactString("9", 1),
        INTERNAL::compactString("10", 2, true), INTERNAL::compactString("11", 2, true), INTERNAL::compactString("12", 2, true), INTERNAL::compactString("13", 2, true), INTERNAL::compactString("14", 2, true), INTERNAL::compactString("15", 2, true), INTERNAL::compactString("16", 2, true), INTERNAL::compactString("17", 2, true), INTERNAL::compactString("18", 2, true), INTERNAL::compactString("19", 2, true),
        INTERNAL::compactString("20", 2, true), INTERNAL::compactString("21", 2, true), INTERNAL::compactString("22", 2, true), INTERNAL::compactString("23", 2, true), INTERNAL::compactString("24", 2, true), INTERNAL::compactString("25", 2, true), INTERNAL::compactString("26", 2, true), INTERNAL::compactString("27", 2, true), INTERNAL::compactString("28", 2, true), INTERNAL::compactString("29", 2, true),
        INTERNAL::compactString("30", 2, true), INTERNAL::compactString("31", 2, true), INTERNAL::compactString("32", 2, true), INTERNAL::compactString("33", 2, true), INTERNAL::compactString("34", 2, true), INTERNAL::compactString("35", 2, true), INTERNAL::compactString("36", 2, true), INTERNAL::compactString("37", 2, true), INTERNAL::compactString("38", 2, true), INTERNAL::compactString("39", 2, true),
        INTERNAL::compactString("40", 2, true), INTERNAL::compactString("41", 2, true), INTERNAL::compactString("42", 2, true), INTERNAL::compactString("43", 2, true), INTERNAL::compactString("44", 2, true), INTERNAL::compactString("45", 2, true), INTERNAL::compactString("46", 2, true), INTERNAL::compactString("47", 2, true), INTERNAL::compactString("48", 2, true), INTERNAL::compactString("49", 2, true),
        INTERNAL::compactString("50", 2, true), INTERNAL::compactString("51", 2, true), INTERNAL::compactString("52", 2, true), INTERNAL::compactString("53", 2, true), INTERNAL::compactString("54", 2, true), INTERNAL::compactString("55", 2, true), INTERNAL::compactString("56", 2, true), INTERNAL::compactString("57", 2, true), INTERNAL::compactString("58", 2, true), INTERNAL::compactString("59", 2, true),
        INTERNAL::compactString("60", 2, true), INTERNAL::compactString("61", 2, true), INTERNAL::compactString("62", 2, true), INTERNAL::compactString("63", 2, true), INTERNAL::compactString("64", 2, true), INTERNAL::compactString("65", 2, true), INTERNAL::compactString("66", 2, true), INTERNAL::compactString("67", 2, true), INTERNAL::compactString("68", 2, true), INTERNAL::compactString("69", 2, true),
        INTERNAL::compactString("70", 2, true), INTERNAL::compactString("71", 2, true), INTERNAL::compactString("72", 2, true), INTERNAL::compactString("73", 2, true), INTERNAL::compactString("74", 2, true), INTERNAL::compactString("75", 2, true), INTERNAL::compactString("76", 2, true), INTERNAL::compactString("77", 2, true), INTERNAL::compactString("78", 2, true), INTERNAL::compactString("79", 2, true),
        INTERNAL::compactString("80", 2, true), INTERNAL::compactString("81", 2, true), INTERNAL::compactString("82", 2, true), INTERNAL::compactString("83", 2, true), INTERNAL::compactString("84", 2, true), INTERNAL::compactString("85", 2, true), INTERNAL::compactString("86", 2, true), INTERNAL::compactString("87", 2, true), INTERNAL::compactString("88", 2, true), INTERNAL::compactString("89", 2, true),
        INTERNAL::compactString("90", 2, true), INTERNAL::compactString("91", 2, true), INTERNAL::compactString("92", 2, true), INTERNAL::compactString("93", 2, true), INTERNAL::compactString("94", 2, true), INTERNAL::compactString("95", 2, true), INTERNAL::compactString("96", 2, true), INTERNAL::compactString("97", 2, true), INTERNAL::compactString("98", 2, true), INTERNAL::compactString("99", 2, true),
        INTERNAL::compactString("100", 3, true), INTERNAL::compactString("101", 3, true), INTERNAL::compactString("102", 3, true), INTERNAL::compactString("103", 3, true), INTERNAL::compactString("104", 3, true), INTERNAL::compactString("105", 3, true), INTERNAL::compactString("106", 3, true), INTERNAL::compactString("107", 3, true), INTERNAL::compactString("108", 3, true), INTERNAL::compactString("109", 3, true),
        INTERNAL::compactString("110", 3, true), INTERNAL::compactString("111", 3, true), INTERNAL::compactString("112", 3, true), INTERNAL::compactString("113", 3, true), INTERNAL::compactString("114", 3, true), INTERNAL::compactString("115", 3, true), INTERNAL::compactString("116", 3, true), INTERNAL::compactString("117", 3, true), INTERNAL::compactString("118", 3, true), INTERNAL::compactString("119", 3, true),
        INTERNAL::compactString("120", 3, true), INTERNAL::compactString("121", 3, true), INTERNAL::compactString("122", 3, true), INTERNAL::compactString("123", 3, true), INTERNAL::compactString("124", 3, true), INTERNAL::compactString("125", 3, true), INTERNAL::compactString("126", 3, true), INTERNAL::compactString("127", 3, true), INTERNAL::compactString("128", 3, true), INTERNAL::compactString("129", 3, true),
        INTERNAL::compactString("130", 3, true), INTERNAL::compactString("131", 3, true), INTERNAL::compactString("132", 3, true), INTERNAL::compactString("133", 3, true), INTERNAL::compactString("134", 3, true), INTERNAL::compactString("135", 3, true), INTERNAL::compactString("136", 3, true), INTERNAL::compactString("137", 3, true), INTERNAL::compactString("138", 3, true), INTERNAL::compactString("139", 3, true),
        INTERNAL::compactString("140", 3, true), INTERNAL::compactString("141", 3, true), INTERNAL::compactString("142", 3, true), INTERNAL::compactString("143", 3, true), INTERNAL::compactString("144", 3, true), INTERNAL::compactString("145", 3, true), INTERNAL::compactString("146", 3, true), INTERNAL::compactString("147", 3, true), INTERNAL::compactString("148", 3, true), INTERNAL::compactString("149", 3, true),
        INTERNAL::compactString("150", 3, true), INTERNAL::compactString("151", 3, true), INTERNAL::compactString("152", 3, true), INTERNAL::compactString("153", 3, true), INTERNAL::compactString("154", 3, true), INTERNAL::compactString("155", 3, true), INTERNAL::compactString("156", 3, true), INTERNAL::compactString("157", 3, true), INTERNAL::compactString("158", 3, true), INTERNAL::compactString("159", 3, true),
        INTERNAL::compactString("160", 3, true), INTERNAL::compactString("161", 3, true), INTERNAL::compactString("162", 3, true), INTERNAL::compactString("163", 3, true), INTERNAL::compactString("164", 3, true), INTERNAL::compactString("165", 3, true), INTERNAL::compactString("166", 3, true), INTERNAL::compactString("167", 3, true), INTERNAL::compactString("168", 3, true), INTERNAL::compactString("169", 3, true),
        INTERNAL::compactString("170", 3, true), INTERNAL::compactString("171", 3, true), INTERNAL::compactString("172", 3, true), INTERNAL::compactString("173", 3, true), INTERNAL::compactString("174", 3, true), INTERNAL::compactString("175", 3, true), INTERNAL::compactString("176", 3, true), INTERNAL::compactString("177", 3, true), INTERNAL::compactString("178", 3, true), INTERNAL::compactString("179", 3, true),
        INTERNAL::compactString("180", 3, true), INTERNAL::compactString("181", 3, true), INTERNAL::compactString("182", 3, true), INTERNAL::compactString("183", 3, true), INTERNAL::compactString("184", 3, true), INTERNAL::compactString("185", 3, true), INTERNAL::compactString("186", 3, true), INTERNAL::compactString("187", 3, true), INTERNAL::compactString("188", 3, true), INTERNAL::compactString("189", 3, true),
        INTERNAL::compactString("190", 3, true), INTERNAL::compactString("191", 3, true), INTERNAL::compactString("192", 3, true), INTERNAL::compactString("193", 3, true), INTERNAL::compactString("194", 3, true), INTERNAL::compactString("195", 3, true), INTERNAL::compactString("196", 3, true), INTERNAL::compactString("197", 3, true), INTERNAL::compactString("198", 3, true), INTERNAL::compactString("199", 3, true),
        INTERNAL::compactString("200", 3, true), INTERNAL::compactString("201", 3, true), INTERNAL::compactString("202", 3, true), INTERNAL::compactString("203", 3, true), INTERNAL::compactString("204", 3, true), INTERNAL::compactString("205", 3, true), INTERNAL::compactString("206", 3, true), INTERNAL::compactString("207", 3, true), INTERNAL::compactString("208", 3, true), INTERNAL::compactString("209", 3, true),
        INTERNAL::compactString("210", 3, true), INTERNAL::compactString("211", 3, true), INTERNAL::compactString("212", 3, true), INTERNAL::compactString("213", 3, true), INTERNAL::compactString("214", 3, true), INTERNAL::compactString("215", 3, true), INTERNAL::compactString("216", 3, true), INTERNAL::compactString("217", 3, true), INTERNAL::compactString("218", 3, true), INTERNAL::compactString("219", 3, true),
        INTERNAL::compactString("220", 3, true), INTERNAL::compactString("221", 3, true), INTERNAL::compactString("222", 3, true), INTERNAL::compactString("223", 3, true), INTERNAL::compactString("224", 3, true), INTERNAL::compactString("225", 3, true), INTERNAL::compactString("226", 3, true), INTERNAL::compactString("227", 3, true), INTERNAL::compactString("228", 3, true), INTERNAL::compactString("229", 3, true),
        INTERNAL::compactString("230", 3, true), INTERNAL::compactString("231", 3, true), INTERNAL::compactString("232", 3, true), INTERNAL::compactString("233", 3, true), INTERNAL::compactString("234", 3, true), INTERNAL::compactString("235", 3, true), INTERNAL::compactString("236", 3, true), INTERNAL::compactString("237", 3, true), INTERNAL::compactString("238", 3, true), INTERNAL::compactString("239", 3, true),
        INTERNAL::compactString("240", 3, true), INTERNAL::compactString("241", 3, true), INTERNAL::compactString("242", 3, true), INTERNAL::compactString("243", 3, true), INTERNAL::compactString("244", 3, true), INTERNAL::compactString("245", 3, true), INTERNAL::compactString("246", 3, true), INTERNAL::compactString("247", 3, true), INTERNAL::compactString("248", 3, true), INTERNAL::compactString("249", 3, true),
        INTERNAL::compactString("250", 3, true), INTERNAL::compactString("251", 3, true), INTERNAL::compactString("252", 3, true), INTERNAL::compactString("253", 3, true), INTERNAL::compactString("254", 3, true), INTERNAL::compactString("255", 3, true)
    };
}

#endif