#include "Text_Field.h"


std::vector<GGUI::UTF> GGUI::Text_Field::Render(){
    std::vector<GGUI::UTF> Result = Element::Render();

    //make a smaller buffer for the inner text.
    std::vector<GGUI::UTF> Text_Buffer;

    std::pair<int, int> Text_Dimensions = Get_Text_Dimensions(Data);
    Text_Buffer.resize(Text_Dimensions.first * Text_Dimensions.second);

    switch (Text_Position)
    {
    case TEXT_LOCATION::CENTER:
        Center_Text(Text_Buffer, Width, Height);
        break;
    
    default:
        break;
    }
}


std::pair<int, int> GGUI::Text_Field::Get_Text_Dimensions(std::string text){
    //calculate the most longest row of text and set it as the width
    int longest_row = 0;
    int Current_Row = 0;

    int longest_Height = 0;
    for (auto& c : text){
        if (c == '\n'){
            if (Current_Row > longest_row)
                longest_row = Current_Row;
            
            Current_Row = 0;
            longest_Height++;
        }
        else{
            Current_Row++;
        }
    }

    return {longest_row, longest_Height};
}

//The Text buffer needs to contain already left centered text.
void GGUI::Text_Field::Center_Text(std::vector<UTF>& Text, int width, int height){



}

