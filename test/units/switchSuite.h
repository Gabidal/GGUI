#ifndef _SWITCH_SUITE_H_
#define _SWITCH_SUITE_H_

#include "utils.h"
#include <ggui_dev.h>

// element class testing, no styling from style class, only internal styling member testing instead.

namespace tester {
    class switchSuite : public utils::TestSuite {
    public:
        switchSuite() : utils::TestSuite("Switch element behaviour") {
            add_test("test_box_state_remembrance", "Verify switchBox holds its state correctly after onClick", test_box_state_remembrance);
        }
    private:

        static void test_box_state_remembrance() {
            using namespace GGUI;

            switchBox SB(
                visualState("0", "1") | onClick([](element*){ return true; }) // Enable clicking and enter functions
            );

            SB.compile();

            // For an element to get onFocus it needs to be hovered upon first:
            INTERNAL::Mouse = {0, 0};            // Let's test it on all four corners
            INTERNAL::eventHandler();            // run pipeline 
            ASSERT_TRUE(SB.isHovered());         // now let's see if the hoverable has onHover enabled
        
            // Now we can create an click input ourselves and put it into the inputs list and call eventHandler to parse it
            INTERNAL::Inputs.push_back(new GGUI::input(0, constants::MOUSE_LEFT_CLICKED));
            INTERNAL::eventHandler();            // run pipeline

            // Now we can simply check if SB has isFocused enabled
            ASSERT_TRUE(SB.isFocused());

            // Now let's send the left click again and see if it changes the switchbox status 
            INTERNAL::Inputs.push_back(new GGUI::input(0, constants::MOUSE_LEFT_CLICKED));
            INTERNAL::eventHandler();            // run pipeline
            ASSERT_TRUE(SB.isSelected());

            // Now we can also test that pressing the switchBox again should disable it
            INTERNAL::Inputs.push_back(new GGUI::input(0, constants::MOUSE_LEFT_CLICKED));
            INTERNAL::eventHandler();            // run pipeline
            ASSERT_FALSE(SB.isSelected());
        }
    };
}

#endif