#ifndef _UNITS_H_
#define _UNITS_H_

#include <string>

#include "Super_String.h"
#include "Constants.h"
#include "Color.h"

namespace GGUI{
    
    // Literal type
    class FVector2{
    public:
        float X = 0;
        float Y = 0;

        /**
         * @brief Default constructor
         *
         * Initializes the FVector2 with the given x and y values.
         *
         * @param x The x-coordinate. Default is 0.0f.
         * @param y The y-coordinate. Default is 0.0f.
         */
        constexpr FVector2(float x = 0.0f, float y = 0.0f) noexcept
            : X(x), Y(y) {}

        /**
         * @brief Copy constructor
         *
         * Initializes the FVector2 by copying another FVector2.
         *
         * @param other The FVector2 to copy.
         */
        constexpr FVector2(const FVector2& other) noexcept = default;

        /**
         * @brief Move constructor
         *
         * Initializes the FVector2 by moving another FVector2.
         *
         * @param other The FVector2 to move.
         */
        constexpr FVector2(FVector2&& other) noexcept = default;

        /**
         * @brief Copy assignment operator
         *
         * Assigns another FVector2 to this one by copying its values.
         *
         * @param other The FVector2 to copy.
         * @return A reference to this FVector2.
         */
        constexpr FVector2& operator=(const FVector2& other) noexcept = default;

        /**
         * @brief Move assignment operator
         *
         * Moves the values from another FVector2 to this one.
         *
         * @param other The FVector2 to move.
         * @return A reference to this FVector2.
         */
        constexpr FVector2& operator=(FVector2&& other) noexcept = default;

        /**
         * @brief + operator with a float
         *
         * Adds a float to FVector2, creating a new FVector2.
         *
         * @param num The float to add.
         * @return A new FVector2 with the added float.
         */
        constexpr FVector2 operator+(float num) const noexcept {
            return FVector2(X + num, Y + num);
        }

        /**
         * @brief - operator with a float
         *
         * Subtracts a float from FVector2, creating a new FVector2.
         *
         * @param num The float to subtract.
         * @return A new FVector2 with the subtracted float.
         */
        constexpr FVector2 operator-(float num) const noexcept {
            return FVector2(X - num, Y - num);
        }


        /**
         * @brief * operator with a float
         *
         * Multiplies the FVector2 by a float, creating a new FVector2.
         *
         * @param num The float to multiply.
         * @return A new FVector2 with the multiplied float.
         */
        constexpr FVector2 operator*(float num) const noexcept {
            return FVector2(X * num, Y * num);
        }
    };
    
    // Literal type
    class FVector3 : public FVector2 {
    public:
        float Z = 0;

        /**
         * @brief Default constructor
         *
         * Initializes the FVector3 with the given x, y, and z values.
         *
         * @param x The x-coordinate. Default is 0.0f.
         * @param y The y-coordinate. Default is 0.0f.
         * @param z The z-coordinate. Default is 0.0f.
         */
        constexpr FVector3(float x = 0.0f, float y = 0.0f, float z = 0.0f) noexcept
            : FVector2(x, y), Z(z) {}

        /**
         * @brief Copy constructor
         *
         * Initializes the FVector3 by copying another FVector3.
         *
         * @param other The FVector3 to copy.
         */
        constexpr FVector3(const FVector3& other) noexcept = default;

        /**
         * @brief Move constructor
         *
         * Initializes the FVector3 by moving another FVector3.
         *
         * @param other The FVector3 to move.
         */
        constexpr FVector3(FVector3&& other) noexcept = default;

        /**
         * @brief Copy assignment operator
         *
         * Assigns another FVector3 to this one by copying its values.
         *
         * @param other The FVector3 to copy.
         * @return A reference to this FVector3.
         */
        constexpr FVector3& operator=(const FVector3& other) noexcept = default;

        /**
         * @brief Move assignment operator
         *
         * Assigns another FVector3 to this one by moving its values.
         *
         * @param other The FVector3 to move.
         * @return A reference to this FVector3.
         */
        constexpr FVector3& operator=(FVector3&& other) noexcept = default;

        /**
         * @brief + operator with a float
         *
         * Adds a float to FVector3, creating a new FVector3.
         *
         * @param num The float to add.
         * @return A new FVector3 with the added float.
         */
        constexpr FVector3 operator+(float num) const noexcept {
            return FVector3(X + num, Y + num, Z + num);
        }


        /**
         * @brief - operator with a float
         *
         * Subtracts a float from FVector3, creating a new FVector3.
         *
         * @param num The float to subtract.
         * @return A new FVector3 with the subtracted float.
         */
        constexpr FVector3 operator-(float num) const noexcept {
            return FVector3(X - num, Y - num, Z - num);
        }

        /**
         * @brief * operator with a float
         *
         * Multiplies the FVector3 by a float, creating a new FVector3.
         *
         * @param num The float to multiply.
         * @return A new FVector3 with the multiplied float.
         */
        constexpr FVector3 operator*(float num) const noexcept {
            return FVector3(X * num, Y * num, Z * num);
        }

        /**
         * @brief + operator with another FVector3
         *
         * Adds another FVector3 to this one, creating a new FVector3.
         *
         * @param other The FVector3 to add.
         * @return A new FVector3 with the added values.
         */
        constexpr FVector3 operator+(const FVector3& other) const noexcept {
            return FVector3(X + other.X, Y + other.Y, Z + other.Z);
        }

        /**
         * @brief - operator with another FVector3
         *
         * Subtracts another FVector3 from this one, creating a new FVector3.
         *
         * @param other The FVector3 to subtract.
         * @return A new FVector3 with the subtracted values.
         */
        constexpr FVector3 operator-(const FVector3& other) const noexcept {
            return FVector3(X - other.X, Y - other.Y, Z - other.Z);
        }

        /**
         * @brief * operator with another FVector3 (component-wise multiplication)
         *
         * Performs component-wise multiplication with another FVector3, creating a new FVector3.
         *
         * @param other The FVector3 to multiply.
         * @return A new FVector3 with the component-wise multiplied values.
         */
        constexpr FVector3 operator*(const FVector3& other) const noexcept {
            return FVector3(X * other.X, Y * other.Y, Z * other.Z);
        }
    };

    class IVector3{
    public:
        int X = 0;  //Horizontal
        int Y = 0;  //Vertical
        int Z = 0;  //priority (the higher the more likely it will be at top).

        /**
         * @brief Default constructor
         *
         * Initializes the IVector3 with the given x, y and z values.
         *
         * @param x The x-coordinate. Default is 0.
         * @param y The y-coordinate. Default is 0.
         * @param z The z-coordinate. Default is 0.
         */
        constexpr IVector3(int x = 0, int y = 0, int z = 0) noexcept
            : X(x), Y(y), Z(z) {}

        /**
         * @brief Copy constructor
         *
         * Initializes the IVector3 by copying another IVector3.
         *
         * @param other The IVector3 to copy.
         */
        constexpr IVector3(const IVector3& other) noexcept = default;

        /**
         * @brief Move constructor
         *
         * Initializes the IVector3 by moving another IVector3.
         *
         * @param other The IVector3 to move.
         */
        constexpr IVector3(IVector3&& other) noexcept = default;

        /**
         * @brief Copy assignment operator
         *
         * Assigns another IVector3 to this one by copying its values.
         *
         * @param other The IVector3 to copy.
         * @return A reference to this IVector3.
         */
        constexpr IVector3& operator=(const IVector3& other) noexcept = default;

        /**
         * @brief Move assignment operator
         *
         * Moves the values from another IVector3 to this one.
         *
         * @param other The IVector3 to move.
         * @return A reference to this IVector3.
         */
        constexpr IVector3& operator=(IVector3&& other) noexcept = default;

        /**
         * @brief += operator with a pointer to an IVector3
         *
         * Adds the values of the IVector3 pointed to by the pointer to this IVector3.
         *
         * @param other The pointer to the IVector3 to add.
         */
        constexpr void operator+=(IVector3* other) noexcept {
            X += other->X;
            Y += other->Y;
            Z += other->Z;
        }


        /**
         * @brief += operator with an FVector2
         *
         * Adds the values of the FVector2 to this IVector3.
         *
         * @param other The FVector2 to add.
         */
        constexpr void operator+=(FVector2 other) noexcept {
            X += other.X;
            Y += other.Y;
        }

        /**
         * @brief += operator with another IVector3
         *
         * Adds the values of another IVector3 to this one.
         *
         * @param other The IVector3 to add.
         */
        constexpr void operator+=(IVector3 other) noexcept {
            X += other.X;  // Add the x-coordinate
            Y += other.Y;  // Add the y-coordinate
            Z += other.Z;  // Add the z-coordinate
        }

        /**
         * @brief + operator with another IVector3
         *
         * Creates a new IVector3 with the added values of this IVector3 and the other IVector3.
         *
         * @param other The IVector3 to add.
         * @return A new IVector3 with the added values.
         */
        constexpr IVector3 operator+(const IVector3& other) const noexcept {
            return IVector3(X + other.X, Y + other.Y, Z + other.Z);
        }


        /**
         * @brief * operator with a float
         *
         * Multiplies the IVector3 by a float, creating a new IVector3.
         *
         * @param num The float to multiply.
         * @return A new IVector3 with the multiplied float.
         */
        constexpr IVector3 operator*(float num) const noexcept {
            return IVector3(X * num, Y * num, Z * num); // Multiply each coordinate by num
        }

        /**
         * @brief Converts the IVector3 to a string.
         * 
         * This function returns a string in the format "X, Y, Z" where X, Y, and Z are the coordinates of the IVector3.
         * The output string is designed to be human-readable, and is not designed to be efficient for serialization or other purposes.
         * 
         * @return A string representation of the IVector3.
         */
        std::string To_String(){
            return std::to_string(X) + ", " + std::to_string(Y) + ", " + std::to_string(Z);
        }
    
        /**
         * @brief Converts the IVector3 to a string.
         * 
         * This function returns a string in the format "X, Y, Z" where X, Y, and Z are the coordinates of the IVector3.
         * The output string is designed to be human-readable, and is not designed to be efficient for serialization or other purposes.
         * 
         * @return A string representation of the IVector3.
         */
        std::string To_String() const {
            return std::to_string(X) + ", " + std::to_string(Y) + ", " + std::to_string(Z);
        }
    };

    class UTF{
    public:
        unsigned char FLAGS = UTF_FLAG::IS_ASCII;

        char Ascii = ' ';
        const char* Unicode = " ";
        int Unicode_Length = 1; // Does not include the null terminator.

        RGBA Foreground; 
        RGBA Background;

        UTF(){}

        ~UTF(){}

        /**
         * @brief Copy constructor for the UTF class.
         *
         * This constructor initializes a new UTF object as a copy of another UTF object.
         *
         * @param other The UTF object to copy.
         */
        constexpr UTF(const GGUI::UTF& other)
            : FLAGS(other.FLAGS),             // Initialize FLAGS from the other object
              Ascii(other.Ascii),             // Initialize Ascii from the other object
              Unicode(other.Unicode),         // Initialize Unicode from the other object
              Unicode_Length(other.Unicode_Length), // Initialize Unicode_Length from the other object
              Foreground(other.Foreground),   // Initialize Foreground from the other object
              Background(other.Background)    // Initialize Background from the other object
        {}

        /**
         * @brief Constructs a new UTF object from a single character and a pair of foreground and background colors.
         * @param data The character to store in the UTF object.
         * @param color A pair of RGB objects representing the foreground and background colors. If not provided, defaults to {{}, {}}.
         * @details The FLAGS of the UTF object are set to UTF_FLAG::IS_ASCII, and the Ascii member is set to the provided character.
         *          The Foreground and Background members are set to the provided colors.
         */
        UTF(char data, std::pair<RGB, RGB> color = {{}, {}}){
            Ascii = data;
            Foreground = {color.first};
            Background = {color.second};
            FLAGS = UTF_FLAG::IS_ASCII;
        }

        /**
         * @brief Constructs a new UTF object from a C-style string and a pair of foreground and background colors.
         * @param data The C-style string to store in the UTF object.
         * @param color A pair of RGB objects representing the foreground and background colors. If not provided, defaults to {{}, {}}.
         * @details The FLAGS of the UTF object are set to UTF_FLAG::IS_UNICODE, and the Unicode member is set to the provided string.
         *          The Foreground and Background members are set to the provided colors.
         */
        UTF(const char* data, std::pair<RGB, RGB> color = {{}, {}}) {
            Unicode = data;  // Store the C-style string in the Unicode member
            Unicode_Length = std::strlen(data);  // Calculate and store the length of the string
            
            Foreground = {color.first};  // Set the foreground color
            Background = {color.second};  // Set the background color
            FLAGS = UTF_FLAG::IS_UNICODE;  // Set the FLAGS to indicate Unicode data
        }

        /**
         * @brief Constructs a new UTF object from a std::string and a pair of foreground and background colors.
         * @param data The std::string to store in the UTF object.
         * @param color A pair of RGB objects representing the foreground and background colors. If not provided, defaults to {{}, {}}.
         * @details The FLAGS of the UTF object are set to UTF_FLAG::IS_UNICODE, and the Unicode member is set to the provided string.
         *          The Foreground and Background members are set to the provided colors.
         */
        UTF(const std::string& data, std::pair<RGB, RGB> color = {{}, {}}){
            Unicode = data.data();
            Unicode_Length = data.size() - 1;
            
            Foreground = {color.first};
            Background = {color.second};
            FLAGS = UTF_FLAG::IS_UNICODE;
        }

        /**
         * @brief Constructs a new UTF object from a Compact_String and a pair of foreground and background colors.
         * @param CS The Compact_String to store in the UTF object.
         * @param color A pair of RGB objects representing the foreground and background colors. If not provided, defaults to {{}, {}}.
         * @details The FLAGS of the UTF object are set depending on the size of the Compact_String.
         *          If the size is 1, the FLAGS are set to UTF_FLAG::IS_ASCII and the Ascii member is set to the provided character.
         *          If the size is greater than 1, the FLAGS are set to UTF_FLAG::IS_UNICODE and the Unicode member is set to the provided string.
         *          The Foreground and Background members are set to the provided colors.
         */
        UTF(const Compact_String CS, std::pair<RGB, RGB> color = {{}, {}}){
            if (CS.Size == 1){
                // If the size of the Compact_String is 1, set the FLAGS to IS_ASCII and the Ascii member to the provided character.
                Ascii = CS.Data.Ascii_Data;
                FLAGS = UTF_FLAG::IS_ASCII;
            }
            else{
                // If the size of the Compact_String is greater than 1, set the FLAGS to IS_UNICODE and the Unicode member to the provided string.
                Unicode = CS.Data.Unicode_Data;
                Unicode_Length = CS.Size;
                FLAGS = UTF_FLAG::IS_UNICODE;
            }
            
            // Set the foreground and background colors.
            Foreground = {color.first};
            Background = {color.second};
        }

        /**
         * @brief Checks if a specific UTF flag is set.
         * @details This function takes a UTF flag as a parameter 
         *          and checks if it is set in the FLAGS member.
         * 
         * @param utf_flag The UTF flag to check.
         * @return True if the flag is set, otherwise false.
         */
        bool Is(unsigned char utf_flag) {
            // Perform bitwise AND to check if the utf_flag is set in FLAGS
            return (FLAGS & utf_flag) != 0;
        }

        /**
         * @brief Sets a specific UTF flag.
         * @details This function takes a UTF flag as a parameter and sets it in the FLAGS member.
         * 
         * @param utf_flag The UTF flag to set.
         */
        void Set_Flag(unsigned char utf_flag){
            // Perform bitwise OR to set the utf_flag in FLAGS
            FLAGS |= utf_flag;
        }

        /**
         * @brief Sets the foreground color of the UTF element.
         * @details This function takes a RGB color as a parameter and sets it as the foreground color of the UTF element.
         * 
         * @param color The RGB color to set as the foreground color.
         */
        void Set_Foreground(RGB color){
            Foreground = color;
        }

        /**
         * @brief Sets the background color of the UTF element.
         * @details This function takes an RGB color as a parameter and sets it as the background color of the UTF element.
         * 
         * @param color The RGB color to set as the background color.
         */
        void Set_Background(RGB color) {
            Background = color;
        }

        /**
         * @brief Sets the foreground and background color of the UTF element.
         * @details This function takes a pair of RGB colors as a parameter and sets the first element of the pair as the foreground color and the second element of the pair as the background color of the UTF element.
         * 
         * @param primals A pair of RGB colors. The first element of the pair is the foreground color and the second element of the pair is the background color.
         */
        void Set_Color(std::pair<RGB, RGB> primals){
            Foreground = primals.first;
            Background = primals.second;
        }

        /**
         * @brief Sets the text of the UTF element.
         * @details This function takes a std::string as a parameter and sets it as the text of the UTF element.
         *          The text is stored as a pointer to the data of the std::string, and the length of the text is
         *          stored in the Unicode_Length member variable.
         * 
         * @param data The std::string to set as the text of the UTF element.
         */
        void Set_Text(std::string data){
            Unicode = data.data();
            Unicode_Length = data.size() -1;
            FLAGS = UTF_FLAG::IS_UNICODE;
        }

        /**
         * @brief Sets the text of the UTF element to a single character.
         * @details This function takes a char as a parameter and sets it as the text of the UTF element.
         *          The text is stored in the Ascii member variable.
         * 
         * @param data The char to set as the text of the UTF element.
         */
        void Set_Text(char data){
            Ascii = data;
            FLAGS = UTF_FLAG::IS_ASCII;
        }

        /**
         * @brief Sets the text of the UTF element to a null-terminated string.
         * @details This function takes a pointer to a null-terminated string as a parameter and sets it as the text of the UTF element.
         *          The text is stored in the Unicode member variable, and the length of the text is
         *          stored in the Unicode_Length member variable.
         *
         * @param data The null-terminated string to set as the text of the UTF element.
         */
        void Set_Text(const char* data){
            Unicode = data;
            Unicode_Length = std::strlen(data);
            FLAGS = UTF_FLAG::IS_UNICODE;
        }


        /**
         * @brief Sets the text of the UTF element to that of another UTF element.
         * @details This function takes another UTF element as a parameter and sets the text of the current UTF element to that of the other UTF element.
         *          The text is copied from the other UTF element, and the flags are also copied.
         *
         * @param other The other UTF element to copy the text from.
         */
        void Set_Text(UTF other){
            Ascii = other.Ascii;
            Unicode = other.Unicode;
            Unicode_Length = other.Unicode_Length;
            FLAGS = other.FLAGS;
        }

        /**
         * @brief Converts the UTF character to a string.
         *
         * This function converts the UTF character to a string by combining the foreground and background colour
         * strings with the character itself.
         *
         * @return The string representation of the UTF character.
         */
        std::string To_String();
        
        /**
         * @brief Converts the UTF character to an encoded string.
         *
         * This function converts the UTF character to an encoded string by applying
         * encoding flags and combining the foreground and background colour strings
         * with the character itself.
         *
         * @return The encoded string representation of the UTF character.
         */
        std::string To_Encoded_String();

        /**
         * @brief Converts the UTF character to a string.
         *
         * This function converts the UTF character to a string by combining the foreground and background colour
         * strings with the character itself.
         *
         * @param Result The result string.
         * @param Text_Overhead The foreground colour and style as a string.
         * @param Background_Overhead The background colour and style as a string.
         * @param Text_Colour The foreground colour as a string.
         * @param Background_Colour The background colour as a string.
         */
        void To_Super_String(GGUI::Super_String* Result, Super_String* Text_Overhead, Super_String* Background_Overhead, Super_String* Text_Colour, Super_String* Background_Colour);
        
        /**
         * @brief Converts the UTF character to an encoded Super_String.
         *
         * This function converts the UTF character to an encoded Super_String by applying
         * encoding flags and combining the foreground and background colour strings
         * with the character itself.
         *
         * @param Result The Super_String to which the encoded string will be added.
         * @param Text_Overhead The Super_String where the foreground colour overhead will be stored.
         * @param Background_Overhead The Super_String where the background colour overhead will be stored.
         * @param Text_Colour The Super_String where the foreground colour will be stored.
         * @param Background_Colour The Super_String where the background colour will be stored.
         */
        void To_Encoded_Super_String(Super_String* Result, Super_String* Text_Overhead, Super_String* Background_Overhead, Super_String* Text_Colour, Super_String* Background_Colour);

        /**
         * @brief Assign a character to the UTF object.
         *
         * This operator assigns a character to the UTF object and sets the text of the UTF object to the character.
         * The flags are cleared, and the foreground and background colours are set to their default values.
         *
         * @param text The character to assign.
         */
        void operator=(char text){
            Set_Text(text);
        }

        /**
         * @brief Assigns a string to the UTF object.
         * @details This operator assigns a string to the UTF object and sets the text of the UTF object to the string.
         *          The flags are cleared, and the foreground and background colours are set to their default values.
         *
         * @param text The string to assign.
         */
        void operator=(const std::string& text){
            Set_Text(text);
        }

        /**
         * @brief Assigns a UTF object to another UTF object.
         * @details This operator assigns a UTF object to another UTF object and sets the text, flags, foreground, and
         *          background colours of the assigned UTF object to those of the other UTF object.
         *
         * @param other The UTF object to assign.
         * @return The assigned UTF object.
         */
        UTF& operator=(const UTF& other){
            Ascii = other.Ascii;
            Unicode = other.Unicode;
            Unicode_Length = other.Unicode_Length;
            FLAGS = other.FLAGS;
            Foreground = other.Foreground;
            Background = other.Background;

            return *this;
        }

        /**
         * @brief Checks if the UTF object has a default text.
         * @details This function checks if the UTF object has a default text, which is a space character.
         *          It returns true if the UTF object has a default text, and false if it does not.
         *
         * @return true if the UTF object has a default text, false if it does not.
         */
        inline bool Has_Default_Text(){
            if (Is(UTF_FLAG::IS_ASCII))
                return Ascii == ' ';
            else
                return Unicode[0] == ' ';
        }

    };

    enum class STAIN_TYPE{
        CLEAN = 0,              // No change
        COLOR = 1 << 0,         // BG and other color related changes
        EDGE = 1 << 1,          // Title and border changes.
        DEEP = 1 << 2,          // Children changes. Deep because the childs are connected via AST.
        STRETCH = 1 << 3,       // Width and or height changes.
        CLASS = 1 << 5,         // This is used to tell the renderer that there are still un_parsed classes.
        STATE = 1 << 6,         // This is for Switches that based on their state display one symbol differently. And also for state handlers.
        MOVE = 1 << 7,          // Enabled, to signal absolute position caching.
    };
 
    /**
     * @brief Performs bitwise OR operation on two STAIN_TYPE values.
     * @details This operator allows combining two STAIN_TYPE values using a bitwise OR operation.
     *          It returns the result as an unsigned integer.
     *
     * @param a The first STAIN_TYPE value.
     * @param b The second STAIN_TYPE value.
     * @return The result of the bitwise OR operation as an unsigned integer.
     */
    inline unsigned int operator|(STAIN_TYPE a, STAIN_TYPE b) {
        // Cast both STAIN_TYPE values to unsigned integers and perform the bitwise OR operation.
        return (unsigned int)a | (unsigned int)b;
    }

    /**
     * @brief Performs bitwise OR operation on a STAIN_TYPE value and an unsigned integer.
     * @details This operator allows combining a STAIN_TYPE value with an unsigned integer using a bitwise OR operation.
     *          It returns the result as an unsigned integer.
     *
     * @param a The STAIN_TYPE value.
     * @param b The unsigned integer.
     * @return The result of the bitwise OR operation as an unsigned integer.
     */
    inline unsigned int operator|(STAIN_TYPE a, unsigned int b){
        return (unsigned int)a | b;
    }

    /**
     * @brief Performs bitwise OR operation on an unsigned integer and a STAIN_TYPE value.
     * @details This operator allows combining an unsigned integer with a STAIN_TYPE value using a bitwise OR operation.
     *          It returns the result as an unsigned integer.
     *
     * @param a The unsigned integer.
     * @param b The STAIN_TYPE value.
     * @return The result of the bitwise OR operation as an unsigned integer.
     */
    inline unsigned int operator|(unsigned int a, STAIN_TYPE b){
        return a | (unsigned int)b;
    }

    class STAIN{
    public:
        STAIN_TYPE Type = STAIN_TYPE::CLEAN;

        /**
         * @brief Checks if the specified STAIN_TYPE is set in the current STAIN object.
         * @details This function checks if a given STAIN_TYPE flag is set in the current
         *          STAIN object. For the CLEAN flag, it checks if the type is less than
         *          or equal to CLEAN. For other flags, it performs a bitwise AND operation.
         *
         * @param f The STAIN_TYPE flag to check.
         * @return true if the specified flag is set; false otherwise.
         */
        bool is(STAIN_TYPE f) {
            // Special handling for the CLEAN flag
            if (f == STAIN_TYPE::CLEAN) {
                return Type <= f;
            }
            // Check if the specified flag is set using bitwise AND
            return ((unsigned int)Type & (unsigned int)f) == (unsigned int)f;
        }

        /**
         * @brief Clears a STAIN_TYPE flag from the current STAIN object.
         * @details This function clears a given STAIN_TYPE flag from the current
         *          STAIN object. It performs a bitwise AND operation with the
         *          bitwise compliment of the specified flag.
         *
         * @param f The STAIN_TYPE flag to clear.
         */
        void Clean(STAIN_TYPE f){
            Type = (STAIN_TYPE)((unsigned int)Type & ~(unsigned int)f);
        }

        /**
         * @brief Clears a STAIN_TYPE flag from the current STAIN object.
         * @details This function clears a given STAIN_TYPE flag from the current
         *          STAIN object. It performs a bitwise AND operation with the
         *          bitwise compliment of the specified flag.
         *
         * @param f The STAIN_TYPE flag to clear.
         */
        void Clean(unsigned int f){
            Type = (STAIN_TYPE)((unsigned int)Type & ~f);
        }

        /**
         * @brief Marks the specified STAIN_TYPE flag as dirty.
         * @details This function sets a given STAIN_TYPE flag on the current
         *          STAIN object, indicating that the element needs to be reprocessed
         *          for the specified attributes.
         *
         * @param f The STAIN_TYPE flag to set.
         */
        void Dirty(STAIN_TYPE f) {
            // Set the specified flag using bitwise OR
            Type = (STAIN_TYPE)((unsigned int)Type | (unsigned int)f);
        }

        /**
         * @brief Marks the specified STAIN_TYPE flag as dirty.
         * @details This function sets a given STAIN_TYPE flag on the current
         *          STAIN object, indicating that the element needs to be reprocessed
         *          for the specified attributes.
         *
         * @param f The STAIN_TYPE flag to set.
         */
        void Dirty(unsigned int f){
            // Set the specified flag using bitwise OR
            Type = (STAIN_TYPE)((unsigned int)Type | f);
        }

    };

    enum class Flags{
        Empty = 0,
        Border = 1 << 0,
        Text_Input = 1 << 1,
        Overflow = 1 << 2,
        Dynamic = 1 << 3,
        Horizontal = 1 << 4,
        Vertical = 1 << 5,
        Align_Left = 1 << 6,
        Align_Right = 1 << 7,
        Align_Center = 1 << 8,
    };
    
    /**
     * @brief Operator to combine two flags.
     * @details
     * This function takes two flags and returns a new flag that is the result of a
     * binary OR operation on the two input flags.
     * @param[in] a The first flag.
     * @param[in] b The second flag.
     * @return The result of the binary OR operation on the two input flags.
     */
    inline Flags operator|(Flags a, Flags b){
        return static_cast<Flags>(static_cast<int>(a) | static_cast<int>(b));
    }


    /**
     * @brief Checks if all the flags in 'b' are set in 'a'.
     * @details
     * This function takes two flags and returns true if all the flags in 'b'
     * are set in 'a'. Otherwise, it returns false.
     * @param[in] a The first flag.
     * @param[in] b The second flag.
     * @return True if all the flags in 'b' are set in 'a', false otherwise.
     */
    inline bool Is(Flags a, Flags b){
        return ((int)a & (int)b) == (int)b;
    }


    /**
     * @brief Checks if any of the flags in 'b' are set in 'a'.
     * @details
     * This function takes two flags and returns true if any of the flags in 'b'
     * are set in 'a'. Otherwise, it returns false.
     * @param[in] a The first flag.
     * @param[in] b The second flag.
     * @return True if any of the flags in 'b' are set in 'a', false otherwise.
     */
    inline bool Has(Flags a, Flags b){
        return ((int)a & (int)b) != 0;
    }


    enum class State{
        UNKNOWN,

        RENDERED,
        HIDDEN

    };

}

#endif