#include "Text_Field.h"
#include "../Renderer.h"


std::vector<GGUI::UTF> GGUI::Text_Field::Render(){
    std::vector<GGUI::UTF> Result;

    if (Dirty || (Previus_Render_Buffer.size() == 0 && Data.size() > 0)){
        Result.resize(this->Width * this->Height);

        //make a smaller buffer for the inner text.
        std::vector<GGUI::UTF> Text_Buffer;

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

        GGUI::Nest_UTF_Text(this, this, Text_Buffer, Result);

        Apply_Colors(this, Result);

        Add_Overhead(this, Result);

        Previus_Render_Buffer = Result;
    }
    else{
        Result = Previus_Render_Buffer;
    }

    Dirty = false;
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
    Update_Frame();
}

std::pair<int, int> GGUI::Text_Field::Get_Text_Dimensions(std::string text){
    //calculate the most longest row of text and set it as the width
    int longest_row = 0;
    int Current_Row = 0;

    int longest_Height = 1;
    for (auto& c : text){
        if (c == '\n'){
            Current_Row = 0;
            longest_Height++;
        }
        else{
            Current_Row++;

            if (Current_Row > longest_row)
                longest_row = Current_Row;    
        }
    }

    return {longest_row, longest_Height};
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
                Result[Y * self->Width + X] = Text[i++];
            }
        }
        else{
            //This means we have to center the text here
            int Starting_X = Width_Center - This_Text_Row_Length / 2;

            for (int X = 0; X < This_Text_Row_Length; X++){
                Result[Y * self->Width + X + Starting_X] = Text[i++];
            }
        }
    }

    return Result;
}

std::vector<GGUI::UTF> GGUI::Text_Field::Left_Text(GGUI::Element* self, std::string Text, GGUI::Element* wrapper){
    std::vector<GGUI::UTF> Result; 

    bool Has_border = self->Has_Border();

    Result.resize(self->Height * self->Width);

    int i = 0;
    for (int Y = Has_border; Y < self->Height - Has_border; Y++){
        for (int X = Has_border; X < self->Width -Has_border; X++){
            Result[Y * self->Width + X] = Text[i++];
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
            int Reverse_X = self->Width - X - 1;
            Result[Y * self->Width + Reverse_X] = Text[i++];
        }
    }
    
    return Result;
}

void GGUI::Text_Field::Set_Data(std::string Data){
    this->Data = Data;
    Dirty = true;
    Update_Frame();
}

std::string GGUI::Text_Field::Get_Data(){
    return Data;
}

void GGUI::Text_Field::Set_Text_Position(TEXT_LOCATION Text_Position){
    this->Text_Position = Text_Position;
    Dirty = true;
    Update_Frame();
}

GGUI::TEXT_LOCATION GGUI::Text_Field::Get_Text_Position(){
    return Text_Position;
}



