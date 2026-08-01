#ifndef _CONSTANTS_H_
#define _CONSTANTS_H_

#include "superString.h"

namespace GGUI{
    namespace SYMBOLS{
        constexpr INTERNAL::compactString TOP_LEFT_CORNER = "┌";//"\e(0\x6c\e(B";
        constexpr INTERNAL::compactString BOTTOM_LEFT_CORNER = "└";//"\e(0\x6d\e(B";
        constexpr INTERNAL::compactString TOP_RIGHT_CORNER = "┐";//"\e(0\x6b\e(B";
        constexpr INTERNAL::compactString BOTTOM_RIGHT_CORNER = "┘";//"\e(0\x6a\e(B";
        constexpr INTERNAL::compactString VERTICAL_LINE = "│";//"\e(0\x78\e(B";
        constexpr INTERNAL::compactString HORIZONTAL_LINE = "─";//"\e(0\x71\e(B";
        constexpr INTERNAL::compactString VERTICAL_RIGHT_CONNECTOR = "├";//"\e(0\x74\e(B";
        constexpr INTERNAL::compactString VERTICAL_LEFT_CONNECTOR = "┤";//"\e(0\x75\e(B";
        constexpr INTERNAL::compactString HORIZONTAL_BOTTOM_CONNECTOR = "┬";//"\e(0\x76\e(B";
        constexpr INTERNAL::compactString HORIZONTAL_TOP_CONNECTOR = "┴";//"\e(0\x77\e(B";
        constexpr INTERNAL::compactString CROSS_CONNECTOR = "┼";//"\e(0\x6e\e(B";

        constexpr INTERNAL::compactString CENTERED_HORIZONTAL_LINE = "━";//"\e(0\x2501\e(B";
        constexpr INTERNAL::compactString FULL_BLOCK = "█";//"\e(0\x2588\e(B";

        constexpr INTERNAL::compactString RADIOBUTTON_OFF = "○";
        constexpr INTERNAL::compactString RADIOBUTTON_ON = "◉";

        constexpr INTERNAL::compactString EMPTY_CHECK_BOX = "☐";
        constexpr INTERNAL::compactString CHECKED_CHECK_BOX = "☒";
    }
}

#endif