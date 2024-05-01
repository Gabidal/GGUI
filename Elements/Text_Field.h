#ifndef _TEXT_FIELD_H_
#define _TEXT_FIELD_H_

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
        std::string Previus_Data = "";
        bool Allow_Text_Input = false;
        
    public:

        Text_Field(){}

        Text_Field(std::string Text, std::unordered_map<std::string, VALUE*> css = {});

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

        void Fully_Stain() override;

        void Set_Data(std::string Data);

        std::string Get_Data();

        void Set_Text_Position(TEXT_LOCATION Text_Position);

        TEXT_LOCATION Get_Text_Position();
        
        void Show_Border(bool state) override;
        
        static std::pair<unsigned int, unsigned int> Get_Text_Dimensions(std::string& text); 

        std::vector<UTF> Render() override;
        
        bool Resize_To(Element* parent) override;

        std::string Get_Name() const override;

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

        static void Center_Text(GGUI::Element* self, std::string Text, GGUI::Element* wrapper, std::vector<GGUI::UTF>& Previus_Render);
        static void Left_Text(GGUI::Element* self, std::string Text, GGUI::Element* wrapper, std::vector<GGUI::UTF>& Previus_Render);
        static void Right_Text(GGUI::Element* self, std::string Text, GGUI::Element* wrapper, std::vector<GGUI::UTF>& Previus_Render);

        Element* Safe_Move() override {
            Text_Field* new_Text_Field = new Text_Field();
            *new_Text_Field = *(Text_Field*)this;

            return new_Text_Field;
        }
    };
}

#endif