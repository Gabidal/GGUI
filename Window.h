#ifndef _WINDOW_H_
#define _WINDOW_H_

#include <string>
#include <vector>
#include <map>

#include "Constants.h"

namespace GGUI{

    class Coordinates{
    public:
        int X = 0;  //Horizontal
        int Y = 0;  //Vertical
        int Z = 0;  //priority (the higher the more likely it will be at top).

        Coordinates(int x = 0, int y = 0, int z = 0){
            X = x;
            Y = y;
            Z = z;
        }

        void operator+=(Coordinates* other){
            X += other->X;
            Y += other->Y;
            Z += other->Z;
        }
    };

    class UTF{
    public:
        std::string Pre_Fix = "";    //color markings and bold info
        std::string Post_Fix = "";   //color reset and bold resert

        bool Is_Unicode = false;

        char Ascii = ' ';
        std::string Unicode = " ";


        UTF(){}

        ~UTF(){}

        UTF(char data, std::string pre_fix = "", std::string post_fix = ""){
            Ascii = data;
            Pre_Fix = pre_fix;
            Post_Fix = post_fix;
        }

        UTF(std::string data, std::string pre_fix = "", std::string post_fix = ""){
            Unicode = data;
            Pre_Fix = pre_fix;
            Post_Fix = post_fix;
            
            Is_Unicode = true;
        }

        std::string To_String(){
            if(Is_Unicode){
                return Pre_Fix + Unicode + Post_Fix;
            }
            else{
                return Pre_Fix + Ascii + Post_Fix;
            }
        }
    };

    class Flags{
    public:
        Coordinates Position;
        bool Border = false;
        int width = 0;
        int height = 0; 
        std::string Back_Ground_Colour = COLOR::BLACK;
        std::string Border_Colour = COLOR::WHITE;
        std::string Text_Colour = COLOR::WHITE;
        int Width = 0;
        int Height = 0;
        class Element* Parent = nullptr;
    };

    class Element : public Flags{
    public:
        std::vector<Element*> Childs;

        int Get_Fitting_Width(Element* child);

        int Get_Fitting_Height(Element* child);

        void Show_Border(bool b);

        bool Has_Border();

        void Add_Child(Element* Child);

        std::vector<Element*> Get_Childs();

        void Remove_Element(Element* handle);

        void Remove_Element(int index);

        void Set_Dimensions(int width, int height);

        int Get_Width();

        int Get_Height();

        void Set_Position(Coordinates c);

        Coordinates Get_Position();

        Coordinates Get_Absolute_Position();

        void Set_Back_Ground_Colour(std::string color);

        std::string Get_Back_Ground_Colour();
        
        void Set_Border_Colour(std::string color);
        
        std::string Get_Border_Colour();
        
        void Set_Text_Colour(std::string color);
        
        std::string Get_Text_Colour();

        virtual std::vector<UTF> Render();

        virtual void Add_Overhead(Element* w, std::vector<UTF>& Result);

        void Nest_Element(Element* Parent, Element* Child, std::vector<UTF>& Parent_Buffer, std::vector<UTF> Child_Buffer);
    };

    class Window : public Element{
        std::string Title = "";  //if this is empty then no title
    public:
        Window(){}

        Window(std::string title, Flags f);

        void Set_Title(std::string t);

        std::string Get_Title();

        std::vector<UTF> Render() override;
        
        void Add_Overhead(Element* w, std::vector<UTF>& Result) override;
    };

}

#endif