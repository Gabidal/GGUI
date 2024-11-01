#ifndef _PROGRESS_BAR_H_
#define _PROGRESS_BAR_H_

#include "Element.h"

#include "../Core/Utils/Super_String.h"
#include "../Core/Utils/Constants.h"
#include "../Core/Utils/Color.h"
#include "../Core/Utils/Units.h"
#include "../Core/Utils/Event.h"
#include "../Core/Utils/Style.h"

namespace GGUI{

    class PROGRESS_STYLE{
    public:
        Compact_String Head;
        Compact_String Body;
        Compact_String Tail;

        RGB Head_Color = GGUI::COLOR::LIGHT_GRAY;
        RGB Body_Color = GGUI::COLOR::GRAY;
        RGB Tail_Color = GGUI::COLOR::GRAY;

        RGB Empty_Color = GGUI::COLOR::DARK_GRAY;

        /**
         * @brief Constructor for the PROGRESS_STYLE class.
         * 
         * @param head The character to use for the head of the progress bar.
         * @param body The character to use for the body of the progress bar.
         * @param tail The character to use for the tail of the progress bar.
         * 
         * The default is to use the centered horizontal line character for all three parts of the progress bar.
         */
        PROGRESS_STYLE(
            const char* head = GGUI::SYMBOLS::CENTERED_HORIZONTAL_LINE.data(),
            const char* body = GGUI::SYMBOLS::CENTERED_HORIZONTAL_LINE.data(),
            const char* tail = GGUI::SYMBOLS::CENTERED_HORIZONTAL_LINE.data()
        ) : Head(head), Body(body), Tail(tail){}

        /**
         * @brief Constructor for the PROGRESS_STYLE class.
         * 
         * @param fill_color The color to use for the head, body, and tail of the progress bar.
         * @param empty_color The color to use for the empty part of the progress bar.
         * 
         * This constructor is used to create a PROGRESS_STYLE with the same color for the head, body, and tail.
         * The default is to use the centered horizontal line character for all three parts of the progress bar.
         */
        PROGRESS_STYLE(RGB fill_color, RGB empty_color) : PROGRESS_STYLE() {
            Head_Color = fill_color;
            Body_Color = fill_color;
            Tail_Color = fill_color;

            Empty_Color = empty_color;
        }

        /**
         * @brief Copy constructor for the PROGRESS_STYLE class.
         * 
         * @param other The other PROGRESS_STYLE object to copy from.
         * 
         * This constructor copies the data from the other PROGRESS_STYLE object to this one.
         */
        PROGRESS_STYLE(const PROGRESS_STYLE& other){
            // Copy data
            Head = other.Head;
            Body = other.Body;
            Tail = other.Tail;
            
            // Copy colors
            Head_Color = other.Head_Color;
            Body_Color = other.Body_Color;
            Tail_Color = other.Tail_Color;

            Empty_Color = other.Empty_Color;
        }

        /**
         * @brief Assignment operator for the PROGRESS_STYLE class.
         * 
         * This assignment operator copies the data from the other PROGRESS_STYLE object to this one.
         * It first checks for self-assignment, then copies the data and colors to the current object.
         * 
         * @param other The other PROGRESS_STYLE object to copy from.
         * @return A reference to the current object.
         */
        PROGRESS_STYLE &operator=(const GGUI::PROGRESS_STYLE & other){
            // Check for self-assignment
            if (this == &other)
                return *this;

            // Copy data
            Head = other.Head;
            Body = other.Body;
            Tail = other.Tail;
            
            // Copy colors
            Head_Color = other.Head_Color;
            Body_Color = other.Body_Color;
            Tail_Color = other.Tail_Color;

            Empty_Color = other.Empty_Color;

            return *this;
        }

    };

    namespace Progress_Bar_Styles{
        extern PROGRESS_STYLE Default;
        extern PROGRESS_STYLE Blocky;
        extern PROGRESS_STYLE Arrow;
    }

    class Progress_Bar : public Element{
    protected:
        float Progress = 0; // 0.0 - 1.0

        PROGRESS_STYLE Progress_Style;

        std::vector<UTF> Content;
    public:

        /**
         * @brief Constructor for Progress_Bar.
         *
         * This constructor calls the Element constructor with the given style and
         * initializes the Progress_Bar object with default values.
         *
         * @param s The style for the Progress_Bar.
         */
        Progress_Bar(Styling s) : Element(s){
            Progress = 0.0f;
            Progress_Style = Progress_Bar_Styles::Default;
        }

        /**
         * @brief Default constructor for Progress_Bar.
         *
         * This constructor is explicitly defined as default, which means that the compiler will generate a default implementation for it.
         * This is needed because otherwise, the compiler would not generate a default constructor for this class, since we have a user-declared constructor.
         */
        Progress_Bar() = default;

        /**
         * @brief Copy assignment operator for the Progress_Bar class.
         * 
         * This assignment operator copies the data from the other Progress_Bar object to this one.
         * It first calls the base class's assignment operator to copy the base class members, then
         * copies the Progress_Bar class members.
         * 
         * @param other The other Progress_Bar object to copy from.
         * @return A reference to the current object.
         */
        Progress_Bar& operator=(const Progress_Bar& other){
            Element::operator=(other);

            // Copy Progress_Bar members
            Progress = other.Progress;
            Progress_Style = other.Progress_Style;
            Content = other.Content;

            return *this;
        }

        /**
         * @brief Returns the index of the head of the progress bar.
         * @details
         * This function returns the index of the head of the progress bar. The head is the character that is drawn at the end of the progress bar when it is not full.
         * The index is calculated by multiplying the width of the progress bar (minus the border on both sides) by the progress value.
         * The result is then rounded down to the nearest integer using the floor() function.
         * @return The index of the head of the progress bar.
         */
        unsigned int Get_Index_of_Head();

        /**
         * @brief Colors the bar with the current progress value.
         * @details
         * This function colors the progress bar with the current progress value. It first colors the empty part of the bar, then fills in the progressed part, and finally replaces the head and tail parts.
         */
        void Color_Bar();

        /**
         * @brief Renders the progress bar into the Render_Buffer.
         * @details This function processes the progress bar to generate a vector of UTF objects representing the current state.
         * It handles different stains such as CLASS, STRETCH, COLOR, EDGE, and DEEP to ensure the progress bar is rendered correctly.
         * @return A vector of UTF objects representing the rendered progress bar.
         */
        std::vector<GGUI::UTF>& Render() override;

        /**
         * @brief Sets the progress value of the progress bar.
         * @details This function updates the progress value of the progress bar. 
         * If the given value exceeds 1.0, a warning is reported, and the function returns without updating.
         * It also updates the color of the progress bar and marks the render buffer as dirty.
         * @param New_Progress The new progress value to set (should be between 0.0 and 1.0).
         */
        void Set_Progress(float New_Progress);
        
        /**
         * @brief Returns the current progress value of the progress bar.
         * @details This function returns the current progress value of the progress bar, which is a float between 0.0 and 1.0.
         * @return The current progress value of the progress bar.
         */
        float Get_Progress();

        /**
         * @brief Toggles the border visibility of the progress bar.
         * @details This function toggles the border visibility of the progress bar.
         *          If the state has changed, it updates the border enabled state, marks the element as dirty for border changes, and updates the frame.
         * @param b The desired state of the border visibility.
         */
        void Show_Border(bool state) override;

        /**
         * @brief Destructor for the Progress_Bar class.
         *
         * This destructor is responsible for properly deallocating all the memory
         * allocated by the Progress_Bar object. It calls the base class destructor
         * to ensure all parent class resources are also cleaned up.
         */
        ~Progress_Bar() override {
            // Call the base destructor to clean up base class resources.
            Element::~Element();
        }
        
        /**
         * @brief Creates a deep copy of the Progress_Bar object.
         * @details This function creates a new Progress_Bar object and copies all the data from the current Progress_Bar object to the new one.
         *          This is useful for creating a new Progress_Bar object that is a modified version of the current one.
         * @return A pointer to the new Progress_Bar object.
         */
        Element* Safe_Move() override {
            Progress_Bar* new_Progress_Bar = new Progress_Bar();
            *new_Progress_Bar = *(Progress_Bar*)this;

            return new_Progress_Bar;
        }

        /**
         * @brief Returns the name of the Progress_Bar object.
         * @details This function returns a string that represents the name of the Progress_Bar object.
         *          The name is constructed by concatenating the name of the Progress_Bar with the 
         *          class name "Progress_Bar", separated by a "<" and a ">".
         * @return The name of the Progress_Bar object.
         */
        std::string Get_Name() const override{
            return "Progress_Bar<" + Name + ">";
        }
    };

}

#endif