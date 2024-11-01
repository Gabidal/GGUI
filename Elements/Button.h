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
    protected:
        /**
         * @brief Sets default button behavior to just call the provided lambda and do nothing else.
         * @param press The lambda to call when the button is clicked.
         */
        void Default_Button_Behaviour(std::function<void (Button* This)> press = [](Button*){}){
            On_Click([this, press](Event*){
                // The default, on_click wont do anything.
                // It will call the provided lambda (if any) and return true (allowing the event to propagate).

                press(this);

                return true;
            });
        }

        /**
         * @brief A constructor for the Button class that should not be used by users.
         * This constructor is for internal use only.
         * It sets up the Button class to use a Text_Field as its child.
         */
        Button() : Element(){
            Style->Childs.push_back(new Text_Field());
        }
    public:

        /**
         * @brief Constructs a Button element with specified text, action, and styling.
         * @param text The text to display on the button.
         * @param press The function to call when the button is pressed.
         * @param s The styling for the button.
         */
        Button(std::string Text, std::function<void (Button* This)> press = [](Button*){}, Styling s = STYLES::CONSTANTS::Default);

        /**
         * @brief Creates a deep copy of the Button object.
         * @return A pointer to the new Button object.
         */
        Element* Safe_Move() override {
            // Create a new Button object and copy all the data from the current Button object to the new one.
            Button* new_Button = new Button();
            *new_Button = *(Button*)this;

            return new_Button;
        }

        /**
         * @brief Gets the name of the Button object.
         * @return A string containing the name of the Button object.
         */
        std::string Get_Name() const override{
            return "Button<" + Name + ">";
        }
    };
}

#endif