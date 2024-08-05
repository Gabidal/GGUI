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

        ~HTML() override{
            if (Handle != nullptr){
                delete Handle;
            }

            // call the base destructor.
            Element::~Element();
        }

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
        OPERATOR_PARSER         = 1 << 2,
        NUMBER_POSTFIX_PARSER   = 1 << 3,
    };

    enum class HTML_POSITION_TYPE{
        STATIC,     // Default positioning, like in GGUI.
        RELATIVE,   // Relative to parent.
        ABSOLUTE,   // Relative to screen.
        FIXED,      // Relative to screen, but does not move with scrolling.
        STICKY,     // Relative to screen until crosses given threshold.
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
        
        std::vector<HTML_Node*> Childs;
        HTML_Node* parent = nullptr;

        FILE_POSITION Position;

        HTML_Token* RAW = nullptr;
        HTML_GROUP_TYPES Type = HTML_GROUP_TYPES::UNKNOWN;

        // Postfixes are in child[0] for numbers.
        // Decimals are also number typed.
        // Operators left is Child[0] and Right at Child[1].
        // Attributes cannot be computed, before some contextual data on AST level is constructed, since the postfix operands depend on these kind of information from parent.
        std::unordered_map<std::string, GGUI::HTML_Token*> Attributes;    // contains ID, Name, Class, Color, BG_Color, etc...
    };

    extern void Parse(std::vector<HTML_Token*>& Input);

    extern std::vector<Element*> Parse_HTML(std::string Raw_Buffer, Element* parent);

    extern std::vector<HTML_Token*>& Parse_HTML(std::vector<HTML_Token*>& Input);

    extern void Parse_Embedded_Bytes(int& i, std::vector<HTML_Token*>& Input);

    extern void Parse_All_Wrappers(int& i, std::vector<HTML_Token*>& Input);

    extern void Parse_Dynamic_Wrappers(int& i, std::vector<HTML_Token*>& Input, std::string word);

    extern void Parse_Wrapper(std::string start_pattern, std::string end_pattern, int& i, std::vector<HTML_Token*>& Input);

    extern const std::vector<HTML_Group> Groups;

    extern std::vector<HTML_Token*> Lex_HTML(std::string Raw_Buffer);

    extern std::vector<HTML_Node*> Parse_Lexed_Tokens(std::vector<HTML_Token*> Input);

    extern std::unordered_map<std::string, std::function<GGUI::Element* (HTML_Node*)>>* HTML_Translators;

    extern std::unordered_map<std::string, double> POSTFIX_COEFFICIENT;

    extern std::unordered_map<std::string, void*> RELATIVE_COEFFICIENT;

    // Hash function for __FILE__
    // the CXX20+ Cannot for some reason use constexpr at compile time, probably because of PREPROCESSOR being computed before CONSTEXPR. 
    constexpr unsigned int hash(const char* str, int h = 0)
    {
        return !str[h] ? 5381 : (hash(str, h+1) * 33) ^ str[h];
    }

    // helper functions
    #define CONCAT_IMPL(x, y) x##y
    #define CONCAT(x, y) CONCAT_IMPL(x, y)

    // For ease of use for adding translators for user custom HTML TAG parsers.
    #define GGUI_Add_Translator(id, handler) \
        auto CONCAT(CONCAT(_, __LINE__), __COUNTER__) = [](){ \
            if (GGUI::HTML_Translators == nullptr){ \
                GGUI::HTML_Translators = new std::unordered_map<std::string, std::function<GGUI::Element* (GGUI::HTML_Node*)>>(); \
            } \
            return GGUI::HTML_Translators->insert({id, handler}); \
        }();


    extern std::vector<Element*> Parse_Translators(std::vector<HTML_Node*>& Input);

    extern HTML_Node* Factory(HTML_Token* Input);

    extern void Parse_Numeric_Postfix(int& i, std::vector<HTML_Token*>& Input);

    extern void Parse_Decimal(int& i, std::vector<HTML_Token*>& Input);

    extern void Parse_Operator(int& i, std::vector<HTML_Token*>& Input, char operator_type);

    extern void Report(std::string problem, FILE_POSITION location);

    extern HTML_Node* Element_To_Node(Element* e);

    extern double Compute_Val(HTML_Token* val, HTML_Node* parent, std::string attr_name);

    extern double Compute_Operator(HTML_Token* op, HTML_Node* parent, std::string attr_name);

    extern double Compute_Post_Fix_As_Coefficient(std::string postfix, HTML_Node* parent, std::string attr_name);

    extern void Translate_Attributes_To_Element(Element* e, HTML_Node* input);

    extern void Translate_Childs_To_Element(Element* e, HTML_Node* input, std::string* Set_Text_To);

}

#endif