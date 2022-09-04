#include "Text_Field.h"
#include "../Renderer.h"


std::vector<GGUI::UTF> GGUI::Text_Field::Render(){
    std::vector<GGUI::UTF> Result = Render_Buffer;

    if (Dirty.is(STAIN_TYPE::STRECH)){
        Result.clear();
        Result.resize(this->Width * this->Height);
        Dirty.Clean(STAIN_TYPE::STRECH);
    }

    //make a smaller buffer for the inner text.
    std::vector<GGUI::UTF> Text_Buffer;

    if (Dirty.is(STAIN_TYPE::TEXT)){
        Dirty.Clean(STAIN_TYPE::TEXT);
        switch (Text_Position)
        {
        case TEXT_LOCATION::CENTER:
            Text_Buffer = Center_Text(this, Data, Parent);
            break;
        case TEXT_LOCATION::LEFT:
            Text_Buffer = Left_Text(this, Data, Parent);
            break;
        case TEXT_LOCATION::RIGHT:
            Text_Buffer = Right_Text(this, Data, Parent);
            break;
        
        default:
            break;
        }

        Result = Text_Buffer;
    }

    if (Dirty.is(STAIN_TYPE::COLOR))
        Apply_Colors(this, Result);

    if (Dirty.is(STAIN_TYPE::EDGE))
        Add_Overhead(this, Result);

    Render_Buffer = Result;

    return Result;
}

void GGUI::Text_Field::Show_Border(bool state){
    if (state && Border == false){
        //enlarge the text field area to compas the borders
        Width += 2;
        Height += 2;
    }
    else if (!state && Border == true){
        //shrink the text field area to compas the borders
        Width -= 2;
        Height -= 2;
    }
    
    Border = state;
    Dirty.Dirty(STAIN_TYPE::EDGE);
}

std::pair<int, int> GGUI::Text_Field::Get_Text_Dimensions(std::string& text){
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

    Dirty.Dirty(STAIN_TYPE::STRECH);

    return true;
}

//The Text buffer needs to contain already left centered text.
std::vector<GGUI::UTF> GGUI::Text_Field::Center_Text(GGUI::Element* self, std::string Text, GGUI::Element* wrapper){
    std::vector<GGUI::UTF> Result; 

    int Width_Center = self->Width / 2;

    Result.resize(self->Height * self->Width);

    int i = 0;
    for (int Y = 0; Y < self->Height; Y++){

        int This_Text_Row_Length = (Text.size() - (Y * self->Width));

        //check if this row of text reaches tot he two ends of this wrapped width.
        if (This_Text_Row_Length > 0){
            //This means that the text has enough size to take all space on this row.
            for (int X = 0; X < self->Width; X++){
                if (i < Text.size()){
                    Result[Y * self->Width + X] = Text[i++];
                }
            }
        }
        else{
            //This means we have to center the text here
            int Starting_X = Width_Center - This_Text_Row_Length / 2;

            for (int X = 0; X < This_Text_Row_Length; X++){       
                if (i < Text.size()){
                    Result[Y * self->Width + X + Starting_X] = Text[i++];
                }
            }
        }
    }

    return Result;
}

std::vector<GGUI::UTF> GGUI::Text_Field::Left_Text(GGUI::Element* self, std::string Text, GGUI::Element* wrapper){
    std::vector<GGUI::UTF> Result; 

    bool Has_border = self->Has_Border() * 2;

    Result.resize(self->Height * self->Width);

    int i = 0;
    for (int Y = Has_border; Y < self->Height - Has_border; Y++){
        for (int X = Has_border; X < self->Width -Has_border; X++){
            if (i < Text.size()){
                Result[Y * self->Width + X] = Text[i++];
            }
        }
    }
    
    return Result;
}

std::vector<GGUI::UTF> GGUI::Text_Field::Right_Text(GGUI::Element* self, std::string Text, GGUI::Element* wrapper){
    std::vector<GGUI::UTF> Result; 

    Result.resize(self->Height * self->Width);

    int i = 0;
    for (int Y = 0; Y < self->Height; Y++){
        for (int X = 0; X < self->Width; X++){
            if (i < Text.size()){
                int Reverse_X = self->Width - X - 1;
                Result[Y * self->Width + Reverse_X] = Text[i++];
            }
        }
    }
    
    return Result;
}

void GGUI::Text_Field::Set_Data(std::string Data){
    this->Data = Data;
    Dirty.Dirty(STAIN_TYPE::TEXT);
}

std::string GGUI::Text_Field::Get_Data(){
    return Data;
}

void GGUI::Text_Field::Set_Text_Position(TEXT_LOCATION Text_Position){
    this->Text_Position = Text_Position;
    Dirty.Dirty(STAIN_TYPE::TEXT);
}

GGUI::TEXT_LOCATION GGUI::Text_Field::Get_Text_Position(){
    return Text_Position;
}

std::string GGUI::Text_Field::Get_Name(){
    return "Text_Field";
}

GGUI::Element* GGUI::Text_Field::Copy(){
    Text_Field* new_element = new Text_Field();

    *new_element = *this;

    //now also update the event handlers.
    for (auto& e : GGUI::Event_Handlers){

        if (e->Host == this){
            //copy the event and make a new one
            Action* new_action = new Action(*e);

            //update the host
            new_action->Host = new_element;

            //add the new action to the event handlers list
            GGUI::Event_Handlers.push_back(new_action);
        }

    }

    return new_element;
}




