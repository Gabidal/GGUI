#include "HTML.h"
#include "../Core/Renderer.h"

#include <vector>

namespace GGUI{

    /**
     * @brief Bitwise OR operator for PARSE_BY.
     * @details This function takes two PARSE_BY values and returns a new PARSE_BY value that is the result of the bitwise OR operation on the two input values.
     * @param first The first PARSE_BY value.
     * @param second The second PARSE_BY value.
     * @return The result of the bitwise OR operation on the two input values.
     */
    PARSE_BY operator|(PARSE_BY first, PARSE_BY second){
        return (PARSE_BY)((unsigned long long)first | (unsigned long long)second);
    }

    /**
     * @brief Bitwise AND operator for PARSE_BY.
     * @details This function takes two PARSE_BY values and returns a new PARSE_BY value that is the result of the bitwise AND operation on the two input values.
     * @param first The first PARSE_BY value.
     * @param second The second PARSE_BY value.
     * @return The result of the bitwise AND operation on the two input values.
     */
    PARSE_BY operator&(PARSE_BY first, PARSE_BY second){
        return (PARSE_BY)((unsigned long long)first & (unsigned long long)second);
    }

    /**
     * @brief Bitwise OR operator for PARSE_BY.
     * @details This function takes a PARSE_BY value and a PARSE_BY value and sets the first value to the result of the bitwise OR operation on the two input values.
     * @param first The first PARSE_BY value.
     * @param second The second PARSE_BY value.
     */
    void operator|=(PARSE_BY& first, PARSE_BY second){
        first = first | second; 
    }

    std::unordered_map<std::string, std::function<GGUI::Element* (HTML_Node*)>>* HTML_Translators = {};

    std::unordered_map<std::string, double> POSTFIX_COEFFICIENT = {
        {"px", 1},
        {"%", 0.01},
        {"vw", 0.01},
        {"vh", 0.01},
        {"em", 1},
        {"rem", 1},
        {"in", 1},
        {"cm", 1},
        {"mm", 1},
        {"pt", 1 / 72},
        {"pc", 1 / 6},
        {"vmin", 0.01},
        {"vmax", 0.01},
    };

    std::unordered_map<std::string, void*> RELATIVE_COEFFICIENT = {
        {"em", nullptr},
        {"ex", nullptr},
        {"ch", nullptr},
        {"rem", nullptr},
        {"vw", nullptr},
        {"vh", nullptr},
        {"vmin", nullptr},
        {"vmax", nullptr},
        {"%", nullptr}
    };

    /**
     * @brief Constructor of the HTML class.
     * @param File_Name The name of the file to open.
     * 
     * This constructor will pause the GGUI renderer and create a new file stream
     * that will read the file and parse the HTML when it is changed.
     * The parsed HTML will be set as the child of this HTML object.
     */
    HTML::HTML(std::string File_Name){
        Pause_GGUI([this, File_Name](){
            Handle = new FILE_STREAM(File_Name, [this](){
                this->Set_Childs(Parse_HTML(Handle->Fast_Read(), this));
            });

            Set_Name(File_Name);
        });
    }

    /**
     * @brief Parses the HTML tokens.
     * @param Input The vector of tokens to parse.
     * 
     * This function parses the HTML tokens by combining wrappers like: <, >, (, ), etc...
     * It also captures decimals, parses operators in Reverse PEMDAS order, and combines dynamic wrappers like: <html>, </html>
     */
    void Parse(std::vector<HTML_Token*>& Input){
        // First combine wrappers like: <, >, (, ), etc...
        for (int i = 0; i < (signed)Input.size(); i++){
            Parse_Embedded_Bytes(i, Input);
            Parse_All_Wrappers(i, Input);
        }

        // Capture decimals
        for (int i = 0; i < (signed)Input.size(); i++)
            Parse_Operator(i, Input, '.');

        for (int i = 0; i < (signed)Input.size(); i++)
            Parse_Numeric_Postfix(i, Input);

        // Reverse PEMDAS order:
        for (int i = 0; i < (signed)Input.size(); i++)
            Parse_Operator(i, Input, '+');
        for (int i = 0; i < (signed)Input.size(); i++)
            Parse_Operator(i, Input, '-');
        for (int i = 0; i < (signed)Input.size(); i++)
            Parse_Operator(i, Input, '*');
        for (int i = 0; i < (signed)Input.size(); i++)
            Parse_Operator(i, Input, '/');

        for (int i = 0; i < (signed)Input.size(); i++)
            Parse_Operator(i, Input, '=');

        // now start combining dynamic wrappers like: <html>, </html>
        for (int i = 0; i < (signed)Input.size(); i++){
            // first try to find the first instance of < token.
            if (Input[i]->Data != "<" || Input[i]->Childs.size() == 0 || Input[i]->Childs[0]->Data == "!")
                continue;

            Parse_Dynamic_Wrappers(i, Input, Input[i]->Childs[0]->Data);
        }
    }

    /**
     * @brief Parses raw HTML buffer into elements.
     * @param Raw_Buffer The raw HTML buffer to parse.
     * @param parent The parent element to set for top-level nodes.
     * @return A vector of parsed HTML elements.
     */
    std::vector<Element*> Parse_HTML(std::string Raw_Buffer, Element* parent) {
        // Lex the raw buffer into HTML tokens.
        std::vector<HTML_Token*> Lexed_Tokens = Lex_HTML(Raw_Buffer);

        // Parse the lexed tokens to handle wrappers, operators, etc.
        Parse(Lexed_Tokens);

        // Convert lexed tokens into parsed HTML nodes.
        std::vector<HTML_Node*> Parsed_Tokens = Parse_Lexed_Tokens(Lexed_Tokens);

        // Set the parent element for the top-level nodes.
        for (auto node : Parsed_Tokens) {
            if (!node->parent) {
                node->parent = Element_To_Node(parent);
            }
        }

        // Translate parsed nodes into elements and return them.
        return Parse_Translators(Parsed_Tokens);
    }

    /**
     * @brief Parses the HTML tokens.
     * @param Input The vector of tokens to parse.
     * @return The parsed vector of HTML tokens.
     * 
     * This function parses the HTML tokens by combining wrappers like: <, >, (, ), etc...
     * It also captures decimals, parses operators in Reverse PEMDAS order, and combines dynamic wrappers like: <html>, </html>
     */
    std::vector<HTML_Token*>& Parse_HTML(std::vector<HTML_Token*>& Input){
        Parse(Input);

        return Input;
    }

    void Parse_Embedded_Bytes([[maybe_unused]] int& i, [[maybe_unused]] std::vector<HTML_Token*>& Input){
        // This function check if the input at i contains '/' which then marks the following tokens to be skipped
        // Example "absc /" a /" "


    }

    /**
     * @brief Parses all wrappers in the given vector of tokens.
     * @param i The index to start from.
     * @param Input The vector of tokens to parse.
     * 
     * This function parses all wrappers in the input, such as:
     * - <>
     * - []
     * - {}
     * - ()
     * - ""
     * - '
     */
    void Parse_All_Wrappers(int& i, std::vector<HTML_Token*>& Input){
        Parse_Wrapper("<", ">", i, Input);  // HTML tags
        Parse_Wrapper("[", "]", i, Input);  // Brackets
        Parse_Wrapper("{", "}", i, Input);  // Braces
        Parse_Wrapper("(", ")", i, Input);  // Parentheses
        Parse_Wrapper("\"", "\"", i, Input);  // Double quotes
        Parse_Wrapper("\'", "\'", i, Input);  // Single quotes
    }

    /**
     * @brief Parses all wrappers in the given vector of tokens.
     * @param i The index to start from.
     * @param Input The vector of tokens to parse.
     * @param word The word which is used to identify the dynamic wrapper.
     * 
     * This function parses all wrappers in the input, such as:
     * - <>
     * - []
     * - {}
     * - ()
     * - ""
     * - '
     */
    void Parse_Dynamic_Wrappers(int& i, std::vector<HTML_Token*>& Input, std::string word){
        if (i < 0)
            return;

        int End_Index = i;
        
        for (End_Index++; End_Index < (signed)Input.size(); End_Index++){

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

                // now also add the Attributes which are defined in the Starting Dynamic Wrapper
                for (auto attr : Input[i]->Childs){
                    if (attr->Data != "=")  // Only support SET operators as attributes.
                        continue;

                    if (attr->Childs.size() != 2)
                        continue;           // The operator must be completely parsed.

                    attr->Type = HTML_GROUP_TYPES::ATTRIBUTE;   // This is for the later system to recognise this as an attribute and set it to the HTML_NODE.
                    New_Wrapper->Childs.push_back(attr);
                }

                // <"title" <- in Input[i]childs[0] > content </title (end_index here)>
                New_Wrapper->Data = Input[i]->Childs[0]->Data; // Name the wrapper with the name.

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


    /**
     * @brief Parses a wrapper token and all of its child tokens.
     * @param start_pattern The pattern that starts the wrapper.
     * @param end_pattern The pattern that ends the wrapper.
     * @param i The index of the start pattern in the input vector.
     * @param Input The input vector of tokens.
     * @details This function starts from the given index and every time it finds a start pattern it starts a new loop from the start pattern index until the end pattern count hits 0 and puts all of the tokens between the start and end pattern into the childs.
     *          The start pattern is not included in the child tokens.
     *          The end pattern is not included in the child tokens.
     *          The parsed by flag is set to the start pattern token.
     *          The function returns nothing, but modifies the input vector by deleting the tokens between the start and end pattern.
     *          If the start pattern is not found in the input vector, the function does nothing.
     *          If the nested count is still above 0 even after looping through all the tokens, just ignore lolw.
     */
    void Parse_Wrapper(std::string start_pattern, std::string end_pattern, int& i, std::vector<HTML_Token*>& Input){
        // This function starts from the given index and every time it finds a start pattern it starts a new loop from the start pattern index until the end pattern count hits 0 and puts all of the tokens between the start and end pattern into the childs.
        if (i < 0)
            return; // Check for instance if the previous pattern already annihilated all the leftover tokens :)
        
        if (Input[i]->Data != start_pattern)
            return;

        int Nested_Count = 1;   // Remember to count the start pattern too.
        int End_Index = i;

        for (End_Index++; End_Index < (signed)Input.size() && Nested_Count > 0; End_Index++){

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

        for (unsigned int i = 0; i < Un_Sanitized.size(); i++){
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

    /**
     * @brief Lexes the raw HTML string into a vector of HTML tokens.
     * @param Raw_Buffer The input HTML string to be tokenized.
     * @return A vector of pointers to HTML_Token objects.
     *
     * This function slices the given HTML text into tokens based on character groups.
     * It identifies different types of tokens such as text, numbers, operators, etc.
     * and returns a vector containing these tokens.
     */
    std::vector<HTML_Token*> Lex_HTML(std::string Raw_Buffer) {
        std::vector<HTML_Token*> Result;
        HTML_Token* Current_Token = new HTML_Token();
        FILE_POSITION Current_Position = FILE_POSITION("unknown", 0, 0);

        for (auto Current_Char : Raw_Buffer) {
            bool All_Characters_Matched = false;

            // Determine the character type based on predefined groups.
            for (auto Group : Groups) {
                if (Group.Start > Current_Char || Group.End < Current_Char)
                    continue;

                All_Characters_Matched = true;

                // Check if a new token group is detected.
                if (Current_Token->Type != Group.Type || !Group.Is_Sticky) {
                    Result.push_back(Current_Token);
                    Current_Token = new HTML_Token(Group.Type, Current_Char, Current_Position);
                } else {
                    // Append character to the current token.
                    Current_Token->Data.push_back(Current_Char);
                }

                break;
            }

            // Handle characters that do not match any group.
            if (!All_Characters_Matched) {
                Result.push_back(Current_Token);
                Current_Token = new HTML_Token(HTML_GROUP_TYPES::UNKNOWN, Current_Char, Current_Position);
            }

            // Update the current position in the file.
            Current_Position.Character++;
            if (Current_Char == '\n') {
                Current_Position.Line_Number++;
                Current_Position.Character = 0;
            }
        }

        // Ensure the last token is added to the result.
        Result.push_back(Current_Token);

        // Sanitize the tokens by removing redundant newlines, spaces, etc.
        Sanitize_HTML_Tokens(Result);

        return Result;
    }

    /**
     * @brief Parses a vector of HTML nodes into elements.
     * @param Input A vector of pointers to HTML_Node objects to be parsed.
     * @return A vector of pointers to Element objects.
     *
     * This function processes each HTML node in the input vector, 
     * tries to find a translator fitting for the node's tag name, 
     * and if found, runs it. If the translator returns a non-null element, 
     * the element is added to the result vector. The translator is run in a 
     * paused GGUI state, meaning all events and updates are paused. After the 
     * translator has been run, the processed node is removed from the input vector.
     */
    std::vector<Element*> Parse_Translators(std::vector<HTML_Node*>& Input){
        std::vector<Element*> Result;

        GGUI::Pause_GGUI([&Input, &Result](){
            for (unsigned int i = 0; i < Input.size(); i++){

                HTML_Node* Current = Input[i];

                // Try to find the translator fitting for this token.
                if (HTML_Translators->find(Current->Tag_Name) == HTML_Translators->end())
                    continue;

                Element* New_Child = HTML_Translators->at(Current->Tag_Name)(Current);

                if (New_Child){
                    Result.push_back(New_Child);
                
                    // If the process was successfully run, then remove this token, since it has been processes fully.
                    Input.erase(Input.begin() + i);
                }
            }
        });

        return Result;
    }

    /**
     * @brief Parses a vector of HTML tokens into HTML nodes.
     * @param Input A vector of pointers to HTML_Token objects to be parsed.
     * @return A vector of pointers to HTML_Node objects.
     *
     * This function processes each HTML token in the input vector, 
     * converts them into HTML nodes using the Factory function, 
     * and returns a vector containing these nodes. 
     * Only non-null nodes are added to the result vector.
     */
    std::vector<HTML_Node*> Parse_Lexed_Tokens(std::vector<HTML_Token*> Input) {
        std::vector<HTML_Node*> Result;

        // Iterate over each token in the input vector.
        for (unsigned int i = 0; i < Input.size(); i++) {
            // Convert each token into an HTML node.
            HTML_Node* tmp = Factory(Input[i]);

            // Add the node to the result vector if it's not null.
            if (tmp)
                Result.push_back(tmp);
        }

        return Result;
    }

    /**
     * @brief Converts an HTML token into an HTML node.
     * @param Input The input HTML token to be converted.
     * @return A pointer to an HTML node.
     *
     * This function processes the given HTML token and converts it into an HTML node.
     * It checks if the child is just a text and if so sets the type of the node to be text.
     * Then it iterates over each child of the token and tries to find attributes.
     * If an attribute is found, it's added to the node's attribute list.
     * If the child is not an attribute, it's converted into an HTML node using this function recursively.
     * Finally, the function returns the HTML node.
     */
    HTML_Node* Factory(HTML_Token* Input) {
        HTML_Node* Result = new HTML_Node();

        Result->Tag_Name = Input->Data;

        // Check if the child is just a text
        if (Input->Type == HTML_GROUP_TYPES::TEXT && Input->Childs.size() == 0) {
            Result->Type = HTML_GROUP_TYPES::TEXT;
        }

        // Try to search for attributes.
        for (auto token : Input->Childs) {
            // Here we assimilate the attributes
            if (token->Type == HTML_GROUP_TYPES::ATTRIBUTE) {
                // Assuming all set operators are simple and not complex, where one side could contains other operators.
                // Complex arithmetics which are set to the value (width=1vw/2px) need to be parsed before assigning it to the attr list, 
                // since the name does not contain either the postfixes or complex AST cumulated values! 
                Result->Attributes[token->Childs[0]->Data] = token->Childs[1];
            }
            else {
                HTML_Node* tmp = Factory(token);

                if (tmp) {
                    tmp->parent = Result;
                    Result->Childs.push_back(tmp);
                }
            }
        }

        Result->Position = Input->Position;

        Result->RAW = Input;

        return Result;
    }

    /**
     * @brief Parses a postfix for a numeric token.
     * @param i The current index in the input vector.
     * @param Input The input vector of HTML tokens to be parsed.
     * 
     * This function processes the token at the given index in the input vector.
     * It checks if the token is a number, and if so, it checks if the next token is a postfix.
     * If the next token is a postfix, it adds it to the current token as a child and marks the current token as having a postfix.
     * Then it removes the postfix token from the input vector.
     */
    void Parse_Numeric_Postfix(int& i, std::vector<HTML_Token*>& Input){
        // Because numbers and letters do not combine in Lexing phase we can assume that all special numbers would have an tailing token which would describe the postfix.
        if (Input[i]->Type != HTML_GROUP_TYPES::NUMBER || Input[i]->Is(PARSE_BY::NUMBER_POSTFIX_PARSER))
            return;

        // check for the postfix
        // basically all the prefixes are text besides the %, which is an operator.
        if (i + 1 < (signed)Input.size() && (Input[i+1]->Type == HTML_GROUP_TYPES::TEXT || Input[i+1]->Data == "%")){
            // Append the postfix to the current token.
            Input[i]->Childs.push_back(Input[i+1]);

            // Mark the current token as having a postfix.
            Input[i]->Parsed_By |= PARSE_BY::NUMBER_POSTFIX_PARSER;

            // Remove the postfix token from the input vector.
            Input.erase(Input.begin() + i + 1);
        }

        return;
    }

    /**
     * @brief Parses a decimal number token.
     * @param i The current index in the input vector.
     * @param Input The input vector of HTML tokens to be parsed.
     * 
     * This function processes the token at the given index in the input vector.
     * If the token is a decimal number, it checks if the left and right side of the decimal token are numbers.
     * If they are, it checks if the decimal number is valid by trying to convert it to a double.
     * If the decimal number is valid, it creates a new token with the decimal value and replaces the current token with it.
     * If the decimal number is invalid, it reports an error.
     */
    void Parse_Decimal(int& i, std::vector<HTML_Token*>& Input){

        // Decimals are '.' operators which have captured an number on their left and right side.
        if (!Input[i]->Is(PARSE_BY::OPERATOR_PARSER) || Input[i]->Data != ".")
            return;

        if (Input[i]->Childs.size() != 2)
            return;

        if (Input[i]->Childs[0]->Type != HTML_GROUP_TYPES::NUMBER || Input[i]->Childs[1]->Type != HTML_GROUP_TYPES::NUMBER)
            return;

        std::string STR_VALUE = Input[i]->Childs[0]->Data + "." + Input[i]->Childs[1]->Data;

        // try to check if the decimal is valid
        try{
            // now put the left side number as the main decimal value and the right side as the fraction.
            [[maybe_unused]] double Decimal_Value = std::stod(STR_VALUE);
        }
        catch(...){
            Report("Invalid decimal number: " + STR_VALUE, Input[i]->Position);
            return;
        }

        HTML_Token* Decimal = new HTML_Token(HTML_GROUP_TYPES::NUMBER, STR_VALUE);
        Decimal->Position = Input[i]->Position;

        // de-allocate the previous
        delete Input[i];

        // replace the current token with the decimal.
        Input[i] = Decimal;
    }

    /**
     * @brief Parses an operator token and its surrounding tokens.
     * @param i The current index in the input vector.
     * @param Input The input vector of HTML tokens to be parsed.
     * @param operator_type The character representing the operator.
     *
     * This function processes the token at the given index in the input vector.
     * It checks if the token is an operator and not already parsed by the operator parser.
     * If the current token matches the operator type and is not already parsed, it adds the
     * left and right tokens as children of the operator token and marks it as parsed by the operator parser.
     * The function also sets the type of the token to ATTRIBUTE and removes the left and right tokens
     * from the input vector. The index is updated accordingly.
     */
    void Parse_Operator(int& i, std::vector<HTML_Token*>& Input, char operator_type) {
        // Ensure there is space around the current token and it is not already parsed.
        if (i == 0 || i + 1 >= static_cast<int>(Input.size()) || Input[i]->Is(PARSE_BY::OPERATOR_PARSER))
            return;

        // Check if the current token matches the specified operator type and has no children.
        if (Input[i]->Data[0] != operator_type && Input[i]->Childs.empty())
            return;

        // Add the left token as a child of the current operator token.
        Input[i]->Childs.push_back(Input[i - 1]);
        
        // Add the right token as a child of the current operator token.
        Input[i]->Childs.push_back(Input[i + 1]);
    
        // Mark the current token as parsed by the operator parser.
        Input[i]->Parsed_By |= PARSE_BY::OPERATOR_PARSER;

        // Set the current token type to ATTRIBUTE.
        Input[i]->Type = HTML_GROUP_TYPES::ATTRIBUTE;

        // Remove the right token from the input vector.
        Input.erase(Input.begin() + i + 1);

        // Remove the left token from the input vector.
        Input.erase(Input.begin() + i - 1);

        // Update the index as the current token position has shifted left by 1.
        i--;
    }

    /**
     * @brief Reports an error to the user.
     * @param problem The error message to display.
     * @param location The location of the error in the file.
     *
     * This function appends the location of the error to the error message and
     * calls the GGUI::Report function to display the error to the user.
     */
    void Report(std::string problem, FILE_POSITION location){
        GGUI::Report(location.To_String() + ": " + problem);
    }

    /**
     * @brief Converts an Element to an HTML node.
     * @param e The element to convert.
     * @return A pointer to an HTML node.
     *
     * This function creates an HTML node from the given element.
     * It sets the tag name of the node to "div" and the type to wrapper.
     * Then it adds the attributes of the element to the node.
     * The width and height of the element are added as number type attributes.
     */
    HTML_Node* Element_To_Node(Element* e){
        HTML_Node* Result = new HTML_Node();

        // Set the tag name to "div" and the type to wrapper.
        Result->Tag_Name = "div";
        Result->Type = HTML_GROUP_TYPES::WRAPPER;

        // Now add the attributes of the element to the node.
        Result->Attributes["width"] = new GGUI::HTML_Token(HTML_GROUP_TYPES::NUMBER, std::to_string(e->Get_Width()));
        Result->Attributes["height"] = new GGUI::HTML_Token(HTML_GROUP_TYPES::NUMBER, std::to_string(e->Get_Height()));

        return Result;
    }


    /**
     * @brief Computes the value of a given token.
     * @param val The token to compute the value of.
     * @param parent The parent node of the token.
     * @param attr_name The name of the attribute to compute the value of.
     * @return The computed value of the token.
     *
     * This function is called by the translator to compute the value of a token.
     * It checks if the token is an operator and if so, it calls the Compute_Operator
     * function to compute the value of the operator. If the token is not an operator, it
     * checks if the token has a postfix and if so, it calls the Compute_Post_Fix_As_Coefficient
     * function to compute the coefficient of the postfix. The computed value is then returned.
     */
    double Compute_Val(HTML_Token* val, HTML_Node* parent, std::string attr_name){
        double Result = 0;

        if (val->Type == HTML_GROUP_TYPES::OPERATOR)
            Result = Compute_Operator(val, parent, attr_name);

        // check the postfix
        else if (val->Is(PARSE_BY::NUMBER_POSTFIX_PARSER))
            Result *= Compute_Post_Fix_As_Coefficient(val->Childs[0]->Data, parent, attr_name);

        return Result;
    }

    /**
     * @brief Computes the result of an operator token.
     * 
     * This function evaluates an operator token by computing the values
     * of its left and right child nodes and applying the operator to
     * those values. Supported operators are +, -, *, /, and =.
     * 
     * @param op The operator token to evaluate.
     * @param parent The parent HTML node of the token.
     * @param attr_name The name of the attribute associated with the token.
     * @return The computed result of the operator.
     */
    double Compute_Operator(HTML_Token* op, HTML_Node* parent, std::string attr_name) {
        double Result = 0;

        // Compute the values of the left and right child nodes.
        double Left = Compute_Val(op->Childs[0], parent, attr_name);
        double Right = Compute_Val(op->Childs[1], parent, attr_name);

        // Apply the operator to the computed values.
        if (op->Data == "+")
            Result = Left + Right;
        else if (op->Data == "-")
            Result = Left - Right;
        else if (op->Data == "*")
            Result = Left * Right;
        else if (op->Data == "/")
            Result = Left / Right;
        else if (op->Data == "=")
            Result = Left = Right;
        else
            Report("Unknown operator: " + op->Data, op->Position);

        return Result;
    }

    /**
     * @brief Computes the result of a postfix token as a coefficient.
     * @details This function evaluates a postfix token as a coefficient by
     *          multiplying the coefficient value with the specified attribute
     *          value of the parent node or the parent's parent node.
     *          Supported postfixes are vw, vh, %, vmin, and vmax.
     * @param postfix The postfix token to evaluate.
     * @param parent The parent HTML node of the token.
     * @param attr_name The name of the attribute associated with the token.
     * @return The computed coefficient.
     */
    double Compute_Post_Fix_As_Coefficient(std::string postfix, HTML_Node* parent, std::string attr_name){
        double Result = POSTFIX_COEFFICIENT[postfix];

        // now check if the post fix is an Relative type.
        if (RELATIVE_COEFFICIENT.find(postfix) == RELATIVE_COEFFICIENT.end())
            return Result;  // if the postfix is not a relative type, then just return the base coefficient.

        /**
         * For relative types, the coefficient is computed based on the
         * attribute value of the parent node or the parent's parent node.
         */
        if (postfix == "vw")
            Result *= std::stod(parent->Attributes["width"]->Data);
        else if (postfix == "vh")
            Result *= std::stod(parent->Attributes["height"]->Data);
        else if (postfix == "%")
            Result *= std::stod(parent->Attributes[attr_name]->Data);
        else if (postfix == "vmin")
            Result *= GGUI::Max(1.0, GGUI::Min(std::stod(parent->Attributes["width"]->Data), std::stod(parent->Attributes["height"]->Data)));
        else if (postfix == "vmax")
            Result *= GGUI::Max(std::stod(parent->Attributes["width"]->Data), std::stod(parent->Attributes["height"]->Data));
        else
            Report("Unknown relative type: " + postfix, parent->Position);

        return Result;
    }


    /**
     * @brief Translates an HTML node's attributes to an Element.
     * @param e The Element to translate the attributes to.
     * @param input The HTML node to translate the attributes from.
     *
     * This function translates the attributes of the given HTML node to the given Element.
     * It sets the Element's width and height attributes using the Compute_Val function.
     * It also sets the Element's flexbox properties if the flex-direction attribute is set.
     */
    void Translate_Attributes_To_Element(Element* e, HTML_Node* input){

        // Keep track of the current position type
        [[maybe_unused]] GGUI::HTML_POSITION_TYPE Current_Position_Type = GGUI::HTML_POSITION_TYPE::STATIC;

        // If the HTML node has the flex-direction attribute, this flag is set to true
        bool USE_FLEX = false;

        // Loop through all the attributes of the HTML node
        for (auto attr : input->Attributes){

            // Set the width of the Element to the value of the width attribute
            if (attr.first == "width")
                e->Set_Width(GGUI::Compute_Val(attr.second, input->parent, attr.first));
            // Set the height of the Element to the value of the height attribute
            else if (attr.first == "height")
                e->Set_Height(GGUI::Compute_Val(attr.second, input->parent, attr.first));
            // Check if the attribute is a flex property
            else if (attr.first.compare(0, 5, "flex-") == 0){
                if (!USE_FLEX){
                    // If the flex-direction attribute is not set, report an error
                    GGUI::Report("Cannot use Flex properties whitout enabling flexbox first!", input->Position);
                    continue;
                }

                // If the flex-direction attribute is set, set the flow direction of the Element
                if (attr.first == "flex-direction"){
                    if (attr.second->Data == "column")
                        ((List_View*)e)->Set_Flow_Direction(GGUI::DIRECTION::COLUMN);
                    else if (attr.second->Data == "row")
                        ((List_View*)e)->Set_Flow_Direction(GGUI::DIRECTION::ROW);
                    else
                        // If the flex-direction attribute has an unknown value, report an error
                        GGUI::Report("Unknown flex-direction: " + attr.second->Data, input->Position);
                }
            }
        }
    }

    /**
     * @brief Translates the child nodes of an HTML node to an Element.
     * @param e The Element to which the child nodes are to be translated.
     * @param input The HTML node containing the child nodes.
     * @param Set_Text_To A pointer to a string where the resulting text will be set.
     *
     * This function processes each child node of the given HTML node.
     * If a child node is of type TEXT, its tag name is added to the Raw_Text vector.
     * If a child node has a tag name "br", a newline character is added to the Raw_Text vector.
     * If a translator is available for a child node's tag name, the child node is translated 
     * to an Element, and the Element is added as a child to the parent Element.
     * Finally, the concatenated text from the Raw_Text vector is set to the provided string pointer.
     */
    void Translate_Childs_To_Element(Element* e, HTML_Node* input, std::string* Set_Text_To) {
        std::string Result = "";
        std::vector<std::string> Raw_Text;

        // Recursively process each child node in the HTML node's children vector.
        for (auto c : input->Childs) {
            // Add text nodes to the Raw_Text vector.
            if (c->Type == HTML_GROUP_TYPES::TEXT)
                Raw_Text.push_back(c->Tag_Name);

            // Add newline for <br> tags.
            if (c->Tag_Name == "br")
                Raw_Text.push_back("\n");

            // Check if a translator exists for the child's tag name.
            if (GGUI::HTML_Translators->find(c->Tag_Name) == GGUI::HTML_Translators->end())
                continue;

            // Translate the child node to an Element and add it to the parent Element.
            GGUI::Element* tmp = GGUI::HTML_Translators->at(c->Tag_Name)(c);
            if (tmp) {
                e->Add_Child(tmp);
            }
        }

        // Concatenate text from Raw_Text and set it to the provided string pointer.
        for (auto t : Raw_Text) {
            Result += t;
            // Add a space between words unless it's the last word.
            if (t != Raw_Text[Raw_Text.size() - 1])
                Result += " ";
        }
        *Set_Text_To = Result;
    }

}