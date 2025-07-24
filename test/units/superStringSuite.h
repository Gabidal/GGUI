#ifndef _SUPER_STRING_SUITE_H_
#define _SUPER_STRING_SUITE_H_

#include "utils.h"

// This test suite tests for validity of Super String and Compact String. 

namespace tester {
    class superStringSuite : public utils::TestSuite {
    public: 
        superStringSuite() : utils::TestSuite("super string tester") {
            add_test("test_compact_string_constructors", "Test compactString constructor variations", test_compact_string_constructors);
            add_test("test_compact_string_ascii", "Test compactString ASCII operations", test_compact_string_ascii);
            add_test("test_compact_string_unicode", "Test compactString Unicode operations", test_compact_string_unicode);
            add_test("test_compact_string_comparison", "Test compactString comparison methods", test_compact_string_comparison);
            add_test("test_compact_string_access", "Test compactString character access", test_compact_string_access);
            add_test("test_compact_string_getters", "Test compactString getter methods", test_compact_string_getters);
            add_test("test_compact_string_setters", "Test compactString setter methods", test_compact_string_setters);
            add_test("test_compact_string_utilities", "Test compactString utility methods", test_compact_string_utilities);
            add_test("test_super_string_constructors", "Test superString constructor variations", test_super_string_constructors);
            add_test("test_super_string_add_methods", "Test superString add methods", test_super_string_add_methods);
            add_test("test_super_string_clear", "Test superString clear functionality", test_super_string_clear);
            add_test("test_super_string_to_string", "Test superString toString conversion", test_super_string_to_string);
            add_test("test_super_string_templates", "Test superString template functionality", test_super_string_templates);
            add_test("test_compact_string_flags", "Test compactString flag operations", test_compact_string_flags);
            add_test("test_edge_cases", "Test edge cases and boundary conditions", test_edge_cases);
        }
    private:
        static void test_compact_string_constructors() {
            // Test default constructor
            GGUI::INTERNAL::compactString cs1;
            ASSERT_EQ((unsigned)0, cs1.size);

            // Test single character constructor
            GGUI::INTERNAL::compactString cs2('A');
            ASSERT_EQ((unsigned)1, cs2.size);
            ASSERT_TRUE(cs2.is(GGUI::INTERNAL::COMPACT_STRING_FLAG::IS_ASCII));
            ASSERT_EQ('A', cs2.getAscii());

            // Test C-string constructor (short string)
            GGUI::INTERNAL::compactString cs3("B");
            ASSERT_EQ((unsigned)1, cs3.size);
            ASSERT_TRUE(cs3.is(GGUI::INTERNAL::COMPACT_STRING_FLAG::IS_ASCII));
            ASSERT_EQ('B', cs3.getAscii());

            // Test C-string constructor (long string)
            GGUI::INTERNAL::compactString cs4("Hello");
            ASSERT_EQ((unsigned)5, cs4.size);
            ASSERT_TRUE(cs4.is(GGUI::INTERNAL::COMPACT_STRING_FLAG::IS_UNICODE));
            ASSERT_TRUE(cs4.is("Hello"));

            // Test constructor with size parameter
            GGUI::INTERNAL::compactString cs5("Test", 4);
            ASSERT_EQ((unsigned)4, cs5.size);

            // Test constructor with force unicode flag
            GGUI::INTERNAL::compactString cs6("X", 1, true);
            ASSERT_EQ((unsigned)1, cs6.size);
            ASSERT_TRUE(cs6.is(GGUI::INTERNAL::COMPACT_STRING_FLAG::IS_UNICODE));
            ASSERT_TRUE(cs6.is("X"));
        }

        static void test_compact_string_ascii() {
            // Test ASCII character storage
            GGUI::INTERNAL::compactString cs('Z');
            ASSERT_TRUE(cs.is(GGUI::INTERNAL::COMPACT_STRING_FLAG::IS_ASCII));
            ASSERT_FALSE(cs.is(GGUI::INTERNAL::COMPACT_STRING_FLAG::IS_UNICODE));
            ASSERT_EQ((unsigned)1, cs.size);

            // Test getAscii method
            ASSERT_EQ('Z', cs.getAscii());

            // Test setAscii method
            cs.setAscii('Y');
            ASSERT_EQ('Y', cs.getAscii());
            ASSERT_EQ((unsigned)1, cs.size);
        }

        static void test_compact_string_unicode() {
            // Test Unicode string storage
            GGUI::INTERNAL::compactString cs("Unicode");
            ASSERT_TRUE(cs.is(GGUI::INTERNAL::COMPACT_STRING_FLAG::IS_UNICODE));
            ASSERT_FALSE(cs.is(GGUI::INTERNAL::COMPACT_STRING_FLAG::IS_ASCII));
            ASSERT_EQ((unsigned)7, cs.size);

            // Test getUnicode method
            const char* unicode_text = cs.getUnicode();
            ASSERT_TRUE(unicode_text != nullptr);
            ASSERT_TRUE(std::string(unicode_text) == "Unicode");

            // Test setUnicode method
            cs.setUnicode("NewText");
            ASSERT_EQ((unsigned)7, cs.size);
            ASSERT_TRUE(std::string(cs.getUnicode()) == "NewText");
        }

        static void test_compact_string_comparison() {
            // Test character comparison
            GGUI::INTERNAL::compactString cs1('A');
            ASSERT_TRUE(cs1.is('A'));
            ASSERT_FALSE(cs1.is('B'));

            // Test string comparison
            GGUI::INTERNAL::compactString cs2("Hello");
            ASSERT_TRUE(cs2.is("Hello"));
            ASSERT_FALSE(cs2.is("World"));

            // Test comparison with wrong type
            ASSERT_FALSE(cs1.is("A")); // ASCII char vs string
            ASSERT_FALSE(cs2.is('H')); // Unicode string vs char
        }

        static void test_compact_string_access() {
            // Test subscript operator with ASCII
            GGUI::INTERNAL::compactString cs1('X');
            ASSERT_EQ('X', cs1[0]);
            ASSERT_EQ('\0', cs1[1]); // Out of bounds

            // Test subscript operator with Unicode
            GGUI::INTERNAL::compactString cs2("Test");
            ASSERT_EQ('T', cs2[0]);
            ASSERT_EQ('e', cs2[1]);
            ASSERT_EQ('s', cs2[2]);
            ASSERT_EQ('t', cs2[3]);
            ASSERT_EQ('\0', cs2[4]); // Out of bounds
            ASSERT_EQ('\0', cs2[-1]); // Negative index
        }

        static void test_compact_string_getters() {
            // Test getAscii with ASCII string
            GGUI::INTERNAL::compactString cs1('M');
            ASSERT_EQ('M', cs1.getAscii());

            // Test getAscii with Unicode string (should return null char)
            GGUI::INTERNAL::compactString cs2("Multi");
            ASSERT_EQ('\0', cs2.getAscii());

            // Test getUnicode with Unicode string
            const char* unicode_str = cs2.getUnicode();
            ASSERT_TRUE(unicode_str != nullptr);
            ASSERT_TRUE(std::string(unicode_str) == "Multi");

            // Test getUnicode with ASCII string (should return nullptr)
            ASSERT_EQ((const char*)nullptr, cs1.getUnicode());
        }

        static void test_compact_string_setters() {
            GGUI::INTERNAL::compactString cs;

            // Test setAscii
            cs.setAscii('K');
            ASSERT_EQ('K', cs.getAscii());
            ASSERT_EQ((unsigned)1, cs.size);
            ASSERT_TRUE(cs.is(GGUI::INTERNAL::COMPACT_STRING_FLAG::IS_ASCII));

            // Test setUnicode
            cs.setUnicode("Testing");
            ASSERT_TRUE(std::string(cs.getUnicode()) == "Testing");
            ASSERT_EQ((unsigned)7, cs.size);
            ASSERT_TRUE(cs.is(GGUI::INTERNAL::COMPACT_STRING_FLAG::IS_UNICODE));
        }

        static void test_compact_string_utilities() {
            // Test empty method
            GGUI::INTERNAL::compactString cs1;
            ASSERT_TRUE(cs1.empty());

            GGUI::INTERNAL::compactString cs2('A');
            ASSERT_FALSE(cs2.empty());

            GGUI::INTERNAL::compactString cs3("Text");
            ASSERT_FALSE(cs3.empty());

            // Test hasDefaultText method
            GGUI::INTERNAL::compactString cs4(' ');
            ASSERT_TRUE(cs4.hasDefaultText());

            GGUI::INTERNAL::compactString cs5(" Default");
            ASSERT_TRUE(cs5.hasDefaultText());

            GGUI::INTERNAL::compactString cs6('X');
            ASSERT_FALSE(cs6.hasDefaultText());

            GGUI::INTERNAL::compactString cs7("NoSpace");
            ASSERT_FALSE(cs7.hasDefaultText());
        }

        static void test_super_string_constructors() {
            // Test default constructor
            GGUI::INTERNAL::superString<10> ss1;
            ASSERT_EQ((unsigned)0, ss1.currentIndex);
            ASSERT_EQ((unsigned)0, ss1.liquefiedSize);

            // Test initializer list constructor
            GGUI::INTERNAL::compactString cs1('A');
            GGUI::INTERNAL::compactString cs2("Hello");
            GGUI::INTERNAL::superString<10> ss2{cs1, cs2};
            ASSERT_EQ((unsigned)2, ss2.currentIndex);
            ASSERT_EQ((unsigned)6, ss2.liquefiedSize); // 1 + 5
        }

        static void test_super_string_add_methods() {
            GGUI::INTERNAL::superString<20> ss;

            // Test add character
            ss.add('A');
            ASSERT_EQ((unsigned)1, ss.currentIndex);
            ASSERT_EQ((unsigned)1, ss.liquefiedSize);

            // Test add string with size
            ss.add("Hello", 5);
            ASSERT_EQ((unsigned)2, ss.currentIndex);
            ASSERT_EQ((unsigned)6, ss.liquefiedSize);

            // Test add compactString
            GGUI::INTERNAL::compactString cs("World");
            ss.add(cs);
            ASSERT_EQ((unsigned)3, ss.currentIndex);
            ASSERT_EQ((unsigned)11, ss.liquefiedSize);

            // Test add another superString (by pointer)
            GGUI::INTERNAL::superString<10> ss2;
            ss2.add('!');
            ss2.add('?');
            ss.add(&ss2);
            ASSERT_EQ((unsigned)5, ss.currentIndex);
            ASSERT_EQ((unsigned)13, ss.liquefiedSize);

            // Test add another superString (by reference)
            GGUI::INTERNAL::superString<10> ss3;
            ss3.add("End");
            ss.add(ss3);
            ASSERT_EQ((unsigned)6, ss.currentIndex);
            ASSERT_EQ((unsigned)16, ss.liquefiedSize);
        }

        static void test_super_string_clear() {
            GGUI::INTERNAL::superString<10> ss;
            
            // Add some content
            ss.add('A');
            ss.add("Hello");
            ASSERT_EQ((unsigned)2, ss.currentIndex);
            ASSERT_EQ((unsigned)6, ss.liquefiedSize);

            // Clear and verify
            ss.clear();
            ASSERT_EQ((unsigned)0, ss.currentIndex);
            ASSERT_EQ((unsigned)0, ss.liquefiedSize);
        }

        static void test_super_string_to_string() {
            GGUI::INTERNAL::superString<10> ss;

            // Test empty toString
            std::string result = ss.toString();
            ASSERT_TRUE(result.empty());

            // Add mixed content
            ss.add('H');
            ss.add("ello");
            ss.add(' ');
            ss.add("World");

            // Test toString
            result = ss.toString();
            ASSERT_TRUE(result == "Hello World");
            ASSERT_EQ((size_t)11, result.length());
        }

        static void test_super_string_templates() {
            // Test different template sizes
            GGUI::INTERNAL::superString<5> small_ss;
            GGUI::INTERNAL::superString<100> large_ss;

            small_ss.add("Small");
            large_ss.add("Large");

            ASSERT_EQ((unsigned)1, small_ss.currentIndex);
            ASSERT_EQ((unsigned)1, large_ss.currentIndex);

            // Test adding from different template sizes
            large_ss.add(small_ss);
            ASSERT_EQ((unsigned)2, large_ss.currentIndex);
            ASSERT_EQ((unsigned)10, large_ss.liquefiedSize); // "Large" + "Small"
        }

        static void test_compact_string_flags() {
            // Test flag constants
            ASSERT_EQ((unsigned char)1, GGUI::INTERNAL::COMPACT_STRING_FLAG::IS_ASCII);
            ASSERT_EQ((unsigned char)2, GGUI::INTERNAL::COMPACT_STRING_FLAG::IS_UNICODE);

            // Test is() method with flags
            GGUI::INTERNAL::compactString ascii_cs('A');
            GGUI::INTERNAL::compactString unicode_cs("Unicode");

            ASSERT_TRUE(ascii_cs.is(GGUI::INTERNAL::COMPACT_STRING_FLAG::IS_ASCII));
            ASSERT_FALSE(ascii_cs.is(GGUI::INTERNAL::COMPACT_STRING_FLAG::IS_UNICODE));

            ASSERT_FALSE(unicode_cs.is(GGUI::INTERNAL::COMPACT_STRING_FLAG::IS_ASCII));
            ASSERT_TRUE(unicode_cs.is(GGUI::INTERNAL::COMPACT_STRING_FLAG::IS_UNICODE));
        }

        static void test_edge_cases() {
            // Test empty string - skip null string test as it may cause issues
            GGUI::INTERNAL::compactString cs2("");
            ASSERT_EQ((unsigned)1, cs2.size);
            ASSERT_EQ('\0', cs2[0]); // Empty string should return null character

            // Test single character string vs char constructor
            GGUI::INTERNAL::compactString cs3("A");
            GGUI::INTERNAL::compactString cs4('A');
            ASSERT_EQ(cs3.size, cs4.size);
            ASSERT_EQ(cs3[0], cs4[0]);

            // Test boundary access
            GGUI::INTERNAL::compactString cs5("Test");
            ASSERT_EQ('\0', cs5[100]);
            ASSERT_EQ('\0', cs5[-10]);

            // Test superString at capacity
            GGUI::INTERNAL::superString<2> small_ss;
            small_ss.add('A');
            small_ss.add('B');
            ASSERT_EQ((unsigned)2, small_ss.currentIndex);

            // Test copy constructors and assignments
            GGUI::INTERNAL::compactString original("Original");
            GGUI::INTERNAL::compactString copy1(original);
            GGUI::INTERNAL::compactString copy2;
            copy2 = original;

            ASSERT_TRUE(copy1.is("Original"));
            ASSERT_TRUE(copy2.is("Original"));
        }
    };
}

#endif