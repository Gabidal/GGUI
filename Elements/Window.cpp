#include "Window.h"
#include "../Core/Renderer.h"

//End of user constructors.

/**
 * @brief A window element that wraps a console window.
 * This element is capable of modifying the window's title, border visibility, and colors.
 * @param title The title string to be displayed in the window's title bar.
 * @param s The Styling object to be used for the window.
 */
GGUI::Window::Window(std::string title, Styling s) : Element(s) {
    Pause_GGUI([this, title](){
        Update_Hidden_Border_Colors();

        Set_Title(title);
    });
}

/**
 * @brief Updates the colors of the hidden borders for the window.
 * This function prioritizes border color variants if they are available;
 * otherwise, it falls back to text colors or default values.
 */
void GGUI::Window::Update_Hidden_Border_Colors() {
    // Check if a custom border color is initialized and set it as the before hiding color
    if (Style->Border_Color.Status >= VALUE_STATE::INITIALIZED)
        Before_Hiding_Border_Color = Style->Border_Color.Value.Get<RGB>();
    // If not, check if a custom text color is initialized and use it instead
    else if (Style->Text_Color.Status >= VALUE_STATE::INITIALIZED)
        Before_Hiding_Border_Color = Style->Text_Color.Value.Get<RGB>();
    // If neither is initialized, use the default border color
    else
        Before_Hiding_Border_Color = STYLES::CONSTANTS::Default.Border_Color.Value.Get<RGB>();

    // Check if a custom border background color is initialized and set it as the before hiding background color
    if (Style->Border_Background_Color.Status >= VALUE_STATE::INITIALIZED)
        Before_Hiding_Border_Background_Color = Style->Border_Background_Color.Value.Get<RGB>();
    // If not, check if a custom background color is initialized and use it instead
    else if (Style->Background_Color.Status >= VALUE_STATE::INITIALIZED)
        Before_Hiding_Border_Background_Color = Style->Background_Color.Value.Get<RGB>();
    // If neither is initialized, use the default border background color
    else
        Before_Hiding_Border_Background_Color = STYLES::CONSTANTS::Default.Border_Background_Color.Value.Get<RGB>();
}

/**
 * @brief Sets the title of the window and updates border visibility and colors accordingly.
 * 
 * This function sets the window's title and ensures that the border is shown if the title is not empty.
 * If the window previously had hidden borders, it updates the border colors based on the background color.
 * 
 * @param t The new title for the window.
 */
void GGUI::Window::Set_Title(std::string t) {
    Pause_GGUI([this, t]() {
        // Set the window title
        Title = t;

        // If the window has no border and the title is not empty, show the border
        if (!Has_Border() && t.size() > 0) {
            Show_Border(true);

            // Mark that the window previously had hidden borders
            Has_Hidden_Borders = true;

            // Set the border colors to the current background color
            Before_Hiding_Border_Color = Get_Background_Color();
            Before_Hiding_Border_Background_Color = Get_Background_Color();
            Set_Border_Color(Get_Background_Color());
            Set_Border_Background_Color(Get_Background_Color());
        }

        // Set the window name to the new title
        Set_Name(t);

        // Mark the edge as dirty to trigger a frame update
        Dirty.Dirty(STAIN_TYPE::EDGE);
    });
}

/**
 * @brief Returns the title of the window.
 * 
 * @return The title of the window as a string.
 */
std::string GGUI::Window::Get_Title() {
    return Title;
}

/**
 * @brief Adds the border of the window to the rendered string.
 * 
 * @param w The window to add the border for.
 * @param Result The string to add the border to.
 */
void GGUI::Window::Add_Overhead(GGUI::Element* w, std::vector<GGUI::UTF>& Result){
    // Clean the edge stain
    Dirty.Clean(STAIN_TYPE::EDGE);

    if (!w->Has_Border())
        // If the window has no border, return
        return;
    
    // Get the custom border style
    GGUI::styled_border* custom_border = &Style->Border_Style;

    for (unsigned int y = 0; y < w->Get_Height(); y++){
        for (unsigned int x = 0; x < w->Get_Width(); x++){
            // Top left corner
            if (y == 0 && x == 0){
                // Add the top left corner symbol with the border color
                Result[y * w->Get_Width() + x] = GGUI::UTF(custom_border->TOP_LEFT_CORNER, w->Compose_All_Border_RGB_Values());
            }
            // Top right corner
            else if (y == 0 && x == w->Get_Width() - 1){
                // Add the top right corner symbol with the border color
                Result[y * w->Get_Width() + x] = GGUI::UTF(custom_border->TOP_RIGHT_CORNER, w->Compose_All_Border_RGB_Values());
            }
            // Bottom left corner
            else if (y == w->Get_Height() - 1 && x == 0){
                // Add the bottom left corner symbol with the border color
                Result[y * w->Get_Width() + x] = GGUI::UTF(custom_border->BOTTOM_LEFT_CORNER, w->Compose_All_Border_RGB_Values());
            }
            // Bottom right corner
            else if (y == w->Get_Height() - 1 && x == w->Get_Width() - 1){
                // Add the bottom right corner symbol with the border color
                Result[y * w->Get_Width() + x] = GGUI::UTF(custom_border->BOTTOM_RIGHT_CORNER, w->Compose_All_Border_RGB_Values());
            }
            // The title will only be written after the top left corner symbol until top right corner symbol and will NOT overflow
            else if (y == 0 && x <= ((GGUI::Window*)w)->Get_Title().size()){
                // Add the title symbol with the text color
                Result[y * w->Get_Width() + x] = GGUI::UTF(((GGUI::Window*)w)->Get_Title()[x - 1], w->Compose_All_Text_RGB_Values());
            }
            // The roof border
            else if (y == 0 || y == w->Get_Height() - 1){
                // Add the horizontal line symbol with the border color
                Result[y * w->Get_Width() + x] = GGUI::UTF(custom_border->HORIZONTAL_LINE, w->Compose_All_Border_RGB_Values());
            }
            // The left border
            else if (x == 0 || x == w->Get_Width() - 1){
                // Add the vertical line symbol with the border color
                Result[y * w->Get_Width() + x] = GGUI::UTF(custom_border->VERTICAL_LINE, w->Compose_All_Border_RGB_Values());
            }
        }
    }
}

/**
 * @brief Gets the name of the window.
 * 
 * @return The name of the window as a string.
 */
std::string GGUI::Window::Get_Name() const{
    return "Window<" + Name + ">";
}

/**
 * @brief Shows or hides the window's border.
 * @details This function toggles the border visibility of the window.
 *          If the state has changed, it updates the border enabled state, marks the element as dirty for border changes, and updates the frame.
 * @param b The desired state of the border visibility.
 */
void GGUI::Window::Show_Border(bool b){
    
    // This means that the window has setted a title whitout the borders, so we need to reinstate them.
    if (Has_Hidden_Borders){
        // Reinstate the border colors
        Set_Border_Color(Before_Hiding_Border_Color);
        Set_Border_Background_Color(Before_Hiding_Border_Background_Color);
    }

    // If the state has changed, update the border enabled state and mark the element as dirty for border changes
    if (b != Style->Border_Enabled.Value){
        Style->Border_Enabled = b;

        Dirty.Dirty(STAIN_TYPE::EDGE);
        Update_Frame();
    }
}

/**
 * @brief Shows or hides the window's border.
 * @details This function toggles the border visibility of the window.
 *          If the state has changed, it updates the border enabled state, marks the element as dirty for border changes, and updates the frame.
 * @param b The desired state of the border visibility.
 * @param Previous_State The current state of the border visibility.
 */
void GGUI::Window::Show_Border(bool b, bool Previous_State){
    
    // This means that the window has setted a title whitout the borders, so we need to reinstate them.
    if (Has_Hidden_Borders){
        // Reinstate the border colors
        Set_Border_Color(Before_Hiding_Border_Color);
        Set_Border_Background_Color(Before_Hiding_Border_Background_Color);
    }

    // If the state has changed, update the border enabled state and mark the element as dirty for border changes
    if (b != Previous_State){
        Style->Border_Enabled = b;

        // Mark the element as dirty for border changes
        Dirty.Dirty(STAIN_TYPE::EDGE);

        // Trigger a frame update to re-render the window
        Update_Frame();
    }
}


/**
 * @brief Sets the background color of the window.
 * @details This function sets the background color of the window to the specified RGB value.
 *          It marks the element as dirty for color updates and triggers a frame update.
 * @param color The RGB color to set as the background color.
 */
void GGUI::Window::Set_Background_Color(RGB color){
    Style->Background_Color = color;

    // Store the background color before hiding the border
    Before_Hiding_Border_Background_Color = color;

    // Mark the element as dirty for color updates
    Dirty.Dirty(STAIN_TYPE::COLOR);

    // Trigger a frame update to re-render the window
    Update_Frame();
}

/**
 * @brief Sets the text color of the window.
 * @details This function sets the text color of the window to the specified RGB value.
 *          It marks the element as dirty for color updates and triggers a frame update.
 * @param color The RGB color to set as the text color.
 */
void GGUI::Window::Set_Text_Color(RGB color) {
    // Store the value in the style
    Style->Text_Color = color;

    // Store the text color before hiding the border
    Before_Hiding_Border_Color = color;

    // Mark the element as dirty for color updates
    Dirty.Dirty(STAIN_TYPE::COLOR);

    // Trigger a frame update to re-render the window
    Update_Frame();
}

/**
 * @brief Sets the background color of the window's border.
 * @details This function sets the background color of the window's border to the specified RGB value.
 *          It marks the element as dirty for color updates and triggers a frame update.
 * @param color The RGB color to set as the background color of the window's border.
 */
void GGUI::Window::Set_Border_Background_Color(RGB color){
    // Store the value in the style
    Style->Border_Background_Color = color;

    // Store the border background color before hiding the border
    Before_Hiding_Border_Background_Color = color;

    // Mark the element as dirty for color updates
    Dirty.Dirty(STAIN_TYPE::COLOR);

    // Trigger a frame update to re-render the window
    Update_Frame();
}

/**
 * @brief Sets the color of the window's border.
 * @details This function sets the color of the window's border to the specified RGB value.
 *          It marks the element as dirty for color updates and triggers a frame update.
 * @param color The RGB color to set as the border color.
 */
void GGUI::Window::Set_Border_Color(RGB color){
    // Store the value in the style
    Style->Border_Color = color;

    // Store the border color before hiding the border
    Before_Hiding_Border_Color = color;

    // Mark the element as dirty for color updates
    Dirty.Dirty(STAIN_TYPE::COLOR);

    // Trigger a frame update to re-render the window
    Update_Frame();
}


