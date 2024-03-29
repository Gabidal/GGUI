#ifndef _SWITCH_H_
#define _SWITCH_H_

#include <vector>
#include <string>

#include "Button.h"

namespace GGUI{
    class Switch : public Element{
    private:
        // DONT GIVE TO USER !!!
        Switch(){}
    protected:
        bool State = false;

        std::string Text = "";
        //COntains the unchecked version of the symbol and the checked version.
        std::vector<std::string> States;

    public:
        Switch(std::string text, std::vector<std::string> states, std::function<void (Element* This)> event = [](Element* e){});

        std::vector<UTF> Render() override;

        void Toggle(){
            State = !State;

            Dirty.Dirty(STAIN_TYPE::STATE);
        }

        std::string Get_Data() { return Text; }

        void Set_Data(std::string data) { Text = data; Dirty.Dirty(STAIN_TYPE::TEXT); }
        
        Element* Safe_Move() override {
            Switch* new_Switch = new Switch();
            *new_Switch = *(Switch*)this;

            return new_Switch;
        }

        std::string Get_Name() const override{
            return "Switch<" + Name + ">";
        }
    };

    class Radio_Button : public Switch{
    public:
        Radio_Button(std::string text) : Switch(text, {SYMBOLS::RADIOBUTTON_OFF, SYMBOLS::RADIOBUTTON_ON}){}

        bool Get_State(){
            return State;
        }
        
        // The Swtich overrides it for us.
        //Element* Safe_Move() override;
        
        std::string Get_Name() const override{
            return "Radio_Button<" + Name + ">";
        }
    };

    class Check_Box : public Switch{
    public:
        Check_Box(std::string text) : Switch(text, {SYMBOLS::EMPTY_CHECK_BOX, SYMBOLS::CHECKED_CHECK_BOX}){}

        bool Get_State(){
            return State;
        }
        
        // The Swtich overrides it for us.
        //Element* Safe_Move() override;

        std::string Get_Name() const override{
            return "Check_Box<" + Name + ">";
        }
    };
}

#endif