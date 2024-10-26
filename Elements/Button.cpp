#include "Button.h"
#include "HTML.h"
#include "../Core/Renderer.h"

namespace GGUI{

    Button::Button(std::string text, std::function<void (Button* This)> press, Styling s) : Element(s){
        Default_Button_Behaviour(press);
        Set_Name(text);

        Allow_Dynamic_Size(true);
        Show_Border(true);

        Add_Child(new Text_Field(text, Styling(align(ALIGN::CENTER))));
    }

}