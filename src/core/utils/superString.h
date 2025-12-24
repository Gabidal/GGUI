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

        class compactString{
        public:
            const char* text = nullptr;
            size_t size = 0;

            /**
             * @brief Empty constructor for the Compact_String class. This is only used for resizing a vector of Compact_Strings, and should not be used directly.
             * @warning Do not use this constructor directly, as it will not initialize the Data property.
             * This constructor initializes a Compact_String object with default values.
             */
            constexpr compactString() = default;

            /**
             * @brief Default copy constructor for Compact_String.
             *
             * Creates a new Compact_String object as a copy of an existing one.
             * This constructor performs a member-wise copy of the source object.
             *
             * @param other The Compact_String instance to copy from.
             */
            constexpr compactString(const compactString&) = default;

            /**
             * @brief Move constructor for Compact_String.
             *
             * Constructs a new Compact_String by transferring the resources from another
             * Compact_String instance. The source object is left in a valid but unspecified state.
             *
             * @param other The Compact_String instance to move from.
             */
            constexpr compactString(compactString&&) = default;

            /**
             * @brief Default copy assignment operator for Compact_String.
             *
             * Assigns the contents of another Compact_String to this one.
             * The default implementation performs a member-wise copy of all fields.
             *
             * @param other The Compact_String instance to copy from.
             * @return Reference to this Compact_String after assignment.
             */
            constexpr compactString& operator=(const compactString&) = default;

            /**
             * @brief Default move assignment operator for Compact_String.
             *
             * Allows assigning the contents of another Compact_String to this one using move semantics.
             * The default implementation efficiently transfers resources from the source object,
             * leaving it in a valid but unspecified state.
             *
             * @return Reference to this Compact_String after assignment.
             */
            constexpr compactString& operator=(compactString&&) = default;

            constexpr compactString(const char* data){
                size_t tmpSize = getLength(data);

                if (tmpSize == 0) {
                    text = asciiToString[0].data();
                    size = 1;
                }
                else {
                    text = data;
                    size = tmpSize;
                }
            }

            constexpr compactString(char data) : text(asciiToString[static_cast<unsigned char>(data)].data()), size(1) {}

            constexpr compactString(const char* data, const size_t Size, const bool forceUnicode = false){
                text = data;
                
                if (forceUnicode) size = Size;
                else size = getLength(text);
            }

            // Fast comparison of type and content
            constexpr bool is(const char* other) const {
                return size > 1 && text && std::strcmp(text, other) == 0;
            }

            // Fast comparison of type and content
            constexpr bool is(char other) const {
                return size == 1 && text && text[0] == other;
            }

            /**
             * @brief Overloaded subscript operator to access character at a given index.
             * @param index The index of the character to access.
             * @return char The character at the specified index.
             */
            constexpr char operator[](int index) const {
                return ((unsigned)index >= size || index < 0 || !text) ? 
                    '\0' : // Return null character if index is out of bounds.
                    text[index];
            }

            constexpr void set(char val) {
                size = 1;
                text = asciiToString[static_cast<unsigned char>(val)].data();
            }

            constexpr void set(const char* val) {
                text = val;
                size = getLength(val);
            }
            
            /**
             * @brief Checks if the UTF object has a default text.
             * @return true if the UTF object has a default text, false otherwise.
             */
            constexpr bool hasDefaultText() const {
                return !empty() && text && text[0] == ' ';
            }

            constexpr bool empty() const {
                // Check if the Compact_String is empty.
                // An empty Compact_String has a size of 0.
                return size == 0;
            }

        protected:
            /**
             * @brief Computes the length of a null-terminated C-string at compile time.
             *
             * This constexpr function iterates through the input string until it encounters
             * the null terminator ('\0'), counting the number of characters.
             *
             * @param str Pointer to the null-terminated C-string.
             * @return The number of characters in the string, excluding the null terminator.
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
         * @class Super_String
         * @brief A container class for efficiently managing and concatenating multiple Compact_String objects.
         *
         * The Super_String class provides a fixed-size array of Compact_String objects, allowing for efficient
         * storage, addition, and concatenation of strings and characters. It maintains an internal index to track
         * the current number of stored elements and a liquefied size representing the total number of characters
         * across all stored strings. The class supports various methods for adding strings, characters, and other
         * Super_String instances, as well as clearing its contents and converting the stored data into a single
         * std::string.
         *
         * @tparam maxSize The maximum number of Compact_String objects that can be stored in the Super_String.
         *
         * @note This class is designed for performance and memory efficiency, making it suitable for scenarios
         *       where frequent string concatenation and manipulation are required.
         */
        template<std::size_t maxSize>
        class superString{
        public:
            // Data points either to inlineStorage (default) or an external window provided by conveyorAllocator.
            compactString* data = nullptr;
            size_t currentIndex = 0;
            size_t liquefiedSize = 0;

        protected:
            // Inline storage to avoid dynamic allocations for the common case.
            std::array<compactString, maxSize> inlineStorage{};
        
        public:

            /**
             * @brief Default constexpr constructor for the Super_String class.
             *
             * Initializes a Super_String object with default values at compile time.
             * This constructor does not perform any custom initialization logic.
             */
            constexpr superString() {
                // By default, use inline storage to avoid heap allocations.
                data = inlineStorage.data();
                currentIndex = 0;
                liquefiedSize = 0;
            }

            /**
             * @brief Constructs a Super_String from an initializer list of Compact_String objects.
             *
             * This constructor allows you to initialize a Super_String with a list of Compact_String
             * instances. Each item in the initializer list is added to the Super_String using the Add method.
             *
             * @param data An initializer list containing Compact_String objects to be added to the Super_String.
             */
            constexpr superString(const std::initializer_list<compactString>& Data) : superString() {
                for (const auto& item : Data) {
                    add(item);
                }
            }

            constexpr superString(compactString* preAllocatedWindow) {
                // Use external window memory; caller manages its lifetime.
                data = preAllocatedWindow;
                currentIndex = 0;
                liquefiedSize = 0;
            }

            constexpr void remap(compactString* preAllocatedWindowHead, const size_t preAllocatedWindowCurrentIndex, const size_t preAllocatedWindowLiquefiedSize) {
                // Use external window memory; caller manages its lifetime.
                data = preAllocatedWindowHead;
                currentIndex = preAllocatedWindowCurrentIndex;
                liquefiedSize = preAllocatedWindowLiquefiedSize;
            }

            /**
             * @brief Clears the contents of the Super_String.
             * 
             * This function resets the current index back to the start of the vector,
             * effectively clearing any stored data.
             */
            constexpr void clear(){
                // Set the current index back to the start of the vector.
                currentIndex = 0;
                liquefiedSize = 0;
            }

            /**
             * @brief Adds a Compact_String to the data vector.
             * 
             * This function appends the given Compact_String to the current position
             * in the Data vector and then increments the Current_Index.
             * 
             * @param other The Compact_String to add to the data vector.
             */
            constexpr void add(const compactString& other){
                // Store the Compact_String in the data vector.
                data[currentIndex++] = other;
                liquefiedSize += other.size; // Update the liquefied size with the size of the new Compact_String.
            }

            /**
             * @brief Adds a new string to the data vector.
             * 
             * This function stores the provided string in the data vector by creating a 
             * Compact_String object from the given data and size, and then adds it to 
             * the Data vector at the current index.
             * 
             * @param data Pointer to the character array containing the string to be added.
             * @param size The size of the string to be added.
             */
            constexpr void add(const char* Data, const int size){
                // Store the string in the Data vector.
                compactString tmp = compactString(Data, size);
                add(tmp);
            }

            /**
             * @brief Adds a character to the Super_String.
             * 
             * This function stores the given character in the data vector
             * and increments the current index.
             * 
             * @param data The character to be added to the Super_String.
             */
            constexpr void add(const char Data){
                // Store the character in the data vector.
                add(compactString(Data));
            }

            /**
             * @brief Adds the contents of another Super_String to this Super_String.
             *
             * This function appends the contents of the provided Super_String to the current
             * Super_String. If the Expected parameter is false, the function will resize the
             * Data vector to accommodate the additional characters.
             *
             * @param other A pointer to the Super_String to be added.
             * @param Expected A boolean flag indicating whether the reservation size is already
             *                 expected to be sufficient. If false, the Data vector will be resized.
             */
            template<std::size_t OtherMaxSize>
            constexpr void add(const superString<OtherMaxSize>* other){
                // Copy the contents of the other Super_String into the Data vector.
                for (size_t i = 0; i < other->currentIndex; i++){
                    add(other->data[i]);
                }
            }
            
            /**
             * @brief Add the contents of another Super_String to this one.
             * @param other The Super_String to add.
             * @param Expected If true, the size of the Data vector will not be changed.
             * @details This function is used to concatenate Super_Strings.
             */
            template<std::size_t OtherMaxSize>
            constexpr void add(const superString<OtherMaxSize>& other){
                // Copy the contents of the other Super_String into the Data vector.
                for (size_t i = 0; i < other.currentIndex; i++){
                    add(other.data[i]);
                }
            }

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
             * @brief Converts the Super_String object to a std::string.
             * 
             * This function goes through the Data vector and calculates the total size of all
             * the strings stored in the vector. It then resizes a std::string to that size and
             * then copies the contents of the Data vector into the std::string.
             * 
             * @return A std::string that contains all the strings stored in the Data vector.
             */
            std::string toString() {
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