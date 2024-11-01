#ifndef _SWITCH_H_
#define _SWITCH_H_

#include <vector>
#include <string>

#include "Button.h"

#include "../Core/Utils/Super_String.h"
#include "../Core/Utils/Constants.h"
#include "../Core/Utils/Color.h"
#include "../Core/Utils/Units.h"
#include "../Core/Utils/Event.h"
#include "../Core/Utils/Style.h"

namespace GGUI{
    class Switch : public Element{
    private:
        /**
         * @brief Private default constructor.
         * @details This constructor is not intended to be used by the user.
         * It is used to prevent the compiler from generating a default constructor.
         */
        // DONT GIVE TO USER !!!
        Switch(){}
    protected:
        bool State = false;

        //COntains the unchecked version of the symbol and the checked version.
        std::vector<std::string> States;

        Text_Field Text;
    public:
        /**
         * @brief Constructs a Switch element with specified text, states, event handler, and styling.
         * @param text The text to display on the switch.
         * @param states A vector containing the unchecked and checked states.
         * @param event The function to call when the switch is toggled.
         * @param s The styling for the switch.
         */
        Switch(std::string text, std::vector<std::string> states, std::function<void (Element* This)> event = []([[maybe_unused]] Element* e){}, Styling s = STYLES::CONSTANTS::Default);

        /**
         * @brief Renders the switch element and its children into the Render_Buffer nested buffer of the window.
         * @details This function processes the switch element to generate a vector of UTF objects representing the current state.
         * It handles different stains such as CLASS, STRETCH, COLOR, EDGE, and DEEP to ensure the switch element is rendered correctly.
         * @return A vector of UTF objects representing the rendered switch element.
         */
        std::vector<GGUI::UTF>& Render() override;

        /**
         * @brief Toggles the state of the switch.
         * @details Flips the current state from checked to unchecked or vice versa,
         * and marks the switch as needing a state update.
         */
        void Toggle() {
            // Flip the current state of the switch
            State = !State;

            // Mark the switch as needing a state update
            Dirty.Dirty(STAIN_TYPE::STATE);
        }

        /**
         * @brief Sets the text of the switch element.
         * @details This function sets the text of the switch element by first pausing the GGUI engine, then setting the text with a space character added to the beginning, and finally updating the switch element's dimensions to fit the new text. The text is then reset in the Render_Buffer nested buffer of the window.
         * @param text The new text for the switch element.
         */
        void Set_Text(std::string text);
        
        /**
         * @brief Creates a deep copy of the Switch object.
         * @details This function creates a new Switch object and copies all the data from the current Switch object to the new one.
         *          This is useful for creating a new Switch object that is a modified version of the current one.
         * @return A pointer to the new Switch object.
         */
        Element* Safe_Move() override {
            // Create a new Switch object
            Switch* new_Switch = new Switch();

            // Copy all data from the current Switch to the new Switch
            *new_Switch = *(Switch*)this;

            // Return the new Switch object
            return new_Switch;
        }

        /**
         * @brief Returns the name of the Switch object.
         * @details This function returns a string that represents the name of the Switch object.
         *          The name is constructed by concatenating the name of the Switch with the 
         *          class name "Switch", separated by a "<" and a ">".
         * @return The name of the Switch object.
         */
        std::string Get_Name() const override{
            return "Switch<" + Name + ">";
        }
    };

    class Radio_Button : public Switch{
    public:
        /**
         * @brief Constructs a Radio_Button object with the specified text.
         * @details A Radio_Button is a special type of Switch that can be either on or off.
         *          The text parameter is the text to display next to the radio button.
         * @param text The text to display next to the radio button.
         */
        Radio_Button(std::string text) : Switch(text, {SYMBOLS::RADIOBUTTON_OFF, SYMBOLS::RADIOBUTTON_ON}){}

        /**
         * @brief Destructor for the Radio_Button class.
         * @details This destructor is responsible for properly deallocating all the memory
         * allocated by the Radio_Button object. It calls the base class destructor
         * to ensure all parent class resources are also cleaned up.
         */
        ~Radio_Button() override{
            // call the base destructor.
            Element::~Element();
        }

        /**
         * @brief Returns the state of the Radio_Button.
         * @details This function returns a boolean value indicating whether the Radio_Button is turned on or off.
         *          The state is represented by the Switch::State property.
         * @return The state of the Radio_Button.
         */
        bool Get_State(){
            return State;
        }
        
        // The Swtich overrides it for us.
        //Element* Safe_Move() override;
        
        /**
         * @brief Returns the name of the Radio_Button object.
         * @details This function returns a string that represents the name of the Radio_Button object.
         *          The name is constructed by concatenating the name of the Radio_Button with the 
         *          class name "Radio_Button", separated by a "<" and a ">".
         * @return The name of the Radio_Button object.
         */
        std::string Get_Name() const override{
            // Return the formatted name of the Radio_Button.
            return "Radio_Button<" + Name + ">";
        }
    };

    class Check_Box : public Switch{
    public:
        /**
         * @brief Constructs a Check_Box object with the specified text.
         * @param text The text to display next to the check box.
         * @details A Check_Box is a special type of Switch that can be either checked or unchecked.
         *          The symbols for the unchecked and checked states are EMPTY_CHECK_BOX and CHECKED_CHECK_BOX, respectively.
         */
        Check_Box(std::string text) : Switch(text, {SYMBOLS::EMPTY_CHECK_BOX, SYMBOLS::CHECKED_CHECK_BOX}){}

        /**
         * @brief Returns the current state of the Radio_Button.
         * @details This function returns a boolean indicating whether the Radio_Button is on or off.
         * @return The state of the Radio_Button.
         */
        bool Get_State(){
            return State; // Return the current state of the Radio_Button.
        }
        
        // The Swtich overrides it for us.
        //Element* Safe_Move() override;

        /**
         * @brief Returns the name of the Check_Box object.
         * @details This function returns a string that represents the name of the Check_Box object.
         *          The name is constructed by concatenating the name of the Check_Box with the 
         *          class name "Check_Box", separated by a "<" and a ">".
         * @return The name of the Check_Box object.
         */
        std::string Get_Name() const override{
            return "Check_Box<" + Name + ">";
        }
    };
}

#endif