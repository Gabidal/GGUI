#ifndef _COLOR_SUITE_H_
#define _COLOR_SUITE_H_

#include "utils.h"
#include "../../src/core/utils/color.h"
#include "../../src/core/utils/constants.h"

// Tests for RGB colour operations (operators, blending) and basic constants.

namespace tester {
    class colorSuite : public utils::TestSuite {
    public:
        colorSuite() : utils::TestSuite("Color operations tester") {
            add_test("test_color_constants", "Validate predefined color constants", test_color_constants);
            add_test("test_color_equality_and_inequality", "Test RGB equality / inequality operators", test_color_equality_and_inequality);
            add_test("test_color_addition_wrap", "Test RGB channel addition (wrap behaviour)", test_color_addition_wrap);
            add_test("test_color_scalar_multiply", "Test RGB scalar multiply", test_color_scalar_multiply);
            add_test("test_color_inversion", "Test RGB inversion operator", test_color_inversion);
            add_test("test_color_alpha_blend", "Test in-place alpha blending add()", test_color_alpha_blend);
        }
    private:
        static void test_color_constants() {
            ASSERT_EQ((unsigned char)255, GGUI::COLOR::WHITE.Red);
            ASSERT_EQ((unsigned char)0, GGUI::COLOR::BLACK.Blue);
            ASSERT_TRUE(GGUI::COLOR::RED == GGUI::RGB(255,0,0));
            ASSERT_TRUE(GGUI::COLOR::TEAL == GGUI::RGB(0,128,128));
        }

        static void test_color_equality_and_inequality() {
            GGUI::RGB a(10,20,30);
            GGUI::RGB b(10,20,30);
            GGUI::RGB c(11,20,30);
            ASSERT_TRUE(a == b);
            ASSERT_FALSE(a == c);
            ASSERT_TRUE(a != c);
        }

        static void test_color_addition_wrap() {
            GGUI::RGB a(250,250,250);
            GGUI::RGB b(10,10,10);
            GGUI::RGB r = a + b; // unsigned char wrap expected 260 -> 4
            ASSERT_EQ((unsigned char)4, r.Red);
            ASSERT_EQ((unsigned char)4, r.Green);
            ASSERT_EQ((unsigned char)4, r.Blue);
        }

        static void test_color_scalar_multiply() {
            GGUI::RGB a(100,50,25);
            GGUI::RGB r = a * 0.5f; // truncation
            ASSERT_EQ((unsigned char)50, r.Red);
            ASSERT_EQ((unsigned char)25, r.Green);
            ASSERT_EQ((unsigned char)12, r.Blue); // 25*0.5 = 12.5 -> trunc
        }

        static void test_color_inversion() {
            GGUI::RGB a(0,128,255);
            GGUI::RGB r = !a;
            ASSERT_EQ((unsigned char)255, r.Red);
            ASSERT_EQ((unsigned char)127, r.Green); // 255-128
            ASSERT_EQ((unsigned char)0, r.Blue);
        }

        static void test_color_alpha_blend() {
            GGUI::RGB base(100,100,100);
            GGUI::RGB overlay(200,0,0);
            base.add(overlay, 0.5f); // expected: (100*0.5 + 200*0.5)=150, (100*0.5+0)=50
            ASSERT_EQ((unsigned char)150, base.Red);
            ASSERT_EQ((unsigned char)50, base.Green);
            ASSERT_EQ((unsigned char)50, base.Blue);
        }
    };
}

#endif
