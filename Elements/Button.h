#ifndef _BUTTON_H_
#define _BUTTON_H_

#include <vector>
#include <string>

#include <functional>

#include "Text_Field.h"

namespace GGUI{

    class Button : public Element{
    protected:
        void Default_Button_Behaviour(std::function<void (Button* This)> press = []([[maybe_unused]] Button* This){}){
            On_Click([=]([[maybe_unused]] Event* e){
                // The default, on_click wont do anything.
                press(this);

                return true;
            });
        }

        // DONT USE AS USER!!
        Button() : Element(){
            Childs.push_back(new Text_Field());
        }
    public:

        Button(std::string Text, std::function<void (Button* This)> press = []([[maybe_unused]] Button* This){});

        Element* Safe_Move() override {
            Button* new_Button = new Button();
            *new_Button = *(Button*)this;

            return new_Button;
        }

        std::string Get_Name() const override{
            return "Button<" + Name + ">";
        }
    };
}

#endif