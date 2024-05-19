#ifndef _LIST_VIEW_H_
#define _LIST_VIEW_H_

#include "Element.h"

namespace GGUI{
    enum class Grow_Direction{
        ROW,
        COLUMN
    };

    class List_View : public Element{
    public:
        //We can always assume that the list starts from the upper left corner, right?
        Element* Last_Child = new Element(0, 0, {0, 0});

        std::vector<std::pair<unsigned int, unsigned int>> Layer_Peeks;

        List_View(Styling css = {}, unsigned int width = 0, unsigned int height = 0, Element* parent = nullptr, Coordinates position = {0, 0, 0});

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

        ~List_View() override{
            for (Element* e : Childs){
                delete e;
            }

            // call the base destructor.
            Element::~Element();
        }

        Element* Handle_Or_Operator(Element* other) override{
            Add_Child(other);
            return this;
        }

        //End of user constructors.

        void Add_Child(Element* e) override;
        
        std::string Get_Name() const override;

        bool Remove(Element* e) override;

        void Set_Growth_Direction(Grow_Direction gd){
            Style->Flow_Priority = (int)gd;
        }

        Grow_Direction Get_Growth_Direction(){
            return (Grow_Direction)Style->Flow_Priority.Value;
        }

        template<typename  T>
        T* Get(int index){
            if (index > (signed)Childs.size() - 1)
                return nullptr;

            if (index < 0)
                index = (signed)Childs.size() + index - 1;

            return (T*)this->Childs[index];
        }

        Element* Safe_Move() override {
            List_View* new_List_View = new List_View();
            *new_List_View = *(List_View*)this;

            return new_List_View;
        }

    };

    class Scroll_View : public Element{
    protected:
        unsigned int Scroll_Index = 0;  // Render based on the offset of the scroll_index by flow direction.
    public:

        // Constructors:
        // -_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_
        Scroll_View(Grow_Direction grow_direction = Grow_Direction::ROW);

        Scroll_View(List_View& container);

        Scroll_View(std::vector<Element*> Childs, Grow_Direction grow_direction = Grow_Direction::ROW);

        Scroll_View(Styling css = {}, unsigned int width = 0, unsigned int height = 0, Element* parent = nullptr, Coordinates position = {0, 0, 0});

        //These next constructors are mainly for users to more easily create elements.
        Scroll_View(
            RGB text_color,
            RGB background_color
        );

        Scroll_View(
            unsigned int width,
            unsigned int height,
            RGB text_color,
            RGB background_color
        );

        Scroll_View(
            unsigned int width,
            unsigned int height,
            RGB text_color,
            RGB background_color,
            RGB border_color,
            RGB border_background_color
        );

        Scroll_View(Element* parent, std::vector<Element*> Tree, Grow_Direction grow_direction = Grow_Direction::ROW);

        // Re-pipeline functions:
        // -_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_
        void Add_Child(Element* e) override;

        void Allow_Scrolling(bool allow);
    
        bool Is_Scrolling_Enabled(){
            return Style->Allow_Scrolling.Value;
        }

        void Scroll_Up() override;

        void Scroll_Down() override;

        bool Remove(Element* e) override;

        std::string Get_Name() const override;

        void Set_Growth_Direction(Grow_Direction gd){
            ((List_View*)Childs[0])->Set_Growth_Direction(gd);
        }

        Grow_Direction Get_Growth_Direction(){
            return ((List_View*)Childs[0])->Get_Growth_Direction();
        }

        template<typename  T>
        T* Get(int index){
            return ((List_View*)Childs[0])->Get<T>(index);
        }

        List_View* Get_Container(){
            return (List_View*)Childs[0];
        }
    
    };
}

#endif