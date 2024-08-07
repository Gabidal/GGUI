#ifndef _WINDOW_H_
#define _WINDOW_H_

#include "Element.h"

//GGUI uses the ANSI escape code
//https://en.wikipedia.org/wiki/ANSI_escape_code
namespace GGUI{
    class Window : public Element{
        std::string Title = "";  //if this is empty then no title

        RGB Before_Hiding_Border_Color = COLOR::WHITE;
        RGB Before_Hiding_Border_Background_Color = COLOR::BLACK;
        bool Has_Hidden_Borders = false;
    public:
        Window() : Element() {
            Update_Hidden_Border_Colors();
        }

        Window(std::string title, std::vector<std::string> classes = {});

        Window(Styling css, unsigned int width = 0, unsigned int height = 0, Element* parent = nullptr, Coordinates* position = nullptr);
        
        Window(std::string title, Styling css, unsigned int width = 0, unsigned int height = 0, Element* parent = nullptr, Coordinates* position = nullptr);

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
            RGB border_color
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

        ~Window() override{
            // call the base destructor.
            Element::~Element();
        }

        //End of user constructors.

        void Update_Hidden_Border_Colors();

        void Set_Title(std::string t);

        std::string Get_Title();
        
        void Add_Overhead(Element* w, std::vector<UTF>& Result) override;

        std::string Get_Name() const override;

        void Show_Border(bool state) override;

        void Show_Border(bool state, bool previus_state) override;

        void Set_Background_Color(RGB color) override;

        void Set_Text_Color(RGB color) override;

        void Set_Border_Background_Color(RGB color) override;

        void Set_Border_Color(RGB color) override;

        Element* Safe_Move() override {
            Window* new_Window = new Window();
            *new_Window = *(Window*)this;

            return new_Window;
        }
    };
}

#endif