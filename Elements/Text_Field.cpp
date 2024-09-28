#include "Text_Field.h"
#include "../Core/Renderer.h"

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

        // This is for the remaining liners to determine if they can append into the previous line or not.
        enum class Line_Reason {
            NONE,
            NEWLINE,
            WORDWRAP
        } Previous_Line_Reason = Line_Reason::NONE;

        for (unsigned int i = 0; i < Text.size(); i++){
            bool flush_row = false;

            if (Text[i] == '\n'){
                // Newlines are not counted as line lengths
                flush_row = true;
                Previous_Line_Reason = Line_Reason::NEWLINE;
            }
            else{   // NOTE: If there is a newline character we need to TOTALLY skip it!!!
                // Since in all situations the delimeter is also wanted to be part of the current line, we need to increase the current line length before deciding if we want to add it.
                current_line.Size++;
            }
            
            // This is for the word wrapping to beautifully end at when word end and not abruptly
            if (Text[i] == ' ' && !Style->Allow_Dynamic_Size.Value){    
                // Check if the current line length added one more word would go over the Width
                // For this we first need to know how long is this next word if there is any
                int New_Word_Length = Text.find_first_of(' ', i + 1) - i;

                if (New_Word_Length + current_line.Size >= Get_Width()){
                    flush_row = true;
                    Previous_Line_Reason = Line_Reason::WORDWRAP;
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
        }

        // Make sure the last line is added
        if (current_line.Size > 0){

            bool Last_Line_Exceeds_Width_With_Current_Line = Text_Cache.size() > 0 && Text_Cache.back().Size >= Get_Width();

            // Add the remaining liners if: There want any previous lines OR the last line exceeds the width with the current line OR the previous line ended with a newline.
            if (
                Text_Cache.size() == 0 ||
                (
                    Last_Line_Exceeds_Width_With_Current_Line &&
                    !Style->Allow_Dynamic_Size.Value
                ) ||
                Previous_Line_Reason == Line_Reason::NEWLINE
            ){
                // If not then add the current line to the Text_Cache
                Text_Cache.push_back(current_line);
            }
            else{
                // If it can be added then add it to the last line
                Text_Cache.back().Size += current_line.Size;
            }

            Longest_Line = Max(Longest_Line, Text_Cache.back().Size);
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
            Set_Width(Max(Longest_Line, Get_Width()));
            Set_Height(Max(Text_Cache.size(), Get_Height()));
        }
    }

    std::vector<GGUI::UTF>&  Text_Field::Render(){
        std::vector<GGUI::UTF>& Result = Render_Buffer;

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
            Result.resize(Get_Width() * Get_Height(), SYMBOLS::EMPTY_UTF);
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
            New_Width = Min(Parent->Get_Width() - Get_Position().X, Text.size());
            Update_Text_Cache();    // re-calculate the new height with the new suggested width.
            New_Height = Min(Parent->Get_Height() - Get_Position().Y, Text_Cache.size());
        }
        
        Set_Dimensions(New_Width, New_Height);
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

            if (Line_Index >= Get_Height())
                break;  // All possible usable lines filled.

            for (unsigned int Y = 0; Y < Get_Height(); Y++){
                for (unsigned int X = 0; X < Get_Width(); X++){

                    if (Y * Get_Width() + X >= line.Size)
                        goto Next_Line;

                    // write to the Result
                    Result[(Y + Line_Index) * Get_Width() + X] = line[Row_Index++];
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

            if (Line_Index >= Get_Height())
                break;  // All possible usable lines filled.

            for (unsigned int Y = 0; Y < Get_Height(); Y++){
                for (int X = (signed)Get_Width() - 1; X >= 0; X--){

                    if (Row_Index < 0)  // If there are no more characters in the line
                        goto Next_Line;

                    // write to the Result
                    Result[(Y + Line_Index) * Get_Width() + (unsigned)X] = line[Row_Index--];  // Decrement Line_Index
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
            unsigned int Start_Pos = (Get_Width() - line.Size) / 2;  // Calculate the starting position

            if (Line_Index >= Get_Height())
                break;  // All possible usable lines filled.

            for (unsigned int Y = 0; Y < Get_Height(); Y++){
                for (unsigned int X = 0; X < Get_Width(); X++){

                    if (X < Start_Pos || X > Start_Pos + line.Size)
                        continue;

                    if (Y * Get_Width() + X >= line.Size)
                        goto Next_Line;

                    // write to the Result
                    Result[(Y + Line_Index) * Get_Width() + X] = line[Row_Index++];
                }

                // If current line has ended and the text is not word wrapped
                if (Style->Allow_Overflow.Value)
                    goto Next_Line; // An break would suffice but use goto for more readability
            }

            Next_Line:;
            Line_Index++;
        }
    }


    void Text_Field::Input(std::function<void(char)> Then){
        Action* addr = new Action(
            Constants::KEY_PRESS,
            [this, Then](GGUI::Event* e){
                if (Focused){
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
            [this, Then](GGUI::Event* e){
                if (Focused){
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
            [this](GGUI::Event*){
                if (Focused){
                    
                    if (Text.size() > 0){
                        Text.pop_back();
                        Dirty.Dirty(STAIN_TYPE::DEEP);
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

}