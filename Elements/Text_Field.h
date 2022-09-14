#ifndef _TEXT_FIELD_H_
#define _TEXT_FIELD_H_

#include <string>
#include <vector>

#include "Element.h"

namespace GGUI{
    enum class TEXT_LOCATION{
        CENTER,
        LEFT,
        RIGHT,
    };

    class Text_Field : public Element{
        std::string Data = "";
        TEXT_LOCATION Text_Position = TEXT_LOCATION::LEFT;
        bool Allow_Text_Input = false;
        //if text can be inputted even when the text is outof bounds.
        bool Allow_Input_Overflow = false;
        bool Allow_Dynamic_Size = false;
        
    public:

        Text_Field(){}

        Text_Field(Flags f){
            *((Flags*)this) = f;
            Dirty.Dirty(STAIN_TYPE::TEXT);
        }

        Text_Field(std::string Text){
            Data = Text;

            std::pair<int, int> D = Get_Text_Dimensions(Data);
            Width = D.first;
            Height = D.second;
            Dirty.Dirty(STAIN_TYPE::TEXT);
        }

        Text_Field(std::string Text, TEXT_LOCATION Text_Position){
            Data = Text;
            this->Text_Position = Text_Position;
            
            std::pair<int, int> D = Get_Text_Dimensions(Text);
            Width = D.first;
            Height = D.second;
            Dirty.Dirty(STAIN_TYPE::TEXT);
        }
        
        Text_Field(std::string Text, Flags f, TEXT_LOCATION Text_Position = TEXT_LOCATION::LEFT){
            Data = Text;
            *((Flags*)this) = f;
            this->Text_Position = Text_Position;
            
            std::pair<unsigned int, unsigned int> D = Get_Text_Dimensions(Text);
            Width = D.first;
            Height = D.second;

            if (Border){
                Width += 2;
                Height += 2;
            }
            Dirty.Dirty(STAIN_TYPE::TEXT);
        }

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