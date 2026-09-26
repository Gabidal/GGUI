#ifndef _SWITCH_H_
#define _SWITCH_H_

#include <vector>
#include <string>

#include "textField.h"

namespace GGUI{
    namespace SYMBOLS {
        constexpr std::string_view RADIOBUTTON_OFF = "○";
        constexpr std::string_view RADIOBUTTON_ON = "◉";
    
        constexpr std::string_view EMPTY_CHECK_BOX = "☐";
        constexpr std::string_view CHECKED_CHECK_BOX = "☒";
    }

    struct visualState {
        terminal::cell Off, On;
    };

    class switchBox : public element {
    protected:
        bool State = false;
        bool SingleSelect = false;   // Represents whether switching this box should disable other single selected switchBoxes under the same container.

        //Contains the unchecked version of the symbol and the checked version.
        terminal::cell Off = ' ', On = ' ';

        textField Text;
    public:
        switchBox() = default;

        ~switchBox() override = default;

        void toggle();

        void setState(bool b);

        void enableSingleSelect();

        bool isSingleSelect() { return SingleSelect; }

        bool isSelected() { return State; }

        void setText(std::string& text);

        std::string getTypedName() const override{
            return "switchBox<" + ID + ">";
        }

        constexpr terminal::cell getStateString() const {
            return State ? On : Off;
        }

        void setStateString(terminal::cell off, terminal::cell on);

        void DisableOthers();
    protected:
        std::vector<terminal::cell>& render() override;

        element* createInstance() const override {
            return new switchBox();
        }
    };

    class radioButton : public switchBox {
    public:
        bool getState(){
            return State;
        }
        
        std::string getTypedName() const override{
            // Return the formatted name of the Radio_Button.
            return "radioButton<" + ID + ">";
        }
    };

    class checkBox : public switchBox {
    public:
        bool getState(){
            return State; // Return the current state of the Check_Box.
        }

        std::string getTypedName() const override{
            return "checkBox<" + ID + ">";
        }
    };
}

#endif