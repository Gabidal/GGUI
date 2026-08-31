#ifndef _TYPES_H_
#define _TYPES_H_

#include <string>
#include <cassert>
#include <span>
#include <vector>

namespace GGUI{
    
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

        constexpr IVector2 operator*(int num) const noexcept {
            return IVector2(x * num, y * num); // Multiply each coordinate by num
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

        constexpr bool operator <(const IVector2& other) const noexcept {
            return (y < other.y) || (y == other.y && x < other.x); // Compare y first, then x if y is equal
        }

        /**
         * @brief Converts the IVector2 to a string
         *
         * Converts the IVector2 to a string representation.
         *
         * @return A string representation of the IVector2.
         */
        constexpr std::string toString() const {
            return "(" + std::to_string(x) + ", " + std::to_string(y) + ")";
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

        constexpr IVector3(IVector2 lower) : IVector2(lower), z(0) {}  // Initialize z to 0 by default

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
        std::string To_String() const {
            return std::to_string(x) + ", " + std::to_string(y) + ", " + std::to_string(z);
        }
    };

    class rectangle {
    public:
        IVector3 position;
        IVector2 size;
        
        constexpr rectangle(IVector3 pos = {}, IVector2 Size = {}) : position(pos), size(Size) {}

        constexpr bool empty() const { return size.x == 0 && size.y == 0; }

        constexpr int left() const { return position.x; }
        constexpr int right() const { return position.x + size.x; }
        constexpr int top() const { return position.y; }
        constexpr int bottom() const { return position.y + size.y; }

        constexpr std::array<IVector2, 4> getCorners() const {
            return {
                IVector2(left(), top()),
                IVector2(right(), top()),
                IVector2(left(), bottom()),
                IVector2(right(), bottom())
            };
        }

        // returns the left and right sides for each row
        std::vector<IVector2> getVerticalFaces() const {
            int start = top();
            int end = bottom();

            std::vector<IVector2> result;
            result.resize((end - start) * 2); // left + right

            for (int y = start; y < end; ++y) {
                result[(y - start) * 2]     = IVector2(left(), y);
                result[(y - start) * 2 + 1] = IVector2(right(), y);
            }

            return result;
        }

        constexpr bool intersects(const rectangle& other) const {
            return !(
                right() <= other.left()    ||
                left()  >= other.right()   ||
                bottom()<= other.top()     ||
                top()   >= other.bottom()
            );
        }

        constexpr bool hits(IVector2 point) const {
            return point.x >= left() && point.x < right() &&
                   point.y >= top()  && point.y < bottom();
        }

        constexpr rectangle intersection(rectangle other) const {
            int x1 = std::max(left(), other.left());
            int y1 = std::max(top(), other.top());

            int x2 = std::min(right(), other.right());
            int y2 = std::min(bottom(), other.bottom());

            if (x1 >= x2 || y1 >= y2)
                return {};

            return rectangle(
                IVector3(x1, y1, 0),
                IVector2(x2 - x1, y2 - y1)
            );
        }
    };

    namespace types{
        struct fittingArea{
            IVector2 negativeOffset;
            IVector2 start;
            IVector2 end;
        };

        enum class borderConnection : uint8_t {
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

        enum class STAIN_TYPE : uint16_t {
            CLEAN = 0,              // No change
            GRAPHICS = 1 << 0,      // BG and other color related changes
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

        template<typename enumType, typename containerType = std::underlying_type_t<enumType>>
        class bitMask {
            static_assert(std::is_enum_v<enumType>, "enumType must be an enum type for bitMask");
        private:
            containerType data;
        public:

            constexpr bitMask() {
                clear();
            }

            template<typename T>
            constexpr bitMask(T initialFlag) {
                static_assert(std::is_same_v<T, enumType> || std::is_same_v<T, containerType>, "bitMask can only be initialized with enumType or containerType.");

                data = static_cast<containerType>(initialFlag);
            }

            constexpr bool has(enumType flags) const {
                return (data & static_cast<containerType>(flags)) == static_cast<containerType>(flags);
            }

            constexpr void set(enumType flags, bool value = true) {
                data = (
                    value ? 
                    (data | static_cast<containerType>(flags)) :
                    (data & ~static_cast<containerType>(flags))
                );
            }

            // returns each activated enum
            constexpr std::vector<enumType> getAll() const {
                std::vector<enumType> result;

                for (containerType i = 0; i < sizeof(containerType) * 8; ++i) {
                    containerType _bitMask = static_cast<containerType>(1) << i;
                    if ((data & _bitMask) != 0) {
                        result.push_back(static_cast<enumType>(_bitMask));
                    }
                }

                return result;
            }

            constexpr void clear() {
                data = static_cast<containerType>(0);
            }

            constexpr bool operator==(const bitMask<enumType, containerType>& other) const {
                return data == other.data;
            }

            constexpr bool operator!=(const bitMask<enumType, containerType>& other) const {
                return data != other.data;
            }

            constexpr bitMask<enumType, containerType> operator|(const bitMask<enumType, containerType>& other) const {
                return data | other.data;
            }

            constexpr bitMask<enumType, containerType> operator&(const bitMask<enumType, containerType>& other) const {
                return data & other.data;
            }

            constexpr bitMask<enumType, containerType>& operator|=(const bitMask<enumType, containerType>& other) {
                data |= other.data;
                return *this;
            }

            template<typename P>
            constexpr bitMask<enumType, containerType>& operator=(P value) {
                static_assert(std::is_same_v<P, enumType> || std::is_same_v<P, containerType>, "Assignment is only allowed for enumType or containerType.");

                data = static_cast<containerType>(value);
                return *this;
            }
        };

        template<typename enumType, typename containerType = std::underlying_type_t<enumType>>
        class linearMask {
            static_assert(std::is_enum_v<enumType>, "enumType must be an enum type for bitMask!");
            static_assert(
                static_cast<containerType>(enumType::__min) < static_cast<containerType>(enumType::__max) &&
                static_cast<containerType>(enumType::DEFAULT) >= static_cast<containerType>(enumType::__min) &&
                static_cast<containerType>(enumType::DEFAULT) <= static_cast<containerType>(enumType::__max), 
                "__min, __max and DEFAULT values must be defined!"
            );
            static_assert(std::is_unsigned_v<containerType>, "Underlying enum's container type must be unsigned!");
            static_assert(
                static_cast<containerType>(enumType::__max) <= std::numeric_limits<containerType>::digits, 
                "__max overflows containerType max bit!"
            );
        protected:
            containerType data;

            constexpr containerType toBitMask(enumType t) const {
                if (t <= enumType::__min) return 0;
    
                return static_cast<containerType>(1) << (static_cast<containerType>(t) - 1);
            }
        public:
            constexpr linearMask(enumType initValue = enumType::DEFAULT) : data(toBitMask(initValue)) {}

            constexpr void add(enumType t) {
                containerType tAsBitMask = toBitMask(t);

                if (tAsBitMask == 0) data = 0;   // If default, then reset all
                else data |= tAsBitMask;
            }
            
            constexpr void remove(enumType t) {
                containerType tAsBitMask = toBitMask(t);

                if (tAsBitMask != 0) data &= ~tAsBitMask;   // If not default, then remove the bit
            }

            constexpr void set(enumType t, bool val) {
                if (val) add(t);
                else remove(t);
            }

            constexpr bool has(enumType t) const {
                containerType tAsBitMask = toBitMask(t);

                return  (tAsBitMask == data) ||     // Either they are identical, 0 == 0
                        (data & tAsBitMask) != 0;   // Or atleast bits from t are present
            }

            constexpr bool has(const linearMask& other) const {
                return (data & other.data) == other.data;
            }

            constexpr std::vector<enumType> getAll() const {
                std::vector<enumType> result;

                for (containerType i = static_cast<containerType>(enumType::__min); i <= static_cast<containerType>(enumType::__max); ++i) {
                    containerType bitMask = static_cast<containerType>(1) << (i - 1);
                    if ((data & bitMask) != 0) {
                        result.push_back(static_cast<enumType>(i));
                    }
                }

                return result;
            }

            constexpr size_t getBitCount() const {
                size_t count = 0;
                containerType tempData = data;

                while (tempData) {
                    count += tempData & 1;
                    tempData >>= 1;
                }

                return count;
            }

            constexpr bool operator==(const linearMask& other) const {
                return data == other.data;
            }

            constexpr linearMask operator|(const linearMask& other) const {
                linearMask result;
                result.data = data | other.data;
                return result;
            }

            constexpr linearMask operator&(const linearMask& other) const {
                linearMask result;
                result.data = data & other.data;
                return result;
            }

            constexpr linearMask operator|(const enumType value) const {
                linearMask result;
                result.data = data | toBitMask(value);
                return result;
            }

            constexpr linearMask operator&(const enumType value) const {
                linearMask result;
                result.data = data & toBitMask(value);
                return result;
            }

            constexpr containerType getData() const { return data; }

            template<typename otherEnumType, typename OtherContainerType = std::underlying_type_t<otherEnumType>>
            constexpr linearMask operator=(const linearMask<otherEnumType, OtherContainerType>& other) {
                static_assert(  // Make sure that the incoming type is same or atleast smaller
                    (std::is_same_v<OtherContainerType, containerType>) ||
                    (sizeof(OtherContainerType) <= sizeof(containerType)),
                    "Only injection of container type is allowed!"
                );

                static_assert(  // Make sure that __min and __max are either aligned or injectable
                    (static_cast<containerType>(otherEnumType::__min) >= static_cast<containerType>(enumType::__min)) &&
                    (static_cast<containerType>(otherEnumType::__max) <= static_cast<containerType>(enumType::__max)),
                    "Only injection of enum type is allowed!"
                );

                // DEFAULT values is not needed to be checked.

                data = other.getData();
                return *this;
            }
        };

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

        enum class STATE : uint8_t {
            UNKNOWN,

            INIT,
            DESTROYED,
            HIDDEN,
            SHOWN
        };
    }

    template<typename T, typename P>
    concept eligibleForWriterViewType = (
        std::is_convertible_v<P, T> ||
        (
            std::is_enum_v<P> &&
            std::is_convertible_v<std::underlying_type_t<P>, T>
        )
    );

    template<typename T>
    class writerView {
        T* data;
        size_t capacity;
        size_t size;
    public:
        /**
         * @brief Constructs a writer view over a pre-allocated buffer.
         *
         * @param preAllocated A span representing the available storage for writing.
         */
        constexpr writerView(const std::span<T>& preAllocated) : data(preAllocated.data()), capacity(preAllocated.size()), size(0) {
            assert(data != nullptr && "Data pointer cannot be null!");
            assert(capacity > 0 && "Preallocated area is empty!");
        }

        /**
         * @brief Writes a single value to the buffer.
         *
         * @param value The value to write.
         */
        template<typename P>
        requires eligibleForWriterViewType<T, P>
        constexpr void write(const P& value) {
            assert(size < capacity && "Cannot write value beyond allocated capacity!");

            data[size++] = static_cast<T>(value);
        }

        /**
         * @brief Writes a sequence of values to the buffer.
         *
         * @param values A span of values to write.
         */
        template<typename P> 
        requires eligibleForWriterViewType<T, P>
        constexpr void write(std::span<P> values) {
            assert(size + values.size() <= capacity && "Cannot write values beyond allocated capacity!");

            for (const auto& value : values) {
                data[size++] = static_cast<T>(value);
            }
        }

        /**
         * @brief Requests the remaining free space in the buffer.
         *
         * @return A writer view over the unfilled portion of the underlying storage.
         */
        [[nodiscard]] constexpr writerView<T> requestFree() const {
            return std::span<T>(data + size, capacity - size);
        }

        /**
         * @brief Commits a previously requested write region.
         *
         * @param requested The requested region to finalize.
         */
        constexpr void commit(const std::span<T>& requested) {
            // First we need to check for validity
            assert(requested.data() >= data && requested.data() + requested.size() <= data + capacity && "Requested span is out of bounds!");
            assert(requested.data() == data + size && "Requested span is out of order!");

            size += requested.size();
        }

        /**
         * @brief Clears all written data from the view.
         */
        constexpr void clear() {
            size = 0;
        }

        /**
         * @brief Returns the number of elements currently written.
         *
         * @return The current element count.
         */
        constexpr size_t getSize() const { return size; }

        /**
         * @brief Returns the total capacity of the underlying buffer.
         *
         * @return The maximum number of elements that can be written.
         */
        constexpr size_t getCapacity() const { return capacity; }

        /**
         * @brief Returns the written data as a span.
         *
         * @return A span covering the current valid data range.
         */
        [[nodiscard]] constexpr std::span<T> getWritten() const { return std::span<T>(data, size); }

        [[nodiscard]] constexpr std::span<T> getStorage() const { return std::span<T>(data, capacity); } 
    };
}

#endif