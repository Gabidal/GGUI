#ifndef _LIST_VIEW_H_
#define _LIST_VIEW_H_

#include "Element.h"

#include <vector>
#include <iostream>

using namespace std;

namespace GGUI{
    enum class Grow_Direction{
        ROW,
        COLUMN
    };

    class List_View : public Element{
    public:

        //cache
        Element* Last_Child = new Element(0, 0, {0, 0});

        std::vector<std::pair<unsigned int, unsigned int>> Layer_Peeks;

        List_View(std::map<std::string, VALUE*> css = {}, unsigned int width = 0, unsigned int height = 0, Element* parent = nullptr, Coordinates position = {0, 0, 0});

        //These next constructors are mainly for users to more easily create elements.
        List_View(
            RGB text_color,
            RGB background_color
        );

        List_View(
            unsigned int width,
            unsigned int height,
            RGB text_color,
            RGB background_color
        );

        List_View(
            unsigned int width,
            unsigned int height,
            RGB text_color,
            RGB background_color,
            RGB border_color,
            RGB border_background_color
        );

        List_View(Element* parent, std::vector<Element*> Tree, Grow_Direction grow_direction = Grow_Direction::ROW);

        Element* Handle_Or_Operator(Element* other) override{
            Add_Child(other);
            return this;
        }

        //End of user constructors.

        void Add_Child(Element* e) override;
        
        //std::vector<UTF> Render() override;

        std::string Get_Name() override;

        void Update_Parent(Element* deleted) override;

        bool Remove(Element* e) override;

        Element* Copy() override;

        void Set_Growth_Direction(Grow_Direction gd){
            At<NUMBER_VALUE>(STYLES::Flow_Priority)->Value = (int)gd;
        }

        Grow_Direction Get_Growth_Direction(){
            return (Grow_Direction)At<NUMBER_VALUE>(STYLES::Flow_Priority)->Value;
        }

    };
}

#endif