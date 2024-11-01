#include "Element.h"
#include "HTML.h"

#include "../Core/Renderer.h"

#include <algorithm>
#include <vector>
#include <cmath>

#undef min
#undef max

namespace GGUI{
    namespace SYMBOLS{
        GGUI::UTF EMPTY_UTF(' ', {GGUI::COLOR::WHITE, GGUI::COLOR::BLACK});
    }
}


/**
 * @brief Gets the colour as a string.
 *
 * @return The colour as a string. Format is: "X;Y;Z" where X, Y, Z are the values of red, green, blue respectively.
 */
std::string GGUI::RGB::Get_Colour() const{
    return Constants::To_String[Red] + Constants::ANSI::SEPARATE + Constants::To_String[Green] + Constants::ANSI::SEPARATE + Constants::To_String[Blue];
}

/**
 * @brief Converts the RGB colour to a string.
 *
 * @param Result The result string.
 */
void GGUI::RGB::Get_Colour_As_Super_String(Super_String* Result) const{
    // Add the red value to the string
    Result->Add(Constants::To_Compact[Red]);
    
    // Add the separator to the string
    Result->Add(Constants::ANSI::SEPARATE);
    
    // Add the green value to the string
    Result->Add(Constants::To_Compact[Green]);
    
    // Add the separator to the string
    Result->Add(Constants::ANSI::SEPARATE);
    
    // Add the blue value to the string
    Result->Add(Constants::To_Compact[Blue]);
}
    
/**
 * @brief A structure to hold the border style of a widget.
 *
 * The style is represented as a vector of strings, each string being a character
 * that will be used to draw the border of the widget. The vector must have the
 * following size and order:
 * - 0: Top left corner
 * - 1: Bottom left corner
 * - 2: Top right corner
 * - 3: Bottom right corner
 * - 4: Vertical line
 * - 5: Horizontal line
 * - 6: Vertical right connector
 * - 7: Vertical left connector
 * - 8: Horizontal bottom connector
 * - 9: Horizontal top connector
 * - 10: Cross connector
 */
GGUI::styled_border::styled_border(std::vector<const char*> values, VALUE_STATE Default) : style_base(Default){
    if(values.size() == 11){
        TOP_LEFT_CORNER = values[0];
        BOTTOM_LEFT_CORNER = values[1];
        TOP_RIGHT_CORNER = values[2];
        BOTTOM_RIGHT_CORNER = values[3];
        VERTICAL_LINE = values[4];
        HORIZONTAL_LINE = values[5];
        VERTICAL_RIGHT_CONNECTOR = values[6];
        VERTICAL_LEFT_CONNECTOR = values[7];
        HORIZONTAL_BOTTOM_CONNECTOR = values[8];
        HORIZONTAL_TOP_CONNECTOR = values[9];
        CROSS_CONNECTOR = values[10];
    }
    else{
        Report_Stack("Internal error: Border style value has wrong number of values. Expected 11 got: '" + std::to_string(values.size()) + "'");
    }
}

/**
 * @brief Copies the values of the given Styling object to the current object.
 *
 * This will copy all the values of the given Styling object to the current object.
 *
 * @param other The Styling object to copy from.
 */
void GGUI::Styling::Copy(const Styling& other){
    Border_Enabled = other.Border_Enabled;
    Text_Color = other.Text_Color;
    Background_Color = other.Background_Color;
    Border_Color = other.Border_Color;
    Border_Background_Color = other.Border_Background_Color;
    Hover_Border_Color = other.Hover_Border_Color;
    Hover_Text_Color = other.Hover_Text_Color;
    Hover_Background_Color = other.Hover_Background_Color;
    Hover_Border_Background_Color = other.Hover_Border_Background_Color;
    Focus_Border_Color = other.Focus_Border_Color;
    Focus_Text_Color = other.Focus_Text_Color;
    Focus_Background_Color = other.Focus_Background_Color;
    Focus_Border_Background_Color = other.Focus_Border_Background_Color;
    Border_Style = other.Border_Style;
    Flow_Priority = other.Flow_Priority;
    Wrap = other.Wrap;
    Allow_Overflow = other.Allow_Overflow;
    Allow_Dynamic_Size = other.Allow_Dynamic_Size;
    Margin = other.Margin;
    Shadow = other.Shadow;
    Opacity = other.Opacity;
    Allow_Scrolling = other.Allow_Scrolling;
}

/**
 * @brief Converts the UTF character to a string.
 *
 * This function converts the UTF character to a string by combining the foreground and background colour
 * strings with the character itself.
 *
 * @return The string representation of the UTF character.
 */
std::string GGUI::UTF::To_String(){
    std::string Result =
        // Get the foreground colour and style as a string
        Foreground.Get_Over_Head(true) + Foreground.Get_Colour() + Constants::ANSI::END_COMMAND + 
        // Get the background colour and style as a string
        Background.Get_Over_Head(false) + Background.Get_Colour() + Constants::ANSI::END_COMMAND;

    if(Is(UTF_FLAG::IS_UNICODE)){
        // Add the const char* to the Result
        Result.append(Unicode, Unicode_Length);
    }
    else{
        Result += Ascii;
    }

    // Add the reset ANSI code to the end of the string
    return Result + Constants::ANSI::RESET_COLOR;
}

/**
 * @brief Converts the UTF character to a string.
 *
 * This function converts the UTF character to a string by combining the foreground and background colour
 * strings with the character itself.
 *
 * @param Result The result string.
 * @param Text_Overhead The foreground colour and style as a string.
 * @param Background_Overhead The background colour and style as a string.
 * @param Text_Colour The foreground colour as a string.
 * @param Background_Colour The background colour as a string.
 */
void GGUI::UTF::To_Super_String(GGUI::Super_String* Result, Super_String* Text_Overhead, Super_String* Background_Overhead, Super_String* Text_Colour, Super_String* Background_Colour){
    // Get the foreground colour and style as a string
    Foreground.Get_Over_Head_As_Super_String(Text_Overhead, true);
    // Get the foreground colour as a string
    Foreground.Get_Colour_As_Super_String(Text_Colour);
    // Get the background colour and style as a string
    Background.Get_Over_Head_As_Super_String(Background_Overhead, false);
    // Get the background colour as a string
    Background.Get_Colour_As_Super_String(Background_Colour);

    Result->Add(Text_Overhead, true);
    Result->Add(Text_Colour, true);
    Result->Add(Constants::ANSI::END_COMMAND);
    Result->Add(Background_Overhead, true);
    Result->Add(Background_Colour, true);

    if (Is(UTF_FLAG::IS_UNICODE)){
        // Add the const char* to the Result
        Result->Add(Unicode, Unicode_Length);
    }
    else{
        Result->Add(Ascii);
    }

    // Add the reset ANSI code to the end of the string
    Result->Add(Constants::ANSI::RESET_COLOR);
}

/**
 * @brief Converts the UTF character to an encoded string.
 *
 * This function converts the UTF character to an encoded string by applying
 * encoding flags and combining the foreground and background colour strings
 * with the character itself.
 *
 * @return The encoded string representation of the UTF character.
 */
std::string GGUI::UTF::To_Encoded_String() {
    std::string Result;

    // Check if the start encoding flag is set
    if (Is(UTF_FLAG::ENCODE_START)) {
        // Add the foreground and background colour and style to the result
        Result = Foreground.Get_Over_Head(true) + Foreground.Get_Colour() + Constants::ANSI::END_COMMAND 
               + Background.Get_Over_Head(false) + Background.Get_Colour() + Constants::ANSI::END_COMMAND;
    }

    // Check if the character is a Unicode character
    if (Is(UTF_FLAG::IS_UNICODE)) {
        // Append the Unicode character to the result
        Result.append(Unicode, Unicode_Length);
    } else {
        // Append the ASCII character to the result
        Result += Ascii;
    }

    // Check if the end encoding flag is set
    if (Is(UTF_FLAG::ENCODE_END)) {
        // Add the reset ANSI code to the end of the string
        Result += Constants::ANSI::RESET_COLOR;
    }

    return Result;
}

/**
 * @brief Converts the UTF character to an encoded Super_String.
 *
 * This function converts the UTF character to an encoded Super_String by applying
 * encoding flags and combining the foreground and background colour strings
 * with the character itself.
 *
 * @param Result The Super_String to which the encoded string will be added.
 * @param Text_Overhead The Super_String where the foreground colour overhead will be stored.
 * @param Background_Overhead The Super_String where the background colour overhead will be stored.
 * @param Text_Colour The Super_String where the foreground colour will be stored.
 * @param Background_Colour The Super_String where the background colour will be stored.
 */
void GGUI::UTF::To_Encoded_Super_String(Super_String* Result, Super_String* Text_Overhead, Super_String* Background_Overhead, Super_String* Text_Colour, Super_String* Background_Colour) {

    if (Is(UTF_FLAG::ENCODE_START)) {
        // Add the foreground and background colour and style to the result
        Foreground.Get_Over_Head_As_Super_String(Text_Overhead, true);
        Foreground.Get_Colour_As_Super_String(Text_Colour);
        Background.Get_Over_Head_As_Super_String(Background_Overhead, false);
        Background.Get_Colour_As_Super_String(Background_Colour);

        Result->Add(Text_Overhead, true);
        Result->Add(Text_Colour, true);
        Result->Add(Constants::ANSI::END_COMMAND);
        Result->Add(Background_Overhead, true);
        Result->Add(Background_Colour, true);
        Result->Add(Constants::ANSI::END_COMMAND);
    }

    if (Is(UTF_FLAG::IS_UNICODE)) {
        // Append the Unicode character to the result
        Result->Add(Unicode, Unicode_Length);
    } else {
        // Append the ASCII character to the result
        Result->Add(Ascii);
    }

    if (Is(UTF_FLAG::ENCODE_END)) {
        // Add the reset ANSI code to the end of the string
        Result->Add(Constants::ANSI::RESET_COLOR);
    }
}

/**
 * The constructor for the Element class.
 *
 * This constructor is used when an Element is created without a parent.
 * In this case, the Element is created as a root object, and it will be
 * automatically added to the list of root objects.
 *
 * @param None
 */
GGUI::Element::Element() {
    Name = std::to_string((unsigned long long)this);
    Parse_Classes();

    Fully_Stain();

    // If this is true, then the user probably:
    // A.) Doesn't know what the fuck he is doing.
    // B.) He is trying to use the OUTBOX feature.
    if (GGUI::Main == nullptr){
        // Lets go with B.
        Report_Stack("OUTBOX not supported, cannot anchor: " + Get_Name());
    }
}

/**
 * The constructor for the Element class that accepts a Styling object.
 *
 * This constructor is used when an Element is created without a parent.
 * In this case, the Element is created as a root object, and it will be
 * automatically added to the list of root objects.
 *
 * @param s The Styling object to use for the Element.
 */
GGUI::Element::Element(Styling s) : Element(){
    Style = new Styling(s);

    Style->Embed_Styles(this);
}

/**
 * @brief Copy constructor for the Element class.
 *
 * This constructor is disabled and should not be used.
 * Instead, use the Copy() method to create a copy of an Element.
 *
 * @param copyable The Element object to be copied.
 */
GGUI::Element::Element(const Element& copyable) {
    // Emit a warning message that the copy constructor should not be used.
    Report("Don't use copy constructor use " + copyable.Get_Name() + "->Copy() instead!!!");
}

/**
 * @brief The destructor for the Element class.
 *
 * This destructor is responsible for properly deallocating all the memory
 * allocated by the Element object.
 *
 * @note This destructor is also responsible for cleaning up the parent
 * element's vector of child elements and the event handlers list.
 */
GGUI::Element::~Element(){
    // Make sure this element is not listed in the parent element.
    // And if it does, then remove it from the parent element.
    if (Parent) {
        // Find the element in the parent's vector of child elements and remove it.
        for (unsigned int i = 0; i < Parent->Style->Childs.size(); i++)
            if (Parent->Style->Childs[i] == this){
                Parent->Style->Childs.erase(Parent->Style->Childs.begin() + i);

                // This may not be enough for the parent to know where to resample the buffer where this child used to be.
                Parent->Dirty.Dirty(STAIN_TYPE::DEEP);

                break;  // There should be no possibility, that there are appended two or more of this exact same element, they should be copied!!!
            }
    }

    // Fire all the childs.
    for (int i = (signed)Style->Childs.size() -1; i >= 0; i--)
        if (Style->Childs[i]->Parent == this) 
            delete Style->Childs[i];

    // Delete all the styles.
    delete Style;

    //now also update the event handlers.
    for (unsigned int i = 0; i < Event_Handlers.size(); i++)
        if (Event_Handlers[i]->Host == this){
            
            //delete the event
            delete Event_Handlers[i];

            //remove the event from the list
            Event_Handlers.erase(Event_Handlers.begin() + i);
        }

    // Now make sure that if the Focused_On element points to this element, then set it to nullptr
    if (Is_Focused())
        GGUI::Focused_On = nullptr;

    // Now make sure that if the Hovered_On element points to this element, then set it to nullptr
    if (Is_Hovered())
        GGUI::Hovered_On = nullptr;
}   

// @brief Marks the Element as fully dirty by setting all stain types.
// 
// This function sets each stain type on the Dirty object, indicating
// that the Element needs to be reprocessed for all attributes.
void GGUI::Element::Fully_Stain() {
    // Mark the element as dirty for all possible stain types to ensure
    // complete re-evaluation and rendering.
    this->Dirty.Dirty(STAIN_TYPE::CLASS | STAIN_TYPE::STRETCH | 
                      STAIN_TYPE::COLOR | STAIN_TYPE::DEEP | 
                      STAIN_TYPE::EDGE | STAIN_TYPE::MOVE);
}

/**
 * @brief Copies the state of the abstract element to the current element.
 *
 * This function will copy the state of the abstract element to the current element.
 * It will copy the following states: focused and show.
 *
 * @param abstract The abstract element to copy the state from.
 */
void GGUI::Element::Inherit_States_From(Element* abstract) {
    Focused = abstract->Focused;
    Show = abstract->Show;
}

/**
 * @brief Composes the RGB values of the text color and background color of the element.
 * 
 * This function will return a pair of RGB values, where the first element is the
 * color of the text and the second element is the color of the background.
 * 
 * If the element is focused, the function will return the RGB values of the focused
 * text color and background color. If the element is hovered, the function will
 * return the RGB values of the hovered text color and background color. Otherwise,
 * the function will return the RGB values of the normal text color and background
 * color.
 * 
 * @return A pair of RGB values representing the text color and background color of the element.
 */
std::pair<GGUI::RGB, GGUI::RGB>  GGUI::Element::Compose_All_Text_RGB_Values(){
    if (Focused){
        return {Style->Focus_Text_Color.Value.Get<RGB>(), Style->Focus_Background_Color.Value.Get<RGB>()};
    }
    else if (Hovered){
        return {Style->Hover_Text_Color.Value.Get<RGB>(), Style->Hover_Background_Color.Value.Get<RGB>()};
    }
    else{
        return {Style->Text_Color.Value.Get<RGB>(), Style->Background_Color.Value.Get<RGB>()};
    }
}

/**
 * @brief Composes the RGB values of the text color of the element.
 * 
 * This function will return the RGB values of the text color of the element.
 * If the element is focused, the function will return the RGB values of the focused
 * text color. If the element is hovered, the function will return the RGB values of the hovered
 * text color. Otherwise, the function will return the RGB values of the normal text color.
 * 
 * @return The RGB color of the element's text.
 */
GGUI::RGB GGUI::Element::Compose_Text_RGB_Values(){
    if (Focused){
        return Style->Focus_Text_Color.Value.Get<RGB>();
    }
    else if (Hovered){
        return Style->Hover_Text_Color.Value.Get<RGB>();
    }
    else{
        return Style->Text_Color.Value.Get<RGB>();
    }
}

/**
 * @brief Composes the RGB values of the background color of the element.
 * 
 * This function will return the RGB values of the background color of the element.
 * If the element is focused, the function will return the RGB values of the focused
 * background color. If the element is hovered, the function will return the RGB values of the hovered
 * background color. Otherwise, the function will return the RGB values of the normal background color.
 * 
 * @return The RGB color of the element's background.
 */
GGUI::RGB GGUI::Element::Compose_Background_RGB_Values(){
    if (Focused){
        return Style->Focus_Background_Color.Value.Get<RGB>();
    }
    else if (Hovered){
        return Style->Hover_Background_Color.Value.Get<RGB>();
    }
    else{
        return Style->Background_Color.Value.Get<RGB>();
    }
}

/**
 * @brief Composes the RGB values of the border color and background color of the element.
 * @details This function will return the RGB values of the border color and background color of the element.
 * If the element is focused, the function will return the RGB values of the focused border color and background color.
 * If the element is hovered, the function will return the RGB values of the hovered border color and background color.
 * Otherwise, the function will return the RGB values of the normal border color and background color.
 * @return A pair of RGB values representing the border color and background color of the element.
 */
std::pair<GGUI::RGB, GGUI::RGB> GGUI::Element::Compose_All_Border_RGB_Values(){
    if (Focused){
        return {Style->Focus_Border_Color.Value.Get<RGB>(), Style->Focus_Border_Background_Color.Value.Get<RGB>()};
    }
    else if (Hovered){
        return {Style->Hover_Border_Color.Value.Get<RGB>(), Style->Hover_Border_Background_Color.Value.Get<RGB>()};
    }
    else{
        return {Style->Border_Color.Value.Get<RGB>(), Style->Border_Background_Color.Value.Get<RGB>()};
    }
}

/**
 * @brief Sets the opacity of the element.
 * @details This function takes a float value between 0.0f and 1.0f and sets the
 * opacity of the element to that value. If the value is greater than 1.0f, the
 * function will report an error and do nothing.
 * @param[in] Opacity The opacity value to set.
 */
void GGUI::Element::Set_Opacity(float Opacity){
    if (Opacity > 1.0f)
        Report_Stack("Opacity value is too high: " + std::to_string(Opacity) + " for element: " + Get_Name());

    Style->Opacity.Set(Opacity);

    Dirty.Dirty(STAIN_TYPE::STRETCH);
    Update_Frame();
}

/**
 * @brief Sets the opacity of the element using an integer percentage.
 * @details This function takes an unsigned integer value between 0 and 100, representing the opacity percentage,
 * and sets the element's opacity. If the value is greater than 100, it will report an error and do nothing.
 * @param[in] Opacity The opacity percentage to set.
 */
void GGUI::Element::Set_Opacity(unsigned int Opacity) {
    // Check if the provided opacity is within valid range (0-100)
    if (Opacity > 100) {
        // Report an error if the opacity value is too high
        Report_Stack("Opacity value is too high: " + std::to_string(Opacity) + " for element: " + Get_Name());
    }

    // Convert the opacity percentage to a float value between 0.0 and 1.0 and set it
    Style->Opacity.Set((float)Opacity / 100.f);

    // Mark the element as dirty to trigger a visual update
    Dirty.Dirty(STAIN_TYPE::STRETCH);
    Update_Frame(); // Update the frame to reflect the changes
}

/**
 * @brief Gets the opacity of the element.
 * @details This function returns the current opacity of the element as a float value.
 *          The opacity is a value between 0.0 and 1.0, where 0.0 is fully transparent
 *          and 1.0 is fully opaque.
 * @return The current opacity of the element.
 */
float GGUI::Element::Get_Opacity() {
    return Style->Opacity.Get();
}

/**
 * @brief Checks if the element is transparent.
 * @details This function determines whether the element is transparent by checking
 *          if the element's opacity is not equal to 1.0f. An opacity less than 1.0f
 *          indicates that the element is partially or fully transparent.
 * @return True if the element is transparent; otherwise, false.
 */
bool GGUI::Element::Is_Transparent() {
    // Get the current opacity of the element and compare it with 1.0f
    return Get_Opacity() != 1.0f;
}

/**
 * @brief Gets the processed width of the element.
 * @details This function returns the width of the element after any post-processing
 *          has been applied. If the element has not been post-processed, the
 *          original width of the element is returned.
 * @return The processed width of the element.
 */
unsigned int GGUI::Element::Get_Processed_Width(){
    if (Post_Process_Width != 0){
        return Post_Process_Width;
    }
    return Get_Width();
}

/**
 * @brief Gets the processed height of the element.
 * @details This function returns the height of the element after any post-processing
 *          has been applied. If the element has not been post-processed, the
 *          original height of the element is returned.
 * @return The processed height of the element.
 */
unsigned int GGUI::Element::Get_Processed_Height(){
    if (Post_Process_Height != 0){
        return Post_Process_Height;
    }
    return Get_Height();
}

/**
 * @brief Configures and displays the shadow for the element.
 * @details This function sets the shadow properties such as direction, color, opacity, 
 *          and length, and applies the shadow effect to the element. It adjusts the 
 *          element's position to account for the shadow and marks the element as dirty 
 *          for a visual update.
 * @param[in] Direction The direction vector of the shadow.
 * @param[in] Shadow_Color The color of the shadow.
 * @param[in] Opacity The opacity of the shadow, between 0.0f (fully transparent) and 1.0f (fully opaque).
 * @param[in] Length The length of the shadow.
 */
void GGUI::Element::Show_Shadow(FVector2 Direction, RGB Shadow_Color, float Opacity, float Length) {
    shadow* properties = &Style->Shadow;

    // Set shadow properties
    properties->Color = Shadow_Color;
    properties->Direction = {Direction.X, Direction.Y, Length};
    properties->Opacity = Opacity;

    // Adjust element's position based on shadow length and opacity
    Style->Position.Direct().X -= Length * Opacity;
    Style->Position.Direct().Y -= Length * Opacity;

    // Apply the inverse of the direction to the element's position
    Style->Position.Direct() += Direction * -1;
    
    // Mark shadow properties as evaluated
    properties->Status = VALUE_STATE::VALUE;

    // Mark the element as dirty for a visual update
    Dirty.Dirty(STAIN_TYPE::STRETCH);
    Update_Frame();
}

/**
 * @brief Displays the shadow for the element.
 * @details This function sets the shadow properties such as direction, color, opacity, and length, and applies the shadow effect to the element. It adjusts the element's position to account for the shadow and marks the element as dirty for a visual update. The direction vector of the shadow is set to (0, 0) by default, which means the shadow will appear directly below the element.
 * @param[in] Shadow_Color The color of the shadow.
 * @param[in] Opacity The opacity of the shadow, between 0.0f (fully transparent) and 1.0f (fully opaque).
 * @param[in] Length The length of the shadow.
 */
void GGUI::Element::Show_Shadow(RGB Shadow_Color, float Opacity, float Length){
    shadow* properties = &Style->Shadow;

    // Set shadow properties
    properties->Color = Shadow_Color;
    properties->Direction = {0, 0, Length};
    properties->Opacity = Opacity;

    // Adjust element's position based on shadow length and opacity
    Style->Position.Direct().X -= Length * Opacity;
    Style->Position.Direct().Y -= Length * Opacity;

    // Mark shadow properties as evaluated
    properties->Status = VALUE_STATE::VALUE;

    // Mark the element as dirty for a visual update
    Dirty.Dirty(STAIN_TYPE::STRETCH);
    Update_Frame();
}

/**
 * @brief Sets the shadow properties for the element.
 * @details This function sets the shadow properties such as direction, color, opacity, and length, and applies the shadow effect to the element. It also marks the element as dirty for a visual update.
 * @param[in] s The shadow properties to set.
 */
void GGUI::Element::Set_Shadow(shadow s){
    Style->Shadow = s;

    // Mark the element as dirty for a visual update
    Dirty.Dirty(STAIN_TYPE::STRETCH);
    Update_Frame();
}

/**
 * @brief Sets the parent of this element.
 * @details This function sets the parent of this element to the given element.
 *          If the given element is nullptr, it will clear the parent of this
 *          element.
 * @param[in] parent The parent element to set.
 */
void GGUI::Element::Set_Parent(Element* parent){
    if (parent){
        Parent = parent;
    } else {
        Parent = nullptr;
    }
}

/**
 * @brief Accumulates all the classes and their styles.
 * @details This method accumulates all the classes and their styles to the
 *          current element.
 */
void GGUI::Element::Parse_Classes(){
    if (Style == nullptr){
        Style = new Styling();
    }

    GGUI::Classes([this](auto& classes){
        //Go through all classes and their styles and accumulate them.
        for(auto Class : Classes){

            // The class wanted has not been yet constructed.
            // Pass it for the next render iteration
            if (classes.find(Class) == classes.end()){
                Dirty.Dirty(STAIN_TYPE::CLASS);
            }

            // Copy the style of the class to the current element.
            Style->Copy(new Styling(classes[Class]));
            
        }
    });
}

/**
 * @brief Sets the focus state of the element.
 * @details Sets the focus state of the element to the given value.
 *          If the focus state changes, the element will be dirtied and the frame will be updated.
 * @param f The new focus state.
 */
void GGUI::Element::Set_Focus(bool f){
    if (f != Focused){
        // If the focus state has changed, dirty the element and update the frame.
        Dirty.Dirty(STAIN_TYPE::COLOR | STAIN_TYPE::EDGE);

        Focused = f;

        Update_Frame();
    }
}

/**
 * @brief Sets the hover state of the element.
 * @details Sets the hover state of the element to the given value.
 *          If the hover state changes, the element will be dirtied and the frame will be updated.
 * @param h The new hover state.
 */
void GGUI::Element::Set_Hover_State(bool h){
    if (h != Hovered){
        // If the hover state has changed, dirty the element and update the frame.
        Dirty.Dirty(STAIN_TYPE::COLOR | STAIN_TYPE::EDGE);

        Hovered = h;

        Update_Frame();
    }
}

/**
 * @brief Retrieves the styling information of the element.
 * @details This function returns the current styling object associated with the element.
 *          The styling object contains various style attributes such as colors, borders, etc.
 * @return The styling object of the element.
 */
GGUI::Styling GGUI::Element::Get_Style() {
    // Return the styling object associated with this element.
    return *Style;
}

/**
 * @brief Sets the styling information of the element.
 * @details This function sets the styling information of the element to the given value.
 *          If the element already has a styling object associated with it, the function will
 *          copy the given styling information to the existing object. Otherwise, the function
 *          will create a new styling object and associate it with the element.
 * @param css The new styling information to associate with the element.
 */
void GGUI::Element::Set_Style(Styling css){
    if (Style)
        Style->Copy(css);
    else
        Style = new Styling(css);

    // Update the frame after changing the styling information.
    Update_Frame();
}

/**
 * @brief Adds a class to the element.
 * @details This function adds the given class to the element's class list.
 *          If the class does not exist in the global class map, a new ID is assigned to the class.
 *          The element is then marked as dirty, which will trigger a re-render of the element.
 * @param class_name The name of the class to add.
 */
void GGUI::Element::Add_Class(std::string class_name){
    // Check if the class already exists in the global class map.
    if (Class_Names.find(class_name) != Class_Names.end()) {
        // If the class already exists, add the existing ID to the element's class list.
        Classes.push_back(Class_Names[class_name]);
    }
    else {
        // If the class does not exist, assign a new ID to the class and add it to the element's class list.
        Classes.push_back(GGUI::Get_Free_Class_ID(class_name));
    }

    // Mark the element as dirty after adding a new class.
    Dirty.Dirty(STAIN_TYPE::STRETCH | STAIN_TYPE::COLOR | STAIN_TYPE::EDGE);
}

/**
 * @brief Checks if the element has the given class.
 * @details This function takes a class name and checks if the element has the class in its class list.
 *          If the class does not exist in the global class map, the function will return false.
 *          If the class exists, the function will return true if the element has the class in its list.
 * @param s The name of the class to check.
 * @return True if the element has the class, false otherwise.
 */
bool GGUI::Element::Has(std::string s) {
    //first convert the string to the ID
    int id = Class_Names[s];

    //then check if the element has the class
    for (unsigned int i = 0; i < Classes.size(); i++) {
        if (Classes[i] == id) {
            return true;
        }
    }

    return false;
}


/**
 * @brief Sets the border visibility of the element.
 * @details This function takes a boolean as a parameter and sets the border visibility of the element accordingly.
 *          If the new state is different from the current state, the element will be marked as dirty with the EDGE stain.
 * @param b The new state of the border visibility.
 */
void GGUI::Element::Show_Border(bool b){
    if (b != Style->Border_Enabled.Value){
        Style->Border_Enabled = b;
        Dirty.Dirty(STAIN_TYPE::EDGE);
        Update_Frame();
    }
}

/**
 * @brief Sets the border visibility of the element.
 * @details This function toggles the border visibility based on the provided state.
 *          If the state has changed, it updates the border visibility, marks the 
 *          element as dirty with the EDGE stain, and updates the frame.
 * @param b The desired state of the border visibility.
 * @param Previous_State The current state of the border visibility.
 */
void GGUI::Element::Show_Border(bool b, bool Previous_State) {
    if (b != Previous_State) {
        // Update the border enabled state
        Style->Border_Enabled = b;

        // Mark the element as dirty for border changes
        Dirty.Dirty(STAIN_TYPE::EDGE);

        // Refresh the element's frame to reflect changes
        Update_Frame();
    }
}

/**
 * @brief Checks if the element has a border.
 * @details This function checks if the element has a border.
 *          It returns true if the element has a border, false otherwise.
 * @return True if the element has a border, false otherwise.
 */
bool GGUI::Element::Has_Border(){
    return Style->Border_Enabled.Value;
}

/**
 * @brief Adds a child element to the element.
 * @details This function adds a child element to the element. If the element has a border, it will
 *          be taken into account when calculating the size of the parent element. If the child element
 *          exceeds the size of the parent element, the parent element will be resized to fit the child
 *          element. If the parent element is not allowed to resize, the child element will be resized to
 *          fit the parent element.
 * @param Child The child element to add.
 */
void GGUI::Element::Add_Child(Element* Child){
    // Check if the child element exceeds the size of the parent element
    bool This_Has_Border = Has_Border();
    bool Child_Has_Border = Child->Has_Border();

    int Border_Offset = (This_Has_Border - Child_Has_Border) * This_Has_Border;

    if (
        Child->Style->Position.Get().X + Child->Get_Width() > (Get_Width() - Border_Offset) || 
        Child->Style->Position.Get().Y + Child->Get_Height() > (Get_Height() - Border_Offset)
    ){
        if (Style->Allow_Dynamic_Size.Value){
            // Add the border offset to the width and the height to count for the border collision and evade it. 
            unsigned int New_Width = GGUI::Max(Child->Style->Position.Get().X + Child->Get_Width() + Border_Offset*2, Get_Width());
            unsigned int New_Height = GGUI::Max(Child->Style->Position.Get().Y + Child->Get_Height() + Border_Offset*2, Get_Height());

            // Resize the parent element to fit the child element
            Set_Height(New_Height);
            Set_Width(New_Width);
        }
        else if (Child->Resize_To(this) == false){

            // Report an error if the child element exceeds the size of the parent element and the parent element is not allowed to resize
            GGUI::Report(
                "Window exceeded static bounds\n "
                "Starts at: {" + std::to_string(Child->Style->Position.Get().X) + ", " + std::to_string(Child->Style->Position.Get().Y) + "}\n "
                "Ends at: {" + std::to_string(Child->Style->Position.Get().X + Child->Get_Width()) + ", " + std::to_string(Child->Style->Position.Get().Y + Child->Get_Height()) + "}\n "
                "Max is at: {" + std::to_string(Get_Width()) + ", " + std::to_string(Get_Height()) + "}\n "
            );

            return;
        }
    }

    // Mark the parent element as dirty with the DEEP stain
    Dirty.Dirty(STAIN_TYPE::DEEP);
    Child->Parent = this;

    // Add the child element to the parent's child list
    Element_Names.insert({Child->Name, Child});

    Style->Childs.push_back(Child);

    // Make sure that elements with higher Z, are rendered later, making them visible as on top.
    Re_Order_Childs();

    // Refresh the parent element's frame to reflect changes
    Update_Frame();
}

/**
 * @brief Adds a vector of child elements to the current element.
 * @param childs The vector of child elements to add.
 *
 * This function adds all the child elements to the current element by calling the Add_Child function for each element in the vector.
 * It also marks the current element as dirty with the DEEP stain after adding all the elements.
 */
void GGUI::Element::Set_Childs(std::vector<Element*> childs){
    Pause_GGUI([this, childs](){
        for (auto& Child : childs){
            Add_Child(Child);
        }
        Dirty.Dirty(STAIN_TYPE::DEEP);
    });
}

/**
 * @brief Retrieves the list of child elements.
 * @details This function returns a reference to the vector containing all child elements
 *          associated with the current element's style.
 * @return A reference to the vector of child elements.
 */
std::vector<GGUI::Element*>& GGUI::Element::Get_Childs() {
    // Return the vector of child elements from the style object.
    return Style->Childs;
}

/**
 * @brief Removes a child element from the current element.
 * @param handle The pointer to the child element to be removed.
 * @return true if the element was successfully removed, false if not.
 *
 * This function iterates through the vector of child elements and checks
 * if the element at the current index is equal to the handle passed as an argument.
 * If it is, the element is deleted and the parent element is marked as dirty with the DEEP and COLOR stains.
 * If the currently focused element is the one being removed, the mouse position is set to the parent element's position.
 */
bool GGUI::Element::Remove(Element* handle){
    for (unsigned int i = 0; i < Style->Childs.size(); i++){
        if (Style->Childs[i] == handle){
            // If the mouse is focused on this about to be deleted element, change mouse position into it's parent Position.
            if (Focused_On == Style->Childs[i]){
                Mouse = Style->Childs[i]->Parent->Style->Position.Get();
            }

            delete handle;

            Dirty.Dirty(STAIN_TYPE::DEEP | STAIN_TYPE::COLOR);

            return true;
        }
    }
    return false;
}

/**
 * @brief Updates the parent element of the current element.
 * @details This function is called when the current element is added, removed, or moved
 *          to a different parent element. It marks the parent element as dirty and
 *          requests a render update.
 * @param New_Element The new parent element.
 *
 * @note If the parent element does not have a valid render buffer (i.e., its
 *       `Is_Displayed()` function returns false), this function marks the parent
 *       element as dirty with the `STAIN_TYPE::DEEP` and `STAIN_TYPE::COLOR` stains.
 *       This ensures that the parent element is re-rendered from scratch when the
 *       rendering thread is updated.
 */
void GGUI::Element::Update_Parent(Element* New_Element){
    // Normally elements don't do anything
    if (!New_Element->Is_Displayed()){
        // Mark the parent element as dirty with the STAIN_TYPE::DEEP and STAIN_TYPE::COLOR stains
        Fully_Stain();
    }

    // When the child is unable to flag changes on parent Render(), like on removal-
    // Then ask the parent to discard the previous buffer and render from scratch.
    if (Parent){
        // Mark the parent element as dirty with the STAIN_TYPE::DEEP and STAIN_TYPE::COLOR stains
        Fully_Stain();
        // Request a render update
        Update_Frame();
    }
}

/**
 * @brief Executes the handler function associated with a given state.
 * @details This function checks if there is a registered handler for the specified state.
 *          If a handler exists, it invokes the handler function.
 * @param s The state for which the handler should be executed.
 */
void GGUI::Element::Check(State s){
    // Check if a handler for the state 's' exists in the map
    if (State_Handlers.find(s) != State_Handlers.end()){
        // Invoke the handler function associated with the state 's'
        State_Handlers[s]();
    }
}

/**
 * @brief Displays or hides the element and all its children.
 * @details This function changes the display status of the element and all its children.
 *          If the element is displayed, all its children are also displayed. If the element is hidden,
 *          all its children are also hidden.
 * @param f A boolean indicating whether to display (true) or hide (false) the element and its children.
 */
void GGUI::Element::Display(bool f){
    // Check if the to be displayed is true and the element wasn't already displayed.
    if (f != Show){
        Dirty.Dirty(STAIN_TYPE::STATE);
        Show = f;

        if (f){
            Check(State::RENDERED);
        }
        else{
            Check(State::HIDDEN);
        }

        // now also update all children, this is for the sake of events, since they do not obey AST structure where parental hidden would stop going deeper into AST events are linear list.
        GGUI::Pause_GGUI([this, f](){
            for (Element* c : Style->Childs){
                c->Display(f);
            }
        });
    }
}

/**
 * @brief Returns whether the element is currently displayed.
 * @details This function is used to check whether the element is currently displayed or hidden.
 *          It returns true if the element is displayed and false if the element is hidden.
 * @return A boolean indicating whether the element is displayed (true) or hidden (false).
 */
bool GGUI::Element::Is_Displayed(){
    return Show;
}

/**
 * @brief Removes the element at a given index from the list of child elements.
 * @details This function checks if the index is valid (i.e. if the index is within the bounds of the vector of child elements).
 *          If the index is valid, it removes the element at the specified index from the vector of child elements and deletes the element.
 *          If the index is invalid, the function returns false.
 * @param index The index of the element to remove.
 * @return True if the element was successfully removed, false otherwise.
 */
bool GGUI::Element::Remove(unsigned int index){
    if (index > Style->Childs.size() - 1){
        return false;
    }

    Element* tmp = Style->Childs[index];

    // If the mouse is currently focused on the element that is about to be deleted, change the mouse position into the element's parent position.
    if (Focused_On == tmp){
        Mouse = tmp->Parent->Style->Position.Get();
    }

    // Delete the element at the specified index from the vector of child elements.
    delete tmp;

    // Mark the element as dirty, so that it will be re-rendered on the next frame.
    Dirty.Dirty(STAIN_TYPE::DEEP | STAIN_TYPE::COLOR);

    return true;
}

/**
 * @brief Removes the element from the parent element.
 * @details This function first checks if the element has a parent.
 *          If the element has a parent, it calls the parent's Remove() function to remove the element from the parent.
 *          If the element does not have a parent, it prints an error message to the console.
 *          The function does not update the frame, so it is the caller's responsibility to update the frame after calling this function.
 */
void GGUI::Element::Remove(){
    if (Parent){
        // Tell the parent what is about to happen.
        // You need to update the parent before removing the child, otherwise the code cannot erase it when it is not found!
        Parent->Remove(this);
    }
    else{
        Report(
            std::string("Cannot remove ") + Get_Name() + std::string(", with no parent\n")
        );
    }
}

/**
 * @brief Set the width and height of the element.
 * @details This function sets the width and height of the element to the specified values.
 *          If the width or height is different from the current width or height, then the element will be resized and the STRETCH stain is set.
 *          The Update_Frame() function is also called to update the frame.
 * @param width The new width of the element.
 * @param height The new height of the element.
 */
void GGUI::Element::Set_Dimensions(unsigned int width, unsigned int height){
    if (width != Get_Width() || height != Get_Height()){
        Set_Width(width);
        Set_Height(height);
        //Fully_Stain();
        Dirty.Dirty(STAIN_TYPE::STRETCH);
        Update_Frame();
    }
}

/**
 * @brief Get the width of the element.
 * @details This function returns the width of the element.
 * @return The width of the element.
 */
unsigned int GGUI::Element::Get_Width(){
    return Style->Width.Get();
}

/**
 * @brief Get the height of the element.
 * @details This function returns the height of the element.
 * @return The height of the element.
 */
unsigned int GGUI::Element::Get_Height() {
    // Retrieve and return the height from the element's style
    return Style->Height.Get();
}

/**
 * @brief Set the width of the element.
 * @details This function sets the width of the element to the specified value.
 *          If the width is different from the current width, then the element will be resized and the STRETCH stain is set.
 *          The Update_Frame() function is also called to update the frame.
 * @param width The new width of the element.
 */
void GGUI::Element::Set_Width(unsigned int width){
    if (width != Get_Width()){
        Style->Width.Set(width);
        // Set the STRETCH stain if the width is changed
        Fully_Stain();
        // Update the frame after resizing
        Update_Frame();
    }
}

/**
 * @brief Set the height of the element.
 * @details This function sets the height of the element to the specified value.
 *          If the height is different from the current height, then the element will be resized and the STRETCH stain is set.
 *          The Update_Frame() function is also called to update the frame.
 * @param height The new height of the element.
 */
void GGUI::Element::Set_Height(unsigned int height){
    if (height != Get_Height()){
        Style->Height.Set(height);
        // Set the STRETCH stain if the height is changed
        Fully_Stain();
        // Update the frame after resizing
        Update_Frame();
    }
}

/**
 * @brief Set the position of the element.
 * @details This function sets the position of the element to the specified coordinates.
 *          If the position changes, the element will be marked as dirty for movement
 *          and the frame will be updated.
 * @param c The new position of the element.
 */
void GGUI::Element::Set_Position(IVector3 c) {
    // Update the element's position in the style
    Style->Position.Set(c);
    
    // Mark the element as dirty for movement updates
    this->Dirty.Dirty(STAIN_TYPE::MOVE);

    // Update the frame to reflect the position change
    Update_Frame();
}

/**
 * @brief Set the position of the element.
 * @details This function sets the position of the element to the specified coordinates.
 *          If the position changes, the element will be marked as dirty for movement
 *          and the frame will be updated.
 * @param c The new position of the element.
 */
void GGUI::Element::Set_Position(IVector3* c){
    if (c){
        // Set the position of the element to the specified coordinates
        Set_Position(*c);
    }
}

/**
 * @brief Get the position of the element.
 * @details This function retrieves the position of the element from its style.
 * @return The position of the element as an IVector3 object.
 */
GGUI::IVector3 GGUI::Element::Get_Position() {
    // Return the position of the element from the style
    return Style->Position.Get();
}

/**
 * @brief Get the absolute position of the element.
 * @details This function returns the cached absolute position of the element.
 *          The absolute position is the position of the element in the context of the entire document or window.
 * @return The absolute position of the element as an IVector3 object.
 */
GGUI::IVector3 GGUI::Element::Get_Absolute_Position() {
    // Return the cached absolute position of the element
    return Absolute_Position_Cache;
}

/**
 * @brief Update the absolute position cache of the element.
 * @details This function updates the cached absolute position of the element by adding the position of the element to the position of its parent.
 */
void GGUI::Element::Update_Absolute_Position_Cache(){
    Absolute_Position_Cache = {0, 0, 0};

    if (Parent){
        // Get the position of the parent
        Absolute_Position_Cache = Parent->Get_Position();
    }

    // Add the position of the element to the position of its parent
    Absolute_Position_Cache += Get_Position();
}

/**
 * @brief Set the margin of the element.
 * @details This function sets the margin of the element to the specified margin values.
 *          The margin is stored in the element's style.
 * @param margin The new margin values for the element.
 */
void GGUI::Element::Set_Margin(margin margin) {
    // Update the element's margin in the style
    Style->Margin = margin;
}

/**
 * @brief Get the margin of the element.
 * @details This function retrieves the margin of the element from its style.
 * @return The margin of the element as a GGUI::margin object.
 */
GGUI::margin GGUI::Element::Get_Margin(){
    return Style->Margin;
}

/**
 * Creates a deep copy of this Element, including all its children.
 * @return A new Element object that is a copy of this one.
 */
GGUI::Element* GGUI::Element::Copy(){
    // Compile time check
    //static_assert(std::is_same<T&, decltype(*this)>::value, "T must be the same as the type of the object");
    Element* new_element = Safe_Move();

    // Make sure the name is also renewed to represent the memory.
    new_element->Set_Name(std::to_string((unsigned long long)new_element));

    // Ptr related members:
    // - Parent
    // - Childs
    // - Style
    // - Event Handlers
    // - Focused_On Clearance
    // - Hovered_On Clearance

    // reset the parent info.
    new_element->Parent = nullptr;

    // copy the childs over.
    for (unsigned int i = 0; i < this->Style->Childs.size(); i++){
        new_element->Style->Childs[i] = this->Style->Childs[i]->Copy();
    }

    // copy the styles over.
    *new_element->Style = *this->Style;

    //now also update the event handlers.
    for (auto& e : Event_Handlers){

        if (e->Host == this){
            //copy the event and make a new one
            Action* new_action = new Action(*e);

            //update the host
            new_action->Host = new_element;

            //add the new action to the event handlers list
            Event_Handlers.push_back(new_action);
        }
    }

    // Clear the Focused on bool
    Focused = false;

    // Clear the Hovered on bool
    Hovered = false;

    return (Element*)new_element;
}

/**
 * @brief Get the fitting dimensions for the given child element.
 *
 * This function takes a child element and calculates the fitting dimensions for it.
 * The fitting dimensions are the width and height of the child element that does not exceed the
 * bounds of the parent element. If the child element is colliding with another child element
 * then the fitting dimensions are reduced to the point where the collision is resolved.
 *
 * @param child The child element for which the fitting dimensions are calculated.
 * @return A pair containing the width and height of the fitting dimensions.
 */
std::pair<unsigned int, unsigned int> GGUI::Element::Get_Fitting_Dimensions(Element* child){
    IVector3 Current_Position = child->Get_Position();

    unsigned int Result_Width = 0;
    unsigned int Result_Height = 0;

    int Border_Offset = (Has_Border() - child->Has_Border()) * Has_Border() * 2;

    /**
     * If there are only zero child or one and it is same as this child then give max.
     * This is because if there is no other child element, then the fitting dimensions
     * are the same as the parent element minus the border offset.
     */
    if (Style->Childs.size() == 0 || Style->Childs.back() == child){
        return {Get_Width() - Border_Offset, Get_Height() - Border_Offset};
    }

    /**
     * Start calculating the fitting dimensions from the top left corner of the child element.
     * This is done by increasing the width and height of the child element until it reaches the edge of the parent element.
     * If the child element collides with another child element, then the fitting dimensions are reduced to the point where the collision is resolved.
     */
    while (true){
        // If the width of the child element is still less than the width of this element minus the border offset
        if (Current_Position.X + (++Result_Width) < Get_Width() - Border_Offset){
            // Increase the width of the child element
            Result_Width++;
        }
        else{
            break;
        }
        
        // If the height of the child element is still less than the height of this element minus the border offset
        if (Current_Position.Y + (++Result_Height) < Get_Height() - Border_Offset){
            // Increase the height of the child element
            Result_Height++;
        }
        else{
            break;
        }
        
        // Check if the child element is colliding with any other child elements.
        for (auto c : Style->Childs) {
            // Use local positioning since this is a civil dispute :)
            if (child != c && Collides(c->Get_Position(), Current_Position, c->Get_Width(), c->Get_Height(), Result_Width, Result_Height)) {
                // If the child element is colliding with another child element, then we can stop here.
                return {Result_Width, Result_Height};
            }
        }
    }

    return {Result_Width, Result_Height};
}

/**
 * @brief Returns the maximum dimensions of the element without exceeding the parent element's dimensions.
 * @return A pair containing the maximum width and height of the element.
 */
std::pair<unsigned int, unsigned int> GGUI::Element::Get_Limit_Dimensions(){
    unsigned int max_width = 0;
    unsigned int max_height = 0;

    if (Parent){
        // If the element has a parent, then get the fitting dimensions from the parent.
        std::pair<unsigned int, unsigned int> Max_Dimensions = Parent->Get_Fitting_Dimensions(this);

        max_width = Max_Dimensions.first;
        max_height = Max_Dimensions.second;
    }
    else{
        // If the element does not have a parent, then get the maximum dimensions from the main window.
        if ((Element*)this == (Element*)GGUI::Main){
            // If the element is the main window, then get the maximum dimensions directly.
            max_width = Max_Width;
            max_height = Max_Height;
        }
        else{
            // If the element is not the main window, then get the maximum dimensions from the main window minus 2 for the border offset.
            max_width = GGUI::Main->Get_Width() - GGUI::Main->Has_Border() * 2;
            max_height = GGUI::Main->Get_Height() - GGUI::Main->Has_Border() * 2;
        }
    }

    return {max_width, max_height};
}

/**
 * @brief Sets the background color of the element.
 * 
 * This function sets the background color of the element to the specified RGB value. 
 * If the border background color is the same as the current background color, 
 * it updates the border background color as well. Marks the element as dirty for 
 * color updates and triggers a frame update.
 * 
 * @param color The RGB color to set as the background color.
 */
void GGUI::Element::Set_Background_Color(RGB color) {
    // Set the background color in the style
    Style->Background_Color = color;
    
    // If the border background color matches the current background color, update it
    if (Style->Border_Background_Color.Value.Get<RGB>() == Style->Background_Color.Value.Get<RGB>()) {
        Style->Border_Background_Color = color;
    }
    
    // Mark the element as dirty for color updates
    Dirty.Dirty(STAIN_TYPE::COLOR);
    
    // Update the frame to reflect the new color
    Update_Frame();
}

/**
 * @brief Retrieves the background color of the element.
 * 
 * This function returns the RGB value of the background color 
 * from the element's style.
 * 
 * @return The RGB color of the element's background.
 */
GGUI::RGB GGUI::Element::Get_Background_Color() {
    // Return the background color from the style
    return Style->Background_Color.Value.Get<RGB>();
}

/**
 * @brief Sets the border color of the element.
 * 
 * This function sets the border color of the element to the specified RGB value. Marks the element as dirty for color updates and triggers a frame update.
 * 
 * @param color The RGB color to set as the border color.
 */
void GGUI::Element::Set_Border_Color(RGB color){
    // Set the border color in the style
    Style->Border_Color = color;
    
    // Mark the element as dirty for color updates
    Dirty.Dirty(STAIN_TYPE::COLOR);
    
    // Update the frame to reflect the new color
    Update_Frame();
}

/**
 * @brief Retrieves the border color of the element.
 * 
 * This function returns the RGB value of the border color 
 * from the element's style.
 * 
 * @return The RGB color of the element's border.
 */
GGUI::RGB GGUI::Element::Get_Border_Color() {
    // Return the border color from the style
    return Style->Border_Color.Value.Get<RGB>();
}

/**
 * @brief Sets the border background color of the element.
 * 
 * This function sets the border background color of the element to the specified RGB value.
 * It marks the element as dirty for color updates and triggers a frame update.
 * 
 * @param color The RGB color to set as the border background color.
 */
void GGUI::Element::Set_Border_Background_Color(RGB color) {
    // Set the border background color in the style
    Style->Border_Background_Color = color;
    
    // Mark the element as dirty for color updates
    Dirty.Dirty(STAIN_TYPE::COLOR);
    
    // Update the frame to reflect the new color
    Update_Frame();
}

/**
 * @brief Retrieves the border background color of the element.
 * 
 * This function returns the RGB value of the border background color
 * from the element's style.
 * 
 * @return The RGB color of the element's border background.
 */
GGUI::RGB GGUI::Element::Get_Border_Background_Color() {
    // Return the border background color from the style
    return Style->Border_Background_Color.Value.Get<RGB>();
}

/**
 * @brief Sets the text color of the element.
 * 
 * This function sets the text color of the element to the specified RGB value. 
 * It marks the element as dirty for color updates and triggers a frame update.
 * 
 * @param color The RGB color to set as the text color.
 */
void GGUI::Element::Set_Text_Color(RGB color){
    Style->Text_Color = color;
    // Mark the element as dirty for color updates
    Dirty.Dirty(STAIN_TYPE::COLOR);
    // Update the frame to reflect the new color
    Update_Frame();
}

/**
 * @brief Sets whether the element is allowed to dynamically resize.
 * 
 * This function enables or disables the ability of the element to 
 * adjust its size based on its content.
 * 
 * @param True A boolean indicating whether dynamic resizing is allowed.
 */
void GGUI::Element::Allow_Dynamic_Size(bool True) {
    // Set the Allow_Dynamic_Size property in the element's style
    Style->Allow_Dynamic_Size = True; 
    // No need to update the frame, as this is used only on content change which triggers a frame update
}

/**
 * @brief Checks whether the element is allowed to dynamically resize.
 * 
 * This function checks the Allow_Dynamic_Size property in the element's style
 * and returns its value.
 * 
 * @return True if the element is allowed to dynamically resize, false otherwise.
 */
bool GGUI::Element::Is_Dynamic_Size_Allowed(){
    return Style->Allow_Dynamic_Size.Value;
}

/**
 * @brief Sets whether the element allows overflow.
 * 
 * This function enables or disables the overflow property of the element,
 * allowing child elements to exceed the parent's dimensions without resizing it.
 * 
 * @param True A boolean indicating whether overflow is allowed.
 */
void GGUI::Element::Allow_Overflow(bool True) {
    // Update the Allow_Overflow property in the element's style
    Style->Allow_Overflow = True; 
    // No need to update the frame, as this is used only on content change which triggers a frame update
}

/**
 * @brief Checks whether the element allows overflow.
 * 
 * This function checks the Allow_Overflow property in the element's style
 * and returns its value.
 * 
 * @return True if the element allows overflow, false otherwise.
 */
bool GGUI::Element::Is_Overflow_Allowed(){
    return Style->Allow_Overflow.Value;
}

/**
 * @brief Retrieves the text color of the element.
 * 
 * This function returns the RGB value of the text color
 * from the element's style.
 * 
 * @return The RGB color of the element's text.
 */
GGUI::RGB GGUI::Element::Get_Text_Color(){
    return Style->Text_Color.Value.Get<RGB>();
}

/**
 * @brief Sets the hover border color of the element.
 * 
 * This function sets the border color of the element when the mouse hovers over it
 * to the specified RGB value. Marks the element as dirty for color updates and
 * triggers a frame update.
 * 
 * @param color The RGB color to set as the hover border color.
 */
void GGUI::Element::Set_Hover_Border_Color(RGB color){
    Style->Hover_Border_Color = color;
    // Mark the element as dirty for color updates
    Dirty.Dirty(STAIN_TYPE::COLOR);
    // Update the frame to reflect the new color
    Update_Frame();
}

/**
 * @brief Retrieves the hover border color of the element.
 * 
 * This function returns the RGB value of the border color when the mouse hovers over the element
 * from the element's style.
 * 
 * @return The RGB color of the element's hover border.
 */
GGUI::RGB GGUI::Element::Get_Hover_Border_Color(){
    return Style->Hover_Border_Color.Value.Get<RGB>();
}

/**
 * @brief Sets the hover background color of the element.
 * 
 * This function sets the background color of the element when the mouse hovers over it
 * to the specified RGB value. Marks the element as dirty for color updates and triggers
 * a frame update.
 * 
 * @param color The RGB color to set as the hover background color.
 */
void GGUI::Element::Set_Hover_Background_Color(RGB color) {
    // Set the hover background color in the style
    Style->Hover_Background_Color = color;
    
    // Mark the element as dirty for color updates
    Dirty.Dirty(STAIN_TYPE::COLOR);
    
    // Update the frame to reflect the new color
    Update_Frame();
}

/**
 * @brief Retrieves the hover background color of the element.
 * 
 * This function returns the RGB value of the background color when the mouse hovers over the element
 * from the element's style.
 * 
 * @return The RGB color of the element's hover background.
 */
GGUI::RGB GGUI::Element::Get_Hover_Background_Color(){
    return Style->Hover_Background_Color.Value.Get<RGB>();
}

/**
 * @brief Sets the hover text color of the element.
 * 
 * This function sets the text color of the element when the mouse hovers over it
 * to the specified RGB value. Marks the element as dirty for color updates and triggers
 * a frame update.
 * 
 * @param color The RGB color to set as the hover text color.
 */
void GGUI::Element::Set_Hover_Text_Color(RGB color) {
    // Set the hover text color in the style
    Style->Hover_Text_Color = color;
    
    // Mark the element as dirty for color updates
    Dirty.Dirty(STAIN_TYPE::COLOR);
    
    // Update the frame to reflect the new color
    Update_Frame();
}

/**
 * @brief Retrieves the hover text color of the element.
 * 
 * This function returns the RGB value of the text color when the mouse hovers over the element
 * from the element's style.
 * 
 * @return The RGB color of the element's hover text.
 */
GGUI::RGB GGUI::Element::Get_Hover_Text_Color(){
    return Style->Hover_Text_Color.Value.Get<RGB>();
}

/**
 * @brief Sets the hover border background color of the element.
 * 
 * This function sets the background color of the element's border 
 * when the mouse hovers over it to the specified RGB value. It marks 
 * the element as dirty for color updates and triggers a frame update.
 * 
 * @param color The RGB color to set as the hover border background color.
 */
void GGUI::Element::Set_Hover_Border_Background_Color(RGB color) {
    // Set the hover border background color in the style
    Style->Hover_Border_Background_Color = color;
    
    // Mark the element as dirty for color updates
    Dirty.Dirty(STAIN_TYPE::COLOR);
    
    // Update the frame to reflect the new color
    Update_Frame();
}

/**
 * @brief Retrieves the hover border background color of the element.
 * 
 * This function returns the RGB value of the background color of the element's border
 * when the mouse hovers over it from the element's style.
 * 
 * @return The RGB color of the element's hover border background.
 */
GGUI::RGB GGUI::Element::Get_Hover_Border_Background_Color(){
    return Style->Hover_Border_Background_Color.Value.Get<RGB>();
}

/**
 * @brief Sets the focus border color of the element.
 * 
 * This function sets the color of the element's border when it is focused to the specified RGB value. It marks the element as dirty for color updates and triggers a frame update.
 * 
 * @param color The RGB color to set as the focus border color.
 */
void GGUI::Element::Set_Focus_Border_Color(RGB color){
    Style->Focus_Border_Color = color;
    
    // Mark the element as dirty for color updates
    Dirty.Dirty(STAIN_TYPE::COLOR);
    
    // Update the frame to reflect the new color
    Update_Frame();
}

/**
 * @brief Retrieves the focus border color of the element.
 * 
 * This function returns the RGB value of the border color when the element is focused
 * from the element's style.
 * 
 * @return The RGB color of the element's focus border.
 */
GGUI::RGB GGUI::Element::Get_Focus_Border_Color() {
    // Return the focus border color from the style
    return Style->Focus_Border_Color.Value.Get<RGB>();
}

/**
 * @brief Sets the focus background color of the element.
 * 
 * This function sets the background color of the element when it is focused to the specified RGB value. It marks the element as dirty for color updates and triggers a frame update.
 * 
 * @param color The RGB color to set as the focus background color.
 */
void GGUI::Element::Set_Focus_Background_Color(RGB color){
    // Set the focus background color in the style
    Style->Focus_Background_Color = color;
    
    // Mark the element as dirty for color updates
    Dirty.Dirty(STAIN_TYPE::COLOR);
    
    // Update the frame to reflect the new color
    Update_Frame();
}

/**
 * @brief Retrieves the focus background color of the element.
 * 
 * This function returns the RGB value of the background color when the element is focused
 * from the element's style.
 * 
 * @return The RGB color of the element's focus background.
 */
GGUI::RGB GGUI::Element::Get_Focus_Background_Color(){
    return Style->Focus_Background_Color.Value.Get<RGB>();
}

/**
 * @brief Sets the focus text color of the element.
 * 
 * This function sets the text color of the element when it is focused to the specified RGB value. It marks the element as dirty for color updates and triggers a frame update.
 * 
 * @param color The RGB color to set as the focus text color.
 */
void GGUI::Element::Set_Focus_Text_Color(RGB color){
    Style->Focus_Text_Color = color;
    // Mark the element as dirty for color updates
    Dirty.Dirty(STAIN_TYPE::COLOR);
    // Update the frame to reflect the new color
    Update_Frame();
}

/**
 * @brief Retrieves the focus text color of the element.
 * 
 * This function returns the RGB value of the text color when the element is focused
 * from the element's style.
 * 
 * @return The RGB color of the element's focus text.
 */
GGUI::RGB GGUI::Element::Get_Focus_Text_Color(){
    return Style->Focus_Text_Color.Value.Get<RGB>();
}

/**
 * @brief Sets the focus border background color of the element.
 * 
 * This function sets the focus border background color of the element to the specified RGB value.
 * It marks the element as dirty for color updates and triggers a frame update.
 * 
 * @param color The RGB color to set as the focus border background color.
 */
void GGUI::Element::Set_Focus_Border_Background_Color(RGB color){
    Style->Focus_Border_Background_Color = color;
    // Mark the element as dirty for color updates
    Dirty.Dirty(STAIN_TYPE::COLOR);
    // Update the frame to reflect the new color
    Update_Frame();
}

/**
 * @brief Retrieves the focus border background color of the element.
 * 
 * This function returns the RGB value of the focus border background color
 * from the element's style.
 * 
 * @return The RGB color of the element's focus border background.
 */
GGUI::RGB GGUI::Element::Get_Focus_Border_Background_Color(){
    return Style->Focus_Border_Background_Color.Value.Get<RGB>();
}

/**
 * @brief Sets the alignment of the element.
 * 
 * This function sets the alignment of the element to the specified ALIGN value.
 * 
 * @param Align The alignment value to set for the element.
 */
void GGUI::Element::Set_Align(GGUI::ALIGN Align){
    // Set the alignment in the style
    Style->Align = Align;
}

/**
 * @brief Sets the alignment of the element.
 * 
 * This function sets the alignment of the element to the specified ALIGN value.
 * 
 * @param Align The alignment value to set for the element.
 */
GGUI::ALIGN GGUI::Element::Get_Align(){
    return Style->Align.Value;
}

/**
 * @brief Sets the flow priority of the element.
 * 
 * This function sets the flow priority of the element to the specified DIRECTION value.
 * The flow priority determines how the element will be aligned in its parent when the parent is a flow layout.
 * 
 * @param Priority The flow priority value to set for the element.
 */
void GGUI::Element::Set_Flow_Priority(GGUI::DIRECTION Priority){
    Style->Flow_Priority = Priority;
}

/**
 * @brief Retrieves the flow priority of the element.
 * 
 * This function returns the DIRECTION value that was previously set with Set_Flow_Priority.
 * The flow priority determines how the element will be aligned in its parent when the parent is a flow layout.
 * 
 * @return The flow priority value of the element.
 */
GGUI::DIRECTION GGUI::Element::Get_Flow_Priority(){
    return Style->Flow_Priority.Value;
}

/**
 * @brief Sets whether the element will wrap its contents to the next line when it hits the edge of the screen.
 * 
 * This function sets whether the element will wrap its contents to the next line when it hits the edge of the screen.
 * If true, the element will wrap its contents to the next line when it hits the edge of the screen.
 * If false, the element will not wrap its contents to the next line when it hits the edge of the screen.
 * 
 * @param Wrap The value to set for whether the element will wrap its contents to the next line.
 */
void GGUI::Element::Set_Wrap(bool Wrap){
    Style->Wrap = Wrap;
}

/**
 * @brief Retrieves the wrap setting of the element.
 * 
 * This function returns whether the element will wrap its contents to the next line
 * when it reaches the edge of the screen.
 * 
 * @return True if the element will wrap its contents, false otherwise.
 */
bool GGUI::Element::Get_Wrap() {
    // Return the wrap value from the styling object
    return Style->Wrap.Value;
}

/**
 * @brief Recursively computes the size of the element based on its children.
 * 
 * This function will go through all the elements that are being displayed and
 * compute their size based on the size of their children. If the element has
 * children and the children have changed, then the element will be resized
 * to fit the children. If the element does not have children, then the function
 * will not do anything.
 * 
 * @note This function is called automatically by the framework when the
 *       elements are being rendered. It is not necessary to call this function
 *       manually.
 */
void GGUI::Element::Compute_Dynamic_Size(){
    // Go through all elements displayed.
    if (!Is_Displayed())
        return;

    if (Children_Changed()){
        // Iterate through all the elements that are being displayed.
        for (auto c : Style->Childs){
            if (!c->Is_Displayed())
                continue;

            // Check the child first if it has to stretch before this can even know if it needs to stretch.
            c->Compute_Dynamic_Size();

            int Border_Offset = (Has_Border() - c->Has_Border()) * Has_Border() * 2;

            // Add the border offset to the width and the height to count for the border collision and evade it. 
            unsigned int New_Width = (unsigned int)GGUI::Max(c->Style->Position.Get().X + (signed int)c->Get_Width() + Border_Offset, (signed int)Get_Width());
            unsigned int New_Height = (unsigned int)GGUI::Max(c->Style->Position.Get().Y + (signed int)c->Get_Height() + Border_Offset, (signed int)Get_Height());

            // but only update those who actually allow dynamic sizing.
            if (Style->Allow_Dynamic_Size.Value && (New_Width != Get_Width() || New_Height != Get_Height())){
                Set_Height(New_Height);
                Set_Width(New_Width);
                Dirty.Dirty(STAIN_TYPE::STRETCH);
            }
        }
    }

    return;
}

/**
 * @brief Renders the element and its children into the Render_Buffer nested buffer of the window.
 * @details This function processes the element to generate a vector of UTF objects representing the current state.
 * It handles different stains such as CLASS, STRETCH, COLOR, and EDGE to ensure the element is rendered correctly.
 * @return A vector of UTF objects representing the rendered element and its children.
 */
std::vector<GGUI::UTF>& GGUI::Element::Render(){
    std::vector<GGUI::UTF>& Result = Render_Buffer;

    //if inned children have changed without this changing, then this will trigger.
    if (Children_Changed() || Has_Transparent_Children()){
        Dirty.Dirty(STAIN_TYPE::DEEP);
    }

    Calculate_Childs_Hitboxes();    // Normally elements will NOT oder their content by hitbox system.

    Compute_Dynamic_Size();

    if (Dirty.is(STAIN_TYPE::CLEAN))
        return Result;

    if (Dirty.is(STAIN_TYPE::CLASS)){
        Parse_Classes();

        Dirty.Clean(STAIN_TYPE::CLASS);
    }

    if (Dirty.is(STAIN_TYPE::STRETCH)){
        // This needs to be called before the actual stretch, since the actual Width and Height have already been modified to the new state, and we need to make sure that is correct according to the percentile of the dynamic attributes that follow the parents diction.
        Style->Evaluate_Dynamic_Attribute_Values(this);

        Result.clear();
        Result.resize(Get_Width() * Get_Height(), SYMBOLS::EMPTY_UTF);
        Dirty.Clean(STAIN_TYPE::STRETCH);

        Dirty.Dirty(STAIN_TYPE::COLOR | STAIN_TYPE::EDGE | STAIN_TYPE::DEEP);
    }

    if (Dirty.is(STAIN_TYPE::MOVE)){
        Dirty.Clean(STAIN_TYPE::MOVE);
        
        Update_Absolute_Position_Cache();
    }

    //Apply the color system to the resized result list
    if (Dirty.is(STAIN_TYPE::COLOR))
        Apply_Colors(this, Result);

    bool Connect_Borders_With_Parent = Has_Border();
    unsigned int Childs_With_Borders = 0;

    //This will add the child windows to the Result buffer
    if (Dirty.is(STAIN_TYPE::DEEP)){
        Dirty.Clean(STAIN_TYPE::DEEP);

        for (auto c : this->Style->Childs){
            if (!c->Is_Displayed())
                continue;

            // check if the child is within the renderable borders.
            if (!Child_Is_Shown(c))
                continue;

            if (c->Has_Border())
                Childs_With_Borders++;

            std::vector<UTF>* tmp = &c->Render();

            if (c->Has_Postprocessing_To_Do())
                tmp = &c->Postprocess();

            Nest_Element(this, c, Result, *tmp);
        }
    }

    if (Childs_With_Borders > 0 && Connect_Borders_With_Parent)
        Dirty.Dirty(STAIN_TYPE::EDGE);

    //This will add the borders if necessary and the title of the window.
    if (Dirty.is(STAIN_TYPE::EDGE))
        Add_Overhead(this, Result);

    // This will calculate the connecting borders.
    if (Childs_With_Borders > 0){
        for (auto A : this->Style->Childs){
            for (auto B : this->Style->Childs){
                if (A == B)
                    continue;

                if (!A->Is_Displayed() || !A->Has_Border() || !B->Is_Displayed() || !B->Has_Border())
                    continue;

                Post_Process_Borders(A, B, Result);
            }

            Post_Process_Borders(this, A, Result);
        }
    }

    return Result;
}

/**
 * @brief Apply the color system to the rendered string.
 *
 * This function applies the color system set by the style to the rendered string.
 * It is called after the element has been rendered and the result is stored in the
 * Result vector.
 *
 * @param w The window to apply the color system to.
 * @param Result The vector containing the rendered string.
 */
void GGUI::Element::Apply_Colors(Element* w, std::vector<UTF>& Result){
    // Clean the color stain after applying the color system.
    Dirty.Clean(STAIN_TYPE::COLOR);

    // Loop over each UTF-8 character in the rendered string and set its color to the
    // color specified in the style.
    for (auto& utf : Result){
        utf.Set_Color(w->Compose_All_Text_RGB_Values());
    }
}

/**
 * @brief Add the border of the window to the rendered string.
 *
 * @param w The window to add the border for.
 * @param Result The string to add the border to.
 */
void GGUI::Element::Add_Overhead(GGUI::Element* w, std::vector<GGUI::UTF>& Result)
{
    Dirty.Clean(STAIN_TYPE::EDGE);

    if (!w->Has_Border())
        return;

    GGUI::styled_border custom_border = Style->Border_Style;

    for (unsigned int y = 0; y < Get_Height(); y++)
    {
        for (unsigned int x = 0; x < Get_Width(); x++)
        {
            //top left corner
            if (y == 0 && x == 0)
            {
                Result[y * Get_Width() + x] = GGUI::UTF(custom_border.TOP_LEFT_CORNER, w->Compose_All_Border_RGB_Values());
            }
            //top right corner
            else if (y == 0 && x == Get_Width() - 1)
            {
                Result[y * Get_Width() + x] = GGUI::UTF(custom_border.TOP_RIGHT_CORNER, w->Compose_All_Border_RGB_Values());
            }
            //bottom left corner
            else if (y == Get_Height() - 1 && x == 0)
            {
                Result[y * Get_Width() + x] = GGUI::UTF(custom_border.BOTTOM_LEFT_CORNER, w->Compose_All_Border_RGB_Values());
            }
            //bottom right corner
            else if (y == Get_Height() - 1 && x == Get_Width() - 1)
            {
                Result[y * Get_Width() + x] = GGUI::UTF(custom_border.BOTTOM_RIGHT_CORNER, w->Compose_All_Border_RGB_Values());
            }
            //The roof border
            else if (y == 0 || y == Get_Height() - 1)
            {
                Result[y * Get_Width() + x] = GGUI::UTF(custom_border.HORIZONTAL_LINE, w->Compose_All_Border_RGB_Values());
            }
            //The left border
            else if (x == 0 || x == Get_Width() - 1)
            {
                Result[y * Get_Width() + x] = GGUI::UTF(custom_border.VERTICAL_LINE, w->Compose_All_Border_RGB_Values());
            }
        }
    }
}

/**
 * @brief Compute the alpha blending of the source element to the destination element.
 * @details This function takes two UTF elements as arguments, the source element and the destination element.
 *          It calculates the alpha blending of the source element to the destination element, by adding the
 *          background color of the source element to the destination element, but only if the source element has
 *          a non-zero alpha value. If the source element has full opacity, then the destination gets fully rewritten
 *          over. If the source element has full transparency, then nothing is done.
 * @param Dest The destination element to which the source element will be blended.
 * @param Source The source element which will be blended to the destination element.
 */
void GGUI::Element::Compute_Alpha_To_Nesting(GGUI::UTF& Dest, GGUI::UTF Source){
    // If the Source element has full opacity, then the destination gets fully rewritten over.
    if (Source.Background.Alpha == UINT8_MAX){
        Dest = Source;
        return;
    }
    
    if (Source.Background.Alpha == std::numeric_limits<unsigned char>::min()) return;         // Dont need to do anything.

    // Color the Destination UTF by the Source UTF background color.
    Dest.Background += Source.Background;
    Dest.Foreground += Source.Background;

    // Check if source has text
    if (!Source.Has_Default_Text()){
        Dest.Set_Text(Source);

        // Set the text color right.
        if (!Dest.Has_Default_Text()){
            Dest.Foreground += Source.Foreground; 
        }
    }
}


/**
 * @brief Gets the fitting area for a child element in its parent.
 * @details This function calculates the area where the child element should be rendered within the parent element.
 *          It takes into account the border offsets of both the parent and the child element as well as their positions.
 *          The function returns a pair of pairs, where the first pair contains the negative offset of the child element from the parent element,
 *          the second pair contains the starting offset of the child element within the parent element and the third pair contains the ending offset of the child element within the parent element.
 * @param Parent The parent element.
 * @param Child The child element.
 * @return A pair of pairs containing the fitting area for the child element within the parent element.
 */
std::pair<std::pair<unsigned int, unsigned int> ,std::pair<std::pair<unsigned int, unsigned int>, std::pair<unsigned int, unsigned int>>> GGUI::Element::Get_Fitting_Area(GGUI::Element* Parent, GGUI::Element* Child){
    bool Border_Offset = (Parent->Has_Border() - Child->Has_Border()) && Parent->Has_Border();
    
    unsigned int Max_Allowed_Height = Parent->Get_Height() - Border_Offset;               //remove bottom borders from calculation
    unsigned int Max_Allowed_Width = Parent->Get_Width() - Border_Offset;                 //remove right borders from calculation

    unsigned int Min_Allowed_Height = 0 + Border_Offset;                            //add top borders from calculation
    unsigned int Min_Allowed_Width = 0 + Border_Offset;                             //add left borders from calculation

    unsigned int Child_Start_Y = Min_Allowed_Height + GGUI::Max(Child->Style->Position.Get().Y, 0);    // If the child is negatively positioned, then put it to zero and minimize the parent height.
    unsigned int Child_Start_X = Min_Allowed_Width + GGUI::Max(Child->Style->Position.Get().X, 0);    

    unsigned int Negative_Offset_X = abs(GGUI::Min(Child->Style->Position.Get().X, 0));
    unsigned int Negative_Offset_Y = abs(GGUI::Min(Child->Style->Position.Get().Y, 0));

    unsigned int Child_End_X = GGUI::Max(0, (int)(Child_Start_X + Child->Get_Processed_Width()) - (int)Negative_Offset_X);
    unsigned int Child_End_Y = GGUI::Max(0, (int)(Child_Start_Y + Child->Get_Processed_Height()) - (int)Negative_Offset_Y);

    Child_End_X = GGUI::Min(Max_Allowed_Width, Child_End_X);
    Child_End_Y = GGUI::Min(Max_Allowed_Height, Child_End_Y);

    // {Negative offset},                             {Child Starting offset},        {Child Ending offset}
    return {{Negative_Offset_X, Negative_Offset_Y}, {{Child_Start_X, Child_Start_Y}, {Child_End_X, Child_End_Y}} };
}

/**
 * @brief Nests a child element into a parent element.
 * @details This function calculates the area where the child element should be rendered within the parent element.
 *          It takes into account the border offsets of both the parent and the child element as well as their positions.
 *          The function then copies the contents of the child element's buffer into the parent element's buffer at the calculated position.
 * @param Parent The parent element.
 * @param Child The child element.
 * @param Parent_Buffer The parent element's buffer.
 * @param Child_Buffer The child element's buffer.
 */
void GGUI::Element::Nest_Element(GGUI::Element* Parent, GGUI::Element* Child, std::vector<GGUI::UTF>& Parent_Buffer, std::vector<GGUI::UTF>& Child_Buffer){
    std::pair<std::pair<unsigned int, unsigned int> ,std::pair<std::pair<unsigned int, unsigned int>, std::pair<unsigned int, unsigned int>>> Limits = Get_Fitting_Area(Parent, Child);

    // This is to combat child elements which are located halfway outside the parent area.
    unsigned int Negative_Offset_X = Limits.first.first;
    unsigned int Negative_Offset_Y = Limits.first.second;

    // Where the child starts to write in the parent buffer.
    unsigned int Start_X =  Limits.second.first.first;
    unsigned int Start_Y =  Limits.second.first.second;

    // Where the child ends it buffer rendering.
    unsigned int End_X = Limits.second.second.first;
    unsigned int End_Y = Limits.second.second.second;

    for (unsigned int y = Start_Y; y < End_Y; y++){
        for (unsigned int x = Start_X; x < End_X; x++){
            // Calculate the position of the child element in its own buffer.
            unsigned int Child_Buffer_Y = (y - Start_Y + Negative_Offset_Y) * Child->Get_Processed_Width();
            unsigned int Child_Buffer_X = (x - Start_X + Negative_Offset_X); 
            Compute_Alpha_To_Nesting(Parent_Buffer[y * Get_Width() + x], Child_Buffer[Child_Buffer_Y + Child_Buffer_X]);
        }
    }
}

inline bool Is_In_Bounds(GGUI::IVector3 index, GGUI::Element* parent){
    // checks if the index is out of bounds
    if (index.X < 0 || index.Y < 0 || index.X >= (signed)parent->Get_Width() || index.Y >= (signed)parent->Get_Height())
        return false;

    return true;
}

inline GGUI::UTF* From(GGUI::IVector3 index, std::vector<GGUI::UTF>& Parent_Buffer, GGUI::Element* Parent){
    return &Parent_Buffer[index.Y * Parent->Get_Width() + index.X];
}

/**
 * @brief Returns a map of the custom border symbols for the given element.
 * @param e The element to get the custom border map for.
 * @return A map of the custom border symbols where the key is the bit mask of the border and the value is the corresponding symbol.
 */
std::unordered_map<unsigned int, const char*> GGUI::Element::Get_Custom_Border_Map(GGUI::Element* e){
    GGUI::styled_border custom_border_style = e->Get_Border_Style();

    return Get_Custom_Border_Map(custom_border_style);
}

/**
 * @brief Returns a map of the custom border symbols for the given border style.
 * The map key is the bit mask of the border and the value is the corresponding symbol.
 * @param custom_border_style The custom border style to get the map for.
 * @return A map of the custom border symbols.
 */
std::unordered_map<unsigned int, const char*> GGUI::Element::Get_Custom_Border_Map(GGUI::styled_border custom_border_style){
    return {
            // corners
            {GGUI::SYMBOLS::CONNECTS_DOWN | GGUI::SYMBOLS::CONNECTS_RIGHT, custom_border_style.TOP_LEFT_CORNER},
            {GGUI::SYMBOLS::CONNECTS_DOWN | GGUI::SYMBOLS::CONNECTS_LEFT, custom_border_style.TOP_RIGHT_CORNER},
            {GGUI::SYMBOLS::CONNECTS_UP | GGUI::SYMBOLS::CONNECTS_RIGHT, custom_border_style.BOTTOM_LEFT_CORNER},
            {GGUI::SYMBOLS::CONNECTS_UP | GGUI::SYMBOLS::CONNECTS_LEFT, custom_border_style.BOTTOM_RIGHT_CORNER},

            // vertical lines
            {GGUI::SYMBOLS::CONNECTS_DOWN | GGUI::SYMBOLS::CONNECTS_UP, custom_border_style.VERTICAL_LINE},

            // horizontal lines
            {GGUI::SYMBOLS::CONNECTS_LEFT | GGUI::SYMBOLS::CONNECTS_RIGHT, custom_border_style.HORIZONTAL_LINE},

            // connectors
            {GGUI::SYMBOLS::CONNECTS_DOWN | GGUI::SYMBOLS::CONNECTS_UP | GGUI::SYMBOLS::CONNECTS_RIGHT, custom_border_style.VERTICAL_RIGHT_CONNECTOR},
            {GGUI::SYMBOLS::CONNECTS_DOWN | GGUI::SYMBOLS::CONNECTS_UP | GGUI::SYMBOLS::CONNECTS_LEFT, custom_border_style.VERTICAL_LEFT_CONNECTOR},

            {GGUI::SYMBOLS::CONNECTS_LEFT | GGUI::SYMBOLS::CONNECTS_RIGHT | GGUI::SYMBOLS::CONNECTS_DOWN, custom_border_style.HORIZONTAL_BOTTOM_CONNECTOR},
            {GGUI::SYMBOLS::CONNECTS_LEFT | GGUI::SYMBOLS::CONNECTS_RIGHT | GGUI::SYMBOLS::CONNECTS_UP, custom_border_style.HORIZONTAL_TOP_CONNECTOR},

            // cross connectors
            {GGUI::SYMBOLS::CONNECTS_LEFT | GGUI::SYMBOLS::CONNECTS_RIGHT | GGUI::SYMBOLS::CONNECTS_UP | GGUI::SYMBOLS::CONNECTS_DOWN, custom_border_style.CROSS_CONNECTOR}
        };
}

/**
 * @brief Sets the custom border style for the element.
 * @details This function sets the custom border style for the element, marks the element's edges as dirty, and ensures that the border is visible.
 * @param style The custom border style to set.
 */
void GGUI::Element::Set_Custom_Border_Style(GGUI::styled_border style) {
    // Set the border style of the element
    Style->Border_Style = style;
    
    // Mark the border as needing an update
    Dirty.Dirty(STAIN_TYPE::EDGE);

    // Ensure the border is visible
    Show_Border(true);
}

/**
 * @brief Gets the custom border style of the element.
 * @return The custom border style of the element.
 */
GGUI::styled_border GGUI::Element::Get_Custom_Border_Style(){
    return Style->Border_Style;
}

/**
 * @brief Posts a process that handles the intersection of borders between two elements and their parent.
 * @details This function posts a process that handles the intersection of borders between two elements and their parent.
 *          The process calculates the intersection points of the borders and then constructs a bit mask that portraits the connections the middle point has.
 *          With the calculated bit mask it can fetch from the 'SYMBOLS::Border_Identifiers' the right border string.
 * @param A The first element.
 * @param B The second element.
 * @param Parent_Buffer The buffer of the parent element.
 */
void GGUI::Element::Post_Process_Borders(Element* A, Element* B, std::vector<UTF>& Parent_Buffer){
    // We only need to calculate the childs points in which they intersect with the parent borders.
    // At these intersecting points of border we will construct a bit mask that portraits the connections the middle point has.
    // With the calculated bit mask we can fetch from the 'SYMBOLS::Border_Identifiers' the right border string.

    // First calculate if the childs borders even touch the parents borders.
    // If not, there is no need to calculate anything.

    // First calculate if the child is outside the parent.
    if (
        B->Style->Position.Get().X + (signed)B->Get_Width() < A->Style->Position.Get().X ||
        B->Style->Position.Get().X > A->Style->Position.Get().X + (signed)A->Get_Width() ||
        B->Style->Position.Get().Y + (signed)B->Get_Height() < A->Style->Position.Get().Y ||
        B->Style->Position.Get().Y > A->Style->Position.Get().Y + (signed)A->Get_Height()
    )
        return;

    // Now calculate if the child is inside the parent.
    if (
        B->Style->Position.Get().X > A->Style->Position.Get().X &&
        B->Style->Position.Get().X + (signed)B->Get_Width() < A->Style->Position.Get().X + (signed)A->Get_Width() &&
        B->Style->Position.Get().Y > A->Style->Position.Get().Y &&
        B->Style->Position.Get().Y + (signed)B->Get_Height() < A->Style->Position.Get().Y + (signed)A->Get_Height()
    )
        return;

    // Now that we are here it means the both boxes interlace each other.
    // We will calculate the hitting points by drawing segments from corner to corner and then comparing one segments x to other segments y, and so forth.

    // two nested loops rotating the x and y usages.
    // store the line x,y into a array for the nested loops to access.
    std::vector<int> Vertical_Line_X_Coordinates = {
        
        B->Style->Position.Get().X,
        A->Style->Position.Get().X,
        B->Style->Position.Get().X + (int)B->Get_Width() - 1,
        A->Style->Position.Get().X + (int)A->Get_Width() - 1,

                
        // A->Style->Position.Get().X,
        // B->Style->Position.Get().X,
        // A->Style->Position.Get().X + A->Width - 1,
        // B->Style->Position.Get().X + B->Width - 1

    };

    std::vector<int> Horizontal_Line_Y_Coordinates = {
        
        A->Style->Position.Get().Y,
        B->Style->Position.Get().Y + (int)B->Get_Height() - 1,
        A->Style->Position.Get().Y,
        B->Style->Position.Get().Y + (int)B->Get_Height() - 1,

        // B->Position.Y,
        // A->Position.Y + A->Height - 1,
        // B->Position.Y,
        // A->Position.Y + A->Height - 1,

    };

    std::vector<IVector3> Crossing_Indicies;

    // Go through singular box
    for (unsigned int Box_Index = 0; Box_Index < Horizontal_Line_Y_Coordinates.size(); Box_Index++){
        // Now just pair the indicies from the two lists.
        Crossing_Indicies.push_back(
            // First pair
            IVector3(
                Vertical_Line_X_Coordinates[Box_Index],
                Horizontal_Line_Y_Coordinates[Box_Index]
            )
        );
    }

    std::unordered_map<unsigned int, const char*> custom_border = Get_Custom_Border_Map(A);

    // Now that we have the crossing points we can start analyzing the ways they connect to construct the bit masks.
    for (auto c : Crossing_Indicies){

        IVector3 Above = { c.X, Max((signed)c.Y - 1, 0) };
        IVector3 Below = { c.X, c.Y + 1 };
        IVector3 Left = { Max((signed)c.X - 1, 0), c.Y };
        IVector3 Right = { c.X + 1, c.Y };

        unsigned int Current_Masks = 0;

        // These selected coordinates can only contain something related to the borders and if the current UTF is unicode then it is an border.
        if (Is_In_Bounds(Above, this) && (
            From(Above, Parent_Buffer, this)->Unicode == A->Get_Custom_Border_Style().VERTICAL_LINE ||
            From(Above, Parent_Buffer, this)->Unicode == B->Get_Custom_Border_Style().VERTICAL_LINE
        ))
            Current_Masks |= SYMBOLS::CONNECTS_UP;

        if (Is_In_Bounds(Below, this) && (
            From(Below, Parent_Buffer, this)->Unicode == A->Get_Custom_Border_Style().VERTICAL_LINE ||
            From(Below, Parent_Buffer, this)->Unicode == B->Get_Custom_Border_Style().VERTICAL_LINE
        ))
            Current_Masks |= SYMBOLS::CONNECTS_DOWN;

        if (Is_In_Bounds(Left, this) && (
            From(Left, Parent_Buffer, this)->Unicode == A->Get_Custom_Border_Style().HORIZONTAL_LINE ||
            From(Left, Parent_Buffer, this)->Unicode == B->Get_Custom_Border_Style().HORIZONTAL_LINE
        ))
            Current_Masks |= SYMBOLS::CONNECTS_LEFT;

        if (Is_In_Bounds(Right, this) && (
            From(Right, Parent_Buffer, this)->Unicode == A->Get_Custom_Border_Style().HORIZONTAL_LINE ||
            From(Right, Parent_Buffer, this)->Unicode == B->Get_Custom_Border_Style().HORIZONTAL_LINE
        ))
            Current_Masks |= SYMBOLS::CONNECTS_RIGHT;

        if (custom_border.find(Current_Masks) == custom_border.end())
            continue;

        From(c, Parent_Buffer, this)->Set_Text(custom_border[Current_Masks]);
    }
}

//End of utility functions.

/**
 * @brief A function that registers a lambda to be executed when the element is clicked.
 * @details The lambda is given a pointer to the Event object that triggered the call.
 *          The lambda is expected to return true if it was successful and false if it failed.
 * @param action The lambda to be called when the element is clicked.
 */
void GGUI::Element::On_Click(std::function<bool(GGUI::Event*)> action){
    Action* a = new Action(
        Constants::MOUSE_LEFT_CLICKED,
        [this, action](GGUI::Event* e){
            // If the element is under the mouse cursor when it is clicked
            if (Collides(this, Mouse)){
                // Construct an Action from the Event obj
                GGUI::Action* wrapper = new GGUI::Action(e->Criteria, action, this);

                // Call the lambda with the wrapper
                action(wrapper);

                //action successfully executed.
                return true;
            }
            //action failed.
            return false;
        },
        this
    );
    GGUI::Event_Handlers.push_back(a);
}

/**
 * @brief A function that registers a lambda to be executed when the element is interacted with in any way.
 * @details The lambda is given a pointer to the Event object that triggered the call.
 *          The lambda is expected to return true if it was successful and false if it failed.
 * @param criteria The criteria to check for when deciding whether to execute the lambda.
 * @param action The lambda to be called when the element is interacted with.
 * @param GLOBAL Whether the lambda should be executed even if the element is not under the mouse.
 */
void GGUI::Element::On(unsigned long long criteria, std::function<bool(GGUI::Event*)> action, bool GLOBAL){
    Action* a = new Action(
        criteria,
        [this, action, GLOBAL](GGUI::Event* e){
            if (Collides(this, Mouse) || GLOBAL){
                // action successfully executed.
                return action(e);
            }
            // action failed.
            return false;
        },
        this
    );
    GGUI::Event_Handlers.push_back(a);
}

/**
 * @brief Check if any children have changed.
 * @details This function will check if any of the children have changed, this is used to determine if the element needs to be re-drawn.
 * @return true if any children have changed, false otherwise.
 */
bool GGUI::Element::Children_Changed(){
    // This is used if an element is recently hidden so the DEEP search wouldn't find it if not for this. 
    // Clean the state changed elements already here.
    if (Dirty.is(STAIN_TYPE::STATE)){
        Dirty.Clean(STAIN_TYPE::STATE);
        return true;
    }

    // Not counting State machine, if element is not being drawn return always false.
    if (!Show)
        return false;

    // If the element is dirty.
    if (Dirty.Type != STAIN_TYPE::CLEAN){
        return true;
    }

    // recursion
    for (auto e : Style->Childs){
        if (e->Children_Changed())
            return true;
    }

    return false;
}

/**
 * @brief Check if there are any transparent children.
 * @details This function determines if the current element or any of its children
 *          are transparent and require redrawing.
 * @return True if any child is transparent and not clean; otherwise, false.
 */
bool GGUI::Element::Has_Transparent_Children() {
    // If the element is not visible, return false.
    if (!Show)
        return false;

    // Check if the current element is transparent and not clean.
    if (Is_Transparent() && Dirty.Type != STAIN_TYPE::CLEAN)
        return true;

    // Recursively check each child element for transparency.
    for (auto e : Style->Childs) {
        if (e->Has_Transparent_Children())
            return true;
    }

    // No transparent children found.
    return false;
}

/**
 * @brief Set the name of the element.
 * @details This function sets the name of the element and stores it in the global Element_Names map.
 * @param name The name of the element.
 */
void GGUI::Element::Set_Name(std::string name){
    // Set the name of the element.
    Name = name;

    // Store the element in the global Element_Names map.
    Element_Names[name] = this;
}

/**
 * @brief Retrieves an element by name.
 * @details This function takes a string argument representing the name of the element
 *          and returns a pointer to the element if it exists in the global Element_Names map.
 * @param name The name of the element to retrieve.
 * @return A pointer to the element if it exists; otherwise, nullptr.
 */
GGUI::Element* GGUI::Element::Get_Element(std::string name){
    Element* Result = nullptr;

    // Check if the element is in the global Element_Names map.
    if (Element_Names.find(name) != Element_Names.end()){
        // If the element exists, assign it to the result.
        Result = Element_Names[name];
    }

    // Return the result.
    return Result;
}

/**
 * @brief Reorders child elements based on their z-position.
 * @details This function sorts the child elements of the current element by their z-coordinate
 *          in ascending order, so that elements with a higher z-coordinate appear later in the list.
 */
void GGUI::Element::Re_Order_Childs() {
    // Sort the child elements using a lambda function to compare the z-coordinates.
    std::sort(Style->Childs.begin(), Style->Childs.end(), [](Element* a, Element* b) {
        // Compare the z-position of the two elements.
        return a->Get_Position().Z < b->Get_Position().Z;
    });
}

/**
 * @brief Focuses the element.
 * @details This function updates the global focus information by setting the mouse position to the element's position and updating the focused element.
 */
void GGUI::Element::Focus() {
    // Set the mouse position to the element's position.
    GGUI::Mouse = this->Style->Position.Get();
    // Update the focused element.
    GGUI::Update_Focused_Element(this);
}

/**
 * @brief Adds a handler function to the state handlers map.
 * @details This function takes a state and a handler function as arguments.
 *          The handler function is stored in the State_Handlers map with the given state as the key.
 * @param s The state for which the handler should be executed.
 * @param job The handler function to be executed when the given state is triggered.
 */
void GGUI::Element::On_State(State s, std::function<void()> job){
    State_Handlers[s] = job;
}

bool Is_Signed(int x){
    return x < 0;
}

int Get_Sign(int x){
    return Is_Signed(x) ? -1 : 1;
}

// Constructs two squares one 2 steps larger on width and height, and given the different indicies.
std::vector<GGUI::IVector3> Get_Surrounding_Indicies(int Width, int Height, GGUI::IVector3 start_offset, GGUI::FVector2 Offset){

    std::vector<GGUI::IVector3> Result;

    // First construct the first square.
    int Bigger_Square_Start_X = start_offset.X - 1;
    int Bigger_Square_Start_Y = start_offset.Y - 1;

    int Bigger_Square_End_X = start_offset.X + Width + 1;
    int Bigger_Square_End_Y = start_offset.Y + Height + 1;

    int Smaller_Square_Start_X = start_offset.X + (Offset.X * GGUI::Min(0, (int)Offset.X));
    int Smaller_Square_Start_Y = start_offset.Y + (Offset.Y * GGUI::Min(0, (int)Offset.Y));

    int Smaller_Square_End_X = start_offset.X + Width - (Offset.X * GGUI::Max(0, (int)Offset.X));
    int Smaller_Square_End_Y = start_offset.Y + Height - (Offset.Y * GGUI::Max(0, (int)Offset.Y));

    for (int y = Bigger_Square_Start_Y; y < Bigger_Square_End_Y; y++){
        for (int x = Bigger_Square_Start_X; x < Bigger_Square_End_X; x++){

            bool Is_Inside_Smaller_Square = x >= Smaller_Square_Start_X && x < Smaller_Square_End_X && y >= Smaller_Square_Start_Y && y < Smaller_Square_End_Y;

            // Check if the current coordinates are outside the smaller square.
            if (!Is_Inside_Smaller_Square)
                Result.push_back({ x, y });
        }
    }

    return Result;

}

/**
 * @brief Checks if the element needs postprocessing.
 * @details This function checks if the element needs postprocessing by checking if the element has a shadow or is transparent.
 * @return True if the element needs postprocessing; otherwise, false.
 */
bool GGUI::Element::Has_Postprocessing_To_Do() {
    // Check if the element has a shadow that needs to be processed.
    bool Has_Shadow_Processing = Style->Shadow.Enabled;

    // Check if the element is transparent and needs to be processed.
    bool Has_Opacity_Processing = Is_Transparent();

    // Return true if the element needs postprocessing; otherwise, false.
    return Has_Shadow_Processing || Has_Opacity_Processing;
}

/**
 * @brief Process the shadow of the element.
 * @details This function processes the shadow of the element by calculating the new buffer size and creating a new buffer with the shadow.
 *          It then offsets the shadow box buffer by the direction and blends it with the original buffer.
 * @param Current_Buffer The buffer to be processed.
 */
void GGUI::Element::Process_Shadow(std::vector<GGUI::UTF>& Current_Buffer){
    if (!Style->Shadow.Enabled)
        return;

    shadow& properties = Style->Shadow;

    // First calculate the new buffer size.
    // This is going to be the new two squares overlapping minus buffer.

    // Calculate the zero origin when the equation is: -properties.Direction.Z * X + properties.Opacity = 0
    // -a * x + o = 0
    // x = o / a

    int Shadow_Length = properties.Direction.Get<FVector3>().Z * properties.Opacity;

    unsigned int Shadow_Box_Width = Get_Width() + (Shadow_Length * 2);
    unsigned int Shadow_Box_Height = Get_Height() + (Shadow_Length * 2);

    std::vector<GGUI::UTF> Shadow_Box;
    Shadow_Box.resize(Shadow_Box_Width * Shadow_Box_Height);

    unsigned char Current_Alpha = properties.Opacity * UINT8_MAX;;
    float previous_opacity = properties.Opacity;
    int Current_Box_Start_X = Shadow_Length;
    int Current_Box_Start_Y = Shadow_Length;

    int Current_Shadow_Width = Get_Width();
    int Current_Shadow_Height = Get_Height();

    for (int i = 0; i < Shadow_Length; i++){
        std::vector<IVector3> Shadow_Indicies = Get_Surrounding_Indicies(
            Current_Shadow_Width,
            Current_Shadow_Height,
            { 
                Current_Box_Start_X--,
                Current_Box_Start_Y--
            },
            properties.Direction.Get<FVector3>()
        );

        Current_Shadow_Width += 2;
        Current_Shadow_Height += 2;

        UTF shadow_pixel;
        shadow_pixel.Background = properties.Color.Get<RGB>();
        shadow_pixel.Background.Alpha = Current_Alpha;

        for (auto& index : Shadow_Indicies){
            Shadow_Box[index.Y * Shadow_Box_Width + index.X] = shadow_pixel;
        }

        previous_opacity *= GGUI::Min(0.9f, (float)properties.Direction.Get<FVector3>().Z);
        Current_Alpha = previous_opacity * UINT8_MAX;;
    }

    // Now offset the shadow box buffer by the direction.
    int Offset_Box_Width = Shadow_Box_Width + abs((int)properties.Direction.Get<FVector3>().X);
    int Offset_Box_Height = Shadow_Box_Height + abs((int)properties.Direction.Get<FVector3>().Y);

    std::vector<GGUI::UTF> Swapped_Buffer = Current_Buffer;

    Current_Buffer.resize(Offset_Box_Width * Offset_Box_Height);

    IVector3 Shadow_Box_Start = {
        GGUI::Max(0, (int)properties.Direction.Get<FVector3>().X),
        GGUI::Max(0, (int)properties.Direction.Get<FVector3>().Y)
    };

    IVector3 Original_Box_Start = {
        Shadow_Box_Start.X - properties.Direction.Get<FVector3>().X + Shadow_Length,
        Shadow_Box_Start.Y - properties.Direction.Get<FVector3>().Y + Shadow_Length
    };

    IVector3 Original_Box_End = {
        Original_Box_Start.X + Get_Width(),
        Original_Box_Start.Y + Get_Height()
    };

    IVector3 Shadow_Box_End = {
        Shadow_Box_Start.X + Shadow_Box_Width,
        Shadow_Box_Start.Y + Shadow_Box_Height
    };

    // Start mixing the shadow box and the original box buffers.
    unsigned int Original_Buffer_Index = 0;
    unsigned int Shadow_Buffer_Index = 0;
    unsigned int Final_Index = 0;

    for (int Raw_Y = 0; Raw_Y < Offset_Box_Height; Raw_Y++){
        for (int Raw_X = 0; Raw_X < Offset_Box_Width; Raw_X++){

            bool Is_Inside_Original_Area = Raw_X >= Original_Box_Start.X &&
                Raw_X < Original_Box_End.X &&
                Raw_Y >= Original_Box_Start.Y &&
                Raw_Y < Original_Box_End.Y;


            bool Is_Inside_Shadow_Box = Raw_X >= Shadow_Box_Start.X &&
                Raw_X < Shadow_Box_End.X &&
                Raw_Y >= Shadow_Box_Start.Y &&
                Raw_Y < Shadow_Box_End.Y;

            if (Is_Inside_Original_Area){
                Current_Buffer[Final_Index++] = Swapped_Buffer[Original_Buffer_Index++];
            }
            else if (Is_Inside_Shadow_Box) {
                Current_Buffer[Final_Index++] = Shadow_Box[Original_Buffer_Index + Shadow_Buffer_Index++];
            }
        }
    }

    Post_Process_Width = Offset_Box_Width;
    Post_Process_Height = Offset_Box_Height;
}

/**
 * @brief Applies the opacity of the element to the given buffer.
 * @details This function will iterate over the given buffer and apply the opacity of the element to the background and foreground of each UTF character.
 * @param Current_Buffer The buffer to be processed.
 */
void GGUI::Element::Process_Opacity(std::vector<GGUI::UTF>& Current_Buffer)
{
    if (!Is_Transparent())
        return;

    // Get the current opacity value of the element.
    float fast_opacity = Style->Opacity.Get();

    // Iterate over each character in the buffer.
    for (unsigned int Y = 0; Y < Get_Processed_Height(); Y++)
    {
        for (unsigned int X = 0; X < Get_Processed_Width(); X++)
        {
            // Get the current UTF character.
            UTF& tmp = Current_Buffer[Y * Get_Processed_Width() + X];

            // Apply the opacity to the background and foreground of the character.
            tmp.Background.Alpha = tmp.Background.Alpha * fast_opacity;
            tmp.Foreground.Alpha = tmp.Foreground.Alpha * fast_opacity;
        }
    }
}

/**
 * @brief
 * This function performs postprocessing on the rendered buffer of the element.
 * It applies the shadow, and then the opacity to the rendered buffer.
 * @return The postprocessed buffer.
 */
std::vector<GGUI::UTF>& GGUI::Element::Postprocess(){
    // Save the rendered buffer to the postprocessed buffer.
    Post_Process_Buffer = Render_Buffer;

    // Process the shadow of the element.
    Process_Shadow(Post_Process_Buffer);

    // Process the opacity of the element.
    Process_Opacity(Post_Process_Buffer);

    // Return the postprocessed buffer.
    return Post_Process_Buffer;
}

/**
 * @brief
 * This function determines if the given element is a direct child of this element (in the DOM tree),
 * and if it is visible on the screen (does not go out of bounds of the parent element).
 * @param other Child element to check
 * @return True if the child element is visible within the bounds of the parent.
 */
bool GGUI::Element::Child_Is_Shown(Element* other){

    // Check if the element has a border
    bool Border_Modifier = (Has_Border() - other->Has_Border()) && Has_Border();

    // Calculate the minimum and maximum coordinates of the child element
    int Minimum_X = other->Style->Position.Get().X + other->Get_Processed_Width();
    int Minimum_Y = other->Style->Position.Get().Y + other->Get_Processed_Height();
    int Maximum_X = other->Style->Position.Get().X - (other->Get_Width() - other->Get_Processed_Width());
    int Maximum_Y = other->Style->Position.Get().Y - (other->Get_Height() - other->Get_Processed_Height());

    // Check if the child element is visible within the bounds of the parent
    bool X_Is_Inside = Minimum_X >= Border_Modifier && Maximum_X < (signed)Get_Width() - Border_Modifier;
    bool Y_Is_Inside = Minimum_Y >= Border_Modifier && Maximum_Y < (signed)Get_Height() - Border_Modifier;

    // Return true if the child element is visible within the bounds of the parent
    return X_Is_Inside && Y_Is_Inside;
}

// UTILS : -_-_-_-_-_-_-_-_-_

GGUI::Element* Translate_Element(GGUI::HTML_Node* input){
    GGUI::Element* Result = new GGUI::Element();

    // Parse the following information given by the HTML_NODE:
    // - Childs Recursive Nesting
    // |-> Parent Linking
    // - Position written inheriting
    // - RAW ptr set to get link to origin  (no need to do anything)
    // - Type (no need to do anything)
    // - Attribute parsing: Styles, Width, Height, BG_Color, Front_Color, Border, Border color, etc.. (All CSS attributes)

    std::string Name = "";

    GGUI::Translate_Childs_To_Element(Result, input, &Name);

    GGUI::Translate_Attributes_To_Element(Result, input);

    return Result;
}

GGUI_Add_Translator("element", Translate_Element);
GGUI_Add_Translator("div", Translate_Element);
GGUI_Add_Translator("li", Translate_Element);