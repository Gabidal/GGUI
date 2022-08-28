#ifndef _WINDOW_H_
#define _WINDOW_H_

#include <string>
#include <vector>
#include <map>

#include "Element.h"

//GGUI uses the ANSI escape code
//https://en.wikipedia.org/wiki/ANSI_escape_code
namespace GGUI{
    class Window : public Element{
        std::string Title = "";  //if this is empty then no title
    public:
        Window(){}

        Window(std::string title, Flags f);

        void Set_Title(std::string t);

        std::string Get_Title();

        std::vector<UTF> Render() override;
        
        void Add_Overhead(Element* w, std::vector<UTF>& Result) override;
    };
}

#endif