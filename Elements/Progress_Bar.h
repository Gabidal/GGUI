#ifndef _PROGRESS_BAR_H_
#define _PROGRESS_BAR_H_

#include "Element.h"

namespace GGUI{

    class PROGRESS_STYLE{
    public:
        const Compact_String Head;
        const Compact_String Body;
        const Compact_String Tail;

        PROGRESS_STYLE(
            const char* head = GGUI::SYMBOLS::CENTERED_HORIZONTAL_LINE.data(),
            const char* body = GGUI::SYMBOLS::CENTERED_HORIZONTAL_LINE.data(),
            const char* tail = GGUI::SYMBOLS::CENTERED_HORIZONTAL_LINE.data()
        ) : Head(head), Body(body), Tail(tail){}

    };

    class Progress_Bar : public Element{
    protected:
        float Progress = 0; // 0.0 - 1.0

        PROGRESS_STYLE Progress_Style;

        std::vector<UTF> Content;
    public:
        Progress_Bar(unsigned int width, unsigned int height, RGB fill_color, RGB background_color, PROGRESS_STYLE style = PROGRESS_STYLE());
        Progress_Bar() = default;

        // Returns the head index based on the progress.
        unsigned int Get_Index_of_Head();

        // Initializes the Content buffer.
        void Populate_Content();

        std::vector<GGUI::UTF>& Render() override;

        void Set_Progress(float New_Progress);
        float Get_Progress();

        void Show_Border(bool state) override;

        ~Progress_Bar() override{
            // call the base destructor.
            Element::~Element();
        }
        
        Element* Safe_Move() override {
            Progress_Bar* new_Progress_Bar = new Progress_Bar();
            std::memcpy(new_Progress_Bar, this, sizeof(this));

            return new_Progress_Bar;
        }

        std::string Get_Name() const override{
            return "Progress_Bar<" + Name + ">";
        }
    };

}

#endif