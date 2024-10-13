#ifndef _STYLE_H_
#define _STYLE_H_

#include "Units.h"
#include <variant>

namespace GGUI{
    // Externies
    class Element;
    class Styling;
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
            // This is used to prevent accidental overwrites.
            VALUE_STATE Status = VALUE_STATE::UNINITIALIZED;

            // This is used to store all appended style_bases through the operator|.
            style_base* Other = nullptr;

            constexpr style_base(VALUE_STATE status, [[maybe_unused]] bool use_constexpr) : Status(status), Other(nullptr){}
            
            style_base(VALUE_STATE status) : Status(status), Other(nullptr){}

            style_base() = default;

            virtual ~style_base() = default;

            style_base* operator|(style_base* other){
                Other = other;

                return this;
            }

            // This function is for the single style classes to be able to imprint their own identity into the Styling class
            virtual void Embed_Value(Styling* host) = 0;

            virtual void Evaluate(Styling* host) = 0;
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
            void Embed_Value([[maybe_unused]] Styling* host) override {};

            void Evaluate(Styling* owner) override = 0;
        };

        class BOOL_VALUE : public style_base{
        public:
            bool Value = false;

            BOOL_VALUE(bool value, VALUE_STATE Default = VALUE_STATE::VALUE) : style_base(Default){
                Value = value;
            }

            constexpr BOOL_VALUE(bool value, VALUE_STATE Default, [[maybe_unused]] bool use_constexpr) : style_base(Default, true), Value(value){}

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
            void Embed_Value([[maybe_unused]] Styling* host) override {};

            void Evaluate([[maybe_unused]] Styling* owner) override {};
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
            void Embed_Value([[maybe_unused]] Styling* host) override {};
            
            void Evaluate(Styling* owner) override = 0;

            int& Direct() { return Value.Direct<int>(); }
        };

        template<typename T>
        class ENUM_VALUE : public style_base{
        public:
            T Value;

            ENUM_VALUE(T value, VALUE_STATE Default = VALUE_STATE::INITIALIZED) : style_base(Default){
                Value = value;
            }

            constexpr ENUM_VALUE(T value, VALUE_STATE Default, [[maybe_unused]] bool use_constexpr) : style_base(Default, true), Value(value){}

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
            void Embed_Value([[maybe_unused]] Styling* host) override {};
            
            void Evaluate([[maybe_unused]] Styling* owner) override {};
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
            void Embed_Value([[maybe_unused]] Styling* host) override {};
            
            void Evaluate(Styling* owner) override = 0;

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

        position(int x, int y, int z = 0, VALUE_STATE Default = VALUE_STATE::VALUE) : Vector(IVector2(x, y, z), Default){}

        position() = default;

        constexpr position(const GGUI::position& other) : Vector(((const Vector&)other).Get(), other.Status, true){}

        position& operator=(const position& other) = default;

        // for dynamically computable values like percentage depended
        // currently covers:
        // - screen space
        void Evaluate(Styling* owner) override;

        void Embed_Value(Styling* host) override;
    };

    class width : public STYLING_INTERNAL::NUMBER_VALUE{
    public:
        width(int value, VALUE_STATE Default = VALUE_STATE::VALUE) : NUMBER_VALUE(value, Default){}

        width() = default;

        constexpr width(const GGUI::width& other) : NUMBER_VALUE(other.Value.Get<int>(), other.Status, true){}

        width& operator=(const width& other) = default;

        // for dynamically computable values like percentage depended
        // currently covers:
        // - screen space
        void Evaluate(Styling* owner) override;

        void Embed_Value(Styling* host) override;

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
        
        // for dynamically computable values like percentage depended
        // currently covers:
        // - screen space
        void Evaluate(Styling* owner) override;
        
        void Embed_Value(Styling* host) override;

        int Get() { return Value.Get<int>(); }

        void Set(int value){
            Value = value;
            Status = VALUE_STATE::VALUE;
        }
    };

    class border_enabled : public STYLING_INTERNAL::BOOL_VALUE{
    public:
        border_enabled(bool value, VALUE_STATE Default = VALUE_STATE::VALUE) : BOOL_VALUE(value, Default){}

        border_enabled() = default;

        constexpr border_enabled(const GGUI::border_enabled& other) : BOOL_VALUE(other.Value, other.Status, true){}

        border_enabled& operator=(const border_enabled& other) = default;

        // for dynamically computable values like percentage depended
        // currently covers:
        // - screen space
        void Evaluate([[maybe_unused]] Styling* owner) override {};
        
        void Embed_Value(Styling* host) override;
    };

    class text_color : public STYLING_INTERNAL::RGB_VALUE{
    public:
        text_color(RGB color, VALUE_STATE Default = VALUE_STATE::VALUE) : RGB_VALUE(color, Default){}

        text_color() = default;

        constexpr text_color(const GGUI::text_color& other) : RGB_VALUE(other.Value.Get<RGB>(), other.Status, true){}

        text_color& operator=(const text_color& other) = default;

        // for dynamically computable values like percentage depended
        // currently covers:
        // - screen space
        void Evaluate(Styling* owner) override;
        
        void Embed_Value(Styling* host) override;
    };

    class background_color : public STYLING_INTERNAL::RGB_VALUE{
    public:
        background_color(RGB color, VALUE_STATE Default = VALUE_STATE::VALUE) : RGB_VALUE(color, Default){}

        background_color() = default;

        constexpr background_color(const GGUI::background_color& other) : RGB_VALUE(other.Value.Get<RGB>(), other.Status, true){}

        background_color& operator=(const background_color& other) = default;

        // for dynamically computable values like percentage depended
        // currently covers:
        // - screen space
        void Evaluate(Styling* owner) override;
        
        void Embed_Value(Styling* host) override;
    };

    class border_color : public STYLING_INTERNAL::RGB_VALUE{
    public:
        border_color(RGB color, VALUE_STATE Default = VALUE_STATE::VALUE) : RGB_VALUE(color, Default){}

        border_color() = default;

        constexpr border_color(const GGUI::border_color& other) : RGB_VALUE(other.Value.Get<RGB>(), other.Status, true){}

        border_color& operator=(const border_color& other) = default;

        // for dynamically computable values like percentage depended
        // currently covers:
        // - screen space
        void Evaluate(Styling* owner) override;
        
        void Embed_Value(Styling* host) override;
    };

    class border_background_color : public STYLING_INTERNAL::RGB_VALUE{
    public:
        border_background_color(RGB color, VALUE_STATE Default = VALUE_STATE::VALUE) : RGB_VALUE(color, Default){}

        border_background_color() = default;

        constexpr border_background_color(const GGUI::border_background_color& other) : RGB_VALUE(other.Value.Get<RGB>(), other.Status, true){}

        border_background_color& operator=(const border_background_color& other) = default;
        
        // for dynamically computable values like percentage depended
        // currently covers:
        // - screen space
        void Evaluate(Styling* owner) override;
        
        void Embed_Value(Styling* host) override;
    };

    class hover_border_color : public STYLING_INTERNAL::RGB_VALUE{
    public:
        hover_border_color(RGB color, VALUE_STATE Default = VALUE_STATE::VALUE) : RGB_VALUE(color, Default){}

        hover_border_color() = default;

        constexpr hover_border_color(const GGUI::hover_border_color& other) : RGB_VALUE(other.Value.Get<RGB>(), other.Status, true){}

        hover_border_color& operator=(const hover_border_color& other) = default;
        
        // for dynamically computable values like percentage depended
        // currently covers:
        // - screen space
        void Evaluate(Styling* owner) override;
        
        void Embed_Value(Styling* host) override;
    };

    class hover_text_color : public STYLING_INTERNAL::RGB_VALUE{
    public:
        hover_text_color(RGB color, VALUE_STATE Default = VALUE_STATE::VALUE) : RGB_VALUE(color, Default){}

        hover_text_color() = default;

        constexpr hover_text_color(const GGUI::hover_text_color& other) : RGB_VALUE(other.Value.Get<RGB>(), other.Status, true){}

        hover_text_color& operator=(const hover_text_color& other) = default;
        
        // for dynamically computable values like percentage depended
        // currently covers:
        // - screen space
        void Evaluate(Styling* owner) override;
        
        void Embed_Value(Styling* host) override;
    };

    class hover_background_color : public STYLING_INTERNAL::RGB_VALUE{
    public:
        hover_background_color(RGB color, VALUE_STATE Default = VALUE_STATE::VALUE) : RGB_VALUE(color, Default){}

        hover_background_color() = default;

        constexpr hover_background_color(const GGUI::hover_background_color& other) : RGB_VALUE(other.Value.Get<RGB>(), other.Status, true){}

        hover_background_color& operator=(const hover_background_color& other) = default;
        
        // for dynamically computable values like percentage depended
        // currently covers:
        // - screen space
        void Evaluate(Styling* owner) override;
        
        void Embed_Value(Styling* host) override;
    };

    class hover_border_background_color : public STYLING_INTERNAL::RGB_VALUE{
    public:
        hover_border_background_color(RGB color, VALUE_STATE Default = VALUE_STATE::VALUE) : RGB_VALUE(color, Default){}

        hover_border_background_color() = default;

        constexpr hover_border_background_color(const GGUI::hover_border_background_color& other) : RGB_VALUE(other.Value.Get<RGB>(), other.Status, true){}

        hover_border_background_color& operator=(const hover_border_background_color& other) = default;
        
        // for dynamically computable values like percentage depended
        // currently covers:
        // - screen space
        void Evaluate(Styling* owner) override;
        
        void Embed_Value(Styling* host) override;
    };

    class focus_border_color : public STYLING_INTERNAL::RGB_VALUE{
    public:
        focus_border_color(RGB color, VALUE_STATE Default = VALUE_STATE::VALUE) : RGB_VALUE(color, Default){}

        focus_border_color() = default;

        constexpr focus_border_color(const GGUI::focus_border_color& other) : RGB_VALUE(other.Value.Get<RGB>(), other.Status, true){}

        focus_border_color& operator=(const focus_border_color& other) = default;
        
        // for dynamically computable values like percentage depended
        // currently covers:
        // - screen space
        void Evaluate(Styling* owner) override;
        
        void Embed_Value(Styling* host) override;
    };

    class focus_text_color : public STYLING_INTERNAL::RGB_VALUE{
    public:
        focus_text_color(RGB color, VALUE_STATE Default = VALUE_STATE::VALUE) : RGB_VALUE(color, Default){}

        focus_text_color() = default;

        constexpr focus_text_color(const GGUI::focus_text_color& other) : RGB_VALUE(other.Value.Get<RGB>(), other.Status, true){}

        focus_text_color& operator=(const focus_text_color& other) = default;
        
        // for dynamically computable values like percentage depended
        // currently covers:
        // - screen space
        void Evaluate(Styling* owner) override;
        
        void Embed_Value(Styling* host) override;
    };

    class focus_background_color : public STYLING_INTERNAL::RGB_VALUE{
    public:
        focus_background_color(RGB color, VALUE_STATE Default = VALUE_STATE::VALUE) : RGB_VALUE(color, Default){}

        focus_background_color() = default;

        constexpr focus_background_color(const GGUI::focus_background_color& other) : RGB_VALUE(other.Value.Get<RGB>(), other.Status, true){}

        focus_background_color& operator=(const focus_background_color& other) = default;
        
        // for dynamically computable values like percentage depended
        // currently covers:
        // - screen space
        void Evaluate(Styling* owner) override;
        
        void Embed_Value(Styling* host) override;
    };

    class focus_border_background_color : public STYLING_INTERNAL::RGB_VALUE{
    public:
        focus_border_background_color(RGB color, VALUE_STATE Default = VALUE_STATE::VALUE) : RGB_VALUE(color, Default){}

        focus_border_background_color() = default;

        constexpr focus_border_background_color(const GGUI::focus_border_background_color& other) : RGB_VALUE(other.Value.Get<RGB>(), other.Status, true){}

        focus_border_background_color& operator=(const focus_border_background_color& other) = default;
        
        // for dynamically computable values like percentage depended
        // currently covers:
        // - screen space
        void Evaluate(Styling* owner) override;
        
        void Embed_Value(Styling* host) override;
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

        void Evaluate([[maybe_unused]] Styling* owner) override {};
        
        void Embed_Value(Styling* host) override;
    };

    class flow_priority : public STYLING_INTERNAL::ENUM_VALUE<DIRECTION>{
    public:
        flow_priority(DIRECTION value, VALUE_STATE Default = VALUE_STATE::VALUE) : ENUM_VALUE(value, Default){}

        flow_priority() = default;

        constexpr flow_priority(const GGUI::flow_priority& other) : ENUM_VALUE(other.Value, other.Status, true){}

        flow_priority& operator=(const flow_priority& other) = default;
        
        // for dynamically computable values like percentage depended
        // currently covers:
        // - screen space
        void Evaluate([[maybe_unused]] Styling* owner) override {};

        void Embed_Value(Styling* host) override;
    };

    class wrap : public STYLING_INTERNAL::BOOL_VALUE{
    public:
        wrap(bool value, VALUE_STATE Default = VALUE_STATE::VALUE) : BOOL_VALUE(value, Default){}

        wrap() = default;

        constexpr wrap(const GGUI::wrap& other) : BOOL_VALUE(other.Value, other.Status, true){}

        wrap& operator=(const wrap& other) = default;
        
        // for dynamically computable values like percentage depended
        // currently covers:
        // - screen space
        void Evaluate([[maybe_unused]] Styling* owner) override {};
        
        void Embed_Value(Styling* host) override;
    };

    class allow_overflow : public STYLING_INTERNAL::BOOL_VALUE{
    public:
        allow_overflow(bool value, VALUE_STATE Default = VALUE_STATE::VALUE) : BOOL_VALUE(value, Default){}

        allow_overflow() = default;

        constexpr allow_overflow(const GGUI::allow_overflow& other) : BOOL_VALUE(other.Value, other.Status, true){}

        allow_overflow& operator=(const allow_overflow& other) = default;
        
        // for dynamically computable values like percentage depended
        // currently covers:
        // - screen space
        void Evaluate([[maybe_unused]] Styling* owner) override {};
        
        void Embed_Value(Styling* host) override;
    };

    class allow_dynamic_size : public STYLING_INTERNAL::BOOL_VALUE{
    public:
        allow_dynamic_size(bool value, VALUE_STATE Default = VALUE_STATE::VALUE) : BOOL_VALUE(value, Default){}

        allow_dynamic_size() = default;

        constexpr allow_dynamic_size(const GGUI::allow_dynamic_size& other) : BOOL_VALUE(other.Value, other.Status, true){}

        allow_dynamic_size& operator=(const allow_dynamic_size& other) = default;
        
        // for dynamically computable values like percentage depended
        // currently covers:
        // - screen space
        void Evaluate([[maybe_unused]] Styling* owner) override {};
        
        void Embed_Value(Styling* host) override;
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
        void Evaluate(Styling* owner) override;
        
        void Embed_Value(Styling* host) override;
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
        void Evaluate(Styling* owner) override;
        
        void Embed_Value(Styling* host) override;
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

        // Since opacity always represents an percentile of its self being displayed on top of its parent.
        void Evaluate([[maybe_unused]] Styling* owner) override {};
        
        void Embed_Value(Styling* host) override;

        inline float Get() { return Value; }

        void Set(float value){
            Value = value;
            Status = VALUE_STATE::VALUE;
        }
    };

    class allow_scrolling : public STYLING_INTERNAL::BOOL_VALUE{
    public:
        allow_scrolling(bool value, VALUE_STATE Default = VALUE_STATE::VALUE) : BOOL_VALUE(value, Default){}

        allow_scrolling() = default;

        constexpr allow_scrolling(const GGUI::allow_scrolling& other) : BOOL_VALUE(other.Value, other.Status, true){}

        allow_scrolling& operator=(const allow_scrolling& other) = default;
        
        // for dynamically computable values like percentage depended
        // currently covers:
        // - screen space
        void Evaluate([[maybe_unused]] Styling* owner) override {};
        
        void Embed_Value(Styling* host) override;
    };

    class align : public STYLING_INTERNAL::ENUM_VALUE<ALIGN>{
    public:
        align(ALIGN value, VALUE_STATE Default = VALUE_STATE::VALUE) : ENUM_VALUE(value, Default){}

        align() = default;

        constexpr align(const GGUI::align& other) : ENUM_VALUE(other.Value, other.Status, true){}

        align& operator=(const align& other) = default;
        
        // for dynamically computable values like percentage depended
        // currently covers:
        // - screen space
        void Evaluate([[maybe_unused]] Styling* owner) override {};
        
        void Embed_Value(Styling* host) override;
    };

    class node : public STYLING_INTERNAL::style_base{
    public:
        Element* Value;

        node(Element* value, VALUE_STATE Default = VALUE_STATE::VALUE) : style_base(Default), Value(value){}

        node() = default;
        
        constexpr node(const GGUI::node& other) : style_base(other.Status, true), Value(other.Value){}

        node& operator=(const node& other){
            // Only copy the information if the other is enabled.
            if (other.Status >= Status){
                Value = other.Value;

                Status = other.Status;
            }
            return *this;
        }

        void Evaluate([[maybe_unused]] Styling* owner) override {};

        void Embed_Value(Styling* host) override;
    };

    class Styling{
    public:
        position Position;

        width Width = 1;
        height Height = 1;

        border_enabled                  Border_Enabled = border_enabled(false, VALUE_STATE::INITIALIZED);
        text_color                      Text_Color = text_color(COLOR::WHITE, VALUE_STATE::INITIALIZED);
        background_color                Background_Color = background_color(COLOR::BLACK, VALUE_STATE::INITIALIZED);
        border_color                    Border_Color = border_color(COLOR::WHITE, VALUE_STATE::INITIALIZED);
        border_background_color         Border_Background_Color = border_background_color(COLOR::BLACK, VALUE_STATE::INITIALIZED);
        
        hover_border_color              Hover_Border_Color = hover_border_color(COLOR::WHITE, VALUE_STATE::INITIALIZED);
        hover_text_color                Hover_Text_Color = hover_text_color(COLOR::WHITE, VALUE_STATE::INITIALIZED);
        hover_background_color          Hover_Background_Color = hover_background_color(COLOR::DARK_GRAY, VALUE_STATE::INITIALIZED);
        hover_border_background_color   Hover_Border_Background_Color = hover_border_background_color(COLOR::BLACK, VALUE_STATE::INITIALIZED);

        focus_border_color              Focus_Border_Color = focus_border_color(COLOR::WHITE, VALUE_STATE::INITIALIZED);
        focus_text_color                Focus_Text_Color = focus_text_color(COLOR::BLACK, VALUE_STATE::INITIALIZED);
        focus_background_color          Focus_Background_Color = focus_background_color(COLOR::WHITE, VALUE_STATE::INITIALIZED);
        focus_border_background_color   Focus_Border_Background_Color = focus_border_background_color(COLOR::BLACK, VALUE_STATE::INITIALIZED);

        styled_border Border_Style;
        
        flow_priority Flow_Priority = flow_priority(DIRECTION::ROW, VALUE_STATE::INITIALIZED);
        wrap Wrap = wrap(false, VALUE_STATE::INITIALIZED);

        allow_overflow Allow_Overflow = allow_overflow(false, VALUE_STATE::INITIALIZED);
        allow_dynamic_size Allow_Dynamic_Size = allow_dynamic_size(false, VALUE_STATE::INITIALIZED);
        margin Margin;

        shadow Shadow;
        opacity Opacity = opacity(1.0f, VALUE_STATE::INITIALIZED);  // 100%

        allow_scrolling Allow_Scrolling = allow_scrolling(false, VALUE_STATE::INITIALIZED);

        align Align = align(ALIGN::LEFT, VALUE_STATE::INITIALIZED);

        std::vector<Element*> Childs;

        Styling() = default;

        Styling(STYLING_INTERNAL::style_base* attributes){
            STYLING_INTERNAL::style_base* current_attribute = attributes;

            // Loop until no further nested attributes.
            while (current_attribute){

                // First embed the current attribute
                current_attribute->Embed_Value(this);

                // Then set the current_attribute into the nested one
                current_attribute = current_attribute->Other;
            }
        }

        void Copy(const Styling& other);

        void Copy(const Styling* other){
            // use the reference one
            Copy(*other);
        }
    
        // This acts like the Render() overload for elements, where the function will go through flagged states and checks what needs to be updated.
        // Called within the STAIN::STRETCH handling, since this is about dynamic size attributes, and they need to be checked when the current element has been resized or its parent has.
        void Evaluate_Dynamic_Attribute_Values(Element* owner);
    

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

        namespace CONSTANTS{
            inline Styling Default;
        }
    };

}

#endif