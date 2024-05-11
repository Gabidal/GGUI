#include "Button.h"
#include "HTML.h"
#include "../Renderer.h"

namespace GGUI{

    Button::Button(std::string Text, std::function<void (Button* This)> press) : Text_Field(Text){
        GGUI::Pause_Renderer([=](){
            Defualt_Button_Behaviour(press);
            Default_Button_Text_Align();
            Enable_Input_Overflow();
            Show_Border(true);
            Set_Name(Text);
        });
    }

}