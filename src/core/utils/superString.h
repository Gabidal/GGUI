#ifndef _SUPER_STRING_H_
#define _SUPER_STRING_H_

#include <string>
#include <cstring>
#include <vector>
#include <variant>
#include <initializer_list>

namespace GGUI{
    
    namespace COMPACT_STRING_FLAG{
        constexpr inline unsigned char IS_ASCII          = 1 << 0;
        constexpr inline unsigned char IS_UNICODE        = 1 << 1;
    };

    // And lighter-weight version of the UTF class. [Probably after making the RGBA use unsigned char instead of float, and thus making the overall size into 32 bits, replace this class with UTF.]
    class Compact_String{
    public:
        std::variant<char, const char*> Text;
        unsigned int Size = 0;

        /**
         * @brief Empty constructor for the Compact_String class. This is only used for resizing a vector of Compact_Strings, and should not be used directly.
         * @warning Do not use this constructor directly, as it will not initialize the Data property.
         * This constructor initializes a Compact_String object with default values.
         */
        constexpr Compact_String() = default;

        /**
         * @brief Constructs a Compact_String object from a C-style string.
         * 
         * This constructor initializes the Compact_String object by determining the length of the input string.
         * If the length of the string is greater than 1, it stores the string data in Unicode_Data.
         * If the length of the string is 1 or less, it stores the single character in Ascii_Data.
         * 
         * @param data A pointer to a null-terminated C-style string.
         */
        constexpr Compact_String(const char* data) : Text(data){
            Size = std::strlen(data); // Get the length of the string.

            // Store the string as Unicode data if its length is greater than 1.
            // Store the single character as ASCII data.
            Size > 1 ? 
                Set_Unicode(data) : 
                Set_Ascii(data[0]);
        }

        /**
         * @brief Constructs a Compact_String object with a single ASCII character.
         * 
         * This constructor initializes the Compact_String with a single character.
         * The character is stored in the Ascii_Data member of the Data union, and
         * the Size is set to 1.
         * 
         * @param data The ASCII character to initialize the Compact_String with.
         */
        constexpr Compact_String(const char data) : Text(data){
            Size = 1;
        }

        /**
         * @brief Constructs a Compact_String object.
         * 
         * This constructor initializes a Compact_String object with the given data and size.
         * It determines the storage format based on the size of the data and the Force_Unicode flag.
         * 
         * @param data A pointer to the character data to be stored.
         * @param size The size of the character data.
         * @param Force_Unicode A boolean flag indicating whether to force the data to be stored as Unicode.
         *                       Defaults to false.
         * 
         * If the size of the data is greater than 1 or if Force_Unicode is true, the data is stored as Unicode.
         * Otherwise, the data is stored as a single ASCII character.
         */
        constexpr Compact_String(const char* data, const unsigned int size, const bool Force_Unicode = false) : Text(data) {
            Size = size;

            // Determine data storage based on size and Force_Unicode flag.
            // Store as Unicode data if size is greater than 1 or forced.
            // Store as a single ASCII character.
            (Size > 1 || Force_Unicode) ? 
                Set_Unicode(data) : 
                Set_Ascii(data[0]);
        }

        /**
         * @brief Checks if a specific UTF flag is set.
         * @param cs_flag The UTF flag to check.
         * @return True if the flag is set, otherwise false.
         */
        constexpr bool Is(unsigned char cs_flag) const {
            return (cs_flag == COMPACT_STRING_FLAG::IS_ASCII && Size == 1) ? true : false;
        }

        // Fast comparison of type and content
        constexpr bool Is(const char* other) const {
            return Is(COMPACT_STRING_FLAG::IS_UNICODE) ? std::strcmp(std::get<const char*>(Text), other) == 0 : false;
        }

        // Fast comparison of type and content
        constexpr bool Is(char other) const {
            return Is(COMPACT_STRING_FLAG::IS_ASCII) ? std::get<char>(Text) == other : false;
        }

        /**
         * @brief Overloaded subscript operator to access character at a given index.
         * 
         * This operator allows access to the character at the specified index.
         * If the size of the string is greater than 1, it returns the character
         * from the Unicode data. If the size is 1, it returns the ASCII data.
         * 
         * @param index The index of the character to access.
         * @return char The character at the specified index.
         */
        constexpr char operator[](unsigned int index) const {
            // If the size is greater than 1, we have to index into the Unicode data.
            // If the size is 1, we just return the ASCII data.
            return Size > 1 ? 
                std::get<const char*>(Text)[index] : 
                std::get<char>(Text);
        }

        constexpr const char* Get_Unicode() const {
            // If the size is greater than 1, return the Unicode data.
            // Otherwise, return a pointer to the ASCII data.
            return Size > 1 ? 
                std::get<const char*>(Text) : 
                nullptr;
        }

        constexpr char Get_Ascii() const {
            // If the size is 1, return the ASCII data.
            // Otherwise, return a null character.
            return Size == 1 ? 
                std::get<char>(Text) : 
                '\0';
        }

        constexpr void Set_Unicode(const char* text) {
            // Set the Text to the Unicode data.
            Text = std::variant<char, const char*>(text);
            Size = std::strlen(text); // Update the size based on the new string.
        }

        constexpr void Set_Ascii(const char text) {
            // Set the Text to the ASCII data.
            Text = std::variant<char, const char*>(text);
            Size = 1; // Update the size to 1 since it's a single character.
        }
        
        /**
         * @brief Checks if the UTF object has a default text.
         * @return true if the UTF object has a default text, false otherwise.
         */
        constexpr bool Has_Default_Text() {
            return Is(COMPACT_STRING_FLAG::IS_ASCII) ? std::get<char>(Text) == ' ' : std::get<const char*>(Text)[0] == ' ';
        }
    };

    // Instead of reconstructing new strings every time, this class stores the components, and then only one time constructs the final string representation.
    class Super_String{
    public:
        std::vector<Compact_String> Data;
        unsigned int Current_Index = 0;

        /**
         * @brief Constructs a Super_String object with a specified final size.
         * 
         * This constructor initializes the Super_String object by resizing the internal
         * data storage to the specified final size and sets the current index to 0.
         * 
         * @param Final_Size The final size to which the internal data storage should be resized.
         *                   Default value is 1.
         */
        Super_String(const unsigned int Final_Size = 1) {
            Data.resize(Final_Size);
            Current_Index = 0;
        }

        Super_String(const std::initializer_list<Compact_String>& data) : Super_String(data.size()) {
            for (const auto& item : data) {
                Add(item);
            }
        }

        /**
         * @brief Clears the contents of the Super_String.
         * 
         * This function resets the current index back to the start of the vector,
         * effectively clearing any stored data.
         */
        constexpr void Clear(){
            // Set the current index back to the start of the vector.
            Current_Index = 0;
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
        void Add(const char* data, const int size){
            // Store the string in the data vector.
            Data[Current_Index++] = Compact_String(data, size);
        }

        /**
         * @brief Adds a character to the Super_String.
         * 
         * This function stores the given character in the data vector
         * and increments the current index.
         * 
         * @param data The character to be added to the Super_String.
         */
        void Add(const char data){
            // Store the character in the data vector.
            Data[Current_Index++] = Compact_String(data);
        }

        /**
         * @brief Adds a string to the data vector.
         * 
         * This function stores the provided string in the data vector by compacting it
         * and placing it at the current index. The current index is then incremented.
         * 
         * @param data The string to be added to the data vector.
         */
        void Add(const std::string& data){
            // Store the string in the data vector.
            Data[Current_Index++] = Compact_String(data.data(), data.size());
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
        void Add(const Super_String* other){
            // Copy the contents of the other Super_String into the Data vector.
            for (unsigned int i = 0; i < other->Current_Index; i++){

                Data[Current_Index++] = other->Data[i];
            }
        }
        
        /**
         * @brief Add the contents of another Super_String to this one.
         * @param other The Super_String to add.
         * @param Expected If true, the size of the Data vector will not be changed.
         * @details This function is used to concatenate Super_Strings.
         */
        void Add(const Super_String& other){
            // Copy the contents of the other Super_String into the Data vector.
            for (unsigned int i = 0; i < other.Current_Index; i++){

                Data[Current_Index++] = other.Data[i];
            }
        }

        /**
         * @brief Adds a Compact_String to the data vector.
         * 
         * This function appends the given Compact_String to the current position
         * in the Data vector and then increments the Current_Index.
         * 
         * @param other The Compact_String to add to the data vector.
         */
        void Add(const Compact_String& other){
            // Store the Compact_String in the data vector.
            Data[Current_Index++] = other;
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
        std::string To_String() const {
            unsigned int Overall_Size = 0;

            // Calculate the total size of all the strings stored in the Data vector.
            for(unsigned int i = 0; i < Current_Index; i++){
                Overall_Size += Data[i].Size;
            }

            // Resize a std::string to the total size.
            std::string result;
            result.resize(Overall_Size);

            // Copy the contents of the Data vector into the std::string.
            int Current_UTF_Insert_Index = 0;
            for(unsigned int i = 0; i < Current_Index; i++){
                Compact_String data = Data[i];

                // Size of ones are always already loaded from memory into a char.
                if (data.Size > 1){
                    // Replace the current contents of the string with the contents of the Unicode data.
                    result.replace(Current_UTF_Insert_Index, data.Size, data.Get_Unicode());

                    Current_UTF_Insert_Index += data.Size;
                }
                else{
                    // Add the single character to the string.
                    result[Current_UTF_Insert_Index++] = data.Get_Ascii();
                }
            }

            return result;
        }
    };

}

#endif