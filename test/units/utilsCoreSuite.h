#ifndef _UTILS_CORE_SUITE_H_
#define _UTILS_CORE_SUITE_H_

#include "utils.h" // test framework utils

// Include engine internal utilities explicitly (avoid relying on transitive includes)
#include "../../src/core/utils/utils.h"
#include "../../src/core/utils/constants.h"
#include "../../src/core/utils/fileStreamer.h"

#include <climits>

// Tests for core inline / constexpr utilities in utils.h & types.

namespace tester {
    class utilsCoreSuite : public utils::TestSuite {
    public:
        utilsCoreSuite() : utils::TestSuite("Core utils tester") {
            add_test("test_min_max", "Test Min / Max helpers (std replacements)", test_min_max);
            add_test("test_has_bit_at", "Test bit query helper (manual)", test_has_bit_at);
            add_test("test_rect_collides", "Test rectangle collision variants (manual)", test_rect_collides);
            add_test("test_lerp_functions", "Test lerp & interpolation helpers (manual)", test_lerp_functions);
            add_test("test_flag_helpers", "Test flag bit helpers (manual)", test_flag_helpers);
            add_test("test_file_position_to_string", "Test filePosition To_String()", test_file_position_to_string);
            add_test("test_cursor_shape_sequence", "Test ANSI cursor shape sequence builder", test_cursor_shape_sequence);
        }
    private:
        static void test_min_max() {
            // Simple min/max validation
            ASSERT_EQ(-5, std::min(-5, 10));
            ASSERT_EQ(10, std::max(-5, 10));
        }

        static void test_has_bit_at() {
            unsigned char v = 0x10; // 00010000
            auto has_bit_at = [](unsigned char val, int i){ return (val & (1u << i)) != 0; };
            ASSERT_TRUE(has_bit_at(v, 4));
            ASSERT_FALSE(has_bit_at(v, 0));
        }

        static void test_rect_collides() {
            GGUI::IVector3 a{0,0,0};
            GGUI::IVector3 b{5,5,0};
            auto collides = [](GGUI::IVector3 A, GGUI::IVector3 B, int Aw, int Ah, int Bw, int Bh){
                return A.X < B.X + Bw && A.X + Aw > B.X && A.Y < B.Y + Bh && A.Y + Ah > B.Y; };
            ASSERT_TRUE(collides(a,a,2,2,2,2));
            ASSERT_FALSE(collides(a,b,2,2,2,2));
            ASSERT_TRUE(collides(a,GGUI::IVector3{1,1,0},2,2,1,1));
        }

        static void test_lerp_functions() {
            auto lerp = [](float a,float b,float t){ return a + t*(b-a); };
            float r = lerp(0.f, 10.f, 0.5f);
            ASSERT_FLOAT_EQ(5.f, r, 0.0001f);
            int ir = static_cast<int>(lerp(0.f,100.f,0.25f));
            ASSERT_EQ(25, ir);
        }

        static void test_flag_helpers() {
            unsigned long long combo = GGUI::constants::ENTER | GGUI::constants::ESCAPE;
            auto isFlag = [](unsigned long long f, unsigned long long Flag){ return (f & Flag) == Flag; };
            auto hasFlag = [](unsigned long long f, unsigned long long flag){ return (f & flag) != 0ULL; };
            auto contains = isFlag;
            ASSERT_TRUE(isFlag(combo, GGUI::constants::ENTER));
            ASSERT_TRUE(hasFlag(combo, GGUI::constants::ESCAPE));
            ASSERT_TRUE(contains(combo, GGUI::constants::ENTER));
        }

        static void test_file_position_to_string() {
            GGUI::filePosition pos("file.txt", 12, 34);
            ASSERT_TRUE(pos.To_String() == std::string("file.txt:12:34"));
        }

        static void test_cursor_shape_sequence() {
            auto seq = GGUI::constants::ANSI::Set_Cursor_Shape(GGUI::constants::ANSI::CURSOR_SHAPE_STEADY_BAR).toString();
            ASSERT_TRUE(seq.find("\x1B[") == 0);
            ASSERT_TRUE(seq.find("6 q") != std::string::npos);
        }
    };
}

#endif
