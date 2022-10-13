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

        //cache
        unsigned int Last_Child_X = 0;
        unsigned int Last_Child_Y = 0;

        std::vector<std::pair<unsigned int, unsigned int>> Layer_Peeks;

        List_View(std::map<std::string, VALUE*> css = {}, unsigned int width = 0, unsigned int height = 0, Element* parent = nullptr, Coordinates position = {0, 0, 0}) : Element(css){
        
            Set_Width(width);
            Set_Height(height);
            Set_Parent(parent);
            if (parent)
                Set_Position(position);

        }

        void Add_Child(Element* e) override;
        
        //std::vector<UTF> Render() override;

        std::string Get_Name() override;

        void Update_Parent(Element* deleted) override;

        bool Remove(Element* e) override;

        Element* Copy() override;

    };
}

#endif