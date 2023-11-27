#ifndef _PROGRESS_BAR_H_
#define _PROGRESS_BAR_H_

#include "Element.h"

namespace GGUI{

    class Progress_Bar : public Element{
    protected:
        float Progress = 0; // 0.0 - 1.0

        void Add_Horizontal_Lines(std::vector<UTF>& buffer);
        std::vector<UTF> Render() override;
        void Apply_Colors(Element* w, std::vector<UTF>& Result) override;
    public:
        void Set_Progress(float New_Progress);
        float Get_Progress();

        void Show_Border(bool state);

        void Set_Fill_Color(RGB value);
        void Set_Empty_Color(RGB value);
    
        Progress_Bar();
        Progress_Bar(unsigned int Width, unsigned int Height = 1);
        Progress_Bar(RGB Fill_Color, RGB Empty_Color);
        Progress_Bar(RGB Fill_COlor, RGB Empty_Color, unsigned int Width, unsigned int Height = 1);

    };

}

#endif