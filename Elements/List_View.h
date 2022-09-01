#ifndef _LIST_VIEW_H_
#define _LIST_VIEW_H_

#include "Element.h"

#include <vector>

namespace GGUI{
    enum class Grow_Direction{
        ROW,
        COLUMN
    };

    class List_View : public Element{
    public:
        Grow_Direction Flow_Priority;
        bool Wrap_Overflow = false;

        //cache
        unsigned int Last_Child_X = 0;
        unsigned int Last_Child_Y = 0;

        std::vector<std::pair<unsigned int, unsigned int>> Layer_Peeks;

        List_View(){}

        List_View(Flags f, Grow_Direction flow_priority = Grow_Direction::ROW, bool wrap = false){
            *((Flags*)this) = f;
            Flow_Priority = flow_priority;
            Wrap_Overflow = wrap;
        }

        void Add_Child(Element* e) override;
        
        //std::vector<UTF> Render() override;

        std::string Get_Name() override;

        void Update_Parent(Element* deleted) override;

    };
}

#endif