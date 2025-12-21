#ifndef _TYPES_H_
#define _TYPES_H_

#include <string>
#include <functional>
#include <chrono>
#include <mutex>
#include <memory>

#include "constants.h"
#include "color.h"

namespace GGUI{
    
    // Literal type
    class FVector2{
    public:
        float x = 0;
        float y = 0;

        /**
         * @brief Default constructor
         *
         * Initializes the FVector2 with the given x and y values.
         *
         * @param x The x-coordinate. Default is 0.0f.
         * @param y The y-coordinate. Default is 0.0f.
         */
        constexpr FVector2(float X = 0.0f, float Y = 0.0f) noexcept : x(X), y(Y) {}

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
            return FVector2(x + num, y + num);
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
            return FVector2(x - num, y - num);
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
            return FVector2(x * num, y * num);
        }
    };
    
    // Literal type
    class FVector3 : public FVector2 {
    public:
        float z = 0;

        /**
         * @brief Default constructor
         *
         * Initializes the FVector3 with the given x, y, and z values.
         *
         * @param x The x-coordinate. Default is 0.0f.
         * @param y The y-coordinate. Default is 0.0f.
         * @param z The z-coordinate. Default is 0.0f.
         */
        constexpr FVector3(float X = 0.0f, float Y = 0.0f, float Z = 0.0f) noexcept : FVector2(X, Y), z(Z) {}

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
            return FVector3(x + num, y + num, z + num);
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
            return FVector3(x - num, y - num, z - num);
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
            return FVector3(x * num, y * num, z * num);
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
            return FVector3(x + other.x, y + other.y, z + other.z);
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
            return FVector3(x - other.x, y - other.y, z - other.z);
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
            return FVector3(x * other.x, y * other.y, z * other.z);
        }
    };

    class IVector2{
    public:
        int x = 0;  //Horizontal
        int y = 0;  //Vertical

        /**
         * @brief Default constructor
         *
         * Initializes the IVector2 with the given x and y values.
         *
         * @param x The x-coordinate. Default is 0.
         * @param y The y-coordinate. Default is 0.
         */
        constexpr IVector2(int X = 0, int Y = 0) noexcept : x(X), y(Y) {}

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
            x += other->x;
            y += other->y;
        }

        /**
         * @brief += operator with an FVector2
         *
         * Adds the values of the FVector2 to this IVector2.
         *
         * @param other The FVector2 to add.
         */
        constexpr void operator+=(FVector2 other) noexcept {
            x += static_cast<int>(other.x);
            y += static_cast<int>(other.y);
        }

        /**
         * @brief += operator with another IVector2
         *
         * Adds the values of another IVector2 to this one.
         *
         * @param other The IVector2 to add.
         */
        constexpr void operator+=(IVector2 other) noexcept {
            x += other.x;  // Add the x-coordinate
            y += other.y;  // Add the y-coordinate
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
            return IVector2(x + other.x, y + other.y);
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
            return IVector2(x - other.x, y - other.y);
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
            return IVector2(static_cast<int>(x * num), static_cast<int>(y * num)); // Multiply each coordinate by num
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
            return x == other.x && y == other.y; // Check if the coordinates are equal
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
            return x != other.x || y != other.y; // Check if the coordinates are not equal
        }

        /**
         * @brief Converts the IVector2 to a string
         *
         * Converts the IVector2 to a string representation.
         *
         * @return A string representation of the IVector2.
         */
        std::string To_String() const {
            return std::to_string(x) + ", " + std::to_string(y);
        }
    };

    class IVector3 : public IVector2{
    public:
        int z = 0;  //priority (the higher the more likely it will be at top).

        /**
         * @brief Default constructor
         *
         * Initializes the IVector3 with the given x, y and z values.
         *
         * @param x The x-coordinate. Default is 0.
         * @param y The y-coordinate. Default is 0.
         * @param z The z-coordinate. Default is 0.
         */
        constexpr IVector3(int X = 0, int Y = 0, int Z = 0) noexcept : IVector2(X, Y), z(Z) {}

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
            x += other->x;
            y += other->y;
            z += other->z;
        }

        /**
         * @brief += operator with another IVector3
         *
         * Adds the values of another IVector3 to this one.
         *
         * @param other The IVector3 to add.
         */
        constexpr void operator+=(IVector3 other) noexcept {
            x += other.x;  // Add the x-coordinate
            y += other.y;  // Add the y-coordinate
            z += other.z;  // Add the z-coordinate
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
            return IVector3(x + other.x, y + other.y, z + other.z);
        }

        constexpr IVector3 operator-(const IVector3& other) const noexcept {
            return IVector3(x - other.x, y - other.y, z - other.z);
        }

        constexpr IVector3 operator+(int constant) const noexcept {
            return IVector3(x + constant, y + constant, z + constant); // Add the constant to each coordinate
        }

        constexpr IVector3 operator-(int constant) const noexcept {
            return IVector3(x - constant, y - constant, z - constant); // Subtract the constant from each coordinate
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
            return IVector3(static_cast<int>(x * num), static_cast<int>(y * num), static_cast<int>(z * num)); // Multiply each coordinate by num
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
            return x == other.x && y == other.y && z == other.z; // Check if the coordinates are equal
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
            return x != other.x || y != other.y || z != other.z; // Check if the coordinates are not equal
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
            return std::to_string(x) + ", " + std::to_string(y) + ", " + std::to_string(z);
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
            return std::to_string(x) + ", " + std::to_string(y) + ", " + std::to_string(z);
        }
    };


    class event{
    public:
        unsigned long long criteria;
    };

    class input : public event{
    public:
        unsigned short x = 0;
        unsigned short y = 0;
        char data = 0;

        // The input information like the character written.
        input(char d, unsigned long long t){
            data = d;
            criteria = t;
        }

        input(IVector3 c, unsigned long long t){
            x = (unsigned short )c.x;
            y = (unsigned short )c.y;
            criteria = t;
        }
    };

    class action : public event{
    public:
        class element* host = nullptr;

        std::function<bool(GGUI::event*)> Job;
        
        std::string ID; 
    
        action() = default;
        action(unsigned long long Criteria, std::function<bool(GGUI::event*)> job, std::string id){
            criteria = Criteria;
            Job = job;
            host = nullptr;
            ID = id;
        }

        action(unsigned long long Criteria, std::function<bool(GGUI::event*)> job, class element* Host, std::string id){
            criteria = Criteria;
            Job = job;
            host = Host;
            ID = id;
        }
    };

    namespace MEMORY_FLAGS{
        inline unsigned char PROLONG_MEMORY     = 1 << 0;
        inline unsigned char RETRIGGER          = 1 << 1;
    }

    class memory : public action{
    public:
        std::chrono::high_resolution_clock::time_point startTime;
        size_t endTime = 0;

        // By default all memories automatically will not prolong each other similar memories.
        unsigned char flags = 0x0;

        // When the job starts, job, prolong previous similar job by this time.
        memory(size_t end, std::function<bool(GGUI::event*)>job, unsigned char Flags = 0x0, std::string id = ""){
            startTime = std::chrono::high_resolution_clock::now();
            endTime = end;
            Job = job;
            flags = Flags;
            ID = id;
        }

        bool is(const unsigned char f) const{
            return (flags & f) > 0;
        }

        void set(const unsigned char f){
            flags |= f;
        }
    };

    namespace INTERNAL{
        struct fittingArea{
            IVector2 negativeOffset;
            IVector2 start;
            IVector2 end;
        };

        enum class borderConnection{
            NONE    = 0 << 0,
            UP      = 1 << 0,
            DOWN    = 1 << 1,
            LEFT    = 1 << 2,
            RIGHT   = 1 << 3
        };

        constexpr bool operator==(const borderConnection lhs, const borderConnection rhs) {
            return static_cast<int>(lhs) == static_cast<int>(rhs);
        }

        constexpr borderConnection operator|(const borderConnection lhs, const borderConnection rhs) {
            return static_cast<borderConnection>(static_cast<int>(lhs) | static_cast<int>(rhs));
        }

        constexpr void operator|=(borderConnection& lhs, const borderConnection rhs) {
            lhs = static_cast<borderConnection>(static_cast<int>(lhs) | static_cast<int>(rhs));
        }

        enum class STAIN_TYPE{
            CLEAN = 0,              // No change
            COLOR = 1 << 0,         // BG and other color related changes
            EDGE = 1 << 1,          // Title and border changes.
            DEEP = 1 << 2,          // Children changes. Deep because the childs are connected via AST.
            STRETCH = 1 << 3,       // Width and or height changes.
            STATE = 1 << 4,         // This is for Switches that based on their state display one symbol differently.
            MOVE = 1 << 5,          // Enabled, to signal absolute position caching.
            FINALIZE = 1 << 6,      // This is used to signal that the element is finalized and the stylings are successfully been embedded.
            RESET = 1 << 7,         // This is to make stretch less slaggy flagging.
            NOT_RENDERED = 1 << 8,  // This is a single time flag, noting when the first render pass occurs for the element, triggers On_Render().
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

        namespace LOGGER{
            extern void log(std::string Text);
        }

        namespace atomic{
            template<typename T>
            class guard {
            public:
                std::mutex shared; // Mutex to guard shared data
                std::unique_ptr<T> data;

                /**
                 * @brief Constructs a Guard object and initializes its Data member.
                 * 
                 * This constructor creates a unique pointer to an instance of type T
                 * and assigns it to the Data member of the Guard object.
                 */
                guard() : data(std::make_unique<T>()) {}

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
                    std::lock_guard<std::mutex> lock(shared); // Automatically manages mutex locking and unlocking
                    try {
                        job(*data);
                    } catch (...) {
                        INTERNAL::LOGGER::log("Failed to execute the function!");
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
                T read() {
                    std::lock_guard<std::mutex> lock(shared);
                    return *data;
                }

                /**
                 * @brief Destructor for the Guard class.
                 *
                 * This destructor ensures that the Data object is properly destroyed
                 * by acquiring a lock on the Shared mutex before resetting the Data.
                 * The use of std::lock_guard ensures that the mutex is automatically
                 * released when the destructor exits, preventing potential deadlocks.
                 */
                ~guard() {
                    std::lock_guard<std::mutex> lock(shared);
                    data.reset(); // Ensures proper destruction
                }
            };   
        }
    
        // This class contains carry flags from previous cycle cross-thread, if another thread had some un-finished things when another thread was already running.
        class carry{
        public:
            bool resize = false;
            bool terminate = false;     // Signals the shutdown of subthreads.

            ~carry() = default;
        };
    }
}

#endif