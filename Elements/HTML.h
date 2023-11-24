#ifndef _HTML_H_
#define _HTML_H_

#include "Element.h"
#include "File_Streamer.h"

namespace GGUI{

    class HTML : public Element{
    private:
        FILE_STREAM* Handle = nullptr;
    public:
        HTML(std::string File_Name);

        Element_Type Get_Type() override { return Element_Type::HTML; }
    };

    enum class HTML_GROUP_TYPES{
        UNKNOWN,
        TEXT,
        KEYWORD,    // div, le, ul, etc...
        NUMBER,
        OPERATOR,   // =, 
        WRAPPER,    // <>, [], {}, (), "", ''
        SPACING,    // newline, ' ', '\t'
    };

    class HTML_Token{
    public:
        HTML_GROUP_TYPES Type = HTML_GROUP_TYPES::UNKNOWN;
        std::string Data = "";
        std::vector<HTML_Token*> Childs;    // also contains attributes!
        FILE_POSITION Position;

        HTML_Token(HTML_GROUP_TYPES Type, std::string Data){
            this->Type = Type;
            this->Data = Data;
        }

        HTML_Token(HTML_GROUP_TYPES Type, char Data, FILE_POSITION position){
            this->Type = Type;
            this->Data.push_back(Data);
            this->Position = position;
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

    extern void Parse(std::vector<HTML_Token*>& Input);

    extern std::vector<Element*> Parse_HTML(std::string Raw_Buffer);

    extern std::vector<HTML_Token*>& Parse_HTML(std::vector<HTML_Token*>& Input);

    extern void Parse_Embedded_Bytes(int& i, std::vector<HTML_Token*>& Input);

    extern void Parse_All_Wrappers(int& i, std::vector<HTML_Token*>& Input);

    extern void Parse_Wrapper(std::string start_pattern, std::string end_pattern, int& i, std::vector<HTML_Token*>& Input);

    extern const std::vector<HTML_Group> Groups;

    extern std::vector<HTML_Token*> Lex_HTML(std::string Raw_Buffer);
}

#endif