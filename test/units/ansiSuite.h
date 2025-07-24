#ifndef _ANSI_SUITE_H_
#define _ANSI_SUITE_H_

#include "utils.h"

// This test suite is designed to test that GGUI generates correct ANSI escape sequence data.

namespace tester {
    class ansiSuite : public utils::TestSuite {
    public:
        ansiSuite() : utils::TestSuite("ANSI escape sequence tester") {
            add_test("test_ansi_constants", "Test ANSI constant values", test_ansi_constants);
            add_test("test_sgr_feature_enabling", "Test SGR feature enabling", test_sgr_feature_enabling);
            add_test("test_private_sgr_feature_enabling", "Test private SGR feature enabling", test_private_sgr_feature_enabling);
            add_test("test_rgb_color_generation", "Test RGB color ANSI generation", test_rgb_color_generation);
            add_test("test_rgb_overhead_generation", "Test RGB overhead ANSI generation", test_rgb_overhead_generation);
            add_test("test_utf_character_color_encoding", "Test UTF character color encoding", test_utf_character_color_encoding);
            add_test("test_predefined_colors", "Test predefined color constants", test_predefined_colors);
            add_test("test_clear_and_reset_sequences", "Test clear and reset ANSI sequences", test_clear_and_reset_sequences);
            add_test("test_cursor_control_sequences", "Test cursor control ANSI sequences", test_cursor_control_sequences);
            add_test("test_text_styling_sgr_codes", "Test text styling SGR codes", test_text_styling_sgr_codes);
            add_test("test_utf_single_character_colorization", "Test UTF single character colorization", test_utf_single_character_colorization);
            add_test("test_utf_string_colorization", "Test UTF string colorization", test_utf_string_colorization);
            add_test("test_utf_complex_color_combinations", "Test UTF complex color combinations", test_utf_complex_color_combinations);
            add_test("test_utf_edge_case_colors", "Test UTF edge case colors", test_utf_edge_case_colors);
        }

    private:
        static void test_ansi_constants() {
            // Test basic ANSI escape sequence constants
            ASSERT_TRUE(std::string(GGUI::constants::ANSI::ESC_CODE.getUnicode()) == "\x1B[");
            ASSERT_EQ(';', GGUI::constants::ANSI::SEPARATE.getAscii());
            ASSERT_EQ('2', GGUI::constants::ANSI::USE_RGB.getAscii());
            ASSERT_EQ('m', GGUI::constants::ANSI::END_COMMAND.getAscii());
            ASSERT_TRUE(std::string(GGUI::constants::ANSI::RESET_COLOR.getUnicode()) == "\x1B[0m");
            
            // Test control sequences
            ASSERT_TRUE(std::string(GGUI::constants::ANSI::CLEAR_SCREEN.getUnicode()) == "\x1B[2J");
            ASSERT_TRUE(std::string(GGUI::constants::ANSI::CLEAR_SCROLLBACK.getUnicode()) == "\x1B[3J");
            ASSERT_TRUE(std::string(GGUI::constants::ANSI::SET_CURSOR_TO_START.getUnicode()) == "\x1B[H");
            ASSERT_TRUE(std::string(GGUI::constants::ANSI::RESET_CONSOLE.getUnicode()) == "\x1B[c");
        }

        static void test_sgr_feature_enabling() {
            // Test enabling bold feature
            auto bold_enabled = GGUI::constants::ANSI::Enable_SGR_Feature(GGUI::constants::ANSI::BOLD);
            std::string expected_bold = "\x1B[1m";
            ASSERT_TRUE(bold_enabled.toString() == expected_bold);

            // Test enabling underline feature
            auto underline_enabled = GGUI::constants::ANSI::Enable_SGR_Feature(GGUI::constants::ANSI::UNDERLINE);
            std::string expected_underline = "\x1B[4m";
            ASSERT_TRUE(underline_enabled.toString() == expected_underline);

            // Test enabling italic feature
            auto italic_enabled = GGUI::constants::ANSI::Enable_SGR_Feature(GGUI::constants::ANSI::ITALIC);
            std::string expected_italic = "\x1B[3m";
            ASSERT_TRUE(italic_enabled.toString() == expected_italic);

            // Test reset SGR
            auto reset_sgr = GGUI::constants::ANSI::Enable_SGR_Feature(GGUI::constants::ANSI::RESET_SGR);
            std::string expected_reset = "\x1B[0m";
            ASSERT_TRUE(reset_sgr.toString() == expected_reset);
        }

        static void test_private_sgr_feature_enabling() {
            // Test enabling mouse cursor (private SGR feature)
            auto mouse_cursor_enabled = GGUI::constants::ANSI::Enable_Private_SGR_Feature(GGUI::constants::ANSI::MOUSE_CURSOR, true);
            std::string expected_enable = "\x1B[?25h";
            ASSERT_TRUE(mouse_cursor_enabled.toString() == expected_enable);

            // Test disabling mouse cursor
            auto mouse_cursor_disabled = GGUI::constants::ANSI::Enable_Private_SGR_Feature(GGUI::constants::ANSI::MOUSE_CURSOR, false);
            std::string expected_disable = "\x1B[?25l";
            ASSERT_TRUE(mouse_cursor_disabled.toString() == expected_disable);

            // Test enabling alternative screen buffer
            auto alt_screen_enabled = GGUI::constants::ANSI::Enable_Private_SGR_Feature(GGUI::constants::ANSI::ALTERNATIVE_SCREEN_BUFFER, true);
            std::string expected_alt_enable = "\x1B[?1049h";
            ASSERT_TRUE(alt_screen_enabled.toString() == expected_alt_enable);

            // Test disabling alternative screen buffer
            auto alt_screen_disabled = GGUI::constants::ANSI::Enable_Private_SGR_Feature(GGUI::constants::ANSI::ALTERNATIVE_SCREEN_BUFFER, false);
            std::string expected_alt_disable = "\x1B[?1049l";
            ASSERT_TRUE(alt_screen_disabled.toString() == expected_alt_disable);
        }

        static void test_rgb_color_generation() {
            // Test RGB color generation for red
            GGUI::RGB red_color(255, 0, 0);
            GGUI::INTERNAL::superString<GGUI::constants::ANSI::maximumNeededPreAllocationForColor> color_string;
            red_color.getColourAsSuperString(&color_string);
            
            std::string expected_red = "255;0;0";
            ASSERT_TRUE(color_string.toString() == expected_red);

            // Test RGB color generation for green
            GGUI::RGB green_color(0, 255, 0);
            GGUI::INTERNAL::superString<GGUI::constants::ANSI::maximumNeededPreAllocationForColor> green_string;
            green_color.getColourAsSuperString(&green_string);
            
            std::string expected_green = "0;255;0";
            ASSERT_TRUE(green_string.toString() == expected_green);

            // Test RGB color generation for blue
            GGUI::RGB blue_color(0, 0, 255);
            GGUI::INTERNAL::superString<GGUI::constants::ANSI::maximumNeededPreAllocationForColor> blue_string;
            blue_color.getColourAsSuperString(&blue_string);
            
            std::string expected_blue = "0;0;255";
            ASSERT_TRUE(blue_string.toString() == expected_blue);

            // Test RGB color generation for custom color
            GGUI::RGB custom_color(128, 64, 192);
            GGUI::INTERNAL::superString<GGUI::constants::ANSI::maximumNeededPreAllocationForColor> custom_string;
            custom_color.getColourAsSuperString(&custom_string);
            
            std::string expected_custom = "128;64;192";
            ASSERT_TRUE(custom_string.toString() == expected_custom);
        }

        static void test_rgb_overhead_generation() {
            // Test text color overhead generation
            GGUI::RGB test_color(100, 150, 200);
            GGUI::INTERNAL::superString<GGUI::constants::ANSI::maximumNeededPreAllocationForOverHead> text_overhead;
            test_color.getOverHeadAsSuperString(&text_overhead, true);
            
            std::string expected_text_overhead = "\x1B[38;2;";
            ASSERT_TRUE(text_overhead.toString() == expected_text_overhead);

            // Test background color overhead generation
            GGUI::INTERNAL::superString<GGUI::constants::ANSI::maximumNeededPreAllocationForOverHead> bg_overhead;
            test_color.getOverHeadAsSuperString(&bg_overhead, false);
            
            std::string expected_bg_overhead = "\x1B[48;2;";
            ASSERT_TRUE(bg_overhead.toString() == expected_bg_overhead);
        }

        static void test_utf_character_color_encoding() {
            // Test UTF character with red foreground and black background
            GGUI::UTF colored_char('A', {GGUI::COLOR::RED, GGUI::COLOR::BLACK});
            auto* result = colored_char.toSuperString();
            
            // The result should contain ANSI escape codes for red text on black background
            std::string ansi_result = result->toString();
            
            // Should contain escape sequence for red foreground (38;2;255;0;0)
            ASSERT_TRUE(ansi_result.find("38;2;255;0;0") != std::string::npos);
            // Should contain escape sequence for black background (48;2;0;0;0)
            ASSERT_TRUE(ansi_result.find("48;2;0;0;0") != std::string::npos);
            // Should contain the character 'A'
            ASSERT_TRUE(ansi_result.find('A') != std::string::npos);
            // Should contain reset sequence
            ASSERT_TRUE(ansi_result.find("\x1B[0m") != std::string::npos);

            delete result;

            // Test UTF character with blue foreground and white background
            GGUI::UTF blue_char('B', {GGUI::COLOR::BLUE, GGUI::COLOR::WHITE});
            auto* blue_result = blue_char.toSuperString();
            
            std::string blue_ansi_result = blue_result->toString();
            
            // Should contain escape sequence for blue foreground (38;2;0;0;255)
            ASSERT_TRUE(blue_ansi_result.find("38;2;0;0;255") != std::string::npos);
            // Should contain escape sequence for white background (48;2;255;255;255)
            ASSERT_TRUE(blue_ansi_result.find("48;2;255;255;255") != std::string::npos);
            // Should contain the character 'B'
            ASSERT_TRUE(blue_ansi_result.find('B') != std::string::npos);

            delete blue_result;
        }

        static void test_predefined_colors() {
            // Test predefined color values using operator== 
            ASSERT_TRUE(GGUI::COLOR::WHITE == GGUI::RGB(255, 255, 255));
            ASSERT_TRUE(GGUI::COLOR::BLACK == GGUI::RGB(0, 0, 0));
            ASSERT_TRUE(GGUI::COLOR::RED == GGUI::RGB(255, 0, 0));
            ASSERT_TRUE(GGUI::COLOR::GREEN == GGUI::RGB(0, 255, 0));
            ASSERT_TRUE(GGUI::COLOR::BLUE == GGUI::RGB(0, 0, 255));
            ASSERT_TRUE(GGUI::COLOR::YELLOW == GGUI::RGB(255, 255, 0));
            ASSERT_TRUE(GGUI::COLOR::ORANGE == GGUI::RGB(255, 128, 0));
            ASSERT_TRUE(GGUI::COLOR::CYAN == GGUI::RGB(0, 255, 255));
            ASSERT_TRUE(GGUI::COLOR::TEAL == GGUI::RGB(0, 128, 128));
            ASSERT_TRUE(GGUI::COLOR::MAGENTA == GGUI::RGB(255, 0, 255));
            ASSERT_TRUE(GGUI::COLOR::GRAY == GGUI::RGB(128, 128, 128));
            
            // Test individual RGB components
            ASSERT_EQ((unsigned char)255, GGUI::COLOR::WHITE.Red);
            ASSERT_EQ((unsigned char)255, GGUI::COLOR::WHITE.Green);
            ASSERT_EQ((unsigned char)255, GGUI::COLOR::WHITE.Blue);
            
            ASSERT_EQ((unsigned char)0, GGUI::COLOR::BLACK.Red);
            ASSERT_EQ((unsigned char)0, GGUI::COLOR::BLACK.Green);
            ASSERT_EQ((unsigned char)0, GGUI::COLOR::BLACK.Blue);
        }

        static void test_clear_and_reset_sequences() {
            // Test that clear screen sequence is correctly formatted
            ASSERT_TRUE(std::string(GGUI::constants::ANSI::CLEAR_SCREEN.getUnicode()) == "\x1B[2J");

            // Test that clear scrollback sequence is correctly formatted
            ASSERT_TRUE(std::string(GGUI::constants::ANSI::CLEAR_SCROLLBACK.getUnicode()) == "\x1B[3J");

            // Test that reset color sequence is correctly formatted
            ASSERT_TRUE(std::string(GGUI::constants::ANSI::RESET_COLOR.getUnicode()) == "\x1B[0m");

            // Test that reset console sequence is correctly formatted
            ASSERT_TRUE(std::string(GGUI::constants::ANSI::RESET_CONSOLE.getUnicode()) == "\x1B[c");
        }

        static void test_cursor_control_sequences() {
            // Test cursor positioning sequence
            ASSERT_TRUE(std::string(GGUI::constants::ANSI::SET_CURSOR_TO_START.getUnicode()) == "\x1B[H");
        }

        static void test_text_styling_sgr_codes() {
            // Test SGR code values are correct strings
            // For single character constants, we need to check if they're ASCII or Unicode
            if (GGUI::constants::ANSI::RESET_SGR.is(GGUI::INTERNAL::COMPACT_STRING_FLAG::IS_ASCII)) {
                ASSERT_EQ('0', GGUI::constants::ANSI::RESET_SGR.getAscii());
            } else {
                ASSERT_TRUE(std::string(GGUI::constants::ANSI::RESET_SGR.getUnicode()) == "0");
            }
            
            if (GGUI::constants::ANSI::BOLD.is(GGUI::INTERNAL::COMPACT_STRING_FLAG::IS_ASCII)) {
                ASSERT_EQ('1', GGUI::constants::ANSI::BOLD.getAscii());
            } else {
                ASSERT_TRUE(std::string(GGUI::constants::ANSI::BOLD.getUnicode()) == "1");
            }
            
            if (GGUI::constants::ANSI::FAINT.is(GGUI::INTERNAL::COMPACT_STRING_FLAG::IS_ASCII)) {
                ASSERT_EQ('2', GGUI::constants::ANSI::FAINT.getAscii());
            } else {
                ASSERT_TRUE(std::string(GGUI::constants::ANSI::FAINT.getUnicode()) == "2");
            }
            
            if (GGUI::constants::ANSI::ITALIC.is(GGUI::INTERNAL::COMPACT_STRING_FLAG::IS_ASCII)) {
                ASSERT_EQ('3', GGUI::constants::ANSI::ITALIC.getAscii());
            } else {
                ASSERT_TRUE(std::string(GGUI::constants::ANSI::ITALIC.getUnicode()) == "3");
            }
            
            if (GGUI::constants::ANSI::UNDERLINE.is(GGUI::INTERNAL::COMPACT_STRING_FLAG::IS_ASCII)) {
                ASSERT_EQ('4', GGUI::constants::ANSI::UNDERLINE.getAscii());
            } else {
                ASSERT_TRUE(std::string(GGUI::constants::ANSI::UNDERLINE.getUnicode()) == "4");
            }

            // Test multi-character constants (these should be Unicode)
            ASSERT_TRUE(std::string(GGUI::constants::ANSI::TEXT_COLOR.getUnicode()) == "38");
            ASSERT_TRUE(std::string(GGUI::constants::ANSI::DEFAULT_TEXT_COLOR.getUnicode()) == "39");
            ASSERT_TRUE(std::string(GGUI::constants::ANSI::BACKGROUND_COLOR.getUnicode()) == "48");
            ASSERT_TRUE(std::string(GGUI::constants::ANSI::DEFAULT_BACKGROUND_COLOR.getUnicode()) == "49");
            ASSERT_TRUE(std::string(GGUI::constants::ANSI::PRIMARY_FONT.getUnicode()) == "10");
            ASSERT_TRUE(std::string(GGUI::constants::ANSI::ALTERNATIVE_FONT_1.getUnicode()) == "11");
            ASSERT_TRUE(std::string(GGUI::constants::ANSI::FRAKTUR.getUnicode()) == "20");
            ASSERT_TRUE(std::string(GGUI::constants::ANSI::NOT_BOLD.getUnicode()) == "21");
            ASSERT_TRUE(std::string(GGUI::constants::ANSI::NORMAL_INTENSITY.getUnicode()) == "22");
            ASSERT_TRUE(std::string(GGUI::constants::ANSI::NOT_UNDERLINE.getUnicode()) == "23");
            ASSERT_TRUE(std::string(GGUI::constants::ANSI::NOT_BLINK.getUnicode()) == "24");
            ASSERT_TRUE(std::string(GGUI::constants::ANSI::INVERT_INVERT_FOREGROUND_WITH_BACKGROUND.getUnicode()) == "27");
        }

        static void test_utf_single_character_colorization() {
            // Test single character with basic colors
            GGUI::UTF red_a('A', {GGUI::COLOR::RED, GGUI::COLOR::BLACK});
            auto* result = red_a.toSuperString();
            std::string output = result->toString();
            
            // Should contain: ESC[38;2;255;0;0mESC[48;2;0;0;0mA ESC[0m
            ASSERT_TRUE(output.find("\x1B[38;2;255;0;0m") != std::string::npos); // Red foreground
            ASSERT_TRUE(output.find("\x1B[48;2;0;0;0m") != std::string::npos);   // Black background
            ASSERT_TRUE(output.find('A') != std::string::npos);                   // Character
            ASSERT_TRUE(output.find("\x1B[0m") != std::string::npos);            // Reset
            
            delete result;

            // Test single character with white on blue
            GGUI::UTF white_x('X', {GGUI::COLOR::WHITE, GGUI::COLOR::BLUE});
            auto* blue_result = white_x.toSuperString();
            std::string blue_output = blue_result->toString();
            
            ASSERT_TRUE(blue_output.find("\x1B[38;2;255;255;255m") != std::string::npos); // White foreground
            ASSERT_TRUE(blue_output.find("\x1B[48;2;0;0;255m") != std::string::npos);     // Blue background
            ASSERT_TRUE(blue_output.find('X') != std::string::npos);                      // Character
            
            delete blue_result;

            // Test special characters
            GGUI::UTF special_char('@', {GGUI::COLOR::YELLOW, GGUI::COLOR::MAGENTA});
            auto* special_result = special_char.toSuperString();
            std::string special_output = special_result->toString();
            
            ASSERT_TRUE(special_output.find("\x1B[38;2;255;255;0m") != std::string::npos); // Yellow foreground
            ASSERT_TRUE(special_output.find("\x1B[48;2;255;0;255m") != std::string::npos); // Magenta background
            ASSERT_TRUE(special_output.find('@') != std::string::npos);                    // Character
            
            delete special_result;
        }

        static void test_utf_string_colorization() {
            // Test string with colors
            GGUI::UTF hello_string("Hello", {GGUI::COLOR::GREEN, GGUI::COLOR::WHITE});
            auto* result = hello_string.toSuperString();
            std::string output = result->toString();
            
            // Should contain green text on white background
            ASSERT_TRUE(output.find("\x1B[38;2;0;255;0m") != std::string::npos);   // Green foreground
            ASSERT_TRUE(output.find("\x1B[48;2;255;255;255m") != std::string::npos); // White background
            ASSERT_TRUE(output.find("Hello") != std::string::npos);                  // Text content
            ASSERT_TRUE(output.find("\x1B[0m") != std::string::npos);               // Reset
            
            delete result;

            // Test longer string with custom colors
            GGUI::UTF custom_string("Testing ANSI", {GGUI::RGB(128, 64, 192), GGUI::RGB(255, 200, 100)});
            auto* custom_result = custom_string.toSuperString();
            std::string custom_output = custom_result->toString();
            
            ASSERT_TRUE(custom_output.find("\x1B[38;2;128;64;192m") != std::string::npos); // Custom foreground
            ASSERT_TRUE(custom_output.find("\x1B[48;2;255;200;100m") != std::string::npos); // Custom background
            ASSERT_TRUE(custom_output.find("Testing ANSI") != std::string::npos);           // Text content
            
            delete custom_result;

            // Test empty string handling
            GGUI::UTF empty_string("", {GGUI::COLOR::RED, GGUI::COLOR::BLACK});
            auto* empty_result = empty_string.toSuperString();
            std::string empty_output = empty_result->toString();
            
            // Should still contain color codes even with empty text
            ASSERT_TRUE(empty_output.find("\x1B[38;2;255;0;0m") != std::string::npos); // Red foreground
            ASSERT_TRUE(empty_output.find("\x1B[48;2;0;0;0m") != std::string::npos);   // Black background
            ASSERT_TRUE(empty_output.find("\x1B[0m") != std::string::npos);            // Reset
            
            delete empty_result;
        }

        static void test_utf_complex_color_combinations() {
            // Test with orange and teal combination
            GGUI::UTF orange_teal('*', {GGUI::COLOR::ORANGE, GGUI::COLOR::TEAL});
            auto* result = orange_teal.toSuperString();
            std::string output = result->toString();
            
            ASSERT_TRUE(output.find("\x1B[38;2;255;128;0m") != std::string::npos);  // Orange foreground
            ASSERT_TRUE(output.find("\x1B[48;2;0;128;128m") != std::string::npos);  // Teal background
            ASSERT_TRUE(output.find('*') != std::string::npos);                     // Character
            
            delete result;

            // Test with gray foreground and cyan background
            GGUI::UTF gray_cyan("TEST", {GGUI::COLOR::GRAY, GGUI::COLOR::CYAN});
            auto* gray_result = gray_cyan.toSuperString();
            std::string gray_output = gray_result->toString();
            
            ASSERT_TRUE(gray_output.find("\x1B[38;2;128;128;128m") != std::string::npos); // Gray foreground
            ASSERT_TRUE(gray_output.find("\x1B[48;2;0;255;255m") != std::string::npos);   // Cyan background
            ASSERT_TRUE(gray_output.find("TEST") != std::string::npos);                   // Text
            
            delete gray_result;

            // Test with identical foreground and background colors
            GGUI::UTF same_colors('H', {GGUI::COLOR::RED, GGUI::COLOR::RED});
            auto* same_result = same_colors.toSuperString();
            std::string same_output = same_result->toString();
            
            // Both foreground and background should be red
            ASSERT_TRUE(same_output.find("\x1B[38;2;255;0;0m") != std::string::npos); // Red foreground
            ASSERT_TRUE(same_output.find("\x1B[48;2;255;0;0m") != std::string::npos); // Red background
            ASSERT_TRUE(same_output.find('H') != std::string::npos);                  // Character
            
            delete same_result;
        }

        static void test_utf_edge_case_colors() {
            // Test with extreme color values (black and white)
            GGUI::UTF black_white('B', {GGUI::COLOR::BLACK, GGUI::COLOR::WHITE});
            auto* result = black_white.toSuperString();
            std::string output = result->toString();
            
            ASSERT_TRUE(output.find("\x1B[38;2;0;0;0m") != std::string::npos);       // Black foreground
            ASSERT_TRUE(output.find("\x1B[48;2;255;255;255m") != std::string::npos); // White background
            ASSERT_TRUE(output.find('B') != std::string::npos);                      // Character
            
            delete result;

            // Test with custom RGB edge values (max values)
            GGUI::UTF max_colors('M', {GGUI::RGB(255, 255, 255), GGUI::RGB(0, 0, 0)});
            auto* max_result = max_colors.toSuperString();
            std::string max_output = max_result->toString();
            
            ASSERT_TRUE(max_output.find("\x1B[38;2;255;255;255m") != std::string::npos); // Max RGB foreground
            ASSERT_TRUE(max_output.find("\x1B[48;2;0;0;0m") != std::string::npos);       // Min RGB background
            ASSERT_TRUE(max_output.find('M') != std::string::npos);                      // Character
            ASSERT_TRUE(max_output.find("\033[0m") != std::string::npos);                // Reset
            
            delete max_result;

            // Test with mid-range RGB values
            GGUI::UTF mid_colors("Mid", {GGUI::RGB(127, 127, 127), GGUI::RGB(128, 128, 128)});
            auto* mid_result = mid_colors.toSuperString();
            std::string mid_output = mid_result->toString();
            
            ASSERT_TRUE(mid_output.find("\x1B[38;2;127;127;127m") != std::string::npos); // Mid-range foreground
            ASSERT_TRUE(mid_output.find("\x1B[48;2;128;128;128m") != std::string::npos); // Slightly different background
            ASSERT_TRUE(mid_output.find("Mid") != std::string::npos);                    // Text
            
            delete mid_result;

            // Test with zero RGB values
            GGUI::UTF zero_colors('0', {GGUI::RGB(0, 0, 0), GGUI::RGB(1, 1, 1)});
            auto* zero_result = zero_colors.toSuperString();
            std::string zero_output = zero_result->toString();
            
            ASSERT_TRUE(zero_output.find("\x1B[38;2;0;0;0m") != std::string::npos); // Zero RGB foreground
            ASSERT_TRUE(zero_output.find("\x1B[48;2;1;1;1m") != std::string::npos); // Minimal RGB background
            ASSERT_TRUE(zero_output.find('0') != std::string::npos);                // Character
            
            delete zero_result;
        }
    };
}

#endif