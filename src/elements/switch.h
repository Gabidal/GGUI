#ifndef _SWITCH_H_
#define _SWITCH_H_

#include <vector>
#include <string>

#include "../core/utils/superString.h"
#include "../core/utils/constants.h"
#include "../core/utils/color.h"
#include "../core/utils/utf.h"
#include "../core/utils/style.h"
#include "textField.h"

namespace GGUI{

    class visualState : public STYLING_INTERNAL::styleBase {
    public:
        const INTERNAL::compactString *Off, *On;

        constexpr visualState(const INTERNAL::compactString& off, const INTERNAL::compactString& on, const VALUE_STATE Default = VALUE_STATE::VALUE) : styleBase(Default), Off(&off), On(&on) {}

        constexpr visualState(const GGUI::visualState& other) : styleBase(other.status), Off(other.Off), On(other.On) {}

        inline ~visualState() override { styleBase::~styleBase(); }

        inline styleBase* copy() const override {
            return new visualState(*this);
        }

        constexpr visualState& operator=(const visualState& other){
            // Only copy the information if the other is enabled.
            if (other.status >= status){
                Off = other.Off;
                On = other.On;

                status = other.status;
            }
            return *this;
        }

        inline void evaluate([[maybe_unused]] const styling* self, [[maybe_unused]] const styling* owner) override {};

        INTERNAL::STAIN_TYPE embedValue(styling* host, element* owner) override;
    };

    class singleSelect : public STYLING_INTERNAL::styleBase {
    public:
        constexpr singleSelect(const VALUE_STATE Default = VALUE_STATE::VALUE) : styleBase(Default) {}

        constexpr singleSelect(const GGUI::singleSelect& other) : styleBase(other.status) {}

        inline ~singleSelect() override { styleBase::~styleBase(); }

        inline styleBase* copy() const override {
            return new singleSelect(*this);
        }

        constexpr singleSelect& operator=(const singleSelect& other){
            // Only copy the information if the other is enabled.
            if (other.status >= status){
                status = other.status;
            }
            return *this;
        }

        inline void evaluate([[maybe_unused]] const styling* self, [[maybe_unused]] const styling* owner) override {};

        INTERNAL::STAIN_TYPE embedValue(styling* host, element* owner) override;
    };

    class switchBox : public element{
    protected:
        bool State = false;
        bool SingleSelect = false;   // Represents whether switching this box should disable other single selected switchBoxes under the same parent.

        //Contains the unchecked version of the symbol and the checked version.
        const INTERNAL::compactString *Off = nullptr, *On = nullptr;

        textField Text;
    public:
        /**
         * @brief Constructs a Switch element with specified text, states, event handler, and styling.
         * @param s The styling for the switch.
         * @param Embed_Styles_On_Construct If true, the styling will be embedded into the switch's style. Only use if you know what you're doing!!!
         */
        switchBox(STYLING_INTERNAL::styleBase& s = STYLES::CONSTANTS::Default, bool Embed_Styles_On_Construct = false);
        switchBox(STYLING_INTERNAL::styleBase&& s, bool Embed_Styles_On_Construct = false) : switchBox(s, Embed_Styles_On_Construct){}

        ~switchBox() override{
            // call the base destructor.
            element::~element();
        }

        /**
         * @brief Renders the switch element and its children into the Render_Buffer nested buffer of the window.
         * @details This function processes the switch element to generate a vector of UTF objects representing the current state.
         * It handles different stains such as CLASS, STRETCH, COLOR, EDGE, and DEEP to ensure the switch element is rendered correctly.
         * @return A vector of UTF objects representing the rendered switch element.
         */
        std::vector<GGUI::UTF>& render() override;

        /**
         * @brief Toggles the state of the switch.
         * @details Flips the current state from checked to unchecked or vice versa,
         * and marks the switch as needing a state update.
         */
        void toggle();

        void setState(bool b);

        void enableSingleSelect();

        bool isSingleSelect() { return SingleSelect; }

        bool isSelected() { return State; }

        /**
         * @brief Sets the text of the switch element.
         * @details This function sets the text of the switch element by first pausing the GGUI engine, then setting the text with a space character added to the beginning, and finally updating the switch element's dimensions to fit the new text. The text is then reset in the Render_Buffer nested buffer of the window.
         * @param text The new text for the switch element.
         */
        void setText(INTERNAL::compactString text);

        void showBorder(bool b) override;
        
        /**
         * @brief Creates a deep copy of the Switch object.
         * @details This function creates a new Switch object and copies all the data from the current Switch object to the new one.
         *          This is useful for creating a new Switch object that is a modified version of the current one.
         * @return A pointer to the new Switch object.
         */
        element* createInstance() const override {
            return new switchBox();
        }

        /**
         * @brief Returns the name of the Switch object.
         * @details This function returns a string that represents the name of the Switch object.
         *          The name is constructed by concatenating the name of the Switch with the 
         *          class name "Switch", separated by a "<" and a ">".
         * @return The name of the Switch object.
         */
        std::string getName() const override{
            return "switchBox<" + Name + ">";
        }

        constexpr INTERNAL::compactString getStateString() const {
            return State ? *On : *Off;
        }

        void setStateString(const INTERNAL::compactString* off, const INTERNAL::compactString* on);
    };

    class radioButton : public switchBox{
    public:
        /**
         * @brief Constructs a radioButton element with optional custom styling and embedding behavior.
         *
         * This constructor initializes a radioButton, inheriting from switchBox, with the specified style and visual state.
         * The visual state is set to display the appropriate radio button symbols for "off" and "on" states.
         *
         * @param s The style to apply to the radioButton. Defaults to STYLES::CONSTANTS::Default.
         * @param Embed_Styles_On_Construct If true, embeds the styles during construction. Defaults to false.
         */
        radioButton(STYLING_INTERNAL::styleBase& s = STYLES::CONSTANTS::Default, bool Embed_Styles_On_Construct = false) : 
            switchBox(s | visualState(SYMBOLS::RADIOBUTTON_OFF, SYMBOLS::RADIOBUTTON_ON), Embed_Styles_On_Construct) {}
        radioButton(STYLING_INTERNAL::styleBase&& s, bool Embed_Styles_On_Construct = false) : radioButton(s, Embed_Styles_On_Construct){}

        /**
         * @brief Returns the state of the Radio_Button.
         * @details This function returns a boolean value indicating whether the Radio_Button is turned on or off.
         *          The state is represented by the Switch::State property.
         * @return The state of the Radio_Button.
         */
        bool getState(){
            return State;
        }
        
        /**
         * @brief Returns the name of the Radio_Button object.
         * @details This function returns a string that represents the name of the Radio_Button object.
         *          The name is constructed by concatenating the name of the Radio_Button with the 
         *          class name "Radio_Button", separated by a "<" and a ">".
         * @return The name of the Radio_Button object.
         */
        std::string getName() const override{
            // Return the formatted name of the Radio_Button.
            return "radioButton<" + Name + ">";
        }

        // Diabled, use the switchBox class type for search
        // element* createInstance() const override {
        //     return new radioButton();
        // }
    };

    class checkBox : public switchBox{
    public:
        /**
         * @brief Constructs a checkBox element with optional styling and embedding behavior.
         *
         * This constructor initializes a checkBox by applying the provided style and visual states
         * for checked and unchecked symbols. It also allows specifying whether to embed styles upon construction.
         *
         * @param s The style to apply to the checkBox. Defaults to STYLES::CONSTANTS::Default.
         * @param Embed_Styles_On_Construct If true, embeds styles during construction. Defaults to false.
         */
        checkBox(STYLING_INTERNAL::styleBase& s = STYLES::CONSTANTS::Default, bool Embed_Styles_On_Construct = false) : 
            switchBox(s | visualState({SYMBOLS::EMPTY_CHECK_BOX, SYMBOLS::CHECKED_CHECK_BOX}), Embed_Styles_On_Construct) {}
        checkBox(STYLING_INTERNAL::styleBase&& s, bool Embed_Styles_On_Construct = false) : checkBox(s, Embed_Styles_On_Construct) {}

        /**
         * @brief Returns the current state of the Check_Box.
         * @details This function returns a boolean indicating whether the Check_Box is checked or unchecked.
         * @return The state of the Check_Box.
         */
        bool getState(){
            return State; // Return the current state of the Check_Box.
        }
        
        /**
         * @brief Returns the name of the Check_Box object.
         * @details This function returns a string that represents the name of the Check_Box object.
         *          The name is constructed by concatenating the name of the Check_Box with the 
         *          class name "Check_Box", separated by a "<" and a ">".
         * @return The name of the Check_Box object.
         */
        std::string getName() const override{
            return "checkBox<" + Name + ">";
        }

        // Disabled, use the switchBox class type.
        // element* createInstance() const override {
        //     return new checkBox();
        // }
    };

    namespace INTERNAL{
        void DisableOthers(switchBox* keepOn);
    }
}

#endif