#ifndef _HTML_H_
#define _HTML_H_

#include "Element.h"
#include "File_Streamer.h"

namespace GGUI{

    class HTML : public Element{
    private:
        // DONT GIVE TO USER !!!
        HTML(){}
    private:
        FILE_STREAM* Handle = nullptr;
    public:
        HTML(std::string File_Name);

        Element* Safe_Move() override {
            HTML* new_HTML = new HTML();
            *new_HTML = *(HTML*)this;

            return new_HTML;
        }

        std::string Get_Name() const override{
            return "HTML<" + Name + ">";
        }
    };

    enum class HTML_GROUP_TYPES{
        UNKNOWN,
        TEXT,
        KEYWORD,    // div, le, ul, etc...
        NUMBER,
        OPERATOR,   // =, 
        WRAPPER,    // <>, [], {}, (), "", ''
        SPACING,    // newline, ' ', '\t'
        ATTRIBUTE,  // Contains attributes as an wrapper extension. id="123"
    };

    enum class PARSE_BY{
        NONE                    = 0,
        TOKEN_WRAPPER           = 1 << 0,
        DYNAMIC_WRAPPER         = 1 << 1, 
        OPERATOR_SET            = 1 << 2,
    };

    extern PARSE_BY operator|(PARSE_BY first, PARSE_BY second);

    extern PARSE_BY operator&(PARSE_BY first, PARSE_BY second);

    extern void operator|=(PARSE_BY& first, PARSE_BY second);

    class HTML_Token{
    public:
        HTML_GROUP_TYPES Type = HTML_GROUP_TYPES::UNKNOWN;
        std::string Data = "";
        std::vector<HTML_Token*> Childs;    // also contains attributes!
        FILE_POSITION Position;

        PARSE_BY Parsed_By = PARSE_BY::NONE;

        HTML_Token(HTML_GROUP_TYPES Type, std::string Data){
            this->Type = Type;
            this->Data = Data;
        }

        HTML_Token(HTML_GROUP_TYPES Type, char Data, FILE_POSITION position){
            this->Type = Type;
            this->Data.push_back(Data);
            this->Position = position;
        }

        // Checks if the Parsed_By contains specific bit mask.
        bool Is(PARSE_BY f){
            return (Parsed_By & f) == f;
        }

        bool Has(PARSE_BY f){
            return (f & Parsed_By) > PARSE_BY::NONE;
        }

        HTML_Token() = default;
    };

    class HTML_Group{
    public:
        HTML_GROUP_TYPES Type = HTML_GROUP_TYPES::UNKNOWN;
        char Start = 0;
        char End = 0;
        bool Is_Sticky = true;

        HTML_Group(HTML_GROUP_TYPES Type, char Start, char End, bool Is_Sticky = true){
            this->Type = Type;
            this->Start = Start;
            this->End = End;
            this->Is_Sticky = Is_Sticky;
        }
    };

    class HTML_Node{
    public:
        std::string Tag_Name = "";  // DIV, HREF, etc...
        
        std::unordered_map<std::string, std::string> Attributes;    // contains ID, Name, Class, Color, BG_Color, etc...

        std::vector<HTML_Node*> Childs;
        HTML_Node* parent = nullptr;

        FILE_POSITION Position;

        HTML_Token* RAW = nullptr;
        HTML_GROUP_TYPES Type = HTML_GROUP_TYPES::UNKNOWN;
    };

    extern void Parse(std::vector<HTML_Token*>& Input);

    extern std::vector<Element*> Parse_HTML(std::string Raw_Buffer);

    extern std::vector<HTML_Token*>& Parse_HTML(std::vector<HTML_Token*>& Input);

    extern void Parse_Embedded_Bytes(int& i, std::vector<HTML_Token*>& Input);

    extern void Parse_All_Wrappers(int& i, std::vector<HTML_Token*>& Input);

    extern void Parse_Dynamic_Wrappers(int& i, std::vector<HTML_Token*>& Input, std::string word);

    extern void Parse_Wrapper(std::string start_pattern, std::string end_pattern, int& i, std::vector<HTML_Token*>& Input);

    extern const std::vector<HTML_Group> Groups;

    extern std::vector<HTML_Token*> Lex_HTML(std::string Raw_Buffer);

    extern std::vector<HTML_Node*> Parse_Lexed_Tokens(std::vector<HTML_Token*> Input);

    extern std::unordered_map<std::string, std::function<GGUI::Element* (HTML_Node*)>> HTML_Translators;

    // For ease of use for adding translators for user custom HTML TAG parsers.
    #define GGUI_Add_Translator(id, handler) auto _ = [](){ return HTML_Translators[id] = handler;}();

    extern std::vector<Element*> Parse_Translators(std::vector<HTML_Node*>& Input);

    extern HTML_Node* Factory(HTML_Token* Input);

    extern void Parse_Operator_Set(int& i, std::vector<HTML_Token*>& Input);

}

#endif