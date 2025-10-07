#ifndef _COLOR_SUITE_H_
#define _COLOR_SUITE_H_

#include "utils.h"
#include <ggui_dev.h>

// Tests for RGB colour operations (operators, blending) and basic constants.

namespace tester {
    class colorSuite : public utils::TestSuite {
    public:
        colorSuite() : utils::TestSuite("Color operations tester") {
            add_test("test_color_instantiation_value", "Test RGB instantiation and default value", test_color_instantiation_value);
            add_test("test_color_blending", "Test RGB color blending", test_color_blending);
        }
    private:
        static void test_color_instantiation_value() {
            ASSERT_EQ(GGUI::COLOR::BLACK, GGUI::RGB{});
        }

        static void test_color_blending() {
            const GGUI::RGB black = GGUI::COLOR::BLACK;
            const GGUI::RGB white = GGUI::COLOR::WHITE;
            const GGUI::RGB gray = GGUI::COLOR::GRAY;
            const GGUI::RGB correct_to_gray = {1, 1, 1};    // UINT8_MAX / 2 -> 127 not 128, so we offset it by one.

            ASSERT_EQ((black * 0.5f + white * 0.5f) + correct_to_gray, gray);
            ASSERT_EQ(gray + black, gray);
            ASSERT_FALSE(white == black);
            ASSERT_TRUE(white != black);
        }
    };
}

#endif
