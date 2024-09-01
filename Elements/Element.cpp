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


std::string GGUI::RGB::Get_Colour() const{
    return Constants::To_String[Red] + Constants::ANSI::SEPARATE + Constants::To_String[Green] + Constants::ANSI::SEPARATE + Constants::To_String[Blue];
}

void GGUI::RGB::Get_Colour_As_Super_String(Super_String* Result) const{
    Result->Add(Constants::To_Compact[Red]);
    Result->Add(Constants::ANSI::SEPARATE);
    Result->Add(Constants::To_Compact[Green]);
    Result->Add(Constants::ANSI::SEPARATE);
    Result->Add(Constants::To_Compact[Blue]);
}
    
GGUI::BORDER_STYLE_VALUE::BORDER_STYLE_VALUE(std::vector<const char*> values, VALUE_STATE Default) : Style_Base(Default){
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
        Report_Stack("Internal error: Border style value has wrong number of values. Expected 9 got: '" + std::to_string(values.size()) + "'");
    }
}

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

std::string GGUI::UTF::To_String(){
    std::string Result =
        Foreground.Get_Over_Head(true) + Foreground.Get_Colour() + Constants::ANSI::END_COMMAND + 
        Background.Get_Over_Head(false) + Background.Get_Colour() + Constants::ANSI::END_COMMAND;

    if(Is(UTF_FLAG::IS_UNICODE)){
        // Add the const char* to the Result
        Result.append(Unicode, Unicode_Length);
    }
    else{
        Result += Ascii;
    }

    return Result + Constants::ANSI::RESET_COLOR;
}

void GGUI::UTF::To_Super_String(GGUI::Super_String* Result, Super_String* Text_Overhead, Super_String* Background_Overhead, Super_String* Text_Colour, Super_String* Background_Colour){
    Foreground.Get_Over_Head_As_Super_String(Text_Overhead, true);
    Foreground.Get_Colour_As_Super_String(Text_Colour);
    Background.Get_Over_Head_As_Super_String(Background_Overhead, false);
    Background.Get_Colour_As_Super_String(Background_Colour);

    Result->Add(Text_Overhead, true);
    Result->Add(Text_Colour, true);
    Result->Add(Constants::ANSI::END_COMMAND);
    Result->Add(Background_Overhead, true);
    Result->Add(Background_Colour, true);

    if (Is(UTF_FLAG::IS_UNICODE)){
        Result->Add(Unicode, Unicode_Length);
    }
    else{
        Result->Add(Ascii);
    }

    Result->Add(Constants::ANSI::RESET_COLOR);
}

std::string GGUI::UTF::To_Encoded_String() {
    std::string Result;
    
    if (Is(UTF_FLAG::ENCODE_START))
        Result = Foreground.Get_Over_Head(true) + Foreground.Get_Colour() + Constants::ANSI::END_COMMAND 
               + Background.Get_Over_Head(false) + Background.Get_Colour() + Constants::ANSI::END_COMMAND;

    if(Is(UTF_FLAG::IS_UNICODE)){
        // Add the const char* to the Result
        Result.append(Unicode, Unicode_Length);
    }
    else{
        Result += Ascii;
    }

    if (Is(UTF_FLAG::ENCODE_END))
        Result += Constants::ANSI::RESET_COLOR;

    return Result;
}

void GGUI::UTF::To_Encoded_Super_String(Super_String* Result, Super_String* Text_Overhead, Super_String* Background_Overhead, Super_String* Text_Colour, Super_String* Background_Colour) {

    if (Is(UTF_FLAG::ENCODE_START)){
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

    if (Is(UTF_FLAG::IS_UNICODE)){
        Result->Add(Unicode, Unicode_Length);
    }
    else{
        Result->Add(Ascii);
    }

    if (Is(UTF_FLAG::ENCODE_END)){
        Result->Add(Constants::ANSI::RESET_COLOR);
    }
}

GGUI::Element::Element(std::string Class, unsigned int width, unsigned int height, Element* parent, Coordinates* position){
    Add_Class("default");

    Name = std::to_string((unsigned long long)this);

    Fully_Stain();

    if (width != 0)
        Set_Width(width);
    if (height != 0)
        Set_Height(height);

    if (parent){
        Set_Parent(parent);

        Set_Position(position);
    }

    Add_Class(Class);

    Parse_Classes();
}

GGUI::Element::Element() {
    Add_Class("default");
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

GGUI::Element::Element(Styling css, unsigned int width, unsigned int height, Element* parent, Coordinates* position){
    Pause_GGUI([this, css, width, height, parent, position](){
        Add_Class("default");
        Parse_Classes();

        bool Previus_Border_State = Has_Border();

        Style = new Styling(css);

        Fully_Stain();

        //Check if the css changed the border state, if so we need to increment or decrement the width & height.
        Show_Border(Has_Border(), Previus_Border_State);

        if (width != 0)
            Set_Width(width);
        if (height != 0)
            Set_Height(height);

        if (parent){
            Set_Parent(parent);

            Set_Position(position);
        }

        Name = std::to_string((unsigned long long)this);
    });
}

GGUI::Element::Element(
    unsigned int width,
    unsigned int height,
    Coordinates position
) : Element(){
    Pause_GGUI([this, width, height, position](){
        Set_Width(width);
        Set_Height(height);

        Set_Position(position);
    });
}

//These next constructors are mainly for users to more easily create elements.
GGUI::Element::Element(
    unsigned int width,
    unsigned int height
) : Element(){
    Pause_GGUI([this, width, height](){
        Set_Width(width);
        Set_Height(height);
    });
}

GGUI::Element::Element(
    unsigned int width,
    unsigned int height,
    RGB text_color,
    RGB background_color
) : Element(){
    Pause_GGUI([this, width, height, text_color, background_color](){
        Set_Width(width);
        Set_Height(height);

        Set_Text_Color(text_color);
        Set_Background_Color(background_color);
    });
}

GGUI::Element::Element(
    unsigned int width,
    unsigned int height,
    RGB text_color,
    RGB background_color,
    RGB border_color,
    RGB border_background_color
) : Element(){
    Pause_GGUI([this, width, height, text_color, background_color, border_color, border_background_color](){
        Set_Width(width);
        Set_Height(height);

        Set_Text_Color(text_color);
        Set_Background_Color(background_color);
        Set_Border_Color(border_color);
        Set_Border_Background_Color(border_background_color);

        Show_Border(true);
    });
}

GGUI::Element::Element(const Element& copyable){
    Report("Don't use copy constructor use " + copyable.Get_Name() + "->Copy() instead!!!");
}

GGUI::Element::~Element(){
    // Ptr related members:
    // - Parent
    // - Childs
    // - Style
    // - Event Handlers
    // - Focused_On Clearance
    // - Hovered_On Clearance

    // Make sure this element is not listed in the parent element.
    // And if it does, then remove it from the parent element.
    for (unsigned int i = 0; Parent && i < Parent->Childs.size(); i++)
        if (Parent->Childs[i] == this){
            Parent->Childs.erase(Parent->Childs.begin() + i);

            // This may not be enough for the parent to know where to resample the buffer where this child used to be.
            Parent->Dirty.Dirty(STAIN_TYPE::DEEP);

            break;  // There should be no possibility, that there are appended two or more of this exact same element, they should be copied!!!
        }

    // Fire all the childs.
    for (int i = (signed)Childs.size() -1; i >= 0; i--)
        if (Childs[i]->Parent == this) 
            delete Childs[i];

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

void GGUI::Element::Fully_Stain(){

    this->Dirty.Dirty(STAIN_TYPE::CLASS | STAIN_TYPE::STRETCH | STAIN_TYPE::COLOR | STAIN_TYPE::DEEP | STAIN_TYPE::EDGE | STAIN_TYPE::MOVE);

}

void GGUI::Element::Inherit_States_From(Element* abstract){
    Focused = abstract->Focused;
    Show = abstract->Show;
}

std::pair<GGUI::RGB, GGUI::RGB>  GGUI::Element::Compose_All_Text_RGB_Values(){
    if (Focused){
        return {Style->Focus_Text_Color.Value, Style->Focus_Background_Color.Value};
    }
    else if (Hovered){
        return {Style->Hover_Text_Color.Value, Style->Hover_Background_Color.Value};
    }
    else{
        return {Style->Text_Color.Value, Style->Background_Color.Value};
    }
}

GGUI::RGB GGUI::Element::Compose_Text_RGB_Values(){
    if (Focused){
        return Style->Focus_Text_Color.Value;
    }
    else if (Hovered){
        return Style->Hover_Text_Color.Value;
    }
    else{
        return Style->Text_Color.Value;
    }
}

GGUI::RGB GGUI::Element::Compose_Background_RGB_Values(){
    if (Focused){
        return Style->Focus_Background_Color.Value;
    }
    else if (Hovered){
        return Style->Hover_Background_Color.Value;
    }
    else{
        return Style->Background_Color.Value;
    }
}

std::pair<GGUI::RGB, GGUI::RGB> GGUI::Element::Compose_All_Border_RGB_Values(){
    if (Focused){
        return {Style->Focus_Border_Color.Value, Style->Focus_Border_Background_Color.Value};
    }
    else if (Hovered){
        return {Style->Hover_Border_Color.Value, Style->Hover_Border_Background_Color.Value};
    }
    else{
        return {Style->Border_Color.Value, Style->Border_Background_Color.Value};
    }
}

//End of user constructors.

// Takes 0.0f to 1.0f
void GGUI::Element::Set_Opacity(float Opacity){
    // Normalize the float of 0.0 - 1.0 to 0 - 100 int value
    Opacity *= 100;
    Style->Opacity = (int)Opacity;

    Dirty.Dirty(STAIN_TYPE::STRETCH);
    Update_Frame();
}

void GGUI::Element::Set_Opacity(unsigned char Opacity){
    // Normalize the unsigned char of 0 - 255 to 0  - 100
    float tmp = (float)Opacity / (float)std::numeric_limits<unsigned char>::max();;
    Style->Opacity = (int)(tmp * 100);

    Dirty.Dirty(STAIN_TYPE::STRETCH);
    Update_Frame();
}

// returns int as 0 - 100
int GGUI::Element::Get_Opacity(){
    return Style->Opacity.Value;
}

bool GGUI::Element::Is_Transparent(){
    int FULL_OPACITY = 100;

    return Get_Opacity() < FULL_OPACITY;
}

unsigned int GGUI::Element::Get_Processed_Width(){
    if (Post_Process_Width != 0){
        return Post_Process_Width;
    }
    return Width;
}
unsigned int GGUI::Element::Get_Processed_Height(){
    if (Post_Process_Height != 0){
        return Post_Process_Height;
    }
    return Height;
}

void GGUI::Element::Show_Shadow(Vector2 Direction, RGB Shadow_Color, float Opacity, float Length){
    SHADOW_VALUE* properties = &Style->Shadow;

    properties->Color = Shadow_Color;
    properties->Direction = {Direction.X, Direction.Y, Length};
    properties->Opacity = Opacity;

    Position.X -= Length * Opacity;
    Position.Y -= Length * Opacity;

    Position += Direction * -1;
    
    properties->Status = VALUE_STATE::VALUE;

    Dirty.Dirty(STAIN_TYPE::STRETCH);
    Update_Frame();
}

void GGUI::Element::Show_Shadow(RGB Shadow_Color, float Opacity, float Length){
    SHADOW_VALUE* properties = &Style->Shadow;

    properties->Color = Shadow_Color;
    properties->Direction = {0, 0, Length};
    properties->Opacity = Opacity;

    Position.X -= Length * Opacity;
    Position.Y -= Length * Opacity;

    properties->Status = VALUE_STATE::VALUE;

    Dirty.Dirty(STAIN_TYPE::STRETCH);
    Update_Frame();
}

void GGUI::Element::Set_Parent(Element* parent){
    if (parent){
        Parent = parent;
    }
}

void GGUI::Element::Parse_Classes(){
    if (Style == nullptr){
        Style = new Styling();
    }

    bool Remember_To_Affect_Width_And_Height_Because_Of_Border = false;
    bool Previus_Border_Value = Style->Border_Enabled.Value;

    //Go through all classes and their styles and accumulate them.
    for(auto& Class : Classes){

        // The class wanted has not been yet constructed.
        // Pass it for the next render iteration
        if (GGUI::Classes.find(Class) == GGUI::Classes.end()){
            Dirty.Dirty(STAIN_TYPE::CLASS);
        }

        Style->Copy(new Styling(GGUI::Classes[Class]));
        
    }

    if (Remember_To_Affect_Width_And_Height_Because_Of_Border){
        Show_Border(Style->Border_Enabled.Value, Previus_Border_Value);
    }
}

void GGUI::Element::Set_Focus(bool f){
    if (f != Focused){
        Dirty.Dirty(STAIN_TYPE::COLOR | STAIN_TYPE::EDGE);

        Focused = f;

        Update_Frame();
    }
}

void GGUI::Element::Set_Hover_State(bool h){
    if (h != Hovered){
        Dirty.Dirty(STAIN_TYPE::COLOR | STAIN_TYPE::EDGE);

        Hovered = h;

        Update_Frame();
    }
}

GGUI::Styling GGUI::Element::Get_Style(){
    return *Style;
}

void GGUI::Element::Set_Style(Styling css){
    if (Style)
        Style->Copy(css);
    else
        Style = new Styling(css);

    Update_Frame();
}

void GGUI::Element::Add_Class(std::string class_name){
    if(Class_Names.find(class_name) != Class_Names.end()){
        Classes.push_back(Class_Names[class_name]);
    }
    else{
        Classes.push_back(GGUI::Get_Free_Class_ID(class_name));
    }
}

bool GGUI::Element::Has(std::string s){
    //first convert the string to the ID
    int id = Class_Names[s];

    //then check if the element has the class
    for(unsigned int i = 0; i < Classes.size(); i++){
        if(Classes[i] == id){
            return true;
        }
    }

    return false;
}

void GGUI::Element::Show_Border(bool b){
    if (b != Style->Border_Enabled.Value){
        Style->Border_Enabled = b;
        Dirty.Dirty(STAIN_TYPE::EDGE);
        Update_Frame();
    }
}

void GGUI::Element::Show_Border(bool b, bool Previus_State){
    if (b != Previus_State){
        Style->Border_Enabled = b;
        Dirty.Dirty(STAIN_TYPE::EDGE);
        Update_Frame();
    }
}

bool GGUI::Element::Has_Border(){
    return Style->Border_Enabled.Value;
}

void GGUI::Element::Add_Child(Element* Child){
    // Dont need to check both sides of the bordering, because the element only grows towards. to the bottom right corner

    bool This_Has_Border = Has_Border();
    bool Child_Has_Border = Child->Has_Border();

    int Border_Offsetter = (This_Has_Border - Child_Has_Border) * This_Has_Border;

    if (
        Child->Position.X + Child->Width > (Width - Border_Offsetter) || 
        Child->Position.Y + Child->Height > (Height - Border_Offsetter)
    ){
        if (Style->Allow_Dynamic_Size.Value){
            //Add the border offsetter to the width and the height to count for the border collision and evade it. 
            unsigned int New_Width = GGUI::Max(Child->Position.X + Child->Width + Border_Offsetter*2, Width);
            unsigned int New_Height = GGUI::Max(Child->Position.Y + Child->Height + Border_Offsetter*2, Height);

            //TODO: Maybe check the parent of this element to check?
            Height = New_Height;
            Width = New_Width;
        }
        else if (Child->Resize_To(this) == false){

            GGUI::Report(
                "Window exeeded static bounds\n "
                "Starts at: {" + std::to_string(Child->Position.X) + ", " + std::to_string(Child->Position.Y) + "}\n "
                "Ends at: {" + std::to_string(Child->Position.X + Child->Width) + ", " + std::to_string(Child->Position.Y + Child->Height) + "}\n "
                "Max is at: {" + std::to_string(Width) + ", " + std::to_string(Height) + "}\n "
            );

            return;
        }
    }

    Dirty.Dirty(STAIN_TYPE::DEEP);
    Child->Parent = this;

    Element_Names.insert({Child->Name, Child});

    Childs.push_back(Child);

    // Make sure that elements with higher Z, are rendered later, making them visible as on top.
    Re_Order_Childs();

    Update_Frame();
}

void GGUI::Element::Set_Childs(std::vector<Element*> childs){
    Pause_GGUI([this, childs](){
        for (auto& Child : childs){
            Add_Child(Child);
        }
    });
}

std::vector<GGUI::Element*>& GGUI::Element::Get_Childs(){
    return Childs;
}

bool GGUI::Element::Remove(Element* handle){
    for (unsigned int i = 0; i < Childs.size(); i++){
        if (Childs[i] == handle){
            //If the mouse if focused on this about to be deleted element, change mouse position into it's parent Position.
            if (Focused_On == Childs[i]){
                Mouse = Childs[i]->Parent->Position;
            }

            delete handle;

            Dirty.Dirty(STAIN_TYPE::DEEP | STAIN_TYPE::COLOR);

            return true;
        }
    }
    return false;
}

void GGUI::Element::Update_Parent(Element* New_Element){
    //normally elements dont do anything
    if (!New_Element->Is_Displayed()){
        Fully_Stain();
    }

    // When the child is unable to flag changes on parent Render(), like on removal-
    // Then ask the parent to discard the previous buffer and render from scratch.    
    if (Parent){
        Fully_Stain();
        Update_Frame(); 
    }
}

void GGUI::Element::Check(State s){
    if (State_Handlers.find(s) != State_Handlers.end()){
        State_Handlers[s]();
    }
}

void GGUI::Element::Display(bool f){
    // Check if the to be displayed is true and the element wasnt already displayed.
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
            for (Element* c : Childs){
                c->Display(f);
            }
        });
    }
}

bool GGUI::Element::Is_Displayed(){
    return Show;
}

bool GGUI::Element::Remove(unsigned int index){
    if (index > Childs.size() - 1){
        return false;
    }
    Element* tmp = Childs[index];

    //If the mouse if focused on this about to be deleted element, change mouse position into it's parent Position.
    if (Focused_On == tmp){
        Mouse = tmp->Parent->Position;
    }

    delete tmp;
    
    Dirty.Dirty(STAIN_TYPE::DEEP | STAIN_TYPE::COLOR);
    
    return true;
}

void GGUI::Element::Remove(){
    if (Parent){
        //tell the parent what is about to happen.
        //you need to update the parent before removing the child, 
        //otherwise the code cannot erase it when it is not found!
        Parent->Remove(this);
    }
    else{
        Report(
            std::string("Cannot remove ") + Get_Name() + std::string(", with no parent\n")
        );
    }
}

void GGUI::Element::Set_Dimensions(unsigned int width, unsigned int height){
    if (width != Width || height != Height){    
        Width = width;
        Height = height;
        //Fully_Stain();
        Dirty.Dirty(STAIN_TYPE::STRETCH);
        Update_Frame();
    }
}

unsigned int GGUI::Element::Get_Width(){
    return Width;
}

unsigned int GGUI::Element::Get_Height(){
    return Height;
}

void GGUI::Element::Set_Width(unsigned int width){
    if (width != Width){
        Width = width;
        Fully_Stain();
        Update_Frame();
    }
}

void GGUI::Element::Set_Height(unsigned int height){
    if (height != Height){
        Height = height;
        Fully_Stain();
        Update_Frame();
    }
}

void GGUI::Element::Set_Position(Coordinates c){
    Position = c;

    this->Dirty.Dirty(STAIN_TYPE::MOVE);

    Update_Frame();
}

void GGUI::Element::Set_Position(Coordinates* c){
    if (c){
        Set_Position(*c);
    }
}

GGUI::Coordinates GGUI::Element::Get_Position(){
    return Position;
}

GGUI::Coordinates GGUI::Element::Get_Absolute_Position(){
    return Style->Absolute_Position_Cache.Value;
}

void GGUI::Element::Update_Absolute_Position_Cache(){
    Style->Absolute_Position_Cache = {0, 0, 0};

    if (Parent){
        Style->Absolute_Position_Cache = Parent->Get_Position();
    }

    Style->Absolute_Position_Cache.Value += Position;
}

void GGUI::Element::Set_Margin(MARGIN_VALUE margin){
    Style->Margin = margin;
}

GGUI::MARGIN_VALUE GGUI::Element::Get_Margin(){
    return Style->Margin;
}

GGUI::Element* GGUI::Element::Copy(){
    //compile time check       
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
    for (unsigned int i = 0; i < this->Get_Childs().size(); i++){
        new_element->Childs[i] = this->Get_Childs()[i]->Copy();
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

std::pair<unsigned int, unsigned int> GGUI::Element::Get_Fitting_Dimensions(Element* child){
    Coordinates Current_Position = child->Get_Position();

    unsigned int Result_Width = 0;
    unsigned int Result_Height = 0;

    int Border_Offset = (Has_Border() - child->Has_Border()) * Has_Border() * 2;

    // if there are only zero child or one and it is same as this child then give max.
    if (Childs.size() == 0 || Childs.back() == child){
        return {Width - Border_Offset, Height - Border_Offset};
    }

    while (true){
        if (Current_Position.X + Result_Width < Width - Border_Offset){
            Result_Width++;
        }
        
        if (Current_Position.Y + Result_Height < Height - Border_Offset){
            Result_Height++;
        }
        else if (Current_Position.X + Result_Width >= Width - Border_Offset && Current_Position.Y + Result_Height >= Height - Border_Offset){
            break;
        }
        
        for (auto c : Childs){
            // Use local positioning since this is a civil dispute :)
            if (child != c && Collides(c->Get_Position(), Current_Position, c->Get_Width(), c->Get_Height(), Result_Width, Result_Height)){
                //there are already other childs occupying this area so we can stop here.
                return {Result_Width, Result_Height};
            }
        }

    }

    return {Result_Width, Result_Height};
}

std::pair<unsigned int, unsigned int> GGUI::Element::Get_Limit_Dimensions(){
    unsigned int max_width = 0;
    unsigned int max_height = 0;

    if (Parent){
        std::pair<unsigned int, unsigned int> Max_Dimensions = Parent->Get_Fitting_Dimensions(this);

        max_width = Max_Dimensions.first;
        max_height = Max_Dimensions.second;
    }
    else{
        if ((Element*)this == (Element*)GGUI::Main){
            max_width = Max_Width;
            max_height = Max_Height;
        }
        else{
            max_width = GGUI::Main->Get_Width() - GGUI::Main->Has_Border() * 2;
            max_height = GGUI::Main->Get_Height() - GGUI::Main->Has_Border() * 2;
        }
    }

    return {max_width, max_height};
}

void GGUI::Element::Set_Background_Color(RGB color){
    Style->Background_Color = color;
    if (Style->Border_Background_Color.Value == Style->Background_Color.Value)
        Style->Border_Background_Color = color;
        
    Dirty.Dirty(STAIN_TYPE::COLOR);
    
    Update_Frame();
}

GGUI::RGB GGUI::Element::Get_Background_Color(){
    return Style->Background_Color.Value;
}

void GGUI::Element::Set_Border_Color(RGB color){
    Style->Border_Color = color;
    Dirty.Dirty(STAIN_TYPE::COLOR);
    Update_Frame();
}

GGUI::RGB GGUI::Element::Get_Border_Color(){
    return Style->Border_Color.Value;
}

void GGUI::Element::Set_Border_Background_Color(RGB color){
    Style->Border_Background_Color = color;
    Dirty.Dirty(STAIN_TYPE::COLOR);
    Update_Frame();
}

GGUI::RGB GGUI::Element::Get_Border_Background_Color(){
    return Style->Border_Background_Color.Value;
}

void GGUI::Element::Set_Text_Color(RGB color){
    Style->Text_Color = color;
    Dirty.Dirty(STAIN_TYPE::COLOR);
    Update_Frame();
}

void GGUI::Element::Allow_Dynamic_Size(bool True){
    Style->Allow_Dynamic_Size = True; 
    // No need to update the frame, since this is used only on content change which has the update frame.
}

bool GGUI::Element::Is_Dynamic_Size_Allowed(){
    return Style->Allow_Dynamic_Size.Value;
}

void GGUI::Element::Allow_Overflow(bool True){
    Style->Allow_Overflow = True; 
    // No need to update the frame, since this is used only on content change which has the update frame.
}

bool GGUI::Element::Is_Overflow_Allowed(){
    return Style->Allow_Overflow.Value;
}

GGUI::RGB GGUI::Element::Get_Text_Color(){
    return Style->Text_Color.Value;
}

void GGUI::Element::Compute_Dynamic_Size(){
    // Go through all elements displayed.
    if (!Is_Displayed())
        return;

    if (Children_Changed()){
        for (auto c : Childs){
            if (!c->Is_Displayed())
                continue;

            // Check the child first if it has to stretch before this can even know if it needs to stretch.
            c->Compute_Dynamic_Size();

            int Border_Offsetter = (Has_Border() - c->Has_Border()) * Has_Border() * 2;

            // Add the border offsetter to the width and the height to count for the border collision and evade it. 
            unsigned int New_Width = (unsigned int)GGUI::Max(c->Position.X + (signed int)c->Width + Border_Offsetter, (signed int)Width);
            unsigned int New_Height = (unsigned int)GGUI::Max(c->Position.Y + (signed int)c->Height + Border_Offsetter, (signed int)Height);

            // but only update those who actually allow dynamic sizing.
            if (Style->Allow_Dynamic_Size.Value && (New_Width != Width || New_Height != Height)){
                Height = New_Height;
                Width = New_Width;
                Dirty.Dirty(STAIN_TYPE::STRETCH);
            }
        }
    }

    return;
}

// Draws into the this.Render_Buffer nested buffer of AST window's
std::vector<GGUI::UTF>& GGUI::Element::Render(){
    std::vector<GGUI::UTF>& Result = Render_Buffer;

    //if inned children have changed without this changing, then this will trigger.
    if (Children_Changed() || Has_Transparent_Children()){
        Dirty.Dirty(STAIN_TYPE::DEEP | STAIN_TYPE::STRETCH);
    }

    Calculate_Childs_Hitboxes();    // Normally elements will NOT oder their content by hitbox system.

    Compute_Dynamic_Size();

    if (Dirty.is(STAIN_TYPE::CLEAN))
        return Result;

    if (Dirty.is(STAIN_TYPE::CLASS)){
        Parse_Classes();

        Dirty.Clean(STAIN_TYPE::CLASS);
    }

    if (Dirty.is(STAIN_TYPE::MOVE)){
        Dirty.Clean(STAIN_TYPE::MOVE);
        
        Update_Absolute_Position_Cache();
    }

    if (Dirty.is(STAIN_TYPE::STRETCH)){
        Result.clear();
        Result.resize(Width * Height, SYMBOLS::EMPTY_UTF);
        Dirty.Clean(STAIN_TYPE::STRETCH);

        Dirty.Dirty(STAIN_TYPE::COLOR | STAIN_TYPE::EDGE | STAIN_TYPE::DEEP);
    }

    //Apply the color system to the resized result list
    if (Dirty.is(STAIN_TYPE::COLOR))
        Apply_Colors(this, Result);

    bool Connect_Borders_With_Parent = Has_Border();
    unsigned int Childs_With_Borders = 0;

    //This will add the child windows to the Result buffer
    if (Dirty.is(STAIN_TYPE::DEEP)){
        Dirty.Clean(STAIN_TYPE::DEEP);

        for (auto c : this->Get_Childs()){
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
        for (auto A : this->Get_Childs()){
            for (auto B : this->Get_Childs()){
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



//These are just utility functions for internal purposes, dont need to sed Dirty.
void GGUI::Element::Apply_Colors(Element* w, std::vector<UTF>& Result){
    Dirty.Clean(STAIN_TYPE::COLOR);

    for (auto& utf : Result){
        utf.Set_Color(w->Compose_All_Text_RGB_Values());
    }
}

void GGUI::Element::Add_Overhead(GGUI::Element* w, std::vector<GGUI::UTF>& Result){
    Dirty.Clean(STAIN_TYPE::EDGE);
    
    if (!w->Has_Border())
        return;

    GGUI::BORDER_STYLE_VALUE custom_border = Style->Border_Style;

    for (unsigned int y = 0; y < Height; y++){
        for (unsigned int x = 0; x < Width; x++){
            //top left corner
            if (y == 0 && x == 0){
                Result[y * Width + x] = GGUI::UTF(custom_border.TOP_LEFT_CORNER, w->Compose_All_Border_RGB_Values());
            }
            //top right corner
            else if (y == 0 && x == Width - 1){
                Result[y * Width + x] = GGUI::UTF(custom_border.TOP_RIGHT_CORNER, w->Compose_All_Border_RGB_Values());
            }
            //bottom left corner
            else if (y == Height - 1 && x == 0){
                Result[y * Width + x] = GGUI::UTF(custom_border.BOTTOM_LEFT_CORNER, w->Compose_All_Border_RGB_Values());
            }
            //bottom right corner
            else if (y == Height - 1 && x == Width - 1){
                Result[y * Width + x] = GGUI::UTF(custom_border.BOTTOM_RIGHT_CORNER, w->Compose_All_Border_RGB_Values());
            }
            //The roof border
            else if (y == 0 || y == Height - 1){
                Result[y * Width + x] = GGUI::UTF(custom_border.HORIZONTAL_LINE, w->Compose_All_Border_RGB_Values());
            }
            //The left border
            else if (x == 0 || x == Width - 1){
                Result[y * Width + x] = GGUI::UTF(custom_border.VERTICAL_LINE, w->Compose_All_Border_RGB_Values());
            }
        }
    }
}

void GGUI::Element::Compute_Alpha_To_Nesting(GGUI::UTF& Dest, GGUI::UTF Source){
    // If the Source element has full opacity, then the destination gets fully rewritten over.
    if (Source.Background.Alpha == std::numeric_limits<unsigned char>::max()){
        Dest = Source;
        return;
    }
    
    if (Source.Background.Alpha == std::numeric_limits<unsigned char>::min()) return;         // Dont need to do anything.

    // Color the Destination UTF by the Source UTF background color.
    Dest.Background += Source.Background;

    // Check if source has text
    if (!Source.Has_Default_Text()){
        Dest.Set_Text(Source);

        // Set the text color right.
        if (!Dest.Has_Default_Text()){
            Dest.Foreground += Source.Foreground; 
        }
    }
}

std::pair<std::pair<unsigned int, unsigned int> ,std::pair<std::pair<unsigned int, unsigned int>, std::pair<unsigned int, unsigned int>>> GGUI::Element::Get_Fitting_Area(GGUI::Element* Parent, GGUI::Element* Child){
    bool Border_Offset = (Parent->Has_Border() - Child->Has_Border()) && Parent->Has_Border();
    
    unsigned int Max_Allowed_Height = Parent->Height - Border_Offset;               //remove bottom borders from calculation
    unsigned int Max_Allowed_Width = Parent->Width - Border_Offset;                 //remove right borders from calculation

    unsigned int Min_Allowed_Height = 0 + Border_Offset;                            //add top borders from calculation
    unsigned int Min_Allowed_Width = 0 + Border_Offset;                             //add left borders from calculation

    unsigned int Child_Start_Y = Min_Allowed_Height + GGUI::Max(Child->Position.Y, 0);    // If the child is negatively positioned, then put it to zero and minimize the parent height.
    unsigned int Child_Start_X = Min_Allowed_Width + GGUI::Max(Child->Position.X, 0);    

    unsigned int Negative_Offset_X = abs(GGUI::Min(Child->Position.X, 0));
    unsigned int Negative_Offset_Y = abs(GGUI::Min(Child->Position.Y, 0));

    unsigned int Child_End_X = GGUI::Max(0, (int)(Child_Start_X + Child->Get_Processed_Width()) - (int)Negative_Offset_X);
    unsigned int Child_End_Y = GGUI::Max(0, (int)(Child_Start_Y + Child->Get_Processed_Height()) - (int)Negative_Offset_Y);

    Child_End_X = GGUI::Min(Max_Allowed_Width, Child_End_X);
    Child_End_Y = GGUI::Min(Max_Allowed_Height, Child_End_Y);

    // {Negative offset},                             {Child Starting offset},        {Child Ending offset}
    return {{Negative_Offset_X, Negative_Offset_Y}, {{Child_Start_X, Child_Start_Y}, {Child_End_X, Child_End_Y}} };
}

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
            unsigned int Child_Buffer_Y = (y - Start_Y + Negative_Offset_Y) * Child->Get_Processed_Width();
            unsigned int Child_Buffer_X = (x - Start_X + Negative_Offset_X); 
            Compute_Alpha_To_Nesting(Parent_Buffer[y * Width + x], Child_Buffer[Child_Buffer_Y + Child_Buffer_X]);
        }
    }
}

inline bool Is_In_Bounds(GGUI::Coordinates index, GGUI::Element* parent){
    // checks if the index is out of bounds
    if (index.X < 0 || index.Y < 0 || index.X >= (signed)parent->Get_Width() || index.Y >= (signed)parent->Get_Height())
        return false;

    return true;
}

inline GGUI::UTF* From(GGUI::Coordinates index, std::vector<GGUI::UTF>& Parent_Buffer, GGUI::Element* Parent){
    return &Parent_Buffer[index.Y * Parent->Get_Width() + index.X];
}

std::unordered_map<unsigned int, const char*> GGUI::Element::Get_Custom_Border_Map(GGUI::Element* e){
    GGUI::BORDER_STYLE_VALUE custom_border_style = e->Get_Border_Style();

    return Get_Custom_Border_Map(custom_border_style);
}

std::unordered_map<unsigned int, const char*> GGUI::Element::Get_Custom_Border_Map(GGUI::BORDER_STYLE_VALUE custom_border_style){
    return {
            {GGUI::SYMBOLS::CONNECTS_DOWN | GGUI::SYMBOLS::CONNECTS_RIGHT, custom_border_style.TOP_LEFT_CORNER},
            {GGUI::SYMBOLS::CONNECTS_DOWN | GGUI::SYMBOLS::CONNECTS_LEFT, custom_border_style.TOP_RIGHT_CORNER},
            {GGUI::SYMBOLS::CONNECTS_UP | GGUI::SYMBOLS::CONNECTS_RIGHT, custom_border_style.BOTTOM_LEFT_CORNER},
            {GGUI::SYMBOLS::CONNECTS_UP | GGUI::SYMBOLS::CONNECTS_LEFT, custom_border_style.BOTTOM_RIGHT_CORNER},

            {GGUI::SYMBOLS::CONNECTS_DOWN | GGUI::SYMBOLS::CONNECTS_UP, custom_border_style.VERTICAL_LINE},

            {GGUI::SYMBOLS::CONNECTS_LEFT | GGUI::SYMBOLS::CONNECTS_RIGHT, custom_border_style.HORIZONTAL_LINE},

            {GGUI::SYMBOLS::CONNECTS_DOWN | GGUI::SYMBOLS::CONNECTS_UP | GGUI::SYMBOLS::CONNECTS_RIGHT, custom_border_style.VERTICAL_RIGHT_CONNECTOR},
            {GGUI::SYMBOLS::CONNECTS_DOWN | GGUI::SYMBOLS::CONNECTS_UP | GGUI::SYMBOLS::CONNECTS_LEFT, custom_border_style.VERTICAL_LEFT_CONNECTOR},

            {GGUI::SYMBOLS::CONNECTS_LEFT | GGUI::SYMBOLS::CONNECTS_RIGHT | GGUI::SYMBOLS::CONNECTS_DOWN, custom_border_style.HORIZONTAL_BOTTOM_CONNECTOR},
            {GGUI::SYMBOLS::CONNECTS_LEFT | GGUI::SYMBOLS::CONNECTS_RIGHT | GGUI::SYMBOLS::CONNECTS_UP, custom_border_style.HORIZONTAL_TOP_CONNECTOR},

            {GGUI::SYMBOLS::CONNECTS_LEFT | GGUI::SYMBOLS::CONNECTS_RIGHT | GGUI::SYMBOLS::CONNECTS_UP | GGUI::SYMBOLS::CONNECTS_DOWN, custom_border_style.CROSS_CONNECTOR}
        };
}

void GGUI::Element::Set_Custom_Border_Style(GGUI::BORDER_STYLE_VALUE style){
    Style->Border_Style = style;
    Dirty.Dirty(STAIN_TYPE::EDGE);

    Show_Border(true);
}

GGUI::BORDER_STYLE_VALUE GGUI::Element::Get_Custom_Border_Style(){
    return Style->Border_Style;
}

void GGUI::Element::Post_Process_Borders(Element* A, Element* B, std::vector<UTF>& Parent_Buffer){
    // We only need to calculate the childs points in which they intersect with the parent borders.
    // At these intersecting points of border we will construct a bit mask that portraits the connections the middle point has.
    // With the calculated bit mask we can fetch from the 'SYMBOLS::Border_Identifiers' the right border string.

    // First calculate if the childs borders even touch the parents borders.
    // If not, there is no need to calculate anything.

    // First calculate if the child is outside the parent.
    if (
        B->Position.X + (signed)B->Width < A->Position.X ||
        B->Position.X > A->Position.X + (signed)A->Width ||
        B->Position.Y + (signed)B->Height < A->Position.Y ||
        B->Position.Y > A->Position.Y + (signed)A->Height
    )
        return;

    // Now calculate if the child is inside the parent.
    if (
        B->Position.X > A->Position.X &&
        B->Position.X + (signed)B->Width < A->Position.X + (signed)A->Width &&
        B->Position.Y > A->Position.Y &&
        B->Position.Y + (signed)B->Height < A->Position.Y + (signed)A->Height
    )
        return;


    // Now that we are here it means the both boxes interlace eachother.
    // We will calculate the hitting points by drawing segments from corner to corner and then comparing one segments x to other segments y, and so forth.

    // two nested loops rotating the x and y usages.
    // store the line x,y into a array for the nested loops to access.
    std::vector<int> Vertical_Line_X_Coordinates = {
        
        B->Position.X,
        A->Position.X,
        B->Position.X + (int)B->Width - 1,
        A->Position.X + (int)A->Width - 1,

                
        // A->Position.X,
        // B->Position.X,
        // A->Position.X + A->Width - 1,
        // B->Position.X + B->Width - 1

    };

    std::vector<int> Horizontal_Line_Y_Cordinates = {
        
        A->Position.Y,
        B->Position.Y + (int)B->Height - 1,
        A->Position.Y,
        B->Position.Y + (int)B->Height - 1,

        // B->Position.Y,
        // A->Position.Y + A->Height - 1,
        // B->Position.Y,
        // A->Position.Y + A->Height - 1,

    };

    std::vector<Coordinates> Crossing_Indicies;

    // Go through singular box
    for (unsigned int Box_Index = 0; Box_Index < Horizontal_Line_Y_Cordinates.size(); Box_Index++){
        // Now just pair the indicies from the two lists.
        Crossing_Indicies.push_back(
            // First pair
            Coordinates(
                Vertical_Line_X_Coordinates[Box_Index],
                Horizontal_Line_Y_Cordinates[Box_Index]
            )
        );
    }

    std::unordered_map<unsigned int, const char*> custom_border = Get_Custom_Border_Map(A);

    // Now that we have the crossing points we can start analyzing the ways they connect to construct the bit masks.
    for (auto c : Crossing_Indicies){

        Coordinates Above = { c.X, Max((signed)c.Y - 1, 0) };
        Coordinates Below = { c.X, c.Y + 1 };
        Coordinates Left = { Max((signed)c.X - 1, 0), c.Y };
        Coordinates Right = { c.X + 1, c.Y };

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

// Gives you an Action wrapper on the Event wrapper
void GGUI::Element::On_Click(std::function<bool(GGUI::Event*)> action){
    Action* a = new Action(
        Constants::MOUSE_LEFT_CLICKED,
        [this, action](GGUI::Event* e){
            if (Collides(this, Mouse)){
                // Construct an Action from the Event obj
                GGUI::Action* wrapper = new GGUI::Action(e->Criteria, action, this);

                action(wrapper);

                //action succesfully executed.
                return true;
            }
            //action failed.
            return false;
        },
        this
    );
    GGUI::Event_Handlers.push_back(a);
}

void GGUI::Element::On(unsigned long long criteria, std::function<bool(GGUI::Event*)> action, bool GLOBAL){
    Action* a = new Action(
        criteria,
        [this, action, GLOBAL](GGUI::Event* e){
            if (Collides(this, Mouse) || GLOBAL){
                //action succesfully executed.
                return action(e);
            }
            //action failed.
            return false;
        },
        this
    );
    GGUI::Event_Handlers.push_back(a);
}

bool GGUI::Element::Children_Changed(){
    // This is used if an element is recently hidden so the DEEP search wound't find it if not for this. 
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
    for (auto e : Childs){
        if (e->Children_Changed())
            return true;
    }

    return false;
}

bool GGUI::Element::Has_Transparent_Children(){
    if (!Show)
        return false;

    // If the current is a transparent and has is NOT clean.
    if (Is_Transparent() && Dirty.Type != STAIN_TYPE::CLEAN)
        return true;
    
    for (auto e : Childs){
        if (e->Has_Transparent_Children())
            return true;
    }

    return false;
}

void GGUI::Element::Set_Name(std::string name){
    Name = name;

    Element_Names[name] = this;
}

GGUI::Element* GGUI::Element::Get_Element(std::string name){
    Element* Result = nullptr;

    if (Element_Names.find(name) != Element_Names.end()){
        Result = Element_Names[name];
    }

    return Result;
}

// Rre orders the childs by the z position, where the biggest z, goes last.
void GGUI::Element::Re_Order_Childs(){
    std::sort(Childs.begin(), Childs.end(), [](Element* a, Element* b){
        return a->Get_Position().Z <= b->Get_Position().Z;
    });
}

void GGUI::Element::Focus(){

    GGUI::Mouse = this->Position;
    GGUI::Update_Focused_Element(this);

}

void GGUI::Element::On_State(State s, std::function<void()> job){
    State_Handlers[s] = job;
}

bool Is_Signed(int x){
    return x < 0;
}

int Get_Sign(int x){
    return Is_Signed(x) ? -1 : 1;
}

// Constructs two squares one 2 steps larger on width and height, and given the differented indicies.
std::vector<GGUI::Coordinates> Get_Surrounding_Indicies(int Width, int Height, GGUI::Coordinates start_offset, GGUI::Vector2 Offset){

    std::vector<GGUI::Coordinates> Result;

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

bool GGUI::Element::Has_Postprocessing_To_Do(){
    bool Has_Shadow_Processing = Style->Shadow.Enabled;

    bool Has_Opacity_Processing = Is_Transparent();

    return Has_Shadow_Processing | Has_Opacity_Processing;
}

void GGUI::Element::Process_Shadow(std::vector<GGUI::UTF>& Current_Buffer){
    if (!Style->Shadow.Enabled)
        return;

    SHADOW_VALUE& properties = Style->Shadow;


    // First calculate the new buffer size.
    // This is going to be the new two squares overlapping minus buffer.

    // Calculate the zero origin when the equation is: -properties.Direction.Z * X + properties.Opacity = 0
    // -a * x + o = 0
    // x = o / a

    int Shadow_Length = properties.Direction.Z * properties.Opacity;

    unsigned int Shadow_Box_Width = Width + (Shadow_Length * 2);
    unsigned int Shadow_Box_Height = Height + (Shadow_Length * 2);
    
    std::vector<GGUI::UTF> Shadow_Box;
    Shadow_Box.resize(Shadow_Box_Width * Shadow_Box_Height);

    unsigned char Current_Alpha = properties.Opacity * std::numeric_limits<unsigned char>::max();;
    float previus_opacity = properties.Opacity;
    int Current_Box_Start_X = Shadow_Length;
    int Current_Box_Start_Y = Shadow_Length;

    int Current_Shadow_Width = Width;
    int Current_Shadow_Height = Height;

    for (int i = 0; i < Shadow_Length; i++){
        std::vector<Coordinates> Shadow_Indicies = Get_Surrounding_Indicies(
            Current_Shadow_Width,
            Current_Shadow_Height,
            { 
                Current_Box_Start_X--,
                Current_Box_Start_Y--
            },
            properties.Direction
        );

        Current_Shadow_Width += 2;
        Current_Shadow_Height += 2;

        UTF shadow_pixel;
        shadow_pixel.Background = properties.Color;
        shadow_pixel.Background.Alpha = Current_Alpha;

        for (auto& index : Shadow_Indicies){
            Shadow_Box[index.Y * Shadow_Box_Width + index.X] = shadow_pixel;
        }

        previus_opacity *= GGUI::Min(0.9f, (float)properties.Direction.Z);
        Current_Alpha = previus_opacity * std::numeric_limits<unsigned char>::max();;
    }

    // Now offset the shadow box buffer by the direction.
    int Offset_Box_Width = Shadow_Box_Width + abs((int)properties.Direction.X);
    int Offset_Box_Height = Shadow_Box_Height + abs((int)properties.Direction.Y);

    std::vector<GGUI::UTF> Swapped_Buffer = Current_Buffer;

    Current_Buffer.resize(Offset_Box_Width * Offset_Box_Height);

    Coordinates Shadow_Box_Start = {
        GGUI::Max(0, (int)properties.Direction.X),
        GGUI::Max(0, (int)properties.Direction.Y)
    };

    Coordinates Original_Box_Start = {
        Shadow_Box_Start.X - properties.Direction.X + Shadow_Length,
        Shadow_Box_Start.Y - properties.Direction.Y + Shadow_Length
    };

    Coordinates Original_Box_End = {
        Original_Box_Start.X + Width,
        Original_Box_Start.Y + Height
    };

    Coordinates Shadow_Box_End = {
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

void GGUI::Element::Process_Opacity(std::vector<GGUI::UTF>& Current_Buffer){
    if (!Is_Transparent())
        return;

    float As_Float = (float)Style->Opacity.Value / 100.0f;

    for (unsigned int Y = 0; Y < Get_Processed_Height(); Y++){
        for (unsigned int X = 0; X < Get_Processed_Width(); X++){
            UTF& tmp = Current_Buffer[Y * Get_Processed_Width() + X];

            tmp.Background.Alpha = tmp.Background.Alpha * As_Float;
            tmp.Foreground.Alpha = tmp.Foreground.Alpha * As_Float;
        }
    }
}

std::vector<GGUI::UTF>& GGUI::Element::Postprocess(){
    Post_Process_Buffer = Render_Buffer;

    Process_Shadow(Post_Process_Buffer);
    //...

    // One of the last postprocessing for total control of when not to display.
    Process_Opacity(Post_Process_Buffer);

    //Render_Buffer = Result;
    return Post_Process_Buffer;
}

bool GGUI::Element::Child_Is_Shown(Element* other){

    bool Border_Modifier = (Has_Border() - other->Has_Border()) && Has_Border();

    // Check if the child element is atleast above the {0, 0} 
    int Minimum_X = other->Position.X + other->Get_Processed_Width();
    int Minimum_Y = other->Position.Y + other->Get_Processed_Height();

    // Check even if the child position is way beyond the parent width and height, if only the shadow for an example is still shown.
    int Maximum_X = other->Position.X - (other->Width - other->Get_Processed_Width());
    int Maximum_Y = other->Position.Y - (other->Height - other->Get_Processed_Height());

    bool X_Is_Inside = Minimum_X >= Border_Modifier && Maximum_X < (signed)Width - Border_Modifier;
    bool Y_Is_Inside = Minimum_Y >= Border_Modifier && Maximum_Y < (signed)Height - Border_Modifier;

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