#ifndef _TEXT_FIELD_SUITE_H_
#define _TEXT_FIELD_SUITE_H_

#include "utils.h"
#include <ggui_dev.h>

// textField input pipeline and editing behaviour.

namespace tester {
    class textFieldSuite : public utils::TestSuite {
    public:
        textFieldSuite() : utils::TestSuite("Text field input behaviour") {
            add_test("mouse_focus_typing", "Typing characters through internal inputs updates textField text", test_mouse_focus_typing);
            add_test("backspace_removes_character", "Backspace input removes the trailing character", test_backspace_removes_character);
        }

    private:
        static void attachInputHandler(GGUI::textField& field) {
            field.input([](GGUI::textField* self, char c) {
                auto current = self->getText();
                current.push_back(c);
                self->setText(current);
            });
        }

        static void hoverElement(GGUI::element& el) {
            GGUI::INTERNAL::mouse = {0, 0, 0};
            GGUI::INTERNAL::eventHandler();
            ASSERT_TRUE(el.isHovered());
        }

        static void focusElement(GGUI::element& el) {
            hoverElement(el);
            GGUI::INTERNAL::inputs.push_back(new GGUI::input(0, GGUI::constants::MOUSE_LEFT_CLICKED));
            GGUI::INTERNAL::eventHandler();
            ASSERT_TRUE(el.isFocused());
        }

        static void test_mouse_focus_typing() {
            using namespace GGUI;

            textField field;
            field.compile();
            attachInputHandler(field);

            focusElement(field);

            const std::string typed = "Hi";
            for (char ch : typed) {
                INTERNAL::inputs.push_back(new input(ch, constants::KEY_PRESS));
                INTERNAL::eventHandler();
            }

            ASSERT_EQ(typed, field.getText());
        }

        static void test_backspace_removes_character() {
            using namespace GGUI;

            textField field;
            field.compile();
            attachInputHandler(field);

            focusElement(field);

            const std::string initial = "OK";
            for (char ch : initial) {
                INTERNAL::inputs.push_back(new input(ch, constants::KEY_PRESS));
                INTERNAL::eventHandler();
            }
            ASSERT_EQ(initial, field.getText());

            INTERNAL::KEYBOARD_STATES[KEYBOARD_BUTTONS::BACKSPACE] = INTERNAL::buttonState(true);
            INTERNAL::inputs.push_back(new input(' ', constants::BACKSPACE));
            INTERNAL::eventHandler();
            INTERNAL::KEYBOARD_STATES[KEYBOARD_BUTTONS::BACKSPACE] = INTERNAL::buttonState(false);

            ASSERT_EQ(std::string("O"), field.getText());
        }
    };
}

#endif
