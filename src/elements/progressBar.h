#ifndef _PROGRESS_BAR_H_
#define _PROGRESS_BAR_H_

#include "element.h"

#include "../core/utils/superString.h"
#include "../core/utils/constants.h"
#include "../core/utils/color.h"
#include "../core/utils/utf.h"
#include "../core/utils/style.h"

namespace GGUI{

    namespace progress{
        enum class partType{
            EMPTY,
            HEAD,
            BODY,
            TAIL
        };

        class part : public STYLING_INTERNAL::styleBase{
        public:
            INTERNAL::compactString character = INTERNAL::compactString(' ');
            RGB color = COLOR::GRAY;
            partType type = partType::EMPTY;

            constexpr part(partType t, RGB fillColor = COLOR::GREEN, INTERNAL::compactString cs = INTERNAL::compactString(' '), const VALUE_STATE Default = VALUE_STATE::VALUE) : styleBase(Default) { type = t; color = fillColor; character = cs; }

            constexpr part() = default;

            inline ~part() override { styleBase::~styleBase(); }

            inline styleBase* copy() const override {
                return new part(*this);
            }
            
            constexpr part& operator=(const part& other){
                // Only copy the information if the other is enabled.
                if (other.status >= status){
                    character = other.character;
                    color = other.color;
                    type = other.type;

                    status = other.status;
                }
                return *this;
            }

            constexpr part(const part& other) : styleBase(other.status), character(other.character), color(other.color), type(other.type) {}

            INTERNAL::STAIN_TYPE embedValue([[maybe_unused]] styling* host, element* owner) override;

            inline void evaluate([[maybe_unused]] const styling* self, [[maybe_unused]] const styling* owner) override {};
        };


        class Bar : public element{
        protected:
            float Progress = 0; // 0.0 - 1.0

            INTERNAL::compactString Head = INTERNAL::compactString('>');
            INTERNAL::compactString Body = INTERNAL::compactString('-');
            INTERNAL::compactString Tail = INTERNAL::compactString('|');
            INTERNAL::compactString Empty = INTERNAL::compactString(' ');

            RGB Head_Color = GGUI::COLOR::LIGHT_GRAY;
            RGB Body_Color = GGUI::COLOR::GRAY;
            RGB Tail_Color = GGUI::COLOR::GRAY;
            RGB Empty_Color = GGUI::COLOR::DARK_GRAY;

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
            Bar(STYLING_INTERNAL::styleBase& s, bool Embed_Styles_On_Construct = false) : element(s, Embed_Styles_On_Construct){
                Progress = 0.0f;
            }
            
            /**
             * @brief Constructs a Bar object with the given style and optional embedding of styles.
             * 
             * @param s A rvalue reference to a STYLING_INTERNAL::styleBase object that defines the style for the Bar.
             * @param Embed_Styles_On_Construct A boolean flag indicating whether to embed styles during construction. 
             *        Defaults to false.
             */
            Bar(STYLING_INTERNAL::styleBase&& s, bool Embed_Styles_On_Construct = false) : Bar(s, Embed_Styles_On_Construct){}

            /**
             * @brief Default constructor for Progress_Bar.
             *
             * This constructor is explicitly defined as default, which means that the compiler will generate a default implementation for it.
             * This is needed because otherwise, the compiler would not generate a default constructor for this class, since we have a user-declared constructor.
             */
            Bar() = default;

            void setHeadCharacter(INTERNAL::compactString cs) { Head = cs; }
            void setBodyCharacter(INTERNAL::compactString cs) { Body = cs; }
            void setTailCharacter(INTERNAL::compactString cs) { Tail = cs; }
            void setEmptyCharacter(INTERNAL::compactString cs) { Empty = cs; }

            void setHeadColor(RGB color) { Head_Color = color; }
            void setBodyColor(RGB color) { Body_Color = color; }
            void setTailColor(RGB color) { Tail_Color = color; }
            void setEmptyColor(RGB color) { Empty_Color = color; }

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
             * @brief Updates the progress of the progress bar by adding the specified value.
             * 
             * This function increments the current progress by the given value, ensuring
             * that the progress does not exceed 1.0f. If the progress exceeds 1.0f after
             * adding the value, the function returns without making any changes.
             * 
             * After updating the progress, the function updates the color of the progress
             * bar, marks the render buffer as dirty to reflect the changes, and triggers
             * a frame update to re-render the progress bar.
             * 
             * @param add The value to add to the current progress. Should be a float
             *            between 0.0f and 1.0f.
             */
            void updateProgress(float add);

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
            ~Bar() override {
                // Call the base destructor to clean up base class resources.
                element::~element();
            }

            /**
             * @brief Returns the name of the Progress_Bar object.
             * @details This function returns a string that represents the name of the Progress_Bar object.
             *          The name is constructed by concatenating the name of the Progress_Bar with the 
             *          class name "Progress_Bar", separated by a "<" and a ">".
             * @return The name of the Progress_Bar object.
             */
            std::string getName() const override{
                return "progressBar<" + Name + ">";
            }

        protected:
            /**
             * @brief Renders the progress bar into the Render_Buffer.
             * @details This function processes the progress bar to generate a vector of UTF objects representing the current state.
             * It handles different stains such as CLASS, STRETCH, COLOR, EDGE, and DEEP to ensure the progress bar is rendered correctly.
             * @return A vector of UTF objects representing the rendered progress bar.
             */
            std::vector<GGUI::UTF>& render() override;
            
            /**
             * @brief Creates a deep copy of the Progress_Bar object.
             * @details This function creates a new Progress_Bar object and copies all the data from the current Progress_Bar object to the new one.
             *          This is useful for creating a new Progress_Bar object that is a modified version of the current one.
             * @return A pointer to the new Progress_Bar object.
             */
            element* createInstance() const override {
                return new Bar();
            }
        };
    }
}

#endif