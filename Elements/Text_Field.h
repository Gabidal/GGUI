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
    protected:
        std::string Data = "";
        TEXT_LOCATION Text_Position = TEXT_LOCATION::LEFT;
        bool Allow_Text_Input = false;
        //if text can be inputted even when the text is outof bounds.
        bool Allow_Input_Overflow = false;
        bool Allow_Dynamic_Size = false;
        
    public:

        Text_Field(){}

        Text_Field(std::string Text, std::map<std::string, VALUE*> css = {}, TEXT_LOCATION Text_Position = TEXT_LOCATION::LEFT) : Element(css) {
            Data = Text;
            this->Text_Position = Text_Position;
            
            std::pair<int, int> D = Get_Text_Dimensions(Text);

            if (At<NUMBER_VALUE>(STYLES::Width)->Value == 0 || At<NUMBER_VALUE>(STYLES::Width)->Value < D.first){
                At<NUMBER_VALUE>(STYLES::Width)->Value = D.first;
            }
            if (At<NUMBER_VALUE>(STYLES::Height)->Value == 0 || At<NUMBER_VALUE>(STYLES::Height)->Value < D.second){
                At<NUMBER_VALUE>(STYLES::Height)->Value = D.second;
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