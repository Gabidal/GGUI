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

        Text_Field(std::string text, ALIGN align = ALIGN::LEFT, int width = 1, int height = 1) : Element(width, height), Text(text){
            // Reserve one row for the text if no newlines.
            Text_Cache.reserve(SETTINGS::Text_Field_Minimum_Line_Count | height);

            Style->Align.Value = align;

            if (Width == 1 && Height == 1){
                Allow_Dynamic_Size(true);
            }

            Update_Text_Cache();
        }

        Text_Field() = default;

        void Set_Text(std::string text);

        std::string Get_Text(){
            return Text;
        }

        std::vector<UTF> Render() override;

        void Align_Text_Left(std::vector<UTF>& Result);
        void Align_Text_Right(std::vector<UTF>& Result);
        void Align_Text_Center(std::vector<UTF>& Result);

    };
}

#endif