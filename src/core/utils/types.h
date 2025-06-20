#ifndef _TYPES_H_
#define _TYPES_H_

#include <string>
#include <functional>
#include <chrono>
#include <mutex>
#include <memory>

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

    class IVector2{
    public:
        short X = 0;  //Horizontal
        short Y = 0;  //Vertical

        /**
         * @brief Default constructor
         *
         * Initializes the IVector2 with the given x and y values.
         *
         * @param x The x-coordinate. Default is 0.
         * @param y The y-coordinate. Default is 0.
         */
        constexpr IVector2(short x = 0, short y = 0) noexcept
            : X(x), Y(y) {}

        /**
         * @brief Copy constructor
         * 
         * Initializes the IVector2 by copying another IVector2.
         * @param other The IVector2 to copy.
         */
        constexpr IVector2(const IVector2& other) noexcept = default;

        /**
         * @brief Move constructor
         * 
         * Initializes the IVector2 by moving another IVector2.
         * @param other The IVector2 to move.
         */
        constexpr IVector2(IVector2&& other) noexcept = default;

        /**
         * @brief Copy assignment operator
         *
         * Assigns another IVector2 to this one by copying its values.
         *
         * @param other The IVector2 to copy.
         * @return A reference to this IVector2.
         */
        constexpr IVector2& operator=(const IVector2& other) noexcept = default;

        /**
         * @brief Move assignment operator
         *
         * Moves the values from another IVector2 to this one.
         *
         * @param other The IVector2 to move.
         * @return A reference to this IVector2.
         */
        constexpr IVector2& operator=(IVector2&& other) noexcept = default;

        /**
         * @brief += operator with a pointer to an IVector2
         *
         * Adds the values of the IVector2 pointed to by the pointer to this IVector2.
         *
         * @param other The pointer to the IVector2 to add.
         */
        constexpr void operator+=(IVector2* other) noexcept {
            X += other->X;
            Y += other->Y;
        }

        /**
         * @brief += operator with an FVector2
         *
         * Adds the values of the FVector2 to this IVector2.
         *
         * @param other The FVector2 to add.
         */
        constexpr void operator+=(FVector2 other) noexcept {
            X += static_cast<short>(other.X);
            Y += static_cast<short>(other.Y);
        }

        /**
         * @brief += operator with another IVector2
         *
         * Adds the values of another IVector2 to this one.
         *
         * @param other The IVector2 to add.
         */
        constexpr void operator+=(IVector2 other) noexcept {
            X += other.X;  // Add the x-coordinate
            Y += other.Y;  // Add the y-coordinate
        }

        /**
         * @brief + operator with another IVector2
         *
         * Creates a new IVector2 with the added values of this IVector2 and the other IVector2.
         *
         * @param other The IVector2 to add.
         * @return A new IVector2 with the added values.
         */
        constexpr IVector2 operator+(const IVector2& other) const noexcept {
            return IVector2(X + other.X, Y + other.Y);
        }

        /**
         * @brief - operator with another IVector2
         *
         * Creates a new IVector2 with the subtracted values of this IVector2 and the other IVector2.
         *
         * @param other The IVector2 to subtract.
         * @return A new IVector2 with the subtracted values.
         */
        constexpr IVector2 operator-(const IVector2& other) const noexcept {
            return IVector2(X - other.X, Y - other.Y);
        }

        /**
         * @brief * operator with a float
         *
         * Multiplies the IVector2 by a float, creating a new IVector2.
         *
         * @param num The float to multiply.
         * @return A new IVector2 with the multiplied float.
         */
        constexpr IVector2 operator*(float num) const noexcept {
            return IVector2(static_cast<short>(X * num), static_cast<short>(Y * num)); // Multiply each coordinate by num
        }

        /**
         * @brief == operator with another IVector2
         * 
         * Compares the IVector2 with another IVector2.
         * 
         * @param other The IVector2 to compare with.
         * @return True if the IVector2s are equal, otherwise false.
         */
        constexpr bool operator==(const IVector2& other) const noexcept {
            return X == other.X && Y == other.Y; // Check if the coordinates are equal
        }

        /**
         * @brief != operator with another IVector2
         * 
         * Compares the IVector2 with another IVector2.
         * 
         * @param other The IVector2 to compare with.
         * @return False if the IVector2s are equal, otherwise true.
         */
        constexpr bool operator!=(const IVector2& other) const noexcept {
            return X != other.X || Y != other.Y; // Check if the coordinates are not equal
        }

        /**
         * @brief Converts the IVector2 to a string
         *
         * Converts the IVector2 to a string representation.
         *
         * @return A string representation of the IVector2.
         */
        std::string To_String() const {
            return std::to_string(X) + ", " + std::to_string(Y);
        }
    };

    class IVector3 : public IVector2{
    public:
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
            : IVector2(x, y), Z(z) {}

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

        constexpr IVector3 operator-(const IVector3& other) const noexcept {
            return IVector3(X - other.X, Y - other.Y, Z - other.Z);
        }

        constexpr IVector3 operator+(int constant) const noexcept {
            return IVector3(X + constant, Y + constant, Z + constant); // Add the constant to each coordinate
        }

        constexpr IVector3 operator-(int constant) const noexcept {
            return IVector3(X - constant, Y - constant, Z - constant); // Subtract the constant from each coordinate
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

    struct fittingArea{
        IVector2 negativeOffset;
        IVector2 start;
        IVector2 end;
    };

    enum class Border_Connection{
        NONE    = 0 << 0,
        UP      = 1 << 0,
        DOWN    = 1 << 1,
        LEFT    = 1 << 2,
        RIGHT   = 1 << 3
    };

    constexpr bool operator==(const Border_Connection lhs, const Border_Connection rhs) {
        return static_cast<int>(lhs) == static_cast<int>(rhs);
    }

    constexpr Border_Connection operator|(const Border_Connection lhs, const Border_Connection rhs) {
        return static_cast<Border_Connection>(static_cast<int>(lhs) | static_cast<int>(rhs));
    }

    constexpr void operator|=(Border_Connection& lhs, const Border_Connection rhs) {
        lhs = static_cast<Border_Connection>(static_cast<int>(lhs) | static_cast<int>(rhs));
    }

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
        Action(unsigned long long criteria, std::function<bool(GGUI::Event*)> job, std::string id){
            Criteria = criteria;
            Job = job;
            Host = nullptr;
            ID = id;
        }

        Action(unsigned long long criteria, std::function<bool(GGUI::Event*)> job, class element* host, std::string id){
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

    enum class STAIN_TYPE{
        CLEAN = 0,              // No change
        COLOR = 1 << 0,         // BG and other color related changes
        EDGE = 1 << 1,          // Title and border changes.
        DEEP = 1 << 2,          // Children changes. Deep because the childs are connected via AST.
        STRETCH = 1 << 3,       // Width and or height changes.
        STATE = 1 << 4,         // This is for Switches that based on their state display one symbol differently.
        MOVE = 1 << 5,          // Enabled, to signal absolute position caching.
        FINALIZE = 1 << 6,      // This is used to signal that the element is finalized and the stylings are successfully been embedded.
        RESET = 1 << 7,         // This is to remove redundant STRETCH flagging.
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
    constexpr unsigned int operator|(const STAIN_TYPE a, const STAIN_TYPE b) {
        // Cast both STAIN_TYPE values to unsigned integers and perform the bitwise OR operation.
        return static_cast<unsigned int>(a) | static_cast<unsigned int>(b);
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
    constexpr unsigned int operator|(const STAIN_TYPE a, const unsigned int b){
        return static_cast<unsigned int>(a) | b;
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
    constexpr unsigned int operator|(const unsigned int a, const STAIN_TYPE b){
        return a | static_cast<unsigned int>(b);
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
        constexpr bool is(const STAIN_TYPE f) const {
            // Special handling for the CLEAN flag
            if (f == STAIN_TYPE::CLEAN) {
                return Type <= f;
            }
            // Check if the specified flag is set using bitwise AND
            return (static_cast<unsigned int>(Type) & static_cast<unsigned int>(f)) == static_cast<unsigned int>(f);
        }

        constexpr bool has(const unsigned int f) const {
            return (static_cast<unsigned int>(Type) & static_cast<unsigned int>(f)) != 0;
        }

        /**
         * @brief Clears a STAIN_TYPE flag from the current STAIN object.
         * @details This function clears a given STAIN_TYPE flag from the current
         *          STAIN object. It performs a bitwise AND operation with the
         *          bitwise compliment of the specified flag.
         *
         * @param f The STAIN_TYPE flag to clear.
         */
        constexpr void Clean(const STAIN_TYPE f){
            Type = (STAIN_TYPE)(static_cast<unsigned int>(Type) & ~static_cast<unsigned int>(f));
        }

        /**
         * @brief Clears a STAIN_TYPE flag from the current STAIN object.
         * @details This function clears a given STAIN_TYPE flag from the current
         *          STAIN object. It performs a bitwise AND operation with the
         *          bitwise compliment of the specified flag.
         *
         * @param f The STAIN_TYPE flag to clear.
         */
        constexpr void Clean(const unsigned int f){
            Type = (STAIN_TYPE)(static_cast<unsigned int>(Type) & ~f);
        }

        /**
         * @brief Marks the specified STAIN_TYPE flag as dirty.
         * @details This function sets a given STAIN_TYPE flag on the current
         *          STAIN object, indicating that the element needs to be reprocessed
         *          for the specified attributes.
         *
         * @param f The STAIN_TYPE flag to set.
         */
        constexpr void Dirty(const STAIN_TYPE f) {
            // Set the specified flag using bitwise OR
            Type = (STAIN_TYPE)(static_cast<unsigned int>(Type) | static_cast<unsigned int>(f));
        }

        /**
         * @brief Marks the specified STAIN_TYPE flag as dirty.
         * @details This function sets a given STAIN_TYPE flag on the current
         *          STAIN object, indicating that the element needs to be reprocessed
         *          for the specified attributes.
         *
         * @param f The STAIN_TYPE flag to set.
         */
        constexpr void Dirty(const unsigned int f){
            // Set the specified flag using bitwise OR
            Type = (STAIN_TYPE)(static_cast<unsigned int>(Type) | f);
        }

    };

    enum class ENCODING_FLAG{
        NONE        = 0 << 0,
        START       = 1 << 0,
        END         = 1 << 1
    };

    constexpr bool operator== (ENCODING_FLAG& a, const ENCODING_FLAG& b) {
        return static_cast<unsigned char>(a) == static_cast<unsigned char>(b);
    }

    constexpr void operator|= (ENCODING_FLAG& a, const ENCODING_FLAG& b) {
        a = static_cast<ENCODING_FLAG>(static_cast<unsigned char>(a) | static_cast<unsigned char>(b));
    }

    constexpr ENCODING_FLAG operator&(const ENCODING_FLAG& a, const ENCODING_FLAG& b) {
        return static_cast<ENCODING_FLAG>(static_cast<unsigned char>(a) & static_cast<unsigned char>(b));
    }

    constexpr ENCODING_FLAG operator|(const ENCODING_FLAG& a, const ENCODING_FLAG& b) {
        return static_cast<ENCODING_FLAG>(static_cast<unsigned char>(a) | static_cast<unsigned char>(b));
    }

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

    enum class ALLOCATION_TYPE{
        UNKNOWN         = 0 << 0,
        STACK           = 1 << 0,
        HEAP            = 1 << 1,
        DATA            = 1 << 2
    };

    namespace INTERNAL{
        namespace LOGGER{
            extern void Log(std::string Text);
        }

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
                        INTERNAL::LOGGER::Log("Failed to execute the function!");
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
    
        // This class contains carry flags from previous cycle cross-thread, if another thread had some un-finished things when another thread was already running.
        class Carry{
        public:
            bool Resize = false;
            bool Terminate = false;     // Signals the shutdown of subthreads.

            ~Carry() = default;
        };
    }
}

#endif