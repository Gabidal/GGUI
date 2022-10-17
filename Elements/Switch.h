#ifndef _SWITCH_H_
#define _SWITCH_H_

#include <vector>
#include <string>

#include "Button.h"

namespace GGUI{
    class Switch : public Element{
    protected:
        bool State = false;

        Button Btn;
        //COntains the unchecked version of the symbol and the checked version.
        std::vector<std::string> States;

    public:
        Switch(std::string text, std::vector<std::string> states, std::function<void (Element* This)> event = [](Element* e){}){
            States = states;

            auto Togler = [=](){
                this->Toggle();

                event(this);
            };

            Btn.Set_Data(text);

            On_Click([=](Event* e){
                Togler();
            });

            int Symbol_Lenght = 1;

            Width = Symbol_Lenght + Btn.Get_Width();
            Height = Btn.Get_Height();

        }

        std::vector<UTF> Render() override;

        void Toggle(){
            State = !State;
        }
    };

    class Radio_Button : public Switch{
    public:
        Radio_Button(std::string text) : Switch(text, {SYMBOLS::RADIOBUTTON_OFF, SYMBOLS::RADIOBUTTON_ON}){}

        bool Get_State(){
            return State;
        }

    };

    class Check_Box : public Switch{
    public:
        Check_Box(std::string text) : Switch(text, {SYMBOLS::EMPTY_CHECK_BOX, SYMBOLS::CHECKED_CHECK_BOX}){}

        bool Get_State(){
            return State;
        }

    };
}

#endif