#ifndef _BUTTON_H_
#define _BUTTON_H_

#include <vector>
#include <string>

#include <functional>

#include "Text_Field.h"

namespace GGUI{

    class Button : public Text_Field{
    protected:
        void Defualt_Button_Behaviour(std::function<void (Button* This)> press = [](Button* This){}){
            On_Click([=](Event* e){
                // The default, on_click wont do anything.
                press(this);
            });
        }
    
        void Default_Button_Text_Align(){
            At<NUMBER_VALUE>(STYLES::Text_Position)->Value = (int)TEXT_LOCATION::CENTER;
        }
    public:

        Button(std::string Text, std::function<void (Button* This)> press = [](Button* This){}) : Text_Field(Text){
            Defualt_Button_Behaviour(press);
            Default_Button_Text_Align();
            Enable_Input_Overflow();
            Dirty.Dirty(STAIN_TYPE::TEXT);
            Show_Border(true);
            Set_Name(Text);
        }

    };
}

#endif