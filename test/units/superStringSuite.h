#ifndef _SUPER_STRING_SUITE_H_
#define _SUPER_STRING_SUITE_H_

#include "utils.h"

// This test suite tests for validity of Super String and Compact String. 

namespace tester {
    class superStringSuite : public utils::TestSuite {
    public: 
        superStringSuite() : utils::TestSuite("super string tester") {
            add_test("compact_default_constructor", "Default constructed compactString is empty (size 0)", [](){
                GGUI::INTERNAL::compactString cs; // size should be 0
                ASSERT_EQ((size_t)0, cs.size);
                ASSERT_TRUE(cs.empty());
            });

            add_test("compact_char_ptr_empty", "Construct from empty C-string becomes ASCII with \\0", [](){
                const char* empty = ""; // length 0 -> stored as ASCII '\\0'
                GGUI::INTERNAL::compactString cs(empty);
                ASSERT_EQ((size_t)1, cs.size); // implementation sets to 1 via setAscii('\\0')
                ASSERT_EQ('\0', cs.text[0]);
            });

            add_test("compact_char_ptr_single", "Single char C-string stored as ASCII", [](){
                const char* one = "A";
                GGUI::INTERNAL::compactString cs(one);
                ASSERT_EQ((size_t)1, cs.size);
                ASSERT_EQ('A', cs.text[0]);
                ASSERT_TRUE(cs.is('A'));
                ASSERT_FALSE(cs.is("A")); // not unicode path
            });

            add_test("compact_char_ptr_multi", "Multi char C-string stored as unicode pointer", [](){
                const char* word = "Hello";
                GGUI::INTERNAL::compactString cs(word);
                ASSERT_EQ((size_t)5, cs.size);
                ASSERT_TRUE(cs.is("Hello"));
                ASSERT_EQ('H', cs[0]);
                ASSERT_EQ('o', cs[4]);
            });

            add_test("compact_char_constructor", "Single char constructor", [](){
                GGUI::INTERNAL::compactString cs('Z');
                ASSERT_EQ((size_t)1, cs.size);
                ASSERT_EQ('Z', cs.text[0]);
            });

            add_test("compact_explicit_size_unicode", "Explicit size constructor chooses unicode when size>1", [](){
                const char* txt = "Hi";
                GGUI::INTERNAL::compactString cs(txt, 2);
                ASSERT_EQ((size_t)2, cs.size);
                ASSERT_TRUE(cs.is("Hi"));
            });

            add_test("compact_force_unicode_small", "Force unicode with size 1", [](){
                const char* txt = "X"; // size parameter 1, force unicode
                GGUI::INTERNAL::compactString cs(txt, 1, true);
                ASSERT_EQ((size_t)1, cs.size); // overridden to Size
                ASSERT_EQ('X', cs[0]);
            });

            add_test("compact_operator_index_bounds", "Index operator returns \\0 for OOB and valid for in-range", [](){
                const char* txt = "World";
                GGUI::INTERNAL::compactString cs(txt);
                ASSERT_EQ('W', cs[0]);
                ASSERT_EQ('d', cs[4]);
                ASSERT_EQ('\0', cs[5]); // out of range
                ASSERT_EQ('\0', cs[-1]); // negative
            });

            add_test("compact_setters", "setAscii and setUnicode adjust size", [](){
                GGUI::INTERNAL::compactString cs('A');
                ASSERT_EQ((size_t)1, cs.size);
                cs.set('B');
                ASSERT_EQ((size_t)1, cs.size);
                ASSERT_EQ('B', cs.text[0]);
                cs.set("Hello");
                ASSERT_EQ((size_t)5, cs.size);
            });

            add_test("compact_has_default_text_ascii_space", "hasDefaultText true for ASCII space", [](){
                GGUI::INTERNAL::compactString cs(' ');
                ASSERT_TRUE(cs.hasDefaultText());
            });

            add_test("compact_has_default_text_unicode_space", "hasDefaultText true when first unicode char space", [](){
                GGUI::INTERNAL::compactString cs(" test");
                ASSERT_TRUE(cs.hasDefaultText());
            });

            // superString tests
            add_test("super_default_constructor", "superString default empty", [](){
                GGUI::INTERNAL::superString<8> ss;
                ASSERT_EQ((size_t)0, ss.currentIndex);
                ASSERT_EQ((size_t)0, ss.liquefiedSize);
            });

            add_test("super_add_char", "Adding chars increments size and liquefiedSize", [](){
                GGUI::INTERNAL::superString<8> ss;
                ss.add('A');
                ss.add('B');
                ASSERT_EQ((size_t)2, ss.currentIndex);
                ASSERT_EQ((size_t)2, ss.liquefiedSize);
                ASSERT_EQ(std::string("AB"), ss.toString());
            });

            add_test("super_add_cstring", "Adding C-string with size", [](){
                GGUI::INTERNAL::superString<8> ss;
                ss.add("Hi", 2);
                ASSERT_EQ((size_t)1, ss.currentIndex);
                ASSERT_EQ((size_t)2, ss.liquefiedSize);
                ASSERT_EQ(std::string("Hi"), ss.toString());
            });

            add_test("super_add_compactString", "Adding compactString", [](){
                GGUI::INTERNAL::superString<8> ss;
                GGUI::INTERNAL::compactString cs("Hello");
                ss.add(cs);
                ASSERT_EQ((size_t)1, ss.currentIndex);
                ASSERT_EQ((size_t)5, ss.liquefiedSize);
                ASSERT_EQ(std::string("Hello"), ss.toString());
            });

            add_test("super_add_multiple_mixed", "Adding mixed entries unicode + ascii", [](){
                GGUI::INTERNAL::superString<8> ss;
                ss.add("Hi", 2); // unicode
                ss.add(' ');
                ss.add("World", 5);
                ASSERT_EQ((size_t)3, ss.currentIndex);
                ASSERT_EQ((size_t)2 + 1 + 5, ss.liquefiedSize);
                ASSERT_EQ(std::string("Hi World"), ss.toString());
            });

            add_test("super_initializer_list", "Initializer list constructor", [](){
                GGUI::INTERNAL::superString<5> ss{ GGUI::INTERNAL::compactString("A"), GGUI::INTERNAL::compactString("BC"), GGUI::INTERNAL::compactString('D') };
                ASSERT_EQ((size_t)3, ss.currentIndex);
                ASSERT_EQ((size_t)1 + 2 + 1, ss.liquefiedSize);
                ASSERT_EQ(std::string("ABCD"), ss.toString());
            });

            add_test("super_add_super_by_ref", "Add another superString by const ref", [](){
                GGUI::INTERNAL::superString<8> a; a.add("Hi",2); a.add(' ');
                GGUI::INTERNAL::superString<8> b; b.add("There",5);
                a.add(b);
                ASSERT_EQ((size_t)3, a.currentIndex);
                ASSERT_EQ((size_t)2 + 1 + 5, a.liquefiedSize);
                ASSERT_EQ(std::string("Hi There"), a.toString());
            });

            add_test("super_add_super_by_ptr", "Add another superString by pointer", [](){
                GGUI::INTERNAL::superString<8> a; a.add('X');
                GGUI::INTERNAL::superString<8> b; b.add('Y'); b.add('Z');
                a.add(&b);
                ASSERT_EQ((size_t)3, a.currentIndex);
                ASSERT_EQ((size_t)3, a.liquefiedSize);
                ASSERT_EQ(std::string("XYZ"), a.toString());
            });

            add_test("super_clear", "clear resets indices and allows reuse", [](){
                GGUI::INTERNAL::superString<8> ss; ss.add('A'); ss.add('B');
                ss.clear();
                ASSERT_EQ((size_t)0, ss.currentIndex);
                ASSERT_EQ((size_t)0, ss.liquefiedSize);
                ss.add('C');
                ASSERT_EQ(std::string("C"), ss.toString());
            });

            add_test("super_truncation_on_default_compact", "Encountering size 0 compactString stops toString loop", [](){
                GGUI::INTERNAL::superString<8> ss;
                ss.add('A');
                // Manually inject a default compactString with size 0 (break condition) then another char
                ss.data[ss.currentIndex++] = GGUI::INTERNAL::compactString(); // size=0
                ss.add('B');
                // liquefiedSize counts all adds including later ones
                ASSERT_EQ((size_t)3, ss.currentIndex);
                ASSERT_EQ((size_t)1 + 0 + 1, ss.liquefiedSize); // second added size 0, third size 1
                // toString stops copying once it encounters size 0; trailing capacity remains default (\0)
                auto result = ss.toString();
                ASSERT_EQ('A', result[0]);
                ASSERT_EQ('\0', result[1]); // second char is null due to pre-sized buffer
                ASSERT_EQ(static_cast<size_t>(2), result.size());
            });

            add_test("super_many_small_parts", "Many single chars concatenated", [](){
                GGUI::INTERNAL::superString<32> ss;
                std::string expected;
                for(char c='a'; c<'a'+10; ++c){
                    ss.add(c);
                    expected.push_back(c);
                }
                ASSERT_EQ((size_t)10, ss.currentIndex);
                ASSERT_EQ((size_t)10, ss.liquefiedSize);
                ASSERT_EQ(expected, ss.toString());
            });

            add_test("super_unicode_and_ascii_mix_access", "Ensure internal ordering preserved", [](){
                GGUI::INTERNAL::superString<10> ss;
                ss.add("AB",2);
                ss.add('C');
                ss.add("DEF",3);
                ss.add('G');
                ASSERT_EQ(std::string("ABCDEFG"), ss.toString());
            });
        }
    private:
        
    };
}

#endif