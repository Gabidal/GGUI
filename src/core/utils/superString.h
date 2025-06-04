#ifndef _SUPER_STRING_H_
#define _SUPER_STRING_H_

#include <string>
#include <cstring>
#include <array>
#include <vector>
#include <variant>
#include <initializer_list>

namespace GGUI{
    
    namespace COMPACT_STRING_FLAG{
        constexpr inline unsigned char IS_ASCII          = 1 << 0;
        constexpr inline unsigned char IS_UNICODE        = 1 << 1;
    };

    /**
     * @class Compact_String
     * @brief A lightweight string class optimized for compact storage of ASCII and Unicode strings.
     *
     * The Compact_String class provides an efficient way to store and manipulate short strings,
     * optimizing for the case where the string is a single ASCII character. For longer strings,
     * it stores a pointer to a null-terminated C-style string. The class uses a std::variant to
     * hold either a single character or a pointer to a string, and maintains the size of the string.
     *
     * Key Features:
     * - Stores either a single ASCII character or a pointer to a C-style string.
     * - Provides constructors for ASCII characters, C-style strings, and explicit size/force Unicode.
     * - Offers fast type and content checks for ASCII and Unicode representations.
     * - Supports subscript operator for character access.
     * - Utility methods for getting and setting ASCII/Unicode data.
     * - Designed for use in scenarios where memory efficiency and fast type checks are important.
     *
     * Usage Notes:
     * - The default constructor is intended only for resizing containers and should not be used directly.
     * - The class does not manage the lifetime of external string data; ensure that any pointer passed
     *   to the class remains valid for the lifetime of the Compact_String instance.
     * - The class is constexpr-friendly for compile-time usage where possible.
     */
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
         * @brief Default copy constructor for Compact_String.
         *
         * Creates a new Compact_String object as a copy of an existing one.
         * This constructor performs a member-wise copy of the source object.
         *
         * @param other The Compact_String instance to copy from.
         */
        constexpr Compact_String(const Compact_String&) = default;

        /**
         * @brief Move constructor for Compact_String.
         *
         * Constructs a new Compact_String by transferring the resources from another
         * Compact_String instance. The source object is left in a valid but unspecified state.
         *
         * @param other The Compact_String instance to move from.
         */
        constexpr Compact_String(Compact_String&&) = default;

        /**
         * @brief Default copy assignment operator for Compact_String.
         *
         * Assigns the contents of another Compact_String to this one.
         * The default implementation performs a member-wise copy of all fields.
         *
         * @param other The Compact_String instance to copy from.
         * @return Reference to this Compact_String after assignment.
         */
        constexpr Compact_String& operator=(const Compact_String&) = default;

        /**
         * @brief Default move assignment operator for Compact_String.
         *
         * Allows assigning the contents of another Compact_String to this one using move semantics.
         * The default implementation efficiently transfers resources from the source object,
         * leaving it in a valid but unspecified state.
         *
         * @return Reference to this Compact_String after assignment.
         */
        constexpr Compact_String& operator=(Compact_String&&) = default;

        /**
         * @brief Constructs a Compact_String object from a C-style string.
         * 
         * This constructor initializes the Compact_String object by determining the length of the input string.
         * If the length of the string is greater than 1, it stores the string data in Unicode_Data.
         * If the length of the string is 1 or less, it stores the single character in Ascii_Data.
         * 
         * @param data A pointer to a null-terminated C-style string.
         */
        constexpr Compact_String(const char* data){
            // Store the string as Unicode data if its length is greater than 1.
            // Store the single character as ASCII data.
            Get_Length(data) > 1 ? 
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
        constexpr Compact_String(const char* data, const unsigned int size, const bool Force_Unicode = false){
            // Determine data storage based on size and Force_Unicode flag.
            // Store as Unicode data if size is greater than 1 or forced.
            // Store as a single ASCII character.
            (size > 1 || Force_Unicode) ? 
            Set_Unicode(data) : 
            Set_Ascii(data[0]);

            // If force unicode has been issued, then the size is probably a non-unicode standard size of zero or one, so we need to override the size.
            if (Force_Unicode)
                Size = size;
        }

        /**
         * @brief Checks if a specific UTF flag is set.
         * @param cs_flag The UTF flag to check.
         * @return True if the flag is set, otherwise false.
         */
        constexpr bool Is(unsigned char cs_flag) const {
            return (cs_flag == COMPACT_STRING_FLAG::IS_ASCII && Size == 1) || (cs_flag == COMPACT_STRING_FLAG::IS_UNICODE && Size > 1) ? true : false;
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
            Size = Get_Length(text); // Update the size based on the new string.
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
        constexpr bool Has_Default_Text() const {
            return Is(COMPACT_STRING_FLAG::IS_ASCII) ? std::get<char>(Text) == ' ' : std::get<const char*>(Text)[0] == ' ';
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
        constexpr size_t Get_Length(const char* str) {
            size_t length = 0;
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
     * @tparam MaxSize The maximum number of Compact_String objects that can be stored in the Super_String.
     *
     * @note This class is designed for performance and memory efficiency, making it suitable for scenarios
     *       where frequent string concatenation and manipulation are required.
     */
    template<std::size_t MaxSize>
    class Super_String{
    public:
        std::array<Compact_String, MaxSize> Data;
        unsigned int Current_Index = 0;
        unsigned int Liquefied_Size = 0;

        /**
         * @brief Default constexpr constructor for the Super_String class.
         *
         * Initializes a Super_String object with default values at compile time.
         * This constructor does not perform any custom initialization logic.
         */
        constexpr Super_String() = default;

        /**
         * @brief Constructs a Super_String from an initializer list of Compact_String objects.
         *
         * This constructor allows you to initialize a Super_String with a list of Compact_String
         * instances. Each item in the initializer list is added to the Super_String using the Add method.
         *
         * @param data An initializer list containing Compact_String objects to be added to the Super_String.
         */
        constexpr Super_String(const std::initializer_list<Compact_String>& data) {
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
            Liquefied_Size = 0;
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
        constexpr void Add(const char* data, const int size){
            // Store the string in the data vector.
            Data[Current_Index++] = Compact_String(data, size);
            Liquefied_Size += size; // Update the liquefied size with the size of the new string.
        }

        /**
         * @brief Adds a character to the Super_String.
         * 
         * This function stores the given character in the data vector
         * and increments the current index.
         * 
         * @param data The character to be added to the Super_String.
         */
        constexpr void Add(const char data){
            // Store the character in the data vector.
            Data[Current_Index++] = Compact_String(data);
            Liquefied_Size += 1; // Update the liquefied size with the size of the new character.
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
        constexpr void Add(const Super_String<OtherMaxSize>* other){
            // Copy the contents of the other Super_String into the Data vector.
            for (unsigned int i = 0; i < other->Current_Index; i++){
                Data[Current_Index++] = other->Data[i];
                Liquefied_Size += other->Data[i].Size; // Update the liquefied size with the size of the new string.
            }
        }
        
        /**
         * @brief Add the contents of another Super_String to this one.
         * @param other The Super_String to add.
         * @param Expected If true, the size of the Data vector will not be changed.
         * @details This function is used to concatenate Super_Strings.
         */
        template<std::size_t OtherMaxSize>
        constexpr void Add(const Super_String<OtherMaxSize>& other){
            // Copy the contents of the other Super_String into the Data vector.
            for (unsigned int i = 0; i < other.Current_Index; i++){
                Data[Current_Index++] = other.Data[i];
                Liquefied_Size += other.Data[i].Size; // Update the liquefied size with the size of the new string.
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
        constexpr void Add(const Compact_String& other){
            // Store the Compact_String in the data vector.
            Data[Current_Index++] = other;
            Liquefied_Size += other.Size; // Update the liquefied size with the size of the new Compact_String.
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
        std::string To_String() {
            // Resize a std::string to the total size.
            std::string result;
            result.resize(Liquefied_Size);

            // Copy the contents of the Data vector into the std::string.
            int Current_UTF_Insert_Index = 0;
            for(unsigned int i = 0; i < Current_Index; i++){
                const Compact_String& data = Data[i];

                if (data.Size == 0)
                    break;

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

    inline std::string To_String(std::vector<Compact_String>* Data, unsigned int Liquefied_Size) {
        // Resize a std::string to the total size.
        std::string result;
        result.resize(Liquefied_Size);

        // Copy the contents of the Data vector into the std::string.
        int Current_UTF_Insert_Index = 0;
        for(unsigned int i = 0; i < Data->size(); i++){
            const Compact_String& data = Data->at(i);

            if (data.Size == 0)
                break;

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

}

#endif