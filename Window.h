#ifndef _WINDOW_H_
#define _WINDOW_H_

#include <string>
#include <vector>

using namespace std;

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

class Text{
public:
    string Pre_Fix = "";    //color markings and bold info
    string Post_Fix = "";   //color reset and bold resert

    bool Is_Unicode = false;

    union{
        char Ascii;
        short Unicode;
    };
    
    Text(char data = 0, string pre_fix = "", string post_fix = ""){
        Ascii = data;
        Pre_Fix = pre_fix;
        Post_Fix = post_fix;
    }

    Text(short data = 0, string pre_fix = "", string post_fix = ""){
        Unicode = data;
        Pre_Fix = pre_fix;
        Post_Fix = post_fix;
        
        Is_Unicode = true;
    }

    string To_String(){
        if(Is_Unicode){
            return Pre_Fix + to_string(Ascii) + Post_Fix;
        }
        else{
            return Pre_Fix + Ascii + Post_Fix;
        }
    }

};

class Info : public Coordinates{
public:
    int Width = 0;
    int Height = 0;
    
    class Window* Parent = nullptr;
};

class Element;
class Window : public Info{
public:
    string Title = "";  //if this is empty then no title
    vector<Element*> Childs;

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

    int Get_Absolute_Y();

};

class Element : public Info{
public:
    union{
        class Text Text;
        class Window* Window;
    };
};

#endif