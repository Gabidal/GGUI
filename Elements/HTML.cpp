#include "HTML.h"

namespace GGUI{
    PARSE_BY operator|(PARSE_BY first, PARSE_BY second){
        return (PARSE_BY)((unsigned long long)first | (unsigned long long)second);
    }

    PARSE_BY operator&(PARSE_BY first, PARSE_BY second){
        return (PARSE_BY)((unsigned long long)first & (unsigned long long)second);
    }

    void operator|=(PARSE_BY& first, PARSE_BY second){
        first = first | second; 
    }

    std::unordered_map<std::string, std::function<GGUI::Element* (HTML_Node*)>> HTML_Translators;

    HTML::HTML(std::string File_Name){
        Handle = new FILE_STREAM(File_Name, [&](){
            this->Set_Childs(Parse_HTML(Handle->Fast_Read()));
        });
    }

    void Parse(std::vector<HTML_Token*>& Input){
        // First combine wrappers like: <, >, (, ), etc...
        for (int i = 0; i < Input.size(); i++){
            Parse_Embedded_Bytes(i, Input);
            Parse_All_Wrappers(i, Input);
            Parse_Operator_Set(i, Input);
        }

        // now start combining dynamic wrappers like: <html>, </html>
        for (int i = 0; i < Input.size(); i++){
            // first try to find the first instance of < token.
            if (Input[i]->Data != "<" || Input[i]->Childs.size() == 0 || Input[i]->Childs[0]->Data == "!")
                continue;

            Parse_Dynamic_Wrappers(i, Input, Input[i]->Childs[0]->Data);
        }
    }

    std::vector<Element*> Parse_HTML(std::string Raw_Buffer){
        std::vector<HTML_Token*> Lexed_Tokens = Lex_HTML(Raw_Buffer);

        Parse(Lexed_Tokens);

        std::vector<HTML_Node*> Parsed_Tokens = Parse_Lexed_Tokens(Lexed_Tokens);

        return Parse_Translators(Parsed_Tokens);
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

    void Parse_Dynamic_Wrappers(int& i, std::vector<HTML_Token*>& Input, std::string word){
        if (i < 0)
            return;

        int End_Index = i;
        
        for (End_Index++; End_Index < Input.size(); End_Index++){

            std::string Current_Name = Input[End_Index]->Data;
            HTML_Token* Current_First_Child = nullptr;    // for </ or <TEXT  instances
            HTML_Token* Current_Last_Child = nullptr;     // for />           instances

            if (Current_Name != "<" || Input[End_Index]->Is(PARSE_BY::DYNAMIC_WRAPPER))
                continue;   // skip

            if (Input[End_Index]->Childs.size() > 0){
                Current_First_Child = Input[End_Index]->Childs[0];
                Current_Last_Child = Input[End_Index]->Childs[Input[End_Index]->Childs.size() -1];
            }

            // Check if the current element is an single liner
            if (Current_Last_Child->Data == "/"){
                // Dont need to do enything, since single tokens already have been wrapped by the previou process.
                // '<', 'TEXT', '/' Single line token
                Input[End_Index]->Data = Current_First_Child->Data; // Name the single line wrapper with the name.

                Input[End_Index]->Parsed_By |= PARSE_BY::DYNAMIC_WRAPPER;
            }
            // Check if the current End_Index points into another dynamic token wrapper.
            else if (Current_First_Child->Data != "/"){
                // '<', 'TEXT', ... means that this is an starter token.
                Parse_Dynamic_Wrappers(End_Index, Input, Current_First_Child->Data);
            }
            else{
                // This means that the token did not end with '/' and started with '/'
                HTML_Token* New_Wrapper = new HTML_Token(HTML_GROUP_TYPES::WRAPPER, word);

                std::vector<HTML_Token*> Cut;
                Cut.insert(Cut.begin(), Input.begin() + i + 1, Input.begin() + End_Index);

                New_Wrapper->Childs = Parse_HTML(Cut);

                // Now delete the tokens we just cut
                Input.erase(Input.begin() + i + 1, Input.begin() + End_Index + 1);

                // Replace the Input[i] with the new wrapper.
                Input[i] = New_Wrapper;

                // Now set the parsed by flag.
                Input[i]->Parsed_By |= PARSE_BY::DYNAMIC_WRAPPER;

                return;
            }
        }
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

            // Check if this is a nested version of this started pattern, but make sure that the current pattern is not already computed.
            if (Input[End_Index]->Data == start_pattern && !Input[End_Index]->Is(PARSE_BY::TOKEN_WRAPPER)){
                Parse_Wrapper(start_pattern, end_pattern, End_Index, Input);
            }

            if (Input[End_Index]->Data == end_pattern)
                Nested_Count--;

            // now check if the nested count is zero
            if (Nested_Count == 0){
                // First cut the list
                std::vector<HTML_Token*> Cut;
                Cut.insert(Cut.begin(), Input.begin() + i + 1, Input.begin() + End_Index);

                Input[i]->Childs = Parse_HTML(Cut);

                // Now delete the tokens we just cut
                Input.erase(Input.begin() + i + 1, Input.begin() + End_Index + 1);

                // Now set the parsed by flag.
                Input[i]->Parsed_By |= PARSE_BY::TOKEN_WRAPPER;
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

    std::vector<Element*> Parse_Translators(std::vector<HTML_Node*>& Input){
        std::vector<Element*> Result;

        for (int i = 0; i < Input.size(); i++){

            HTML_Node* Current = Input[i];

            // Try to find the translator fitting for this token.
            if (HTML_Translators.find(Current->Tag_Name) == HTML_Translators.end())
                continue;

            Element* New_Child = HTML_Translators[Current->Tag_Name](Current);

            if (New_Child){
                Result.push_back(New_Child);
             
                // If theprocess was succesfully runned, then remove this token, since it has been processes fully.
                Input.erase(Input.begin() + i);
            }
        }

        return Result;
    }

    std::vector<HTML_Node*> Parse_Lexed_Tokens(std::vector<HTML_Token*> Input){
        std::vector<HTML_Node*> Result;

        for (int i = 0; i < Input.size(); i++){

            HTML_Node* tmp = Factory(Input[i]);

            if (tmp)
                Result.push_back(tmp);

        }

        return Result;
    }

    HTML_Node* Factory(HTML_Token* Input){
        HTML_Node* Result = new HTML_Node();

        Result->Tag_Name = Input->Data;

        // Check if the child is just a text
        if (Input->Type == HTML_GROUP_TYPES::TEXT && Input->Childs.size() == 0){
            Result->Type = HTML_GROUP_TYPES::TEXT;
        }

        // try to search for attributes.
        for (auto token : Input->Childs){

            if (token->Type == HTML_GROUP_TYPES::ATTRIBUTE){
                // assuming all set operators are simple and not complex, where one side could contains other operators.
                Result->Attributes[token->Childs[0]->Data] = token->Childs[1]->Data;//.insert(token->Childs[0]->Data, token->Childs[1]->Data);
            }
            else{
                HTML_Node* tmp = Factory(token);

                if (tmp){
                    tmp->parent = Result;
                    Result->Childs.push_back(tmp);
                }
            }
        }

        Result->Position = Input->Position;

        Result->RAW = Input;

        return Result;
    }

    void Parse_Operator_Set(int& i, std::vector<HTML_Token*>& Input){
        // left index, this, right index
        // check that there is "space" around this index.
        if (i == 0 || i+1 >= Input.size() || Input[i]->Is(PARSE_BY::OPERATOR_SET))
            return;

        // We could use the operator type, but atm just use = support.
        if (Input[i]->Data != "=" && Input[i]->Childs.size() == 0)
            return;

        // add left
        Input[i]->Childs.push_back(Input[i-1]);
        
        // add right
        Input[i]->Childs.push_back(Input[i+1]);
    
        Input[i]->Parsed_By |= PARSE_BY::OPERATOR_SET;

        Input[i]->Type = HTML_GROUP_TYPES::ATTRIBUTE;

        // remove first right side.
        Input.erase(Input.begin() + i +1);

        // now remove the left side.
        Input.erase(Input.begin() + i -1);

        // also update the I, since this pointer was now moved to the left by 1.
        i--;
    }

}