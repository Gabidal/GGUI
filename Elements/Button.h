#ifndef _BUTTON_H_
#define _BUTTON_H_

#include <vector>
#include <string>

#include <functional>

#include "Text_Field.h"

namespace GGUI{

    class Button : public Text_Field{
        void Default_Border_Init(){
            Border_Focus_Color = COLOR::WHITE;
            Border_Focus_Back_Ground_Color = COLOR::GRAY;
        }

        void Defualt_Button_Behaviour(std::function<void (Button* This)> press = [=](Button* This){}){
            On_Click([=](Event* e){
                // The default, on_click wont do anything.
                press(this);
            });
        }
    
        void Default_Button_Text_Align(){
            Text_Position = TEXT_LOCATION::CENTER;
        }
    public:

        Button(std::function<void (Button* This)> press = [=](Button* This){}){
            Default_Border_Init();
            Defualt_Button_Behaviour(press);
        }

        Button(Style f, std::function<void (Button* This)> press = [=](Button* This){}){
            *((Style*)this) = f;

            Button(press);
        }

        Button(std::string Text, std::function<void (Button* This)> press = [=](Button* This){}){
            Data = Text;
            Dirty.Dirty(STAIN_TYPE::TEXT);

            Button(press);
        }

        Button(std::string Text, Style f, std::function<void (Button* This)> press = [=](Button* This){}){ 
            Button(f, press);
            Data = Text;
            Dirty.Dirty(STAIN_TYPE::TEXT);
        }

    };


}

#endif