#ifndef _PROGRESS_BAR_H_
#define _PROGRESS_BAR_H_

#include "element.h"

#include "../core/utils/superString.h"
#include "../core/utils/constants.h"
#include "../core/utils/color.h"
#include "../core/utils/units.h"
#include "../core/utils/event.h"
#include "../core/utils/style.h"

namespace GGUI{

    class progressStyle{
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
        progressStyle(
            const Compact_String head = GGUI::SYMBOLS::CENTERED_HORIZONTAL_LINE,
            const Compact_String body = GGUI::SYMBOLS::CENTERED_HORIZONTAL_LINE,
            const Compact_String tail = GGUI::SYMBOLS::CENTERED_HORIZONTAL_LINE
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
        progressStyle(RGB fill_color, RGB empty_color) : progressStyle() {
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
        progressStyle(const progressStyle& other){
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

        // Copy assignment operator
        progressStyle& operator=(const progressStyle& other) {
            if (this != &other) {  // Protect against self-assignment
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
            return *this;
        }

    };

    namespace progressBarStyles{
        extern progressStyle Default;
        extern progressStyle Blocky;
        extern progressStyle Arrow;
    }

    class progressBar : public element{
    protected:
        float Progress = 0; // 0.0 - 1.0

        progressStyle Progress_Style;

        std::vector<UTF> Content;
    public:

        /**
         * @brief Constructor for Progress_Bar.
         *
         * This constructor calls the Element constructor with the given style and
         * initializes the Progress_Bar object with default values.
         *
         * @param s The style for the Progress_Bar.
         * @param Embed_Styles_On_Construct If true, the styling will be embedded into the Progress_Bar's style. Only use if you know what you're doing!!!
         */
        progressBar(styling s, bool Embed_Styles_On_Construct = false) : element(s, Embed_Styles_On_Construct){
            Progress = 0.0f;
            Progress_Style = progressBarStyles::Default;
        }

        /**
         * @brief Default constructor for Progress_Bar.
         *
         * This constructor is explicitly defined as default, which means that the compiler will generate a default implementation for it.
         * This is needed because otherwise, the compiler would not generate a default constructor for this class, since we have a user-declared constructor.
         */
        progressBar() = default;

        /**
         * @brief Returns the index of the head of the progress bar.
         * @details
         * This function returns the index of the head of the progress bar. The head is the character that is drawn at the end of the progress bar when it is not full.
         * The index is calculated by multiplying the width of the progress bar (minus the border on both sides) by the progress value.
         * The result is then rounded down to the nearest integer using the floor() function.
         * @return The index of the head of the progress bar.
         */
        unsigned int getIndexofHead();

        /**
         * @brief Colors the bar with the current progress value.
         * @details
         * This function colors the progress bar with the current progress value. It first colors the empty part of the bar, then fills in the progressed part, and finally replaces the head and tail parts.
         */
        void colorBar();

        /**
         * @brief Renders the progress bar into the Render_Buffer.
         * @details This function processes the progress bar to generate a vector of UTF objects representing the current state.
         * It handles different stains such as CLASS, STRETCH, COLOR, EDGE, and DEEP to ensure the progress bar is rendered correctly.
         * @return A vector of UTF objects representing the rendered progress bar.
         */
        std::vector<GGUI::UTF>& render() override;

        /**
         * @brief Sets the progress value of the progress bar.
         * @details This function updates the progress value of the progress bar. 
         * If the given value exceeds 1.0, a warning is reported, and the function returns without updating.
         * It also updates the color of the progress bar and marks the render buffer as dirty.
         * @param New_Progress The new progress value to set (should be between 0.0 and 1.0).
         */
        void setProgress(float New_Progress);
        
        /**
         * @brief Returns the current progress value of the progress bar.
         * @details This function returns the current progress value of the progress bar, which is a float between 0.0 and 1.0.
         * @return The current progress value of the progress bar.
         */
        float getProgress();

        /**
         * @brief Toggles the border visibility of the progress bar.
         * @details This function toggles the border visibility of the progress bar.
         *          If the state has changed, it updates the border enabled state, marks the element as dirty for border changes, and updates the frame.
         * @param b The desired state of the border visibility.
         */
        void showBorder(bool state) override;

        /**
         * @brief Destructor for the Progress_Bar class.
         *
         * This destructor is responsible for properly deallocating all the memory
         * allocated by the Progress_Bar object. It calls the base class destructor
         * to ensure all parent class resources are also cleaned up.
         */
        ~progressBar() override {
            // Call the base destructor to clean up base class resources.
            element::~element();
        }
        
        /**
         * @brief Creates a deep copy of the Progress_Bar object.
         * @details This function creates a new Progress_Bar object and copies all the data from the current Progress_Bar object to the new one.
         *          This is useful for creating a new Progress_Bar object that is a modified version of the current one.
         * @return A pointer to the new Progress_Bar object.
         */
        element* safeMove() const override {
            return new progressBar();
        }

        /**
         * @brief Returns the name of the Progress_Bar object.
         * @details This function returns a string that represents the name of the Progress_Bar object.
         *          The name is constructed by concatenating the name of the Progress_Bar with the 
         *          class name "Progress_Bar", separated by a "<" and a ">".
         * @return The name of the Progress_Bar object.
         */
        std::string getName() const override{
            return "Progress_Bar<" + Name + ">";
        }
    };

}

#endif