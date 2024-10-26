#ifndef _LIST_VIEW_H_
#define _LIST_VIEW_H_

#include "Element.h"

namespace GGUI{
    class List_View : public Element{
    public:
        //We can always assume that the list starts from the upper left corner, right?
        Element* Last_Child = new Element(Styling(position(0, 0) | width(0) | height(0)));

        List_View() : Element(){ Allow_Dynamic_Size(true); }

        List_View(Styling s) : Element(s){ Allow_Dynamic_Size(true); }

        ~List_View() override{
            for (Element* e : Style->Childs){
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
        
        // Given offset will determine where the calculation will start from.
        void Calculate_Childs_Hitboxes(unsigned int Starting_Offset = 0) override;

        std::string Get_Name() const override;

        bool Remove(Element* e) override;

        void Set_Flow_Direction(DIRECTION gd){
            Style->Flow_Priority = gd;
        }

        DIRECTION Get_Flow_Direction(){
            return (DIRECTION)Style->Flow_Priority.Value;
        }

        template<typename  T>
        T* Get(int index){
            if (index > (signed)Style->Childs.size() - 1)
                return nullptr;

            if (index < 0)
                index = (signed)Style->Childs.size() + index - 1;

            return (T*)this->Style->Childs[index];
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
        Scroll_View(Styling s) : Element(s){}

        Scroll_View(List_View& container);

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

        void Set_Growth_Direction(DIRECTION gd){
            ((List_View*)Style->Childs[0])->Set_Flow_Direction(gd);
        }

        DIRECTION Get_Growth_Direction(){
            return ((List_View*)Style->Childs[0])->Get_Flow_Direction();
        }

        template<typename  T>
        T* Get(int index){
            return ((List_View*)Style->Childs[0])->Get<T>(index);
        }

        List_View* Get_Container(){
            return (List_View*)Style->Childs[0];
        }
    
    };
}

#endif