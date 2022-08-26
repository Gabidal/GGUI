#ifndef _WINDOW_H_
#define _WINDOW_H_

#include <string>
#include <vector>

namespace WINDOW{

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

        union{
            char Ascii;
            int Unicode;
        };

        UTF(){
            Unicode = 0;
        }
        
        UTF(char data, std::string pre_fix = "", std::string post_fix = ""){
            Ascii = data;
            Pre_Fix = pre_fix;
            Post_Fix = post_fix;
        }

        UTF(int data, std::string pre_fix = "", std::string post_fix = ""){
            Unicode = data;
            Pre_Fix = pre_fix;
            Post_Fix = post_fix;
            
            Is_Unicode = true;
        }

        std::string To_String(){
            if(Is_Unicode){
                return Pre_Fix + std::to_string(Ascii) + Post_Fix;
            }
            else{
                //transform short into char[2]
                return Pre_Fix + to_string(Unicode) + Post_Fix;
            }
        }

    };

    class Info : public Coordinates{
    public:
        int Width = 0;
        int Height = 0;
        
        class Window* Parent = nullptr;
    };

    class Window : public Info{
    public:
        std::string Title = "";  //if this is empty then no title


        std::vector<Window*> Childs;
        std::vector<UTF*> Content;

        bool Show_Border = true;

        Coordinates Get_Absolute_Position(){
            Coordinates Result = {0, 0};
            
            Window* current_window = this;
            Result += current_window;
            current_window = current_window->Parent;

            while (current_window != nullptr){
                Result += current_window;
                current_window = current_window->Parent;
            }

            return Result;
        }

    };



}

#endif