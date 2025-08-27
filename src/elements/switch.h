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
    protected:
        const INTERNAL::compactString *Off, *On;
    public:

        /**
         * @brief Constructs a visualState object with specified off and on states and a default value state.
         * 
         * @param off A compact string representing the "off" visual state.
         * @param on A compact string representing the "on" visual state.
         * @param Default The default value state, which is of type VALUE_STATE. Defaults to VALUE_STATE::VALUE.
         */
        constexpr visualState(const INTERNAL::compactString& off, const INTERNAL::compactString& on, const VALUE_STATE Default = VALUE_STATE::VALUE) : styleBase(Default), Off(&off), On(&on) {}

        /**
         * @brief Constructs a `visualState` object as a constexpr by copying the values from another `GGUI::visualState` object.
         * 
         * @param other The `GGUI::visualState` object to copy from. Its `status`, `Off`, and `On` properties are used to initialize the new object.
         */
        constexpr visualState(const GGUI::visualState& other) : styleBase(other.status), Off(other.Off), On(other.On) {}

        /**
         * @brief Destructor for the visualState class.
         * 
         * This inline destructor explicitly calls the destructor of the base class
         * styleBase to ensure proper cleanup of resources. The override specifier
         * indicates that this destructor overrides a virtual destructor in the base class.
         */
        inline ~visualState() override { styleBase::~styleBase(); }

        /**
         * @brief Creates a copy of the current visualState object.
         * 
         * This method overrides the copy function from the base class
         * and returns a dynamically allocated copy of the current 
         * visualState instance. The caller is responsible for managing 
         * the memory of the returned object.
         * 
         * @return styleBase* A pointer to a newly created copy of the 
         * current visualState object.
         */
        inline styleBase* copy() const override {
            return new visualState(*this);
        }

        /**
         * @brief Overloaded assignment operator for the visualState class.
         * 
         * This operator assigns the values from another visualState object to the current object,
         * but only if the `status` of the other object is greater than or equal to the `status` 
         * of the current object. If the condition is met, it copies the `Off` and `On` states 
         * as well as the `status` value from the other object.
         * 
         * @param other The visualState object to copy from.
         * @return A reference to the current visualState object after assignment.
         */
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
        /**
         * @brief Constructs a singleSelect object with a default value state.
         * 
         * @param Default The default value state for the singleSelect object. 
         *                It is of type VALUE_STATE and defaults to VALUE_STATE::VALUE.
         */
        constexpr singleSelect(const VALUE_STATE Default = VALUE_STATE::VALUE) : styleBase(Default) {}

        /**
         * @brief Constructs a `singleSelect` object as a constexpr copy of another `singleSelect` object.
         * 
         * @param other The `singleSelect` object to copy from.
         * 
         * This constructor initializes the `singleSelect` object by copying the `status` 
         * from the `styleBase` of the provided `other` object. It is marked as `constexpr`, 
         * allowing it to be evaluated at compile time if the input is also a constant expression.
         */
        constexpr singleSelect(const GGUI::singleSelect& other) : styleBase(other.status) {}

        /**
         * @brief Destructor for the singleSelect class.
         * 
         * This inline destructor explicitly calls the destructor of the base class
         * styleBase to ensure proper cleanup of resources associated with the base class.
         */
        inline ~singleSelect() override { styleBase::~styleBase(); }

        /**
         * @brief Creates a copy of the current singleSelect object.
         * 
         * This method overrides the copy function from the base class 
         * and returns a dynamically allocated copy of the current 
         * singleSelect instance. The caller is responsible for managing 
         * the memory of the returned object.
         * 
         * @return styleBase* A pointer to a new singleSelect object 
         *         that is a copy of the current instance.
         */
        inline styleBase* copy() const override {
            return new singleSelect(*this);
        }

        /**
         * @brief Overloaded assignment operator for the singleSelect class.
         * 
         * This operator assigns the state of another singleSelect object to the current object.
         * The assignment only occurs if the `status` of the other object is greater than or 
         * equal to the `status` of the current object. This ensures that only valid or higher 
         * priority states are copied.
         * 
         * @param other The singleSelect object to copy from.
         * @return A reference to the current singleSelect object after assignment.
         */
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

        /**
         * @brief Constructs a switchBox object with the given style and optional embedding of styles.
         * 
         * @param s A rvalue reference to a STYLING_INTERNAL::styleBase object that defines the styling for the switchBox.
         * @param Embed_Styles_On_Construct A boolean flag indicating whether to embed styles during construction. 
         *        Defaults to false.
         */
        switchBox(STYLING_INTERNAL::styleBase&& s, bool Embed_Styles_On_Construct = false) : switchBox(s, Embed_Styles_On_Construct){}

        /**
         * @brief Destructor for the switchBox class.
         *
         * This destructor ensures that the base class destructor for `element` 
         * is explicitly called to properly clean up resources associated with 
         * the base class.
         */
        ~switchBox() override{
            // call the base destructor.
            element::~element();
        }

        /**
         * @brief Toggles the state of the switch.
         * @details Flips the current state from checked to unchecked or vice versa,
         * and marks the switch as needing a state update.
         */
        void toggle();

        /**
         * @brief Sets the state of the switch box and updates its visual representation.
         * 
         * This function changes the internal state of the switch box to the specified
         * boolean value, marks the state as dirty for internal processing, and updates
         * the frame to reflect the new state.
         * 
         * @param b The new state to set for the switch box. `true` represents an active
         *          state, while `false` represents an inactive state.
         */
        void setState(bool b);

        /**
         * @brief Enables single selection mode for the switchBox.
         * 
         * When single selection mode is enabled, the switchBox ensures that 
         * only one option can be selected at a time.
         */
        void enableSingleSelect();

        /**
         * @brief Checks if the switch element is in single-select mode.
         * 
         * @return true if the switch is in single-select mode, false otherwise.
         */
        bool isSingleSelect() { return SingleSelect; }

        /**
         * @brief Checks if the switch element is currently selected.
         * 
         * @return true if the switch is selected, false otherwise.
         */
        bool isSelected() { return State; }

        /**
         * @brief Sets the text of the switch element.
         * @details This function sets the text of the switch element by first pausing the GGUI engine, then setting the text with a space character added to the beginning, and finally updating the switch element's dimensions to fit the new text. The text is then reset in the Render_Buffer nested buffer of the window.
         * @param text The new text for the switch element.
         */
        void setText(INTERNAL::compactString text);

        /**
         * @brief Toggles the visibility of the border for the switchBox element.
         * 
         * This function enables or disables the border of the switchBox element
         * based on the provided boolean value. When the border state changes, 
         * the width and height of the element are adjusted accordingly to 
         * accommodate the border. The element is marked as dirty to reflect 
         * the border changes, and a frame update is triggered to re-render 
         * the element.
         * 
         * @param b A boolean value indicating whether the border should be 
         *          enabled (true) or disabled (false).
         */
        void showBorder(bool b) override;

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

        /**
         * @brief Retrieves the string representation of the current state.
         * 
         * This function returns a compact string that represents the current
         * state of the switch. If the state is `true`, it returns the string
         * pointed to by `On`. Otherwise, it returns the string pointed to by `Off`.
         * 
         * @return INTERNAL::compactString The string representation of the current state.
         */
        constexpr INTERNAL::compactString getStateString() const {
            return State ? *On : *Off;
        }

        /**
         * @brief Sets the state strings for the switch box.
         * 
         * This function assigns the provided strings to represent the "off" and "on" states
         * of the switch box. It also marks the switch as needing a state update and refreshes
         * its frame to reflect the changes.
         * 
         * @param off Pointer to a compactString representing the "off" state.
         * @param on Pointer to a compactString representing the "on" state.
         */
        void setStateString(const INTERNAL::compactString* off, const INTERNAL::compactString* on);

    protected:
        /**
         * @brief Renders the switch element and its children into the Render_Buffer nested buffer of the window.
         * @details This function processes the switch element to generate a vector of UTF objects representing the current state.
         * It handles different stains such as CLASS, STRETCH, COLOR, EDGE, and DEEP to ensure the switch element is rendered correctly.
         * @return A vector of UTF objects representing the rendered switch element.
         */
        std::vector<GGUI::UTF>& render() override;
        
        /**
         * @brief Creates a deep copy of the Switch object.
         * @details This function creates a new Switch object and copies all the data from the current Switch object to the new one.
         *          This is useful for creating a new Switch object that is a modified version of the current one.
         * @return A pointer to the new Switch object.
         */
        element* createInstance() const override {
            return new switchBox();
        }
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
        radioButton(STYLING_INTERNAL::styleBase& s = STYLES::CONSTANTS::Default, bool Embed_Styles_On_Construct = false) : switchBox(s | visualState(SYMBOLS::RADIOBUTTON_OFF, SYMBOLS::RADIOBUTTON_ON), Embed_Styles_On_Construct) {}
        
        /**
         * @brief Constructs a radioButton object with the specified style and optional embedding of styles.
         * 
         * @param s A `STYLING_INTERNAL::styleBase` object representing the style to be applied to the radio button.
         * @param Embed_Styles_On_Construct A boolean flag indicating whether to embed styles during construction. 
         *        Defaults to `false`.
         */
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
        checkBox(STYLING_INTERNAL::styleBase& s = STYLES::CONSTANTS::Default, bool Embed_Styles_On_Construct = false) : switchBox(s | visualState({SYMBOLS::EMPTY_CHECK_BOX, SYMBOLS::CHECKED_CHECK_BOX}), Embed_Styles_On_Construct) {}
        
        /**
         * @brief Constructor for the checkBox class.
         * 
         * This constructor initializes a checkBox object with the given style and an optional flag
         * to embed styles during construction. It forwards the parameters to another checkBox
         * constructor for initialization.
         * 
         * @param s A style object of type STYLING_INTERNAL::styleBase, used to define the appearance of the checkBox.
         * @param Embed_Styles_On_Construct A boolean flag indicating whether to embed styles during construction. 
         *                                   Defaults to false.
         */
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
        /**
         * @brief Disables other switches in the same group, keeping only the specified switch active.
         * 
         * This function ensures that if the provided switchBox (`keepOn`) is part of a group
         * and is marked as single-select, it will remain active while all other single-select
         * switches in the same group are disabled. If the switch is not part of a group or is
         * not single-select, it toggles the state of the provided switchBox.
         * 
         * @param keepOn Pointer to the switchBox that should remain active or be toggled.
         * 
         * @note If the `keepOn` switchBox does not have a parent or is not part of a group,
         *       the function will simply toggle its state.
         */
        void DisableOthers(switchBox* keepOn);
    }
}

#endif