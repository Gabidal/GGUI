#ifndef _UNITS_H_
#define _UNITS_H_

#include <string>
#include <variant>





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
        short X = 0;  //Horizontal
        short Y = 0;  //Vertical
        short Z = 0;  //priority (the higher the more likely it will be at top).

        /**
         * @brief Default constructor
         *
         * Initializes the IVector3 with the given x, y and z values.
         *
         * @param x The x-coordinate. Default is 0.
         * @param y The y-coordinate. Default is 0.
         * @param z The z-coordinate. Default is 0.
         */
        constexpr IVector3(short x = 0, short y = 0, short z = 0) noexcept
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
         * @brief == operator with another IVector3
         * 
         * Compares the IVector3 with another IVector3.
         * 
         * @param other The IVector3 to compare with.
         * @return True if the IVector3s are equal, otherwise false.
         */
        constexpr bool operator==(const IVector3& other) const noexcept {
            return X == other.X && Y == other.Y && Z == other.Z; // Check if the coordinates are equal
        }

        /**
         * @brief != operator with another IVector3
         * 
         * Compares the IVector3 with another IVector3.
         * 
         * @param other The IVector3 to compare with.
         * @return False if the IVector3s are equal, otherwise true.
         */
        constexpr bool operator!=(const IVector3& other) const noexcept {
            return X != other.X || Y != other.Y || Z != other.Z; // Check if the coordinates are not equal
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

    class UTF {
    public:
        unsigned char FLAGS = UTF_FLAG::IS_ASCII;

        std::variant<char, const char*> Text;
        int Unicode_Length = 1; // Does not include the null terminator.

        RGBA Foreground;
        RGBA Background;

        UTF() {}

        ~UTF() {}

        /**
         * @brief Copy constructor for the UTF class.
         *
         * This constructor initializes a new UTF object as a copy of another UTF object.
         *
         * @param other The UTF object to copy.
         */
        constexpr UTF(const GGUI::UTF& other)
            : FLAGS(other.FLAGS),
              Text(other.Text),
              Unicode_Length(other.Unicode_Length),
              Foreground(other.Foreground),
              Background(other.Background) {}

        /**
         * @brief Constructs a new UTF object from a single character and a pair of foreground and background colors.
         * @param data The character to store in the UTF object.
         * @param color A pair of RGB objects representing the foreground and background colors. If not provided, defaults to {{}, {}}.
         */
        UTF(char data, std::pair<RGB, RGB> color = {{}, {}}) {
            Text = data;
            Foreground = {color.first};
            Background = {color.second};
            FLAGS = UTF_FLAG::IS_ASCII;
        }

        /**
         * @brief Constructs a new UTF object from a C-style string and a pair of foreground and background colors.
         * @param data The C-style string to store in the UTF object.
         * @param color A pair of RGB objects representing the foreground and background colors. If not provided, defaults to {{}, {}}.
         */
        UTF(const char* data, std::pair<RGB, RGB> color = {{}, {}}) {
            Text = data;
            Unicode_Length = std::strlen(data);
            Foreground = {color.first};
            Background = {color.second};
            FLAGS = UTF_FLAG::IS_UNICODE;
        }

        /**
         * @brief Constructs a new UTF object from a std::string and a pair of foreground and background colors.
         * @param data The std::string to store in the UTF object.
         * @param color A pair of RGB objects representing the foreground and background colors. If not provided, defaults to {{}, {}}.
         */
        UTF(const std::string& data, std::pair<RGB, RGB> color = {{}, {}}) {
            Text = data.c_str();
            Unicode_Length = data.size();
            Foreground = {color.first};
            Background = {color.second};
            FLAGS = UTF_FLAG::IS_UNICODE;
        }

        /**
         * @brief Constructs a new UTF object from a Compact_String and a pair of foreground and background colors.
         * @param CS The Compact_String to store in the UTF object.
         * @param color A pair of RGB objects representing the foreground and background colors. If not provided, defaults to {{}, {}}.
         */
        UTF(const Compact_String CS, std::pair<RGB, RGB> color = {{}, {}}) {
            if (CS.Size == 1) {
                Text = CS.Data.Ascii_Data;
                FLAGS = UTF_FLAG::IS_ASCII;
            } else {
                Text = CS.Data.Unicode_Data;
                Unicode_Length = CS.Size;
                FLAGS = UTF_FLAG::IS_UNICODE;
            }
            Foreground = {color.first};
            Background = {color.second};
        }

        /**
         * @brief Checks if a specific UTF flag is set.
         * @param utf_flag The UTF flag to check.
         * @return True if the flag is set, otherwise false.
         */
        bool Is(unsigned char utf_flag) {
            return (FLAGS & utf_flag) != 0;
        }

        // Fast comparison of type and content
        bool Is(const char* other){
            if (Is(UTF_FLAG::IS_ASCII)){
                return false;
            } else {
                return std::strcmp(std::get<const char*>(Text), other) == 0;
            }
        }

        // Fast comparison of type and content
        bool Is(char other){
            if (Is(UTF_FLAG::IS_ASCII)){
                return std::get<char>(Text) == other;
            } else {
                return false;
            }
        }

        /**
         * @brief Sets a specific UTF flag.
         * @param utf_flag The UTF flag to set.
         */
        void Set_Flag(unsigned char utf_flag) {
            FLAGS |= utf_flag;
        }

        /**
         * @brief Sets the foreground color of the UTF element.
         * @param color The RGB color to set as the foreground color.
         */
        void Set_Foreground(RGB color) {
            Foreground = color;
        }

        /**
         * @brief Sets the background color of the UTF element.
         * @param color The RGB color to set as the background color.
         */
        void Set_Background(RGB color) {
            Background = color;
        }

        /**
         * @brief Sets the foreground and background color of the UTF element.
         * @param primals A pair of RGB colors. The first element is the foreground color; the second is the background color.
         */
        void Set_Color(std::pair<RGB, RGB> primals) {
            Foreground = primals.first;
            Background = primals.second;
        }

        /**
         * @brief Sets the text of the UTF element.
         * @param data The std::string to set as the text.
         */
        void Set_Text(const std::string& data) {
            Text = data.c_str();
            Unicode_Length = data.size();
            FLAGS = UTF_FLAG::IS_UNICODE;
        }

        /**
         * @brief Sets the text of the UTF element to a single character.
         * @param data The character to set as the text.
         */
        void Set_Text(char data) {
            Text = data;
            FLAGS = UTF_FLAG::IS_ASCII;
        }

        /**
         * @brief Sets the text of the UTF element to a null-terminated string.
         * @param data The null-terminated string to set as the text.
         */
        void Set_Text(const char* data) {
            Text = data;
            Unicode_Length = std::strlen(data);
            FLAGS = UTF_FLAG::IS_UNICODE;
        }

        /**
         * @brief Sets the text of the UTF element to that of another UTF element.
         * @param other The other UTF element to copy the text from.
         */
        void Set_Text(const UTF& other) {
            Text = other.Text;
            Unicode_Length = other.Unicode_Length;
            FLAGS = other.FLAGS;
        }

        /**
         * @brief Converts the UTF character to a string.
         * @return The string representation of the UTF character.
         */
        std::string To_String();

        /**
         * @brief Converts the UTF character to an encoded string.
         * @return The encoded string representation of the UTF character.
         */
        std::string To_Encoded_String();

        /**
         * @brief Converts the UTF character to a Super_String.
         * @param Result The result string.
         * @param Text_Overhead The foreground colour and style as a string.
         * @param Background_Overhead The background colour and style as a string.
         * @param Text_Colour The foreground colour as a string.
         * @param Background_Colour The background colour as a string.
         */
        void To_Super_String(GGUI::Super_String* Result, Super_String* Text_Overhead, Super_String* Background_Overhead, Super_String* Text_Colour, Super_String* Background_Colour);

        /**
         * @brief Converts the UTF character to an encoded Super_String.
         * @param Result The Super_String to which the encoded string will be added.
         * @param Text_Overhead The Super_String where the foreground colour overhead will be stored.
         * @param Background_Overhead The Super_String where the background colour overhead will be stored.
         * @param Text_Colour The Super_String where the foreground colour will be stored.
         * @param Background_Colour The Super_String where the background colour will be stored.
         */
        void To_Encoded_Super_String(Super_String* Result, Super_String* Text_Overhead, Super_String* Background_Overhead, Super_String* Text_Colour, Super_String* Background_Colour);

        /**
         * @brief Assign a character to the UTF object.
         * @param text The character to assign.
         */
        void operator=(char text) {
            Set_Text(text);
        }

        /**
         * @brief Assigns a string to the UTF object.
         * @param text The string to assign.
         */
        void operator=(const std::string& text) {
            Set_Text(text);
        }

        /**
         * @brief Assigns a UTF object to another UTF object.
         * @param other The UTF object to assign.
         * @return The assigned UTF object.
         */
        UTF& operator=(const UTF& other) {
            Text = other.Text;
            Unicode_Length = other.Unicode_Length;
            FLAGS = other.FLAGS;
            Foreground = other.Foreground;
            Background = other.Background;
            return *this;
        }

        /**
         * @brief Checks if the UTF object has a default text.
         * @return true if the UTF object has a default text, false otherwise.
         */
        inline bool Has_Default_Text() {
            if (Is(UTF_FLAG::IS_ASCII)) {
                return std::get<char>(Text) == ' ';
            } else {
                return std::get<const char*>(Text)[0] == ' ';
            }
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
        FINALIZE = 1 << 8,      // This is used to signal that the element is finalized and the stylings are successfully been embedded.
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


    enum class STATE{
        UNKNOWN,

        INIT,
        DESTROYED,
        HIDDEN,
        SHOWN

    };

}

#endif
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

        /**
         * @brief Empty constructor for the Compact_String class. This is only used for resizing a vector of Compact_Strings, and should not be used directly.
         * @warning Do not use this constructor directly, as it will not initialize the Data property.
         * This constructor initializes a Compact_String object with default values.
         */
        Compact_String() = default;

        /**
         * @brief Constructs a Compact_String object from a C-style string.
         * 
         * This constructor initializes the Compact_String object by determining the length of the input string.
         * If the length of the string is greater than 1, it stores the string data in Unicode_Data.
         * If the length of the string is 1 or less, it stores the single character in Ascii_Data.
         * 
         * @param data A pointer to a null-terminated C-style string.
         */
        Compact_String(const char* data){
            Size = std::strlen(data); // Get the length of the string.

            if (Size > 1)
                Data.Unicode_Data = data; // Store the string data.
            else
                Data.Ascii_Data = data[0]; // Store the single character.
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
        Compact_String(char data){
            Data.Ascii_Data = data;
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
        char operator[](unsigned int index) const {
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

        /**
         * @brief Constructs a Super_String object with a specified final size.
         * 
         * This constructor initializes the Super_String object by resizing the internal
         * data storage to the specified final size and sets the current index to 0.
         * 
         * @param Final_Size The final size to which the internal data storage should be resized.
         *                   Default value is 1.
         */
        Super_String(unsigned int Final_Size = 1) {
            Data.resize(Final_Size);
            Current_Index = 0;
        }

        /**
         * @brief Clears the contents of the Super_String.
         * 
         * This function resets the current index back to the start of the vector,
         * effectively clearing any stored data.
         */
        void Clear(){
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
        void Add(const char* data, int size){
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
        void Add(char data){
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
        void Add(Super_String* other, bool Expected = false){
            // Enlarge the reservation if necessary.
            if (!Expected)
                Data.resize(Current_Index + other->Current_Index);

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
#ifndef _CONSTANTS_H_
#define _CONSTANTS_H_

#include <string>
#include <unordered_map>
#include <chrono>



namespace GGUI{

    class UTF;

    namespace UTF_FLAG{
        constexpr inline unsigned char IS_ASCII          = 1 << 0;
        constexpr inline unsigned char IS_UNICODE        = 1 << 1;
        constexpr inline unsigned char ENCODE_START      = 1 << 2;
        constexpr inline unsigned char ENCODE_END        = 1 << 3;
    };

    namespace SYMBOLS{
        static const std::string TOP_LEFT_CORNER = "┌";//"\e(0\x6c\e(B";
        static const std::string BOTTOM_LEFT_CORNER = "└";//"\e(0\x6d\e(B";
        static const std::string TOP_RIGHT_CORNER = "┐";//"\e(0\x6b\e(B";
        static const std::string BOTTOM_RIGHT_CORNER = "┘";//"\e(0\x6a\e(B";
        static const std::string VERTICAL_LINE = "│";//"\e(0\x78\e(B";
        static const std::string HORIZONTAL_LINE = "─";//"\e(0\x71\e(B";
        static const std::string VERTICAL_RIGHT_CONNECTOR = "├";//"\e(0\x74\e(B";
        static const std::string VERTICAL_LEFT_CONNECTOR = "┤";//"\e(0\x75\e(B";
        static const std::string HORIZONTAL_BOTTOM_CONNECTOR = "┬";//"\e(0\x76\e(B";
        static const std::string HORIZONTAL_TOP_CONNECTOR = "┴";//"\e(0\x77\e(B";
        static const std::string CROSS_CONNECTOR = "┼";//"\e(0\x6e\e(B";

        static const std::string CENTERED_HORIZONTAL_LINE = "━";//"\e(0\x2501\e(B";
        static const std::string FULL_BLOCK = "█";//"\e(0\x2588\e(B";

        inline unsigned int CONNECTS_UP = 1 << 0;
        inline unsigned int CONNECTS_DOWN = 1 << 1;
        inline unsigned int CONNECTS_LEFT = 1 << 2;
        inline unsigned int CONNECTS_RIGHT = 1 << 3;

        static const std::string RADIOBUTTON_OFF = "○";
        static const std::string RADIOBUTTON_ON = "◉";

        static const std::string EMPTY_CHECK_BOX = "☐";
        static const std::string CHECKED_CHECK_BOX = "☒";

        extern GGUI::UTF EMPTY_UTF;
    }

    namespace TIME{
        inline constexpr static  unsigned int MILLISECOND = 1; 
        inline constexpr static  unsigned int SECOND = MILLISECOND * 1000;
        inline constexpr static  unsigned int MINUTE = SECOND * 60;
        inline constexpr static  unsigned int HOUR = MINUTE * 60;
    }

    namespace Constants{
        namespace ANSI{
            // 1 to ESC_CODE
            // 1 to Text_Color | Background_Color
            // 1 to SEPARATE
            // 1 to USE_RGB
            // 1 to SEPARATE
            static const unsigned int Maximum_Needed_Pre_Allocation_For_Over_Head = 1 + 1 + 1 + 1 + 1;

            // 1 to Red
            // 1 to SEPARATE
            // 1 to Green
            // 1 to SEPARATE
            // 1 to Blue
            static const unsigned int Maximum_Needed_Pre_Allocation_For_Color = 1 + 1 + 1 + 1 + 1;

            // 5 to Text_Overhead
            // 5 to Text_Colour
            // 1 to END_COMMAND
            // 5 to Background_Overhead
            // 5 to Background_Colour
            // 1 to END_COMMAND
            // 1 to Data
            // 1 to RESET_COLOR
            static const unsigned int Maximum_Needed_Pre_Allocation_For_Encoded_Super_String = 
                Maximum_Needed_Pre_Allocation_For_Over_Head + Maximum_Needed_Pre_Allocation_For_Color + 1 +
                Maximum_Needed_Pre_Allocation_For_Over_Head + Maximum_Needed_Pre_Allocation_For_Color + 1 + 1 + 1;
        
            // 1 to Escape code
            // 1 to private SGR telltale '?'
            // 1 to Feature to be disabled or enabled
            // 1 to Enable/Disable feature told above.
            static const unsigned int Maximum_Needed_Pre_Allocation_For_Enabling_Or_Disabling_Private_SGR_Feature = 1 + 1 + 1 + 1;
        
            // 1 to Escape code
            // 1 to feature to be enabled
            // 1 to END_COMMAND
            static const unsigned int Maximum_Needed_Pre_Allocation_For_Enabling_Or_Disabling_SGR_Feature = 1 + 1 + 1;

            // CSI (Control Sequence Introducer) sequences.
            static const std::string ESC_CODE = "\x1B[";      // Also known as \e[ or \o33
            static const std::string SEPARATE = ";";
            static const std::string USE_RGB = "2";
            static const std::string END_COMMAND = "m";
            static const std::string CLEAR_SCREEN = ESC_CODE + "2J";
            static const std::string CLEAR_SCROLLBACK = ESC_CODE + "3J";
            static const std::string SET_CURSOR_TO_START = ESC_CODE + "H";
            static const std::string RESET_CONSOLE = ESC_CODE + "c";
            static const std::string RESET_COLOR = ESC_CODE + '0' + END_COMMAND;  // Basically same as RESET_SGR but baked the end command into it for Super_String

            /**
             * @brief Enable or disable a private SGR feature
             * 
             * @param command The SGR command to enable or disable
             * @param Enable If true, enable the feature. Otherwise, disable it.
             * @return A Super_String object with the correct escape sequence to enable or disable the feature
             */
            inline Super_String Enable_Private_SGR_Feature(const std::string& command, bool Enable = true) { 
                Super_String Result(Maximum_Needed_Pre_Allocation_For_Enabling_Or_Disabling_Private_SGR_Feature);

                // Add the escape code
                Result.Add(ESC_CODE);

                // Add the private SGR telltale '?'
                Result.Add('?');

                // Add the command to enable or disable
                Result.Add(command);

                // Finally, add the enable or disable code
                if (Enable)
                    Result.Add('h');
                else
                    Result.Add('l');

                return Result;
            }


            // SGR (Select Graphic Rendition)
            
            /**
             * @brief Enable a SGR feature
             * 
             * @param command The command to enable
             * @return A Super_String object with the correct escape sequence to enable the feature
             */
            inline Super_String Enable_SGR_Feature(const std::string& command) {
                Super_String Result(Maximum_Needed_Pre_Allocation_For_Enabling_Or_Disabling_SGR_Feature);

                // Add the escape code
                Result.Add(ESC_CODE);

                // Add the command to enable
                Result.Add(command);

                // Add the end command
                Result.Add(END_COMMAND);

                return Result;
            }

            // SGR constants
            static const std::string RESET_SGR = "0";                                  // Removes all SGR features. 
            static const std::string BOLD = "1";                                       // Not widely supported!
            static const std::string FAINT = "2";                                      // Not widely supported!
            static const std::string ITALIC = "3";                                     // Not widely supported! (Can also be same as blink)
            static const std::string UNDERLINE = "4";              
            static const std::string SLOW_BLINK = "5";                                 // ~150 BPM
            static const std::string RAPID_BLINK = "6";                                // Not widely supported!
            static const std::string INVERT_FOREGROUND_WITH_BACKGROUND = "7";          // Not widely supported!
            static const std::string CONCEAL = "8";                                    // Not widely supported!
            static const std::string CROSSED_OUT = "9";                                // Not widely supported!
            static const std::string PRIMARY_FONT = "10";                              // Sets the default font.
            static const std::string ALTERNATIVE_FONT_1 = "11";                        // Custom font slot.
            static const std::string ALTERNATIVE_FONT_2 = "12";                        // Custom font slot.
            static const std::string ALTERNATIVE_FONT_3 = "13";                        // Custom font slot.
            static const std::string ALTERNATIVE_FONT_4 = "14";                        // Custom font slot.
            static const std::string ALTERNATIVE_FONT_5 = "15";                        // Custom font slot.
            static const std::string ALTERNATIVE_FONT_6 = "16";                        // Custom font slot.
            static const std::string ALTERNATIVE_FONT_7 = "17";                        // Custom font slot.
            static const std::string ALTERNATIVE_FONT_8 = "18";                        // Custom font slot.
            static const std::string ALTERNATIVE_FONT_9 = "19";                        // Custom font slot.
            static const std::string FRAKTUR = "20";                                   // Not widely supported! (But cool font)
            static const std::string NOT_BOLD = "21";                                  // Removes the BOLD feature
            static const std::string NORMAL_INTENSITY = "22";                          // Removes BOLD and ITALIC and other affixes.
            static const std::string NOT_UNDERLINE = "23";                             // Removes UNDERLINE.
            static const std::string NOT_BLINK = "24";                                 // Removes BLINK.
            static const std::string INVERT_INVERT_FOREGROUND_WITH_BACKGROUND = "27";  // Inverts the INVERT_FOREGROUND_WITH_BACKGROUND.
            static const std::string TEXT_COLOR = "38";                               // Sets the foreground color.
            static const std::string DEFAULT_TEXT_COLOR = "39";                       // Sets the default color.
            static const std::string BACKGROUND_COLOR = "48";                         // Sets the background color.
            static const std::string DEFAULT_BACKGROUND_COLOR = "49";                 // Sets the default color.

            // Private SGR codes
            static const std::string REPORT_MOUSE_HIGHLIGHTS = "1000";
            static const std::string REPORT_MOUSE_BUTTON_WHILE_MOVING = "1002";
            static const std::string REPORT_MOUSE_ALL_EVENTS = "1003";

            static const std::string MOUSE_CURSOR = "25";
            static const std::string SCREEN_CAPTURE = "47"; // 47l = restores screen, 47h = saves screen
            static const std::string ALTERNATIVE_SCREEN_BUFFER = "1049"; // 1049l = disables alternative buffer, 1049h = enables alternative buffer
            // End of enable settings for ANSI

            // ACC (ASCII Control Characters)
            inline char NONE = 0;
            inline char START_OF_HEADING = 1;
            inline char START_OF_TEXT = 2;
            inline char END_OF_TEXT = 3;
            inline char END_OF_TRANSMISSION = 4;
            inline char ENQUIRY = 5;
            inline char ACKNOWLEDGE = 6;
            inline char BELL = 7;
            inline char BACKSPACE = 8;
            inline char HORIZONTAL_TAB = 9;
            inline char LINE_FEED = 10;             // Also known as newline
            inline char VERTICAL_TAB = 11;
            inline char FORM_FEED = 12;
            inline char CARRIAGE_RETURN = 13;
            inline char SHIFT_OUT = 14;
            inline char SHIFT_IN = 15;
            inline char DATA_LINK_ESCAPE = 16;
            inline char DEVICE_CONTROL_1 = 17;
            inline char DEVICE_CONTROL_2 = 18;
            inline char DEVICE_CONTROL_3 = 19;
            inline char DEVICE_CONTROL_4 = 20;
            inline char NEGATIVE_ACKNOWLEDGE = 21;
            inline char SYNCHRONOUS_IDLE = 22;
            inline char END_OF_TRANSMISSION_BLOCK = 23;
            inline char CANCEL = 24;
            inline char END_OF_MEDIUM = 25;
            inline char SUBSTITUTE = 26;
            inline char ESCAPE = 27;
            inline char FILE_SEPARATOR = 28;
            inline char GROUP_SEPARATOR = 29;
            inline char RECORD_SEPARATOR = 30;
            inline char UNIT_SEPARATOR = 31;
        }

        inline unsigned long long NONE = (unsigned long long)1 << 0;
        inline unsigned long long ENTER = (unsigned long long)1 << 1;
        inline unsigned long long ESCAPE = (unsigned long long)1 << 2;
        inline unsigned long long BACKSPACE = (unsigned long long)1 << 3;
        inline unsigned long long TAB = (unsigned long long)1 << 4;
        inline unsigned long long UP = (unsigned long long)1 << 5;
        inline unsigned long long DOWN = (unsigned long long)1 << 6;
        inline unsigned long long LEFT = (unsigned long long)1 << 7;
        inline unsigned long long RIGHT = (unsigned long long)1 << 8;
        inline unsigned long long SPACE = (unsigned long long)1 << 9;
        inline unsigned long long SHIFT = (unsigned long long)1 << 10;
        inline unsigned long long ALT = (unsigned long long)1 << 11;
        inline unsigned long long CONTROL = (unsigned long long)1 << 12;
        inline unsigned long long SUPER = (unsigned long long)1 << 13;
        inline unsigned long long HOME = (unsigned long long)1 << 14;
        inline unsigned long long INSERT = (unsigned long long)1 << 15;
        inline unsigned long long DELETE = (unsigned long long)1 << 16;
        inline unsigned long long END = (unsigned long long)1 << 17;
        inline unsigned long long PAGE_UP = (unsigned long long)1 << 18;
        inline unsigned long long PAGE_DOWN = (unsigned long long)1 << 19;
        inline unsigned long long F0 = (unsigned long long)1 << 20;
        inline unsigned long long F1 = (unsigned long long)1 << 21;
        inline unsigned long long F2 = (unsigned long long)1 << 22;
        inline unsigned long long F3 = (unsigned long long)1 << 23;
        inline unsigned long long F4 = (unsigned long long)1 << 24;
        inline unsigned long long F5 = (unsigned long long)1 << 25;
        inline unsigned long long F6 = (unsigned long long)1 << 26;
        inline unsigned long long F7 = (unsigned long long)1 << 27;
        inline unsigned long long F8 = (unsigned long long)1 << 28;
        inline unsigned long long F9 = (unsigned long long)1 << 29;
        inline unsigned long long F10 = (unsigned long long)1 << 30;
        inline unsigned long long F11 = (unsigned long long)1 << 31;
        inline unsigned long long F12 = (unsigned long long)1 << 32;
        inline unsigned long long F13 = (unsigned long long)1 << 33;
        inline unsigned long long F14 = (unsigned long long)1 << 34;
        inline unsigned long long F15 = (unsigned long long)1 << 35;
        inline unsigned long long F16 = (unsigned long long)1 << 36;

        // Should not fucking exist bro!
        //inline unsigned long long SHIFT_TAB = (unsigned long long)1 << 37;


        //key_Press includes [a-z, A-Z] & [0-9]
        inline unsigned long long KEY_PRESS = (unsigned long long)1 << 38;

        // EASY MOUSE API
        inline unsigned long long MOUSE_LEFT_CLICKED = (unsigned long long)1 << 39;
        inline unsigned long long MOUSE_MIDDLE_CLICKED = (unsigned long long)1 << 40;
        inline unsigned long long MOUSE_RIGHT_CLICKED = (unsigned long long)1 << 41;

        // NOTE: These will be spammed until it is not pressed anymore!
        inline unsigned long long MOUSE_LEFT_PRESSED = (unsigned long long)1 << 42;
        inline unsigned long long MOUSE_MIDDLE_PRESSED = (unsigned long long)1 << 43;
        inline unsigned long long MOUSE_RIGHT_PRESSED = (unsigned long long)1 << 44;

        inline unsigned long long MOUSE_MIDDLE_SCROLL_UP = (unsigned long long)1 << 45;
        inline unsigned long long MOUSE_MIDDLE_SCROLL_DOWN = (unsigned long long)1 << 46;
    
        // At compile time generate 0-255 representations as const char* values.
        constexpr const char* To_String[256] = {
            "0", "1", "2", "3", "4", "5", "6", "7", "8", "9",
            "10", "11", "12", "13", "14", "15", "16", "17", "18", "19",
            "20", "21", "22", "23", "24", "25", "26", "27", "28", "29",
            "30", "31", "32", "33", "34", "35", "36", "37", "38", "39",
            "40", "41", "42", "43", "44", "45", "46", "47", "48", "49",
            "50", "51", "52", "53", "54", "55", "56", "57", "58", "59",
            "60", "61", "62", "63", "64", "65", "66", "67", "68", "69",
            "70", "71", "72", "73", "74", "75", "76", "77", "78", "79",
            "80", "81", "82", "83", "84", "85", "86", "87", "88", "89",
            "90", "91", "92", "93", "94", "95", "96", "97", "98", "99",
            "100", "101", "102", "103", "104", "105", "106", "107", "108", "109",
            "110", "111", "112", "113", "114", "115", "116", "117", "118", "119",
            "120", "121", "122", "123", "124", "125", "126", "127", "128", "129",
            "130", "131", "132", "133", "134", "135", "136", "137", "138", "139",
            "140", "141", "142", "143", "144", "145", "146", "147", "148", "149",
            "150", "151", "152", "153", "154", "155", "156", "157", "158", "159",
            "160", "161", "162", "163", "164", "165", "166", "167", "168", "169",
            "170", "171", "172", "173", "174", "175", "176", "177", "178", "179",
            "180", "181", "182", "183", "184", "185", "186", "187", "188", "189",
            "190", "191", "192", "193", "194", "195", "196", "197", "198", "199",
            "200", "201", "202", "203", "204", "205", "206", "207", "208", "209",
            "210", "211", "212", "213", "214", "215", "216", "217", "218", "219",
            "220", "221", "222", "223", "224", "225", "226", "227", "228", "229",
            "230", "231", "232", "233", "234", "235", "236", "237", "238", "239",
            "240", "241", "242", "243", "244", "245", "246", "247", "248", "249",
            "250", "251", "252", "253", "254", "255"
        };

        static const Compact_String To_Compact[256] = {
            Compact_String("0", 1), Compact_String("1", 1), Compact_String("2", 1), Compact_String("3", 1), Compact_String("4", 1), Compact_String("5", 1), Compact_String("6", 1), Compact_String("7", 1), Compact_String("8", 1), Compact_String("9", 1),
            Compact_String("10", 2), Compact_String("11", 2), Compact_String("12", 2), Compact_String("13", 2), Compact_String("14", 2), Compact_String("15", 2), Compact_String("16", 2), Compact_String("17", 2), Compact_String("18", 2), Compact_String("19", 2),
            Compact_String("20", 2), Compact_String("21", 2), Compact_String("22", 2), Compact_String("23", 2), Compact_String("24", 2), Compact_String("25", 2), Compact_String("26", 2), Compact_String("27", 2), Compact_String("28", 2), Compact_String("29", 2),
            Compact_String("30", 2), Compact_String("31", 2), Compact_String("32", 2), Compact_String("33", 2), Compact_String("34", 2), Compact_String("35", 2), Compact_String("36", 2), Compact_String("37", 2), Compact_String("38", 2), Compact_String("39", 2),
            Compact_String("40", 2), Compact_String("41", 2), Compact_String("42", 2), Compact_String("43", 2), Compact_String("44", 2), Compact_String("45", 2), Compact_String("46", 2), Compact_String("47", 2), Compact_String("48", 2), Compact_String("49", 2),
            Compact_String("50", 2), Compact_String("51", 2), Compact_String("52", 2), Compact_String("53", 2), Compact_String("54", 2), Compact_String("55", 2), Compact_String("56", 2), Compact_String("57", 2), Compact_String("58", 2), Compact_String("59", 2),
            Compact_String("60", 2), Compact_String("61", 2), Compact_String("62", 2), Compact_String("63", 2), Compact_String("64", 2), Compact_String("65", 2), Compact_String("66", 2), Compact_String("67", 2), Compact_String("68", 2), Compact_String("69", 2),
            Compact_String("70", 2), Compact_String("71", 2), Compact_String("72", 2), Compact_String("73", 2), Compact_String("74", 2), Compact_String("75", 2), Compact_String("76", 2), Compact_String("77", 2), Compact_String("78", 2), Compact_String("79", 2),
            Compact_String("80", 2), Compact_String("81", 2), Compact_String("82", 2), Compact_String("83", 2), Compact_String("84", 2), Compact_String("85", 2), Compact_String("86", 2), Compact_String("87", 2), Compact_String("88", 2), Compact_String("89", 2),
            Compact_String("90", 2), Compact_String("91", 2), Compact_String("92", 2), Compact_String("93", 2), Compact_String("94", 2), Compact_String("95", 2), Compact_String("96", 2), Compact_String("97", 2), Compact_String("98", 2), Compact_String("99", 2),
            Compact_String("100", 3), Compact_String("101", 3), Compact_String("102", 3), Compact_String("103", 3), Compact_String("104", 3), Compact_String("105", 3), Compact_String("106", 3), Compact_String("107", 3), Compact_String("108", 3), Compact_String("109", 3),
            Compact_String("110", 3), Compact_String("111", 3), Compact_String("112", 3), Compact_String("113", 3), Compact_String("114", 3), Compact_String("115", 3), Compact_String("116", 3), Compact_String("117", 3), Compact_String("118", 3), Compact_String("119", 3),
            Compact_String("120", 3), Compact_String("121", 3), Compact_String("122", 3), Compact_String("123", 3), Compact_String("124", 3), Compact_String("125", 3), Compact_String("126", 3), Compact_String("127", 3), Compact_String("128", 3), Compact_String("129", 3),
            Compact_String("130", 3), Compact_String("131", 3), Compact_String("132", 3), Compact_String("133", 3), Compact_String("134", 3), Compact_String("135", 3), Compact_String("136", 3), Compact_String("137", 3), Compact_String("138", 3), Compact_String("139", 3),
            Compact_String("140", 3), Compact_String("141", 3), Compact_String("142", 3), Compact_String("143", 3), Compact_String("144", 3), Compact_String("145", 3), Compact_String("146", 3), Compact_String("147", 3), Compact_String("148", 3), Compact_String("149", 3),
            Compact_String("150", 3), Compact_String("151", 3), Compact_String("152", 3), Compact_String("153", 3), Compact_String("154", 3), Compact_String("155", 3), Compact_String("156", 3), Compact_String("157", 3), Compact_String("158", 3), Compact_String("159", 3),
            Compact_String("160", 3), Compact_String("161", 3), Compact_String("162", 3), Compact_String("163", 3), Compact_String("164", 3), Compact_String("165", 3), Compact_String("166", 3), Compact_String("167", 3), Compact_String("168", 3), Compact_String("169", 3),
            Compact_String("170", 3), Compact_String("171", 3), Compact_String("172", 3), Compact_String("173", 3), Compact_String("174", 3), Compact_String("175", 3), Compact_String("176", 3), Compact_String("177", 3), Compact_String("178", 3), Compact_String("179", 3),
            Compact_String("180", 3), Compact_String("181", 3), Compact_String("182", 3), Compact_String("183", 3), Compact_String("184", 3), Compact_String("185", 3), Compact_String("186", 3), Compact_String("187", 3), Compact_String("188", 3), Compact_String("189", 3),
            Compact_String("190", 3), Compact_String("191", 3), Compact_String("192", 3), Compact_String("193", 3), Compact_String("194", 3), Compact_String("195", 3), Compact_String("196", 3), Compact_String("197", 3), Compact_String("198", 3), Compact_String("199", 3),
            Compact_String("200", 3), Compact_String("201", 3), Compact_String("202", 3), Compact_String("203", 3), Compact_String("204", 3), Compact_String("205", 3), Compact_String("206", 3), Compact_String("207", 3), Compact_String("208", 3), Compact_String("209", 3),
            Compact_String("210", 3), Compact_String("211", 3), Compact_String("212", 3), Compact_String("213", 3), Compact_String("214", 3), Compact_String("215", 3), Compact_String("216", 3), Compact_String("217", 3), Compact_String("218", 3), Compact_String("219", 3),
            Compact_String("220", 3), Compact_String("221", 3), Compact_String("222", 3), Compact_String("223", 3), Compact_String("224", 3), Compact_String("225", 3), Compact_String("226", 3), Compact_String("227", 3), Compact_String("228", 3), Compact_String("229", 3),
            Compact_String("230", 3), Compact_String("231", 3), Compact_String("232", 3), Compact_String("233", 3), Compact_String("234", 3), Compact_String("235", 3), Compact_String("236", 3), Compact_String("237", 3), Compact_String("238", 3), Compact_String("239", 3),
            Compact_String("240", 3), Compact_String("241", 3), Compact_String("242", 3), Compact_String("243", 3), Compact_String("244", 3), Compact_String("245", 3), Compact_String("246", 3), Compact_String("247", 3), Compact_String("248", 3), Compact_String("249", 3),
            Compact_String("250", 3), Compact_String("251", 3), Compact_String("252", 3), Compact_String("253", 3), Compact_String("254", 3), Compact_String("255", 3)
        };
    }

    namespace POSITION{
        extern short Max_Z;
    }

    namespace BUTTON_STATES{
        static const std::string ESC = "ECS";
        static const std::string F1 = "F1";
        static const std::string F2 = "F2";
        static const std::string F3 = "F3";
        static const std::string F4 = "F4";
        static const std::string F5 = "F5";
        static const std::string F6 = "F6";
        static const std::string F7 = "F7";
        static const std::string F8 = "F8";
        static const std::string F9 = "F9";
        static const std::string F10 = "F10";
        static const std::string F11 = "F11";
        static const std::string F12 = "F12";
        static const std::string PRTSC = "PRTSC";
        static const std::string SCROLL_LOCK = "SCROLL_LOCK";
        static const std::string PAUSE = "PAUSE";
        static const std::string SECTION = "SECTION";
        static const std::string BACKSPACE = "BACKSPACE";
        static const std::string TAB = "TAB";
        static const std::string ENTER = "ENTER";
        static const std::string CAPS = "CAPS";
        static const std::string SHIFT = "SHIFT";
        static const std::string CONTROL = "CTRL";
        static const std::string SUPER = "SUPER";
        static const std::string ALT = "ALT";
        static const std::string SPACE = "SPACE";
        static const std::string ALTGR = "ALTGR";
        static const std::string FN = "FN";
        static const std::string INS = "INS";
        static const std::string HOME = "HOME";
        static const std::string PAGE_UP = "PAGE_UP";
        static const std::string DELETE = "DELETE";
        static const std::string INSERT = "INSERT";
        static const std::string END = "END";
        static const std::string PAGE_DOWN = "PAGE_DOWN";

        static const std::string UP = "UP";
        static const std::string DOWN = "DOWN";
        static const std::string LEFT = "LEFT";
        static const std::string RIGHT = "RIGHT";

        static const std::string MOUSE_LEFT = "MOUSE_LEFT";
        static const std::string MOUSE_MIDDLE = "MOUSE_MIDDLE";
        static const std::string MOUSE_RIGHT = "MOUSE_RIGHT";
        static const std::string MOUSE_SCROLL_UP = "MOUSE_SCROLL_UP";
        static const std::string MOUSE_SCROLL_DOWN = "MOUSE_SCROLL_DOWN";
    };

    inline std::unordered_map<std::string, unsigned long long> BUTTON_STATES_TO_CONSTANTS_BRIDGE = {

        {BUTTON_STATES::ESC, Constants::ESCAPE},
        {BUTTON_STATES::F1, Constants::F1},
        {BUTTON_STATES::F2, Constants::F2},
        {BUTTON_STATES::F3, Constants::F3},
        {BUTTON_STATES::F4, Constants::F4},
        {BUTTON_STATES::F5, Constants::F5},
        {BUTTON_STATES::F6, Constants::F6},
        {BUTTON_STATES::F7, Constants::F7},
        {BUTTON_STATES::F8, Constants::F8},
        {BUTTON_STATES::F9, Constants::F9},
        {BUTTON_STATES::F10, Constants::F10},
        {BUTTON_STATES::F11, Constants::F11},
        {BUTTON_STATES::F12, Constants::F12},
        //{BUTTON_STATES::PRTSC, Constants::PRINT_SCREEN},
        //{BUTTON_STATES::SCROLL_LOCK, Constants::SCROLL_LOCK},
        //{BUTTON_STATES::PAUSE, Constants::PAUSE},
        //{BUTTON_STATES::SECTION, Constants::SECTION},
        {BUTTON_STATES::BACKSPACE, Constants::BACKSPACE},
        {BUTTON_STATES::TAB, Constants::TAB},
        {BUTTON_STATES::ENTER, Constants::ENTER},
        //{BUTTON_STATES::CAPS, Constants::CAPS},
        {BUTTON_STATES::SHIFT, Constants::SHIFT},
        {BUTTON_STATES::CONTROL, Constants::CONTROL},
        {BUTTON_STATES::SUPER, Constants::SUPER},
        {BUTTON_STATES::ALT, Constants::ALT},
        {BUTTON_STATES::SPACE, Constants::SPACE},
        //{BUTTON_STATES::ALTGR, Constants::ALTGR},
        //{BUTTON_STATES::FN, Constants::FN},
        {BUTTON_STATES::INS, Constants::INSERT},
        {BUTTON_STATES::HOME, Constants::HOME},
        {BUTTON_STATES::PAGE_UP, Constants::PAGE_UP},
        {BUTTON_STATES::DELETE, Constants::DELETE},
        {BUTTON_STATES::INSERT, Constants::INSERT},
        {BUTTON_STATES::END, Constants::END},
        {BUTTON_STATES::PAGE_DOWN, Constants::PAGE_DOWN},

        {BUTTON_STATES::UP, Constants::UP},
        {BUTTON_STATES::DOWN, Constants::DOWN},
        {BUTTON_STATES::LEFT, Constants::LEFT},
        {BUTTON_STATES::RIGHT, Constants::RIGHT},

        {BUTTON_STATES::MOUSE_LEFT, Constants::MOUSE_LEFT_CLICKED},
        {BUTTON_STATES::MOUSE_MIDDLE, Constants::MOUSE_MIDDLE_CLICKED},
        {BUTTON_STATES::MOUSE_RIGHT, Constants::MOUSE_RIGHT_CLICKED},
        {BUTTON_STATES::MOUSE_SCROLL_UP, Constants::MOUSE_MIDDLE_SCROLL_UP},
        {BUTTON_STATES::MOUSE_SCROLL_DOWN, Constants::MOUSE_MIDDLE_SCROLL_DOWN},
    };
}

#endif
#ifndef _COLOR_H_
#define _COLOR_H_

#include <string>
#include <limits>
#include <math.h>





namespace GGUI{
    class RGB{
    public:
        unsigned char Red = 0;
        unsigned char Green = 0;
        unsigned char Blue = 0;

        constexpr RGB(unsigned char r, unsigned char g, unsigned char b) : Red(r), Green(g), Blue(b) {}

        constexpr RGB() = default;
        
        /**
         * @brief Gets the colour as a string.
         *
         * @return The colour as a string. Format is: "X;Y;Z" where X, Y, Z are the values of red, green, blue respectively.
         */
        std::string Get_Colour() const;

        /**
         * @brief Converts the RGB colour to a string.
         *
         * @param Result The result string.
         */
        void Get_Colour_As_Super_String(Super_String* Result) const;
    
        /**
         * @brief Generates an ANSI escape code for setting text or background color using RGB values.
         * 
         * @param Is_Text_Color Determines whether the escape code is for text color (true) or background color (false).
         * @return A string containing the ANSI escape code for the specified color type.
         */
        std::string Get_Over_Head(bool Is_Text_Color = true) const {
            if (Is_Text_Color) {
                // Return escape code for text color
                return Constants::ANSI::ESC_CODE + Constants::ANSI::TEXT_COLOR + Constants::ANSI::SEPARATE + Constants::ANSI::USE_RGB + Constants::ANSI::SEPARATE;
            } else {
                // Return escape code for background color
                return Constants::ANSI::ESC_CODE + Constants::ANSI::BACKGROUND_COLOR + Constants::ANSI::SEPARATE + Constants::ANSI::USE_RGB + Constants::ANSI::SEPARATE;
            }
        }

        /**
         * @brief Populates a Super_String with ANSI escape codes for text or background color.
         * 
         * This function adds the escape codes for either text or background color to the provided Super_String.
         * Ensure that the Result has been initialized with at least Maximum_Needed_Pre_Allocation_For_Over_Head.
         * 
         * @param Result A pointer to the Super_String to populate with escape codes.
         * @param Is_Text_Color A boolean to determine if the codes are for text color (true) or background color (false).
         */
        void Get_Over_Head_As_Super_String(Super_String* Result, bool Is_Text_Color = true) const {
            if (Is_Text_Color) {
                // Add escape codes for text color
                Result->Add(Constants::ANSI::ESC_CODE);
                Result->Add(Constants::ANSI::TEXT_COLOR);
                Result->Add(Constants::ANSI::SEPARATE);
                Result->Add(Constants::ANSI::USE_RGB);
                Result->Add(Constants::ANSI::SEPARATE);
            } else {
                // Add escape codes for background color
                Result->Add(Constants::ANSI::ESC_CODE);
                Result->Add(Constants::ANSI::BACKGROUND_COLOR);
                Result->Add(Constants::ANSI::SEPARATE);
                Result->Add(Constants::ANSI::USE_RGB);
                Result->Add(Constants::ANSI::SEPARATE);
            }
        }
    
        constexpr bool operator==(const RGB& Other) const{
            // only take the bits from the first 3 unsigned chars
            //return (*(unsigned int*)this & 0xFFFFFF) == (*(unsigned int*)&Other & 0xFFFFFF);
            return Red == Other.Red && Green == Other.Green && Blue == Other.Blue;
        }
    
        constexpr RGB operator+(const RGB& Other) const{
            return RGB(Red + Other.Red, Green + Other.Green, Blue + Other.Blue);
        }

        constexpr RGB operator*(const float Scalar) const{
            return RGB((unsigned char)((float)Red * Scalar), (unsigned char)((float)Green * Scalar), (unsigned char)((float)Blue * Scalar));
        }
    };

    class RGBA : public RGB{
    public:
        unsigned char Alpha = UINT8_MAX;

        constexpr RGBA(unsigned char r, unsigned char g, unsigned char b, unsigned char a) : RGB(r, g, b){
            Alpha = a;
        }

        RGBA() = default;

        RGBA(RGB primal) : RGB(primal){}
    
        // For float API's
        constexpr void Set_Alpha(float a){
            Alpha = (unsigned char)(a * UINT8_MAX);
        }

        constexpr float Get_Alpha() const{
            return (float)Alpha / UINT8_MAX;
        }

        constexpr bool operator==(const RGBA& Other) const{
            // only take the bits which are the 4 unsigned chars, which is same as single integer.
            // Starting from Red since Red is the first member and normally Virtual function will also allocate their own space before members.
            return *(unsigned int*)&this->Red == *(unsigned int*)&Other;
        }

        constexpr RGBA operator*(const RGBA& Other) const{
            float Decimal_Alpha = Other.Get_Alpha();
            // Make the reverse alpha
            float Reverse_Alpha = 1 - Decimal_Alpha;
            
            return RGBA(
                ((float)this->Red * Reverse_Alpha) * ((float)Other.Red * Decimal_Alpha), 
                ((float)this->Green * Reverse_Alpha) * ((float)Other.Green * Decimal_Alpha), 
                ((float)this->Blue * Reverse_Alpha) * ((float)Other.Blue * Decimal_Alpha),
                Alpha
            );
        }

        constexpr RGBA operator+(const RGBA& Other) const{
            float Decimal_Alpha = Other.Get_Alpha();
            // Make the reverse alpha
            float Reverse_Alpha = 1 - Decimal_Alpha;

            return RGBA(
                ((float)this->Red * Reverse_Alpha) + ((float)Other.Red * Decimal_Alpha), 
                ((float)this->Green * Reverse_Alpha) + ((float)Other.Green * Decimal_Alpha), 
                ((float)this->Blue * Reverse_Alpha) + ((float)Other.Blue * Decimal_Alpha),
                Alpha
            );
        }

        constexpr RGBA operator*=(const RGBA& Other){
            float Decimal_Alpha = Other.Get_Alpha();
            // Make the reverse alpha
            float Reverse_Alpha = 1 - Decimal_Alpha;

            this->Red = ((float)this->Red * Reverse_Alpha) * ((float)Other.Red * Decimal_Alpha);
            this->Green = ((float)this->Green * Reverse_Alpha) * ((float)Other.Green * Decimal_Alpha);
            this->Blue = ((float)this->Blue * Reverse_Alpha) * ((float)Other.Blue * Decimal_Alpha);

            return *this;
        }

        constexpr RGBA operator+=(const RGBA& Other){
            float Decimal_Alpha = Other.Get_Alpha();
            // Make the reverse alpha
            float Reverse_Alpha = 1 - Decimal_Alpha;

            this->Red = ((float)this->Red * Reverse_Alpha) + ((float)Other.Red * Decimal_Alpha);
            this->Green = ((float)this->Green * Reverse_Alpha) + ((float)Other.Green * Decimal_Alpha);
            this->Blue = ((float)this->Blue * Reverse_Alpha) + ((float)Other.Blue * Decimal_Alpha);

            return *this;
        }

    };

    namespace COLOR{
        static constexpr RGB WHITE = RGB(255, 255, 255);
        static constexpr RGB BLACK = RGB(0, 0, 0);
        static constexpr RGB RED = RGB(255, 0, 0);
        static constexpr RGB GREEN = RGB(0, 255, 0);
        static constexpr RGB BLUE = RGB(0, 0, 255);
        static constexpr RGB YELLOW = RGB(255, 255, 0);
        static constexpr RGB ORANGE = RGB(255, 128, 0);
        static constexpr RGB CYAN = RGB(0, 255, 255);
        static constexpr RGB TEAL = RGB(0, 128, 128);
        static constexpr RGB MAGENTA = RGB(255, 0, 255);
        static constexpr RGB GRAY = RGB(128, 128, 128);
        static constexpr RGB LIGHT_RED = RGB(255, 128, 128);
        static constexpr RGB LIGHT_GREEN = RGB(128, 255, 128);
        static constexpr RGB LIGHT_BLUE = RGB(128, 128, 255);
        static constexpr RGB LIGHT_YELLOW = RGB(255, 255, 128);
        static constexpr RGB LIGHT_CYAN = RGB(128, 255, 255);
        static constexpr RGB LIGHT_MAGENTA = RGB(255, 128, 255);
        static constexpr RGB LIGHT_GRAY = RGB(192, 192, 192);
        static constexpr RGB DARK_RED = RGB(128, 0, 0);
        static constexpr RGB DARK_GREEN = RGB(0, 128, 0);
        static constexpr RGB DARK_BLUE = RGB(0, 0, 128);
        static constexpr RGB DARK_YELLOW = RGB(128, 128, 0);
        static constexpr RGB DARK_CYAN = RGB(0, 128, 128);
        static constexpr RGB DARK_MAGENTA = RGB(128, 0, 128);
        static constexpr RGB DARK_GRAY = RGB(64, 64, 64);
    }

    /**
     * Linear interpolation function
     * @param a The start value
     * @param b The end value
     * @param t The interpolation value, between 0 and 1
     * @return The interpolated value
     */
    template<typename T>
    constexpr T lerp(T a, T b, T t) {
        // Clamp t between a and b
        return a + t * (b - a);
    }

    /**
     * @brief Performs gamma-corrected linear interpolation between two values.
     * 
     * @tparam T The type of the input values.
     * @tparam P The type of the interpolation factor.
     * @param a The start value.
     * @param b The end value.
     * @param t The interpolation factor, typically between 0 and 1.
     * @return The interpolated value, gamma-corrected and cast back to type T.
     */
    template<typename T, typename P>
    constexpr T Interpolate(T a, T b, P t) {
        // Define gamma value for correction
        constexpr float gamma = 2.2F;

        // Apply gamma correction to input values and perform linear interpolation
        const float c_f = lerp<float>(std::pow(static_cast<float>(a), gamma), std::pow(static_cast<float>(b), gamma), t);

        // Reverse gamma correction and cast back to original type
        return static_cast<T>(std::pow(c_f, 1.F / gamma));
    }

    /**
     * @brief Interpolates between two RGB colors using linear interpolation.
     * If SETTINGS::ENABLE_GAMMA_CORRECTION is enabled, the interpolation is done in a gamma-corrected space.
     * @param A The start RGB color.
     * @param B The end RGB color.
     * @param Distance The interpolation factor, typically between 0 and 1.
     * @return The interpolated RGB color.
     */
    extern GGUI::RGB Lerp(GGUI::RGB A, GGUI::RGB B, float Distance);
}

#endif
#ifndef _EVENT_H_
#define _EVENT_H_

#include <functional>
#include <chrono>



namespace GGUI{
    
    class Event{
    public:
        unsigned long long Criteria;
    };

    class Input : public Event{
    public:
        unsigned short X = 0;
        unsigned short Y = 0;
        char Data = 0;

        // The input information like the character written.
        Input(char d, unsigned long long t){
            Data = d;
            Criteria = t;
        }

        Input(IVector3 c, unsigned long long t){
            X = (unsigned short )c.X;
            Y = (unsigned short )c.Y;
            Criteria = t;
        }
    };

    class Action : public Event{
    public:
        class element* Host = nullptr;

        std::function<bool(GGUI::Event*)> Job;
        
        std::string ID; 
    
        Action() = default;
        Action(unsigned long long criteria, std::function<bool(GGUI::Event*)> job, std::string id = ""){
            Criteria = criteria;
            Job = job;
            Host = nullptr;
            ID = id;
        }

        Action(unsigned long long criteria, std::function<bool(GGUI::Event*)> job, class element* host, std::string id = ""){
            Criteria = criteria;
            Job = job;
            Host = host;
            ID = id;
        }
    };

    namespace MEMORY_FLAGS{
        inline unsigned char PROLONG_MEMORY     = 1 << 0;
        inline unsigned char RETRIGGER          = 1 << 1;
    };

    class Memory : public Action{
    public:
        std::chrono::high_resolution_clock::time_point Start_Time;
        size_t End_Time = 0;

        // By default all memories automatically will not prolong each other similar memories.
        unsigned char Flags = 0x0;

        // When the job starts, job, prolong previous similar job by this time.
        Memory(size_t end, std::function<bool(GGUI::Event*)>job, unsigned char flags = 0x0, std::string id = ""){
            Start_Time = std::chrono::high_resolution_clock::now();
            End_Time = end;
            Job = job;
            Flags = flags;
            ID = id;
        }

        bool Is(const unsigned char f) const{
            return (Flags & f) > 0;
        }

        void Set(const unsigned char f){
            Flags |= f;
        }
    };

}

#endif
#ifndef _STYLE_H_
#define _STYLE_H_




#include <variant>
#include <array>
#include <string>
#include <cassert>

namespace GGUI{
    // Externies
    class element;
    class styling;
    enum class STAIN_TYPE;
    namespace INTERNAL{
        extern void reportStack(std::string Problem);
        extern void EXIT(int signum);

        template <typename T>
        std::string Get_Type_Name() {
        #if defined(__clang__) || defined(__GNUC__)
            constexpr const char* func = __PRETTY_FUNCTION__;
        #elif defined(_MSC_VER)
            constexpr const char* func = __FUNCSIG__;
        #else
            #error Unsupported compiler
        #endif

            // Extract the type name from the function signature
            const char* start = strstr(func, "T = ") + 4; // Find "T = " and move the pointer past it
            const char* end = strchr(start, ';');         // Find the closing bracket
            size_t length = end - start;                  // Calculate the length of the type name

            return std::string(start, length);            // Construct and return a std::string
        }
    }

    enum class ALIGN{
        UP,
        DOWN,
        LEFT,
        RIGHT,
        CENTER
    };

    enum class VALUE_STATE{
        UNINITIALIZED,
        INITIALIZED,
        VALUE
    };

    enum class EMBED_ORDER{
        INSTANT,
        DELAYED
    };

    enum class DIRECTION{
        ROW,
        COLUMN
    };

    enum class EVALUATION_TYPE{
        DEFAULT,        // no further evaluation needed, just return the value
        PIXELS,         // the value is multiplied by 1:1
        PERCENTAGE,     // the value is a percentage of the parent attribute
        EM,             // the value is a multiple of the font size
        REM,            // the value is a multiple of the root font size
        VW,             // the value is a percentage of the viewport width
        VH,             // the value is a percentage of the viewport height
        CH,             // the value is a relative to the width of the "0" (zero) character in the element’s font.
        VMIN,           // the value is a percentage of the viewport’s smaller dimension
        VMAX,           // the value is a percentage of the viewport’s larger dimension
        EX,             // the value is a multiple of the x-height of the element’s font
        FRACTION,       // the value is a fraction of the parent attribute

        // PHYSICAL MEASUREMENTS
        CM,             // the value is in centimeters defaults to 1:1 in terminal mode
        MM,             // the value is in millimeters defaults to 1:1 in terminal mode
        IN,             // the value is in inches defaults to 1:1 in terminal mode
        PT,             // the value is in points defaults to 1:1 in terminal mode
        PC,             // the value is in picas defaults to 1:1 in terminal mode
    };

    // This namespace is an wrapper for the user not to see these !!
    namespace STYLING_INTERNAL{

        template<typename P>
        constexpr bool Is_Non_Discriminant_Scalar(const P value, const float scalar);

        template<typename P>
        constexpr std::string To_String(const P value);

        template<typename T>
        class value {
        protected:
            T data;
            float percentage;   // This will be changed later on into an std::variant holding different scaling types.
            EVALUATION_TYPE evaluation_type = EVALUATION_TYPE::DEFAULT;

        public:
            /**
             * Constructor for value class
             * @param value The value to be stored in the variant
             * @param type The type of the value
             * @param use_constexpr Whether to use constexpr or not
             */
            constexpr value(T value, EVALUATION_TYPE type = EVALUATION_TYPE::DEFAULT)
                : data(value), percentage(0.0f), evaluation_type(type) {}

            /**
             * Constructor for value class
             * @param value The value to be stored in the variant
             * @param type The type of the value
             * @param use_constexpr Whether to use constexpr or not
             *
             * This constructor is used when the value is a float and the type is
             * specified. The constexpr parameter is used to determine whether to
             * use a constexpr constructor or not.
             */
            constexpr value(float value, EVALUATION_TYPE type = EVALUATION_TYPE::PERCENTAGE)
                : data{}, percentage(value), evaluation_type(type) {}

            /**
             * Copy constructor
             * @param other The object to be copied from
             *
             * This constructor is used to create a copy of the value object.
             * The data and the evaluation type are copied from the other object.
             */
            constexpr value(const value<T>& other)
                : data(other.data), percentage(other.percentage), evaluation_type(other.evaluation_type) {}


            /**
             * Assignment operator
             * @param other The object to be assigned from
             * @return The object itself, for chaining
             *
             * This operator is used to assign the data and the evaluation type
             * from another object of the same type.
             */
            constexpr value& operator=(const value& other) {
                // Copy the data and the evaluation type from the other object
                data = other.data;
                percentage = other.percentage;
                evaluation_type = other.evaluation_type;
                // Return the object itself, for chaining
                return *this;
            }

            /**
             * Assignment operator
             * @param initialization_data The value to be assigned to the data member
             * @return The object itself, for chaining
             *
             * This operator is used to assign a value of type T to the data member.
             * The type of the value is set to DEFAULT.
             */
            constexpr value& operator=(T initialization_data) {
                // Set the data to the value passed in
                data = initialization_data;
                // Set the evaluation type to DEFAULT
                evaluation_type = EVALUATION_TYPE::DEFAULT;
                // Return the object itself, for chaining
                return *this;
            }

            /**
             * Assignment operator for float type
             * @param initialization_data The float value to be assigned to the data member
             * @return The object itself, for chaining
             *
             * This operator is used to assign a float value to the data member.
             * The type of the value is set to PERCENTAGE.
             */
            constexpr value& operator=(float initialization_data) {
                // Set the data to the float value passed in
                percentage = initialization_data;
                // Set the evaluation type to PERCENTAGE
                evaluation_type = EVALUATION_TYPE::PERCENTAGE;
                // Return the object itself, for chaining
                return *this;
            }

            constexpr bool operator==(const value<T>& other) const {
                if (evaluation_type != other.evaluation_type){
                    INTERNAL::reportStack("Cannot compare two different eval type values!");
                    INTERNAL::EXIT(1);
                    return false;   // for warnings.
                }
                else{
                    switch (evaluation_type)
                    {
                    case EVALUATION_TYPE::DEFAULT:
                        return data == other.data;
                    case EVALUATION_TYPE::PERCENTAGE:
                        return percentage == other.percentage;
                    default:
                        INTERNAL::reportStack("Evaluation type: " + std::to_string((int)evaluation_type) + " not supported!");
                        INTERNAL::EXIT(1);
                        return false;   // for warnings.
                    }
                }
            }

            constexpr value<T> operator+(const value<T>& other){
                if (evaluation_type != other.evaluation_type){
                    INTERNAL::reportStack("Cannot add two different eval type values!");
                    INTERNAL::EXIT(1);
                    return false;   // for warnings.
                }
                else{
                    switch (evaluation_type)
                    {
                    case EVALUATION_TYPE::DEFAULT:
                        return value<T>(data + other.data);
                    case EVALUATION_TYPE::PERCENTAGE:
                        return value<T>(percentage + other.percentage);
                    default:
                        INTERNAL::reportStack("Evaluation type: " + std::to_string((int)evaluation_type) + " not supported!");
                        INTERNAL::EXIT(1);
                        return value<T>(0);
                    }
                }
            }

            constexpr value<T> operator-(const value<T>& other){
                if (evaluation_type != other.evaluation_type){
                    // TODO: add capability to call Report_Stack in Styles.h
                    INTERNAL::LOGGER::Log("Cannot substract two different eval type values!");
                    INTERNAL::EXIT(1);
                    return false;   // for warnings.
                }
                else{
                    switch (evaluation_type)
                    {
                    case EVALUATION_TYPE::DEFAULT:
                        return value<T>(data - other.data);
                    case EVALUATION_TYPE::PERCENTAGE:
                        return value<T>(percentage - other.percentage);
                    default:
                        INTERNAL::LOGGER::Log("Evaluation type: " + std::to_string((int)evaluation_type) + " not supported!");
                        INTERNAL::EXIT(1);
                        return value<T>(0);
                    }
                }
            }

            /**
             * Evaluate function
             * @param parental_value The value to be multiplied by. Only used if the evaluation type is PERCENTAGE.
             * 
             * This function is used to evaluate the value of the variant based on the evaluation type.
             * If the evaluation type is DEFAULT, the data is returned without any modification.
             * If the evaluation type is PERCENTAGE, the parental value is multiplied by the data and the result is returned.
             * If the evaluation type is not supported, an error message is printed and the data is returned without any modification.
             */
            void Evaluate(T parental_value) {
                switch (evaluation_type) {
                    case EVALUATION_TYPE::DEFAULT:
                        // If the evaluation type is DEFAULT then just return the data without any modification
                        return;
                    case EVALUATION_TYPE::PERCENTAGE:
                        // If the evaluation type is PERCENTAGE then multiply the parental value by the data and return the result
                        data = static_cast<T>(static_cast<T>(parental_value) * percentage);

                        #if GGUI_DEBUG

                        if (Is_Non_Discriminant_Scalar<T>(parental_value, percentage)){
                            INTERNAL::LOGGER::Log("Percentage value of: '" + std::to_string(percentage) + "' causes non-discriminant results with: '" + To_String(parental_value) + "'.");
                        }

                        #endif

                        return;
                    default:
                        INTERNAL::LOGGER::Log("Evaluation type not supported!");
                        // If the evaluation type is not supported then just return the data without any modification
                        return;
                }
            }

            /**
             * Get the value of the variant.
             * @tparam P The type to cast the variant to.
             * @return The value of the variant as the requested type.
             * @throws std::bad_variant_access If the requested type doesn't match the type of the data.
             */
            template<typename P>
            constexpr P Get() const {
                return data;
            }

            /**
             * Getter methods
             * @tparam P The type to cast the variant to.
             * @return The value of the variant as the requested type.
             * @throws std::bad_variant_access If the requested type doesn't match the type of the data.
             */
            template<typename P>
            inline constexpr P Get() {                
                return data;
            }

            /**
             * Get the evaluation type of the variant.
             * @return The evaluation type of the variant.
             */
            constexpr EVALUATION_TYPE Get_Type() { return evaluation_type; }

            /**
             * Get the evaluation type of the variant.
             * @return The evaluation type of the variant.
             */
            constexpr EVALUATION_TYPE Get_Type() const { return evaluation_type; }

            /**
             * @brief Direct access to the underlying data of the variant.
             *        This is an exclusive Dev function, with serious repercussions for misuse.
             * @tparam P The type to cast the variant to.
             * @return A reference to the underlying data of the variant.
             * @throws std::bad_variant_access If the requested type doesn't match the type of the data.
             */
            template<typename P>
            inline constexpr P& Direct() { 
                return data;
            }

            /**
             * @brief Set the value of the variant.
             * @param value The value to set the variant to.
             * @details This sets the value of the variant to the provided value.
             *          The evaluation type is set to EVALUATION_TYPE::DEFAULT.
             */
            inline constexpr void Set(T value) {
                data = value;
                evaluation_type = EVALUATION_TYPE::DEFAULT;
            }

            /**
             * @brief Set the value of the variant to a percentage.
             * @param value The value to set the variant to.
             * @details This sets the value of the variant to the provided value, and sets the evaluation type to EVALUATION_TYPE::PERCENTAGE.
             */
            inline constexpr void Set(float value){
                percentage = value;
                evaluation_type = EVALUATION_TYPE::PERCENTAGE;
            }
        };

        class style_base{
        public:
            // This is used to prevent accidental overwrites.
            VALUE_STATE Status = VALUE_STATE::UNINITIALIZED;

            // Represents when the value is embedded.
            EMBED_ORDER Order = EMBED_ORDER::INSTANT;

            // This is used to store all appended style_bases through the operator|.
            style_base* Other;

            /**
             * @brief Construct a new constexpr style_base object.
             * @param status The status to initialize the style_base with.
             * @param use_constexpr A flag indicating whether to use constexpr. This parameter is not used.
             */
            constexpr style_base(VALUE_STATE status = VALUE_STATE::UNINITIALIZED, EMBED_ORDER order = EMBED_ORDER::INSTANT) : Status(status), Order(order), Other(nullptr){}

            /**
             * @brief Destructor of the style_base class.
             *
             * This destructor takes care of deleting the "Other" pointer if it is a heap-allocated object.
             * It does this by calling the Is_Deletable() function to check if the pointer is likely to be
             * a heap-allocated object. If it is, it deletes the object using the delete keyword.
             */
            virtual ~style_base();

            virtual style_base* Copy() const = 0;

            /**
             * @brief Overload the | operator to allow for appending of style_bases.
             * @param other The style_base to append.
             * @return A pointer to the style_base that was appended to.
             * @note This bind the two objects indefinefly!
             */
            constexpr style_base* operator|(style_base* other){
                other->Other = this;

                return other;
            }

            /**
             * @brief Overload the | operator to allow for appending of style_bases.
             * @param other The style_base to append.
             * @return A reference to the style_base that was appended to.
             * @note This bind the two objects indefinefly!
             */
            constexpr style_base& operator|(style_base& other){
                other.Other = this;

                return other;
            }

            /**
             * @brief Overload the | operator to allow for appending of style_bases using move semantics.
             * @param other The style_base to append.
             * @return A reference to the style_base that was appended to.
             * @details This function is used to append a style_base to another style_base using move semantics.
             *          It sets the Other pointer of the moved object to the current object. Then it returns
             *          a reference to the moved object to ensure proper chaining of operations.
             * @note Since operators for "a | b | c" will re-write the previous "a | b" when at c with "ab | c",
             *       it will produce "ac" and not "abc". Thus, we return the secondary object so that: 
             *       "a | b | c" -> "b(a) | c" -> "c(b(a))".
             */
            constexpr style_base& operator|(style_base&& other){
                other.Other = this;

                return other;
            }

            /**
             * @brief Evaluates the style value based on a given Styling object.
             * @param host The Styling object to evaluate the style value with.
             * @details This function is used to evaluate the style value based on a given Styling object.
             *          It is called by the Styling class when the style value needs to be evaluated.
             *          The function is responsible for setting the Status variable to the evaluated status.
             *          The function is also responsible for setting the Value variable to the evaluated value.
             *          The function should be implemented by the derived classes to perform the evaluation.
             */
            virtual void Evaluate([[maybe_unused]] styling* host) {};

            /**
             * @brief Imprints the style's identity into the Styling object.
             * @param host The Styling object to imprint the style into.
             * @param owner The Element that owns the style.
             * @return The type of stain this style will leave.
             * @details This function allows single style classes to incorporate their unique characteristics into a Styling object. 
             *          It should be implemented by derived classes to define how the style affects the Styling and Element objects.
             */
            virtual STAIN_TYPE Embed_Value([[maybe_unused]] styling* host, [[maybe_unused]] element* owner) { return STAIN_TYPE::CLEAN; };
        };

        class RGB_VALUE : public style_base{
        public:
            value<RGB> Value = RGB(0, 0, 0);

            /**
             * @brief Construct a new RGB_VALUE object using constexpr.
             * @param value The RGB value to set.
             * @param Default The default value state.
             * @param use_constexpr Flag indicating whether to use constexpr.
             * @details This constructor initializes an RGB_VALUE object with the given parameters,
             *          using constexpr for compile-time evaluation.
             */
            constexpr RGB_VALUE(const GGUI::RGB value, VALUE_STATE Default = VALUE_STATE::VALUE) 
                : style_base(Default), Value(value, EVALUATION_TYPE::DEFAULT) {}

            /**
             * @brief Construct a new RGB_VALUE object using constexpr.
             * @param value The relative percentage of inherited from parent.
             * @param Default The default value state.
             * @param use_constexpr Flag indicating whether to use constexpr.
             * @details This constructor initializes an RGB_VALUE object with the given parameters,
             *          using constexpr for compile-time evaluation.
             */
            constexpr RGB_VALUE(const float value, VALUE_STATE Default = VALUE_STATE::VALUE) 
                : style_base(Default), Value(value, EVALUATION_TYPE::PERCENTAGE) {}
            
            constexpr RGB_VALUE() = default;

            /**
             * @brief Destructor for the RGB_VALUE class.
             * @details This destructor is necessary to ensure that the base class destructor is called.
             */
            ~RGB_VALUE() override { style_base::~style_base(); }

            style_base* Copy() const override {
                return new RGB_VALUE(*this);
            }

            /**
             * @brief Overload the assignment operator for RGB_VALUE.
             * @param other The other RGB_VALUE object to assign from.
             * @return A reference to this RGB_VALUE object.
             * @details This function assigns the value and status of the other RGB_VALUE object to this one.
             *          It only copies the information if the other object is enabled.
             */
            constexpr RGB_VALUE& operator=(const RGB_VALUE& other){
                // Only copy the information if the other is enabled.
                if (other.Status >= Status){
                    Value = other.Value;

                    Status = other.Status;
                }
                return *this;
            }

            /**
             * @brief Overload the assignment operator for RGB_VALUE.
             * @param other The other RGB_VALUE object to assign from.
             * @return A reference to this RGB_VALUE object.
             * @details This function assigns the RGB value of the other RGB_VALUE object to this one.
             *          It sets the status of the object to VALUE_STATE::VALUE after the assignment.
             */
            constexpr RGB_VALUE& operator=(const GGUI::RGB other){
                Value = other;
                Status = VALUE_STATE::VALUE;
                return *this;
            }
        
            /**
             * @brief Overload the compare operator for RGB_VALUE.
             * @param other The other RGB_VALUE object to compare against.
             * @return A boolean indicating whether the two RGB_VALUE objects are equal.
             * @details This function compares the value and status of the two RGB_VALUE objects.
             */
            constexpr bool operator==(const RGB_VALUE& other) const{
                return Value.Get<RGB>() == other.Value.Get<RGB>();
            }

            /**
             * @brief Overload the compare operator for RGB_VALUE.
             * @param other The other RGB_VALUE object to compare against.
             * @return A boolean indicating whether the two RGB_VALUE objects are not equal.
             * @details This function compares the value and status of the two RGB_VALUE objects.
             */
            constexpr bool operator!=(const RGB_VALUE& other) const{
                return !(Value.Get<RGB>() == other.Value.Get<RGB>());
            }

            /**
             * @brief Copy constructor for RGB_VALUE.
             * @param other The RGB_VALUE object to copy from.
             * @details This constructor creates a new RGB_VALUE object that is a copy of the other one.
             *          It copies the value and status of the other object, and sets the status to VALUE_STATE::VALUE.
             */
            constexpr RGB_VALUE(const GGUI::STYLING_INTERNAL::RGB_VALUE& other) : style_base(other.Status), Value(other.Value){}

            /**
             * @brief Embeds the value of an RGB_VALUE object into a Styling object.
             * @param host The Styling object to embed the value into.
             * @param owner The Element that owns the Styling object.
             * @return A STAIN_TYPE indicating the type of stain that was embedded.
             * @details This function does not actually embed any values and simply returns STAIN_TYPE::CLEAN.
             */
            STAIN_TYPE Embed_Value([[maybe_unused]] styling* host, element* owner) override;

            /**
             * @brief Evaluate the RGB_VALUE.
             * @param owner The styling owner to evaluate against.
             * @details This is a pure virtual function that subclasses must implement to define how the RGB value is evaluated.
             */
            void Evaluate([[maybe_unused]] styling* owner) override {};
        };

        class BOOL_VALUE : public style_base{
        public:
            bool Value = false;

            /**
             * @brief Construct a new constexpr BOOL_VALUE object.
             * @param value The boolean value to set.
             * @param Default The default value state to use.
             * @param use_constexpr A flag indicating whether to use constexpr. This parameter is not used.
             */
            constexpr BOOL_VALUE(bool value, VALUE_STATE Default = VALUE_STATE::VALUE) 
                : style_base(Default), Value(value) {}
            
            constexpr BOOL_VALUE() = default;

            /**
             * @brief Destructor for the BOOL_VALUE class.
             * @details This destructor is responsible for properly deallocating all the memory
             * allocated by the BOOL_VALUE object, including its parent class resources.
             */
            ~BOOL_VALUE() override { style_base::~style_base(); }

            style_base* Copy() const override {
                return new BOOL_VALUE(*this);
            }

            /**
             * @brief Overload the assignment operator for BOOL_VALUE.
             * @param other The other BOOL_VALUE object to assign from.
             * @return A reference to this BOOL_VALUE object.
             * @details This function assigns the value and status of the other BOOL_VALUE object to this one.
             *          It only copies the information if the other object is enabled.
             */
            constexpr BOOL_VALUE& operator=(const BOOL_VALUE& other){
                // Only copy the information if the other is enabled.
                if (other.Status >= Status){
                    Value = other.Value; // Copy the value from the other object
                    
                    Status = other.Status; // Update the status to the other object's status
                }
                return *this; // Return a reference to this object
            }

            /**
             * @brief Overload the assignment operator for BOOL_VALUE.
             * @param other The boolean value to assign to this BOOL_VALUE.
             * @return A reference to this BOOL_VALUE object.
             * @details This function assigns the boolean value to the BOOL_VALUE object,
             *          setting the status to VALUE_STATE::VALUE.
             */
            constexpr BOOL_VALUE& operator=(const bool other){
                Value = other; // Assign the boolean value to the Value member
                Status = VALUE_STATE::VALUE; // Set the status to indicate a valid value
                return *this; // Return a reference to this object
            }
        
            /**
             * @brief Copy constructor for BOOL_VALUE.
             * @param other The BOOL_VALUE object to copy from.
             * @details This constructor creates a new BOOL_VALUE object that is a copy of the other one.
             *          It copies the value and status of the other object.
             */
            constexpr BOOL_VALUE(const GGUI::STYLING_INTERNAL::BOOL_VALUE& other) 
                : style_base(other.Status), Value(other.Value) {}
            
            /**
             * @brief Evaluate the BOOL_VALUE.
             * @param owner The styling owner to evaluate against.
             * @details This function is a no-op for BOOL_VALUE, as it does not have any dynamically computable values.
             */
            void Evaluate([[maybe_unused]] styling* owner) override {};
            
            /**
             * @brief Embeds the value of a BOOL_VALUE object into a Styling object.
             * @param host The Styling object to embed the value into.
             * @param owner The Element that owns the Styling object.
             * @return A STAIN_TYPE indicating the type of stain that was embedded.
             * @details This function does not actually embed any values and simply returns STAIN_TYPE::CLEAN.
             */
            STAIN_TYPE Embed_Value([[maybe_unused]] styling* host,  element* owner) override;
        };
        
        class NUMBER_VALUE : public style_base{
        public:
            value<int> Value = 0;

            /**
             * @brief Construct a new NUMBER_VALUE object from a float.
             * @param value The floating point value to initialize the NUMBER_VALUE with.
             * @param Default The default value state of the NUMBER_VALUE.
             * @details This constructor initializes the NUMBER_VALUE with the provided float value and default state.
             *          The value is converted to a percentage (multiplying by 0.01) and stored as a float in the Value member.
             */
            constexpr NUMBER_VALUE(float value, VALUE_STATE Default = VALUE_STATE::VALUE) : style_base(Default), Value(value, EVALUATION_TYPE::PERCENTAGE){}

            /**
             * @brief Construct a new NUMBER_VALUE object from an integer using constexpr.
             * @param value The integer value to initialize the NUMBER_VALUE with.
             * @param Default The default value state of the NUMBER_VALUE.
             * @param use_constexpr A flag indicating whether to use constexpr. This parameter is not used.
             * @details This constructor initializes a NUMBER_VALUE object with the provided integer value and default state,
             *          using constexpr for compile-time evaluation.
             */
            constexpr NUMBER_VALUE(int value, VALUE_STATE Default = VALUE_STATE::VALUE) : style_base(Default), Value(value, EVALUATION_TYPE::DEFAULT){}
            
            constexpr NUMBER_VALUE(unsigned int value, VALUE_STATE Default = VALUE_STATE::VALUE) : style_base(Default), Value((signed int)value, EVALUATION_TYPE::DEFAULT){}

            constexpr NUMBER_VALUE() = default;

            /**
             * @brief Destructor for NUMBER_VALUE.
             * @details This destructor is responsible for cleaning up the resources allocated by the NUMBER_VALUE object.
             *          It is marked as `override` to ensure that it is called when the object is destroyed.
             */
            ~NUMBER_VALUE() override { style_base::~style_base(); }

            style_base* Copy() const override {
                return new NUMBER_VALUE(*this);
            }

            /**
             * @brief Overload the assignment operator for NUMBER_VALUE.
             * @param other The other NUMBER_VALUE object to assign from.
             * @return A reference to this NUMBER_VALUE object.
             * @details This function assigns the value and status of the other NUMBER_VALUE object to this one.
             *          It only copies the information if the other object is enabled.
             */
            constexpr NUMBER_VALUE& operator=(const NUMBER_VALUE& other){
                // Only copy the information if the other is enabled.
                if (other.Status >= Status){
                    Value = other.Value;

                    Status = other.Status;
                }
                return *this;
            }

            /**
             * @brief Overload the assignment operator for NUMBER_VALUE.
             * @param other The other number to assign from.
             * @return A reference to this NUMBER_VALUE object.
             * @details This function assigns the value of the other number to this NUMBER_VALUE object.
             *          It sets the status to VALUE_STATE::VALUE and returns the modified object.
             */
            constexpr NUMBER_VALUE& operator=(const int other){
                Value = other;
                Status = VALUE_STATE::VALUE;
                return *this;
            }

            /**
             * @brief Overload the compare operator for the NUMBER_VALUE class.
             * @param other The other NUMBER_VALUE object to compare against.
             * @return true if the two objects are equal; false otherwise.
             * @details This function compares the value and status of the two NUMBER_VALUE objects.
             */
            constexpr bool operator==(const NUMBER_VALUE& other) const{
                return Value.Get<int>() == other.Value.Get<int>();
            }

            /**
             * @brief Overload the compare operator for the NUMBER_VALUE class.
             * @param other The other NUMBER_VALUE object to compare against.
             * @return true if the two objects are not equal; false otherwise.
             * @details This function compares the value and status of the two NUMBER_VALUE objects.
             */
            constexpr bool operator!=(const NUMBER_VALUE& other) const{
                return Value.Get<int>() != other.Value.Get<int>();
            }
        
            /**
             * @brief Construct a new NUMBER_VALUE object from another NUMBER_VALUE object using constexpr.
             * @param other The other NUMBER_VALUE object to construct from.
             * @details This constructor initializes a new NUMBER_VALUE object with the same value and status as the given object.
             */
            constexpr NUMBER_VALUE(const GGUI::STYLING_INTERNAL::NUMBER_VALUE& other) : style_base(other.Status), Value(other.Value){}
            
            /**
             * @brief Embeds the value of a NUMBER_VALUE object into a Styling object.
             * @param host The Styling object to embed the value into.
             * @param owner The Element that owns the Styling object.
             * @return A STAIN_TYPE indicating the type of stain that was embedded.
             * @details This function does not actually embed any values and simply returns STAIN_TYPE::CLEAN.
             */
            STAIN_TYPE Embed_Value(styling* host, element* owner) override;
            
            /**
             * @brief Evaluate the RGB_VALUE.
             * @param owner The styling owner to evaluate against.
             * @details This is a pure virtual function that subclasses must implement to define how the RGB value is evaluated.
             *          When called, the function should evaluate the RGB value based on the owner object and set the Value property accordingly.
             */
            void Evaluate([[maybe_unused]] styling* owner) override {};

            /**
             * @brief Directly access the value of this NUMBER_VALUE object.
             * @return A reference to the value of this NUMBER_VALUE object.
             * @details This function returns a reference to the value of this NUMBER_VALUE object, allowing it to be directly accessed and modified.
             */
            inline constexpr int& Direct() { return Value.Direct<int>(); }
        };

        template<typename T>
        class ENUM_VALUE : public style_base{
        public:
            T Value;

            /**
             * @brief Construct a new ENUM_VALUE object using constexpr.
             * @param value The enum value to set.
             * @param Default The default value state.
             * @details This constructor initializes an ENUM_VALUE object with the given enum value and default state,
             *          using constexpr for compile-time evaluation.
             */
            constexpr ENUM_VALUE(T value, VALUE_STATE Default = VALUE_STATE::VALUE) : style_base(Default), Value(value){}

            constexpr ENUM_VALUE() = default;

            /**
             * @brief Destructor for ENUM_VALUE.
             * @details This destructor is responsible for cleaning up the resources allocated by the ENUM_VALUE object.
             *          It is marked as `override` to ensure that it is called when the object is destroyed.
             */
            ~ENUM_VALUE() override { style_base::~style_base(); }

            style_base* Copy() const override {
                return new ENUM_VALUE(*this);
            }

            /**
             * @brief Overload the assignment operator for ENUM_VALUE.
             * @param other The other ENUM_VALUE object to assign from.
             * @return A reference to this ENUM_VALUE object.
             * @details This function assigns the value and status of the other ENUM_VALUE object to this one.
             *          It only copies the information if the other object is enabled.
             */
            constexpr ENUM_VALUE& operator=(const ENUM_VALUE& other){
                // Only copy the information if the other is enabled.
                if (other.Status >= Status){
                    Value = other.Value;

                    Status = other.Status;
                }
                return *this;
            }

            /**
             * @brief Overload the assignment operator for ENUM_VALUE.
             * @param other The other value to assign from.
             * @return A reference to this ENUM_VALUE object.
             * @details This function assigns the value and status of the other ENUM_VALUE object to this one.
             *          It only copies the information if the other object is enabled.
             */
            constexpr ENUM_VALUE& operator=(const T other){
                Value = other;
                Status = VALUE_STATE::VALUE;
                return *this;
            }
        
            /**
             * @brief Copy constructor for ENUM_VALUE.
             * @param other The ENUM_VALUE object to copy from.
             * @details This constructor creates a new ENUM_VALUE object that is a copy of the other one.
             *          It copies the value and status of the other object.
             */
            constexpr ENUM_VALUE(const GGUI::STYLING_INTERNAL::ENUM_VALUE<T>& other) 
                : style_base(other.Status), Value(other.Value) {}
                        
            /**
             * @brief Evaluate the style.
             * @param owner The Styling object that owns this style.
             * @details This function is used to evaluate the style with the given Styling object.
             *          It is used to support dynamic values like percentage depended values.
             *          The function does not do anything as of now.
             */
            void Evaluate([[maybe_unused]] styling* owner) override {};

            /**
             * @brief Embed the value of this style into the given Styling object.
             * @param host The Styling object to embed the value into.
             * @param owner The Element that owns the Styling object.
             * @return The type of stain that this style embeds.
             * @details This function embeds the value of this style into the given Styling object.
             *          The value is not evaluated or modified in any way.
             *          It is used to support dynamic values like percentage depended values.
             *          The function does not do anything as of now.
             */
            STAIN_TYPE Embed_Value([[maybe_unused]] styling* host, [[maybe_unused]] element* owner) override { return STAIN_TYPE::CLEAN; };
        };
        
        class Vector : public style_base{
        public:
            value<int> X = 0;
            value<int> Y = 0;
            value<int> Z = 0;

            /**
             * @brief Construct a new Vector object using constexpr.
             * @param value The value to set.
             * @param Default The default value state.
             * @param use_constexpr Flag indicating whether to use constexpr.
             * @details This constructor initializes a Vector object with the given parameters,
             *          using constexpr for compile-time evaluation.
             */
            constexpr Vector(const GGUI::IVector3 value, VALUE_STATE Default = VALUE_STATE::VALUE) : style_base(Default), 
                X(value.X, EVALUATION_TYPE::DEFAULT), Y(value.Y, EVALUATION_TYPE::DEFAULT), Z(value.Z, EVALUATION_TYPE::DEFAULT){}
            
            constexpr Vector(value<int> x, value<int> y, value<int> z = 0, VALUE_STATE Default = VALUE_STATE::VALUE) : style_base(Default), X(x), Y(y), Z(z){}

            constexpr Vector() = default;

            /**
             * @brief Destructor for Vector.
             * @details This destructor is responsible for cleaning up all resources allocated by the Vector object.
             *          It calls the base class destructor (style_base::~style_base()) to ensure all parent class resources
             *          are properly cleaned up.
             */
            ~Vector() override { style_base::~style_base(); }

            style_base* Copy() const override {
                return new Vector(*this);
            }

            /**
             * @brief Overload the assignment operator for Vector.
             * @param other The other Vector object to assign from.
             * @return A reference to this Vector object.
             * @details This function assigns the value and status of the other Vector object to this one.
             *          It only copies the information if the other object is enabled.
             */
            constexpr Vector& operator=(const Vector& other){
                // Only copy the information if the other is enabled.
                if (other.Status >= Status){
                    X = other.X;
                    Y = other.Y;
                    Z = other.Z;

                    Status = other.Status;
                }
                return *this;
            }

            /**
             * @brief Overload the assignment operator for Vector.
             * @param other The IVector3 object to assign from.
             * @return A reference to this Vector object.
             * @details This function assigns the value and status of the other IVector3 object to this one.
             *          It sets the status to VALUE_STATE::VALUE and the value to the given IVector3 object.
             */
            constexpr Vector& operator=(const GGUI::IVector3 other){
                X = other.X;
                Y = other.Y;
                Z = other.Z;
                Status = VALUE_STATE::VALUE;
                return *this;
            }

            /** 
             * @brief Overload the equals to operator for Vector.
             * @param other The other Vector object to compare with.
             * @return A boolean indicating whether the two Vector objects are equal.
             */
            constexpr bool operator==(const Vector& other) const {
                return X == other.X && Y == other.Y && Z == other.Z;
            }

            /** 
             * @brief Overload the not equals to operator for Vector.
             * @param other The other Vector object to compare with.
             * @return A boolean indicating whether the two Vector objects are not equal.
             */
            constexpr bool operator!=(const Vector& other) const {
                return !(*this == other);
            }

            /**
             * @brief Construct a new constexpr Vector object from another Vector.
             * @param other The Vector to copy from.
             * @details This constructor initializes a Vector object using the status and value from another Vector object,
             *          utilizing constexpr for compile-time evaluation.
             */
            constexpr Vector(const GGUI::STYLING_INTERNAL::Vector& other) 
                : style_base(other.Status), X(other.X), Y(other.Y), Z(other.Z) {}
            
            /**
             * @brief Get the current value of the Vector.
             * @return The current IVector3 value.
             * @details Packs the values into an IVector3; Returns it.
             */
            inline constexpr IVector3 Get() { 
                return IVector3(X.Get<int>(), Y.Get<int>(), Z.Get<int>());
            }

            /**
             * @brief Get the current value of the Vector.
             * @return The current IVector3 value.
             * @details This function returns the current value stored in the Vector.
             *          This value can be used to get the current value of the Vector as an IVector3 object.
             */
            inline constexpr IVector3 Get() const { return IVector3(X.Get<int>(), Y.Get<int>(), Z.Get<int>()); }

            /**
             * @brief Set the current value of the Vector.
             * @param value The new value to set the Vector to.
             * @details This function sets the current value of the Vector to the given IVector3 object.
             *          It also sets the Status of the Vector to VALUE_STATE::VALUE.
             */
            inline constexpr void Set(IVector3 value){
                X = value.X;
                Y = value.Y;
                Z = value.Z;
                Status = VALUE_STATE::VALUE;
            }

            Vector operator+(const Vector& other) {
                // check if debug mode is on
                #ifdef GGUI_DEBUG
                assert(X.Get_Type() == other.X.Get_Type() && "two different evaluation types for X");
                assert(Y.Get_Type() == other.Y.Get_Type() && "two different evaluation types for Y");
                assert(Z.Get_Type() == other.Z.Get_Type() && "two different evaluation types for Z");
                #endif

                return Vector(X + other.X, Y + other.Y, Z + other.Z);
            }

            Vector operator-(const Vector& other){
                // check if debug mode is on
                #ifdef GGUI_DEBUG
                assert(X.Get_Type() != other.X.Get_Type() && "two different evaluation types for X");
                assert(Y.Get_Type() != other.Y.Get_Type() && "two different evaluation types for Y");
                assert(Z.Get_Type() != other.Z.Get_Type() && "two different evaluation types for Z");
                #endif

                return Vector(X - other.X, Y - other.Y, Z - other.Z);
            }
        
            /**
             * @brief Evaluate the Vector value.
             * @param owner The Styling object that the Vector is a part of.
             * @details This function evaluates the Vector value.
             *          For dynamically computable values like percentage depended this function is overridden.
             *          Currently it covers:
             *          - screen space
             */
            void Evaluate([[maybe_unused]] styling* owner) override {};
            
            /**
             * @brief Embeds the value of a Vector object into a Styling object.
             * @param host The Styling object to embed the value into.
             * @param owner The Element that owns the Styling object.
             * @return A STAIN_TYPE indicating the type of stain that was embedded.
             * @details This function does not actually embed any values and simply returns STAIN_TYPE::CLEAN.
             */
            STAIN_TYPE Embed_Value([[maybe_unused]] styling* host,  element* owner) override;
        };
    
        /**
         * @brief Checks if there is a leftover when multiplying two values.
         *
         * This function takes two parameters of type P and float, respectively, and checks if there is any leftover
         * when multiplying them. It does this by comparing the product of the integer parts of the parameters with
         * the product of the original parameters.
         *
         * @tparam P The type of the first parameter.
         * @param A The first parameter.
         * @param B The second parameter.
         * @return true if there is a leftover, false otherwise.
         */
        template<typename P>
        constexpr bool Has_Left_Over(P A, float B){
            return (static_cast<int>(A) * static_cast<int>(B)) - (static_cast<float>(A) * B) != 0;
        }

        /**
         * @brief Checks if a given value is a non-discriminant scalar.
         * @tparam P The type of the value to be checked.
         * @param value The value to be checked.
         * @param scalar The scalar value to compare against.
         * @return true if the value is a non-discriminant scalar, false otherwise.
         * 
         * @note Supported types: float | int | unsigned char | unsigned int | RGB | RGBA | FVector2 | FVector3 | IVector3 | GGUI::STYLING_INTERNAL::Vector | RGB_VALUE | NUMBER_VALUE
         */
        template<typename P>
        constexpr bool Is_Non_Discriminant_Scalar(const P value, const float scalar){
            // Skip checking for redundant scalars.
            if (scalar == 1.0f || scalar == 0.0f){
                return false;
            }

            // For types of int and float, we can use basic std::fmod
            if constexpr (std::is_same_v<P, float> || std::is_same_v<P, int> || std::is_same_v<P, unsigned char> || std::is_same_v<P, unsigned int>){
                return Has_Left_Over<P>(value, scalar);
            }
            else if constexpr (std::is_same_v<P, RGB>){
                return Is_Non_Discriminant_Scalar<unsigned char>(value.Red, scalar) && Is_Non_Discriminant_Scalar<unsigned char>(value.Green, scalar) && Is_Non_Discriminant_Scalar<unsigned char>(value.Blue, scalar);
            }
            else if constexpr (std::is_same_v<P, RGBA>){
                return Is_Non_Discriminant_Scalar<unsigned char>(value.Alpha, scalar) && Is_Non_Discriminant_Scalar<RGB>(value, scalar);
            }
            else if constexpr (std::is_same_v<P, FVector2>){
                return Is_Non_Discriminant_Scalar<float>(value.X, scalar) && Is_Non_Discriminant_Scalar<float>(value.Y, scalar);
            }
            else if constexpr (std::is_same_v<P, FVector3>){
                return Is_Non_Discriminant_Scalar<float>(value.Z, scalar) && Is_Non_Discriminant_Scalar<FVector2>(value, scalar);
            }
            else if constexpr (std::is_same_v<P, IVector3>){
                return Is_Non_Discriminant_Scalar<int>(value.X, scalar) && Is_Non_Discriminant_Scalar<int>(value.Y, scalar) && Is_Non_Discriminant_Scalar<int>(value.Z, scalar);
            }
            else if constexpr (std::is_same_v<P, GGUI::STYLING_INTERNAL::Vector>){
                return Is_Non_Discriminant_Scalar<IVector3>(value.Get(), scalar);
            }
            else if constexpr (std::is_same_v<P, RGB_VALUE>){
                return Is_Non_Discriminant_Scalar<RGB>(static_cast<RGB_VALUE>(value).Value.Get<RGB>(), scalar);
            }
            else if constexpr (std::is_same_v<P, NUMBER_VALUE>){
                return Is_Non_Discriminant_Scalar<int>(static_cast<NUMBER_VALUE>(value).Value.Get<int>(), scalar);
            }
            else {
                static_assert(!std::is_same_v<P, P>, "Unsupported type!");
            }
        }

        template<typename P>
        constexpr std::string To_String(const P value){
            if constexpr (std::is_same_v<P, std::string> || std::is_same_v<P, const char*> || std::is_same_v<P, char*>){
                // These are already strings
                return value;
            }
            else if constexpr (std::is_same_v<P, float> || std::is_same_v<P, int> || std::is_same_v<P, unsigned char> || std::is_same_v<P, unsigned int>){
                return std::to_string(value);
            }
            else if constexpr (std::is_same_v<P, RGB>){
                return To_String(value.Red) + ", " + To_String(value.Green) + ", " + To_String(value.Blue);
            }
            else if constexpr (std::is_same_v<P, RGBA>){
                return To_String(static_cast<RGB>(value)) + ", " + To_String(value.Alpha);
            }
            else if constexpr (std::is_same_v<P, FVector2>){
                return To_String(value.X) + ", " + To_String(value.Y);
            }
            else if constexpr (std::is_same_v<P, FVector3>){
                return To_String(static_cast<FVector2>(value)) + ", " + To_String(value.Z);
            }
            else if constexpr (std::is_same_v<P, IVector3>){
                return To_String(value.X) + ", " + To_String(value.Y) + ", " + To_String(value.Z);
            }
            else if constexpr (std::is_same_v<P, GGUI::STYLING_INTERNAL::Vector>){
                return To_String(value.Get());
            }
            else if constexpr (std::is_same_v<P, RGB_VALUE>){
                return To_String(static_cast<RGB_VALUE>(value).Value.Get<RGB>());
            }
            else if constexpr (std::is_same_v<P, NUMBER_VALUE>){
                return To_String(static_cast<NUMBER_VALUE>(value).Value.Get<int>());
            }
            else {
                static_assert(!std::is_same_v<P, P>, "Unsupported type!");
            }
        }
    }

    class position : public STYLING_INTERNAL::Vector{
    public:
        constexpr position(IVector3 value, VALUE_STATE Default = VALUE_STATE::VALUE) : Vector(value, Default){}

        constexpr position(Vector&& value, VALUE_STATE Default = VALUE_STATE::VALUE) : Vector(value.X, value.Y, value.Z, Default){
            Transform_Center_To_Top_Left_Origin();
        }

        constexpr position(STYLING_INTERNAL::value<int> X, STYLING_INTERNAL::value<int> Y, STYLING_INTERNAL::value<int> Z = 0, VALUE_STATE Default = VALUE_STATE::VALUE) : Vector(X, Y, Z, Default){
            Transform_Center_To_Top_Left_Origin();
        }

        ~position() override { style_base::~style_base(); }

        style_base* Copy() const override {
            return new position(*this);
        }

        constexpr position(const GGUI::position& other) : Vector(other){}

        constexpr position& operator=(const position& other) = default;

        // for dynamically computable values like percentage depended
        // currently covers:
        // - screen space
        void Evaluate(styling* owner) override;

        STAIN_TYPE Embed_Value(styling* host, element* owner) override;
    protected:
        /**
         * @brief Transforms the position from center origin to top-left origin.
         *
         * This function adjusts the X and Y coordinates of the position by adding a 0.5f offset
         * if their evaluation type is PERCENTAGE. This transformation is used to convert a 
         * position that is originally centered to one that is based on the top-left corner.
         *
         * @note The Z coordinate is not affected by this transformation.
         */
        constexpr void Transform_Center_To_Top_Left_Origin(){
            // Add 0.5f offset to the vectors
            if (X.Get_Type() == EVALUATION_TYPE::PERCENTAGE)
                X = X + 0.5f;
            if (Y.Get_Type() == EVALUATION_TYPE::PERCENTAGE)
                Y = Y + 0.5f;
            // no need to affect Z.
        }
    };

    class width : public STYLING_INTERNAL::NUMBER_VALUE{
    public:
        constexpr width(int value, VALUE_STATE Default = VALUE_STATE::VALUE) : NUMBER_VALUE(value, Default){}
        constexpr width(unsigned int value, VALUE_STATE Default = VALUE_STATE::VALUE) : NUMBER_VALUE(value, Default){}

        constexpr width(float value, VALUE_STATE Default = VALUE_STATE::VALUE) : NUMBER_VALUE(value, Default){}

        ~width() override { style_base::~style_base(); }

        style_base* Copy() const override {
            return new width(*this);
        }

        constexpr width(const GGUI::width& other) : NUMBER_VALUE(other){}

        constexpr width& operator=(const width& other) = default;

        void Evaluate(styling* owner) override;

        STAIN_TYPE Embed_Value(styling* host, element* owner) override;

        inline constexpr int Get() { return Value.Get<int>(); }

        void Set(int value){
            Value = value;
            Status = VALUE_STATE::VALUE;
        }
    };

    class height : public STYLING_INTERNAL::NUMBER_VALUE{
    public:
        constexpr height(int value, VALUE_STATE Default = VALUE_STATE::VALUE) : NUMBER_VALUE(value, Default){}
        constexpr height(unsigned int value, VALUE_STATE Default = VALUE_STATE::VALUE) : NUMBER_VALUE(value, Default){}

        constexpr height(float value, VALUE_STATE Default = VALUE_STATE::VALUE) : NUMBER_VALUE(value, Default){}

        ~height() override { style_base::~style_base(); }

        style_base* Copy() const override {
            return new height(*this);
        }

        constexpr height(const GGUI::height& other) : NUMBER_VALUE(other){}

        constexpr height& operator=(const height& other) = default;
        
        // for dynamically computable values like percentage depended
        // currently covers:
        // - screen space
        void Evaluate(styling* owner) override;
        
        STAIN_TYPE Embed_Value(styling* host, element* owner) override;

        inline constexpr int Get() { return Value.Get<int>(); }

        void Set(int value){
            Value = value;
            Status = VALUE_STATE::VALUE;
        }
    };

    class enable_border : public STYLING_INTERNAL::BOOL_VALUE{
    public:
        constexpr enable_border(bool value, VALUE_STATE Default = VALUE_STATE::VALUE) : BOOL_VALUE(value, Default){}

        constexpr enable_border(const GGUI::enable_border& other) : BOOL_VALUE(other.Value, other.Status){}

        ~enable_border() override { style_base::~style_base(); }

        style_base* Copy() const override {
            return new enable_border(*this);
        }

        constexpr enable_border& operator=(const enable_border& other) = default;

        constexpr bool operator==(const enable_border& other) const{
            return Value == other.Value;
        }

        constexpr bool operator!=(const enable_border& other) const{
            return Value != other.Value;
        }

        // for dynamically computable values like percentage depended
        // currently covers:
        // - screen space
        void Evaluate([[maybe_unused]] styling* owner) override {};
        
        STAIN_TYPE Embed_Value(styling* host, element* owner) override;
    };

    class text_color : public STYLING_INTERNAL::RGB_VALUE{
    public:
        constexpr text_color(RGB color, VALUE_STATE Default = VALUE_STATE::VALUE) : RGB_VALUE(color, Default){}

        constexpr text_color(float relative_percentage, VALUE_STATE Default = VALUE_STATE::VALUE) : RGB_VALUE(relative_percentage, Default){}

        ~text_color() override { style_base::~style_base(); }

        style_base* Copy() const override {
            return new text_color(*this);
        }

        constexpr text_color(const GGUI::text_color& other) : RGB_VALUE(other){}

        constexpr text_color& operator=(const text_color& other) = default;

        // for dynamically computable values like percentage depended
        // currently covers:
        // - screen space
        void Evaluate(styling* owner) override;
        
        STAIN_TYPE Embed_Value(styling* host, element* owner) override;
    };

    class background_color : public STYLING_INTERNAL::RGB_VALUE{
    public:
        constexpr background_color(RGB color, VALUE_STATE Default = VALUE_STATE::VALUE) : RGB_VALUE(color, Default){}

        constexpr background_color(float relative_percentage, VALUE_STATE Default = VALUE_STATE::VALUE) : RGB_VALUE(relative_percentage, Default){}

        ~background_color() override { style_base::~style_base(); }

        style_base* Copy() const override {
            return new background_color(*this);
        }

        constexpr background_color(const GGUI::background_color& other) : RGB_VALUE(other){}

        constexpr background_color& operator=(const background_color& other) = default;

        // for dynamically computable values like percentage depended
        // currently covers:
        // - screen space
        void Evaluate(styling* owner) override;
        
        STAIN_TYPE Embed_Value(styling* host, element* owner) override;
    };

    class border_color : public STYLING_INTERNAL::RGB_VALUE{
    public:
        constexpr border_color(RGB color, VALUE_STATE Default = VALUE_STATE::VALUE) : RGB_VALUE(color, Default){}

        constexpr border_color(float relative_percentage, VALUE_STATE Default = VALUE_STATE::VALUE) : RGB_VALUE(relative_percentage, Default){}

        ~border_color() override { style_base::~style_base(); }

        style_base* Copy() const override {
            return new border_color(*this);
        }

        constexpr border_color(const GGUI::border_color& other) : RGB_VALUE(other){}

        constexpr border_color& operator=(const border_color& other) = default;

        // for dynamically computable values like percentage depended
        // currently covers:
        // - screen space
        void Evaluate(styling* owner) override;
        
        STAIN_TYPE Embed_Value(styling* host, element* owner) override;
    };

    class border_background_color : public STYLING_INTERNAL::RGB_VALUE{
    public:
        constexpr border_background_color(RGB color, VALUE_STATE Default = VALUE_STATE::VALUE) : RGB_VALUE(color, Default){}

        constexpr border_background_color(float relative_percentage, VALUE_STATE Default = VALUE_STATE::VALUE) : RGB_VALUE(relative_percentage, Default){}

        ~border_background_color() override { style_base::~style_base(); }

        style_base* Copy() const override {
            return new border_background_color(*this);
        }

        constexpr border_background_color(const GGUI::border_background_color& other) : RGB_VALUE(other){}

        constexpr border_background_color& operator=(const border_background_color& other) = default;
        
        // for dynamically computable values like percentage depended
        // currently covers:
        // - screen space
        void Evaluate(styling* owner) override;
        
        STAIN_TYPE Embed_Value(styling* host, element* owner) override;
    };

    class hover_border_color : public STYLING_INTERNAL::RGB_VALUE{
    public:
        constexpr hover_border_color(RGB color, VALUE_STATE Default = VALUE_STATE::VALUE) : RGB_VALUE(color, Default){}

        constexpr hover_border_color(float relative_percentage, VALUE_STATE Default = VALUE_STATE::VALUE) : RGB_VALUE(relative_percentage, Default){}

        ~hover_border_color() override { style_base::~style_base(); }

        style_base* Copy() const override {
            return new hover_border_color(*this);
        }

        constexpr hover_border_color(const GGUI::hover_border_color& other) : RGB_VALUE(other){}

        constexpr hover_border_color& operator=(const hover_border_color& other) = default;
        
        // for dynamically computable values like percentage depended
        // currently covers:
        // - screen space
        void Evaluate(styling* owner) override;
        
        STAIN_TYPE Embed_Value(styling* host, element* owner) override;
    };

    class hover_text_color : public STYLING_INTERNAL::RGB_VALUE{
    public:
        constexpr hover_text_color(RGB color, VALUE_STATE Default = VALUE_STATE::VALUE) : RGB_VALUE(color, Default){}

        constexpr hover_text_color(float  relative_percentage, VALUE_STATE Default = VALUE_STATE::VALUE) : RGB_VALUE(relative_percentage, Default){}

        ~hover_text_color() override { style_base::~style_base(); }

        style_base* Copy() const override {
            return new hover_text_color(*this);
        }

        constexpr hover_text_color(const GGUI::hover_text_color& other) : RGB_VALUE(other){}

        constexpr hover_text_color& operator=(const hover_text_color& other) = default;
        
        // for dynamically computable values like percentage depended
        // currently covers:
        // - screen space
        void Evaluate(styling* owner) override;
        
        STAIN_TYPE Embed_Value(styling* host, element* owner) override;
    };

    class hover_background_color : public STYLING_INTERNAL::RGB_VALUE{
    public:
        constexpr hover_background_color(RGB color, VALUE_STATE Default = VALUE_STATE::VALUE) : RGB_VALUE(color, Default){}

        constexpr hover_background_color(float relative_percentage, VALUE_STATE Default = VALUE_STATE::VALUE) : RGB_VALUE(relative_percentage, Default){}

        ~hover_background_color() override { style_base::~style_base(); }

        style_base* Copy() const override {
            return new hover_background_color(*this);
        }

        constexpr hover_background_color(const GGUI::hover_background_color& other) : RGB_VALUE(other){}

        constexpr hover_background_color& operator=(const hover_background_color& other) = default;
        
        // for dynamically computable values like percentage depended
        // currently covers:
        // - screen space
        void Evaluate(styling* owner) override;
        
        STAIN_TYPE Embed_Value(styling* host, element* owner) override;
    };

    class hover_border_background_color : public STYLING_INTERNAL::RGB_VALUE{
    public:
        constexpr hover_border_background_color(RGB color, VALUE_STATE Default = VALUE_STATE::VALUE) : RGB_VALUE(color, Default){}

        constexpr hover_border_background_color(float relative_percentage, VALUE_STATE Default = VALUE_STATE::VALUE) : RGB_VALUE(relative_percentage, Default){}

        ~hover_border_background_color() override { style_base::~style_base(); }

        style_base* Copy() const override {
            return new hover_border_background_color(*this);
        }

        constexpr hover_border_background_color(const GGUI::hover_border_background_color& other) : RGB_VALUE(other){}

        constexpr hover_border_background_color& operator=(const hover_border_background_color& other) = default;
        
        // for dynamically computable values like percentage depended
        // currently covers:
        // - screen space
        void Evaluate(styling* owner) override;
        
        STAIN_TYPE Embed_Value(styling* host, element* owner) override;
    };

    class focus_border_color : public STYLING_INTERNAL::RGB_VALUE{
    public:
        constexpr focus_border_color(RGB color, VALUE_STATE Default = VALUE_STATE::VALUE) : RGB_VALUE(color, Default){}

        constexpr focus_border_color(float relative_percentage, VALUE_STATE Default = VALUE_STATE::VALUE) : RGB_VALUE(relative_percentage, Default){}

        ~focus_border_color() override { style_base::~style_base(); }

        style_base* Copy() const override {
            return new focus_border_color(*this);
        }

        constexpr focus_border_color(const GGUI::focus_border_color& other) : RGB_VALUE(other){}

        constexpr focus_border_color& operator=(const focus_border_color& other) = default;
        
        // for dynamically computable values like percentage depended
        // currently covers:
        // - screen space
        void Evaluate(styling* owner) override;
        
        STAIN_TYPE Embed_Value(styling* host, element* owner) override;
    };

    class focus_text_color : public STYLING_INTERNAL::RGB_VALUE{
    public:
        constexpr focus_text_color(RGB color, VALUE_STATE Default = VALUE_STATE::VALUE) : RGB_VALUE(color, Default){}

        constexpr focus_text_color(float relative_percentage, VALUE_STATE Default = VALUE_STATE::VALUE) : RGB_VALUE(relative_percentage, Default){}

        ~focus_text_color() override { style_base::~style_base(); }

        style_base* Copy() const override {
            return new focus_text_color(*this);
        }

        constexpr focus_text_color(const GGUI::focus_text_color& other) : RGB_VALUE(other){}

        constexpr focus_text_color& operator=(const focus_text_color& other) = default;
        
        // for dynamically computable values like percentage depended
        // currently covers:
        // - screen space
        void Evaluate(styling* owner) override;
        
        STAIN_TYPE Embed_Value(styling* host, element* owner) override;
    };

    class focus_background_color : public STYLING_INTERNAL::RGB_VALUE{
    public:
        constexpr focus_background_color(RGB color, VALUE_STATE Default = VALUE_STATE::VALUE) : RGB_VALUE(color, Default){}

        constexpr focus_background_color(float relative_percentage, VALUE_STATE Default = VALUE_STATE::VALUE) : RGB_VALUE(relative_percentage, Default){}

        ~focus_background_color() override { style_base::~style_base(); }

        style_base* Copy() const override {
            return new focus_background_color(*this);
        }

        constexpr focus_background_color(const GGUI::focus_background_color& other) : RGB_VALUE(other){}

        constexpr focus_background_color& operator=(const focus_background_color& other) = default;
        
        // for dynamically computable values like percentage depended
        // currently covers:
        // - screen space
        void Evaluate(styling* owner) override;
        
        STAIN_TYPE Embed_Value(styling* host, element* owner) override;
    };

    class focus_border_background_color : public STYLING_INTERNAL::RGB_VALUE{
    public:
        constexpr focus_border_background_color(RGB color, VALUE_STATE Default = VALUE_STATE::VALUE) : RGB_VALUE(color, Default){}

        constexpr focus_border_background_color(float relative_percentage, VALUE_STATE Default = VALUE_STATE::VALUE) : RGB_VALUE(relative_percentage, Default){}

        ~focus_border_background_color() override { style_base::~style_base(); }

        style_base* Copy() const override {
            return new focus_border_background_color(*this);
        }

        constexpr focus_border_background_color(const GGUI::focus_border_background_color& other) : RGB_VALUE(other){}

        constexpr focus_border_background_color& operator=(const focus_border_background_color& other) = default;
        
        // for dynamically computable values like percentage depended
        // currently covers:
        // - screen space
        void Evaluate(styling* owner) override;
        
        STAIN_TYPE Embed_Value(styling* host, element* owner) override;
    };

    class styled_border : public STYLING_INTERNAL::style_base{
    public:
        const char* TOP_LEFT_CORNER             = "┌";//"\e(0\x6c\e(B";
        const char* BOTTOM_LEFT_CORNER          = "└";//"\e(0\x6d\e(B";
        const char* TOP_RIGHT_CORNER            = "┐";//"\e(0\x6b\e(B";
        const char* BOTTOM_RIGHT_CORNER         = "┘";//"\e(0\x6a\e(B";
        const char* VERTICAL_LINE               = "│";//"\e(0\x78\e(B";
        const char* HORIZONTAL_LINE             = "─";//"\e(0\x71\e(B";
        const char* VERTICAL_RIGHT_CONNECTOR    = "├";//"\e(0\x74\e(B";
        const char* VERTICAL_LEFT_CONNECTOR     = "┤";//"\e(0\x75\e(B";
        const char* HORIZONTAL_BOTTOM_CONNECTOR = "┬";//"\e(0\x76\e(B";
        const char* HORIZONTAL_TOP_CONNECTOR    = "┴";//"\e(0\x77\e(B";
        const char* CROSS_CONNECTOR             = "┼";//"\e(0\x6e\e(B";
    
        /**
         * @brief A structure to hold the border style of a widget.
         *
         * The style is represented as a vector of strings, each string being a character
         * that will be used to draw the border of the widget. The vector must have the
         * following size and order:
         * - 0: Top left corner
         * - 1: Bottom left corner
         * - 2: Top right corner
         * - 3: Bottom right corner
         * - 4: Vertical line
         * - 5: Horizontal line
         * - 6: Vertical right connector
         * - 7: Vertical left connector
         * - 8: Horizontal bottom connector
         * - 9: Horizontal top connector
         * - 10: Cross connector
         */
        constexpr styled_border(std::array<const char*, 11> values, VALUE_STATE Default = VALUE_STATE::VALUE) : style_base(Default){
            TOP_LEFT_CORNER = values[0];
            BOTTOM_LEFT_CORNER = values[1];
            TOP_RIGHT_CORNER = values[2];
            BOTTOM_RIGHT_CORNER = values[3];
            VERTICAL_LINE = values[4];
            HORIZONTAL_LINE = values[5];
            VERTICAL_RIGHT_CONNECTOR = values[6];
            VERTICAL_LEFT_CONNECTOR = values[7];
            HORIZONTAL_BOTTOM_CONNECTOR = values[8];
            HORIZONTAL_TOP_CONNECTOR = values[9];
            CROSS_CONNECTOR = values[10];
        }

        constexpr styled_border() = default;

        ~styled_border() override { style_base::~style_base(); }

        style_base* Copy() const override {
            return new styled_border(*this);
        }

        constexpr styled_border& operator=(const styled_border& other){
            if (other.Status >= Status){
                TOP_LEFT_CORNER = other.TOP_LEFT_CORNER;
                BOTTOM_LEFT_CORNER = other.BOTTOM_LEFT_CORNER;
                TOP_RIGHT_CORNER = other.TOP_RIGHT_CORNER;
                BOTTOM_RIGHT_CORNER = other.BOTTOM_RIGHT_CORNER;
                VERTICAL_LINE = other.VERTICAL_LINE;
                HORIZONTAL_LINE = other.HORIZONTAL_LINE;
                VERTICAL_RIGHT_CONNECTOR = other.VERTICAL_RIGHT_CONNECTOR;
                VERTICAL_LEFT_CONNECTOR = other.VERTICAL_LEFT_CONNECTOR;
                HORIZONTAL_BOTTOM_CONNECTOR = other.HORIZONTAL_BOTTOM_CONNECTOR;
                HORIZONTAL_TOP_CONNECTOR = other.HORIZONTAL_TOP_CONNECTOR;
                CROSS_CONNECTOR = other.CROSS_CONNECTOR;

                Status = other.Status;
            }
            return *this;
        }
    
        constexpr styled_border(const GGUI::styled_border& other) : style_base(other.Status){
            TOP_LEFT_CORNER = other.TOP_LEFT_CORNER;
            BOTTOM_LEFT_CORNER = other.BOTTOM_LEFT_CORNER;
            TOP_RIGHT_CORNER = other.TOP_RIGHT_CORNER;
            BOTTOM_RIGHT_CORNER = other.BOTTOM_RIGHT_CORNER;
            VERTICAL_LINE = other.VERTICAL_LINE;
            HORIZONTAL_LINE = other.HORIZONTAL_LINE;
            VERTICAL_RIGHT_CONNECTOR = other.VERTICAL_RIGHT_CONNECTOR;
            VERTICAL_LEFT_CONNECTOR = other.VERTICAL_LEFT_CONNECTOR;
            HORIZONTAL_BOTTOM_CONNECTOR = other.HORIZONTAL_BOTTOM_CONNECTOR;
            HORIZONTAL_TOP_CONNECTOR = other.HORIZONTAL_TOP_CONNECTOR;
            CROSS_CONNECTOR = other.CROSS_CONNECTOR;
        }

        void Evaluate([[maybe_unused]] styling* owner) override {};
        
        STAIN_TYPE Embed_Value(styling* host, element* owner) override;
    };

    class flow_priority : public STYLING_INTERNAL::ENUM_VALUE<DIRECTION>{
    public:
        constexpr flow_priority(DIRECTION value, VALUE_STATE Default = VALUE_STATE::VALUE) : ENUM_VALUE(value, Default){}

        ~flow_priority() override { style_base::~style_base(); }

        style_base* Copy() const override {
            return new flow_priority(*this);
        }

        constexpr flow_priority(const GGUI::flow_priority& other) : ENUM_VALUE(other.Value, other.Status){}

        constexpr flow_priority& operator=(const flow_priority& other) = default;
        
        // for dynamically computable values like percentage depended
        // currently covers:
        // - screen space
        void Evaluate([[maybe_unused]] styling* owner) override {};

        STAIN_TYPE Embed_Value(styling* host, element* owner) override;
    };

    class wrap : public STYLING_INTERNAL::BOOL_VALUE{
    public:
        constexpr wrap(bool value, VALUE_STATE Default = VALUE_STATE::VALUE) : BOOL_VALUE(value, Default){}

        ~wrap() override { style_base::~style_base(); }

        style_base* Copy() const override {
            return new wrap(*this);
        }

        constexpr wrap(const GGUI::wrap& other) : BOOL_VALUE(other.Value, other.Status){}

        constexpr wrap& operator=(const wrap& other) = default;
        
        // for dynamically computable values like percentage depended
        // currently covers:
        // - screen space
        void Evaluate([[maybe_unused]] styling* owner) override {};
        
        STAIN_TYPE Embed_Value(styling* host, element* owner) override;
    };

    class allow_overflow : public STYLING_INTERNAL::BOOL_VALUE{
    public:
        constexpr allow_overflow(bool value, VALUE_STATE Default = VALUE_STATE::VALUE) : BOOL_VALUE(value, Default){}

        ~allow_overflow() override { style_base::~style_base(); }

        style_base* Copy() const override {
            return new allow_overflow(*this);
        }

        constexpr allow_overflow(const GGUI::allow_overflow& other) : BOOL_VALUE(other.Value, other.Status){}

        constexpr allow_overflow& operator=(const allow_overflow& other) = default;
        
        // for dynamically computable values like percentage depended
        // currently covers:
        // - screen space
        void Evaluate([[maybe_unused]] styling* owner) override {};
        
        STAIN_TYPE Embed_Value(styling* host, element* owner) override;
    };

    class allow_dynamic_size : public STYLING_INTERNAL::BOOL_VALUE{
    public:
        constexpr allow_dynamic_size(bool value, VALUE_STATE Default = VALUE_STATE::VALUE) : BOOL_VALUE(value, Default){}

        ~allow_dynamic_size() override { style_base::~style_base(); }

        style_base* Copy() const override {
            return new allow_dynamic_size(*this);
        }

        constexpr allow_dynamic_size(const GGUI::allow_dynamic_size& other) : BOOL_VALUE(other.Value, other.Status){}

        constexpr allow_dynamic_size& operator=(const allow_dynamic_size& other) = default;
        
        // for dynamically computable values like percentage depended
        // currently covers:
        // - screen space
        void Evaluate([[maybe_unused]] styling* owner) override {};
        
        STAIN_TYPE Embed_Value(styling* host, element* owner) override;
    };

    class margin : public STYLING_INTERNAL::style_base{
    public:
        STYLING_INTERNAL::value<unsigned int> Top = (unsigned)0;
        STYLING_INTERNAL::value<unsigned int> Bottom = (unsigned)0;
        STYLING_INTERNAL::value<unsigned int> Left = (unsigned)0;
        STYLING_INTERNAL::value<unsigned int> Right = (unsigned)0;

        constexpr margin(unsigned int top, unsigned int bottom, unsigned int left, unsigned int right, VALUE_STATE Default = VALUE_STATE::VALUE) : style_base(Default), Top(top), Bottom(bottom), Left(left), Right(right){}

        constexpr margin() = default;

        ~margin() override { style_base::~style_base(); }

        style_base* Copy() const override {
            return new margin(*this);
        }

        // operator overload for copy operator
        constexpr margin& operator=(const margin& other){
            // Only copy the information if the other is enabled.
            if (other.Status >= Status){
                Top = other.Top;
                Bottom = other.Bottom;
                Left = other.Left;
                Right = other.Right;

                Status = other.Status;
            }
            return *this;
        }

        constexpr margin(const GGUI::margin& other) : style_base(other.Status), Top(other.Top), Bottom(other.Bottom), Left(other.Left), Right(other.Right){}
        
        // for dynamically computable values like percentage depended
        // currently covers:
        // - screen space
        void Evaluate(styling* owner) override;
        
        STAIN_TYPE Embed_Value(styling* host, element* owner) override;
    };

    class shadow : public STYLING_INTERNAL::style_base{
    public:
        STYLING_INTERNAL::value<FVector3> Direction = FVector3{0, 0, 0.5};
        STYLING_INTERNAL::value<RGB> Color = RGB{};
        float Opacity = 1;
        bool Enabled = false;

        constexpr shadow(FVector3 direction, RGB color, float opacity, bool enabled, VALUE_STATE Default = VALUE_STATE::VALUE) : style_base(Default){
            Direction = direction;
            Color = color;
            Opacity = opacity;
            Enabled = enabled;
        }

        constexpr shadow() = default;

        ~shadow() override { style_base::~style_base(); }

        style_base* Copy() const override {
            return new shadow(*this);
        }

        constexpr shadow& operator=(const shadow& other){
            // Only copy the information if the other is enabled.
            if (other.Status >= Status){
                Direction = other.Direction;
                Color = other.Color;
                Opacity = other.Opacity;
                Enabled = other.Enabled;

                Status = other.Status;
            }
            return *this;
        }
    
        constexpr shadow(const GGUI::shadow& other) : style_base(other.Status), Direction(other.Direction), Color(other.Color), Opacity(other.Opacity), Enabled(other.Enabled){}
    
        // for dynamically computable values like percentage depended
        // currently covers:
        // - screen space
        void Evaluate(styling* owner) override;
        
        STAIN_TYPE Embed_Value(styling* host, element* owner) override;
    };

    class opacity : public STYLING_INTERNAL::style_base{
    protected:
        float Value = 1.0f;
    public:
        constexpr opacity(float value, VALUE_STATE state = VALUE_STATE::VALUE) : style_base(state), Value(value){}

        ~opacity() override { style_base::~style_base(); }

        style_base* Copy() const override {
            return new opacity(*this);
        }

        constexpr opacity& operator=(const opacity& other){
            // Only copy the information if the other is enabled.
            if (other.Status >= Status){
                Value = other.Value;

                Status = other.Status;
            }
            return *this;
        }

        constexpr opacity(const GGUI::opacity& other) : style_base(other.Status), Value(other.Value){}

        // Since opacity always represents an percentile of its self being displayed on top of its parent.
        void Evaluate([[maybe_unused]] styling* owner) override {};
        
        STAIN_TYPE Embed_Value(styling* host, element* owner) override;

        inline constexpr float Get() { return Value; }

        inline constexpr void Set(float value){
            Value = value;
            Status = VALUE_STATE::VALUE;
        }
    };

    class allow_scrolling : public STYLING_INTERNAL::BOOL_VALUE{
    public:
        constexpr allow_scrolling(bool value, VALUE_STATE Default = VALUE_STATE::VALUE) : BOOL_VALUE(value, Default){}

        ~allow_scrolling() override { style_base::~style_base(); }

        style_base* Copy() const override {
            return new allow_scrolling(*this);
        }

        constexpr allow_scrolling(const GGUI::allow_scrolling& other) : BOOL_VALUE(other.Value, other.Status){}

        constexpr allow_scrolling& operator=(const allow_scrolling& other) = default;
        
        // for dynamically computable values like percentage depended
        // currently covers:
        // - screen space
        void Evaluate([[maybe_unused]] styling* owner) override {};
        
        STAIN_TYPE Embed_Value(styling* host, element* owner) override;
    };

    class align : public STYLING_INTERNAL::ENUM_VALUE<ALIGN>{
    public:
        constexpr align(ALIGN value, VALUE_STATE Default = VALUE_STATE::VALUE) : ENUM_VALUE(value, Default){}

        ~align() override { style_base::~style_base(); }

        style_base* Copy() const override {
            return new align(*this);
        }

        constexpr align(const GGUI::align& other) : ENUM_VALUE(other.Value, other.Status){}

        constexpr align& operator=(const align& other) = default;
        
        // for dynamically computable values like percentage depended
        // currently covers:
        // - screen space
        void Evaluate([[maybe_unused]] styling* owner) override {};
        
        STAIN_TYPE Embed_Value(styling* host, element* owner) override;
    };

    class node : public STYLING_INTERNAL::style_base{
    public:
        element* Value;

        constexpr node(element* value = nullptr, VALUE_STATE Default = VALUE_STATE::VALUE) : style_base(Default, EMBED_ORDER::DELAYED), Value(value){}
        
        constexpr node(const GGUI::node& other) : style_base(other.Status, EMBED_ORDER::DELAYED), Value(other.Value){}

        ~node() override { style_base::~style_base(); }

        style_base* Copy() const override;

        constexpr node& operator=(const node& other){
            // Only copy the information if the other is enabled.
            if (other.Status >= Status){
                Value = other.Value;

                Status = other.Status;

                Order = other.Order;
            }
            return *this;
        }

        void Evaluate([[maybe_unused]] styling* owner) override {};

        STAIN_TYPE Embed_Value(styling* host, element* owner) override;
    };

    class childs : public STYLING_INTERNAL::style_base{
    public:
        std::initializer_list<element*> Value;

        constexpr childs(std::initializer_list<element*> value, VALUE_STATE Default = VALUE_STATE::VALUE) : style_base(Default, EMBED_ORDER::DELAYED), Value(value){}

        constexpr childs(const GGUI::childs& other) : style_base(other.Status, EMBED_ORDER::DELAYED), Value(other.Value){}

        ~childs() override { style_base::~style_base(); }

        style_base* Copy() const override;

        constexpr childs& operator=(const childs& other){
            // Only copy the information if the other is enabled.
            if (other.Status >= Status){
                Value = other.Value;

                Status = other.Status;

                Order = other.Order;
            }
            return *this;
        }

        void Evaluate([[maybe_unused]] styling* owner) override {};

        STAIN_TYPE Embed_Value(styling* host, element* owner) override;
    };

    class on_init : public STYLING_INTERNAL::style_base{
    public:
        void (*Value)(element* self);

        constexpr on_init(void (*value)(element* self), VALUE_STATE Default = VALUE_STATE::VALUE) : style_base(Default), Value(value){}

        constexpr on_init(const GGUI::on_init& other) : style_base(other.Status), Value(other.Value){}

        ~on_init() override { style_base::~style_base(); }

        style_base* Copy() const override {
            return new on_init(*this);
        }

        constexpr on_init& operator=(const on_init& other){
            // Only copy the information if the other is enabled.
            if (other.Status >= Status){
                Value = other.Value;

                Status = other.Status;
            }
            return *this;
        }

        void Evaluate([[maybe_unused]] styling* owner) override {};

        STAIN_TYPE Embed_Value(styling* host, element* owner) override;
    };

    class on_destroy : public STYLING_INTERNAL::style_base{
    public:
        void (*Value)(element* self);

        constexpr on_destroy(void (*value)(element* self), VALUE_STATE Default = VALUE_STATE::VALUE) : style_base(Default), Value(value){}

        constexpr on_destroy(const GGUI::on_destroy& other) : style_base(other.Status), Value(other.Value){}

        ~on_destroy() override { style_base::~style_base(); }

        style_base* Copy() const override {
            return new on_destroy(*this);
        }

        constexpr on_destroy& operator=(const on_destroy& other){
            // Only copy the information if the other is enabled.
            if (other.Status >= Status){
                Value = other.Value;

                Status = other.Status;
            }
            return *this;
        }

        void Evaluate([[maybe_unused]] styling* owner) override {};

        STAIN_TYPE Embed_Value(styling* host, element* owner) override;
    };

    class on_hide : public STYLING_INTERNAL::style_base{
    public:
        void (*Value)(element* self);

        constexpr on_hide(void (*value)(element* self), VALUE_STATE Default = VALUE_STATE::VALUE) : style_base(Default), Value(value){}

        constexpr on_hide(const GGUI::on_hide& other) : style_base(other.Status), Value(other.Value){}

        ~on_hide() override { style_base::~style_base(); }

        style_base* Copy() const override {
            return new on_hide(*this);
        }

        constexpr on_hide& operator=(const on_hide& other){
            // Only copy the information if the other is enabled.
            if (other.Status >= Status){
                Value = other.Value;

                Status = other.Status;
            }
            return *this;
        }

        void Evaluate([[maybe_unused]] styling* owner) override {};

        STAIN_TYPE Embed_Value(styling* host, element* owner) override;
    };

    class on_show : public STYLING_INTERNAL::style_base{
    public:
        void (*Value)(element* self);

        constexpr on_show(void (*value)(element* self), VALUE_STATE Default = VALUE_STATE::VALUE) : style_base(Default), Value(value){}

        constexpr on_show(const GGUI::on_show& other) : style_base(other.Status), Value(other.Value){}

        ~on_show() override { style_base::~style_base(); }

        style_base* Copy() const override {
            return new on_show(*this);
        }

        constexpr on_show& operator=(const on_show& other){
            // Only copy the information if the other is enabled.
            if (other.Status >= Status){
                Value = other.Value;

                Status = other.Status;
            }
            return *this;
        }

        void Evaluate([[maybe_unused]] styling* owner) override {};

        STAIN_TYPE Embed_Value(styling* host, element* owner) override;
    };

    class name : public STYLING_INTERNAL::style_base{
    public:
        const char* Value;

        constexpr name(const char* value, VALUE_STATE Default = VALUE_STATE::VALUE) : style_base(Default), Value(value){}

        constexpr name(const GGUI::name& other) : style_base(other.Status), Value(other.Value){}

        ~name() override { style_base::~style_base(); }

        style_base* Copy() const override {
            return new name(*this);
        }

        constexpr name& operator=(const name& other){
            // Only copy the information if the other is enabled.
            if (other.Status >= Status){
                Value = other.Value;

                Status = other.Status;
            }
            return *this;
        }

        void Evaluate([[maybe_unused]] styling* owner) override {};

        STAIN_TYPE Embed_Value(styling* host, element* owner) override;
    };

    class title : public name{
    public:
        constexpr title(const char* value, VALUE_STATE Default = VALUE_STATE::VALUE) : name(value, Default){}

        ~title() override { style_base::~style_base(); }

        style_base* Copy() const override {
            return new title(*this);
        }

        constexpr title(const GGUI::title& other) : name(other.Value, other.Status){}

        constexpr title& operator=(const title& other){
            // Only copy the information if the other is enabled.
            if (other.Status >= Status){
                Value = other.Value;

                Status = other.Status;
            }
            return *this;
        }

        STAIN_TYPE Embed_Value(styling* host, element* owner) override;
    };

    class display : public STYLING_INTERNAL::BOOL_VALUE{
    public:
        constexpr display(bool value, VALUE_STATE Default = VALUE_STATE::VALUE) : BOOL_VALUE(value, Default){}

        ~display() override { style_base::~style_base(); }

        style_base* Copy() const override {
            return new display(*this);
        }

        constexpr display(const GGUI::display& other) : BOOL_VALUE(other.Value, other.Status){}

        constexpr display& operator=(const display& other){
            // Only copy the information if the other is enabled.
            if (other.Status >= Status){
                Value = other.Value;

                Status = other.Status;
            }
            return *this;
        }

        void Evaluate([[maybe_unused]] styling* owner) override {};

        STAIN_TYPE Embed_Value(styling* host, element* owner) override;
    };

    class sprite;
    class on_draw : public STYLING_INTERNAL::style_base{
    public:
        GGUI::sprite (*Value)(unsigned int x, unsigned int y);

        constexpr on_draw(GGUI::sprite (*value)(unsigned int x, unsigned int y), VALUE_STATE Default = VALUE_STATE::VALUE) : style_base(Default), Value(value){}
        
        constexpr on_draw(const GGUI::on_draw& other) : style_base(other.Status), Value(other.Value){}

        ~on_draw() override { style_base::~style_base(); }

        style_base* Copy() const override {
            return new on_draw(*this);
        }

        constexpr on_draw& operator=(const on_draw& other){
            // Only copy the information if the other is enabled.
            if (other.Status >= Status){
                Value = other.Value;

                Status = other.Status;
            }
            return *this;
        }

        void Evaluate([[maybe_unused]] styling* owner) override {};

        STAIN_TYPE Embed_Value(styling* host, element* owner) override;
    };

    class text : public STYLING_INTERNAL::style_base{
    public:
        const char* Value;

        constexpr text(const char* value, VALUE_STATE Default = VALUE_STATE::VALUE) : style_base(Default), Value(value){}

        constexpr text(const GGUI::text& other) : style_base(other.Status), Value(other.Value){}

        ~text() override { style_base::~style_base(); }

        style_base* Copy() const override {
            return new text(*this);
        }

        constexpr text& operator=(const text& other){
            // Only copy the information if the other is enabled.
            if (other.Status >= Status){
                Value = other.Value;

                Status = other.Status;
            }
            return *this;
        }

        void Evaluate([[maybe_unused]] styling* owner) override {};

        STAIN_TYPE Embed_Value(styling* host, element* owner) override;
    };

    class on_click : public STYLING_INTERNAL::style_base{
    public:
        void (*Value)(element* self);

        constexpr on_click(void (*value)(element* self), VALUE_STATE Default = VALUE_STATE::VALUE) : style_base(Default), Value(value){}

        constexpr on_click(const GGUI::on_click& other) : style_base(other.Status), Value(other.Value){}

        ~on_click() override { style_base::~style_base(); }

        style_base* Copy() const override {
            return new on_click(*this);
        }

        constexpr on_click& operator=(const on_click& other){
            // Only copy the information if the other is enabled.
            if (other.Status >= Status){
                Value = other.Value;

                Status = other.Status;
            }
            return *this;
        }

        void Evaluate([[maybe_unused]] styling* owner) override {};

        STAIN_TYPE Embed_Value(styling* host, element* owner) override;
    };

    class styling{
    public:
        position Position                                               = position(IVector3(0, 0, 0), VALUE_STATE::INITIALIZED);

        width Width                                                     = width(1, VALUE_STATE::INITIALIZED);
        height Height                                                   = height(1, VALUE_STATE::INITIALIZED);

        enable_border                   Border_Enabled                  = enable_border(false, VALUE_STATE::INITIALIZED);
        text_color                      Text_Color                      = text_color(COLOR::WHITE, VALUE_STATE::INITIALIZED);
        background_color                Background_Color                = background_color(COLOR::BLACK, VALUE_STATE::INITIALIZED);
        border_color                    Border_Color                    = border_color(COLOR::WHITE, VALUE_STATE::INITIALIZED);
        border_background_color         Border_Background_Color         = border_background_color(COLOR::BLACK, VALUE_STATE::INITIALIZED);
        
        hover_border_color              Hover_Border_Color              = hover_border_color(COLOR::WHITE, VALUE_STATE::INITIALIZED);
        hover_text_color                Hover_Text_Color                = hover_text_color(COLOR::WHITE, VALUE_STATE::INITIALIZED);
        hover_background_color          Hover_Background_Color          = hover_background_color(COLOR::DARK_GRAY, VALUE_STATE::INITIALIZED);
        hover_border_background_color   Hover_Border_Background_Color   = hover_border_background_color(COLOR::BLACK, VALUE_STATE::INITIALIZED);

        focus_border_color              Focus_Border_Color              = focus_border_color(COLOR::WHITE, VALUE_STATE::INITIALIZED);
        focus_text_color                Focus_Text_Color                = focus_text_color(COLOR::BLACK, VALUE_STATE::INITIALIZED);
        focus_background_color          Focus_Background_Color          = focus_background_color(COLOR::WHITE, VALUE_STATE::INITIALIZED);
        focus_border_background_color   Focus_Border_Background_Color   = focus_border_background_color(COLOR::BLACK, VALUE_STATE::INITIALIZED);

        styled_border                   Border_Style;
        
        flow_priority                   Flow_Priority                   = flow_priority(DIRECTION::ROW, VALUE_STATE::INITIALIZED);
        wrap                            Wrap                            = wrap(false, VALUE_STATE::INITIALIZED);

        allow_overflow                  Allow_Overflow                  = allow_overflow(false, VALUE_STATE::INITIALIZED);
        allow_dynamic_size              Allow_Dynamic_Size              = allow_dynamic_size(false, VALUE_STATE::INITIALIZED);
        margin                          Margin;

        shadow                          Shadow;
        opacity                         Opacity                         = opacity(1.0f, VALUE_STATE::INITIALIZED);  // 100%

        allow_scrolling                 Allow_Scrolling                 = allow_scrolling(false, VALUE_STATE::INITIALIZED);

        align                           Align                           = align(ALIGN::LEFT, VALUE_STATE::INITIALIZED);

        std::vector<element*>           Childs;

        /**
         * @brief Default constructor for the Styling class.
         * 
         * This constructor initializes a new instance of the Styling class with default values.
         */
        styling() = default;

        /**
         * @brief Constructs a Styling object with the given style attributes.
         * 
         * @param attributes A pointer to a style_base object containing the style attributes.
         */
        styling(STYLING_INTERNAL::style_base* attributes){
            un_parsed_styles = attributes;
        }

        /**
         * @brief Constructs a Styling object with the given style attributes.
         * 
         * @param attributes A reference to a style_base object containing the style attributes.
         */
        styling(STYLING_INTERNAL::style_base& attributes){
            un_parsed_styles = &attributes;
        }

        /**
         * @brief Constructs a Styling object with the given style attributes.
         * 
         * @param attributes A style_base object containing the style attributes.
         */
        styling(STYLING_INTERNAL::style_base&& attributes){
            un_parsed_styles = &attributes;
        }

        /**
         * Embeds the styles of the current styling object into the element.
         * 
         * This function is used to embed the styles of the current styling object into the element.
         * It takes the element as a parameter and embeds the styles into it.
         * The styles are embedded by looping through the un_parsed_styles list and calling the Embed_Value function on each attribute.
         * The Embed_Value function is responsible for embedding the attribute into the element.
         * The changes to the element are recorded in the changes variable, which is of type STAIN.
         * The type of the changes is then added to the element's stains list.
         * The function returns nothing.
         * @param owner The element to which the styles will be embedded.
         */
        void Embed_Styles(element* owner);

        /**
         * @brief Copies the values of the given Styling object to the current object.
         *
         * This will copy all the values of the given Styling object to the current object.
         *
         * @param other The Styling object to copy from.
         */
        void Copy(const styling& other);

        /**
         * @brief Copies the styling information from another Styling object.
         * 
         * This function copies the styling information from the provided 
         * Styling object pointer. It internally calls the overloaded Copy 
         * function that takes a reference to a Styling object.
         * 
         * @param other A pointer to the Styling object from which to copy the styling information.
         */
        void Copy(const styling* other){
            // use the reference one
            Copy(*other);
        }

        void Copy_Un_Parsed_Styles();
        
        // Returns the point of interest of whom the Evaluation will reference to.
        styling* Get_Reference(element* owner);

        /**
         * @brief Evaluates all dynamic attribute values for the owner element.
         *
         * This function evaluates the dynamic attribute values of the styling associated
         * with the specified element. It determines the point of interest, which is
         * either the element's parent or the element itself if no parent exists,
         * and uses its style as a reference for evaluation.
         *
         * @param owner The element whose dynamic attributes are to be evaluated.
         * @return True if there wae changes in the attributes evaluated, false otherwise.
         */
        bool Evaluate_Dynamic_Attribute_Values(element* owner);

        bool Evaluate_Dynamic_Position(element* owner, styling* reference = nullptr);

        bool Evaluate_Dynamic_Dimensions(element* owner, styling* reference = nullptr);

        bool Evaluate_Dynamic_Border(element* owner, styling* reference = nullptr);
    
        bool Evaluate_Dynamic_Colors(element* owner, styling* reference = nullptr);
    protected:
    
        // The construction time given styles are first put here, before embedding them into this class.
        STYLING_INTERNAL::style_base* un_parsed_styles = nullptr;
    };

    namespace STYLES{
        namespace BORDER{
            const inline styled_border Double = std::array<const char*, 11>{
                "╔", "╚", "╗", "╝", "║", "═", "╠", "╣", "╦", "╩", "╬"
            };

            const inline styled_border Round = std::array<const char*, 11>{
                "╭", "╰", "╮", "╯", "│", "─", "├", "┤", "┬", "┴", "┼"
            };

            const inline styled_border Single = std::array<const char*, 11>{
                "┌", "└", "┐", "┘", "│", "─", "├", "┤", "┬", "┴", "┼"
            };

            const inline styled_border Bold = std::array<const char*, 11>{
                "▛", "▙", "▜", "▟", "█", "▅", "▉", "▉", "▉", "▉", "▉"
            };

            const inline styled_border Modern = std::array<const char*, 11>{
                "/", "\\", "\\", "/", "|", "-", "|", "|", "-", "-", "+"
            };
            
        }

        namespace CONSTANTS{
            inline styling Default;
        }

        inline enable_border border = enable_border(true);
        inline display hide = display(false);

        // CAUTION!: These anchoring vector presets, are made to work where the origin is at the center (0, 0).
        inline GGUI::STYLING_INTERNAL::Vector left = GGUI::STYLING_INTERNAL::Vector(-0.5f, 0.0f);
        // CAUTION!: These anchoring vector presets, are made to work where the origin is at the center (0, 0).
        inline GGUI::STYLING_INTERNAL::Vector top = GGUI::STYLING_INTERNAL::Vector(0.0f, -0.5f);
        // CAUTION!: These anchoring vector presets, are made to work where the origin is at the center (0, 0).
        inline GGUI::STYLING_INTERNAL::Vector right = GGUI::STYLING_INTERNAL::Vector(0.5f, 0.0f);
        // CAUTION!: These anchoring vector presets, are made to work where the origin is at the center (0, 0).
        inline GGUI::STYLING_INTERNAL::Vector bottom = GGUI::STYLING_INTERNAL::Vector(0.0f, 0.5f);
        // CAUTION!: These anchoring vector presets, are made to work where the origin is at the center (0, 0).
        inline GGUI::STYLING_INTERNAL::Vector center = GGUI::STYLING_INTERNAL::Vector(0.0f, 0.0f);
        // CAUTION!: These anchoring vector presets, are made to work where the origin is at the center (0, 0).
        inline GGUI::STYLING_INTERNAL::Vector prioritize = GGUI::STYLING_INTERNAL::Vector(0.0f, 0.0f, POSITION::Max_Z);
    };

}

#endif
#ifndef _ELEMENT_H_
#define _ELEMENT_H_

#include <string>
#include <sstream>
#include <cstring>
#include <vector>
#include <unordered_map>
#include <functional>
#include <chrono>
#include <atomic>
#include <limits>

#include <iostream>








namespace GGUI{
    class element{
    protected:
        // Shadows and some other postprocessing effects can have an impact on the final width and height of the element.
        unsigned int Post_Process_Width = 0;
        unsigned int Post_Process_Height = 0;

        // Only fetch one parent UP, and own position +, then child repeat in Render pipeline.
        IVector3 Absolute_Position_Cache;

        class element* Parent = nullptr;

        // Determines if the element is rendered or not.
        bool Show = true;
        
        std::vector<UTF> Render_Buffer;
        std::vector<UTF> Post_Process_Buffer;

        // State machine for render pipeline only focus on changed aspects.
        STAIN Dirty;
        
        // For styling to have shared styles.
        std::vector<int> Classes;

        bool Focused = false;
        bool Hovered = false;

        // Human readable ID.
        std::string Name = "";

        // For long term support made this a pointer to avoid size mismatch.
        styling* Style = nullptr;

        void (*On_Init)(element*) = nullptr;
        void (*On_Destroy)(element*) = nullptr;
        void (*On_Hide)(element*) = nullptr;
        void (*On_Show)(element*) = nullptr;
    public:

        /**
         * @brief The constructor for the Element class that accepts a Styling object.
         *
         * This constructor is used when an Element is created without a parent.
         * In this case, the Element is created as a root object, and it will be
         * automatically added to the list of root objects.
         *
         * @param s The Styling object to use for the Element.
         * @param Embed_Styles_On_Construct A flag indicating whether to embed the styles on construction. Only use if you know what you're doing!!!
         */
        element(styling s = STYLES::CONSTANTS::Default, bool Embed_Styles_On_Construct = false);

        /**
         * @brief For correctly copying data between elements, try the Copy() function.
         * Copying is removed, so that Slicing doesn't happen for the VTable
         */
        element(const element&) = delete;
        element& operator=(const GGUI::element&) = delete;

        element& operator=(element&&) = default;
        element(element&&) = default;

        /**
         * @brief The destructor for the Element class.
         *
         * This destructor is responsible for properly deallocating all the memory
         * allocated by the Element object.
         *
         * @note This destructor is also responsible for cleaning up the parent
         * element's vector of child elements and the event handlers list.
         */
        virtual ~element();

        /**
         * @brief Safely moves the current Element object to a new memory location.
         * 
         * This function creates a new Element object, copies the contents of the 
         * current Element object to the new object, and returns a pointer to the 
         * newly created object.
         * 
         * @return Element* Pointer to the newly created Element object.
         */
        virtual element* safeMove(){
            return new element();
        }

        /**
         * @brief Creates a deep copy of this Element, including all its children.
         * 
         * @return A new Element object that is a copy of this one.
         */
        element* copy();

        /**
         * @brief Embeds styles into the current element and its child elements.
         * 
         * This function calls the Embed_Styles method on the current element's style,
         * passing the current element as a parameter. It then recursively calls the 
         * Embed_Styles method on each child element's style.
         */
        void embedStyles();

        void addStyling(styling& s){
            Style->Copy(s);
        }

        /** 
         * @brief Marks the Element as fully dirty by setting all stain types.
         * 
         * This function sets each stain type on the Dirty object, indicating
         * that the Element needs to be reprocessed for all attributes.
         */
        void fullyStain();

        /**
         * @brief Accumulates all the classes and their styles.
         * @details This method accumulates all the classes and their styles to the
         *          current element.
         */
        void parseClasses();

        /**
         * @brief Returns the Dirty object for the Element.
         * @details This function returns the Dirty object, which is a bitfield
         *          that keeps track of what needs to be reprocessed on the
         *          Element when it is asked to render.
         * @return A reference to the Dirty object.
         */
        STAIN& getDirty(){
            return Dirty;
        }


        /**
         * @brief Returns true if the element is currently focused.
         * @return A boolean indicating whether the element is focused.
         */
        bool isFocused() const{
            return Focused;
        }

        /**
         * @brief Sets the focus state of the element.
         * @details Sets the focus state of the element to the given value.
         *          If the focus state changes, the element will be dirtied and the frame will be updated.
         * @param f The new focus state.
         */
        void setFocus(bool f);

        /**
         * @brief Returns true if the element is currently hovered.
         * @return A boolean indicating whether the element is hovered.
         */
        bool isHovered() const{
            return Hovered;
        }

        /**
         * @brief Sets the hover state of the element.
         * @details Sets the hover state of the element to the given value.
         *          If the hover state changes, the element will be dirtied and the frame will be updated.
         * @param h The new hover state.
         */
        void setHoverState(bool h);

        /**
         * @brief Executes the handler function associated with a given state.
         * @details This function checks if there is a registered handler for the specified state.
         *          If a handler exists, it invokes the handler function.
         * @param s The state for which the handler should be executed.
         */
        inline void check(STATE s){
            if (s == STATE::INIT && On_Init){
                // Since the rendering hasn't yet started and the function here may be reliant on some relative information, we need to evaluate the the dynamic values.
                Style->Evaluate_Dynamic_Attribute_Values(this);

                On_Init(this);
            }
            else if (s == STATE::DESTROYED && On_Destroy)
                On_Destroy(this);
            else if (s == STATE::HIDDEN && On_Hide)
                On_Hide(this);
            else if (s == STATE::SHOWN && On_Show)
                On_Show(this);
        }

        /**
         * @brief Retrieves the styling information of the element.
         * @details This function returns the current styling object associated with the element.
         *          The styling object contains various style attributes such as colors, borders, etc.
         * @return The styling object of the element.
         */
        styling getStyle() const;

        styling* getDirectStyle();

        /**
         * @brief Sets the styling information of the element.
         * @details This function sets the styling information of the element to the given value.
         *          If the element already has a styling object associated with it, the function will
         *          copy the given styling information to the existing object. Otherwise, the function
         *          will create a new styling object and associate it with the element.
         * @param css The new styling information to associate with the element.
         */
        void setStyle(styling css);

        /**
         * @brief Calculates the hitboxes of all child elements of the element.
         * @details This function calculates the hitboxes of all child elements of the element.
         *          The hitbox of a child element is the area of the element that is actually visible
         *          on the screen. The function takes the starting offset into the child array as an
         *          argument. If no argument is provided, the function starts at the beginning of the
         *          child array.
         * @param Starting_Offset The starting offset into the child array. If no argument is provided,
         *                         the function starts at the beginning of the child array.
         */
        virtual void calculateChildsHitboxes([[maybe_unused]] unsigned int Starting_Offset = 0) {}

        /**
         * @brief Adds a class to the element.
         * @details This function adds the given class to the element's class list.
         *          If the class does not exist in the global class map, a new ID is assigned to the class.
         *          The element is then marked as dirty, which will trigger a re-render of the element.
         * @param class_name The name of the class to add.
         */
        void addClass(std::string class_name);

        /**
         * @brief Sets the opacity of the element.
         * @details This function takes a float value between 0.0f and 1.0f and sets the
         * opacity of the element to that value. If the value is greater than 1.0f, the
         * function will report an error and do nothing.
         * @param[in] Opacity The opacity value to set.
         */
        void setOpacity(float Opacity);
        
        /**
         * @brief Sets the opacity of the element using an integer percentage.
         * @details This function takes an unsigned integer value between 0 and 100, representing the opacity percentage,
         * and sets the element's opacity. If the value is greater than 100, it will report an error and do nothing.
         * @param[in] Opacity The opacity percentage to set.
         */
        void setOpacity(unsigned int Opacity);

        /**
         * @brief Gets the current border style of the element.
         * @details This function returns the current border style of the element.
         *          The border style is a structure that contains the strings for
         *          the top left corner, top right corner, bottom left corner, bottom
         *          right corner, horizontal line, vertical line, vertical right connector,
         *          vertical left connector, horizontal bottom connector, horizontal top
         *          connector, and cross connector.
         * @return The current border style of the element.
         */
        styled_border getBorderStyle() const {
            return Style->Border_Style;
        }

        /**
         * @brief Gets the opacity of the element.
         * @details This function returns the current opacity of the element as a float value.
         *          The opacity is a value between 0.0 and 1.0, where 0.0 is fully transparent
         *          and 1.0 is fully opaque.
         * @return The current opacity of the element.
         */
        float getOpacity() const; 

        /**
         * @brief Checks if the element is transparent.
         * @details This function determines whether the element is transparent by checking
         *          if the element's opacity is not equal to 1.0f. An opacity less than 1.0f
         *          indicates that the element is partially or fully transparent.
         * @return True if the element is transparent; otherwise, false.
         */
        bool isTransparent() const;

        /**
         * @brief Gets the processed width of the element.
         * @details This function returns the width of the element after any post-processing
         *          has been applied. If the element has not been post-processed, the
         *          original width of the element is returned.
         * @return The processed width of the element.
         */
        unsigned int getProcessedWidth();

        /**
         * @brief Gets the processed height of the element.
         * @details This function returns the height of the element after any post-processing
         *          has been applied. If the element has not been post-processed, the
         *          original height of the element is returned.
         * @return The processed height of the element.
         */
        unsigned int getProcessedHeight();

        /**
         * @brief Configures and displays the shadow for the element.
         * @details This function sets the shadow properties such as direction, color, opacity, 
         *          and length, and applies the shadow effect to the element. It adjusts the 
         *          element's position to account for the shadow and marks the element as dirty 
         *          for a visual update.
         * @param Direction The direction vector of the shadow.
         * @param Shadow_Color The color of the shadow.
         * @param Opacity The opacity of the shadow, between 0.0f (fully transparent) and 1.0f (fully opaque).
         * @param Length The length of the shadow.
         */
        void showShadow(FVector2 Direction, RGB Shadow_Color, float Opacity = 1, float Length = 0.5);

        /**
         * @brief Displays the shadow for the element.
         * @details This function sets the shadow properties such as direction, color, opacity, and length, and applies the shadow effect to the element. It adjusts the element's position to account for the shadow and marks the element as dirty for a visual update. The direction vector of the shadow is set to (0, 0) by default, which means the shadow will appear directly below the element.
         * @param Shadow_Color The color of the shadow.
         * @param Opacity The opacity of the shadow, between 0.0f (fully transparent) and 1.0f (fully opaque).
         * @param Length The length of the shadow.
         */
        void showShadow(RGB Shadow_Color, float Opacity = 1, float Length = 0.5);

        /**
         * @brief Sets the shadow properties for the element.
         * @details This function sets the shadow properties such as direction, color, opacity, and length, and applies the shadow effect to the element. It also marks the element as dirty for a visual update.
         * @param s The shadow properties to set.
         */
        void setShadow(shadow s);

        /**
         * @brief Retrieves the parent element.
         * @details This function returns the parent element of the current element.
         *          If the element has no parent, it will return nullptr.
         * @return A pointer to the parent element.
         */
        element* getParent() const{
            return Parent;
        }

        /**
         * @brief Sets the parent of this element.
         * @details This function sets the parent of this element to the given element.
         *          If the given element is nullptr, it will clear the parent of this
         *          element.
         * @param parent The parent element to set.
         */
        void setParent(element* parent);

        /**
         * @brief Checks if the element has the given class.
         * @details This function takes a class name and checks if the element has the class in its class list.
         *          If the class does not exist in the global class map, the function will return false.
         *          If the class exists, the function will return true if the element has the class in its list.
         * @param s The name of the class to check.
         * @return True if the element has the class, false otherwise.
         */
        bool has(std::string s) const;

        /**
         * @brief Checks if the element has the given class ID.
         * @details This function takes a class ID and checks if the element has the class in its class list.
         *          If the class does not exist in the global class map, the function will return false.
         *          If the class exists, the function will return true if the element has the class in its list.
         * @param s The ID of the class to check.
         * @return True if the element has the class, false otherwise.
         */
        bool has(int s) const{
            // Iterate through the class list of the element
            for (auto i : Classes){
                // If the class ID matches the given ID, return true
                if (i == s)
                    return true;
            }
            // If no match is found, return false
            return false;
        }

        /**
         * @brief Get the fitting dimensions for the given child element.
         *
         * This function takes a child element and calculates the fitting dimensions for it.
         * The fitting dimensions are the width and height of the child element that does not exceed the
         * bounds of the parent element. If the child element is colliding with another child element
         * then the fitting dimensions are reduced to the point where the collision is resolved.
         *
         * @param child The child element for which the fitting dimensions are calculated.
         * @return A pair containing the width and height of the fitting dimensions.
         */
        std::pair<unsigned int, unsigned int> getFittingDimensions(element* child);

        /**
         * @brief Returns the maximum dimensions of the element without exceeding the parent element's dimensions.
         * @return A pair containing the maximum width and height of the element.
         */
        std::pair<unsigned int, unsigned int> getLimitDimensions();

        /**
         * @brief Sets the border visibility of the element.
         * @details This function takes a boolean as a parameter and sets the border visibility of the element accordingly.
         *          If the new state is different from the current state, the element will be marked as dirty with the EDGE stain.
         * @param b The new state of the border visibility.
         */
        virtual void showBorder(bool b);

        /**
         * @brief Sets the border visibility of the element.
         * @details This function toggles the border visibility based on the provided state.
         *          If the state has changed, it updates the border visibility, marks the 
         *          element as dirty with the EDGE stain, and updates the frame.
         * @param b The desired state of the border visibility.
         * @param Previous_State The current state of the border visibility.
         */
        virtual void showBorder(bool b, bool Previous_state);

        /**
         * @brief Checks if the element has a border.
         * @details This function checks if the element has a border.
         *          It returns true if the element has a border, false otherwise.
         * @return True if the element has a border, false otherwise.
         */
        bool hasBorder();

        /**
         * @brief Displays or hides the element and all its children.
         * @details This function changes the display status of the element and all its children.
         *          If the element is displayed, all its children are also displayed. If the element is hidden,
         *          all its children are also hidden.
         * @param f A boolean indicating whether to display (true) or hide (false) the element and its children.
         */
        void display(bool f);

        /**
         * @brief Returns whether the element is currently displayed.
         * @details This function is used to check whether the element is currently displayed or hidden.
         *          It returns true if the element is displayed and false if the element is hidden.
         * @return A boolean indicating whether the element is displayed (true) or hidden (false).
         */
        bool isDisplayed();

        /**
         * @brief Adds a child element to the element.
         * @details This function adds a child element to the element. If the element has a border, it will
         *          be taken into account when calculating the size of the parent element. If the child element
         *          exceeds the size of the parent element, the parent element will be resized to fit the child
         *          element. If the parent element is not allowed to resize, the child element will be resized to
         *          fit the parent element.
         * @param Child The child element to add.
         */
        virtual void addChild(element* Child);

        /**
         * @brief Adds a vector of child elements to the current element.
         * @param childs The vector of child elements to add.
         *
         * This function adds all the child elements to the current element by calling the Add_Child function for each element in the vector.
         * It also marks the current element as dirty with the DEEP stain after adding all the elements.
         */
        virtual void setChilds(std::vector<element*> childs);

        /**
         * @brief Check if any children have changed.
         * @details This function will check if any of the children have changed, this is used to determine if the element needs to be re-drawn.
         * @return true if any children have changed, false otherwise.
         */
        bool childrenChanged();
        
        /**
         * @brief Check if there are any transparent children.
         * @details This function determines if the current element or any of its children
         *          are transparent and require redrawing.
         * @return True if any child is transparent and not clean; otherwise, false.
         */
        bool hasTransparentChildren();    

        /**
         * @brief Retrieves the list of child elements.
         * @details This function returns a reference to the vector containing all child elements
         *          associated with the current element's style.
         * @return A reference to the vector of child elements.
         */
        virtual std::vector<element*>& getChilds();

        /**
         * @brief Removes a child element from the current element.
         * @param handle The pointer to the child element to be removed.
         * @return true if the element was successfully removed, false if not.
         *
         * This function iterates through the vector of child elements and checks
         * if the element at the current index is equal to the handle passed as an argument.
         * If it is, the element is deleted and the parent element is marked as dirty with the DEEP and COLOR stains.
         * If the currently focused element is the one being removed, the mouse position is set to the parent element's position.
         */
        virtual bool remove(element* handle);

        /**
         * @brief Removes the element at a given index from the list of child elements.
         * @details This function checks if the index is valid (i.e. if the index is within the bounds of the vector of child elements).
         *          If the index is valid, it removes the element at the specified index from the vector of child elements and deletes the element.
         *          If the index is invalid, the function returns false.
         * @param index The index of the element to remove.
         * @return True if the element was successfully removed, false otherwise.
         */
        virtual bool remove(unsigned int index);

        /**
         * @brief Set the width and height of the element.
         * @details This function sets the width and height of the element to the specified values.
         *          If the width or height is different from the current width or height, then the element will be resized and the STRETCH stain is set.
         *          The Update_Frame() function is also called to update the frame.
         * @param width The new width of the element.
         * @param height The new height of the element.
         */
        void setDimensions(unsigned int width, unsigned int height);

        /**
         * @brief Get the width of the element.
         * @details This function returns the width of the element.
         * @return The width of the element.
         */
        constexpr unsigned int getWidth(){ return Style->Width.Get(); }

        /**
         * @brief Get the height of the element.
         * @details This function returns the height of the element.
         * @return The height of the element.
         */
        constexpr unsigned int getHeight() { return Style->Height.Get(); }

        /**
         * @brief Set the width of the element.
         * @details This function sets the width of the element to the specified value.
         *          If the width is different from the current width, then the element will be resized and the STRETCH stain is set.
         *          The Update_Frame() function is also called to update the frame.
         * @param width The new width of the element.
         */
        void setWidth(unsigned int width);

        /**
         * @brief Set the height of the element.
         * @details This function sets the height of the element to the specified value.
         *          If the height is different from the current height, then the element will be resized and the STRETCH stain is set.
         *          The Update_Frame() function is also called to update the frame.
         * @param height The new height of the element.
         */
        void setHeight(unsigned int height);

        /**
         * @brief Retrieves the evaluation type of the width property.
         * 
         * This function returns the evaluation type of the width property
         * from the style's width value.
         * 
         * @return EVALUATION_TYPE The evaluation type of the width property.
         */
        EVALUATION_TYPE getWidthType() { return Style->Width.Value.Get_Type(); }

        /**
         * @brief Retrieves the evaluation type of the height value.
         * 
         * This function returns the evaluation type of the height value from the style's height property.
         * 
         * @return EVALUATION_TYPE The evaluation type of the height value.
         */
        EVALUATION_TYPE getHeightType() { return Style->Height.Value.Get_Type(); }

        /**
         * @brief Set the position of the element.
         * @details This function sets the position of the element to the specified coordinates.
         *          If the position changes, the element will be marked as dirty for movement
         *          and the frame will be updated.
         * @param c The new position of the element.
         */
        void setPosition(IVector3 c);
       
        /**
         * @brief Set the position of the element.
         * @details This function sets the position of the element to the specified coordinates.
         *          If the position changes, the element will be marked as dirty for movement
         *          and the frame will be updated.
         * @param c The new position of the element.
         */
        void setPosition(IVector3* c);

        /**
         * @brief Get the position of the element.
         * @details This function retrieves the position of the element from its style.
         * @return The position of the element as an IVector3 object.
         */
        constexpr IVector3 getPosition() { return Style->Position.Get(); }

        /**
         * @brief Get the absolute position of the element.
         * @details This function returns the cached absolute position of the element.
         *          The absolute position is the position of the element in the context of the entire document or window.
         * @return The absolute position of the element as an IVector3 object.
         */
        constexpr IVector3 getAbsolutePosition() { return Absolute_Position_Cache; }

        /**
         * @brief Update the absolute position cache of the element.
         * @details This function updates the cached absolute position of the element by adding the position of the element to the position of its parent.
         */
        void updateAbsolutePositionCache();

        /**
         * @brief Set the margin of the element.
         * @details This function sets the margin of the element to the specified margin values.
         *          The margin is stored in the element's style.
         * @param margin The new margin values for the element.
         */
        void setMargin(margin margin);

        /**
         * @brief Get the margin of the element.
         * @details This function retrieves the margin of the element from its style.
         * @return The margin of the element as a GGUI::margin object.
         */
        margin getMargin() { return Style->Margin; }
        
        /**
         * @brief Sets the background color of the element.
         * 
         * This function sets the background color of the element to the specified RGB value. 
         * If the border background color is the same as the current background color, 
         * it updates the border background color as well. Marks the element as dirty for 
         * color updates and triggers a frame update.
         * 
         * @param color The RGB color to set as the background color.
         */
        virtual void setBackgroundColor(RGB color);

        /**
         * @brief Retrieves the background color of the element.
         * 
         * This function returns the RGB value of the background color 
         * from the element's style.
         * 
         * @return The RGB color of the element's background.
         */
        constexpr RGB getBackgroundColor() { return Style->Background_Color.Value.Get<RGB>(); }
        
        /**
         * @brief Sets the border color of the element.
         * 
         * This function sets the border color of the element to the specified RGB value. Marks the element as dirty for color updates and triggers a frame update.
         * 
         * @param color The RGB color to set as the border color.
         */
        virtual void setBorderColor(RGB color);
        
        /**
         * @brief Retrieves the border color of the element.
         * 
         * This function returns the RGB value of the border color 
         * from the element's style.
         * 
         * @return The RGB color of the element's border.
         */
        constexpr RGB getBorderColor(){ return Style->Border_Color.Value.Get<RGB>(); }

        /**
         * @brief Sets the border background color of the element.
         * 
         * This function sets the border background color of the element to the specified RGB value.
         * It marks the element as dirty for color updates and triggers a frame update.
         * 
         * @param color The RGB color to set as the border background color.
         */
        virtual void setBorderBackgroundColor(RGB color);
        
        /**
         * @brief Retrieves the border background color of the element.
         * 
         * This function returns the RGB value of the border background color
         * from the element's style.
         * 
         * @return The RGB color of the element's border background.
         */
        constexpr RGB getBorderBackgroundColor(){ return Style->Border_Background_Color.Value.Get<RGB>(); }
        
        /**
         * @brief Sets the text color of the element.
         * 
         * This function sets the text color of the element to the specified RGB value. 
         * It marks the element as dirty for color updates and triggers a frame update.
         * 
         * @param color The RGB color to set as the text color.
         */
        virtual void setTextColor(RGB color);

        /**
         * @brief Retrieves the text color of the element.
         * 
         * This function returns the RGB value of the text color
         * from the element's style.
         * 
         * @return The RGB color of the element's text.
         */
        constexpr RGB getTextColor(){ return Style->Text_Color.Value.Get<RGB>(); }

        /**
         * @brief Sets the hover border color of the element.
         * 
         * This function sets the border color of the element when the mouse hovers over it
         * to the specified RGB value. Marks the element as dirty for color updates and
         * triggers a frame update.
         * 
         * @param color The RGB color to set as the hover border color.
         */
        void setHoverBorderColor(RGB color);

        /**
         * @brief Retrieves the hover border color of the element.
         * 
         * This function returns the RGB value of the border color when the mouse hovers over the element
         * from the element's style.
         * 
         * @return The RGB color of the element's hover border.
         */
        constexpr RGB getHoverBorderColor(){ return Style->Hover_Border_Color.Value.Get<RGB>(); }

        /**
         * @brief Sets the hover background color of the element.
         * 
         * This function sets the background color of the element when the mouse hovers over it
         * to the specified RGB value. Marks the element as dirty for color updates and triggers
         * a frame update.
         * 
         * @param color The RGB color to set as the hover background color.
         */
        void setHoverBackgroundColor(RGB color);

        /**
         * @brief Retrieves the hover background color of the element.
         * 
         * This function returns the RGB value of the background color when the mouse hovers over the element
         * from the element's style.
         * 
         * @return The RGB color of the element's hover background.
         */
        constexpr RGB getHoverBackgroundColor(){ return Style->Hover_Background_Color.Value.Get<RGB>(); }

        /**
         * @brief Sets the hover text color of the element.
         * 
         * This function sets the text color of the element when the mouse hovers over it
         * to the specified RGB value. Marks the element as dirty for color updates and triggers
         * a frame update.
         * 
         * @param color The RGB color to set as the hover text color.
         */
        void setHoverTextColor(RGB color);

        /**
         * @brief Retrieves the hover text color of the element.
         * 
         * This function returns the RGB value of the text color when the mouse hovers over the element
         * from the element's style.
         * 
         * @return The RGB color of the element's hover text.
         */
        constexpr RGB getHoverTextColor(){ return Style->Hover_Text_Color.Value.Get<RGB>(); }

        /**
         * @brief Sets the hover border background color of the element.
         * 
         * This function sets the background color of the element's border 
         * when the mouse hovers over it to the specified RGB value. It marks 
         * the element as dirty for color updates and triggers a frame update.
         * 
         * @param color The RGB color to set as the hover border background color.
         */
        void setHoverBorderBackgroundColor(RGB color);

        /**
         * @brief Retrieves the hover border background color of the element.
         * 
         * This function returns the RGB value of the background color of the element's border
         * when the mouse hovers over it from the element's style.
         * 
         * @return The RGB color of the element's hover border background.
         */
        constexpr RGB getHoverBorderBackgroundColor(){ return Style->Hover_Border_Background_Color.Value.Get<RGB>(); }

        /**
         * @brief Sets the focus border color of the element.
         * 
         * This function sets the color of the element's border when it is focused to the specified RGB value. It marks the element as dirty for color updates and triggers a frame update.
         * 
         * @param color The RGB color to set as the focus border color.
         */
        void setFocusBorderColor(RGB color);

        /**
         * @brief Retrieves the focus border color of the element.
         * 
         * This function returns the RGB value of the border color when the element is focused
         * from the element's style.
         * 
         * @return The RGB color of the element's focus border.
         */
        constexpr RGB getFocusBorderColor(){ return Style->Focus_Border_Color.Value.Get<RGB>(); }

        /**
         * @brief Sets the focus background color of the element.
         * 
         * This function sets the background color of the element when it is focused to the specified RGB value. It marks the element as dirty for color updates and triggers a frame update.
         * 
         * @param color The RGB color to set as the focus background color.
         */
        void setFocusBackgroundColor(RGB color);

        /**
         * @brief Retrieves the focus background color of the element.
         * 
         * This function returns the RGB value of the background color when the element is focused
         * from the element's style.
         * 
         * @return The RGB color of the element's focus background.
         */
        constexpr RGB getFocusBackgroundColor(){ return Style->Focus_Background_Color.Value.Get<RGB>(); }

        /**
         * @brief Sets the focus text color of the element.
         * 
         * This function sets the text color of the element when it is focused to the specified RGB value. It marks the element as dirty for color updates and triggers a frame update.
         * 
         * @param color The RGB color to set as the focus text color.
         */
        void setFocusTextColor(RGB color);

        /**
         * @brief Retrieves the focus text color of the element.
         * 
         * This function returns the RGB value of the text color when the element is focused
         * from the element's style.
         * 
         * @return The RGB color of the element's focus text.
         */
        constexpr RGB getFocusTextColor(){ return Style->Focus_Text_Color.Value.Get<RGB>(); }

        /**
         * @brief Sets the focus border background color of the element.
         * 
         * This function sets the focus border background color of the element to the specified RGB value.
         * It marks the element as dirty for color updates and triggers a frame update.
         * 
         * @param color The RGB color to set as the focus border background color.
         */
        void setFocusBorderBackgroundColor(RGB color);

        /**
         * @brief Retrieves the focus border background color of the element.
         * 
         * This function returns the RGB value of the focus border background color
         * from the element's style.
         * 
         * @return The RGB color of the element's focus border background.
         */
        constexpr RGB getFocusBorderBackgroundColor(){ return Style->Focus_Border_Background_Color.Value.Get<RGB>(); }

        /**
         * @brief Sets the alignment of the element.
         * 
         * This function sets the alignment of the element to the specified ALIGN value.
         * 
         * @param Align The alignment value to set for the element.
         */
        void setAlign(ALIGN a);

        /**
         * @brief Sets the alignment of the element.
         * 
         * This function sets the alignment of the element to the specified ALIGN value.
         * 
         * @param Align The alignment value to set for the element.
         */
        constexpr ALIGN getAlign(){ return Style->Align.Value; }

        /**
         * @brief Sets the flow priority of the element.
         * 
         * This function sets the flow priority of the element to the specified DIRECTION value.
         * The flow priority determines how the element will be aligned in its parent when the parent is a flow layout.
         * 
         * @param Priority The flow priority value to set for the element.
         */
        void setFlowPriority(DIRECTION d);

        /**
         * @brief Retrieves the flow priority of the element.
         * 
         * This function returns the DIRECTION value that was previously set with Set_Flow_Priority.
         * The flow priority determines how the element will be aligned in its parent when the parent is a flow layout.
         * 
         * @return The flow priority value of the element.
         */
        constexpr DIRECTION getFlowPriority(){ return Style->Flow_Priority.Value; }

        /**
         * @brief Sets whether the element will wrap its contents to the next line when it hits the edge of the screen.
         * 
         * This function sets whether the element will wrap its contents to the next line when it hits the edge of the screen.
         * If true, the element will wrap its contents to the next line when it hits the edge of the screen.
         * If false, the element will not wrap its contents to the next line when it hits the edge of the screen.
         * 
         * @param Wrap The value to set for whether the element will wrap its contents to the next line.
         */
        void setWrap(bool w);

        /**
         * @brief Retrieves the wrap setting of the element.
         * 
         * This function returns whether the element will wrap its contents to the next line
         * when it reaches the edge of the screen.
         * 
         * @return True if the element will wrap its contents, false otherwise.
         */
        constexpr bool getWrap(){ return Style->Wrap.Value; }

        /**
         * @brief Sets whether the element is allowed to dynamically resize.
         * 
         * This function enables or disables the ability of the element to 
         * adjust its size based on its content.
         * 
         * @param True A boolean indicating whether dynamic resizing is allowed.
         */
        void allowDynamicSize(bool True);

        /**
         * @brief Checks whether the element is allowed to dynamically resize.
         * 
         * This function checks the Allow_Dynamic_Size property in the element's style
         * and returns its value.
         * 
         * @return True if the element is allowed to dynamically resize, false otherwise.
         */
        constexpr bool isDynamicSizeAllowed(){ return Style->Allow_Dynamic_Size.Value; }

        /**
         * @brief Sets whether the element allows overflow.
         * 
         * This function enables or disables the overflow property of the element,
         * allowing child elements to exceed the parent's dimensions without resizing it.
         * 
         * @param True A boolean indicating whether overflow is allowed.
         */ 
        void allowOverflow(bool True);

        /**
         * @brief Checks whether the element allows overflow.
         * 
         * This function checks the Allow_Overflow property in the element's style
         * and returns its value.
         * 
         * @return True if the element allows overflow, false otherwise.
         */
        constexpr bool isOverflowAllowed(){ return Style->Allow_Overflow.Value; }
        
        /**
         * @brief Gets the fitting area for a child element in its parent.
         * @details This function calculates the area where the child element should be rendered within the parent element.
         *          It takes into account the border offsets of both the parent and the child element as well as their positions.
         *          The function returns a pair of pairs, where the first pair contains the negative offset of the child element from the parent element,
         *          the second pair contains the starting offset of the child element within the parent element and the third pair contains the ending offset of the child element within the parent element.
         * @param Parent The parent element.
         * @param Child The child element.
         * @return A pair of pairs containing the fitting area for the child element within the parent element.
         */
        static std::pair<std::pair<unsigned int, unsigned int> ,std::pair<std::pair<unsigned int, unsigned int>, std::pair<unsigned int, unsigned int>>> getFittingArea(GGUI::element* Parent, GGUI::element* Child);
                
        /**
         * @brief Recursively computes the size of the element based on its children.
         * 
         * This function will go through all the elements that are being displayed and
         * compute their size based on the size of their children. If the element has
         * children and the children have changed, then the element will be resized
         * to fit the children. If the element does not have children, then the function
         * will not do anything.
         * 
         * @note This function is called automatically by the framework when the
         *       elements are being rendered. It is not necessary to call this function
         *       manually.
         */
        void computeDynamicSize();

        /**
         * @brief Renders the element and its children into the Render_Buffer nested buffer of the window.
         * @details This function processes the element to generate a vector of UTF objects representing the current state.
         * It handles different stains such as CLASS, STRETCH, COLOR, and EDGE to ensure the element is rendered correctly.
         * @return A vector of UTF objects representing the rendered element and its children.
         */
        virtual std::vector<GGUI::UTF>& render();

        /**
         * @brief Updates the parent element of the current element.
         * @details This function is called when the current element is added, removed, or moved
         *          to a different parent element. It marks the parent element as dirty and
         *          requests a render update.
         * @param New_Element The new parent element.
         *
         * @note If the parent element does not have a valid render buffer (i.e., its
         *       `Is_Displayed()` function returns false), this function marks the parent
         *       element as dirty with the `STAIN_TYPE::DEEP` and `STAIN_TYPE::COLOR` stains.
         *       This ensures that the parent element is re-rendered from scratch when the
         *       rendering thread is updated.
         */
        virtual void updateParent(element* New_Element);

        /**
         * @brief Add the border of the window to the rendered string.
         *
         * @param w The window to add the border for.
         * @param Result The string to add the border to.
         */
        virtual void addOverhead(element* w, std::vector<UTF>& Result);

        /**
         * @brief Apply the color system to the rendered string.
         *
         * This function applies the color system set by the style to the rendered string.
         * It is called after the element has been rendered and the result is stored in the
         * Result vector.
         *
         * @param w The window to apply the color system to.
         * @param Result The vector containing the rendered string.
         */
        void applyColors(element* w, std::vector<UTF>& Result);

        /**
         * @brief Resizes the element to fit the size of its parent element.
         * @details This function is called when the parent element is resized and the
         *          current element is a child of the parent element. It resizes the
         *          current element to fit the size of its parent element. If the parent
         *          element does not have a valid render buffer (i.e., its
         *          `Is_Displayed()` function returns false), this function does
         *          nothing.
         * @param parent The parent element to resize to.
         * @return true if the resize was successful, false otherwise.
         */
        virtual bool resizeTo([[maybe_unused]] element* parent){
            return false;
        }

        /**
         * @brief Compute the alpha blending of the source element to the destination element.
         * @details This function takes two UTF elements as arguments, the source element and the destination element.
         *          It calculates the alpha blending of the source element to the destination element, by adding the
         *          background color of the source element to the destination element, but only if the source element has
         *          a non-zero alpha value. If the source element has full opacity, then the destination gets fully rewritten
         *          over. If the source element has full transparency, then nothing is done.
         * @param Dest The destination element to which the source element will be blended.
         * @param Source The source element which will be blended to the destination element.
         */
        void computeAlphaToNesting(GGUI::UTF& Dest, GGUI::UTF Source);

        /**
         * @brief Nests a child element into a parent element.
         * @details This function calculates the area where the child element should be rendered within the parent element.
         *          It takes into account the border offsets of both the parent and the child element as well as their positions.
         *          The function then copies the contents of the child element's buffer into the parent element's buffer at the calculated position.
         * @param Parent The parent element.
         * @param Child The child element.
         * @param Parent_Buffer The parent element's buffer.
         * @param Child_Buffer The child element's buffer.
         */
        void nestElement(element* Parent, element* Child, std::vector<UTF>& Parent_Buffer, std::vector<UTF>& Child_Buffer);

        /**
         * @brief Returns a map of the custom border symbols for the given element.
         * @param e The element to get the custom border map for.
         * @return A map of the custom border symbols where the key is the bit mask of the border and the value is the corresponding symbol.
         */
        std::unordered_map<unsigned int, const char*> getCustomBorderMap(element* e);

        /**
         * @brief Returns a map of the custom border symbols for the given border style.
         * The map key is the bit mask of the border and the value is the corresponding symbol.
         * @param custom_border_style The custom border style to get the map for.
         * @return A map of the custom border symbols.
         */
        std::unordered_map<unsigned int, const char*> getCustomBorderMap(GGUI::styled_border custom_border_style);

        /**
         * @brief Sets the custom border style for the element.
         * @details This function sets the custom border style for the element, marks the element's edges as dirty, and ensures that the border is visible.
         * @param style The custom border style to set.
         */
        void setCustomBorderStyle(GGUI::styled_border style);

        /**
         * @brief Gets the custom border style of the element.
         * @return The custom border style of the element.
         */
        GGUI::styled_border getCustomBorderStyle(){ return Style->Border_Style; }

        /**
         * @brief Posts a process that handles the intersection of borders between two elements and their parent.
         * @details This function posts a process that handles the intersection of borders between two elements and their parent.
         *          The process calculates the intersection points of the borders and then constructs a bit mask that portraits the connections the middle point has.
         *          With the calculated bit mask it can fetch from the 'SYMBOLS::Border_Identifiers' the right border string.
         * @param A The first element.
         * @param B The second element.
         * @param Parent_Buffer The buffer of the parent element.
         */
        void postProcessBorders(element* A, element* B, std::vector<UTF>& Parent_Buffer);

        /**
         * @brief Composes the RGB values of the text color and background color of the element.
         * 
         * This function will return a pair of RGB values, where the first element is the
         * color of the text and the second element is the color of the background.
         * 
         * If the element is focused, the function will return the RGB values of the focused
         * text color and background color. If the element is hovered, the function will
         * return the RGB values of the hovered text color and background color. Otherwise,
         * the function will return the RGB values of the normal text color and background
         * color.
         * 
         * @return A pair of RGB values representing the text color and background color of the element.
         */
        std::pair<RGB, RGB>  composeAllTextRGBValues();

        /**
         * @brief Composes the RGB values of the text color of the element.
         * 
         * This function will return the RGB values of the text color of the element.
         * If the element is focused, the function will return the RGB values of the focused
         * text color. If the element is hovered, the function will return the RGB values of the hovered
         * text color. Otherwise, the function will return the RGB values of the normal text color.
         * 
         * @return The RGB color of the element's text.
         */
        RGB  composeTextRGBValues();
        
        /**
         * @brief Composes the RGB values of the background color of the element.
         * 
         * This function will return the RGB values of the background color of the element.
         * If the element is focused, the function will return the RGB values of the focused
         * background color. If the element is hovered, the function will return the RGB values of the hovered
         * background color. Otherwise, the function will return the RGB values of the normal background color.
         * 
         * @return The RGB color of the element's background.
         */
        RGB  composeBackgroundRGBValues();

        /**
         * @brief Composes the RGB values of the border color and background color of the element.
         * @details This function will return the RGB values of the border color and background color of the element.
         * If the element is focused, the function will return the RGB values of the focused border color and background color.
         * If the element is hovered, the function will return the RGB values of the hovered border color and background color.
         * Otherwise, the function will return the RGB values of the normal border color and background color.
         * @return A pair of RGB values representing the border color and background color of the element.
         */
        std::pair<RGB, RGB>  composeAllBorderRGBValues();

        /**
         * @brief Returns the name of the element.
         * @details This function returns a string that represents the name of the element.
         *          The name is constructed by concatenating the name of the element with the 
         *          class name of the element, separated by a "<" and a ">".
         * @return The name of the element.
         */
        virtual std::string getName() const {
            return "Element<" + Name + ">";
        }

        /**
         * @brief Set the name of the element.
         * @details This function sets the name of the element and stores it in the global Element_Names map.
         * @param name The name of the element.
         */
        void setName(std::string name);

        /**
         * @brief Removes the element from the parent element.
         * @details This function first checks if the element has a parent.
         *          If the element has a parent, it calls the parent's Remove() function to remove the element from the parent.
         *          If the element does not have a parent, it prints an error message to the console.
         *          The function does not update the frame, so it is the caller's responsibility to update the frame after calling this function.
         */
        void remove();

        /**
         * @brief A function that registers a lambda to be executed when the element is clicked.
         * @details The lambda is given a pointer to the Event object that triggered the call.
         *          The lambda is expected to return true if it was successful and false if it failed.
         * @param action The lambda to be called when the element is clicked.
         */
        void onClick(std::function<bool(GGUI::Event*)> action);

        /**
         * @brief A function that registers a lambda to be executed when the element is interacted with in any way.
         * @details The lambda is given a pointer to the Event object that triggered the call.
         *          The lambda is expected to return true if it was successful and false if it failed.
         * @param criteria The criteria to check for when deciding whether to execute the lambda.
         * @param action The lambda to be called when the element is interacted with.
         * @param GLOBAL Whether the lambda should be executed even if the element is not under the mouse.
         */
        void on(unsigned long long criteria, std::function<bool(GGUI::Event*)> action, bool GLOBAL = false);

        /**
         * @brief Retrieves an element by name.
         * @details This function takes a string argument representing the name of the element
         *          and returns a pointer to the element if it exists in the global Element_Names map.
         * @param name The name of the element to retrieve.
         * @return A pointer to the element if it exists; otherwise, nullptr.
         */
        element* getElement(std::string name);

        // TEMPLATES
        //-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_

        /**
         * @brief Retrieves all child elements that have the same type as the given template.
         * @details This function takes a template argument representing the type of the elements to retrieve.
         *          It goes through the child AST, and checks if the element in question is the same type as the template T.
         *          If the element in question is of the same type as the template T, it adds it to the result vector.
         *          The function then recursively goes through the child AST, and checks if any of the child elements are of the same type as the template T.
         *          If any of the child elements are of the same type as the template T, it adds them to the result vector.
         *          The function then returns the result vector, which contains all child elements that have the same type as the given template.
         * @param T The type of the elements to retrieve.
         * @return A vector of pointers to the elements that have the same type as the given template.
         */
        template<typename T>
        std::vector<T*> getElements(){
            std::vector<T*> result;

            // Check if the element in question is of the same type as the template T.
            if (typeid(*this) == typeid(T)){
                result.push_back((T*)this);
            }

            // Go through the child AST, and check if any of the child elements are of the same type as the template T.
            for (auto e : Style->Childs){
                // Recursively go through the child AST, and check if any of the child elements are of the same type as the template T.
                std::vector<T*> child_result = e->getElements<T>();

                // Add the results of the recursive call to the result vector.
                result.insert(result.end(), child_result.begin(), child_result.end());
            }

            // Return the result vector, which contains all child elements that have the same type as the given template.
            return result;
        }

        /**
         * @brief Retrieves all nested elements, including this element.
         * @details This function collects all nested elements recursively, starting from this element.
         *          If 'Show_Hidden' is false, hidden elements are excluded from the result.
         * @param Show_Hidden Flag to determine whether to include hidden elements in the result.
         * @return A vector of pointers to all nested elements.
         */
        std::vector<element*> getAllNestedElements(bool Show_Hidden = false) {
            std::vector<element*> result;

            // If the element is not visible and hidden elements should not be shown, return an empty vector.
            if (!Show && !Show_Hidden)
                return {};

            // Add the current element to the result vector.
            result.push_back(this);

            // Recursively retrieve all nested elements from child elements.
            for (auto e : getChilds()) {
                std::vector<element*> child_result = e->getAllNestedElements(Show_Hidden);
                result.insert(result.end(), child_result.begin(), child_result.end());
            }

            // Return the result vector containing all nested elements.
            return result;
        }

        //-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_

        /**
         * @brief Default virtual function for scrolling up.
         * @details By default, elements do not have inherent scrolling abilities.
         *          This function is used as a base for other elements to implement their own scrolling.
         */
        virtual void scrollUp() {}

        /**
         * @brief Default virtual function for scrolling down.
         * @details By default, elements do not have inherent scrolling abilities.
         *          This function is used as a base for other elements to implement their own scrolling.
         */
        virtual void scrollDown() {}

        /**
         * @brief Reorders child elements based on their z-position.
         * @details This function sorts the child elements of the current element by their z-coordinate
         *          in ascending order, so that elements with a higher z-coordinate appear later in the list.
         */
        void reOrderChilds();

        /**
         * @brief Focuses the element.
         * @details This function updates the global focus information by setting the mouse position to the element's position and updating the focused element.
         */
        void focus();

        /**
         * @brief Adds a handler function to the state handlers map.
         * @details This function takes a state and a handler function as arguments.
         *          The handler function is stored in the State_Handlers map with the given state as the key.
         * @param s The state for which the handler should be executed.
         * @param job The handler function to be executed
         */
        void onState(STATE s, void (*job)(element* self));

        /**
         * @brief Checks if the element needs postprocessing.
         * @details This function checks if the element needs postprocessing by checking if the element has a shadow or is transparent.
         * @return True if the element needs postprocessing; otherwise, false.
         */
        bool hasPostprocessingToDo();

        /**
         * @brief Process the shadow of the element.
         * @details This function processes the shadow of the element by calculating the new buffer size and creating a new buffer with the shadow.
         *          It then offsets the shadow box buffer by the direction and blends it with the original buffer.
         * @param Current_Buffer The buffer to be processed.
         */
        void processShadow(std::vector<GGUI::UTF>& Current_Buffer);

        /**
         * @brief Applies the opacity of the element to the given buffer.
         * @details This function will iterate over the given buffer and apply the opacity of the element to the background and foreground of each UTF character.
         * @param Current_Buffer The buffer to be processed.
         */
        void processOpacity(std::vector<GGUI::UTF>& Current_Buffer);

        /**
         * @brief
         * This function performs postprocessing on the rendered buffer of the element.
         * It applies the shadow, and then the opacity to the rendered buffer.
         * @return The postprocessed buffer.
         */
        virtual std::vector<GGUI::UTF>& postprocess();

        // Customization helper function
        //-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_
        
        /**
         * @brief Adds a stain to the element.
         * @details This function adds the given stain to the element's stains list.
         *          The stains list is used to keep track of which properties of the element need to be re-evaluated.
         *          The function takes a STAIN_TYPE as a parameter and adds it to the list of stains.
         * @param s The stain to be added.
         */
        void addStain(STAIN_TYPE s){
            Dirty.Dirty(s);
        }

        /**
         * @brief
         * This function determines if the given element is a direct child of this element (in the DOM tree),
         * and if it is visible on the screen (does not go out of bounds of the parent element).
         * @param other Child element to check
         * @return True if the child element is visible within the bounds of the parent.
         */
        bool childIsShown(element* other);

        inline void setOnInit(void (*func)(element* self)){
            On_Init = func;
        }

        inline void setOnDestroy(void (*func)(element* self)){
            On_Destroy = func;
        }

        inline void setOnHide(void (*func)(element* self)){
            On_Hide = func;
        }

        inline void setOnShow(void (*func)(element* self)){
            On_Show = func;
        }

        inline void forceStyleEvaluation(){
            if (Style)
                Style->Evaluate_Dynamic_Attribute_Values(this);
        }
    };
}

#endif
#ifndef _TEXT_FIELD_H_
#define _TEXT_FIELD_H_










namespace GGUI{
    class textField : public element{
    protected:
        std::string Text = "";

        // This will hold the text by lines, and does not re-allocate memory for whole text, only for indicies.
        std::vector<Compact_String> Text_Cache; 

        /**
         * @brief Updates the text cache list by newlines, and if no found then set the Text as the zeroth index.
         * @details This function will also determine the longest line length and store it in the class.
         * @note This function will also check if the lines can be appended to the previous line or not.
         */
        void updateTextCache();
    public:

        /**
         * @brief Constructor for the Text_Field class.
         * @details This constructor takes an optional Styling parameter, and an optional string parameter.
         *          If the string parameter is not given, it defaults to an empty string.
         *          If the Styling parameter is not given, it defaults to a Styling object with
         *          the default values for the Height and Width properties.
         *          The Styling parameter is used to set the style of the Text_Field object.
         * @param text The text to be displayed in the Text_Field object.
         * @param s The Styling object to use for the Text_Field object.
         * @param Embed_Styles_On_Construct If true, the styling will be embedded into the Text_Field's style. Only use if you know what you're doing!!!
         */
        textField(styling s = STYLES::CONSTANTS::Default, bool Embed_Styles_On_Construct = false) : element(s, Embed_Styles_On_Construct){

            // Since Styling Height and Width are defaulted to 1, we can use this one row to reserve for one line.
            Text_Cache.reserve(getHeight());

            if (getWidth() == 1 && getHeight() == 1){
                allowDynamicSize(true);
            }

            // Update the text cache list by newlines, and if no found then set the Text as the zeroth index.
            updateTextCache();
        }

        /**
         * @brief Sets the size of the text field to fill its parent element.
         * @details The function first checks if dynamic sizing is allowed for
         *          the text field and its parent. It then calculates the new
         *          width and height based on the parent's dimensions and the
         *          text size. If the parent allows dynamic sizing, it stretches
         *          to accommodate the text; otherwise, it constrains the size
         *          within the parent's boundaries.
         */
        void setSizeToFillParent();

        /**
         * @brief Sets the text of the text field.
         * @details This function first stops the GGUI engine, then sets the text with a space character added to the beginning, and finally updates the text field's dimensions to fit the new text. The text is then reset in the Render_Buffer nested buffer of the window.
         * @param text The new text for the text field.
         */
        void setText(std::string text);

        /**
         * @brief Gets the text of the text field.
         * @details This function returns the string containing the text of the text field.
         * @return The text of the text field as a string.
         */
        std::string getText(){
            return Text;
        }

        /**
         * @brief Renders the text field into the Render_Buffer.
         * @details This function processes the text field to generate a vector of UTF objects representing the current state.
         * It handles different stains such as CLASS, STRETCH, COLOR, EDGE, and DEEP to ensure the text field is rendered correctly.
         * @return A vector of UTF objects representing the rendered text field.
         */
        std::vector<GGUI::UTF>& render() override;

        /**
         * @brief Aligns text to the left within the text field.
         * @param Result A vector of UTF objects to store the aligned text.
         * @details This function iterates over each line in the Text_Cache and aligns them to the left side 
         *          of the text field. The function respects the maximum height and width of the text field 
         *          and handles overflow according to the Style settings.
         */
        void alignTextLeft(std::vector<UTF>& Result);
        
        /**
         * @brief Aligns text to the right within the text field.
         * @param Result A vector of UTF objects to store the aligned text.
         * @details This function iterates over each line in the Text_Cache and aligns them to the right side
         *          of the text field. The function respects the maximum height and width of the text field
         *          and handles overflow according to the Style settings.
         */
        void alignTextRight(std::vector<UTF>& Result);
        
        /**
         * @brief Aligns text to the center within the text field.
         * @param Result A vector of UTF objects to store the aligned text.
         * @details This function iterates over each line in the Text_Cache and aligns them to the center of the text field. The function respects the maximum height and width of the text field
         *          and handles overflow according to the Style settings.
         */
        void alignTextCenter(std::vector<UTF>& Result);

        /**
         * @brief Listens for input and calls a function when user presses any key.
         * @param Then A function that takes a character as input and does something with it.
         * @details This function creates three actions (for key press, enter, and backspace) that listen for input when the text field is focused. If the event is a key press or enter, it
         *          calls the Then function with the character as input. If the event is a backspace, it removes the last character from the text field. In all cases, it marks the text field as
         *          dirty and updates the frame.
         */
        void input(std::function<void(char)> Then);

        element* safeMove() override {
            return new textField();
        }
    };
}

#endif
#ifndef _GUARD_H_
#define _GUARD_H_

#include <functional>
#include <mutex>
#include <string>
#include <memory>

namespace GGUI{
    namespace INTERNAL{
        extern void reportStack(std::string Problem);

        namespace atomic{

            template<typename T>
            class Guard {
            public:
                std::mutex Shared; // Mutex to guard shared data
                std::unique_ptr<T> Data;

                /**
                 * @brief Constructs a Guard object and initializes its Data member.
                 * 
                 * This constructor creates a unique pointer to an instance of type T
                 * and assigns it to the Data member of the Guard object.
                 */
                Guard() : Data(std::make_unique<T>()) {}

                /**
                 * @brief Functor to execute a job with thread safety.
                 * 
                 * This operator() function takes a std::function that operates on a reference to a T object.
                 * It ensures that the job is executed with mutual exclusion by using a std::lock_guard to lock
                 * the mutex. If the job throws an exception, it catches it and reports the failure.
                 * 
                 * @param job A std::function that takes a reference to a T object and performs some operation.
                 * 
                 * @throws Any exception thrown by the job function will be caught and reported.
                 */
                void operator()(std::function<void(T&)> job) {
                    std::lock_guard<std::mutex> lock(Shared); // Automatically manages mutex locking and unlocking
                    try {
                        job(*Data);
                    } catch (...) {
                        reportStack("Failed to execute the function!");
                    }
                }

                /**
                 * @brief Reads the data in a thread-safe manner.
                 * 
                 * This function acquires a lock on the shared mutex to ensure that the data
                 * is read in a thread-safe manner. It returns a copy of the data.
                 * 
                 * @return T A copy of the data.
                 */
                T Read() {
                    std::lock_guard<std::mutex> lock(Shared);
                    return *Data;
                }

                /**
                 * @brief Destructor for the Guard class.
                 *
                 * This destructor ensures that the Data object is properly destroyed
                 * by acquiring a lock on the Shared mutex before resetting the Data.
                 * The use of std::lock_guard ensures that the mutex is automatically
                 * released when the destructor exits, preventing potential deadlocks.
                 */
                ~Guard() {
                    std::lock_guard<std::mutex> lock(Shared);
                    Data.reset(); // Ensures proper destruction
                }
            };   
        }
    }
}

#endif
#ifndef _FILE_STREAMER_H_
#define _FILE_STREAMER_H_








// 

#include <fstream>
#include <functional>
#include <unordered_map>
#include <stdio.h>
#include <deque>
#include <fcntl.h>

#if _WIN32
    #include <io.h>
#else
    #include <unistd.h>
#endif

namespace GGUI{

    /*
        Utilities to manage file streams.
    */

    class fileStream;

    extern std::unordered_map<std::string, fileStream*> File_Streamer_Handles;

    /**
     * @brief Adds an event handler that is called when the file is changed.
     * @param File_Name The name of the file to add the event handler for.
     * @param Handle The event handler to be called when the file is changed.
     *
     * If there is already a file handle for this file name, the event handler is
     * added to the list of event handlers for that file. If not, a new file
     * handle is created and the event handler is added to the list of event
     * handlers for that file.
     */
    extern void addFileStreamHandle(std::string File_Handle, std::function<void()> Handle);

    /**
     * @brief Returns the file stream handle associated with the given file name.
     * @param File_Name The name of the file to retrieve the handle for.
     * @return The file stream handle associated with the given file name, or nullptr if no handle exists.
     */
    extern fileStream* getFileStreamHandle(std::string File_Name);

    /**
     * @brief Returns the current working directory of the program.
     * @return The current working directory as a string.
     *
     * This function returns the current working directory of the program
     * as a string. This is useful for finding the location of configuration
     * files, data files, and other resources that need to be accessed from
     * the program.
     */
    extern std::string getCurrentLocation();
 
    /**
     * @brief Pulls the current contents of STDIN as a string.
     * @return The contents of STDIN as a string.
     *
     * This function pulls the current contents of STDIN as a string. This is
     * useful for applications that need to access the output of a previous
     * process. Note that this function will only work if the application was
     * started as a non TTY enabled application. If the application was started
     * as a TTY enabled application, this function will fail.
     */
    extern std::string pullSTDIN();
    
    /**
     * @brief Checks if the application was started from a TTY
     * @return True if the application was started from a TTY, false otherwise
     *
     * This function checks if the application was started from a TTY
     * by checking if STDIN is a TTY.
     */
    extern bool hasStartedAsTTY();

    namespace INTERNAL{
        // When ever creating a new Buffer Capture, the previous Buffer Capture will not get notified about new lines of text, after the new Buffer Capture had been constructed.
        // These black boxes work like Stack Frames, where the data collected will be deleted when the current "Frame" capturer is destructed.
        class bufferCapture : public std::streambuf{
        private:
            std::streambuf* STD_COUT_RESTORATION_HANDLE = nullptr;
            std::string Current_Line = "";
            std::deque<std::string> Console_History;

            // Multiple handlers.
            std::vector<std::function<void()>> On_Change = {};

            // For speeding up.
            std::unordered_map<bufferCapture*, bool> Synced;

            std::string Name = "";
        public:
            // We could just search it from the global listing, but that would be slow.
            // Stuck into the constructed position.
            const bool Is_Global = false;

            /**
             * @brief Construct a BUFFER_CAPTURE
             * @param on_change Function which will be called when the BUFFER_CAPTURE had changed.
             * @param Name Name of the BUFFER_CAPTURE
             * @param Global If true, this BUFFER_CAPTURE will inform all other global BUFFER_CAPTURES about the change.
             *
             * Construct a BUFFER_CAPTURE
             * This will store the previous handle of std::cout.
             * This will also insert this as the new cout output stream.
             * If Global is true, this BUFFER_CAPTURE will inform all other global BUFFER_CAPTURES about the change.
             */
            bufferCapture(std::function<void()> on_change, std::string Name = "", bool Global = false);

            /**
             * @brief Default constructor
             *
             * This constructor is explicitly defined as default, which means that the compiler will generate a default implementation for it.
             * This is needed because otherwise, the compiler would not generate a default constructor for this class, since we have a user-declared constructor.
             */
            bufferCapture() = default;

            /**
             * @brief Destructor
             *
             * Called when the BUFFER_CAPTURE is going out of scope.
             * This will call the Close() method to restore the original std::cout stream.
             */
            ~bufferCapture() {
                close();
            }
            
            /**
             * @brief Handles character overflow for BUFFER_CAPTURE.
             * @param c Character to be added to the current line.
             * @return The result of writing the character to the original std::cout buffer.
             *
             * This function is called whenever the buffer overflows, typically when a new character is inserted. 
             * It handles new lines by storing the current line in the console history and notifying all registered 
             * change handlers. If this BUFFER_CAPTURE is global, it informs all other global BUFFER_CAPTURES about 
             * the change.
             */
            int overflow(int c) override;
            
            /**
             * @brief Close the BUFFER_CAPTURE and restore the original std::cout stream.
             *
             * This function is called when the BUFFER_CAPTURE is going out of scope, typically when it is destructed.
             * It checks if the STD_COUT_RESTORATION_HANDLE is nullptr to avoid a double-close of the BUFFER_CAPTURE.
             * If not nullptr, it sets the original std::cout stream back to the previous handle.
             */
            void close();

            /**
             * @brief Reads the console history and returns it as a single string.
             *
             * This function concatenates all lines stored in the console history
             * and returns them as a single string, with each line separated by a newline character.
             * 
             * @return A string containing the entire console history.
             */
            std::string read();

            /**
             * @brief Adds a change handler function to the list.
             * 
             * This function will be called whenever a change occurs. 
             * It appends the provided change handler function to the 
             * internal list of change handlers.
             *
             * @param on_change The function to be called on change.
             */
            void addOnChangeHandler(std::function<void()> on_change){                
                // Append the change handler function to the list
                On_Change.push_back(on_change);
            }

            /**
             * @brief Synchronizes this BUFFER_CAPTURE with the provided informer.
             * 
             * This function attempts to synchronize the current BUFFER_CAPTURE with another
             * by sharing the latest console history lines. If the BUFFER_CAPTUREs have previously
             * been synchronized, only the latest line is shared. Otherwise, it checks compatibility
             * and synchronizes the entire history as needed.
             * 
             * @param Informer The BUFFER_CAPTURE containing the latest data to synchronize with.
             * @return True if synchronization was successful, false otherwise.
             */
            bool sync(bufferCapture* Informer);

            /**
             * @brief Gets the name of this BUFFER_CAPTURE.
             * 
             * If a name has not been set, it defaults to "BUFFER_CAPTURE<address>".
             * @return The name of this BUFFER_CAPTURE.
             */
            std::string getName();

            /**
             * @brief Sets the name of this BUFFER_CAPTURE.
             * @param Name The new name of this BUFFER_CAPTURE.
             *
             * This function sets the name of this BUFFER_CAPTURE.
             * If a name has not been set, it defaults to "BUFFER_CAPTURE<address>".
             * @see Get_Name()
             */
            void setName(std::string Name){
                this->Name = Name;
            }
        };

    }

    enum class FILE_STREAM_TYPE{
        UN_INITIALIZED  = 0 << 0,
        READ            = 1 << 0,
        WRITE           = 1 << 1,
        STD_CAPTURE     = 1 << 2
    };

    class fileStream{
    private:
        INTERNAL::bufferCapture* Buffer_Capture = nullptr;
        std::fstream Handle;
        std::vector<std::function<void()>> On_Change = {};
        std::string Previous_Content = "";
        unsigned long long Previous_Hash = 0;
        FILE_STREAM_TYPE Type = FILE_STREAM_TYPE::UN_INITIALIZED;
    public:
        std::string Name = "";

        /**
         * @brief Constructor of the FILE_STREAM class.
         * @param file_name The name of the file to open.
         * @param on_change The event handler to be called when the file is changed.
         * @param type describes the pipe method, READ/WRITE/STD_CAPTURE
         * @param atomic If set will not add the file stream to the amassed file stream handle service.
         *
         * If read_from_std_cout is true, a new file is created where the std::cout is piped into.
         * If there is already a file handle for this file name, the event handler is added to the list of event
         * handlers for that file. If not, a new file handle is created and the event handler is added to the list
         * of event handlers for the new file.
         */
        fileStream(std::string file_name, std::function<void()> on_change = [](){}, FILE_STREAM_TYPE type = FILE_STREAM_TYPE::READ, bool atomic = false);

        /**
         * @brief Intended for Logger Atomic::Guard, do not use as User!
         */
        fileStream() = default;

        fileStream(const fileStream&) = delete;
        fileStream& operator=(const fileStream&) = delete;

        fileStream(fileStream&&) = default;  // Allow move
        fileStream& operator=(fileStream&&) = default;

        /**
         * @brief Destructor for the FILE_STREAM class.
         *
         * This destructor ensures that resources are properly released
         * when a FILE_STREAM object is destroyed. It closes the associated
         * BUFFER_CAPTURE if it exists and also closes the file handle.
         */
        ~fileStream();

        /**
         * @brief Read the content of the file.
         * @return The content of the file as a string.
         * 
         * This function will read the content of the file and return it as a string.
         * If the file is empty, this function will return an empty string.
         * If the file does not exist, or any other error occurs, this function will return an empty string and print an error message to the console.
         * If read_from_std_cout is true, this function will read the content from the buffer capture instead of the file.
         */
        std::string Read();

        /**
         * @brief overwrites the given buffer into the file, clearing the previous content of said file.
         * @param Buffer The buffer to write into the file.
         */
        void Write(std::string Buffer);

        /**
         * @brief Append line of text to the end of the file.
         * @param Line The line of text to append to the file.
         */
        void Append(std::string Line);
    
        /**
         * @brief Read the content of the file quickly without checking if the file has changed.
         * @return The content of the file as a string.
         *
         * This function is faster than the normal Read() function, but it does not check if the file has changed.
         * If the file has changed since the last call to Read(), this function will return the old content.
         * If the file does not exist, or any other error occurs, this function will return an empty string and print an error message to the console.
         * If read_from_std_cout is true, this function will read the content from the buffer capture instead of the file.
         */
        std::string Fast_Read() { return Previous_Content; }

        /**
         * @brief Checks if the file has changed and notifies the event handlers if so.
         *
         * This function is called by the FILE_STREAM class when it wants to check if the file has changed.
         * It reads the new content from the file, calculates the hash of the new content, and compares it with the previous hash.
         * If the hash has changed, it notifies all the event handlers by calling them.
         */
        void Changed();

        /**
         * @brief Adds a new event handler to the list of event handlers for this file.
         * @param on_change The function to be called when the file changes.
         * 
         * This function adds a new event handler to the list of event handlers for this file.
         * If read_from_std_cout is true, the event handler is added to the list of event handlers of the Buffer_Capture object.
         * Otherwise, the event handler is added to the list of event handlers of this FILE_STREAM object.
         */
        void Add_On_Change_Handler(std::function<void()> on_change){
            if (Buffer_Capture)
                Buffer_Capture->addOnChangeHandler(on_change);
            else
                On_Change.push_back(on_change);
        }

        /**
         * @brief Checks if the file stream is a std::cout stream.
         * @return True if the file stream is a std::cout stream, false otherwise.
         *
         * This function checks if the file stream is a std::cout stream, i.e. if it is a stream that is
         * associated with the BUFFER_CAPTURE class. If it is, it returns true, otherwise it returns false.
         */
        bool Is_Cout_Stream(){
            return Buffer_Capture != nullptr;
        }

        FILE_STREAM_TYPE Get_type(){
            return Type;
        }
    };

    class filePosition{
    public:
        std::string File_Name = "";     // Originated.
        unsigned int Line_Number = 0;   // Y
        unsigned int Character = 0;     // X

        /**
         * @brief Constructor for the FILE_POSITION class.
         * @param File_Name The name of the file that the position is in.
         * @param Line_Number The line number of the position.
         * @param Character The character number of the position.
         * 
         * This constructor creates a new FILE_POSITION object with the given file name, line number and character number.
         */
        filePosition(std::string File_Name, unsigned int Line_Number, unsigned int Character){
            this->File_Name = File_Name;
            this->Line_Number = Line_Number;
            this->Character = Character;
        }

        /**
         * @brief Default constructor for the FILE_POSITION class.
         * @details This constructor creates a new FILE_POSITION object with default values for the file name, line number and character number.
         */
        filePosition() = default;

        /**
         * @brief Converts the FILE_POSITION object to a string.
         * @return A string that represents the FILE_POSITION object in the format "File_Name:Line_Number:Character".
         *
         * This function converts the FILE_POSITION object to a string by concatenating the file name, line number and character number with a colon in between.
         * The resulting string can be used to log or display the position of the file stream.
         */
        std::string To_String(){
            return File_Name + ":" + std::to_string(Line_Number) + ":" + std::to_string(Character);
        }
    };

    #if _WIN32
        class CMD{
        private:
            void* In;
            void* Out;
        public:
            /**
             * @brief Constructor for the CMD class
             * @details Initializes a pipe for inter-process communication.
             *          The pipe is created with security attributes that allow
             *          the handles to be inherited by child processes.
             */
            CMD();

            /**
             * @brief Destructor for the CMD class
             * @details Closes the handles created by the constructor and releases
             *          any resources associated with the pipe.
             */
            ~CMD() = default;

            /**
             * @brief Runs a command in a shell and returns its output as a string
             * @param Command The command to execute
             * @return The output of the command as a string
             *
             * This function runs a command in a shell and captures its output. It
             * then returns the output as a string.
             */
            std::string Run(std::string command);
        };
    #else
        class CMD{  // Unix implementation:
        private:
            union __INTERNAL_CMD_FILE_DESCRIPTOR__ {
                struct __INTERNAL_CMD_WAY__ {
                    int In;
                    int Out;
                } Way;
                int FDS[2];
            } File_Descriptor;
        public:

            /**
             * @brief Constructor for the CMD class
             *
             * This function creates a pipe and stores the file descriptors in the
             * File_Descriptor structure. The constructor is used to create a new
             * object of the CMD class.
             */
            CMD();
            
            /**
             * @brief Destructor for the CMD class
             * @details Closes the handles created by the constructor and releases
             *          any resources associated with the pipe.
             */
            ~CMD() = default;

            /**
             * @brief Run a command using the system shell and capture its output.
             * @param Command The command to be executed.
             * @return The output of the command as a string.
             * 
             * This function forks a new process to execute the specified command
             * using the system shell. The output of the command is redirected to
             * a pipe and captured by the parent process. The function returns the
             * captured output as a string.
             */
            std::string Run(std::string command);
        };
    #endif
}

#endif
#ifndef _CANVAS_H_
#define _CANVAS_H_










#include <vector>

namespace GGUI{

    class canvas : public element{
    private:
        // DONT GIVE THIS TO USER!
        canvas(){}
    protected:
        std::vector<RGB> Buffer;
    public:
        /**
         * @brief Constructor for the Canvas class.
         * @details This constructor initializes a Canvas object with the specified styling.
         * @param s The styling to be applied to the Canvas.
         * @param Embed_Styles_On_Construct If true, the styling will be embedded into the canvas's style. Only use if you know what you're doing!!!
         */
        canvas(styling s, bool Embed_Styles_On_Construct = false) : element(s, Embed_Styles_On_Construct){}
           
        /**
         * @brief Set the value of a pixel on the canvas.
         * @details
         * This function sets the value of a pixel on the canvas. The coordinates are in pixels relative to the top left corner of the canvas.
         * The color is an RGB object. The Flush argument determines whether or not to call Update_Frame() after setting the pixel.
         * @param x The x coordinate of the pixel.
         * @param y The y coordinate of the pixel.
         * @param color The color of the pixel.
         * @param Flush Whether or not to call Update_Frame() after setting the pixel.
         */
        void set(unsigned int x, unsigned int y, RGB color, bool Flush = true);
        
        /**
         * @brief Flushes the canvas.
         * @details
         * This function flushes the canvas by calling Update_Frame().
         * It is used to update the canvas immediately after making changes to it.
         */
        void flush();
        
        /**
         * @brief Renders the canvas and returns the result.
         * @details This function processes the canvas to generate a vector of UTF objects representing the current state.
         * It handles different stains such as CLASS, STRETCH, COLOR, and EDGE to ensure the canvas is rendered correctly.
         * @return A vector of UTF objects representing the rendered canvas.
         */
        std::vector<GGUI::UTF>&  render() override;

        /**
         * @brief Creates a deep copy of the Canvas and returns it as a movable Element.
         * @return A deep copy of the Canvas as a movable Element.
         */
        element* safeMove() override {
            return new canvas();
        }

        /**
         * @brief Returns the name of the Canvas as a string.
         * @details The returned string is a combination of the class name and the Name property.
         * @return A string representing the name of the Canvas.
         */
        std::string getName() const override {
            return "Canvas<" + Name + ">";
        }
    };

    class sprite{
    public:
        std::vector<GGUI::UTF> Frames;

        int Offset = 0;     // This is for more beautiful mass animation systems
        int Speed = 1;      // Using decimals too slow hmmm...

        int Frame_Distance = 1;

        bool Is_Power_Of_Two = false;

        /**
         * @brief Constructor for Sprite class.
         * @details This constructor initializes a Sprite object with a vector of UTF objects representing the frames,
         * an offset to determine when to start playing the animation, and a speed to control the animation playback.
         * @param frames A vector of UTF objects representing the frames of the animation.
         * @param offset The number of frames to skip before playing the animation.
         * @param speed The speed of the animation playback.
         */
        sprite(std::vector<GGUI::UTF> frames, int offset = 0, int speed = 1);

        /**
         * @brief Constructs a Sprite object with a single frame.
         * @details This constructor initializes the Sprite with a single UTF frame, setting the offset and speed for animation.
         * @param frame A UTF object representing the single frame of the sprite.
         * @param offset The number of frames to skip before playing the animation. Default is 0.
         * @param speed The speed of the animation playback. Default is 1.
         */
        sprite(GGUI::UTF frame, int offset = 0, int speed = 1) 
            : Offset(offset), Speed(speed), Frame_Distance(1) {
            // Add the provided frame to the Frames vector.
            Frames.push_back(frame);
        }

        /**
         * @brief Constructs a Sprite object with default values.
         * @details This constructor sets the Sprite to have a single UTF frame, which is a space character, and sets the offset and speed for animation.
         */
        sprite() : Frame_Distance(1){
            // Set the default frame to a space character.
            Frames.push_back(GGUI::UTF(' '));
            
            // Set the default offset and speed values.
            Offset = 0;
            Speed = 1;
            
            // Set the Is_Power_Of_Two flag to false, indicating the sprite does not have a power of two size.
            Is_Power_Of_Two = false;
        }

        /**
         * @brief Renders a UTF character based on the sprite's current frame and speed.
         * @param Current_Frame The current frame of the animation.
         * @return The rendered UTF character.
         */
        UTF render(unsigned char Current_Time);
    };

    namespace GROUP_TYPE{
        // Defines the group sizes for Sprite group optimizing.
        inline unsigned char QUAD = 1 << 2;
        inline unsigned char HEX = 1 << 3;
        inline unsigned char OCTAL = 1 << 4;
    }

    class terminalCanvas : public element{
    private:
        // DONT GIVE THIS TO USER!!!
        terminalCanvas(){}
    protected:
        std::vector<sprite> Buffer;

        unsigned char Current_Animation_Frame = 0;

        // For speeding up sprite sets, to avoid redundant checks in unordered_maps.
        bool Multi_Frame = false;

        GGUI::sprite (*On_Draw)(unsigned int x, unsigned int y) = 0;
    public:
        terminalCanvas(styling s) : element(s){}
        
        ~terminalCanvas() override;

        void setNextAnimationFrame() { Current_Animation_Frame++; }

        void set(unsigned int x, unsigned int y, sprite& sprite, bool Flush = true);

        void set(unsigned int x, unsigned int y, sprite&& sprite, bool Flush = true);

        void set(unsigned int x, unsigned int y, UTF& sprite, bool Flush = true);
        
        void flush(bool Force_Flush = false);
        
        std::vector<GGUI::UTF>&  render() override;
        
        void groupHeuristics();

        void group(unsigned int Start_Index, int length);

        bool isMultiFrame(){ return Multi_Frame; }

        /**
         * @brief Creates a deep copy of the Terminal_Canvas and returns it as a movable Element.
         * @return A deep copy of the Terminal_Canvas as a movable Element.
         */
        element* safeMove() override {
            return new terminalCanvas();
        }

        /**
         * @brief Returns the name of the Terminal_Canvas as a string.
         * @details The returned string is a combination of the class name and the Name property.
         * @return A string representing the name of the Terminal_Canvas.
         */
        std::string getName() const override {
            // Concatenate class name and Name property to form the full name.
            return "Terminal_Canvas<" + Name + ">";
        }
    
        /**
         * @brief Embeds a vector of points into the canvas.
         * @param pixels A vector of points where the x and y coordinates are embedded in the vector by row major order.
         * @param border_style The style of border to use for constructing the bit masks.
         * @param flush Whether to flush the buffer after embedding the vector.
         * @return void
         * @details
         * This function takes a vector of points and embeds them into the canvas. The points are expected to be in row major order
         * and the vector should have a size equal to the usable area of the canvas. The function will then construct the bit masks
         * by analyzing the ways the points connect to each other. The bit masks are then used to construct the symbols on the canvas.
         * The symbols are looked up in the custom_border map based on the bit mask. If the symbol is not found in the map, the point is skipped.
         * The function will then set the points in the canvas to the corresponding symbol. If flush is true, the buffer is flushed after
         * the points are set.
         */
        void embedPoints(std::vector<bool> pixels, styled_border border_style = GGUI::STYLES::BORDER::Single, bool Flush = true);

        void setOnDraw(GGUI::sprite (*On_Draw)(unsigned int x, unsigned int y)){
            this->On_Draw = On_Draw;
        }
    };

    namespace DRAW{

        /**
         * @brief Draws a line on the canvas.
         * @param x1 The x-coordinate of the first point.
         * @param y1 The y-coordinate of the first point.
         * @param x2 The x-coordinate of the second point.
         * @param y2 The y-coordinate of the second point.
         * @param pixels The vector of pixels of the canvas.
         * @param width The width of the canvas.
         * @details
         * This function draws a line on the canvas by setting the pixels to true.
         * It uses the Bresenham line drawing algorithm to determine which pixels to set.
         */
        void line(int x1, int y1, int x2, int y2, std::vector<bool>& pixels, int width);

        /**
         * @brief Helper function for the above, creates a line on a given buffer.
         * @param Start The starting point of the line.
         * @param End The ending point of the line.
         * @param Buffer_Width The width of the buffer.
         * @return A vector of booleans representing the line on the buffer.
         * @details
         * This function creates a line on a given buffer by setting the pixels to true.
         * It uses the Bresenham line drawing algorithm to determine which pixels to set.
         */
        std::vector<bool> line(FVector2 Start, FVector2 End, int Buffer_Width);

        /**
         * @brief Symmetrical circle draw helper.
         * @param x_center The x position of the center of the circle.
         * @param y_center The y position of the center of the circle.
         * @param x The current x position of the circle.
         * @param y The current y position of the circle.
         * @param pixels The vector of pixels of the canvas.
         * @param width The width of the canvas.
         * @details
         * This function is a helper function for drawing a circle on the canvas.
         * It fills in the circle symmetrically by setting the pixels to true.
         */
        void symmetryFillerForCircle(int x_center, int y_center, int x, int y, std::vector<bool>& pixels, int width);

        /**
         * @brief Fills a circle in a given buffer with true values.
         * @param x_center The x position of the center of the circle.
         * @param y_center The y position of the center of the circle.
         * @param r The radius of the circle.
         * @param pixels The buffer to fill.
         * @param width The width of the buffer.
         * @details
         * This function fills a circle in a given buffer with true values by
         * using the Bresenham circle drawing algorithm to determine which pixels
         * to set.
         */
        void circle(int x_center, int y_center, int r, std::vector<bool>& pixels, int width);

        /**
         * @brief Fills a circle in a given buffer with true values.
         * @param Center The center of the circle.
         * @param Radius The radius of the circle.
         * @param Buffer_Width The width of the buffer.
         * @return A boolean vector representing the circle.
         * @details
         * This function fills a circle in a given buffer with true values by
         * using the Bresenham circle drawing algorithm to determine which pixels
         * to set.
         */
        std::vector<bool> circle(FVector2 Center, int Radius, int Buffer_Width);

        /**
         * @brief Draws a cubic Bezier curve in a given buffer with true values.
         * @param P0 The first control point of the curve.
         * @param P1 The second control point of the curve.
         * @param P2 The third control point of the curve.
         * @param P3 The fourth control point of the curve.
         * @param Buffer_Width The width of the buffer.
         * @param pixels The boolean vector representing the buffer.
         * @details
         * This function draws a cubic Bezier curve in a given buffer with true values by
         * using the parametric equation of the Bezier curve to determine which pixels
         * to set.
         */
        void cubicBezierCurve(FVector2 P0, FVector2 P1, FVector2 P2, FVector2 P3, std::vector<bool>& pixels, int width);
        
        /**
         * @brief Draws a cubic Bezier curve in a given buffer with true values.
         * @param P0 The first control point of the curve.
         * @param P1 The second control point of the curve.
         * @param P2 The third control point of the curve.
         * @param P3 The fourth control point of the curve.
         * @param Buffer_Width The width of the buffer.
         * @return A boolean vector representing the buffer with true values where the curve is drawn.
         * @details
         * This function draws a cubic Bezier curve in a given buffer with true values by
         * using the parametric equation of the Bezier curve to determine which pixels
         * to set.
         */
        std::vector<bool> cubicBezierCurve(FVector2 P0, FVector2 P1, FVector2 P2, FVector2 P3, int Buffer_Width);

    }

    namespace FONT{
        // Based on: https://learn.microsoft.com/en-us/typography/opentype/spec/otff
        class fontHeader{
        public:
        };

        fontHeader parseFontFile(std::string File_Name);
    }

}

#endif
#ifndef _BUTTON_H_
#define _BUTTON_H_

#include <vector>
#include <string>

#include <functional>










namespace GGUI{

    class button : public element{
    public:

        /**
         * @brief Constructs a Button element with specified text, action, and styling.
         * @param text The text to display on the button.
         * @param press The function to call when the button is pressed.
         * @param s The styling for the button.
         * @param Embed_Styles_On_Construct If true, the styling will be embedded into the button's style. Only use if you know what you're doing!!!
         */
        button(styling s = STYLES::CONSTANTS::Default, bool Embed_Styles_On_Construct = false);

        /**
         * @brief Creates a deep copy of the Button object.
         * @return A pointer to the new Button object.
         */
        element* safeMove() override {
            // Create a new Button object and copy all the data from the current Button object to the new one.
            return new button();
        }

        /**
         * @brief Gets the name of the Button object.
         * @return A string containing the name of the Button object.
         */
        std::string getName() const override{
            return "Button<" + Name + ">";
        }

        void setText(std::string Text){
            // There should always be an Text_Field child
            if (Style->Childs.size() == 0){
                INTERNAL::reportStack("No Text_Field child found in Button: " + getName());
            }

            ((textField*)Style->Childs.back())->setText(Text);
        }
    };
}

#endif
#ifndef _LOGGER_H_
#define _LOGGER_H_



namespace GGUI{

    class fileStream;

    namespace INTERNAL{
        // Contains Logging utils.
        namespace LOGGER{
            // File handle for logging to files for Atomic access across different threads.
            extern atomic::Guard<fileStream> Handle;

            extern void Init();

            extern void Log(std::string Text);
        };
    }
}

#endif
#ifndef _RENDERER_H_
#define _RENDERER_H_

#undef min
#undef max

#include <functional>
#include <thread>
#include <atomic>
#include <mutex>
#include <condition_variable>














//GGUI uses the ANSI escape code
//https://en.wikipedia.org/wiki/ANSI_escape_code
namespace GGUI{

    namespace INTERNAL{
        class bufferCapture;

        namespace atomic{
            enum class status{
                RESUMED,
                PAUSED,
                LOCKED,
                NOT_INITIALIZED
            };

            extern std::mutex Mutex;
            extern std::condition_variable Condition;

            extern status Pause_Render_Thread;
        }

        // Inits with 'NOW()' when created
        class buttonState {
        public:
            bool State;
            std::chrono::high_resolution_clock::time_point Capture_Time;

            buttonState(bool state = false) : State(state), Capture_Time(std::chrono::high_resolution_clock::now()) {}
        };

        extern std::vector<UTF>& Abstract_Frame_Buffer;                 //2D clean vector without bold nor color
        extern std::string Frame_Buffer;                                //string with bold and color, this what gets drawn to console.

        extern std::vector<INTERNAL::bufferCapture*> Global_Buffer_Captures;

        extern unsigned int Max_Width;
        extern unsigned int Max_Height;

        extern atomic::Guard<std::vector<Memory>> Remember;

        extern std::vector<Action*> Event_Handlers;
        extern std::vector<Input*> Inputs;
        
        extern std::unordered_map<std::string, element*> Element_Names;

        extern element* Focused_On;
        extern element* Hovered_On;

        extern IVector3 Mouse;    
        extern bool Mouse_Movement_Enabled;

        extern std::unordered_map<std::string, buttonState> KEYBOARD_STATES;

        extern time_t MAX_UPDATE_SPEED;
        extern int Inputs_Per_Second;
        extern int Inputs_Per_Query;

        extern unsigned long long Render_Delay;    // describes how long previous render cycle took in ms
        extern unsigned long long Event_Delay;    // describes how long previous memory tasks took in ms

        extern atomic::Guard<std::unordered_map<int, styling>> Classes;
        extern std::unordered_map<std::string, int> Class_Names;

        extern window* Main;  

        extern std::unordered_map<GGUI::terminalCanvas*, bool> Multi_Frame_Canvas;

        // Represents the update speed of each elapsed loop of passive events, which do NOT need user as an input.
        extern time_t MAX_UPDATE_SPEED;
        extern time_t MIN_UPDATE_SPEED;    // Close approximation to 60 fps.
        extern time_t CURRENT_UPDATE_SPEED;
        extern float Event_Thread_Load;  // Describes the load of animation and events from 0.0 to 1.0. Will reduce the event thread pause.

        extern unsigned long long Render_Delay;    // describes how long previous render cycle took in ms
        extern unsigned long long Event_Delay;    // describes how long previous memory tasks took in ms
        extern unsigned long long Input_Delay;     // describes how long previous input tasks took in ms

        extern std::string now();

        extern std::string constructLoggerFileName();

        extern void reportStack(std::string Problem);

        /**
         * @brief Initializes platform-specific settings for console handling.
         * @details This function sets up the console handles and modes required for input and output operations.
         *          It enables mouse and window input, sets UTF-8 mode for output, and prepares the console for
         *          handling specific ANSI features.
         */
        extern void initPlatformStuff();
        
        
        /**
         * @brief Sleep for the specified amount of milliseconds.
         * @details This function is used to pause the execution of the program for a specified amount of time.
         *          It is implemented differently for each platform, so on Windows, it calls the Sleep function,
         *          while on Linux and macOS it calls the usleep function.
         * @param mm The number of milliseconds to sleep.
         */
        void SLEEP(unsigned int milliseconds);
        
        /**
         * @brief Renders the current frame to the console.
         * 
         * This function moves the console cursor to the top left corner of the screen
         * and writes the contents of the Frame_Buffer to the console.
         * 
         * @note The number of bytes written to the console is stored in a temporary
         * variable but is not used elsewhere in the function.
         */
        extern void renderFrame();

        /**
         * @brief Updates the maximum width and height of the console window.
         * 
         * This function retrieves the current console screen buffer information and updates
         * the maximum width and height based on the console window dimensions. If the console
         * information is not retrieved correctly, an error message is reported. Additionally,
         * if the main window is active, its dimensions are set to the updated maximum width
         * and height.
         */
        extern void updateMaxWidthAndHeight();
        
        /**
         * @brief Updates the frame.
         * @details This function updates the frame. It's the main entry point for the rendering thread.
         * @note This function will return immediately if the rendering thread is paused.
         */
        void updateFrame();
        
        /**
         * @brief Queries and appends new input records to the existing buffered input.
         *
         * This function reads input records from the console and appends them to the 
         * existing buffered input which has not yet been processed. It uses the previous 
         * size of the raw input buffer to determine the starting point for new input records.
         *
         * @note The function ensures that negative numbers do not create overflows by 
         *       using the maximum of the remaining capacity and the total capacity.
         *
         * @param None
         * @return None
         */
        extern void queryInputs();
        
        /**
         * @brief Gracefully shuts down the application.
         *
         * This function performs a series of steps to gracefully shut down the application:
         * 1. Logs the initiation of the termination process.
         * 2. Signals subthreads to terminate.
         * 3. Waits for all subthreads to join.
         * 4. Reverts the console to its normal mode.
         * 5. Cleans up platform-specific resources and settings.
         * 6. Logs the successful shutdown of the application.
         * 7. Exits the application with the specified exit code.
         *
         * @param signum The exit code to be used when terminating the application.
         */
        extern void EXIT(int Signum = 0);
    }

    /**
     * @brief Exits the application with the given signal number.
     * 
     * This function calls the INTERNAL::Exit function with the provided signal number.
     * 
     * @param Signum The signal number to exit with. Default is 0.
     */
    inline void EXIT(int Signum = 0) { INTERNAL::EXIT(Signum); }

    /**
     * @brief Processes mouse input events and updates the input list.
     * @details This function checks the state of mouse buttons (left, right, and middle)
     *          and determines if they have been pressed or clicked. It compares the current
     *          state with the previous state and the duration the button has been pressed.
     *          Based on these checks, it creates corresponding input objects and adds them
     *          to the Inputs list.
     */
    extern void mouseAPI();

    /**
     * @brief Handles mouse scroll events.
     * @details This function checks if the mouse scroll up or down button has been pressed and if the focused element is not null.
     *          If the focused element is not null, it calls the scroll up or down function on the focused element.
     */
    extern void scrollAPI();

    /**
     * @brief Returns the length of a Unicode character based on the first byte.
     * @details This function takes the first byte of a Unicode character and returns its length in bytes.
     *          If the character is not a Unicode character, it returns 1.
     * @param first_char The first byte of the character.
     * @return The length of the character in bytes.
     */
    extern int getUnicodeLength(char first_char);

    /**
     * @brief Gets the current maximum width of the terminal.
     * @details This function returns the current maximum width of the terminal. If the width is 0, it will set the carry flag to indicate that a resize is needed to be performed.
     *
     * @return The current maximum width of the terminal.
     */
    extern int getMaxWidth();

    /**
     * @brief Gets the current maximum height of the terminal.
     * @details This function returns the current maximum height of the terminal. If the height is 0, it will set the carry flag to indicate that a resize is needed to be performed.
     *
     * @return The current maximum height of the terminal.
     */
    extern int getMaxHeight();

    /**
     * @brief Converts a vector of UTFs into a Super_String.
     * @details This function takes a vector of UTFs, and converts it into a Super_String. The resulting Super_String is stored in a cache, and the cache is resized if the window size has changed.
     * @param Text The vector of UTFs to convert.
     * @param Width The width of the window.
     * @param Height The height of the window.
     * @return A pointer to the resulting Super_String.
     */
    extern GGUI::Super_String* liquifyUTFText(std::vector<GGUI::UTF>& Text, int Width, int Height);

    /**
     * @brief Updates the frame.
     * @details This function updates the frame. It's the main entry point for the rendering thread.
     * @note This function will return immediately if the rendering thread is paused.
     */
    extern void updateFrame();
    
    /**
     * @brief Pauses the rendering thread.
     * @details This function pauses the rendering thread. The thread will wait until the rendering thread is resumed.
     */
    extern void pauseGGUI();

    /**
     * @brief Resumes the rendering thread.
     * @details This function resumes the rendering thread after it has been paused.
     * @param restore_render_to The status to restore the rendering thread to.
     */
    extern void resumeGGUI(INTERNAL::atomic::status restore_render_to = INTERNAL::atomic::status::RESUMED);

    /**
     * @brief This function is a helper for the smart memory system to recall which tasks should be prolonged, and which should be deleted.
     * @details This function is a lambda function that is used by the Atomic::Guard class to prolong or delete memories in the smart memory system.
     *          It takes a pointer to a vector of Memory objects and prolongs or deletes the memories in the vector based on the time difference between the current time and the memory's start time.
     */
    extern void recallMemories();

    /**
     * @brief Removes focus from the currently focused element and its children.
     * @details This function checks if there is a currently focused element.
     *          If there is, it sets the focus state on the element and its children to false.
     *          Focus is only removed if the element's current focus state differs from the desired state.
     */
    extern void unFocusElement();

    /**
     * @brief Removes the hover state from the currently hovered element and its children.
     * @details This function checks if there is a currently hovered element.
     *          If there is, it sets the hover state on the element and its children to false.
     *          Hover is only removed if the element's current hover state differs from the desired state.
     */
    extern void unHoverElement();

    /**
     * @brief Updates the currently focused element to a new candidate.
     * @details This function checks if the new candidate is the same as the current focused element.
     *          If not, it removes the focus from the current element and all its children.
     *          Then, it sets the focus on the new candidate element and all its children.
     * @param new_candidate The new element to focus on.
     */
    extern void updateFocusedElement(GGUI::element* new_candidate);

    /**
     * @brief Updates the currently hovered element to a new candidate.
     * @details This function checks if the new candidate is the same as the current hovered element.
     *          If not, it removes the hover state from the current element and all its children.
     *          Then, it sets the hover state on the new candidate element and all its children.
     * @param new_candidate The new element to hover on.
     */
    extern void updateHoveredElement(GGUI::element* new_candidate);

    /**
     * @brief Handles all events in the system.
     * @details This function goes through all event handlers and checks if the event criteria matches any of the inputs.
     *          If a match is found, it calls the event handler job with the input as an argument.
     *          If the job is successful, it removes the input from the list of inputs.
     *          If the job is unsuccessful, it reports an error.
     */
    extern void eventHandler();

    /**
     * Get the ID of a class by name, assigning a new ID if it doesn't exist.
     * 
     * @param n The name of the class.
     * @return The ID of the class.
     */
    extern int getFreeClassID(std::string n);

    /**
     * @brief Adds a new class with the specified name and styling.
     * @details This function retrieves a unique class ID for the given name.
     *          It then associates the provided styling with this class ID 
     *          in the `Classes` map.
     * 
     * @param name The name of the class.
     * @param Styling The styling to be associated with the class.
     */
    extern void addClass(std::string name, styling Styling);

    /**
     * @brief Initializes the GGUI system and returns the main window.
     * 
     * @return The main window of the GGUI system.
     */
    extern GGUI::window* initGGUI();

    /**
     * @brief Reports an error to the user.
     * @param Problem The error message to display.
     * @note If the main window is not created yet, the error will be printed to the console.
     * @note This function is thread safe.
     */
    extern void report(std::string Problem);

    /**
     * @brief Nests a text buffer into a parent buffer while considering the childs position and size.
     * 
     * @param Parent The parent element which the text is being nested into.
     * @param child The child element which's text is being nested.
     * @param Text The text buffer to be nested.
     * @param Parent_Buffer The parent buffer which the text is being nested into.
     */
    extern void nestUTFText(GGUI::element* Parent, GGUI::element* child, std::vector<GGUI::UTF> Text, std::vector<GGUI::UTF>& Parent_Buffer);

    /**
     * @brief Pauses all other GGUI internal threads and calls the given function.
     * @details This function will pause all other GGUI internal threads and call the given function.
     * @param f The function to call.
     */
    extern void pauseGGUI(std::function<void()> f);

    /**
     * @brief Use GGUI in a simple way.
     * @details This is a simple way to use GGUI. It will pause all other GGUI internal threads, initialize GGUI, call the given function, sleep for the given amount of milliseconds, and then exit GGUI.
     * @param DOM The function that will add all the elements to the root window.
     * @param Sleep_For The amount of milliseconds to sleep after calling the given function.
     */
    extern void GGUI(std::function<void()> DOM, unsigned long long Sleep_For = 0);

    /**
     * @brief Use GGUI in a simple way.
     * @details This is a simple way to use GGUI. It will pause all other GGUI internal threads, initialize GGUI, add all the elements to the root window, sleep for the given amount of milliseconds, and then exit GGUI.
     * @param App The whole GGUI Application that GGUI holds.
     * @param Sleep_For The amount of milliseconds to sleep after calling the given function.
     */
    extern void GGUI(styling App, unsigned long long Sleep_For = 0);

    /**
     * @brief Handles the pressing of the tab key.
     * @details This function selects the next tabbed element as focused and not hovered.
     *          If the shift key is pressed, it goes backwards in the list of tabbed elements.
     */
    extern void handleTabulator();

    /**
     * @brief Handles escape key press events.
     * @details This function checks if the escape key has been pressed and if the focused element is not null.
     *          If the focused element is not null, it calls the Un_Focus_Element function to remove the focus.
     *          If the focused element is null but the hovered element is not null, it calls the Un_Hover_Element
     *          function to remove the hover.
     */
    extern void handleEscape();

    /**
     * @brief Encodes a buffer of UTF elements by setting start and end flags based on color changes.
     * 
     * @param Buffer A vector of UTF elements to be encoded.
     * @details The function marks the beginning and end of color strips within the buffer. 
     *          It checks each UTF element's foreground and background colors with its adjacent elements
     *          to determine where encoding strips start and end.
     */
    extern void encodeBuffer(std::vector<GGUI::UTF>& Buffer);

    /**
     * @brief Notifies all global buffer capturers about the latest data to be captured.
     *
     * This function is used to inform all global buffer capturers about the latest data to be captured.
     * It iterates over all global buffer capturers and calls their Sync() method to update their data.
     *
     * @param informer Pointer to the buffer capturer with the latest data.
     */
    extern void informAllGlobalBufferCaptures(INTERNAL::bufferCapture* informer);

    /**
     * @brief Determines if a given pointer is likely deletable (heap-allocated).
     *
     * This function assesses whether a pointer may belong to the heap by comparing its
     * position relative to known memory sections such as the stack, heap, and data segments.
     *
     * @param ptr Pointer to be evaluated.
     * @return True if the pointer is likely deletable (heap-allocated), false otherwise.
     */
    extern bool isDeletable(void* ptr);
}

#endif
#ifndef _LIST_VIEW_H_
#define _LIST_VIEW_H_










namespace GGUI{
    class listView : public element{
    public:
        //We can always assume that the list starts from the upper left corner, right?
        element* Last_Child = new element(styling(position(0, 0) | width(0) | height(0)));

        /**
         * @brief Default constructor for List_View.
         * 
         * This constructor calls the default constructor of Element and sets the Allow_Dynamic_Size property to true.
         */
        listView() : element(){ allowDynamicSize(true); }

        /**
         * @brief Constructor for List_View.
         * 
         * This constructor calls the Element constructor with the given style and then sets the Allow_Dynamic_Size property to true.
         * This is so that the list view can automatically resize itself based on the elements it contains.
         * 
         * @param s The style for the list view.
         */
        listView(styling s, bool Embed_Styles_On_Construct = false) : element(s, Embed_Styles_On_Construct){ allowDynamicSize(true); }

        /**
         * @brief Destructor for the List_View class.
         *
         * This destructor is responsible for properly deallocating all the memory
         * allocated by the List_View object, including its child elements.
         */
        ~listView() override {
            // Delete all child elements to avoid memory leaks.
            for (element* e : Style->Childs) {
                delete e;
            }

            // Call the base class destructor to ensure all parent class resources are cleaned up.
            element::~element();
        }

        //End of user constructors.

        /**
         * @brief Adds a child element to the list view.
         * @details This function adds a child element to the list view and manages the positioning and sizing
         *          of the child element within the list view. It takes into account the list's flow direction,
         *          border offsets, and dynamic sizing capabilities.
         * @param e The child element to be added.
         */
        void addChild(element* e) override;
        
        /**
         * @brief Calculates the hitboxes of all child elements of the list view.
         * @details This function is similar to the Remove(Element* c) like behaviour.
         *          It takes into account the border offsets of both the current and the next element as well as their positions.
         *          For an horizontal list, it checks if the next element's width is greater than the current element's width.
         *          For a vertical list, it checks if the next element's height is greater than the current element's height.
         *          If the next element is greater in size than the current element, it sets the maximum width/height to the next element's width/height.
         *          It finally sets the dimensions of the list view to the maximum width and height if the list view is dynamically sized and the maximum width/height is greater than the current width/height.
         * @param Starting_Offset The starting offset into the child array.
         */
        void calculateChildsHitboxes(unsigned int Starting_Offset = 0) override;

        /**
         * @brief Gets the name of the list view.
         * @details This function returns the name of the list view in the format "List_View<Name>".
         * @return The name of the list view.
         */
        std::string getName() const override;

        /**
         * @brief Removes a child element from the list view.
         * @param remove The child element to be removed.
         * @return true if the element was successfully removed, false if not.
         *
         * This function removes a child element from the list view and updates the position of all elements following the removed element.
         * It also recalculates the width and height of the list view and updates the dimensions of the list view if it is dynamically sized.
         */
        bool remove(element* e) override;

        /**
         * @brief Sets the flow direction of the list view.
         * @details This function sets the flow priority of the list view to the specified direction.
         *          The flow direction determines how the child elements are arranged within the list view.
         * @param gd The direction to set as the flow priority.
         */
        void setFlowDirection(DIRECTION gd){
            Style->Flow_Priority = gd;
        }

        /**
         * @brief Gets the current flow direction of the list view.
         * @details This function returns the current flow direction of the list view.
         * @return The flow direction of the list view.
         */
        DIRECTION getFlowDirection(){
            return (DIRECTION)Style->Flow_Priority.Value;
        }

        /**
         * @brief Gets a child element from the list view by its index.
         * @details This function returns a pointer to the child element at the specified index.
         *          The index is checked to be within the range of the child array.
         * @param index The index of the child element to retrieve.
         * @return The child element at the specified index, or nullptr if the index is out of range.
         */
        template<typename  T>
        T* get(int index){
            if (index > (signed)Style->Childs.size() - 1)
                return nullptr;

            if (index < 0)
                index = (signed)Style->Childs.size() + index - 1;

            return (T*)this->Style->Childs[index];
        }

        /**
         * @brief Creates a deep copy of the List_View object.
         * @details This function creates a new List_View object and copies all the data from the current List_View object to the new one.
         * @return A pointer to the new List_View object.
         */
        element* safeMove() override {
            return new listView();
        }
    };

    class scrollView : public element{
    protected:
        unsigned int Scroll_Index = 0;  // Render based on the offset of the scroll_index by flow direction.
    public:

        /**
         * @brief Constructor for the Scroll_View class.
         * @details This constructor initializes a Scroll_View object with the specified styling.
         * @param s The styling to be applied to the Scroll_View.
         * @param Embed_Styles_On_Construct If true, the styling will be embedded into the Scroll_View's style. Only use if you know what you're doing!!!
         */
        scrollView(styling s, bool Embed_Styles_On_Construct = false) : element(s, Embed_Styles_On_Construct) {}

        /**
         * @brief Constructor for the Scroll_View class.
         * @details This constructor initializes a Scroll_View object with a reference to a List_View object.
         * @param container The List_View object to be used as the container for the Scroll_View.
         */
        scrollView(listView& container);

        /**
         * @brief Adds a child element to the Scroll_View.
         * @details This function adds a child element to the Scroll_View and marks the Scroll_View as dirty with the DEEP stain.
         * @param e The child element to be added.
         */
        void addChild(element* e) override;

        /**
         * @brief Enables or disables scrolling for the Scroll_View.
         * @details This function updates the scrolling capability of the Scroll_View.
         *          If scrolling is enabled, it ensures that scrolling events are registered.
         * @param allow A boolean indicating whether to enable or disable scrolling.
         */
        void allowScrolling(bool allow);
    
        /**
         * @brief Checks if the scrolling is enabled for the Scroll_View.
         * @details This function checks the value of the Allow_Scrolling property of the Scroll_View's styling.
         * @return A boolean indicating whether the scrolling is enabled for the Scroll_View.
         */
        bool isScrollingEnabled(){
            return Style->Allow_Scrolling.Value;
        }

        /**
         * @brief Scrolls the view up by one index.
         * @details Decreases the scroll index if it is greater than zero and updates the container's position based on the growth direction.
         * Marks the view as dirty for a deep update.
         */
        void scrollUp() override;

        /**
         * @brief Scrolls the view down by one index.
         * @details Increases the scroll index by one and updates the container's position based on the growth direction.
         * Marks the view as dirty for a deep update.
         */
        void scrollDown() override;

        /**
         * @brief Removes a child element from the scroll view.
         * @details This function forwards the request to the Remove(Element* remove) function of the container.
         * @param remove The element to be removed.
         * @return true if the element was successfully removed, false if not.
         */
        bool remove(element* e) override;

        /**
         * @brief Gets the name of the scroll view.
         * @details This function returns the name of the scroll view.
         * @return The name of the scroll view.
         */
        std::string getName() const override;

        /**
         * @brief Sets the growth direction of the scroll view.
         * @details This function forwards the request to the Set_Flow_Direction(DIRECTION gd) function of the container.
         * @param gd The direction value to set as the growth direction.
         */
        void setGrowthDirection(DIRECTION gd){
            ((listView*)Style->Childs[0])->setFlowDirection(gd);
        }

        /**
         * @brief Gets the current growth direction of the scroll view.
         * @details This function retrieves the current growth direction of the scroll view.
         * @return The current growth direction of the scroll view.
         */
        DIRECTION getGrowthDirection(){
            return ((listView*)Style->Childs[0])->getFlowDirection();
        }

        /**
         * @brief Gets a child element from the scroll view by its index.
         * @details This function forwards the request to the Get(int index) function of the container.
         * @param index The index of the child element to retrieve.
         * @return The child element at the specified index, or nullptr if the index is out of range.
         */
        template<typename  T>
        T* get(int index){
            return ((listView*)Style->Childs[0])->get<T>(index);
        }

        /**
         * @brief Gets the container of the scroll view.
         * @details This function retrieves the container of the scroll view, which is a List_View.
         * @return The container of the scroll view.
         */
        listView* getContainer(){
            return (listView*)Style->Childs[0];
        }
    
    };
}

#endif
#ifndef _SETTINGS_H_
#define _SETTINGS_H_

#include <chrono>
#include <string>

namespace GGUI{
    namespace SETTINGS{
        // How fast for a detection of hold down situation.
        extern unsigned long long Mouse_Press_Down_Cooldown;
        
        extern bool Word_Wrapping;

        extern std::chrono::milliseconds Thread_Timeout;

        extern bool ENABLE_GAMMA_CORRECTION;

        namespace LOGGER{

            extern std::string File_Name;

        }

        /**
         * @brief Initializes the settings for the application.
         *
         * This function sets up the necessary configurations for the application
         * by initializing the logger file name using the internal logger file name
         * construction method.
         */
        extern void initSettings();
    };
}

#endif
#ifndef _WINDOW_H_
#define _WINDOW_H_










//GGUI uses the ANSI escape code
//https://en.wikipedia.org/wiki/ANSI_escape_code
namespace GGUI{
    class window : public element{
        std::string Title = "";  //if this is empty then no title

        RGB Before_Hiding_Border_Color = COLOR::WHITE;
        RGB Before_Hiding_Border_Background_Color = COLOR::BLACK;
        bool Has_Hidden_Borders = false;
    public:
    
        /**
         * @brief A window element that wraps a console window.
         * This element is capable of modifying the window's title, border visibility, and colors.
         * @param title The title string to be displayed in the window's title bar.
         * @param s The Styling object to be used for the window.
         * @param Embed_Styles_On_Construct If true, the styling will be embedded into the window's style. Only use if you know what you're doing!!!
         */
        window(styling s = STYLES::CONSTANTS::Default, bool Embed_Styles_On_Construct = false);

        /**
         * @brief Destructor for the Window class.
         *
         * This destructor is responsible for cleaning up the memory allocated by the Window object.
         * It calls the base class destructor to ensure all parent class resources are cleaned up.
         */
        ~window() override{
            // call the base destructor.
            element::~element();
        }

        //End of user constructors.

        /**
         * @brief Updates the colors of the hidden borders for the window.
         * This function prioritizes border color variants if they are available;
         * otherwise, it falls back to text colors or default values.
         */
        void updateHiddenBorderColors();

        /**
         * @brief Sets the title of the window and updates border visibility and colors accordingly.
         * 
         * This function sets the window's title and ensures that the border is shown if the title is not empty.
         * If the window previously had hidden borders, it updates the border colors based on the background color.
         * 
         * @param t The new title for the window.
         */
        void setTitle(std::string t);

        /**
         * @brief Returns the title of the window.
         * 
         * @return The title of the window as a string.
         */
        std::string getTitle();
        
        /**
         * @brief Adds the border of the window to the rendered string.
         * 
         * @param w The window to add the border for.
         * @param Result The string to add the border to.
         */
        void addOverhead(element* w, std::vector<UTF>& Result) override;

        /**
         * @brief Gets the name of the window.
         * 
         * @return The name of the window as a string.
         */
        std::string getName() const override;

        /**
         * @brief Shows or hides the window's border.
         * @details This function toggles the border visibility of the window.
         *          If the state has changed, it updates the border enabled state, marks the element as dirty for border changes, and updates the frame.
         * @param b The desired state of the border visibility.
         */
        void showBorder(bool state) override;

        /**
         * @brief Shows or hides the window's border.
         * @details This function toggles the border visibility of the window.
         *          If the state has changed, it updates the border enabled state, marks the element as dirty for border changes, and updates the frame.
         * @param b The desired state of the border visibility.
         * @param Previous_State The current state of the border visibility.
         */
        void showBorder(bool state, bool previus_state) override;

        /**
         * @brief Sets the background color of the window.
         * @details This function sets the background color of the window to the specified RGB value.
         *          It marks the element as dirty for color updates and triggers a frame update.
         * @param color The RGB color to set as the background color.
         */
        void setBackgroundColor(RGB color) override;

        /**
         * @brief Sets the text color of the window.
         * @details This function sets the text color of the window to the specified RGB value.
         *          It marks the element as dirty for color updates and triggers a frame update.
         * @param color The RGB color to set as the text color.
         */
        void setTextColor(RGB color) override;

        /**
         * @brief Sets the background color of the window's border.
         * @details This function sets the background color of the window's border to the specified RGB value.
         *          It marks the element as dirty for color updates and triggers a frame update.
         * @param color The RGB color to set as the background color of the window's border.
         */
        void setBorderBackgroundColor(RGB color) override;

        /**
         * @brief Sets the color of the window's border.
         * @details This function sets the color of the window's border to the specified RGB value.
         *          It marks the element as dirty for color updates and triggers a frame update.
         * @param color The RGB color to set as the border color.
         */
        void setBorderColor(RGB color) override;

        /**
         * @brief Creates a deep copy of the Window object.
         * @details This function creates a new Window object and copies all the data from the current Window object to the new one.
         * @return A pointer to the new Window object.
         */
        element* safeMove() override {
            return new window();
        }
    };
}

#endif
#ifndef _HTML_H_
#define _HTML_H_











namespace GGUI{

    class HTML : public element{
    private:
        // DONT GIVE TO USER !!!
        HTML(){}
    private:
        fileStream* Handle = nullptr;
    public:
    
        /**
         * @brief Constructor of the HTML class.
         * @param File_Name The name of the file to open.
         * 
         * This constructor will pause the GGUI renderer and create a new file stream
         * that will read the file and parse the HTML when it is changed.
         * The parsed HTML will be set as the child of this HTML object.
         */
        HTML(std::string File_Name);

        /**
         * @brief Destructor of the HTML class.
         * 
         * This destructor is responsible for properly deallocating all the memory
         * allocated by the HTML object.
         * It will also close the file stream associated with the HTML object.
         */
        ~HTML() override{
            if (Handle != nullptr){
                delete Handle;
            }

            // Call the base destructor to ensure that all the resources are properly
            // released.
            element::~element();
        }

        /**
         * @brief Creates a deep copy of the HTML object.
         * @return A pointer to the new HTML object.
         * 
         * This function will create a new HTML object and copy all the data from the current
         * HTML object to the new one. This is useful for creating a new HTML object that is
         * a modified version of the current one.
         */
        element* safeMove() override {
            return new HTML();
        }

        /**
         * @brief Gets the name of the HTML object.
         * @return The name of the HTML object.
         * 
         * This function will return the name of the HTML object, which is the name
         * of the file that was opened using the HTML constructor.
         */
        std::string getName() const override{
            return "HTML<" + Name + ">";
        }
    };

    enum class HTML_GROUP_TYPES{
        UNKNOWN,
        TEXT,
        NUMBER,
        OPERATOR,   // =, 
        WRAPPER,    // <>, [], {}, (), "", ''
        SPACING,    // newline, ' ', '\t'
        ATTRIBUTE,  // Contains attributes as an wrapper extension. id="123"
    };

    enum class PARSE_BY{
        NONE                    = 0,
        TOKEN_WRAPPER           = 1 << 0,
        DYNAMIC_WRAPPER         = 1 << 1, 
        OPERATOR_PARSER         = 1 << 2,
        NUMBER_POSTFIX_PARSER   = 1 << 3,
    };

    enum class HTML_POSITION_TYPE{
        STATIC,     // Default positioning, like in GGUI.
        RELATIVE,   // Relative to parent.
        ABSOLUTE,   // Relative to screen.
        FIXED,      // Relative to screen, but does not move with scrolling.
        STICKY,     // Relative to screen until crosses given threshold.
    };

    /**
     * @brief Bitwise OR operator for PARSE_BY.
     * @details This function takes two PARSE_BY values and returns a new PARSE_BY value that is the result of the bitwise OR operation on the two input values.
     * @param first The first PARSE_BY value.
     * @param second The second PARSE_BY value.
     * @return The result of the bitwise OR operation on the two input values.
     */
    extern PARSE_BY operator|(PARSE_BY first, PARSE_BY second);

    /**
     * @brief Bitwise AND operator for PARSE_BY.
     * @details This function takes two PARSE_BY values and returns a new PARSE_BY value that is the result of the bitwise AND operation on the two input values.
     * @param first The first PARSE_BY value.
     * @param second The second PARSE_BY value.
     * @return The result of the bitwise AND operation on the two input values.
     */
    extern PARSE_BY operator&(PARSE_BY first, PARSE_BY second);

    /**
     * @brief Bitwise OR operator for PARSE_BY.
     * @details This function takes a PARSE_BY value and a PARSE_BY value and sets the first value to the result of the bitwise OR operation on the two input values.
     * @param first The first PARSE_BY value.
     * @param second The second PARSE_BY value.
     */
    extern void operator|=(PARSE_BY& first, PARSE_BY second);

    class HTMLToken{
    public:
        HTML_GROUP_TYPES Type = HTML_GROUP_TYPES::UNKNOWN;
        std::string Data = "";
        std::vector<HTMLToken*> Childs;    // also contains attributes!
        filePosition Position;

        PARSE_BY Parsed_By = PARSE_BY::NONE;

        /**
         * @brief Constructor for an HTML_Token.
         * @param Type The type of the token.
         * @param Data The data of the token.
         */
        HTMLToken(HTML_GROUP_TYPES Type, std::string Data) {
            this->Type = Type;
            this->Data = Data;
        }

        /**
         * @brief Constructor for an HTML_Token.
         * @param Type The type of the token.
         * @param Data The data of the token.
         * @param position The position of the token in the source file.
         */
        HTMLToken(HTML_GROUP_TYPES Type, char Data, filePosition position){
            this->Type = Type;
            this->Data.push_back(Data);
            this->Position = position;
        }

        /**
         * @brief Checks if the Parsed_By contains specific bit mask.
         * @details This function takes a PARSE_BY value and checks if the bit mask of that value is set in the Parsed_By of the current token.
         * @param f The PARSE_BY value to check.
         * @return True if the bit mask of the value is set, otherwise false.
         */
        bool is(PARSE_BY f){
            return (Parsed_By & f) == f;
        }

        /**
         * @brief Checks if the Parsed_By contains specific bit mask.
         * @details This function takes a PARSE_BY value and checks if the bit mask of that value is set in the Parsed_By of the current token.
         * @param f The PARSE_BY value to check.
         * @return True if the bit mask of the value is set, otherwise false.
         */
        bool has(PARSE_BY f) {
            return (f & Parsed_By) > PARSE_BY::NONE;
        }

        /**
         * @brief Default constructor for an HTML_Token.
         * @details This constructor does not initialize any values and should be used with caution.
         */
        HTMLToken() = default;
    };

    class HTMLGroup{
    public:
        HTML_GROUP_TYPES Type = HTML_GROUP_TYPES::UNKNOWN;
        char Start = 0;
        char End = 0;
        bool Is_Sticky = true;

        /**
         * @brief Constructor for an HTML_Group.
         * @details This constructor sets the type, start and end of the group.
         * @param Type The type of the group.
         * @param Start The start of the group.
         * @param End The end of the group.
         * @param Is_Sticky Is the group sticky?
         */
        HTMLGroup(HTML_GROUP_TYPES Type, char Start, char End, bool Is_Sticky = true){
            this->Type = Type;
            this->Start = Start;
            this->End = End;
            this->Is_Sticky = Is_Sticky;
        }
    };

    class HTMLNode{
    public:
        std::string Tag_Name = "";  // DIV, HREF, etc...
        
        std::vector<HTMLNode*> Childs;
        HTMLNode* parent = nullptr;

        filePosition Position;

        HTMLToken* RAW = nullptr;
        HTML_GROUP_TYPES Type = HTML_GROUP_TYPES::UNKNOWN;

        // Postfixes are in child[0] for numbers.
        // Decimals are also number typed.
        // Operators left is Child[0] and Right at Child[1].
        // Attributes cannot be computed, before some contextual data on AST level is constructed, since the postfix operands depend on these kind of information from parent.
        std::unordered_map<std::string, GGUI::HTMLToken*> Attributes;    // contains ID, Name, Class, Color, BG_Color, etc...
    };

    /**
     * @brief Parses the HTML tokens.
     * @param Input The vector of tokens to parse.
     * 
     * This function parses the HTML tokens by combining wrappers like: <, >, (, ), etc...
     * It also captures decimals, parses operators in Reverse PEMDAS order, and combines dynamic wrappers like: <html>, </html>
     */
    extern void parse(std::vector<HTMLToken*>& Input);

    /**
     * @brief Parses raw HTML buffer into elements.
     * @param Raw_Buffer The raw HTML buffer to parse.
     * @param parent The parent element to set for top-level nodes.
     * @return A vector of parsed HTML elements.
     */
    extern std::vector<element*> parseHTML(std::string Raw_Buffer, element* parent);
    
    /**
     * @brief Parses the HTML tokens.
     * @param Input The vector of tokens to parse.
     * @return The parsed vector of HTML tokens.
     * 
     * This function parses the HTML tokens by combining wrappers like: <, >, (, ), etc...
     * It also captures decimals, parses operators in Reverse PEMDAS order, and combines dynamic wrappers like: <html>, </html>
     */
    extern std::vector<HTMLToken*>& parseHTML(std::vector<HTMLToken*>& Input);

    extern void parseEmbeddedBytes(int& i, std::vector<HTMLToken*>& Input);

    /**
     * @brief Parses all wrappers in the given vector of tokens.
     * @param i The index to start from.
     * @param Input The vector of tokens to parse.
     * 
     * This function parses all wrappers in the input, such as:
     * - <>
     * - []
     * - {}
     * - ()
     * - ""
     * - '
     */
    extern void parseAllWrappers(int& i, std::vector<HTMLToken*>& Input);

    /**
     * @brief Parses all wrappers in the given vector of tokens.
     * @param i The index to start from.
     * @param Input The vector of tokens to parse.
     * @param word The word which is used to identify the dynamic wrapper.
     * 
     * This function parses all wrappers in the input, such as:
     * - <>
     * - []
     * - {}
     * - ()
     * - ""
     * - '
     */
    extern void parseDynamicWrappers(int& i, std::vector<HTMLToken*>& Input, std::string word);

    /**
     * @brief Parses a wrapper token and all of its child tokens.
     * @param start_pattern The pattern that starts the wrapper.
     * @param end_pattern The pattern that ends the wrapper.
     * @param i The index of the start pattern in the input vector.
     * @param Input The input vector of tokens.
     * @details This function starts from the given index and every time it finds a start pattern it starts a new loop from the start pattern index until the end pattern count hits 0 and puts all of the tokens between the start and end pattern into the childs.
     *          The start pattern is not included in the child tokens.
     *          The end pattern is not included in the child tokens.
     *          The parsed by flag is set to the start pattern token.
     *          The function returns nothing, but modifies the input vector by deleting the tokens between the start and end pattern.
     *          If the start pattern is not found in the input vector, the function does nothing.
     *          If the nested count is still above 0 even after looping through all the tokens, just ignore lolw.
     */
    extern void parseWrapper(std::string start_pattern, std::string end_pattern, int& i, std::vector<HTMLToken*>& Input);

    extern const std::vector<HTMLGroup> Groups;

    /**
     * @brief Lexes the raw HTML string into a vector of HTML tokens.
     * @param Raw_Buffer The input HTML string to be tokenized.
     * @return A vector of pointers to HTML_Token objects.
     *
     * This function slices the given HTML text into tokens based on character groups.
     * It identifies different types of tokens such as text, numbers, operators, etc.
     * and returns a vector containing these tokens.
     */
    extern std::vector<HTMLToken*> lexHTML(std::string Raw_Buffer);

    /**
     * @brief Parses a vector of HTML tokens into HTML nodes.
     * @param Input A vector of pointers to HTML_Token objects to be parsed.
     * @return A vector of pointers to HTML_Node objects.
     *
     * This function processes each HTML token in the input vector, 
     * converts them into HTML nodes using the Factory function, 
     * and returns a vector containing these nodes. 
     * Only non-null nodes are added to the result vector.
     */
    extern std::vector<HTMLNode*> parseLexedTokens(std::vector<HTMLToken*> Input);

    extern std::unordered_map<std::string, std::function<GGUI::element* (HTMLNode*)>>* HTMLTranslators;

    extern std::unordered_map<std::string, double> POSTFIX_COEFFICIENT;

    extern std::unordered_map<std::string, void*> RELATIVE_COEFFICIENT;

    /**
     * @brief A simple hash function for strings.
     * @param str The string to be hashed.
     * @param h The current hash value.
     * @return The hashed string.
     * @details This function uses the djb2 hash algorithm to hash the given string.
     *          The hash value is computed in a way that is independent of the order of the characters in the string.
     *          The hash value is a 32-bit unsigned integer.
     */
    constexpr unsigned int hash(const char* str, int h = 0)
    {
        return !str[h] ? 5381 : (hash(str, h+1) * 33) ^ str[h];
    }


    // Helper macro to concatenate two tokens
    #define CONCAT_IMPL(x, y) x##y
    // Macro to concatenate two tokens, using the implementation macro
    #define CONCAT(x, y) CONCAT_IMPL(x, y)

    /**
     * @brief Macro to add a translator for custom HTML tag parsers.
     * @param id The identifier for the HTML tag.
     * @param handler The function handler for processing the HTML tag.
     * 
     * This macro creates a unique variable using the line number and counter.
     * It initializes the HTML_Translators map if it's not already initialized,
     * and inserts the provided id and handler into the map.
     */
    #define GGUIAddTranslator(id, handler) \
        auto CONCAT(CONCAT(_, __LINE__), __COUNTER__) = [](){ \
            if (GGUI::HTMLTranslators == nullptr){ \
                GGUI::HTMLTranslators = new std::unordered_map<std::string, std::function<GGUI::element* (GGUI::HTMLNode*)>>(); \
            } \
            return GGUI::HTMLTranslators->insert({id, handler}); \
        }();

    /**
     * @brief Parses a vector of HTML nodes into elements.
     * @param Input A vector of pointers to HTML_Node objects to be parsed.
     * @return A vector of pointers to Element objects.
     *
     * This function processes each HTML node in the input vector, 
     * tries to find a translator fitting for the node's tag name, 
     * and if found, runs it. If the translator returns a non-null element, 
     * the element is added to the result vector. The translator is run in a 
     * paused GGUI state, meaning all events and updates are paused. After the 
     * translator has been run, the processed node is removed from the input vector.
     */
    extern std::vector<element*> parseTranslators(std::vector<HTMLNode*>& Input);

    /**
     * @brief Converts an HTML token into an HTML node.
     * @param Input The input HTML token to be converted.
     * @return A pointer to an HTML node.
     *
     * This function processes the given HTML token and converts it into an HTML node.
     * It checks if the child is just a text and if so sets the type of the node to be text.
     * Then it iterates over each child of the token and tries to find attributes.
     * If an attribute is found, it's added to the node's attribute list.
     * If the child is not an attribute, it's converted into an HTML node using this function recursively.
     * Finally, the function returns the HTML node.
     */
    extern HTMLNode* factory(HTMLToken* Input);

    /**
     * @brief Parses a postfix for a numeric token.
     * @param i The current index in the input vector.
     * @param Input The input vector of HTML tokens to be parsed.
     * 
     * This function processes the token at the given index in the input vector.
     * It checks if the token is a number, and if so, it checks if the next token is a postfix.
     * If the next token is a postfix, it adds it to the current token as a child and marks the current token as having a postfix.
     * Then it removes the postfix token from the input vector.
     */
    extern void parseNumericPostfix(int& i, std::vector<HTMLToken*>& Input);

    /**
     * @brief Parses a decimal number token.
     * @param i The current index in the input vector.
     * @param Input The input vector of HTML tokens to be parsed.
     * 
     * This function processes the token at the given index in the input vector.
     * If the token is a decimal number, it checks if the left and right side of the decimal token are numbers.
     * If they are, it checks if the decimal number is valid by trying to convert it to a double.
     * If the decimal number is valid, it creates a new token with the decimal value and replaces the current token with it.
     * If the decimal number is invalid, it reports an error.
     */
    extern void parseDecimal(int& i, std::vector<HTMLToken*>& Input);

    /**
     * @brief Parses an operator token and its surrounding tokens.
     * @param i The current index in the input vector.
     * @param Input The input vector of HTML tokens to be parsed.
     * @param operator_type The character representing the operator.
     *
     * This function processes the token at the given index in the input vector.
     * It checks if the token is an operator and not already parsed by the operator parser.
     * If the current token matches the operator type and is not already parsed, it adds the
     * left and right tokens as children of the operator token and marks it as parsed by the operator parser.
     * The function also sets the type of the token to ATTRIBUTE and removes the left and right tokens
     * from the input vector. The index is updated accordingly.
     */
    extern void parseOperator(int& i, std::vector<HTMLToken*>& Input, char operator_type);

    /**
     * @brief Reports an error to the user.
     * @param problem The error message to display.
     * @param location The location of the error in the file.
     *
     * This function appends the location of the error to the error message and
     * calls the GGUI::Report function to display the error to the user.
     */
    extern void report(std::string problem, filePosition location);

    /**
     * @brief Converts an Element to an HTML node.
     * @param e The element to convert.
     * @return A pointer to an HTML node.
     *
     * This function creates an HTML node from the given element.
     * It sets the tag name of the node to "div" and the type to wrapper.
     * Then it adds the attributes of the element to the node.
     * The width and height of the element are added as number type attributes.
     */
    extern HTMLNode* elementToNode(element* e);

    /**
     * @brief Computes the value of a given token.
     * @param val The token to compute the value of.
     * @param parent The parent node of the token.
     * @param attr_name The name of the attribute to compute the value of.
     * @return The computed value of the token.
     *
     * This function is called by the translator to compute the value of a token.
     * It checks if the token is an operator and if so, it calls the Compute_Operator
     * function to compute the value of the operator. If the token is not an operator, it
     * checks if the token has a postfix and if so, it calls the Compute_Post_Fix_As_Coefficient
     * function to compute the coefficient of the postfix. The computed value is then returned.
     */
    extern double computeVal(HTMLToken* val, HTMLNode* parent, std::string attr_name);

    /**
     * @brief Computes the result of an operator token.
     * 
     * This function evaluates an operator token by computing the values
     * of its left and right child nodes and applying the operator to
     * those values. Supported operators are +, -, *, /, and =.
     * 
     * @param op The operator token to evaluate.
     * @param parent The parent HTML node of the token.
     * @param attr_name The name of the attribute associated with the token.
     * @return The computed result of the operator.
     */
    extern double computeOperator(HTMLToken* op, HTMLNode* parent, std::string attr_name);

    /**
     * @brief Computes the result of a postfix token as a coefficient.
     * @details This function evaluates a postfix token as a coefficient by
     *          multiplying the coefficient value with the specified attribute
     *          value of the parent node or the parent's parent node.
     *          Supported postfixes are vw, vh, %, vmin, and vmax.
     * @param postfix The postfix token to evaluate.
     * @param parent The parent HTML node of the token.
     * @param attr_name The name of the attribute associated with the token.
     * @return The computed coefficient.
     */
    extern double computePostFixAsCoefficient(std::string postfix, HTMLNode* parent, std::string attr_name);

    /**
     * @brief Translates an HTML node's attributes to an Element.
     * @param e The Element to translate the attributes to.
     * @param input The HTML node to translate the attributes from.
     *
     * This function translates the attributes of the given HTML node to the given Element.
     * It sets the Element's width and height attributes using the Compute_Val function.
     * It also sets the Element's flexbox properties if the flex-direction attribute is set.
     */
    extern void translateAttributesToElement(element* e, HTMLNode* input);

    /**
     * @brief Translates the child nodes of an HTML node to an Element.
     * @param e The Element to which the child nodes are to be translated.
     * @param input The HTML node containing the child nodes.
     * @param Set_Text_To A pointer to a string where the resulting text will be set.
     *
     * This function processes each child node of the given HTML node.
     * If a child node is of type TEXT, its tag name is added to the Raw_Text vector.
     * If a child node has a tag name "br", a newline character is added to the Raw_Text vector.
     * If a translator is available for a child node's tag name, the child node is translated 
     * to an Element, and the Element is added as a child to the parent Element.
     * Finally, the concatenated text from the Raw_Text vector is set to the provided string pointer.
     */
    extern void translateChildsToElement(element* e, HTMLNode* input, std::string* Set_Text_To);

}

#endif
#ifndef _SIMD_H_
#define _SIMD_H_



namespace GGUI{

    #if defined(__AVX512F__)
        #include <immintrin.h>

        inline const unsigned int MAX_SIMD_SIZE = 16;
    #elif defined(__AVX__)
        #include <immintrin.h>

        inline const unsigned int MAX_SIMD_SIZE = 8;
    #elif defined(__SSE__)
        #include <xmmintrin.h>

        inline const unsigned int MAX_SIMD_SIZE = 4;
    #else
        inline const unsigned int MAX_SIMD_SIZE = 1;
    #endif

    // The number represents how many 32 bit float value pairs can it calculate at the same time.
    void simdDivision4(float* a, float* b, float* c);
    void simdDivision8(float* a, float* b, float* c);
    void simdDivision16(float* a, float* b, float* c);

    // Calls the right division SIMD operator depending on the length
    void operateSIMDDivision(float* dividend, float* divider, float* result, int length){
        if(length == 4){
            simdDivision4(dividend, divider, result);
        }else if(length == 8){
            simdDivision8(dividend, divider, result);
        }else if(length == 16){
            simdDivision16(dividend, divider, result);
        }else{
            INTERNAL::reportStack("Calling SIMD division with longer sequence than allowed: " + std::to_string(length) + " elements.");
        }
    }

    void operateSIMDModulo(float* dividend, float* divider, float* result, int length){
        // Uses the division variants and then calculates for each the modulo
        operateSIMDDivision(dividend, divider, result, length);

        for(int i = 0; i < length; i++){
            // by the formula: a - b * floor(a / b)
            result[i] = dividend[i] - divider[i] * floor(result[i]);
        }
    }

    #if defined(__SSE__)
        void simdDivision4(float* a, float* b, float* c) {
            __m128 va = _mm_loadu_ps(a);
            __m128 vb = _mm_loadu_ps(b);
            __m128 vc = _mm_div_ps(va, vb);
            _mm_storeu_ps(c, vc);
        }
    #else
        void simdDivision4(float* a, float* b, float* c) {
            *c = *a / *b;
            *(c + 1) = *(a + 1) / *(b + 1);
            *(c + 2) = *(a + 2) / *(b + 2);
            *(c + 3) = *(a + 3) / *(b + 3);
        }
    #endif

    #if defined(__AVX__)
        void simdDivision8(float* a, float* b, float* c) {
            __m256 va = _mm256_loadu_ps(a);
            __m256 vb = _mm256_loadu_ps(b);
            __m256 vc = _mm256_div_ps(va, vb);
            _mm256_storeu_ps(c, vc);
        }
    #else
        void simdDivision8(float* a, float* b, float* c) {
            // use the one stage lower SIMD function variant.
            simdDivision4(a, b, c);
            simdDivision4(a + 4, b + 4, c + 4);
        }
    #endif

    #if defined(__AVX512F__)
        void simdDivision16(float* a, float* b, float* c) {
            __m512 va = _mm512_loadu_ps(a);
            __m512 vb = _mm512_loadu_ps(b);
            __m512 vc = _mm512_div_ps(va, vb);
            _mm512_storeu_ps(c, vc);
        }
    #else
        void simdDivision16(float* a, float* b, float* c) {
            // use the one stage lower SIMD function variant.
            simdDivision8(a, b, c);
            simdDivision8(a + 8, b + 8, c + 8);
        }
    #endif
}

#endif
#ifndef _PROGRESS_BAR_H_
#define _PROGRESS_BAR_H_










namespace GGUI{

    class progressStyle{
    public:
        Compact_String Head;
        Compact_String Body;
        Compact_String Tail;

        RGB Head_Color = GGUI::COLOR::LIGHT_GRAY;
        RGB Body_Color = GGUI::COLOR::GRAY;
        RGB Tail_Color = GGUI::COLOR::GRAY;

        RGB Empty_Color = GGUI::COLOR::DARK_GRAY;

        /**
         * @brief Constructor for the PROGRESS_STYLE class.
         * 
         * @param head The character to use for the head of the progress bar.
         * @param body The character to use for the body of the progress bar.
         * @param tail The character to use for the tail of the progress bar.
         * 
         * The default is to use the centered horizontal line character for all three parts of the progress bar.
         */
        progressStyle(
            const char* head = GGUI::SYMBOLS::CENTERED_HORIZONTAL_LINE.data(),
            const char* body = GGUI::SYMBOLS::CENTERED_HORIZONTAL_LINE.data(),
            const char* tail = GGUI::SYMBOLS::CENTERED_HORIZONTAL_LINE.data()
        ) : Head(head), Body(body), Tail(tail){}

        /**
         * @brief Constructor for the PROGRESS_STYLE class.
         * 
         * @param fill_color The color to use for the head, body, and tail of the progress bar.
         * @param empty_color The color to use for the empty part of the progress bar.
         * 
         * This constructor is used to create a PROGRESS_STYLE with the same color for the head, body, and tail.
         * The default is to use the centered horizontal line character for all three parts of the progress bar.
         */
        progressStyle(RGB fill_color, RGB empty_color) : progressStyle() {
            Head_Color = fill_color;
            Body_Color = fill_color;
            Tail_Color = fill_color;

            Empty_Color = empty_color;
        }

        /**
         * @brief Copy constructor for the PROGRESS_STYLE class.
         * 
         * @param other The other PROGRESS_STYLE object to copy from.
         * 
         * This constructor copies the data from the other PROGRESS_STYLE object to this one.
         */
        progressStyle(const progressStyle& other){
            // Copy data
            Head = other.Head;
            Body = other.Body;
            Tail = other.Tail;
            
            // Copy colors
            Head_Color = other.Head_Color;
            Body_Color = other.Body_Color;
            Tail_Color = other.Tail_Color;

            Empty_Color = other.Empty_Color;
        }

        // Copy assignment operator
        progressStyle& operator=(const progressStyle& other) {
            if (this != &other) {  // Protect against self-assignment
                // Copy data
                Head = other.Head;
                Body = other.Body;
                Tail = other.Tail;

                // Copy colors
                Head_Color = other.Head_Color;
                Body_Color = other.Body_Color;
                Tail_Color = other.Tail_Color;

                Empty_Color = other.Empty_Color;
            }
            return *this;
        }

    };

    namespace progressBarStyles{
        extern progressStyle Default;
        extern progressStyle Blocky;
        extern progressStyle Arrow;
    }

    class progressBar : public element{
    protected:
        float Progress = 0; // 0.0 - 1.0

        progressStyle Progress_Style;

        std::vector<UTF> Content;
    public:

        /**
         * @brief Constructor for Progress_Bar.
         *
         * This constructor calls the Element constructor with the given style and
         * initializes the Progress_Bar object with default values.
         *
         * @param s The style for the Progress_Bar.
         * @param Embed_Styles_On_Construct If true, the styling will be embedded into the Progress_Bar's style. Only use if you know what you're doing!!!
         */
        progressBar(styling s, bool Embed_Styles_On_Construct = false) : element(s, Embed_Styles_On_Construct){
            Progress = 0.0f;
            Progress_Style = progressBarStyles::Default;
        }

        /**
         * @brief Default constructor for Progress_Bar.
         *
         * This constructor is explicitly defined as default, which means that the compiler will generate a default implementation for it.
         * This is needed because otherwise, the compiler would not generate a default constructor for this class, since we have a user-declared constructor.
         */
        progressBar() = default;

        /**
         * @brief Returns the index of the head of the progress bar.
         * @details
         * This function returns the index of the head of the progress bar. The head is the character that is drawn at the end of the progress bar when it is not full.
         * The index is calculated by multiplying the width of the progress bar (minus the border on both sides) by the progress value.
         * The result is then rounded down to the nearest integer using the floor() function.
         * @return The index of the head of the progress bar.
         */
        unsigned int getIndexofHead();

        /**
         * @brief Colors the bar with the current progress value.
         * @details
         * This function colors the progress bar with the current progress value. It first colors the empty part of the bar, then fills in the progressed part, and finally replaces the head and tail parts.
         */
        void colorBar();

        /**
         * @brief Renders the progress bar into the Render_Buffer.
         * @details This function processes the progress bar to generate a vector of UTF objects representing the current state.
         * It handles different stains such as CLASS, STRETCH, COLOR, EDGE, and DEEP to ensure the progress bar is rendered correctly.
         * @return A vector of UTF objects representing the rendered progress bar.
         */
        std::vector<GGUI::UTF>& render() override;

        /**
         * @brief Sets the progress value of the progress bar.
         * @details This function updates the progress value of the progress bar. 
         * If the given value exceeds 1.0, a warning is reported, and the function returns without updating.
         * It also updates the color of the progress bar and marks the render buffer as dirty.
         * @param New_Progress The new progress value to set (should be between 0.0 and 1.0).
         */
        void setProgress(float New_Progress);
        
        /**
         * @brief Returns the current progress value of the progress bar.
         * @details This function returns the current progress value of the progress bar, which is a float between 0.0 and 1.0.
         * @return The current progress value of the progress bar.
         */
        float getProgress();

        /**
         * @brief Toggles the border visibility of the progress bar.
         * @details This function toggles the border visibility of the progress bar.
         *          If the state has changed, it updates the border enabled state, marks the element as dirty for border changes, and updates the frame.
         * @param b The desired state of the border visibility.
         */
        void showBorder(bool state) override;

        /**
         * @brief Destructor for the Progress_Bar class.
         *
         * This destructor is responsible for properly deallocating all the memory
         * allocated by the Progress_Bar object. It calls the base class destructor
         * to ensure all parent class resources are also cleaned up.
         */
        ~progressBar() override {
            // Call the base destructor to clean up base class resources.
            element::~element();
        }
        
        /**
         * @brief Creates a deep copy of the Progress_Bar object.
         * @details This function creates a new Progress_Bar object and copies all the data from the current Progress_Bar object to the new one.
         *          This is useful for creating a new Progress_Bar object that is a modified version of the current one.
         * @return A pointer to the new Progress_Bar object.
         */
        element* safeMove() override {
            return new progressBar();
        }

        /**
         * @brief Returns the name of the Progress_Bar object.
         * @details This function returns a string that represents the name of the Progress_Bar object.
         *          The name is constructed by concatenating the name of the Progress_Bar with the 
         *          class name "Progress_Bar", separated by a "<" and a ">".
         * @return The name of the Progress_Bar object.
         */
        std::string getName() const override{
            return "Progress_Bar<" + Name + ">";
        }
    };

}

#endif
#ifndef _UTILS_H_
#define _UTILS_H_
/**
 * This is an Utils file made for the Renderer.cpp to use internally, these are just removed to clean up the source code.
 */



namespace GGUI{

    class element;
    
    namespace INTERNAL{
        // This class contains carry flags from previous cycle cross-thread, if another thread had some un-finished things when another thread was already running.
        class Carry{
        public:
            bool Resize = false;
            bool Terminate = false;     // Signals the shutdown of subthreads.

            ~Carry() = default;
        };
    }

    /**
     * @brief Converts an unsigned long long integer to its uppercase hexadecimal string representation.
     * 
     * This function takes an unsigned long long integer and formats it as a hexadecimal string
     * in uppercase. The resulting string does not include a "0x" prefix.
     * 
     * @param value The unsigned long long integer to be converted to a hexadecimal string.
     * @return A std::string containing the uppercase hexadecimal representation of the input value.
     */
    extern std::string Hex(unsigned long long value);

    /**
     * @brief Checks if two rectangles collide.
     *
     * This function determines whether two rectangles, defined by their top-left
     * corners and dimensions, overlap in a 2D space.
     *
     * @param A The top-left corner of the first rectangle as a GGUI::IVector3.
     * @param B The top-left corner of the second rectangle as a GGUI::IVector3.
     * @param A_Width The width of the first rectangle.
     * @param A_Height The height of the first rectangle.
     * @param B_Width The width of the second rectangle.
     * @param B_Height The height of the second rectangle.
     * @return true if the rectangles overlap, false otherwise.
     */
    extern bool Collides(GGUI::IVector3 A, GGUI::IVector3 B, int A_Width = 1, int A_Height = 1, int B_Width = 1, int B_Height = 1);


    /**
     * @brief Checks if two GGUI elements collide.
     * 
     * This function determines whether two GGUI elements, `a` and `b`, collide with each other.
     * If the elements are the same (i.e., `a` is equal to `b`), the function returns the value of `Identity`.
     * Otherwise, it checks for collision based on the absolute positions and dimensions of the elements.
     * 
     * @param a Pointer to the first GGUI element.
     * @param b Pointer to the second GGUI element.
     * @param Identity Boolean value to return if the elements are the same.
     * @return true if the elements collide, false otherwise.
     */
    extern bool Collides(GGUI::element* a, GGUI::element* b, bool Identity = true);

    /**
     * @brief Checks if a given point collides with a specified element.
     * 
     * This function determines if the point `b` collides with the element `a` by 
     * calling another `Collides` function with the element's absolute position, 
     * width, height, and the point's assumed dimensions of 1x1.
     * 
     * @param a Pointer to the GGUI::Element to check for collision.
     * @param b The point (as GGUI::IVector3) to check for collision with the element.
     * @return true if the point collides with the element, false otherwise.
     */
    extern bool Collides(GGUI::element* a, GGUI::IVector3 b);

    /**
     * @brief Recursively finds the most accurate element that contains the given position.
     * 
     * This function checks if the given position is within the bounds of the parent element.
     * If it is, it then checks all the child elements of the parent to see if any of them
     * contain the position. If a child element contains the position, the function is called
     * recursively on that child element. If no child element contains the position, the parent
     * element is returned.
     * 
     * @param c The position to check, represented as an IVector3.
     * @param Parent The parent element to start the search from.
     * @return Element* The most accurate element that contains the given position, or nullptr if the position is not within the bounds of the parent element.
     */
    extern element* Get_Accurate_Element_From(IVector3 c, element* Parent);

    /**
     * @brief Finds the upper element relative to the current element's position.
     * 
     * This function retrieves the current element based on the mouse position and 
     * attempts to find an element directly above it by moving one pixel up. If an 
     * upper element is found and it is not the main element, the position of the 
     * upper element is returned. Otherwise, the position of the current element is returned.
     * 
     * @return IVector3 The position of the upper element if found, otherwise the position of the current element.
     */
    extern IVector3 Find_Upper_Element();

    /**
     * @brief Finds the lower element relative to the current element.
     * 
     * This function retrieves the current element based on the mouse position
     * and then attempts to find an element that is positioned directly below it.
     * 
     * @return IVector3 The position of the lower element if found, otherwise the position of the current element.
     */
    extern IVector3 Find_Lower_Element();

    /**
     * @brief Finds the element to the left of the current element.
     *
     * This function retrieves the current element based on the mouse position
     * and attempts to find an element one pixel to the left of it. If such an
     * element is found, its position is returned. If no left element is found,
     * the position of the current element is returned.
     *
     * @return IVector3 The position of the left element if found, otherwise the position of the current element.
     */
    extern IVector3 Find_Left_Element();

    /**
     * @brief Finds the element to the right of the current element.
     * 
     * This function first retrieves the current element based on the mouse position
     * and the main internal context. If the current element is found, it calculates
     * the position of the element to the right by moving one pixel to the right of
     * the current element's position. It then attempts to retrieve the element at
     * this new position.
     * 
     * @return IVector3 The position of the element to the right if found, otherwise
     *                  the position of the current element.
     */
    extern IVector3 Find_Right_Element();

    /**
     * @brief Returns the smaller of two signed long long integers.
     * 
     * This function compares two signed long long integers and returns the smaller of the two.
     * 
     * @param a The first signed long long integer to compare.
     * @param b The second signed long long integer to compare.
     * @return The smaller of the two signed long long integers.
     */
    extern signed long long Min(signed long long a, signed long long b);

    /**
     * @brief Returns the maximum of two signed long long integers.
     *
     * This function compares two signed long long integers and returns the greater of the two.
     *
     * @param a The first signed long long integer to compare.
     * @param b The second signed long long integer to compare.
     * @return The greater of the two signed long long integers.
     */
    extern signed long long Max(signed long long a, signed long long b);

    /**
     * @brief Checks if a bit is set in a char.
     * @details This function takes a char and an index as input and checks if the bit at the specified index is set.
     *          It returns true if the bit is set and false if it is not.
     *
     * @param val The char to check the bit in.
     * @param i The index of the bit to check.
     *
     * @return True if the bit is set, false if it is not.
     */
    extern bool Has_Bit_At(char val, int i);

    /**
     * @brief Gets the contents of a given position in the buffer.
     * @details This function takes a position in the buffer and returns the contents of that position. If the position is out of bounds, it will return nullptr.
     * @param Absolute_Position The position to get the contents of.
     * @return The contents of the given position, or nullptr if the position is out of bounds.
     */
    extern GGUI::UTF* Get(GGUI::IVector3 Absolute_Position);

    /**
     * @brief Calculates the current load of the GGUI thread based on the given current position.
     * @param Min The minimum value the load can have.
     * @param Max The maximum value the load can have.
     * @param Position The current position of the load.
     * @return The current load of the GGUI thread from 0 to 1.
     */
    extern float Lerp(int Min, int Max, int Position);

    /**
     * @brief Checks if the given flag is set in the given flags.
     * @details This function takes two unsigned long long parameters, one for the flags and one for the flag to check. It returns true if the flag is set in the flags, otherwise it returns false.
     *
     * @param f The flags to check.
     * @param Flag The flag to check for.
     * @return True if the flag is set, otherwise false.
     */
    extern bool Is(unsigned long long f, unsigned long long Flag);

    /**
     * @brief Checks if a flag is set in a set of flags.
     * @details This function takes two unsigned long long parameters, one for the flags and one for the flag to check. It returns true if the flag is set in the flags, otherwise it returns false.
     *
     * @param f The flags to check.
     * @param flag The flag to check for.
     * @return True if the flag is set, otherwise false.
     */
    extern bool Has(unsigned long long f, unsigned long long flag);

    /**
     * @brief Checks if all flags in small are set in big.
     * @details This function takes two unsigned long long parameters, one for the flags to check and one for the flags to check against. It returns true if all flags in small are set in big, otherwise it returns false.
     *
     * @param big The flags to check against.
     * @param small The flags to check.
     * @return True if all flags in small are set in big, otherwise false.
     */
    extern bool Contains(unsigned long long big, unsigned long long Small);

    /**
     * @brief Determines if a given pointer is likely deletable (heap-allocated).
     *
     * This function assesses whether a pointer may belong to the heap by comparing its
     * position relative to known memory sections such as the stack, heap, and data segments.
     *
     * @param ptr Pointer to be evaluated.
     * @return True if the pointer is likely deletable (heap-allocated), false otherwise.
     */
    extern bool isDeletable(void* ptr);
}

#endif
#ifndef _SWITCH_H_
#define _SWITCH_H_

#include <vector>
#include <string>










namespace GGUI{
    class switchBox : public element{
    private:
        /**
         * @brief Private default constructor.
         * @details This constructor is not intended to be used by the user.
         * It is used to prevent the compiler from generating a default constructor.
         */
        // DONT GIVE TO USER !!!
        switchBox(){}
    protected:
        bool State = false;

        //COntains the unchecked version of the symbol and the checked version.
        std::vector<std::string> States;

        textField Text;
    public:
        /**
         * @brief Constructs a Switch element with specified text, states, event handler, and styling.
         * @param text The text to display on the switch.
         * @param states A vector containing the unchecked and checked states.
         * @param event The function to call when the switch is toggled.
         * @param s The styling for the switch.
         * @param Embed_Styles_On_Construct If true, the styling will be embedded into the switch's style. Only use if you know what you're doing!!!
         */
        switchBox(
            std::string text,
            std::vector<std::string> states,
            std::function<void (element* This)> event = []([[maybe_unused]] element* e){}, 
            styling s = STYLES::CONSTANTS::Default,
            bool Embed_Styles_On_Construct = false
        );

        /**
         * @brief Renders the switch element and its children into the Render_Buffer nested buffer of the window.
         * @details This function processes the switch element to generate a vector of UTF objects representing the current state.
         * It handles different stains such as CLASS, STRETCH, COLOR, EDGE, and DEEP to ensure the switch element is rendered correctly.
         * @return A vector of UTF objects representing the rendered switch element.
         */
        std::vector<GGUI::UTF>& render() override;

        /**
         * @brief Toggles the state of the switch.
         * @details Flips the current state from checked to unchecked or vice versa,
         * and marks the switch as needing a state update.
         */
        void toggle() {
            // Flip the current state of the switch
            State = !State;

            // Mark the switch as needing a state update
            Dirty.Dirty(STAIN_TYPE::STATE);
        }

        /**
         * @brief Sets the text of the switch element.
         * @details This function sets the text of the switch element by first pausing the GGUI engine, then setting the text with a space character added to the beginning, and finally updating the switch element's dimensions to fit the new text. The text is then reset in the Render_Buffer nested buffer of the window.
         * @param text The new text for the switch element.
         */
        void setText(std::string text);
        
        /**
         * @brief Creates a deep copy of the Switch object.
         * @details This function creates a new Switch object and copies all the data from the current Switch object to the new one.
         *          This is useful for creating a new Switch object that is a modified version of the current one.
         * @return A pointer to the new Switch object.
         */
        element* safeMove() override {
            return new switchBox();
        }

        /**
         * @brief Returns the name of the Switch object.
         * @details This function returns a string that represents the name of the Switch object.
         *          The name is constructed by concatenating the name of the Switch with the 
         *          class name "Switch", separated by a "<" and a ">".
         * @return The name of the Switch object.
         */
        std::string getName() const override{
            return "Switch<" + Name + ">";
        }
    };

    class radioButton : public switchBox{
    public:
        /**
         * @brief Constructs a Radio_Button object with the specified text.
         * @details A Radio_Button is a special type of Switch that can be either on or off.
         *          The text parameter is the text to display next to the radio button.
         * @param text The text to display next to the radio button.
         */
        radioButton(std::string text) : switchBox(text, {SYMBOLS::RADIOBUTTON_OFF, SYMBOLS::RADIOBUTTON_ON}){}

        /**
         * @brief Destructor for the Radio_Button class.
         * @details This destructor is responsible for properly deallocating all the memory
         * allocated by the Radio_Button object. It calls the base class destructor
         * to ensure all parent class resources are also cleaned up.
         */
        ~radioButton() override{
            // call the base destructor.
            element::~element();
        }

        /**
         * @brief Returns the state of the Radio_Button.
         * @details This function returns a boolean value indicating whether the Radio_Button is turned on or off.
         *          The state is represented by the Switch::State property.
         * @return The state of the Radio_Button.
         */
        bool getState(){
            return State;
        }
        
        // The Swtich overrides it for us.
        //Element* Safe_Move() override;
        
        /**
         * @brief Returns the name of the Radio_Button object.
         * @details This function returns a string that represents the name of the Radio_Button object.
         *          The name is constructed by concatenating the name of the Radio_Button with the 
         *          class name "Radio_Button", separated by a "<" and a ">".
         * @return The name of the Radio_Button object.
         */
        std::string getName() const override{
            // Return the formatted name of the Radio_Button.
            return "Radio_Button<" + Name + ">";
        }
    };

    class checkBox : public switchBox{
    public:
        /**
         * @brief Constructs a Check_Box object with the specified text.
         * @param text The text to display next to the check box.
         * @details A Check_Box is a special type of Switch that can be either checked or unchecked.
         *          The symbols for the unchecked and checked states are EMPTY_CHECK_BOX and CHECKED_CHECK_BOX, respectively.
         */
        checkBox(std::string text) : switchBox(text, {SYMBOLS::EMPTY_CHECK_BOX, SYMBOLS::CHECKED_CHECK_BOX}){}

        /**
         * @brief Returns the current state of the Radio_Button.
         * @details This function returns a boolean indicating whether the Radio_Button is on or off.
         * @return The state of the Radio_Button.
         */
        bool getState(){
            return State; // Return the current state of the Radio_Button.
        }
        
        // The Swtich overrides it for us.
        //Element* Safe_Move() override;

        /**
         * @brief Returns the name of the Check_Box object.
         * @details This function returns a string that represents the name of the Check_Box object.
         *          The name is constructed by concatenating the name of the Check_Box with the 
         *          class name "Check_Box", separated by a "<" and a ">".
         * @return The name of the Check_Box object.
         */
        std::string getName() const override{
            return "Check_Box<" + Name + ">";
        }
    };
}

#endif
#ifndef _THREAD_H_
#define _THREAD_H_

namespace GGUI{
    namespace INTERNAL{
        
        /**
         * @brief The Renderer function is responsible for managing the rendering loop.
         * It waits for a condition to resume rendering, processes rendering tasks, and
         * then pauses itself until the condition is met again.
         * 
         * The function performs the following steps:
         * 1. Waits for the render thread to be resumed.
         * 2. Saves the current time.
         * 3. Checks if the rendering scheduler needs to be terminated.
         * 4. Processes carry flags and updates the maximum width and height if needed.
         * 5. Renders the main frame buffer.
         * 6. Encodes the buffer for optimization.
         * 7. Converts the abstract frame buffer to a string and renders the frame.
         * 8. Calculates the render delay.
         * 9. Pauses the render thread and notifies all waiting threads.
         */
        extern void renderer();

        /**
         * @brief Event_Thread is a function that runs an infinite loop to handle various events and tasks.
         * 
         * This function performs the following tasks in each iteration of the loop:
         * - Resets the thread load counter and updates the previous time.
         * - Calls functions to recall memories, go through file streams, and refresh the multi-frame canvas.
         * - Checks for termination signals and breaks out of the loop if the terminate flag is set.
         * - Updates the current time and calculates the delta time.
         * - Adjusts the current update speed based on the event thread load.
         * - Sleeps for a calculated duration to control the update speed.
         * 
         * The function is designed to be used in a multi-threaded environment where it can be paused and resumed as needed.
         * 
         * @note If uncapped FPS is desired, the sleep code can be disabled.
         */
        extern void eventThread();

        /**
         * @brief Function that continuously handles user input in a separate thread.
         *
         * This function runs an infinite loop where it performs the following steps:
         * 1. Waits for user input by calling INTERNAL::Query_Inputs().
         * 2. Pauses the GGUI system and performs the following actions:
         *    - Records the current time as INTERNAL::Previous_Time.
         *    - Translates the queried inputs using INTERNAL::Translate_Inputs().
         *    - Processes scroll and mouse inputs using SCROLL_API() and MOUSE_API().
         *    - Calls the event handlers to react to the parsed input using Event_Handler().
         *    - Records the current time as INTERNAL::Current_Time.
         *    - Calculates the delta time (input delay) and stores it in INTERNAL::Input_Delay.
         */
        extern void inputThread();
    }
}

#endif
