#ifndef _BUTTON_H_
#define _BUTTON_H_

#include <vector>
#include <string>

#include <functional>

#include "Text_Field.h"

#include "../Core/Utils/Super_String.h"
#include "../Core/Utils/Constants.h"
#include "../Core/Utils/Color.h"
#include "../Core/Utils/Units.h"
#include "../Core/Utils/Event.h"
#include "../Core/Utils/Style.h"

namespace GGUI{

    class Button : public Element{
    public:

        /**
         * @brief Constructs a Button element with specified text, action, and styling.
         * @param text The text to display on the button.
         * @param press The function to call when the button is pressed.
         * @param s The styling for the button.
         * @param Embed_Styles_On_Construct If true, the styling will be embedded into the button's style. Only use if you know what you're doing!!!
         */
        Button(Styling s = STYLES::CONSTANTS::Default, bool Embed_Styles_On_Construct = false);

        /**
         * @brief Creates a deep copy of the Button object.
         * @return A pointer to the new Button object.
         */
        Element* Safe_Move() override {
            // Create a new Button object and copy all the data from the current Button object to the new one.
            return new Button();
        }

        /**
         * @brief Gets the name of the Button object.
         * @return A string containing the name of the Button object.
         */
        std::string Get_Name() const override{
            return "Button<" + Name + ">";
        }

        void Set_Text(std::string Text){
            // There should always be an Text_Field child
            if (Style->Childs.size() == 0){
                Report_Stack("No Text_Field child found in Button: " + Get_Name());
            }

            ((Text_Field*)Style->Childs.back())->Set_Text(Text);
        }
    };
}

#endif