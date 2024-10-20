#ifndef _PROGRESS_BAR_H_
#define _PROGRESS_BAR_H_

#include "Element.h"

namespace GGUI{

    class PROGRESS_STYLE{
    public:
        Compact_String Head;
        Compact_String Body;
        Compact_String Tail;

        RGB Head_Color = GGUI::COLOR::LIGHT_GRAY;
        RGB Body_Color = GGUI::COLOR::GRAY;
        RGB Tail_Color = GGUI::COLOR::GRAY;

        RGB Empty_Color = GGUI::COLOR::DARK_GRAY;

        PROGRESS_STYLE(
            const char* head = GGUI::SYMBOLS::CENTERED_HORIZONTAL_LINE.data(),
            const char* body = GGUI::SYMBOLS::CENTERED_HORIZONTAL_LINE.data(),
            const char* tail = GGUI::SYMBOLS::CENTERED_HORIZONTAL_LINE.data()
        ) : Head(head), Body(body), Tail(tail){}

        PROGRESS_STYLE(RGB fill_color, RGB empty_color) : PROGRESS_STYLE() {
            Head_Color = fill_color;
            Body_Color = fill_color;
            Tail_Color = fill_color;

            Empty_Color = empty_color;
        }

        PROGRESS_STYLE(const PROGRESS_STYLE& other){
            Head = other.Head;
            Body = other.Body;
            Tail = other.Tail;

            Head_Color = other.Head_Color;
            Body_Color = other.Body_Color;
            Tail_Color = other.Tail_Color;

            Empty_Color = other.Empty_Color;
        }

        PROGRESS_STYLE &operator=(const GGUI::PROGRESS_STYLE & other){
            // Check for self-assignment
            if (this == &other)
                return *this;

            // Copy data
            Head = other.Head;
            Body = other.Body;
            Tail = other.Tail;
            
            Head_Color = other.Head_Color;
            Body_Color = other.Body_Color;
            Tail_Color = other.Tail_Color;

            Empty_Color = other.Empty_Color;

            return *this;
        }

    };

    namespace Progress_Bar_Styles{
        extern PROGRESS_STYLE Default;
        extern PROGRESS_STYLE Blocky;
        extern PROGRESS_STYLE Arrow;
    }

    class Progress_Bar : public Element{
    protected:
        float Progress = 0; // 0.0 - 1.0

        PROGRESS_STYLE Progress_Style;

        std::vector<UTF> Content;
    public:

        Progress_Bar(Styling s) : Element(s){}

        Progress_Bar() = default;

        Progress_Bar& operator=(const Progress_Bar& other){
            Element::operator=(other);

            Progress = other.Progress;
            Progress_Style = other.Progress_Style;
            Content = other.Content;

            return *this;
        }

        // Returns the head index based on the progress.
        unsigned int Get_Index_of_Head();

        // Initializes the Content buffer.
        void Color_Bar();

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
            *new_Progress_Bar = *(Progress_Bar*)this;

            return new_Progress_Bar;
        }

        std::string Get_Name() const override{
            return "Progress_Bar<" + Name + ">";
        }
    };

}

#endif