#ifndef _STYLE_H_
#define _STYLE_H_

#include "Units.h"
#include <variant>

namespace GGUI{
    // Externies
    class Element;
    extern void Report_Stack(std::string Problem);

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
        template<typename T>
        class value {
        protected:
            std::variant<T, float> data;  // Can hold either normal value or percentage
            EVALUATION_TYPE evaluation_type = EVALUATION_TYPE::DEFAULT;

        public:
            constexpr value(T value, EVALUATION_TYPE type, [[maybe_unused]] bool use_constexpr)
                : data(value), evaluation_type(type) {}

            constexpr value(float value, EVALUATION_TYPE type, [[maybe_unused]] bool use_constexpr)
                : data(value), evaluation_type(type) {}

            value(T value, EVALUATION_TYPE type = EVALUATION_TYPE::DEFAULT)
                : data(value), evaluation_type(type) {}

            value(float value, EVALUATION_TYPE type = EVALUATION_TYPE::PERCENTAGE)
                : data(value), evaluation_type(type) {}

            // Copy constructor
            constexpr value(const value<T>& other)
                : data(other.data), evaluation_type(other.evaluation_type) {}

            // Assignment operators
            constexpr value& operator=(const value& other) {
                data = other.data;
                evaluation_type = other.evaluation_type;
                return *this;
            }

            constexpr value& operator=(T initialization_data) {
                data = initialization_data;
                evaluation_type = EVALUATION_TYPE::DEFAULT;
                return *this;
            }

            constexpr value& operator=(float initialization_data) {
                data = initialization_data;
                evaluation_type = EVALUATION_TYPE::PERCENTAGE;
                return *this;
            }

            // Evaluate function
            T Evaluate(T parental_value) {
                switch (evaluation_type) {
                    case EVALUATION_TYPE::DEFAULT:
                        return std::get<T>(data);
                    case EVALUATION_TYPE::PERCENTAGE:
                        return static_cast<T>(static_cast<T>(parental_value) * std::get<float>(data));
                    default:
                        Report_Stack("Evaluation type not supported!");
                        return std::get<T>(data);
                }
            }

            template<typename P>
            constexpr P Get() const {
                // check at compile time if the compiler is compiling with debug or release flags, if debug then enable data type check
                #ifdef _DEBUG
                if (!std::holds_alternative<P>(data)) {
                    Report_Stack("Value is not of the requested type!");
                    throw std::bad_variant_access();  // Exception if the requested type doesn't match
                }
                #endif
                return std::get<P>(data);  // In release mode, or after the check passes in debug mode
            }

            // Getter methods
            template<typename P>
            inline P Get() {                
                // check at compile time if the compiler is compiling with debug or release flags, if debug then enable data type check
                #ifdef _DEBUG
                if (!std::holds_alternative<P>(data)) {
                    Report_Stack("Value is not of the requested type!");
                    throw std::bad_variant_access();  // Exception if the requested type doesn't match
                }
                #endif
                return std::get<P>(data);  // In release mode, or after the check passes in debug mode
            }

            // This is an exclusive Dev function, with serious repercussions for misuse.
            template<typename P>
            P& Direct() { 
                // check at compile time if the compiler is compiling with debug or release flags, if debug then enable data type check
                #ifdef _DEBUG
                if (!std::holds_alternative<P>(data)) {
                    Report_Stack("Value is not of the requested type!");
                    throw std::bad_variant_access();  // Exception if the requested type doesn't match
                }
                #endif
                return std::get<P>(data);  // In release mode, or after the check passes in debug mode
            }

            inline void Set(T value) {
                data = value;
                evaluation_type = EVALUATION_TYPE::DEFAULT;
            }

            inline void Set(float value){
                data = value;
                evaluation_type = EVALUATION_TYPE::PERCENTAGE;
            }
        };

        class style_base{
        public:
            VALUE_STATE Status = VALUE_STATE::UNINITIALIZED;

            constexpr style_base(VALUE_STATE status, [[maybe_unused]] bool use_constexpr) : Status(status){}
            
            style_base(VALUE_STATE status) : Status(status){}

            style_base() = default;

            // Normally style base doesn't do anything.
            // virtual Style_Base* operator|(Style_Base* other) = 0;

            // This function is for the single style classes to be able to imprint their own identity into the Element object which hosts these styles.
            virtual void Embed_Value(Element* host) = 0;
        };

        class RGB_VALUE : public style_base{
        public:
            value<RGB> Value = RGB(0, 0, 0);

            RGB_VALUE(RGB value, VALUE_STATE Default = VALUE_STATE::VALUE) : style_base(Default){
                Value = value;
            }

            constexpr RGB_VALUE(const GGUI::RGB value, VALUE_STATE Default, [[maybe_unused]] bool use_constexpr) : style_base(Default, true), Value(value, EVALUATION_TYPE::DEFAULT, true){}

            RGB_VALUE() = default;

            // operator overload for copy operator
            RGB_VALUE& operator=(const RGB_VALUE& other){
                // Only copy the information if the other is enabled.
                if (other.Status >= Status){
                    Value = other.Value;

                    Status = other.Status;
                }
                return *this;
            }

            RGB_VALUE& operator=(const GGUI::RGB other){
                Value = other;
                Status = VALUE_STATE::VALUE;
                return *this;
            }
        
            constexpr RGB_VALUE(const GGUI::STYLING_INTERNAL::RGB_VALUE& other) : style_base(other.Status, true), Value(other.Value){}

            // The basic style types do not have imprint methods.
            void Embed_Value([[maybe_unused]] Element* host) override {};
        };

        class BOOL_VALUE : public style_base{
        public:
            bool Value = false;

            BOOL_VALUE(bool value, VALUE_STATE Default = VALUE_STATE::VALUE) : style_base(Default){
                Value = value;
            }

            BOOL_VALUE() = default;

            // operator overload for copy operator
            BOOL_VALUE& operator=(const BOOL_VALUE& other){
                // Only copy the information if the other is enabled.
                if (other.Status >= Status){
                    Value = other.Value;

                    Status = other.Status;
                }
                return *this;
            }

            BOOL_VALUE& operator=(const bool other){
                Value = other;
                Status = VALUE_STATE::VALUE;
                return *this;
            }
        
            constexpr BOOL_VALUE(const GGUI::STYLING_INTERNAL::BOOL_VALUE& other) : style_base(other.Status, true), Value(other.Value){}
            
            // The basic style types do not have imprint methods.
            void Embed_Value([[maybe_unused]] Element* host) override {};
        };
        
        class NUMBER_VALUE : public style_base{
        public:
            value<int> Value = 0;

            NUMBER_VALUE(int value, VALUE_STATE Default = VALUE_STATE::VALUE) : style_base(Default){
                Value = value;
            }

            NUMBER_VALUE(float value, VALUE_STATE Default = VALUE_STATE::VALUE) : style_base(Default), Value(value, EVALUATION_TYPE::PERCENTAGE){}

            constexpr NUMBER_VALUE(int value, VALUE_STATE Default, [[maybe_unused]] bool use_constexpr) : style_base(Default, true), Value(value, EVALUATION_TYPE::DEFAULT, true){}

            NUMBER_VALUE() = default;

            // operator overload for copy operator
            NUMBER_VALUE& operator=(const NUMBER_VALUE& other){
                // Only copy the information if the other is enabled.
                if (other.Status >= Status){
                    Value = other.Value;

                    Status = other.Status;
                }
                return *this;
            }

            NUMBER_VALUE& operator=(const int other){
                Value = other;
                Status = VALUE_STATE::VALUE;
                return *this;
            }
        
            constexpr NUMBER_VALUE(const GGUI::STYLING_INTERNAL::NUMBER_VALUE& other) : style_base(other.Status, true), Value(other.Value){}
            
            // The basic style types do not have imprint methods.
            void Embed_Value([[maybe_unused]] Element* host) override {};

            int& Direct() { return Value.Direct<int>(); }
        };

        template<typename T>
        class ENUM_VALUE : public style_base{
        public:
            T Value;

            ENUM_VALUE(T value, VALUE_STATE Default = VALUE_STATE::INITIALIZED) : style_base(Default){
                Value = value;
            }

            ENUM_VALUE() = default;

            // operator overload for copy operator
            ENUM_VALUE& operator=(const ENUM_VALUE& other){
                // Only copy the information if the other is enabled.
                if (other.Status >= Status){
                    Value = other.Value;

                    Status = other.Status;
                }
                return *this;
            }

            ENUM_VALUE& operator=(const T other){
                Value = other;
                Status = VALUE_STATE::VALUE;
                return *this;
            }
        
            constexpr ENUM_VALUE(const GGUI::STYLING_INTERNAL::ENUM_VALUE<T>& other) : style_base(other.Status, true), Value(other.Value){}
            
            // The basic style types do not have imprint methods.
            void Embed_Value([[maybe_unused]] Element* host) override {};
        };
        
        class Vector : public style_base{
        public:
            value<IVector2> Value = IVector2();

            Vector(IVector2 value, VALUE_STATE Default = VALUE_STATE::VALUE) : style_base(Default){
                Value = value;
            }

            constexpr Vector(const GGUI::IVector2 value, VALUE_STATE Default, [[maybe_unused]] bool use_constexpr) : style_base(Default, true), Value(value, EVALUATION_TYPE::DEFAULT, true){}

            Vector() = default;

            // operator overload for copy operator
            Vector& operator=(const Vector& other){
                // Only copy the information if the other is enabled.
                if (other.Status >= Status){
                    Value = other.Value;

                    Status = other.Status;
                }
                return *this;
            }

            Vector& operator=(const GGUI::IVector2 other){
                Value = other;
                Status = VALUE_STATE::VALUE;
                return *this;
            }

            constexpr Vector(const GGUI::STYLING_INTERNAL::Vector& other) : style_base(other.Status, true), Value(other.Value){}
            
            // The basic style types do not have imprint methods.
            void Embed_Value([[maybe_unused]] Element* host) override {};

            IVector2 Get() { return Value.Get<IVector2>(); }
            constexpr IVector2 Get() const { return Value.Get<IVector2>(); }

            void Set(IVector2 value){
                Value = value;
                Status = VALUE_STATE::VALUE;
            }

            IVector2& Direct() { return Value.Direct<IVector2>(); }
        };
    }

    class position : public STYLING_INTERNAL::Vector{
    public:
        position(IVector2 value, VALUE_STATE Default = VALUE_STATE::VALUE) : Vector(value, Default){}

        position() = default;

        constexpr position(const GGUI::position& other) : Vector(((const Vector&)other).Get(), other.Status, true){}

        position& operator=(const position& other) = default;

        void Embed_Value(Element* host) override;
    };

    class width : public STYLING_INTERNAL::NUMBER_VALUE{
    public:
        width(int value, VALUE_STATE Default = VALUE_STATE::VALUE) : NUMBER_VALUE(value, Default){}

        width() = default;

        constexpr width(const GGUI::width& other) : NUMBER_VALUE(other.Value.Get<int>(), other.Status, true){}

        width& operator=(const width& other) = default;

        void Embed_Value(Element* host) override;

        int Get() { return Value.Get<int>(); }

        void Set(int value){
            Value = value;
            Status = VALUE_STATE::VALUE;
        }
    };

    class height : public STYLING_INTERNAL::NUMBER_VALUE{
    public:
        height(int value, VALUE_STATE Default = VALUE_STATE::VALUE) : NUMBER_VALUE(value, Default){}

        height() = default;

        constexpr height(const GGUI::height& other) : NUMBER_VALUE(other.Value.Get<int>(), other.Status, true){}

        height& operator=(const height& other) = default;

        void Embed_Value(Element* host) override;

        int Get() { return Value.Get<int>(); }

        void Set(int value){
            Value = value;
            Status = VALUE_STATE::VALUE;
        }
    };

    class text_color : public STYLING_INTERNAL::RGB_VALUE{
    public:
        text_color(RGB color, VALUE_STATE Default = VALUE_STATE::VALUE) : RGB_VALUE(color, Default){}

        text_color() = default;

        constexpr text_color(const GGUI::text_color& other) : RGB_VALUE(other.Value.Get<RGB>(), other.Status, true){}

        text_color& operator=(const text_color& other) = default;

        void Embed_Value(Element* host) override;
    };

    class background_color : public STYLING_INTERNAL::RGB_VALUE{
    public:
        background_color(RGB color, VALUE_STATE Default = VALUE_STATE::VALUE) : RGB_VALUE(color, Default){}

        background_color() = default;

        constexpr background_color(const GGUI::background_color& other) : RGB_VALUE(other.Value.Get<RGB>(), other.Status, true){}

        background_color& operator=(const background_color& other) = default;

        void Embed_Value(Element* host) override;
    };

    class border_color : public STYLING_INTERNAL::RGB_VALUE{
    public:
        border_color(RGB color, VALUE_STATE Default = VALUE_STATE::VALUE) : RGB_VALUE(color, Default){}

        border_color() = default;

        constexpr border_color(const GGUI::border_color& other) : RGB_VALUE(other.Value.Get<RGB>(), other.Status, true){}

        border_color& operator=(const border_color& other) = default;

        void Embed_Value(Element* host) override;
    };

    class border_background_color : public STYLING_INTERNAL::RGB_VALUE{
    public:
        border_background_color(RGB color, VALUE_STATE Default = VALUE_STATE::VALUE) : RGB_VALUE(color, Default){}

        border_background_color() = default;

        constexpr border_background_color(const GGUI::border_background_color& other) : RGB_VALUE(other.Value.Get<RGB>(), other.Status, true){}

        border_background_color& operator=(const border_background_color& other) = default;

        void Embed_Value(Element* host) override;
    };

    class margin : public STYLING_INTERNAL::style_base{
    public:
        STYLING_INTERNAL::value<unsigned int> Top = (unsigned)0;
        STYLING_INTERNAL::value<unsigned int> Bottom = (unsigned)0;
        STYLING_INTERNAL::value<unsigned int> Left = (unsigned)0;
        STYLING_INTERNAL::value<unsigned int> Right = (unsigned)0;

        margin(unsigned int top = 0, unsigned int bottom = 0, unsigned int left = 0, unsigned int right = 0, VALUE_STATE Default = VALUE_STATE::VALUE) : style_base(Default), Top(top), Bottom(bottom), Left(left), Right(right){}

        // operator overload for copy operator
        margin& operator=(const margin& other){
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

        constexpr margin(const GGUI::margin& other) : style_base(other.Status, true), Top(other.Top), Bottom(other.Bottom), Left(other.Left), Right(other.Right){}
        
        // for dynamically computable values like percentage depended
        // currently covers:
        // - screen space
        margin Evaluate(Element* owner);

        void Embed_Value(Element* host) override;
    };

    class shadow : public STYLING_INTERNAL::style_base{
    public:
        STYLING_INTERNAL::value<FVector3> Direction = FVector3{0, 0, 0.5};
        STYLING_INTERNAL::value<RGB> Color = RGB{};
        float Opacity = 1;
        bool Enabled = false;

        shadow(FVector3 direction, RGB color, float opacity, bool enabled, VALUE_STATE Default = VALUE_STATE::VALUE) : style_base(Default){
            Direction = direction;
            Color = color;
            Opacity = opacity;
            Enabled = enabled;
        }

        shadow() : style_base(){}

        shadow& operator=(const shadow& other){
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
    
        constexpr shadow(const GGUI::shadow& other) : style_base(other.Status, true), Direction(other.Direction), Color(other.Color), Opacity(other.Opacity), Enabled(other.Enabled){}
    
        // for dynamically computable values like percentage depended
        // currently covers:
        // - screen space
        shadow Evaluate(Element* owner);

        void Embed_Value(Element* host) override;
    };

    class opacity : public STYLING_INTERNAL::style_base{
    protected:
        float Value;
    public:
        opacity(float value, VALUE_STATE state) : style_base(state), Value(value){}

        opacity() = default;

        opacity& operator=(const opacity& other){
            // Only copy the information if the other is enabled.
            if (other.Status >= Status){
                Value = other.Value;

                Status = other.Status;
            }
            return *this;
        }

        constexpr opacity(const GGUI::opacity& other) : style_base(other.Status, true), Value(other.Value){}

        void Embed_Value(Element* host) override;

        inline float Get() { return Value; }

        void Set(float value){
            Value = value;
            Status = VALUE_STATE::VALUE;
        }
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

        styled_border(std::vector<const char*> values, VALUE_STATE Default = VALUE_STATE::VALUE);

        // Re-import defaults:
        styled_border() = default; // This should also call the base class
        ~styled_border() = default;
        styled_border& operator=(const styled_border& other){
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
    
        constexpr styled_border(const GGUI::styled_border& other) : style_base(other.Status, true){
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

        void Embed_Value(Element* host) override;
    };

    class Styling{
    public:
        position Position;

        width Width = 1;
        width Height = 1;

        STYLING_INTERNAL::BOOL_VALUE Border_Enabled = STYLING_INTERNAL::BOOL_VALUE(false, VALUE_STATE::INITIALIZED);
        text_color                  Text_Color;
        background_color            Background_Color;
        border_color                Border_Color;
        border_background_color     Border_Background_Color;
        
        STYLING_INTERNAL::RGB_VALUE Hover_Border_Color;
        STYLING_INTERNAL::RGB_VALUE Hover_Text_Color;
        STYLING_INTERNAL::RGB_VALUE Hover_Background_Color;
        STYLING_INTERNAL::RGB_VALUE Hover_Border_Background_Color;

        STYLING_INTERNAL::RGB_VALUE Focus_Border_Color;
        STYLING_INTERNAL::RGB_VALUE Focus_Text_Color;
        STYLING_INTERNAL::RGB_VALUE Focus_Background_Color;
        STYLING_INTERNAL::RGB_VALUE Focus_Border_Background_Color;

        styled_border Border_Style;
        
        STYLING_INTERNAL::ENUM_VALUE<DIRECTION> Flow_Priority = STYLING_INTERNAL::ENUM_VALUE<DIRECTION>(DIRECTION::ROW, VALUE_STATE::INITIALIZED);
        STYLING_INTERNAL::BOOL_VALUE Wrap = STYLING_INTERNAL::BOOL_VALUE(false, VALUE_STATE::INITIALIZED);

        STYLING_INTERNAL::BOOL_VALUE Allow_Overflow = STYLING_INTERNAL::BOOL_VALUE(false, VALUE_STATE::INITIALIZED);
        STYLING_INTERNAL::BOOL_VALUE Allow_Dynamic_Size = STYLING_INTERNAL::BOOL_VALUE(false, VALUE_STATE::INITIALIZED);
        margin Margin;

        shadow Shadow;
        opacity Opacity = opacity(1.0f, VALUE_STATE::INITIALIZED);  // 100%

        STYLING_INTERNAL::BOOL_VALUE Allow_Scrolling = STYLING_INTERNAL::BOOL_VALUE(false, VALUE_STATE::INITIALIZED);

        // Only fetch one parent UP, and own position +, then child repeat.
        STYLING_INTERNAL::Vector Absolute_Position_Cache;

        STYLING_INTERNAL::ENUM_VALUE<ALIGN> Align = STYLING_INTERNAL::ENUM_VALUE<ALIGN>(ALIGN::LEFT, VALUE_STATE::INITIALIZED);

        Styling() = default;

        void Copy(const Styling& other);

        void Copy(const Styling* other){
            // use the reference one
            Copy(*other);
        }
    
        // This acts like the Render() overload for elements, where the function will go through flagged states and checks what needs to be updated.
        // It is also highly recommended to call this right after assessing which stylings an element has.
        void Evaluate_Dynamic_Attribute_Values();
    };

    namespace STYLES{
        namespace BORDER{
            const inline styled_border Double = std::vector<const char*>{
                "╔", "╚", "╗", "╝", "║", "═", "╠", "╣", "╦", "╩", "╬"
            };

            const inline styled_border Round = std::vector<const char*>{
                "╭", "╰", "╮", "╯", "│", "─", "├", "┤", "┬", "┴", "┼"
            };

            const inline styled_border Single = std::vector<const char*>{
                "┌", "└", "┐", "┘", "│", "─", "├", "┤", "┬", "┴", "┼"
            };

            const inline styled_border Bold = std::vector<const char*>{
                "▛", "▙", "▜", "▟", "█", "▅", "▉", "▉", "▉", "▉", "▉"
            };

            const inline styled_border Modern = std::vector<const char*>{
                "/", "\\", "\\", "/", "|", "-", "|", "|", "-", "-", "+"
            };
            
        }
    };

}

#endif