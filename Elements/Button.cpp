#include "Button.h"
#include "HTML.h"

namespace GGUI{

    Element* Translate_Button(HTML_Node* input){
        if (input->Tag_Name != "button")
            return nullptr;

        //Button* Result = new Button();



    }

    GGUI_Add_Translator("button", Translate_Button);

}