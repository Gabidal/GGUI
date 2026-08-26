#ifndef _CONSTANTS_H_
#define _CONSTANTS_H_

#include <string_view>

namespace GGUI{
    namespace SYMBOLS{
        constexpr std::string_view TOP_LEFT_CORNER = "┌";//"\e(0\x6c\e(B";
        constexpr std::string_view BOTTOM_LEFT_CORNER = "└";//"\e(0\x6d\e(B";
        constexpr std::string_view TOP_RIGHT_CORNER = "┐";//"\e(0\x6b\e(B";
        constexpr std::string_view BOTTOM_RIGHT_CORNER = "┘";//"\e(0\x6a\e(B";
        constexpr std::string_view VERTICAL_LINE = "│";//"\e(0\x78\e(B";
        constexpr std::string_view HORIZONTAL_LINE = "─";//"\e(0\x71\e(B";
        constexpr std::string_view VERTICAL_RIGHT_CONNECTOR = "├";//"\e(0\x74\e(B";
        constexpr std::string_view VERTICAL_LEFT_CONNECTOR = "┤";//"\e(0\x75\e(B";
        constexpr std::string_view HORIZONTAL_BOTTOM_CONNECTOR = "┬";//"\e(0\x76\e(B";
        constexpr std::string_view HORIZONTAL_TOP_CONNECTOR = "┴";//"\e(0\x77\e(B";
        constexpr std::string_view CROSS_CONNECTOR = "┼";//"\e(0\x6e\e(B";

        constexpr std::string_view CENTERED_HORIZONTAL_LINE = "━";//"\e(0\x2501\e(B";
        constexpr std::string_view FULL_BLOCK = "█";//"\e(0\x2588\e(B";

        constexpr std::string_view RADIOBUTTON_OFF = "○";
        constexpr std::string_view RADIOBUTTON_ON = "◉";

        constexpr std::string_view EMPTY_CHECK_BOX = "☐";
        constexpr std::string_view CHECKED_CHECK_BOX = "☒";
    }
}

#endif