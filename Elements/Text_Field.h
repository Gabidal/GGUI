#ifndef _TEXT_FIELD_H_
#define _TEXT_FIELD_H_

#include "Element.h"

namespace GGUI{
    class Text_Field : public Element{
    protected:
        std::string Text = "";

        // This will hold the text by lines, and does not re-allocate memory for whole text, only for indicies.
        std::vector<Compact_String> Text_Cache; 

        void Update_Text_Cache();
    public:

        Text_Field(std::string text, Styling s = STYLES::CONSTANTS::Default) : Element(s), Text(text){

            // Since Styling Height and Width are defaulted to 1, we can use this one row to reserve for one line.
            Text_Cache.reserve(Get_Height());

            if (Get_Width() == 1 && Get_Height() == 1){
                Allow_Dynamic_Size(true);
            }

            Update_Text_Cache();
        }

        void Set_Size_To_Fill_Parent();

        Text_Field() = default;

        void Set_Text(std::string text);

        std::string Get_Text(){
            return Text;
        }

        std::vector<GGUI::UTF>& Render() override;

        void Align_Text_Left(std::vector<UTF>& Result);
        void Align_Text_Right(std::vector<UTF>& Result);
        void Align_Text_Center(std::vector<UTF>& Result);

        // For custom input handling:
        void Input(std::function<void(char)> Then);
    };
}

#endif