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

        //COntains the unchecked version of the symbol and the checked version.
        std::vector<std::string> States;

        Text_Field Text;
    public:
        Switch(std::string text, std::vector<std::string> states, std::function<void (Element* This)> event = []([[maybe_unused]] Element* e){}, Styling s = STYLES::CONSTANTS::Default);

        std::vector<GGUI::UTF>& Render() override;

        void Toggle(){
            State = !State;

            Dirty.Dirty(STAIN_TYPE::STATE);
        }

        void Set_Text(std::string text);
        
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

        ~Radio_Button() override{
            // call the base destructor.
            Element::~Element();
        }

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