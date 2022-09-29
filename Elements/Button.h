#ifndef _BUTTON_H_
#define _BUTTON_H_

#include <vector>
#include <string>

#include <functional>

#include "Text_Field.h"

namespace GGUI{

    class Button : public Text_Field{
    protected:
        void Defualt_Button_Behaviour(std::function<void (Button* This)> press = [=](Button* This){}){
            On_Click([=](Event* e){
                // The default, on_click wont do anything.
                press(this);
            });
        }
    
        void Default_Button_Text_Align(){
            Text_Position = TEXT_LOCATION::CENTER;
        }

        Button(bool Blank){}
    public:

        Button(std::function<void (Button* This)> press = [=](Button* This){}){
            Defualt_Button_Behaviour(press);
            Default_Button_Text_Align();
        }

        Button(std::string Text, std::function<void (Button* This)> press = [=](Button* This){}){
            Data = Text;
            Enable_Input_Overflow();
            Dirty.Dirty(STAIN_TYPE::TEXT);

            Button(press);
        }

    };
}

#endif