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

    class style_base{
    public:
        VALUE_STATE Status = VALUE_STATE::UNINITIALIZED;

        constexpr style_base(VALUE_STATE status, [[maybe_unused]] bool use_constexpr) : Status(status){}
        
        style_base(VALUE_STATE status) : Status(status){}

        style_base() = default;

        // Normally style base doesn't do anything.
        // virtual Style_Base* operator|(Style_Base* other) = 0;
    };

    class margin : public style_base{
    public:
        unsigned int Top = 0;
        unsigned int Bottom = 0;
        unsigned int Left = 0;
        unsigned int Right = 0;

        margin(unsigned int top = 0, unsigned int bottom = 0, unsigned int left = 0, unsigned int right = 0, VALUE_STATE Default = VALUE_STATE::VALUE) : style_base(Default){
            Top = top;
            Bottom = bottom;
            Left = left;
            Right = right;
        }

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

        constexpr margin(const GGUI::margin& other) : style_base(other.Status, true){
            Top = other.Top;
            Bottom = other.Bottom;
            Left = other.Left;
            Right = other.Right;
        }
    };

    class location : public style_base{
    public:
        IVector2 Value = IVector2();

        location(IVector2 value, VALUE_STATE Default = VALUE_STATE::VALUE) : style_base(Default){
            Value = value;
        }

        location() = default;

        // operator overload for copy operator
        location& operator=(const location& other){
            // Only copy the information if the other is enabled.
            if (other.Status >= Status){
                Value = other.Value;

                Status = other.Status;
            }
            return *this;
        }

        location& operator=(const GGUI::IVector2 other){
            Value = other;
            Status = VALUE_STATE::VALUE;
            return *this;
        }

        constexpr location(const GGUI::location& other) : style_base(other.Status, true), Value(other.Value){}
    };

    class shadow : public style_base{
    public:
        FVector3 Direction = {0, 0, 0.5};
        RGB Color = {};
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
    };

    class styled_border : public style_base{
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
    };

    // This namespace is an wrapper for the user not to see these !!
    namespace STYLING_INTERNAL{
        class RGB_VALUE : public style_base{
        public:
            RGB Value = RGB(0, 0, 0);

            RGB_VALUE(RGB value, VALUE_STATE Default = VALUE_STATE::VALUE) : style_base(Default){
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
        
            constexpr RGB_VALUE(const GGUI::STYLING_INTERNAL::RGB_VALUE& other) : style_base(other.Status, true), Value(other.Value){}
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
        };
        
        class NUMBER_VALUE : public style_base{
        public:
            int Value = 0;

            NUMBER_VALUE(int value, VALUE_STATE Default = VALUE_STATE::VALUE) : style_base(Default){
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
        
            constexpr NUMBER_VALUE(const GGUI::STYLING_INTERNAL::NUMBER_VALUE& other) : style_base(other.Status, true), Value(other.Value){}
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
        };
    }

    class Styling{
    public:
        STYLING_INTERNAL::BOOL_VALUE Border_Enabled = STYLING_INTERNAL::BOOL_VALUE(false, VALUE_STATE::INITIALIZED);
        STYLING_INTERNAL::RGB_VALUE Text_Color;
        STYLING_INTERNAL::RGB_VALUE Background_Color;
        STYLING_INTERNAL::RGB_VALUE Border_Color;
        STYLING_INTERNAL::RGB_VALUE Border_Background_Color;
        
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
        STYLING_INTERNAL::NUMBER_VALUE Opacity = STYLING_INTERNAL::NUMBER_VALUE(100, VALUE_STATE::INITIALIZED);  // 100%

        STYLING_INTERNAL::BOOL_VALUE Allow_Scrolling = STYLING_INTERNAL::BOOL_VALUE(false, VALUE_STATE::INITIALIZED);

        // Only fetch one parent UP, and own position +, then child repeat.
        location Absolute_Position_Cache;

        STYLING_INTERNAL::ENUM_VALUE<ALIGN> Align = STYLING_INTERNAL::ENUM_VALUE<ALIGN>(ALIGN::LEFT, VALUE_STATE::INITIALIZED);

        Styling() = default;

        void Copy(const Styling& other);

        void Copy(const Styling* other){
            // use the reference one
            Copy(*other);
        }
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