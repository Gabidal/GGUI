#ifndef _ELEMENT_H_
#define _ELEMENT_H_

#include <string>
#include <vector>
#include <map>
#include <functional>
#include <chrono>

//GGUI uses the ANSI escape code
//https://en.wikipedia.org/wiki/ANSI_escape_code
namespace GGUI{
    namespace SYMBOLS{
        inline std::string TOP_LEFT_CORNER = "\e(0\x6c\e(B";
        inline std::string BOTTOM_LEFT_CORNER = "\e(0\x6d\e(B";
        inline std::string TOP_RIGHT_CORNER = "\e(0\x6b\e(B";
        inline std::string BOTTOM_RIGHT_CORNER = "\e(0\x6a\e(B";
        inline std::string VERTICAL_LINE = "\e(0\x78\e(B";
        inline std::string HORIZONTAL_LINE = "\e(0\x71\e(B";
    }

    namespace Constants{
        inline std::string ESC = "\e[";
        inline std::string SEPERATE = ";";
        inline std::string TEXT_COLOUR = "38";
        inline std::string BACK_GROUND_COLOUR = "48";
        inline std::string RESET_TEXT_COLOUR;
        inline std::string RESET_BACK_GROUND_COLOUR;
        inline std::string USE_RGB = "2";
        inline std::string END_COMMAND = "m";
        inline std::string CLEAR_SCREEN = ESC + "2J";

        inline unsigned long long NON = 1 << 0;
        inline unsigned long long ENTER = 1 << 1;
        inline unsigned long long ESCAPE = 1 << 2;
        inline unsigned long long BACKSPACE = 1 << 3;
        inline unsigned long long TAB = 1 << 4;
        inline unsigned long long UP = 1 << 5;
        inline unsigned long long DOWN = 1 << 6;
        inline unsigned long long LEFT = 1 << 7;
        inline unsigned long long RIGHT = 1 << 8;
        inline unsigned long long SPACE = 1 << 9;
        inline unsigned long long SHIFT = 1 << 10;

        inline unsigned long long KEY_PRESS = 1 << 9;

        inline void Init();
    }

    namespace TIME{
        inline const  unsigned int MILLISECOND = 1; 
        inline const  unsigned int SECOND = MILLISECOND * 1000;
        inline const  unsigned int MINUTE = SECOND * 60;
        inline const  unsigned int HOUR = MINUTE * 60;
    }

    class RGB{
    public:
        union{
            unsigned char Red = 0;
            unsigned char R;
        };
        union{
            unsigned char Green = 0;
            unsigned char G;
        };
        union{
            unsigned char Blue = 0;
            unsigned char B;
        };

        RGB(unsigned char r, unsigned char g, unsigned char b){
            R = r;
            G = g;
            B = b;
        }

        RGB(){}

        std::string Get_Colour(){
            return std::to_string(Red) + Constants::SEPERATE + std::to_string(Green) + Constants::SEPERATE + std::to_string(Blue);
        }
    
        std::string Get_Over_Head(bool Is_Text_Color = true){
            if(Is_Text_Color){
                return Constants::ESC + Constants::TEXT_COLOUR + Constants::SEPERATE + Constants::USE_RGB + Constants::SEPERATE;
            }
            else{
                return Constants::ESC + Constants::BACK_GROUND_COLOUR + Constants::SEPERATE + Constants::USE_RGB + Constants::SEPERATE;
            }
        }
    
        bool operator==(const RGB& Other){
            return (Red == Other.Red) && (Green == Other.Green) && (Blue == Other.Blue);
        }

    };

    namespace COLOR{
        static const RGB WHITE = RGB(255, 255, 255);
        static const RGB BLACK = RGB(0, 0, 0);
        static const RGB RED = RGB(255, 0, 0);
        static const RGB GREEN = RGB(0, 255, 0);
        static const RGB BLUE = RGB(0, 0, 255);
        static const RGB YELLOW = RGB(255, 255, 0);
        static const RGB CYAN = RGB(0, 255, 255);
        static const RGB MAGENTA = RGB(255, 0, 255);
        static const RGB GRAY = RGB(128, 128, 128);
        static const RGB LIGHT_RED = RGB(255, 0, 0);
        static const RGB LIGHT_GREEN = RGB(0, 255, 0);
        static const RGB LIGHT_BLUE = RGB(0, 0, 255);
        static const RGB LIGHT_YELLOW = RGB(255, 255, 0);
        static const RGB LIGHT_CYAN = RGB(0, 255, 255);
        static const RGB LIGHT_MAGENTA = RGB(255, 0, 255);
        static const RGB LIGHT_GRAY = RGB(192, 192, 192);
        static const RGB DARK_RED = RGB(128, 0, 0);
        static const RGB DARK_GREEN = RGB(0, 128, 0);
        static const RGB DARK_BLUE = RGB(0, 0, 128);
        static const RGB DARK_YELLOW = RGB(128, 128, 0);
        static const RGB DARK_CYAN = RGB(0, 128, 128);
        static const RGB DARK_MAGENTA = RGB(128, 0, 128);
        static const RGB DARK_GRAY = RGB(64, 64, 64);
    }

    void Constants::Init(){
        RESET_TEXT_COLOUR = ESC + TEXT_COLOUR + SEPERATE + USE_RGB + SEPERATE + RGB(255, 255, 255).Get_Colour() + END_COMMAND;
        RESET_BACK_GROUND_COLOUR = ESC + BACK_GROUND_COLOUR + SEPERATE + USE_RGB + SEPERATE + RGB(0, 0, 0).Get_Colour() + END_COMMAND;
    }

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

        UTF(char data, std::string pre_fix = ""){
            Ascii = data;
            Pre_Fix = pre_fix;

            if (pre_fix != "")
                Post_Fix = Constants::RESET_TEXT_COLOUR + Constants::RESET_BACK_GROUND_COLOUR;
        }

        UTF(std::string data, std::string pre_fix = ""){
            Unicode = data;
            Pre_Fix = pre_fix;

            if (pre_fix != "")
                Post_Fix = Constants::RESET_TEXT_COLOUR + Constants::RESET_BACK_GROUND_COLOUR;
            
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

    class Event{
    public:
        unsigned long long Criteria;
    };

    class Input : public Event{
    public:
        char Data = 0;
        unsigned int X = 0;
        unsigned int Y = 0;
        int Scale = 1;

        Input(char d, unsigned long long t){
            Data = d;
            Criteria = t;
        }

        Input(Coordinates c, unsigned long long t, int s = 1){
            X = c.X;
            Y = c.Y;
            Criteria = t;
            Scale = s;
        }
    };

    class Action : public Event{
    public:
        class Element* Host;

        std::function<void(GGUI::Event* e)> Job;
    
        Action(){}
        Action(unsigned long long criteria, std::function<void(GGUI::Event* e)> job){
            Criteria = criteria;
            Job = job;
        }

        Action(unsigned long long criteria, std::function<void(GGUI::Event* e)> job, class Element* host){
            Criteria = criteria;
            Job = job;
            Host = host;
        }
    };

    class Memory : public Action{
    public:
        size_t Start_Time = 0;
        size_t End_Time = 0;

        Memory(size_t end, std::function<void(GGUI::Event* e)>job){
            Start_Time = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
            End_Time = end;
            Job = job;
        }
    };

    class Flags{
    public:
        Coordinates Position;
        int Width = 0;
        int Height = 0;
        bool Border = false;

        RGB Text_Colour = RGB(255, 255, 255);
        RGB Back_Ground_Colour = RGB(0, 0, 0);

        RGB Border_Colour = RGB(255, 255, 255);
        RGB Border_Back_Ground_Color = RGB(0, 0, 0);

        RGB Text_Focus_Color = RGB(255, 255, 255);
        RGB Back_Ground_Focus_Color = RGB(50, 50, 50);

        RGB Border_Focus_Color = RGB(255, 255, 255);
        RGB Border_Focus_Back_Ground_Color = RGB(50, 50, 50);
        
        //INTERNAL FLAGS
        class Element* Parent = nullptr;
        bool Dirty = false;

    };

    class Element : public Flags{
    public:
        std::vector<Element*> Childs;

        bool Focused = false;

        Element() {}

        std::pair<int, int> Get_Fitting_Dimensions(Element* child);

        virtual void Show_Border(bool b);

        bool Has_Border();

        virtual void Add_Child(Element* Child);

        std::vector<Element*>& Get_Childs();

        bool Remove(Element* handle);

        bool Remove(int index);

        void Set_Dimensions(int width, int height);

        int Get_Width();

        int Get_Height();

        void Set_Position(Coordinates c);

        Coordinates Get_Position();

        Coordinates Get_Absolute_Position();

        void Set_Back_Ground_Colour(RGB color);

        RGB Get_Back_Ground_Colour();
        
        void Set_Border_Colour(RGB color);
        
        RGB Get_Border_Colour();

        void Set_Border_Back_Ground_Color(RGB color);
        
        RGB Get_Border_Back_Ground_Color();
        
        void Set_Text_Colour(RGB color);
        
        RGB Get_Text_Colour();

        virtual std::vector<UTF> Render();

        virtual void Update_Parent(Element* deleted){
            //normally elements dont do anything
            if (Parent)
                Parent->Update_Parent(deleted);
        }

        virtual void Add_Overhead(Element* w, std::vector<UTF>& Result);

        virtual void Apply_Colors(Element* w, std::vector<UTF>& Result);

        virtual bool Resize_To(Element* parent){
            return false;
        }

        void Nest_Element(Element* Parent, Element* Child, std::vector<UTF>& Parent_Buffer, std::vector<UTF> Child_Buffer);

        std::string Compose_All_Text_RGB_Values(){
            if (Focused){
                return Text_Focus_Color.Get_Over_Head(true) + 
                Text_Focus_Color.Get_Colour() + 
                Constants::END_COMMAND + 
                Back_Ground_Focus_Color.Get_Over_Head(false) + 
                Back_Ground_Focus_Color.Get_Colour() +
                Constants::END_COMMAND;
            }
            else{
                return Text_Colour.Get_Over_Head(true) + 
                Text_Colour.Get_Colour() + 
                Constants::END_COMMAND + 
                Back_Ground_Colour.Get_Over_Head(false) + 
                Back_Ground_Colour.Get_Colour() +
                Constants::END_COMMAND;
            }
        }

        std::string Compose_All_Border_RGB_Values(){
            if (Focused){
                return Border_Focus_Color.Get_Over_Head(true) + 
                Border_Focus_Color.Get_Colour() + 
                Constants::END_COMMAND + 
                Border_Focus_Back_Ground_Color.Get_Over_Head(false) + 
                Border_Focus_Back_Ground_Color.Get_Colour() +
                Constants::END_COMMAND;
            }
            else{
                return Border_Colour.Get_Over_Head(true) + 
                Border_Colour.Get_Colour() + 
                Constants::END_COMMAND + 
                Border_Back_Ground_Color.Get_Over_Head(false) + 
                Border_Back_Ground_Color.Get_Colour() +
                Constants::END_COMMAND;
            }
        }

        virtual std::string Get_Name(){
            return "Element";
        }

        
        virtual Element* Copy();

        //Makes suicide.
        void Remove(){
            if (Parent){
                Update_Parent(this);
                Parent->Remove(this);
            }
        }

        //Event handlers
        void On_Click(std::function<void(GGUI::Event* e)> action);

    


    };
}

#endif