#ifndef _SWITCH_H_
#define _SWITCH_H_

#include <vector>
#include <string>

#include "Button.h"

namespace STYLES{

    inline std::string STATE_TRUE       = "state_true";
    inline std::string STATE_FALSE      = "state_false";

}

namespace GGUI{
    class Switch : public Button{
    protected:
        bool State = false;

    public:
        //Calls the button whitout initializing the button features.
        Switch(std::function<void (Element* This)> press = [=](Element* This){}) : Button(true){
            //By pipelining the press function from the Switch we can get the current switch state.
            Defualt_Button_Behaviour(press);

        }

        void Toggle(){
            State = !State;
        }
    };
}

#endif