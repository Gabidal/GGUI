#include "Text_Field.h"
#include "../Renderer.h"

namespace GGUI{

    // Called when text_field has a deep stain
    void Text_Field::Update_Text_Cache(){

        Text_Cache.clear();

        if (Text_Cache.capacity() < SETTINGS::Text_Field_Minimum_Line_Count){
            // This should not happen
            Report_Stack("Internal error with zero capacity in: " + this->Get_Name());
        }

        // Will determine the text cache list by newlines, and if no found then set the Text as the zeroth index.
        Compact_String current_line(Text.data(), 0);

        for (unsigned int i = 0; i < Text.size(); i++){

            if (Text[i] == '\n'){
                // Newlines are not counted as line lengths
                Text_Cache.push_back(current_line);

                // reset current
                current_line = Compact_String(Text.data() + i + 1, 0);
            }
            else{
                current_line.Size++;
            }

        }

        // Make sure the last line is added
        if (current_line.Size > 0){
            Text_Cache.push_back(current_line);
        }

    }

    std::vector<UTF> Text_Field::Render(){
        std::vector<GGUI::UTF> Result = Render_Buffer;

        //if inned children have changed without this changing, then this will trigger.
        if (Children_Changed() || Has_Transparent_Children()){
            Dirty.Dirty(STAIN_TYPE::DEEP | STAIN_TYPE::STRETCH);
        }

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

        //This will add the child windows to the Result buffer
        if (Dirty.is(STAIN_TYPE::DEEP)){
            Dirty.Clean(STAIN_TYPE::DEEP);

            if (Style->Align.Value == ALIGN::LEFT)
                Align_Text_Left(Result);
            else if (Style->Align.Value == ALIGN::RIGHT)
                Align_Text_Right(Result);
            else if (Style->Align.Value == ALIGN::CENTER)
                Align_Text_Center(Result);

        }

        //This will add the borders if necessary and the title of the window.
        if (Dirty.is(STAIN_TYPE::EDGE))
            Add_Overhead(this, Result);

        Render_Buffer = Result;

        return Result;
    }

    void Text_Field::Align_Text_Left(std::vector<UTF>& Result){
        for (Compact_String line : Text_Cache){
            int Line_Index = 0;

            for (int Y = 0; Y < Height; Y++){
                for (int X = 0; X < Width; X++){

                    if (Y * Width + X >= Line_Index)
                        goto Next_Line;

                    // write to the Result
                    Result[Y * Width + X] = line[Line_Index++];
                }

                // If current line has ended and the text is not word wrapped
                if (!Style->Allow_Overflow.Value)
                    goto Next_Line; // An break would suffice but use goto for more readability

            }

            Next_Line:;
        }
    }

    void Text_Field::Align_Text_Right(std::vector<UTF>& Result){
        for (Compact_String line : Text_Cache){
            int Line_Index = line.Size - 1;  // Start from the end of the line

            for (int Y = 0; Y < Height; Y++){
                for (int X = Width - 1; X >= 0; X--){

                    if (Line_Index < 0)  // If there are no more characters in the line
                        goto Next_Line;

                    // write to the Result
                    Result[Y * Width + X] = line[Line_Index--];  // Decrement Line_Index
                }

                // If current line has ended and the text is not word wrapped
                if (!Style->Allow_Overflow.Value)
                    goto Next_Line; // An break would suffice but use goto for more readability
            }

            Next_Line:;
        }
    }

    void Text_Field::Align_Text_Center(std::vector<UTF>& Result){
        for (Compact_String line : Text_Cache){
            int Line_Index = 0;  // Start from the beginning of the line
            int Start_Pos = (Width - line.Size) / 2;  // Calculate the starting position

            for (int Y = 0; Y < Height; Y++){
                for (int X = 0; X < Width; X++){

                    if (X < Start_Pos || Line_Index >= line.Size)  // If the current position is before the start or after the end of the line
                        continue;

                    // write to the Result
                    Result[Y * Width + X] = line[Line_Index++];  // Increment Line_Index
                }

                // If current line has ended and the text is not word wrapped
                if (!Style->Allow_Overflow.Value)
                    break;
            }
        }
    }


}