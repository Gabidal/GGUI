#include "Text_Field.h"
#include "../Renderer.h"
#include "HTML.h"

#include <algorithm>

GGUI::Text_Field::Text_Field(std::string Text, Styling css) : Element(css) {
    Data = Text;
    
    std::pair<int, int> D = Get_Text_Dimensions(Text);

    if (Width == 0 || Width < D.first){
        Width = D.first;
    }
    if (Height == 0 || Height < D.second){
        Height = D.second;
    }
}

//These next constructors are mainly for users to more easily create elements.
GGUI::Text_Field::Text_Field(
    std::string Text,
    RGB text_color,
    RGB background_color
) : Text_Field(Text, {}){
    Pause_Renderer([=](){
        Set_Text_Color(text_color);
        Set_Background_Color(background_color);
    });
}

GGUI::Text_Field::Text_Field(
    std::string Text,
    RGB text_color,
    RGB background_color,
    RGB border_color,
    RGB border_background_color
) : Text_Field(Text, {}){    
    Pause_Renderer([=](){
        Set_Text_Color(text_color);
        Set_Background_Color(background_color);
        Set_Border_Color(border_color);
        Set_Border_Background_Color(border_background_color);
        
        Show_Border(true);
    });
}

//End of user constructors.

std::vector<GGUI::UTF> GGUI::Text_Field::Render(){
    std::vector<GGUI::UTF> Result = Render_Buffer;
            
    if (Dirty.is(STAIN_TYPE::CLEAN))
        return Result;

    if (Dirty.is(STAIN_TYPE::CLASS)){
        Parse_Classes();

        Dirty.Clean(STAIN_TYPE::CLASS);
    }

    if (Dirty.is(STAIN_TYPE::MOVE)){
        Dirty.Clean(STAIN_TYPE::MOVE);

        Absolute_Position_Cache = {0, 0, 0};

        if (Parent){
            Absolute_Position_Cache = Parent->Get_Position();
        }

        Absolute_Position_Cache += Position;
    }

    if (Data.size() != Previus_Data.size()){
        Dirty.Dirty(STAIN_TYPE::STRETCH);
        Previus_Data = Data;
    }

    if (Dirty.is(STAIN_TYPE::STRETCH)){
        Result.clear();
        Result.resize(Width * Height, SYMBOLS::EMPTY_UTF);
        Dirty.Clean(STAIN_TYPE::STRETCH);
        
        Dirty.Dirty(STAIN_TYPE::COLOR | STAIN_TYPE::EDGE | STAIN_TYPE::DEEP);
    }

    if (Dirty.is(STAIN_TYPE::DEEP)){
        Dirty.Clean(STAIN_TYPE::DEEP);

        switch ((TEXT_LOCATION)Style->Text_Position.Value)
        {
        case TEXT_LOCATION::CENTER:
            Center_Text(this, Data, Parent, Result);
            break;
        case TEXT_LOCATION::LEFT:
            Left_Text(this, Data, Parent, Result);
            break;
        case TEXT_LOCATION::RIGHT:
            Right_Text(this, Data, Parent, Result);
            break;
        default:
            break;
        }
    }

    if (Dirty.is(STAIN_TYPE::COLOR))
        Apply_Colors(this, Result);

    if (Dirty.is(STAIN_TYPE::EDGE))
        Add_Overhead(this, Result);

    Render_Buffer = Result;

    return Result;
}

void GGUI::Text_Field::Show_Border(bool state){
    if (state && Style->Border_Enabled.Value == false){
        //enlarge the text field area to compas the borders
        Width += 2;
        Height += 2;
    }
    else if (!state && Style->Border_Enabled.Value == true){
        //shrink the text field area to compas the borders
        Width -= 2;
        Height -= 2;
    }
    
    Style->Border_Enabled = state;
    Dirty.Dirty(STAIN_TYPE::EDGE);

    Update_Frame();
}

std::pair<unsigned int, unsigned int> GGUI::Text_Field::Get_Text_Dimensions(std::string& text){
    //calculate the most longest row of text and set it as the width
    int longest_row = 0;
    int Current_Row = 0;

    int longest_Height = 1;
    for (int c = 0; c < text.size(); c++){
        if (text[c] == '\n'){
            Current_Row = 0;
            longest_Height++;

            text.erase(text.begin() + c--);
        }
        else{
            Current_Row++;

            if (Current_Row > longest_row)
                longest_row = Current_Row;    
        }
    }

    return {longest_row, longest_Height};
}

bool GGUI::Text_Field::Resize_To(Element* parent){
    std::pair<int, int> Max_Dimensions = parent->Get_Fitting_Dimensions(this);

    int New_Width = Max_Dimensions.first - Has_Border() * 2;
    int New_Height = Data.size() / New_Width + Has_Border() * 2;

    //now we can check if the new_height is larger than the max_height
    if (New_Height > Max_Dimensions.second){
        return false;
    }
    
    Width = New_Width;
    Height = New_Height;

    Dirty.Dirty(STAIN_TYPE::STRETCH);
    Update_Frame();

    return true;
}

//The Text buffer needs to contain already left centered text.
void GGUI::Text_Field::Center_Text(GGUI::Element* self, std::string Text, GGUI::Element* wrapper, std::vector<GGUI::UTF>& Result){
    int self_width = self->Get_Width();
    int self_height = self->Get_Height();

    int Has_Border = self->Has_Border();

    int Width_Center = self_width / 2;

    if (Result.size() == 0)
        Result.resize(self_height * self_width);

    int i = 0;
    for (int Y = Has_Border; Y < self_height - Has_Border; Y++){

        int Row_Index = Y - Has_Border;

        // Calculate on what row we are on.
        int This_Text_Row_Length = (Text.size() - (Row_Index * self_width));

        //check if this row of text reaches tot he two ends of this wrapped width.
        if (This_Text_Row_Length > 0){
            //This means that the text has enough size to take all space on this row.
            for (int X = Has_Border; X < self_width - Has_Border; X++){
                if (i < Text.size()){
                    Result[Y * self_width + X] = Text[i++];
                }
            }
        }
        else{
            //This means we have to center the text here
            int Starting_X = Width_Center - This_Text_Row_Length / 2;

            for (int X = Has_Border; X < This_Text_Row_Length - Has_Border; X++){       
                if (i < Text.size()){
                    Result[Y * self_width + X + Starting_X] = Text[i++];
                }
            }
        }
    }
}

void GGUI::Text_Field::Left_Text(GGUI::Element* self, std::string Text, GGUI::Element* wrapper, std::vector<GGUI::UTF>& Result){
    int self_width = self->Get_Width();
    int self_height = self->Get_Height();

    bool Has_border = self->Has_Border();

    if (Result.size() == 0)
        Result.resize(self_height * self_width);

    std::vector<int> New_Line_Indicies;

    for (int i = 0; i < Text.size(); i++){

        if (Text[i] == '\n'){
            New_Line_Indicies.push_back(i);

            Text.erase(Text.begin() + i--);
        }
    }

    std::reverse(New_Line_Indicies.begin(), New_Line_Indicies.end());

    int i = 0;
    for (int Y = Has_border; Y < (self_height - Has_border); Y++){
        for (int X = Has_border; X < (self_width - Has_border); X++){
            if (i < Text.size()){

                if (New_Line_Indicies.size() > 0 && i == New_Line_Indicies.back()){
                    New_Line_Indicies.pop_back();
                    break;  //close the x loop and increase the Y loop.
                }

                int Character_Lenght = GGUI::Get_Unicode_Length(Text[i]);

                if (Character_Lenght == 1){
                    Result[Y * self_width + X] = Text[i];
                }
                else{
                    Result[Y * self_width + X] = Text.substr(0, Character_Lenght);
                }

                i += Character_Lenght;
            }
        }

        //every wrapped line acts like a newline so delete the next newline
        if (New_Line_Indicies.size() > 0 && i == New_Line_Indicies.back()){
            New_Line_Indicies.pop_back();
        }
    }
}

void GGUI::Text_Field::Right_Text(GGUI::Element* self, std::string Text, GGUI::Element* wrapper, std::vector<GGUI::UTF>& Result){
    int self_width = self->Get_Width();
    int self_height = self->Get_Height();

    bool Has_border = self->Has_Border();

    if (Result.size() == 0)
        Result.resize(self_height * self_width);

    std::vector<int> New_Line_Indicies;

    for (int i = 0; i < Text.size(); i++){

        if (Text[i] == '\n'){
            New_Line_Indicies.push_back(i);

            Text.erase(Text.begin() + i--);
        }
    }

    std::reverse(New_Line_Indicies.begin(), New_Line_Indicies.end());

    int i = 0;
    for (int Y = Has_border; Y < (self_height - Has_border); Y++){
        for (int X = Has_border; X < (self_width - Has_border); X++){
            if (i < Text.size()){

                if (New_Line_Indicies.size() > 0 && i == New_Line_Indicies.back()){
                    New_Line_Indicies.pop_back();
                    break;  //close the x loop and increase the Y loop.
                }

                int Reversed_X = self_width - X - 1;

                Result[Y * self_width + Reversed_X] = Text[i++];
            }
        }

        //every wrapped line acts like a newline so delete the next newline
        if (New_Line_Indicies.size() > 0 && i == New_Line_Indicies.back()){
            New_Line_Indicies.pop_back();
        }
    }
}

void GGUI::Text_Field::Set_Data(std::string Data){
    this->Data = Data;

    std::pair<unsigned int, unsigned int> dim = Get_Text_Dimensions(Data);

    GGUI::STAIN_TYPE flags = STAIN_TYPE::DEEP;

    if (dim.first > Width || dim.second > Height){
        flags = (GGUI::STAIN_TYPE)(flags | STAIN_TYPE::STRETCH);

        Width = dim.first;
        Height = dim.second;
    }

    Dirty.Dirty(flags);
    Update_Frame();
}

std::string GGUI::Text_Field::Get_Data(){
    return Data;
}

void GGUI::Text_Field::Set_Text_Position(TEXT_LOCATION Text_Position){
    Style->Text_Position = (int)Text_Position;
    Dirty.Dirty(STAIN_TYPE::DEEP);
    Update_Frame();
}

GGUI::TEXT_LOCATION GGUI::Text_Field::Get_Text_Position(){
    return (TEXT_LOCATION)Style->Text_Position.Value;
}

std::string GGUI::Text_Field::Get_Name() const {
    return "Text_Field<" + Name + ">";
}

void GGUI::Text_Field::Input(std::function<void(char)> Then){
    Allow_Text_Input = true;    

    Action* addr = new Action(
        Constants::KEY_PRESS,
        [=](GGUI::Event* e){
            if (Focused && Allow_Text_Input){
                //We know the event was gifted as Input*
                GGUI::Input* input = (GGUI::Input*)e;

                //First 
                Then(input->Data);
                Update_Frame();

                return true;
            }
            //action failed.
            return false;
        },
        this
    );
    GGUI::Event_Handlers.push_back(addr);

    Action* enter = new Action(
        Constants::ENTER,
        [=](GGUI::Event* e){
            if (Focused && Allow_Text_Input){
                //We know the event was gifted as Input*
                GGUI::Input* input = (GGUI::Input*)e;

                //First 
                Then(input->Data);
                Update_Frame();

                return true;
            }
            //action failed.
            return false;
        },
        this
    );
    GGUI::Event_Handlers.push_back(enter);

    Action* back_space = new Action(
        Constants::BACKSPACE,
        [=](GGUI::Event* e){
            if (Focused && Allow_Text_Input){
                
                if (Data.size() > 0){
                    Data.pop_back();
                    Dirty.Dirty(STAIN_TYPE::DEEP | STAIN_TYPE::EDGE);
                    Update_Frame();

                }

                return true;
            }
            //action failed.
            return false;
        },
        this
    );
    GGUI::Event_Handlers.push_back(back_space);
}

void GGUI::Text_Field::Enable_Text_Input(){
    //Check if an event handler of this caliber was launched already on previus text input enabling.
    for (auto& e : GGUI::Event_Handlers){
        if (e->Host == this && e->Criteria == Constants::KEY_PRESS){
            return;
        }
    }

    //If there was no event handlers for this job type, then make new ones.
    Input([=](char input){
        std::string tmp_Data = Data;
        tmp_Data.push_back(input);

        std::pair<unsigned int, unsigned int> Dimensions = Get_Text_Dimensions(tmp_Data);

        if (Dimensions.first > Width - (Has_Border() * 2) || Dimensions.second > Height - (Has_Border() * 2)){

            std::pair<unsigned int, unsigned int> max_dimensions = this->Parent->Get_Fitting_Dimensions(this);
            if (Style->Allow_Overflow.Value){
                Data.push_back(input);
                Dirty.Dirty(STAIN_TYPE::DEEP | STAIN_TYPE::EDGE);
                Update_Frame();
            }
            else if (Style->Allow_Dynamic_Size.Value){

                //check what to grow.
                if (Dimensions.first > Width - (Has_Border() * 2)){
                    Width++;
                }
                else if (Dimensions.second > Height - (Has_Border() * 2)){
                    Height++;
                }
                
                Data.push_back(input);
                Dirty.Dirty(STAIN_TYPE::DEEP | STAIN_TYPE::STRETCH | STAIN_TYPE::EDGE | STAIN_TYPE::COLOR);
                Update_Frame();
            }
            else{
                Report(
                    "The text field is too small to fit the text. Either increase the size of the text field or allow dynamic size."
                );
            }
        }
        else{                
            Data.push_back(input);
            Dirty.Dirty(STAIN_TYPE::DEEP | STAIN_TYPE::EDGE);
            Update_Frame();
        }
    });
}

void GGUI::Text_Field::Disable_Text_Input(){
    Allow_Text_Input = false;
    Update_Frame();
}

void GGUI::Text_Field::Enable_Input_Overflow(){

    Style->Allow_Overflow = true;
}

void GGUI::Text_Field::Disable_Input_Overflow(){

    Style->Allow_Overflow = false;
}

void GGUI::Text_Field::Enable_Dynamic_Size(){

    Style->Allow_Overflow = true;
}

void GGUI::Text_Field::Disable_Dynamic_Size(){

    Style->Allow_Overflow = false;
}

GGUI::Element* Translate_Text_Fields(GGUI::HTML_Node* input){
    GGUI::Text_Field* Result = new GGUI::Text_Field();

    // Parse the following information given by the HTML_NODE:
    // - Childs Recursive Nesting
    // |-> Parent Linking
    // - Position written inheriting
    // - RAW ptr set to get link to origin  (no need to do anything)
    // - Type (no need to do anything)
    // - Attribute parsing: Styles, Width, Height, BG_Color, Front_Color, Border, Border color, etc.. (All CSS attributes)

    std::string Text = "";

    GGUI::Translate_Childs_To_Element(Result, input, &Text);
    
    // Since the translate childs to element put the name as the raw text inside it, we need to redirect it to the Data member.
    Result->Set_Data(Text);

    // now reset the name to be same as the 
    Result->Set_Name(std::to_string((unsigned long long)Result));

    GGUI::Translate_Attributes_To_Element(Result, input);

    return Result;
}

GGUI_Add_Translator("dt", Translate_Text_Fields);
GGUI_Add_Translator("dd", Translate_Text_Fields);
GGUI_Add_Translator("label", Translate_Text_Fields);
GGUI_Add_Translator("textarea", Translate_Text_Fields);
GGUI_Add_Translator("title", Translate_Text_Fields);

