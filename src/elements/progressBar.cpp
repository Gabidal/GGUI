#include "progressBar.h"
#include "../core/renderer.h"

#include <string>
#include <math.h>
#include <algorithm>

using namespace std;

namespace GGUI{

    namespace progress{
        INTERNAL::STAIN_TYPE part::embedValue([[maybe_unused]] styling* host, element* owner){
            if (dynamic_cast<Bar*>(owner)){
                Bar* castedOwner = ((Bar*)owner);

                if (type == partType::HEAD){
                    castedOwner->setHeadCharacter(character);
                    castedOwner->setHeadColor(color);
                }
                else if (type == partType::BODY){
                    castedOwner->setBodyCharacter(character);
                    castedOwner->setBodyColor(color);
                }
                else if (type == partType::TAIL){
                    castedOwner->setTailCharacter(character);
                    castedOwner->setTailColor(color);
                }
                else if (type == partType::EMPTY){
                    castedOwner->setEmptyCharacter(character);
                    castedOwner->setEmptyColor(color);
                }
                else{
                    throw std::runtime_error("Unknown part type in progress bar part!");
                }
            }
            else
                throw std::runtime_error("Progress Bar Head Style can only be used with a Bar element!");
        
            return INTERNAL::STAIN_TYPE::COLOR;
        }

        /**
         * @brief Returns the index of the head of the progress bar.
         * @details
         * This function returns the index of the head of the progress bar. The head is the character that is drawn at the end of the progress bar when it is not full.
         * The index is calculated by multiplying the width of the progress bar (minus the border on both sides) by the progress value.
         * The result is then rounded down to the nearest integer using the floor() function.
         * @return The index of the head of the progress bar.
         */
        unsigned int Bar::getIndexofHead(){
            return floor(Progress * (getWidth() - hasBorder() * 2));
        }

        /**
         * @brief Colors the bar with the current progress value.
         * @details
         * This function colors the progress bar with the current progress value. It first colors the empty part of the bar, then fills in the progressed part, and finally replaces the head and tail parts.
         */
        void Bar::colorBar(){
            if (Content.empty() || Content.size() != getWidth() - hasBorder() * 2){
                // Resize the content to fit the width of the progress bar minus the borders
                Content.clear();
                Content.resize(getWidth() - hasBorder() * 2, UTF(Body, { Empty_Color, getBackgroundColor() }));
            }

            // First color the progressed part of the bar
            std::fill(Content.begin(), Content.begin() + getIndexofHead(), UTF(Body, { Body_Color , getBackgroundColor() }));

            // Now fill in the empty part
            std::fill(Content.begin() + getIndexofHead(), Content.end(), UTF(Empty, { Empty_Color, getBackgroundColor() }));

            // now replace the head part
            Content[getIndexofHead()] = UTF(Head, { Head_Color, getBackgroundColor() });

            // now replace the tail part
            Content.front() = UTF(Tail, { Tail_Color, getBackgroundColor() });
        }

        /**
         * @brief Renders the progress bar into the Render_Buffer.
         * @details This function processes the progress bar to generate a vector of UTF objects representing the current state.
         * It handles different stains such as CLASS, STRETCH, COLOR, EDGE, and DEEP to ensure the progress bar is rendered correctly.
         * @return A vector of UTF objects representing the rendered progress bar.
         */
        std::vector<UTF>& Bar::render() {
            std::vector<UTF>& Result = renderBuffer;

            // Check for Dynamic attributes
            if(Style->evaluateDynamicDimensions(this))
                Dirty.Dirty(INTERNAL::STAIN_TYPE::STRETCH);

            if (Style->evaluateDynamicPosition(this))
                Dirty.Dirty(INTERNAL::STAIN_TYPE::MOVE);

            if (Style->evaluateDynamicColors(this))
                Dirty.Dirty(INTERNAL::STAIN_TYPE::COLOR);

            if (Style->evaluateDynamicBorder(this))
                Dirty.Dirty(INTERNAL::STAIN_TYPE::EDGE);

            // If the progress bar is clean, return the current render buffer.
            if (Dirty.is(INTERNAL::STAIN_TYPE::CLEAN))
                return Result;

            if (Dirty.is(INTERNAL::STAIN_TYPE::RESET)){
                Dirty.Clean(INTERNAL::STAIN_TYPE::RESET);

                std::fill(renderBuffer.begin(), renderBuffer.end(), SYMBOLS::EMPTY_UTF);
                
                Dirty.Dirty(INTERNAL::STAIN_TYPE::COLOR | INTERNAL::STAIN_TYPE::EDGE | INTERNAL::STAIN_TYPE::DEEP);
            }

            // Handle the STRETCH stain by evaluating dynamic attributes and resizing the result buffer.
            if (Dirty.is(INTERNAL::STAIN_TYPE::STRETCH)) {
                Result.clear();
                Result.resize(getWidth() * getHeight(), SYMBOLS::EMPTY_UTF);
                colorBar();
                Dirty.Clean(INTERNAL::STAIN_TYPE::STRETCH);
                Dirty.Dirty(INTERNAL::STAIN_TYPE::COLOR | INTERNAL::STAIN_TYPE::EDGE | INTERNAL::STAIN_TYPE::DEEP);
            }

            if (Dirty.is(INTERNAL::STAIN_TYPE::MOVE)) {
                Dirty.Clean(INTERNAL::STAIN_TYPE::MOVE);

                updateAbsolutePositionCache();
            }

            // Apply the color system to the resized result list
            if (Dirty.is(INTERNAL::STAIN_TYPE::COLOR)){        
                // Clean the color stain after applying the color system.
                Dirty.Clean(INTERNAL::STAIN_TYPE::COLOR);

                applyColors(Result);
            }

            // Add child windows to the Result buffer if the DEEP stain is detected.
            if (Dirty.is(INTERNAL::STAIN_TYPE::DEEP)) {
                Dirty.Clean(INTERNAL::STAIN_TYPE::DEEP);
                int Starting_Y = hasBorder();
                int Starting_X = hasBorder();
                int Ending_Y = getHeight() - hasBorder();
                int Ending_X = getWidth() - hasBorder();

                for (int y = Starting_Y; y < Ending_Y; y++)
                    for (int x = Starting_X; x < Ending_X; x++)
                        Result[y * getWidth() + x] = Content[x - Starting_X];
            }

            // Add borders and titles if the EDGE stain is detected.
            if (Dirty.is(INTERNAL::STAIN_TYPE::EDGE)){
                renderBorders(Result);
                renderTitle(Result);
            }

            return Result;
        }

        /**
         * @brief Sets the progress value of the progress bar.
         * @details This function updates the progress value of the progress bar. 
         * If the given value exceeds 1.0, a warning is reported, and the function returns without updating.
         * It also updates the color of the progress bar and marks the render buffer as dirty.
         * @param New_Progress The new progress value to set (should be between 0.0 and 1.0).
         */
        void Bar::setProgress(float New_Progress) {
            // Check if the new progress value exceeds the maximum limit
            if (New_Progress > 1.0f) {
                // Report a percentage overflow warning
                report(getName() + " got a percentage overflow!");
                return;
            }

            // Update the progress value
            Progress = New_Progress;

            // Update the color of the progress bar based on the new progress value
            colorBar();

            // Mark the render buffer as dirty to reflect changes
            Dirty.Dirty(INTERNAL::STAIN_TYPE::DEEP);

            // Trigger a frame update to re-render the progress bar
            updateFrame();
        }

        /**
         * @brief Returns the current progress value of the progress bar.
         * @details This function returns the current progress value of the progress bar, which is a float between 0.0 and 1.0.
         * @return The current progress value of the progress bar.
         */
        float Bar::getProgress() {
            return Progress;
        }

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
        void Bar::updateProgress(float add){
            if (Progress + add > 1.0f){
                return;
            }

            Progress += add;

            colorBar();

            // Mark the render buffer as dirty to reflect changes
            Dirty.Dirty(INTERNAL::STAIN_TYPE::DEEP | INTERNAL::STAIN_TYPE::COLOR);

            // Trigger a frame update to re-render the progress bar
            updateFrame();
        }

        /**
         * @brief Toggles the border visibility of the progress bar.
         * @details This function toggles the border visibility of the progress bar.
         *          If the state has changed, it updates the border enabled state, marks the element as dirty for border changes, and updates the frame.
         * @param b The desired state of the border visibility.
         */
        void Bar::showBorder(bool b) {
            if (b != Style->Border_Enabled.value) {
                Style->Border_Enabled = b;

                // Adjust the width and height of the progress bar based on the border state
                if (b) Style->Width.direct() += 2;
                else Style->Height.direct() -= 2;

                // Mark the element as dirty for border changes
                Dirty.Dirty(INTERNAL::STAIN_TYPE::EDGE);

                // Trigger a frame update to re-render the progress bar
                updateFrame();
            }
        }
    }
}
