#include "Text_Field.h"
#include "../Renderer.h"

namespace GGUI{

    // Called when text_field has a deep stain
    void Text_Field::Update_Text_Cache(){

        Text_Cache.clear();

        if (Text_Cache.capacity() < 1){
            // This should not happen
            Report_Stack("Internal error with zero capacity in: " + this->Get_Name());
        }

        // Will determine the text cache list by newlines, and if no found then set the Text as the zeroth index.
        Compact_String current_line(Text.data(), 0, true);
        unsigned int Longest_Line = 0;

        for (unsigned int i = 0; i < Text.size(); i++){
            bool flush_row = false;

            if (Text[i] == '\n'){
                // Newlines are not counted as line lengths
                flush_row = true;
            }
            else if (Text[i] == ' '){
                // This is for the word wrapping to beautifully end at when word end and not abruptly
                
                // Check if the current line length added one more word would go over the Width
                // For this we first need to know how long is this next word if there is any
                int New_Word_Length = Text.find_first_of(' ', i + 1) - i;

                if (New_Word_Length + current_line.Size >= Width && !Style->Allow_Dynamic_Size.Value){
                    // We want to add the space to this row
                    current_line.Size++;

                    flush_row = true;
                }
            }

            if (flush_row){
                // If the next word would go over the Width then add the current line to the Text_Cache
                Text_Cache.push_back(current_line);

                // check if the current line is longer than the longest line
                if (current_line.Size > Longest_Line)
                    Longest_Line = current_line.Size;

                // reset current
                current_line = Compact_String(Text.data() + i + 1, 0, true);
            }
            else{
                current_line.Size++;
            }
        }

        // Make sure the last line is added
        if (current_line.Size > 0){
            // check if this new word could be added to the last line
            if (Text_Cache.size() == 0 || current_line.Size + Text_Cache.back().Size >= Width){
                // If not then add the current line to the Text_Cache
                Text_Cache.push_back(current_line);

                Longest_Line = Max(Longest_Line, current_line.Size);
            }
            else{
                // If it can be added then add it to the last line
                Text_Cache.back().Size += current_line.Size;

                Longest_Line = Max(Longest_Line, Text_Cache.back().Size);
            }
        }

        // now we need to go through each compact string and make sure that those that are enforced as unicode's but are still 1 long, need to be transformed into the char bearing.
        for (Compact_String& line : Text_Cache){
            // We need to take care of the "Force Unicode" shenanigans before we add it to this list.
            if (line.Size == 1){
                line.Data.Ascii_Data = line.Data.Unicode_Data[0];
            }
        }

        // Now we can check if Dynamic size is enabled, if so then resize Text_Field by the new sizes
        if (Style->Allow_Dynamic_Size.Value){
            // Set the new size
            Set_Dimensions(Max(Longest_Line, Width), Max(Text_Cache.size(), Height));
        }
    }

    std::vector<UTF> Text_Field::Render(){
        std::vector<GGUI::UTF> Result = Render_Buffer;

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

    void Text_Field::Set_Size_To_Fill_Parent(){
        if (!Is_Dynamic_Size_Allowed())
            return;

        int New_Width, New_Height;

        if (Parent->Is_Dynamic_Size_Allowed()){
            // Since the parent can just stretch we can set the max size.
            New_Width = Text.size();
            New_Height = 1;
        }
        else{
            New_Width = Min(Parent->Get_Width() - Position.X, Text.size());
            Update_Text_Cache();    // re-calculate the new height with the new suggested width.
            New_Height = Min(Parent->Get_Height() - Position.Y, Text_Cache.size());
        }
        
        Set_Dimensions(New_Width, New_Height);
    }

    void Text_Field::Set_Parent(Element* parent){
        if (parent){
            Parent = parent;

            Set_Size_To_Fill_Parent();
        }
    }

    void Text_Field::Set_Position(Coordinates c){
        Position = c;

        this->Dirty.Dirty(STAIN_TYPE::MOVE);

        Set_Size_To_Fill_Parent();
    }

    void Text_Field::Set_Text(std::string text){
        Text = text;
        Update_Text_Cache();

        Dirty.Dirty(STAIN_TYPE::DEEP | STAIN_TYPE::STRETCH);

        Update_Frame();
    }

    void Text_Field::Align_Text_Left(std::vector<UTF>& Result){
        unsigned int Line_Index = 0;    // To keep track of the inter-line rowing.

        for (Compact_String line : Text_Cache){
            unsigned int Row_Index = 0;

            if (Line_Index >= Height)
                break;  // All possible usable lines filled.

            for (unsigned int Y = 0; Y < Height; Y++){
                for (unsigned int X = 0; X < Width; X++){

                    if (Y * Width + X >= line.Size)
                        goto Next_Line;

                    // write to the Result
                    Result[(Y + Line_Index) * Width + X] = line[Row_Index++];
                }

                // If current line has ended and the text is not word wrapped
                if (Style->Allow_Overflow.Value)
                    goto Next_Line; // An break would suffice but use goto for more readability

            }

            Next_Line:;
            Line_Index++;
        }
    }

    void Text_Field::Align_Text_Right(std::vector<UTF>& Result){        
        unsigned int Line_Index = 0;    // To keep track of the inter-line rowing.

        for (Compact_String line : Text_Cache){
            int Row_Index = line.Size - 1;  // Start from the end of the line

            if (Line_Index >= Height)
                break;  // All possible usable lines filled.

            for (unsigned int Y = 0; Y < Height; Y++){
                for (int X = (signed)Width - 1; X >= 0; X--){

                    if (Row_Index < 0)  // If there are no more characters in the line
                        goto Next_Line;

                    // write to the Result
                    Result[(Y + Line_Index) * Width + (unsigned)X] = line[Row_Index--];  // Decrement Line_Index
                }

                // If current line has ended and the text is not word wrapped
                if (Style->Allow_Overflow.Value)
                    goto Next_Line; // An break would suffice but use goto for more readability
            }

            Next_Line:;
            Line_Index++;
        }
    }

    void Text_Field::Align_Text_Center(std::vector<UTF>& Result){
        unsigned int Line_Index = 0;    // To keep track of the inter-line rowing.

        for (Compact_String line : Text_Cache){
            unsigned int Row_Index = 0;  // Start from the beginning of the line
            unsigned int Start_Pos = (Width - line.Size) / 2;  // Calculate the starting position

            if (Line_Index >= Height)
                break;  // All possible usable lines filled.

            for (unsigned int Y = 0; Y < Height; Y++){
                for (unsigned int X = 0; X < Width; X++){

                    if (X < Start_Pos || X > Start_Pos + line.Size)
                        continue;

                    if (Y * Width + X >= line.Size)
                        goto Next_Line;

                    // write to the Result
                    Result[(Y + Line_Index) * Width + X] = line[Row_Index++];
                }

                // If current line has ended and the text is not word wrapped
                if (Style->Allow_Overflow.Value)
                    goto Next_Line; // An break would suffice but use goto for more readability
            }

            Next_Line:;
            Line_Index++;
        }
    }


}