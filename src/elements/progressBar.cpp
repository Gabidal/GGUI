#include "progressBar.h"
#include "../core/core.h"

#include <string>
#include <math.h>
#include <algorithm>

using namespace std;

namespace GGUI{

    namespace progress{
        stain::base part::embedValue([[maybe_unused]] styling* host, element* owner){
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
        
            return stain::types::GRAPHICS;
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

        void Bar::colorBar(){
            IVector2 start = {hasBorder(), hasBorder()};
            IVector2 end = {getWidth() - hasBorder(), getHeight() - hasBorder()};

            size_t borderedOffset = hasBorder() * getHeight() + hasBorder();    // the final + hasBorder is for the +1 x offset for the left side wall
            auto beginAfterOffset = cellBuffer.begin() + borderedOffset;
            auto endBeforeOffset = cellBuffer.end() - borderedOffset;

            // First color the progressed part of the bar
            std::fill(beginAfterOffset, beginAfterOffset + getIndexofHead(), Body);

            // Add identity for progressed part
            graphicalIdentityPool.push_back(activeStyle{
                {   // rectangle 
                    start, 
                    {getIndexofHead(), start.y()}
                },
                Body_Color,
                getBackgroundColor()
            });

            // Now fill in the empty part
            std::fill(beginAfterOffset + getIndexofHead(), endBeforeOffset, Empty);

            graphicalIdentityPool.push_back(activeStyle{
                {   // rectangle 
                    {start.x() + getIndexofHead(), start.y()}, 
                    {end.x() - start.x() - getIndexofHead(), start.y()}
                },
                Empty_Color,
                getBackgroundColor()
            });

            // now replace the head part
            cellBuffer[borderedOffset + getIndexofHead()] = Head;

            graphicalIdentityPool.push_back(activeStyle{
                {   // rectangle 
                    {start.x() + getIndexofHead(), start.y()}, 
                    {1, start.y()}
                },
                Head_Color,
                getBackgroundColor()
            });

            // now replace the tail part
            cellBuffer[borderedOffset] = Tail;

            graphicalIdentityPool.push_back(activeStyle{
                {   // rectangle 
                    {start.x(), start.y()}, 
                    {1, start.y()}
                },
                Tail_Color,
                getBackgroundColor()
            });
        }

        /**
         * @brief Renders the progress bar into the Render_Buffer.
         * @details This function processes the progress bar to generate a vector of UTF objects representing the current state.
         * It handles different stains such as CLASS, STRETCH, COLOR, EDGE, and DEEP to ensure the progress bar is rendered correctly.
         * @return A vector of UTF objects representing the rendered progress bar.
         */
        std::vector<terminal::cell>& Bar::render() {
            std::vector<terminal::cell>& Result = cellBuffer;

            // Check for Dynamic attributes
            if(style->evaluateDynamicDimensions(this))
                flags |= (stain::types::STRETCH);

            if (style->evaluateDynamicPosition(this))
                flags |= (stain::types::MOVE);

            if (style->evaluateDynamicGraphics(this))
                flags |= (stain::types::GRAPHICS);

            if (style->evaluateDynamicBorder(this))
                flags |= (stain::types::EDGE);

            // If the progress bar is clean, return the current render buffer.
            if (flags.isEmpty())
                return Result;

            if (flags.has(stain::types::RESET)){
                flags ^= (stain::types::RESET);

                std::fill(cellBuffer.begin(), cellBuffer.end(), ' ');
                
                flags |= (stain::base(stain::types::GRAPHICS) | stain::types::EDGE);
            }

            // Handle the STRETCH stain by evaluating dynamic attributes and resizing the result buffer.
            if (flags.has(stain::types::STRETCH)) {
                Result.clear();
                Result.resize(getWidth() * getHeight(), ' ');

                flags ^= (stain::types::STRETCH);
                flags |= (stain::base(stain::types::GRAPHICS) | stain::types::EDGE);
            }

            if (flags.has(stain::types::NOT_RENDERED)) {
                if (onRender) onRender(this);

                // Clean regardless of On_Render existing or not.
                flags ^= (stain::types::NOT_RENDERED);
            }

            if (flags.has(stain::types::MOVE)) {
                flags ^= (stain::types::MOVE);

                updateAbsolutePositionCache();
            }

            // Nothing to do here
            flags ^= (stain::types::DEEP);

            // Apply the color system to the resized result list
            if (flags.has(stain::types::GRAPHICS)){        
                // Clean the color stain after applying the color system.
                flags ^= (stain::types::GRAPHICS);

                graphicalIdentityPool.clear();
                graphicalReflectionPool.clear();

                colorBar();

                compileActiveGraphics();
            }

            // Add borders and titles if the EDGE stain is detected.
            if (flags.has(stain::types::EDGE)){
                flags ^= (stain::types::EDGE);

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
                logger::log(getName() + " got a percentage overflow!");
                return;
            }

            // Update the progress value
            Progress = New_Progress;

            // Mark the render buffer as dirty to reflect changes
            flags |= (stain::types::GRAPHICS);

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

            // Mark the render buffer as dirty to reflect changes
            flags |= (stain::types::GRAPHICS);

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
            if (b != style->Border_Enabled.value) {
                style->Border_Enabled = b;

                // Adjust the width and height of the progress bar based on the border state
                if (b) style->Width.set(style->Width.get() + 2);
                else style->Height.set(style->Height.get() - 2);

                // Mark the element as dirty for border changes
                flags |= (stain::types::EDGE);

                // Trigger a frame update to re-render the progress bar
                updateFrame();
            }
        }
    }
}
