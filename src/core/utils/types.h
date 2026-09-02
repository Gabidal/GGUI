#ifndef _TYPES_H_
#define _TYPES_H_

#include <string>
#include <cassert>
#include <span>
#include <vector>
#include <cstdint>
#include <type_traits>
#include <array>

namespace GGUI{
    template <typename targetT, size_t targetDIM, typename otherT, size_t otherDIM>
    concept injectableVector = (
        otherDIM <= targetDIM &&      // Injection is allowed
        (
            std::is_arithmetic_v<otherT> ||     // This allows conversion between int<->float
            (
                std::is_enum_v<otherT> &&       // -or enum class inheriting the same type
                std::is_same_v<std::underlying_type_t<otherT>, targetT>
            )
        )
    );

    template<typename T, size_t DIM>
    requires (std::is_default_constructible_v<T> && std::is_arithmetic_v<T>)
    struct NVector {
        std::array<T, DIM> axis = {};    // X_0, X_1, X_2, ..., X_n-1

        static constexpr size_t dimensions = DIM;

        template<typename otherT = T, size_t otherDIM = DIM>
        requires injectableVector<T, DIM, otherT, otherDIM>
        constexpr NVector(const std::array<otherT, otherDIM>& values = {}) {
            for (size_t i = 0; i < otherDIM; i++) axis[i] = static_cast<T>(values[i]);
        }

        template<typename... otherTs>
        requires injectableVector<T, DIM, std::common_type_t<otherTs...>, sizeof...(otherTs)>
        constexpr NVector(otherTs... values) : axis{ static_cast<T>(values)... } {}

        // Copy constructor
        template<typename otherT = T, size_t otherDIM = DIM>
        requires injectableVector<T, DIM, otherT, otherDIM>
        constexpr NVector(const NVector<otherT, otherDIM>& other) : NVector(other.axis) {}

        // Move constructor
        template<typename otherT = T, size_t otherDIM = DIM>
        requires injectableVector<T, DIM, otherT, otherDIM>
        constexpr NVector(NVector<otherT, otherDIM>&& other) : NVector(other.axis) {}

        // Surjection helper
        template<size_t otherDIM = DIM, typename otherT = T>    // templates prioritize size since that is the common cause for surjection
        requires injectableVector<T, DIM, otherT, otherDIM>
        constexpr NVector<otherT, otherDIM> surjection() const {
            NVector<otherT, otherDIM> result;
            for (size_t i = 0; i < otherDIM; i++) result.axis[i] = static_cast<otherT>(axis[i]);
            return result;
        }

        // These operators use the constructor conversions stated above so they don't need to use requirement checks.
        // ===-===-===-===-===-===-===-===-===-===-===-===-===-===-===-===

        // += operator
        constexpr NVector& operator+=(const NVector& other) {
            for (size_t i = 0; i < DIM; i++) axis[i] += other.axis[i];
            return *this;
        }

        // + operator
        constexpr NVector operator+(const NVector& other) const {
            NVector result = *this;
            result += other;
            return result;
        }

        // -= operator
        constexpr NVector& operator-=(const NVector& other) {
            for (size_t i = 0; i < DIM; i++) axis[i] -= other.axis[i];
            return *this;
        }

        // - operator
        constexpr NVector operator-(const NVector& other) const {
            NVector result = *this;
            result -= other;
            return result;
        }

        // *= operator with scalar
        constexpr NVector& operator*=(const T& scalar) {
            for (size_t i = 0; i < DIM; i++) axis[i] *= scalar;
            return *this;
        }

        // *= operator with vector
        constexpr NVector& operator*=(const NVector& other) {
            for (size_t i = 0; i < DIM; i++) axis[i] *= other.axis[i];
            return *this;
        }

        // * operator with scalar
        constexpr NVector operator*(const T& scalar) const {
            NVector result = *this;
            result *= scalar;
            return result;
        }

        // * operator with vector
        constexpr NVector operator*(const NVector& other) const {
            NVector result = *this;
            result *= other;
            return result;
        }

        // ==, !=, <, <=, >, and >= operator
        constexpr auto operator<=>(const NVector&) const = default;

        // ===-===-===-===-===-===-===-===-===-===-===-===-===-===-===-===
        
        std::string toString() const {
            std::string result = "(";
            for (size_t i = 0; i < DIM; i++) {
                result += std::to_string(axis[i]);
                if (i < DIM - 1) result += ", ";
            }
            result += ")";
            return result;
        }
        
        // Axis accessors
        // ===-===-===-===-===-===-===-===-===-===-===-===-===-===-===-===

        // 1D+ Accessors
        constexpr T& x() requires (DIM >= 1) { return axis[0]; }
        constexpr const T& x() const requires (DIM >= 1) { return axis[0]; }

        // 2D+ Accessors
        constexpr T& y() requires (DIM >= 2) { return axis[1]; }
        constexpr const T& y() const requires (DIM >= 2) { return axis[1]; }

        // 3D+ Accessors
        constexpr T& z() requires (DIM >= 3) { return axis[2]; }
        constexpr const T& z() const requires (DIM >= 3) { return axis[2]; }
    };

    using IVector2 = NVector<int16_t, 2>;
    using IVector3 = NVector<int16_t, 3>;
    using FVector2 = NVector<float, 2>;
    using FVector3 = NVector<float, 3>;

    class rectangle {
    public:
        IVector3 position;
        IVector2 size;
        
        constexpr rectangle(IVector3 pos = {}, IVector2 Size = {}) : position(pos), size(Size) {}

        constexpr bool empty() const { return size.x() == 0 && size.y() == 0; }

        constexpr int left() const { return position.x(); }
        constexpr int right() const { return position.x() + size.x(); }
        constexpr int top() const { return position.y(); }
        constexpr int bottom() const { return position.y() + size.y(); }

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
            return point.x() >= left() && point.x() < right() &&
                   point.y() >= top()  && point.y() < bottom();
        }

        constexpr rectangle intersection(const rectangle& other) const {
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

    template<typename enumType, typename containerType = std::underlying_type_t<enumType>>
    class bitMask {
        static_assert(std::is_enum_v<enumType>, "enumType must be an enum type for bitMask");
    protected:
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

        constexpr bool is(enumType flags) const {
            return (data & static_cast<containerType>(flags)) == static_cast<containerType>(flags);
        }

        constexpr bool is(const bitMask<enumType, containerType>& other) const {
            return (data & other.data) == other.data;
        }

        constexpr bool has(enumType flags) const {
            return (data & static_cast<containerType>(flags)) != 0;
        }

        constexpr bool has(const bitMask<enumType, containerType>& other) const {
            return (data & static_cast<containerType>(other.data)) != 0;
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

        constexpr bool isEmpty() const {
            return data == static_cast<containerType>(0);
        }

        constexpr bool operator==(const bitMask<enumType, containerType>& other) const {
            return data == other.data;
        }

        constexpr bool operator!=(const bitMask<enumType, containerType>& other) const {
            return data != other.data;
        }

        constexpr bitMask<enumType, containerType> operator|(const bitMask<enumType, containerType>& other) const {
            return static_cast<containerType>(data | other.data);
        }

        constexpr bitMask<enumType, containerType> operator|(enumType other) const {
            return static_cast<containerType>(data | static_cast<containerType>(other));
        }

        constexpr bitMask<enumType, containerType> operator&(const bitMask<enumType, containerType>& other) const {
            return static_cast<containerType>(data & other.data);
        }

        // removes all flags from other
        constexpr bitMask<enumType, containerType>& operator^=(const bitMask<enumType, containerType>& other) {
            data ^= other.data;
            return *this;
        }

        constexpr bitMask<enumType, containerType>& operator^=(enumType other) {
            data ^= static_cast<containerType>(other);
            return *this;
        }

        constexpr bitMask<enumType, containerType>& operator|=(const bitMask<enumType, containerType>& other) {
            data |= other.data;
            return *this;
        }

        constexpr bitMask<enumType, containerType>& operator|=(enumType other) {
            data |= static_cast<containerType>(other);
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

    namespace stain {
        enum class types : uint16_t {
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

        struct base : public bitMask<stain::types> {
            using bitMask<stain::types>::bitMask;  // Inherit operators

            base() : bitMask<stain::types>() {}
        };

    }

    enum class STATE : uint8_t {
        UNKNOWN,

        INIT,
        DESTROYED,
        HIDDEN,
        SHOWN
    };

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