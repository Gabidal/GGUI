#include "progressBar.h"
#include "../core/core.h"

#include "../core/utils/logger.h"

#include <string>
#include <math.h>
#include <algorithm>

using namespace std;

namespace GGUI{
    namespace progress{

        void Bar::colorBar() {
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

        std::vector<terminal::cell>& Bar::render() {
            std::vector<terminal::cell>& Result = cellBuffer;

            evaluateDynamicAttributes();

            // NULLOP
            flags ^= (stain::types::DEEP);
            flags ^= stain::types::COMBINE_BORDERS;

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

        void Bar::setProgress(float New_Progress) {
            // Check if the new progress value exceeds the maximum limit
            if (New_Progress > 1.0f) {
                // Report a percentage overflow warning
                logger::log(getTypedName() + " got a percentage overflow!");
                return;
            }

            // Update the progress value
            Progress = New_Progress;

            // Mark the render buffer as dirty to reflect changes
            flags |= (stain::types::GRAPHICS);

            // Trigger a frame update to re-render the progress bar
            updateFrame();
        }

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
    }
}
