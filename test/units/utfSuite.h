#ifndef _UTF_SUITE_H_
#define _UTF_SUITE_H_

#include "utils.h"
#include "../../src/core/utils/utf.h"
#include "../../src/core/utils/utils.h"

// Tests for UTF wrapper (flags, colour setting, text mutation).

namespace tester {
    class utfSuite : public utils::TestSuite {
    public:
        utfSuite() : utils::TestSuite("UTF wrapper tester") {
            add_test("test_utf_flag_setting", "Test setting and querying encoding flags", test_utf_flag_setting);
            add_test("test_utf_color_setting", "Test foreground/background color setters", test_utf_color_setting);
            add_test("test_utf_text_mutation", "Test setText overloads", test_utf_text_mutation);
            add_test("test_utf_to_string_basic", "Test ANSI encoding of a colored UTF", test_utf_to_string_basic);
        }
    private:
        static void test_utf_flag_setting() {
            GGUI::UTF u('A');
            u.setFlag(GGUI::INTERNAL::ENCODING_FLAG::START);
            ASSERT_TRUE(u.is(GGUI::INTERNAL::ENCODING_FLAG::START));
            u.setFlag(GGUI::INTERNAL::ENCODING_FLAG::END);
            ASSERT_TRUE(u.is(GGUI::INTERNAL::ENCODING_FLAG::END));
        }

        static void test_utf_color_setting() {
            GGUI::UTF u('B');
            u.setForeground(GGUI::COLOR::RED);
            u.setBackground(GGUI::COLOR::BLUE);
            // Encode and verify sequences present
            std::string encoded = GGUI::toString(u);
            ASSERT_TRUE(encoded.find("38;2;255;0;0") != std::string::npos);
            ASSERT_TRUE(encoded.find("48;2;0;0;255") != std::string::npos);
        }

        static void test_utf_text_mutation() {
            GGUI::UTF u('C');
            ASSERT_TRUE(u.is('C'));
            u.setText('D');
            ASSERT_TRUE(u.is('D'));
            u.setText("Hello");
            ASSERT_TRUE(u.is("Hello"));
        }

        static void test_utf_to_string_basic() {
            GGUI::UTF u('X', {GGUI::COLOR::GREEN, GGUI::COLOR::BLACK});
            std::string out = GGUI::toString(u);
            ASSERT_TRUE(out.find('X') != std::string::npos);
            ASSERT_TRUE(out.find("38;2;0;255;0") != std::string::npos);
            ASSERT_TRUE(out.find("48;2;0;0;0") != std::string::npos);
            ASSERT_TRUE(out.find("\x1B[0m") != std::string::npos); // reset
        }
    };
}

#endif
