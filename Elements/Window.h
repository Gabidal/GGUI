#ifndef _WINDOW_H_
#define _WINDOW_H_

#include <string>
#include <vector>

#include "Element.h"

//GGUI uses the ANSI escape code
//https://en.wikipedia.org/wiki/ANSI_escape_code
namespace GGUI{
    class Window : public Element{
        std::string Title = "";  //if this is empty then no title
    public:
        Window(){}

        Window(std::string title, std::vector<std::string> classes = {});

        Window(std::map<std::string, VALUE*> css, unsigned int width = 0, unsigned int height = 0, Element* parent = nullptr, Coordinates* position = nullptr);
        
        Window(std::string title, std::map<std::string, VALUE*> css, unsigned int width = 0, unsigned int height = 0, Element* parent = nullptr, Coordinates* position = nullptr);

        //These next constructors are mainly for users to more easily create elements.
        Window(
            std::string title, 
            unsigned int width,
            unsigned int height
        );

        Window(
            std::string title, 
            unsigned int width,
            unsigned int height,
            RGB text_color,
            RGB background_color
        );

        Window(
            std::string title, 
            unsigned int width,
            unsigned int height,
            RGB text_color,
            RGB background_color,
            RGB border_color,
            RGB border_background_color
        );

        Window(
            std::string title,
            unsigned int width,
            unsigned int height,
            std::vector<Element*> Tree 
        );

        //End of user constructors.



        void Set_Title(std::string t);

        std::string Get_Title();

        std::vector<UTF> Render() override;
        
        void Add_Overhead(Element* w, std::vector<UTF>& Result) override;

        std::string Get_Name() override;

        Element* Copy() override;
    };
}

#endif