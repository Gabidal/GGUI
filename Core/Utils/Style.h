#ifndef _STYLE_H_
#define _STYLE_H_

#include "Units.h"

namespace GGUI{
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

    class Style_Base{
    public:
        VALUE_STATE Status = VALUE_STATE::UNINITIALIZED;

        constexpr Style_Base(VALUE_STATE status, [[maybe_unused]] bool use_constexpr) : Status(status){}
        
        Style_Base(VALUE_STATE status) : Status(status){}

        Style_Base() = default;

        // Normally style base doesn't do anything.
        // virtual Style_Base* operator|(Style_Base* other) = 0;
    };

    class MARGIN_VALUE : public Style_Base{
    public:
        unsigned int Top = 0;
        unsigned int Bottom = 0;
        unsigned int Left = 0;
        unsigned int Right = 0;

        MARGIN_VALUE(unsigned int top = 0, unsigned int bottom = 0, unsigned int left = 0, unsigned int right = 0, VALUE_STATE Default = VALUE_STATE::VALUE) : Style_Base(Default){
            Top = top;
            Bottom = bottom;
            Left = left;
            Right = right;
        }

        // operator overload for copy operator
        MARGIN_VALUE& operator=(const MARGIN_VALUE& other){
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

        constexpr MARGIN_VALUE(const GGUI::MARGIN_VALUE& other) : Style_Base(other.Status, true){
            Top = other.Top;
            Bottom = other.Bottom;
            Left = other.Left;
            Right = other.Right;
        }
    };

    class COORDINATES_VALUE : public Style_Base{
    public:
        Coordinates Value = Coordinates();

        COORDINATES_VALUE(Coordinates value, VALUE_STATE Default = VALUE_STATE::VALUE) : Style_Base(Default){
            Value = value;
        }

        COORDINATES_VALUE() = default;

        // operator overload for copy operator
        COORDINATES_VALUE& operator=(const COORDINATES_VALUE& other){
            // Only copy the information if the other is enabled.
            if (other.Status >= Status){
                Value = other.Value;

                Status = other.Status;
            }
            return *this;
        }

        COORDINATES_VALUE& operator=(const GGUI::Coordinates other){
            Value = other;
            Status = VALUE_STATE::VALUE;
            return *this;
        }

        constexpr COORDINATES_VALUE(const GGUI::COORDINATES_VALUE& other) : Style_Base(other.Status, true), Value(other.Value){}
    };

    class SHADOW_VALUE : public Style_Base{
    public:
        Vector3 Direction = {0, 0, 0.5};
        RGB Color = {};
        float Opacity = 1;
        bool Enabled = false;

        SHADOW_VALUE(Vector3 direction, RGB color, float opacity, bool enabled, VALUE_STATE Default = VALUE_STATE::VALUE) : Style_Base(Default){
            Direction = direction;
            Color = color;
            Opacity = opacity;
            Enabled = enabled;
        }

        SHADOW_VALUE() : Style_Base(){}

        SHADOW_VALUE& operator=(const SHADOW_VALUE& other){
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
    
        constexpr SHADOW_VALUE(const GGUI::SHADOW_VALUE& other) : Style_Base(other.Status, true), Direction(other.Direction), Color(other.Color), Opacity(other.Opacity), Enabled(other.Enabled){}
    };

    class BORDER_STYLE_VALUE : public Style_Base{
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

        BORDER_STYLE_VALUE(std::vector<const char*> values, VALUE_STATE Default = VALUE_STATE::VALUE);

        // Re-import defaults:
        BORDER_STYLE_VALUE() = default; // This should also call the base class
        ~BORDER_STYLE_VALUE() = default;
        BORDER_STYLE_VALUE& operator=(const BORDER_STYLE_VALUE& other){
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
    
        constexpr BORDER_STYLE_VALUE(const GGUI::BORDER_STYLE_VALUE& other) : Style_Base(other.Status, true){
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
    };

    class RGB_VALUE : public Style_Base{
    public:
        RGB Value = RGB(0, 0, 0);

        RGB_VALUE(RGB value, VALUE_STATE Default = VALUE_STATE::VALUE) : Style_Base(Default){
            Value = value;
        }

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
    
        constexpr RGB_VALUE(const GGUI::RGB_VALUE& other) : Style_Base(other.Status, true), Value(other.Value){}
    };

    class BOOL_VALUE : public Style_Base{
    public:
        bool Value = false;

        BOOL_VALUE(bool value, VALUE_STATE Default = VALUE_STATE::VALUE) : Style_Base(Default){
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
    
        constexpr BOOL_VALUE(const GGUI::BOOL_VALUE& other) : Style_Base(other.Status, true), Value(other.Value){}
    };
    
    class NUMBER_VALUE : public Style_Base{
    public:
        int Value = 0;

        NUMBER_VALUE(int value, VALUE_STATE Default = VALUE_STATE::VALUE) : Style_Base(Default){
            Value = value;
        }

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
    
        constexpr NUMBER_VALUE(const GGUI::NUMBER_VALUE& other) : Style_Base(other.Status, true), Value(other.Value){}
    };

    template<typename T>
    class ENUM_VALUE : public Style_Base{
    public:
        T Value;

        ENUM_VALUE(T value, VALUE_STATE Default = VALUE_STATE::INITIALIZED) : Style_Base(Default){
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
    
        constexpr ENUM_VALUE(const GGUI::ENUM_VALUE<T>& other) : Style_Base(other.Status, true), Value(other.Value){}
    };

    class Styling{
    public:
        BOOL_VALUE Border_Enabled = BOOL_VALUE(false, VALUE_STATE::INITIALIZED);
        RGB_VALUE Text_Color;
        RGB_VALUE Background_Color;
        RGB_VALUE Border_Color;
        RGB_VALUE Border_Background_Color;
        
        RGB_VALUE Hover_Border_Color;
        RGB_VALUE Hover_Text_Color;
        RGB_VALUE Hover_Background_Color;
        RGB_VALUE Hover_Border_Background_Color;

        RGB_VALUE Focus_Border_Color;
        RGB_VALUE Focus_Text_Color;
        RGB_VALUE Focus_Background_Color;
        RGB_VALUE Focus_Border_Background_Color;

        BORDER_STYLE_VALUE Border_Style;
        
        ENUM_VALUE<DIRECTION> Flow_Priority = ENUM_VALUE<DIRECTION>(DIRECTION::ROW, VALUE_STATE::INITIALIZED);
        BOOL_VALUE Wrap = BOOL_VALUE(false, VALUE_STATE::INITIALIZED);

        BOOL_VALUE Allow_Overflow = BOOL_VALUE(false, VALUE_STATE::INITIALIZED);
        BOOL_VALUE Allow_Dynamic_Size = BOOL_VALUE(false, VALUE_STATE::INITIALIZED);
        MARGIN_VALUE Margin;

        SHADOW_VALUE Shadow;
        NUMBER_VALUE Opacity = NUMBER_VALUE(100, VALUE_STATE::INITIALIZED);  // 100%

        BOOL_VALUE Allow_Scrolling = BOOL_VALUE(false, VALUE_STATE::INITIALIZED);

        // Only fetch one parent UP, and own position +, then child repeat.
        COORDINATES_VALUE Absolute_Position_Cache;

        ENUM_VALUE<ALIGN> Align = ENUM_VALUE<ALIGN>(ALIGN::LEFT, VALUE_STATE::INITIALIZED);

        Styling() = default;

        void Copy(const Styling& other);

        void Copy(const Styling* other){
            // use the reference one
            Copy(*other);
        }
    };

    namespace STYLES{
        namespace BORDER{
            const inline BORDER_STYLE_VALUE Double = std::vector<const char*>{
                "╔", "╚", "╗", "╝", "║", "═", "╠", "╣", "╦", "╩", "╬"
            };

            const inline BORDER_STYLE_VALUE Round = std::vector<const char*>{
                "╭", "╰", "╮", "╯", "│", "─", "├", "┤", "┬", "┴", "┼"
            };

            const inline BORDER_STYLE_VALUE Single = std::vector<const char*>{
                "┌", "└", "┐", "┘", "│", "─", "├", "┤", "┬", "┴", "┼"
            };

            const inline BORDER_STYLE_VALUE Bold = std::vector<const char*>{
                "▛", "▙", "▜", "▟", "█", "▅", "▉", "▉", "▉", "▉", "▉"
            };

            const inline BORDER_STYLE_VALUE Modern = std::vector<const char*>{
                "/", "\\", "\\", "/", "|", "-", "|", "|", "-", "-", "+"
            };
            
        }
    };

}

#endif