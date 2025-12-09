#ifndef _ANSI_SUITE_H_
#define _ANSI_SUITE_H_

#include "utils.h"
#include <ggui_dev.h>

// ANSI escape sequence and UTF conversion validation.

namespace tester {
    class ansiSuite : public utils::TestSuite {
    public:
        ansiSuite() : utils::TestSuite("ANSI / UTF core tester") {
            add_test("sgr_basic_enable", "Enable basic SGR features", test_sgr_basic_enable);
            add_test("private_sgr_enable_disable", "Enable/Disable private SGR features", test_private_sgr_enable_disable);
            add_test("cursor_shape_sequences", "Cursor shape control sequence formatting", test_cursor_shape_sequences);
            add_test("rgb_overhead_generation", "RGB overhead (text/background) generation & overload parity", test_rgb_overhead_generation);
            add_test("utf_superstring_transparency", "UTF toSuperString transparent vs opaque background", test_utf_superstring_transparency);
        }

    private:
        static void test_sgr_basic_enable() {
            auto bold = GGUI::constants::ANSI::enableSGRFeature(GGUI::constants::ANSI::BOLD).toString();
            auto underline = GGUI::constants::ANSI::enableSGRFeature(GGUI::constants::ANSI::UNDERLINE).toString();
            auto reset = GGUI::constants::ANSI::enableSGRFeature(GGUI::constants::ANSI::RESET_SGR).toString();

            ASSERT_EQ(std::string("\x1B[1m"), bold);
            ASSERT_EQ(std::string("\x1B[4m"), underline);
            ASSERT_EQ(std::string("\x1B[0m"), reset);
        }

        static void test_private_sgr_enable_disable() {
            auto cursor_on = GGUI::constants::ANSI::enablePrivateSGRFeature(GGUI::constants::ANSI::MOUSE_CURSOR, true).toString();
            auto cursor_off = GGUI::constants::ANSI::enablePrivateSGRFeature(GGUI::constants::ANSI::MOUSE_CURSOR, false).toString();
            auto alt_on = GGUI::constants::ANSI::enablePrivateSGRFeature(GGUI::constants::ANSI::ALTERNATIVE_SCREEN_BUFFER, true).toString();
            auto alt_off = GGUI::constants::ANSI::enablePrivateSGRFeature(GGUI::constants::ANSI::ALTERNATIVE_SCREEN_BUFFER, false).toString();

            ASSERT_EQ(std::string("\x1B[?25h"), cursor_on);
            ASSERT_EQ(std::string("\x1B[?25l"), cursor_off);
            ASSERT_EQ(std::string("\x1B[?1049h"), alt_on);
            ASSERT_EQ(std::string("\x1B[?1049l"), alt_off);

            // Should not end with 'm' (they use h/l terminators)
            ASSERT_TRUE(cursor_on.back() == 'h');
            ASSERT_TRUE(cursor_off.back() == 'l');
        }

        static void test_cursor_shape_sequences() {
            auto blinking_block = GGUI::constants::ANSI::setCursorShape(GGUI::constants::ANSI::CURSOR_SHAPE_BLINKING_BLOCK).toString();
            auto steady_bar = GGUI::constants::ANSI::setCursorShape(GGUI::constants::ANSI::CURSOR_SHAPE_STEADY_BAR).toString();

            ASSERT_EQ(std::string("\x1B[0 q"), blinking_block);
            ASSERT_EQ(std::string("\x1B[6 q"), steady_bar);

            // Ensure no stray 'm'
            ASSERT_TRUE(blinking_block.find('m') == std::string::npos);
        }

        static void test_rgb_overhead_generation() {
            GGUI::RGB color(1,2,3);

            // Pointer overload
            GGUI::INTERNAL::superString<GGUI::constants::ANSI::maximumNeededPreAllocationForOverHead> fg_overhead_ptr;
            color.getOverHeadAsSuperString(&fg_overhead_ptr, true);
            GGUI::INTERNAL::superString<GGUI::constants::ANSI::maximumNeededPreAllocationForOverHead> bg_overhead_ptr;
            color.getOverHeadAsSuperString(&bg_overhead_ptr, false);

            // Return-value overload
            auto fg_overhead_val = color.getOverHeadAsSuperString(true);
            auto bg_overhead_val = color.getOverHeadAsSuperString(false);

            ASSERT_EQ(std::string("\x1B[38;2;"), fg_overhead_ptr.toString());
            ASSERT_EQ(std::string("\x1B[48;2;"), bg_overhead_ptr.toString());
            ASSERT_EQ(fg_overhead_ptr.toString(), fg_overhead_val.toString());
            ASSERT_EQ(bg_overhead_ptr.toString(), bg_overhead_val.toString());
        }

        static void test_utf_superstring_transparency() {
            GGUI::UTF opaque('A', {GGUI::COLOR::RED, GGUI::COLOR::BLUE});
            std::string opaque_out = GGUI::toString(opaque, false);
            ASSERT_TRUE(opaque_out.find("\x1B[38;2;255;0;0m") != std::string::npos); // Foreground
            ASSERT_TRUE(opaque_out.find("\x1B[48;2;0;0;255m") != std::string::npos); // Background
            ASSERT_TRUE(opaque_out.find("\x1B[0m") != std::string::npos); // Reset

            GGUI::UTF transparent('B', {GGUI::COLOR::GREEN, GGUI::COLOR::BLUE});
            std::string transparent_out = GGUI::toString(transparent, true); // transparent background
            ASSERT_TRUE(transparent_out.find("\x1B[38;2;0;255;0m") != std::string::npos); // Foreground present
            ASSERT_TRUE(transparent_out.find("48;2;0;0;255") == std::string::npos); // Background sequence absent
            ASSERT_TRUE(transparent_out.find("\x1B[0m") != std::string::npos); // Still reset
        }

    };
}

#endif