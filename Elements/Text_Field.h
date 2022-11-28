#ifndef _TEXT_FIELD_H_
#define _TEXT_FIELD_H_

#include <string>
#include <vector>

#include "Element.h"

namespace GGUI{
    enum class TEXT_LOCATION{
        LEFT,
        CENTER,
        RIGHT,
    };

    class Text_Field : public Element{
    protected:
        std::string Data = "";
        bool Allow_Text_Input = false;
        
    public:

        Text_Field(){}

        Text_Field(std::string Text, std::map<std::string, VALUE*> css = {});

        //These next constructors are mainly for users to more easily create elements.

        Text_Field(
            std::string Text,
            RGB text_color,
            RGB background_color
        );

        Text_Field(
            std::string Text,
            RGB text_color,
            RGB background_color,
            RGB border_color,
            RGB border_background_color
        );

        //End of user constructors.

        void Set_Data(std::string Data);

        std::string Get_Data();

        void Set_Text_Position(TEXT_LOCATION Text_Position);

        TEXT_LOCATION Get_Text_Position();
        
        void Show_Border(bool state) override;
        
        static std::pair<unsigned int, unsigned int> Get_Text_Dimensions(std::string& text); 

        std::vector<UTF> Render() override;
        
        bool Resize_To(Element* parent) override;

        std::string Get_Name() override;

        Element* Copy() override;

        //async function, 
        void Input(std::function<void(char)> Then);

        void Enable_Text_Input();

        void Disable_Text_Input();

        bool Is_Input_Allowed(){
            return Allow_Text_Input;
        }

        //Non visual updates dont need to update frame
        void Enable_Input_Overflow();

        //Non visual updates dont need to update frame
        void Disable_Input_Overflow();

        void Enable_Dynamic_Size();

        void Disable_Dynamic_Size();


        static std::vector<UTF> Center_Text(GGUI::Element* self, std::string Text, GGUI::Element* wrapper);
        static std::vector<UTF> Left_Text(GGUI::Element* self, std::string Text, GGUI::Element* wrapper);
        static std::vector<UTF> Right_Text(GGUI::Element* self, std::string Text, GGUI::Element* wrapper);
    };
}

#endif