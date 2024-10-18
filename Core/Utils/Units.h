#ifndef _UNITS_H_
#define _UNITS_H_

#include <string>

#include "Constants.h"
#include "Color.h"

namespace GGUI{
    
    // Literal type
    class FVector2{
    public:
        float X = 0;
        float Y = 0;

        // Default constructor
        constexpr FVector2(float x = 0.0f, float y = 0.0f) noexcept
            : X(x), Y(y) {}

        // Copy constructor
        constexpr FVector2(const FVector2& other) noexcept = default;

        // Move constructor
        constexpr FVector2(FVector2&& other) noexcept = default;

        // Copy assignment operator
        constexpr FVector2& operator=(const FVector2& other) noexcept = default;

        // Move assignment operator
        constexpr FVector2& operator=(FVector2&& other) noexcept = default;

        // + operator with a float
        constexpr FVector2 operator+(float num) const noexcept {
            return FVector2(X + num, Y + num);
        }

        // - operator with a float
        constexpr FVector2 operator-(float num) const noexcept {
            return FVector2(X - num, Y - num);
        }

        // * operator with a float
        constexpr FVector2 operator*(float num) const noexcept {
            return FVector2(X * num, Y * num);
        }
    };
    
    // Literal type
    class FVector3 : public FVector2 {
    public:
        float Z = 0;

        // Default constructor
        constexpr FVector3(float x = 0.0f, float y = 0.0f, float z = 0.0f) noexcept
            : FVector2(x, y), Z(z) {}

        // Copy constructor
        constexpr FVector3(const FVector3& other) noexcept = default;

        // Move constructor
        constexpr FVector3(FVector3&& other) noexcept = default;

        // Copy assignment operator
        constexpr FVector3& operator=(const FVector3& other) noexcept = default;

        // Move assignment operator
        constexpr FVector3& operator=(FVector3&& other) noexcept = default;

        // + operator with a float
        constexpr FVector3 operator+(float num) const noexcept {
            return FVector3(X + num, Y + num, Z + num);
        }

        // - operator with a float
        constexpr FVector3 operator-(float num) const noexcept {
            return FVector3(X - num, Y - num, Z - num);
        }

        // * operator with a float
        constexpr FVector3 operator*(float num) const noexcept {
            return FVector3(X * num, Y * num, Z * num);
        }

        // + operator with another FVector3
        constexpr FVector3 operator+(const FVector3& other) const noexcept {
            return FVector3(X + other.X, Y + other.Y, Z + other.Z);
        }

        // - operator with another FVector3
        constexpr FVector3 operator-(const FVector3& other) const noexcept {
            return FVector3(X - other.X, Y - other.Y, Z - other.Z);
        }

        // * operator with another FVector3 (component-wise multiplication)
        constexpr FVector3 operator*(const FVector3& other) const noexcept {
            return FVector3(X * other.X, Y * other.Y, Z * other.Z);
        }
    };

    class IVector3{
    public:
        int X = 0;  //Horizontal
        int Y = 0;  //Vertical
        int Z = 0;  //priority (the higher the more likely it will be at top).

        // Default constructor
        constexpr IVector3(int x = 0, int y = 0, int z = 0) noexcept
            : X(x), Y(y), Z(z) {}

        // Copy constructor
        constexpr IVector3(const IVector3& other) noexcept = default;

        // Move constructor
        constexpr IVector3(IVector3&& other) noexcept = default;

        // Copy assignment operator
        constexpr IVector3& operator=(const IVector3& other) noexcept = default;

        // Move assignment operator
        constexpr IVector3& operator=(IVector3&& other) noexcept = default;

        // += operator for IVector2 pointer
        constexpr void operator+=(IVector3* other) noexcept {
            X += other->X;
            Y += other->Y;
            Z += other->Z;
        }

        // += operator for FVector2
        constexpr void operator+=(FVector2 other) noexcept {
            X += other.X;
            Y += other.Y;
        }

        // += operator for IVector2
        constexpr void operator+=(IVector3 other) noexcept {
            X += other.X;
            Y += other.Y;
            Z += other.Z;
        }

        // + operator for IVector2
        constexpr IVector3 operator+(const IVector3& other) const noexcept {
            return IVector3(X + other.X, Y + other.Y, Z + other.Z);
        }

        constexpr IVector3 operator*(float num) const noexcept {
            return IVector3(X * num, Y * num, Z * num);
        }

        std::string To_String(){
            return std::to_string(X) + ", " + std::to_string(Y) + ", " + std::to_string(Z);
        }
    
        // To_String function (cannot be constexpr due to std::string)
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

        constexpr UTF(const GGUI::UTF& other) : FLAGS(other.FLAGS), Ascii(other.Ascii), Unicode(other.Unicode), Unicode_Length(other.Unicode_Length), Foreground(other.Foreground), Background(other.Background){}

        // {Foreground, Background}
        UTF(char data, std::pair<RGB, RGB> color = {{}, {}}){
            Ascii = data;
            Foreground = {color.first};
            Background = {color.second};
            FLAGS = UTF_FLAG::IS_ASCII;
        }

        UTF(const char* data, std::pair<RGB, RGB> color = {{}, {}}){
            Unicode = data;
            Unicode_Length = std::strlen(data);
            
            Foreground = {color.first};
            Background = {color.second};
            FLAGS = UTF_FLAG::IS_UNICODE;
        }

        UTF(const std::string& data, std::pair<RGB, RGB> color = {{}, {}}){
            Unicode = data.data();
            Unicode_Length = data.size() - 1;
            
            Foreground = {color.first};
            Background = {color.second};
            FLAGS = UTF_FLAG::IS_UNICODE;
        }

        UTF(const Compact_String CS, std::pair<RGB, RGB> color = {{}, {}}){
            if (CS.Size == 1){
                Ascii = CS.Data.Ascii_Data;
                Foreground = {color.first};
                Background = {color.second};
                FLAGS = UTF_FLAG::IS_ASCII;
            }
            else{
                Unicode = CS.Data.Unicode_Data;
                Unicode_Length = CS.Size;
                Foreground = {color.first};
                Background = {color.second};
                FLAGS = UTF_FLAG::IS_UNICODE;
            }
        }

        bool Is(unsigned char utf_flag){
            // Check if the bit mask contains the bits
            return (FLAGS & utf_flag) > 0;
        }

        void Set_Flag(unsigned char utf_flag){
            FLAGS |= utf_flag;
        }

        void Set_Foreground(RGB color){
            Foreground = color;
        }

        void Set_Background(RGB color){
            Background = color;
        }

        void Set_Color(std::pair<RGB, RGB> primals){
            Foreground = primals.first;
            Background = primals.second;
        }

        void Set_Text(std::string data){
            Unicode = data.data();
            Unicode_Length = data.size() -1;
            FLAGS = UTF_FLAG::IS_UNICODE;
        }

        void Set_Text(char data){
            Ascii = data;
            FLAGS = UTF_FLAG::IS_ASCII;
        }

        void Set_Text(const char* data){
            Unicode = data;
            Unicode_Length = std::strlen(data);
            FLAGS = UTF_FLAG::IS_UNICODE;
        }

        void Set_Text(UTF other){
            Ascii = other.Ascii;
            Unicode = other.Unicode;
            Unicode_Length = other.Unicode_Length;
            FLAGS = other.FLAGS;
        }

        std::string To_String();
        std::string To_Encoded_String();    // For UTF Strip Encoding.

        // Needs Result to be initalized with Maximum_Needed_Pre_Allocation_For_Super_String at max.
        void To_Super_String(GGUI::Super_String* Result, Super_String* Text_Overhead, Super_String* Background_Overhead, Super_String* Text_Colour, Super_String* Background_Colour);
        
        // Needs Result to be initalized with Maximum_Needed_Pre_Allocation_For_Super_String at max.
        void To_Encoded_Super_String(Super_String* Result, Super_String* Text_Overhead, Super_String* Background_Overhead, Super_String* Text_Colour, Super_String* Background_Colour);

        void operator=(char text){
            Set_Text(text);
        }

        void operator=(const std::string& text){
            Set_Text(text);
        }

        UTF& operator=(const UTF& other){
            Ascii = other.Ascii;
            Unicode = other.Unicode;
            Unicode_Length = other.Unicode_Length;
            FLAGS = other.FLAGS;
            Foreground = other.Foreground;
            Background = other.Background;

            return *this;
        }

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
 
    inline unsigned int operator|(STAIN_TYPE a, STAIN_TYPE b){
        return (unsigned int)a | (unsigned int)b;
    }

    inline unsigned int operator|(STAIN_TYPE a, unsigned int b){
        return (unsigned int)a | b;
    }

    inline unsigned int operator|(unsigned int a, STAIN_TYPE b){
        return a | (unsigned int)b;
    }

    class STAIN{
    public:
        STAIN_TYPE Type = STAIN_TYPE::CLEAN;

        bool is(STAIN_TYPE f){
            if (f == STAIN_TYPE::CLEAN){
                return Type <= f;
            }
            return ((unsigned int)Type & (unsigned int)f) == (unsigned int)f;
        }

        void Clean(STAIN_TYPE f){
            Type = (STAIN_TYPE)((unsigned int)Type & ~(unsigned int)f);
        }

        void Clean(unsigned int f){
            Type = (STAIN_TYPE)((unsigned int)Type & ~f);
        }

        void Dirty(STAIN_TYPE f){
            Type = (STAIN_TYPE)((unsigned int)Type | (unsigned int)f);
        }

        void Dirty(unsigned int f){
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
    
    inline Flags operator|(Flags a, Flags b){
        return static_cast<Flags>(static_cast<int>(a) | static_cast<int>(b));
    }

    inline bool Is(Flags a, Flags b){
        return ((int)a & (int)b) == (int)b;
    }

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