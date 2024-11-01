#ifndef _SUPER_STRING_H_
#define _SUPER_STRING_H_

#include <string>
#include <cstring>
#include <vector>

namespace GGUI{
    // And lighter-weight version of the UTF class. [Probably after making the RGBA use unsigned char instead of float, and thus making the overall size into 32 bits, replace this class with UTF.]
    class Compact_String{
    public:
        union{
            const char* Unicode_Data;
            char Ascii_Data;
        } Data = { nullptr };

        unsigned int Size = 0;

        /// Empty constructor for the Compact_String class. This is only used for resizing a vector of Compact_Strings, and should not be used directly.
        /// @warning Do not use this constructor directly, as it will not initialize the Data property.
        Compact_String() = default;

        /// Construct a Compact_String from a null-terminated string.
        /// @param data The string data.
        Compact_String(const char* data){
            Size = std::strlen(data); // Get the length of the string.

            if (Size > 1)
                Data.Unicode_Data = data; // Store the string data.
            else
                Data.Ascii_Data = data[0]; // Store the single character.
        }

        /// Construct a Compact_String from a single character.
        /// @param data The character data.
        Compact_String(char data){
            Data.Ascii_Data = data;
            Size = 1;
        }

        /// Construct a Compact_String with specified size.
        /// @param data The string data.
        /// @param size The length of the string.
        /// @param Force_Unicode Force the use of Unicode data storage, even if size is 1.
        Compact_String(const char* data, unsigned int size, bool Force_Unicode = false) {
            Size = size;

            // Determine data storage based on size and Force_Unicode flag.
            if (Size > 1 || Force_Unicode) {
                // Store as Unicode data if size is greater than 1 or forced.
                Data.Unicode_Data = data;
            } else {
                // Store as a single ASCII character.
                Data.Ascii_Data = data[0];
            }
        }

        /// Get the character at the specified index.
        /// @param index The index of the character.
        /// @return The character at the specified index.
        char operator[](unsigned int index) {
            // If the size is greater than 1, we have to index into the Unicode data.
            if (Size > 1)
                return Data.Unicode_Data[index];
            else
                // If the size is 1, we just return the ASCII data.
                return Data.Ascii_Data;
        }
    };

    // Instead of reconstructing new strings every time, this class stores the components, and then only one time constructs the final string representation.
    class Super_String{
    public:
        std::vector<Compact_String> Data;
        unsigned int Current_Index = 0;

        /// Construct a Super_String with a specified final size.
        /// @param Final_Size The size of the final string.
        Super_String(unsigned int Final_Size = 1) {
            Data.resize(Final_Size);
            Current_Index = 0;
        }

        /// Clear the current index to the start of the data vector.
        /// @details This allows for the same object to be used to construct multiple strings.
        void Clear(){
            /// Set the current index back to the start of the vector.
            Current_Index = 0;
        }

        /// Add a new string to the data vector.
        /// @param data The string to add.
        /// @param size The size of the string.
        void Add(const char* data, int size){
            /// Store the string in the data vector.
            Data[Current_Index++] = Compact_String(data, size);
        }

        /// Add a new single character to the data vector.
        /// @param data The character to add.
        void Add(char data){
            /// Store the character in the data vector.
            Data[Current_Index++] = Compact_String(data);
        }

        /// Add a new string to the data vector.
        /// @param data The string to add.
        void Add(const std::string& data){
            /// Store the string in the data vector.
            Data[Current_Index++] = Compact_String(data.data(), data.size());
        }

        /// Add the contents of another Super_String to this one.
        /// @param other The Super_String to add.
        /// @param Expected If true, the size of the Data vector will not be changed.
        /// @details This function is used to concatenate Super_Strings.
        void Add(Super_String* other, bool Expected = false){
            /// Enlarge the reservation if necessary.
            if (!Expected)
                Data.resize(Current_Index + other->Current_Index);

            /// Copy the contents of the other Super_String into the Data vector.
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
        void Add(Super_String& other, bool Expected = false){
            // enlarge the reservation
            if (!Expected)
                Data.resize(Current_Index + other.Current_Index);

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
        std::string To_String(){
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
                    result.replace(Current_UTF_Insert_Index, data.Size, data.Data.Unicode_Data);

                    Current_UTF_Insert_Index += data.Size;
                }
                else{
                    // Add the single character to the string.
                    result[Current_UTF_Insert_Index++] = data.Data.Ascii_Data;
                }
            }

            return result;
        }
    };

}

#endif