#include "Progress_Bar.h"
#include <string>
#include <math.h>
#include <algorithm>
#include "../Core/Renderer.h"

using namespace std;

namespace GGUI{

    namespace Progress_Bar_Styles{
        inline PROGRESS_STYLE Default = PROGRESS_STYLE();
        inline PROGRESS_STYLE Blocky = PROGRESS_STYLE(GGUI::SYMBOLS::FULL_BLOCK.data(), GGUI::SYMBOLS::FULL_BLOCK.data(), GGUI::SYMBOLS::FULL_BLOCK.data());
        inline PROGRESS_STYLE Arrow = PROGRESS_STYLE(">", "=", "=");
    }

    /**
     * @brief Returns the index of the head of the progress bar.
     * @details
     * This function returns the index of the head of the progress bar. The head is the character that is drawn at the end of the progress bar when it is not full.
     * The index is calculated by multiplying the width of the progress bar (minus the border on both sides) by the progress value.
     * The result is then rounded down to the nearest integer using the floor() function.
     * @return The index of the head of the progress bar.
     */
    unsigned int Progress_Bar::Get_Index_of_Head(){
        return floor(Progress * (Get_Width() - Has_Border() * 2));
    }

    /**
     * @brief Colors the bar with the current progress value.
     * @details
     * This function colors the progress bar with the current progress value. It first colors the empty part of the bar, then fills in the progressed part, and finally replaces the head and tail parts.
     */
    void Progress_Bar::Color_Bar(){
        // First color the empty part of the bar
        for (unsigned int i = 0; i < Get_Width() - Has_Border() * 2; i++)
            Content[i] = UTF(Progress_Style.Body, { Progress_Style.Empty_Color , Get_Background_Color() });

        // Now fill in the progressed part
        for (unsigned int i = 0; i < Get_Index_of_Head(); i++)
            Content[i].Foreground = Progress_Style.Body_Color;

        // now replace the head part
        Content[Get_Index_of_Head()] = UTF(Progress_Style.Head, { Progress_Style.Head_Color, Get_Background_Color() });

        // now replace the tail part
        Content[0] = UTF(Progress_Style.Tail, { Progress_Style.Tail_Color, Get_Background_Color() });

    }

    /**
     * @brief Renders the progress bar into the Render_Buffer.
     * @details This function processes the progress bar to generate a vector of UTF objects representing the current state.
     * It handles different stains such as CLASS, STRETCH, COLOR, EDGE, and DEEP to ensure the progress bar is rendered correctly.
     * @return A vector of UTF objects representing the rendered progress bar.
     */
    std::vector<UTF>& Progress_Bar::Render() {
        std::vector<UTF>& Result = Render_Buffer;

        // If the progress bar is clean, return the current render buffer.
        if (Dirty.is(STAIN_TYPE::CLEAN))
            return Result;

        // Parse classes if the CLASS stain is detected.
        if (Dirty.is(STAIN_TYPE::CLASS)) {
            Parse_Classes();
            Dirty.Clean(STAIN_TYPE::CLASS);
        }

        // Handle the STRETCH stain by evaluating dynamic attributes and resizing the result buffer.
        if (Dirty.is(STAIN_TYPE::STRETCH)) {
            Style->Evaluate_Dynamic_Attribute_Values(this);
            Result.clear();
            Result.resize(Get_Width() * Get_Height(), SYMBOLS::EMPTY_UTF);
            Content.resize(Get_Width() - Has_Border() * 2, UTF(Progress_Style.Body, { Progress_Style.Empty_Color, Get_Background_Color() }));
            Color_Bar();
            Dirty.Clean(STAIN_TYPE::STRETCH);
            Dirty.Dirty(STAIN_TYPE::COLOR | STAIN_TYPE::EDGE | STAIN_TYPE::DEEP);
        }

        // Update the absolute position cache if the MOVE stain is detected.
        if (Dirty.is(STAIN_TYPE::MOVE)) {
            Dirty.Clean(STAIN_TYPE::MOVE);
            Update_Absolute_Position_Cache();
        }

        // Apply the color system if the COLOR stain is detected.
        if (Dirty.is(STAIN_TYPE::COLOR))
            Apply_Colors(this, Result);

        // Add child windows to the Result buffer if the DEEP stain is detected.
        if (Dirty.is(STAIN_TYPE::DEEP)) {
            Dirty.Clean(STAIN_TYPE::DEEP);
            int Starting_Y = Has_Border();
            int Starting_X = Has_Border();
            int Ending_Y = Get_Height() - Has_Border();
            int Ending_X = Get_Width() - Has_Border();

            for (int y = Starting_Y; y < Ending_Y; y++)
                for (int x = Starting_X; x < Ending_X; x++)
                    Result[y * Get_Width() + x] = Content[x - Starting_X];
        }

        // Add borders and titles if the EDGE stain is detected.
        if (Dirty.is(STAIN_TYPE::EDGE))
            Add_Overhead(this, Result);

        return Result;
    }

    /**
     * @brief Sets the progress value of the progress bar.
     * @details This function updates the progress value of the progress bar. 
     * If the given value exceeds 1.0, a warning is reported, and the function returns without updating.
     * It also updates the color of the progress bar and marks the render buffer as dirty.
     * @param New_Progress The new progress value to set (should be between 0.0 and 1.0).
     */
    void Progress_Bar::Set_Progress(float New_Progress) {
        // Check if the new progress value exceeds the maximum limit
        if (New_Progress > 1.00) {
            // Report a percentage overflow warning
            Report(Get_Name() + " got a percentage overflow!");
            return;
        }

        // Update the progress value
        Progress = New_Progress;

        // Update the color of the progress bar based on the new progress value
        Color_Bar();

        // Mark the render buffer as dirty to reflect changes
        Dirty.Dirty(STAIN_TYPE::DEEP);

        // Trigger a frame update to re-render the progress bar
        Update_Frame();
    }

    /**
     * @brief Returns the current progress value of the progress bar.
     * @details This function returns the current progress value of the progress bar, which is a float between 0.0 and 1.0.
     * @return The current progress value of the progress bar.
     */
    float Progress_Bar::Get_Progress() {
        return Progress;
    }

    /**
     * @brief Toggles the border visibility of the progress bar.
     * @details This function toggles the border visibility of the progress bar.
     *          If the state has changed, it updates the border enabled state, marks the element as dirty for border changes, and updates the frame.
     * @param b The desired state of the border visibility.
     */
    void Progress_Bar::Show_Border(bool b) {
        if (b != Style->Border_Enabled.Value) {
            Style->Border_Enabled = b;

            // Adjust the width and height of the progress bar based on the border state
            if (b) Style->Width.Direct() += 2;
            else Style->Height.Direct() -= 2;

            // Mark the element as dirty for border changes
            Dirty.Dirty(STAIN_TYPE::EDGE);

            // Trigger a frame update to re-render the progress bar
            Update_Frame();
        }
    }
}
