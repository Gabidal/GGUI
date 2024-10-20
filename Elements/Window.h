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
        Window(std::string title = "", Styling s = STYLES::CONSTANTS::Default) : Element(s) {
            Update_Hidden_Border_Colors();

            Title = title;
        }

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