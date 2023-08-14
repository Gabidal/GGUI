#include "HTML.h"


namespace GGUI{
    HTML::HTML(std::string File_Name){
        Handle = new FILE_STREAM(File_Name, [&](){
            this->Set_Childs(Parse_HTML(Handle->Fast_Read()));
        });
    }

    void Parse(std::vector<HTML_Token*>& Input){
        for (int i = 0; i < Input.size(); i++){
            Parse_Embedded_Bytes(i, Input);
            Parse_All_Wrappers(i, Input);
        }
    }

    std::vector<Element*> Parse_HTML(std::string Raw_Buffer){
        std::vector<HTML_Token*> Lexed_Tokens = Lex_HTML(Raw_Buffer);

        Parse(Lexed_Tokens);
    }

    std::vector<HTML_Token*>& Parse_HTML(std::vector<HTML_Token*>& Input){
        Parse(Input);

        return Input;
    }

    void Parse_Embedded_Bytes(int& i, std::vector<HTML_Token*>& Input){
        // This function check if the input at i contains '/' which then marks the following tokens to be skipped
        // Example "absc /" a /" "


    }

    void Parse_All_Wrappers(int& i, std::vector<HTML_Token*>& Input){
        Parse_Wrapper("<", ">", i, Input);
        Parse_Wrapper("[", "]", i, Input);
        Parse_Wrapper("{", "}", i, Input);
        Parse_Wrapper("(", ")", i, Input);
        Parse_Wrapper("\"", "\"", i, Input);
        Parse_Wrapper("\'", "\'", i, Input);
    }

    void Parse_Wrapper(std::string start_pattern, std::string end_pattern, int& i, std::vector<HTML_Token*>& Input){
        // This function starts from the given index and every time it finds a start pattern it starts a new loop from the start pattern index until the end pattern count hits 0 and puts all of the tokens between the start and end pattern into the childs.
        if (i < 0)
            return; // Check for instance if the previous pattern already annihilated all the leftover tokens :)
        
        if (Input[i]->Data != start_pattern)
            return;

        int Nested_Count = 1;   // Remember to count the start pattern too.
        int End_Index = i;

        for (End_Index++; End_Index < Input.size() && Nested_Count > 0; End_Index++){

            if (Input[End_Index]->Data == start_pattern){
                Parse_Wrapper(start_pattern, end_pattern, End_Index, Input);
            }

            if (Input[End_Index]->Data == end_pattern)
                Nested_Count--;

            // now check if the nested count is zero
            if (Nested_Count == 0){
                // First cut the list
                std::vector<HTML_Token*> Cut;
                Cut.insert(Cut.begin(), Input.begin() + i + 1, Input.begin() + End_Index - 1);

                Input[i]->Childs = Parse_HTML(Cut);

                // Now delete the tokens we just cut
                Input.erase(Input.begin() + i + 1, Input.begin() + End_Index - 1);

                // Now set the index to the end index
                i = End_Index;
            }
        }
    
    
        // if the nested count is still above 0 even after looping through all the tokens, just ignore lolw.
    }

    const std::vector<HTML_Group> Groups = {
        {HTML_GROUP_TYPES::NUMBER, 48, 57},             // All numbers
        {HTML_GROUP_TYPES::TEXT, 65, 90},               // Capital letters 
        {HTML_GROUP_TYPES::TEXT, 97, 122},              // Lowercase letters
        {HTML_GROUP_TYPES::TEXT, 95, 95},               // '_'
        {HTML_GROUP_TYPES::OPERATOR, 33, 47, false},    // ! upto /
        {HTML_GROUP_TYPES::OPERATOR, 58, 64, false},    // : upto @
        {HTML_GROUP_TYPES::OPERATOR, 91, 94, false},    // [ upto ^
        {HTML_GROUP_TYPES::OPERATOR, 123, 126, false},  // { upto ~
        {HTML_GROUP_TYPES::SPACING, 10, 10, false},     // newline
        {HTML_GROUP_TYPES::SPACING, 32, 32, false},     // Space
        {HTML_GROUP_TYPES::SPACING, 9, 9, false},       // Tabulator
    };

    void Sanitize_HTML_Tokens(std::vector<HTML_Token*>& Un_Sanitized){
        // If there are multiple newlines or spaces ot tabulators combine them into one.
        std::string Previous_Data = "";

        const std::vector<std::string> Clampable_Types = {
            "\n", " ", "\t"
        };

        for (int i = 0; i < Un_Sanitized.size(); i++){
            // First check if the current token is similiar to the previous token or not.
            if (Un_Sanitized[i]->Data != Previous_Data){
                Previous_Data = Un_Sanitized[i]->Data;
                continue;
            }

            // Check if the current token is one of the clampables.
            for (auto clampable : Clampable_Types){
                if (Un_Sanitized[i]->Data != clampable)
                    continue;

                Un_Sanitized.erase(Un_Sanitized.begin() + i--);

                break;
            }
        }
    }

    std::vector<HTML_Token*> Lex_HTML(std::string Raw_Buffer){
        // Slice the given text into HTML_Tokens.
        // Where text:
        /*
            <dev>
                <le>1.#!a</le>
                <le>2</le>
                <le>3</le>
            </dev>
        */
        std::vector<HTML_Token*> Result;
        HTML_Token* Current_Token = new HTML_Token();
        FILE_POSITION Current_Position = FILE_POSITION("unknown", 0, 0);

        for (auto Current_Char : Raw_Buffer){

            bool All_Characters_Matched = false;
            // First detect the current char type.
            for (auto Group : Groups){
                // start <= char >= end
                if (Group.Start > Current_Char || Group.End < Current_Char)
                    continue;

                All_Characters_Matched = true;

                // New group detected
                if (Current_Token->Type != Group.Type || !Group.Is_Sticky){
                    Result.push_back(Current_Token);

                    Current_Token = new HTML_Token(Group.Type, Current_Char, Current_Position);
                }
                // Same group detected
                else{
                    Current_Token->Data.push_back(Current_Char);
                }

                break;
            }
            
            if (!All_Characters_Matched){
                Result.push_back(Current_Token);

                Current_Token = new HTML_Token(HTML_GROUP_TYPES::UNKNOWN, Current_Char, Current_Position);
            }

            Current_Position.Character++;

            if (Current_Char == '\n'){
                Current_Position.Line_Number++;
                Current_Position.Character = 0;
            }
        }

        // Make sure that the last currently processing token is also flushed to the resulting list.
        Result.push_back(Current_Token);

        Sanitize_HTML_Tokens(Result);

        return Result;
    }
}