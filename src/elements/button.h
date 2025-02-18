#ifndef _BUTTON_H_
#define _BUTTON_H_

#include <vector>
#include <string>

#include <functional>

#include "textField.h"

#include "../core/utils/superString.h"
#include "../core/utils/constants.h"
#include "../core/utils/color.h"
#include "../core/utils/units.h"
#include "../core/utils/event.h"
#include "../core/utils/style.h"

namespace GGUI{

    class button : public element{
    public:

        /**
         * @brief Constructs a Button element with specified text, action, and styling.
         * @param text The text to display on the button.
         * @param press The function to call when the button is pressed.
         * @param s The styling for the button.
         * @param Embed_Styles_On_Construct If true, the styling will be embedded into the button's style. Only use if you know what you're doing!!!
         */
        button(styling s = STYLES::CONSTANTS::Default, bool Embed_Styles_On_Construct = false);

        /**
         * @brief Creates a deep copy of the Button object.
         * @return A pointer to the new Button object.
         */
        element* safeMove() override {
            // Create a new Button object and copy all the data from the current Button object to the new one.
            return new button();
        }

        /**
         * @brief Gets the name of the Button object.
         * @return A string containing the name of the Button object.
         */
        std::string getName() const override{
            return "Button<" + Name + ">";
        }

        void setText(std::string Text){
            // There should always be an Text_Field child
            if (Style->Childs.size() == 0){
                INTERNAL::reportStack("No Text_Field child found in Button: " + getName());
            }

            ((textField*)Style->Childs.back())->setText(Text);
        }
    };
}

#endif