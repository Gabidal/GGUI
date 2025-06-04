#include "progressBar.h"
#include "../core/renderer.h"

#include <string>
#include <math.h>
#include <algorithm>

using namespace std;

namespace GGUI{

    namespace progressBarStyles{
        inline progressStyle Default = progressStyle();
        inline progressStyle Blocky = progressStyle(GGUI::SYMBOLS::FULL_BLOCK, GGUI::SYMBOLS::FULL_BLOCK, GGUI::SYMBOLS::FULL_BLOCK);
        inline progressStyle Arrow = progressStyle(">", "=", "=");
    }

    /**
     * @brief Returns the index of the head of the progress bar.
     * @details
     * This function returns the index of the head of the progress bar. The head is the character that is drawn at the end of the progress bar when it is not full.
     * The index is calculated by multiplying the width of the progress bar (minus the border on both sides) by the progress value.
     * The result is then rounded down to the nearest integer using the floor() function.
     * @return The index of the head of the progress bar.
     */
    unsigned int progressBar::getIndexofHead(){
        return floor(Progress * (getWidth() - hasBorder() * 2));
    }

    /**
     * @brief Colors the bar with the current progress value.
     * @details
     * This function colors the progress bar with the current progress value. It first colors the empty part of the bar, then fills in the progressed part, and finally replaces the head and tail parts.
     */
    void progressBar::colorBar(){
        // First color the empty part of the bar
        for (unsigned int i = 0; i < getWidth() - hasBorder() * 2; i++)
            Content[i] = UTF(Progress_Style.Body, { Progress_Style.Empty_Color , getBackgroundColor() });

        // Now fill in the progressed part
        for (unsigned int i = 0; i < getIndexofHead(); i++)
            Content[i].Foreground = Progress_Style.Body_Color;

        // now replace the head part
        Content[getIndexofHead()] = UTF(Progress_Style.Head, { Progress_Style.Head_Color, getBackgroundColor() });

        // now replace the tail part
        Content[0] = UTF(Progress_Style.Tail, { Progress_Style.Tail_Color, getBackgroundColor() });

    }

    /**
     * @brief Renders the progress bar into the Render_Buffer.
     * @details This function processes the progress bar to generate a vector of UTF objects representing the current state.
     * It handles different stains such as CLASS, STRETCH, COLOR, EDGE, and DEEP to ensure the progress bar is rendered correctly.
     * @return A vector of UTF objects representing the rendered progress bar.
     */
    std::vector<UTF>& progressBar::render() {
        std::vector<UTF>& Result = Render_Buffer;

        // Check for Dynamic attributes
        if(Style->Evaluate_Dynamic_Dimensions(this))
            Dirty.Dirty(STAIN_TYPE::STRETCH);

        if (Style->Evaluate_Dynamic_Position(this))
            Dirty.Dirty(STAIN_TYPE::MOVE);

        if (Style->Evaluate_Dynamic_Colors(this))
            Dirty.Dirty(STAIN_TYPE::COLOR);

        if (Style->Evaluate_Dynamic_Border(this))
            Dirty.Dirty(STAIN_TYPE::EDGE);

        // If the progress bar is clean, return the current render buffer.
        if (Dirty.is(STAIN_TYPE::CLEAN))
            return Result;

        // Handle the STRETCH stain by evaluating dynamic attributes and resizing the result buffer.
        if (Dirty.is(STAIN_TYPE::STRETCH)) {
            Result.clear();
            Result.resize(getWidth() * getHeight(), SYMBOLS::EMPTY_UTF);
            Content.resize(getWidth() - hasBorder() * 2, UTF(Progress_Style.Body, { Progress_Style.Empty_Color, getBackgroundColor() }));
            colorBar();
            Dirty.Clean(STAIN_TYPE::STRETCH);
            Dirty.Dirty(STAIN_TYPE::COLOR | STAIN_TYPE::EDGE | STAIN_TYPE::DEEP);
        }

        if (Dirty.is(STAIN_TYPE::MOVE)) {
            Dirty.Clean(STAIN_TYPE::MOVE);

            updateAbsolutePositionCache();
        }

        // Apply the color system to the resized result list
        if (Dirty.is(STAIN_TYPE::COLOR)){        
            // Clean the color stain after applying the color system.
            Dirty.Clean(STAIN_TYPE::COLOR);

            applyColors(this, Result);
        }

        // Add child windows to the Result buffer if the DEEP stain is detected.
        if (Dirty.is(STAIN_TYPE::DEEP)) {
            Dirty.Clean(STAIN_TYPE::DEEP);
            int Starting_Y = hasBorder();
            int Starting_X = hasBorder();
            int Ending_Y = getHeight() - hasBorder();
            int Ending_X = getWidth() - hasBorder();

            for (int y = Starting_Y; y < Ending_Y; y++)
                for (int x = Starting_X; x < Ending_X; x++)
                    Result[y * getWidth() + x] = Content[x - Starting_X];
        }

        // Add borders and titles if the EDGE stain is detected.
        if (Dirty.is(STAIN_TYPE::EDGE))
            addOverhead(this, Result);

        return Result;
    }

    /**
     * @brief Sets the progress value of the progress bar.
     * @details This function updates the progress value of the progress bar. 
     * If the given value exceeds 1.0, a warning is reported, and the function returns without updating.
     * It also updates the color of the progress bar and marks the render buffer as dirty.
     * @param New_Progress The new progress value to set (should be between 0.0 and 1.0).
     */
    void progressBar::setProgress(float New_Progress) {
        // Check if the new progress value exceeds the maximum limit
        if (New_Progress > 1.00) {
            // Report a percentage overflow warning
            report(getName() + " got a percentage overflow!");
            return;
        }

        // Update the progress value
        Progress = New_Progress;

        // Update the color of the progress bar based on the new progress value
        colorBar();

        // Mark the render buffer as dirty to reflect changes
        Dirty.Dirty(STAIN_TYPE::DEEP);

        // Trigger a frame update to re-render the progress bar
        updateFrame();
    }

    /**
     * @brief Returns the current progress value of the progress bar.
     * @details This function returns the current progress value of the progress bar, which is a float between 0.0 and 1.0.
     * @return The current progress value of the progress bar.
     */
    float progressBar::getProgress() {
        return Progress;
    }

    /**
     * @brief Toggles the border visibility of the progress bar.
     * @details This function toggles the border visibility of the progress bar.
     *          If the state has changed, it updates the border enabled state, marks the element as dirty for border changes, and updates the frame.
     * @param b The desired state of the border visibility.
     */
    void progressBar::showBorder(bool b) {
        if (b != Style->Border_Enabled.Value) {
            Style->Border_Enabled = b;

            // Adjust the width and height of the progress bar based on the border state
            if (b) Style->Width.Direct() += 2;
            else Style->Height.Direct() -= 2;

            // Mark the element as dirty for border changes
            Dirty.Dirty(STAIN_TYPE::EDGE);

            // Trigger a frame update to re-render the progress bar
            updateFrame();
        }
    }
}
