#include "Button.h"
#include "HTML.h"
#include "../Renderer.h"

namespace GGUI{

    Button::Button(std::string Text, std::function<void (Button* This)> press) : Text_Field(Text, ALIGN::CENTER){
        GGUI::Pause_Renderer([=](){
            Default_Button_Behaviour(press);
            Allow_Overflow(true);
            Show_Border(true);
            Set_Name(Text);
        });
    }

}