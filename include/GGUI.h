#pragma once
#ifndef _GGUI_H_
#define _GGUI_H_

//This header is used with the GGUI.lib.


#include <string>
#include <vector>
#include <map>
#include <functional>
#include <chrono>
#include <atomic>

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
        inline std::string Text_Color = "38";
        inline std::string Back_Ground_Color = "48";
        inline std::string RESET_Text_Color;
        inline std::string RESET_Back_Ground_Color;
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

        //key_Press includes [a-z, A-Z] & [0-9]
        inline unsigned long long KEY_PRESS = 1 << 11;

        inline void Init();
    }

    namespace TIME{
        inline constexpr static  unsigned int MILLISECOND = 1; 
        inline constexpr static  unsigned int SECOND = MILLISECOND * 1000;
        inline constexpr static  unsigned int MINUTE = SECOND * 60;
        inline constexpr static  unsigned int HOUR = MINUTE * 60;
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
                return Constants::ESC + Constants::Text_Color + Constants::SEPERATE + Constants::USE_RGB + Constants::SEPERATE;
            }
            else{
                return Constants::ESC + Constants::Back_Ground_Color + Constants::SEPERATE + Constants::USE_RGB + Constants::SEPERATE;
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
        RESET_Text_Color = ESC + Text_Color + SEPERATE + USE_RGB + SEPERATE + RGB(255, 255, 255).Get_Colour() + END_COMMAND;
        RESET_Back_Ground_Color = ESC + Back_Ground_Color + SEPERATE + USE_RGB + SEPERATE + RGB(0, 0, 0).Get_Colour() + END_COMMAND;
    }

    class Coordinates{
    public:
        unsigned int X = 0;  //Horizontal
        unsigned int Y = 0;  //Vertical
        unsigned int Z = 0;  //priority (the higher the more likely it will be at top).

        Coordinates(unsigned int x = 0, unsigned int y = 0, unsigned int z = 0){
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
                Post_Fix = Constants::RESET_Text_Color + Constants::RESET_Back_Ground_Color;
        }

        UTF(std::string data, std::string pre_fix = ""){
            Unicode = data;
            Pre_Fix = pre_fix;

            if (pre_fix != "")
                Post_Fix = Constants::RESET_Text_Color + Constants::RESET_Back_Ground_Color;
            
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

        std::function<bool(GGUI::Event* e)> Job;
    
        Action(){}
        Action(unsigned long long criteria, std::function<bool(GGUI::Event* e)> job){
            Criteria = criteria;
            Job = job;
        }

        Action(unsigned long long criteria, std::function<bool(GGUI::Event* e)> job, class Element* host){
            Criteria = criteria;
            Job = job;
            Host = host;
        }
    };

    class Memory : public Action{
    public:
        std::chrono::high_resolution_clock::time_point Start_Time;
        size_t End_Time = 0;

        Memory(size_t end, std::function<bool(GGUI::Event* e)>job){
            Start_Time = std::chrono::high_resolution_clock::now();
            End_Time = end;
            Job = job;
        }
    };

    class Style{
    public:
        Coordinates Position;
        unsigned int Width = 0;
        unsigned int Height = 0;
        bool Border = false;

        RGB Text_Color = RGB(255, 255, 255);
        RGB Back_Ground_Color = RGB(0, 0, 0);

        RGB Border_Colour = RGB(255, 255, 255);
        RGB Border_Back_Ground_Color = RGB(0, 0, 0);

        RGB Text_Focus_Color = RGB(255, 255, 255);
        RGB Back_Ground_Focus_Color = RGB(50, 50, 50);

        RGB Border_Focus_Color = RGB(255, 255, 255);
        RGB Border_Focus_Back_Ground_Color = RGB(50, 50, 50);
        
        //INTERNAL FLAGS
        class Element* Parent = nullptr;
        bool Show = true;
    };

    enum class STAIN_TYPE{
        CLEAN = 0,        //No change
        COLOR = 1 << 0,  //BG and other color related changes
        EDGE = 1 << 1,   //title and border changes.
        DEEP = 1 << 2,   //children changes. Deep because the childs are connected via AST.
        STRECH = 1 << 3,  //width and or height changes.
        TEXT = 1 << 4,   //text changes, this is primarily for text_field
    };
 
    inline unsigned int operator|(STAIN_TYPE a, STAIN_TYPE b){
        return (unsigned int)a | (unsigned int)b;
    }

    inline unsigned int operator|(STAIN_TYPE a, unsigned int b){
        return (unsigned int)a | b;
    }

    inline unsigned int operator|(unsigned int a, STAIN_TYPE b){
        return a | (unsigned int)b;
    }

    class STAIN{
    public:
        STAIN_TYPE Type = (STAIN_TYPE)(STAIN_TYPE::COLOR | STAIN_TYPE::EDGE | STAIN_TYPE::DEEP | STAIN_TYPE::STRECH);

        bool is(STAIN_TYPE f){
            return ((unsigned int)Type & (unsigned int)f) == (unsigned int)f;
        }

        void Clean(STAIN_TYPE f){
            Type = (STAIN_TYPE)((unsigned int)Type & ~(unsigned int)f);
        }

        void Dirty(STAIN_TYPE f){
            Type = (STAIN_TYPE)((unsigned int)Type | (unsigned int)f);
        }

        void Dirty(unsigned int f){
            Type = (STAIN_TYPE)((unsigned int)Type | f);
        }

        void Stain_All(){
            Dirty((STAIN_TYPE::COLOR | STAIN_TYPE::EDGE | STAIN_TYPE::DEEP | STAIN_TYPE::STRECH));
        }

    };

    class Element : public Style{
    public:
        std::vector<Element*> Childs;

        bool Focused = false;

        //Internal
        std::vector<UTF> Render_Buffer;
        STAIN Dirty;

        Element() {}

        //returns borders in mind.
        std::pair<unsigned int, unsigned int> Get_Fitting_Dimensions(Element* child);

        virtual void Show_Border(bool b);

        bool Has_Border();

        void Display(bool f);

        bool Is_Displayed(){
            return Show;
        }

        virtual void Add_Child(Element* Child);

        bool Children_Changed();

        std::vector<Element*>& Get_Childs();

        virtual bool Remove(Element* handle);

        bool Remove(int index);

        void Set_Dimensions(int width, int height);

        int Get_Width();

        int Get_Height();

        void Set_Position(Coordinates c);

        Coordinates Get_Position();

        Coordinates Get_Absolute_Position();

        void Set_Background_Color(RGB color);

        RGB Get_Background_Color();
        
        void Set_Border_Colour(RGB color);
        
        RGB Get_Border_Colour();

        void Set_Border_Background_Color(RGB color);
        
        RGB Get_Border_Background_Color();
        
        void Set_Text_Color(RGB color);
        
        RGB Get_Text_Color();

        virtual std::vector<UTF> Render();

        virtual void Update_Parent(Element* New_Element);

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
                return Text_Color.Get_Over_Head(true) + 
                Text_Color.Get_Colour() + 
                Constants::END_COMMAND + 
                Back_Ground_Color.Get_Over_Head(false) + 
                Back_Ground_Color.Get_Colour() +
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

        bool Has_Internal_Changes();

        virtual Element* Copy();

        //Makes suicide.
        void Remove();

        //Event handlers
        void On_Click(std::function<void(GGUI::Event* e)> action);

    


    };
    
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

        List_View(Style f, Grow_Direction flow_priority = Grow_Direction::ROW, bool wrap = false){
            *((Style*)this) = f;
            Flow_Priority = flow_priority;
            Wrap_Overflow = wrap;
        }

        void Add_Child(Element* e) override;
        
        //std::vector<UTF> Render() override;

        std::string Get_Name() override;

        void Update_Parent(Element* deleted) override;

        bool Remove(Element* e) override;

        Element* Copy() override;

    };

    enum class TEXT_LOCATION{
        CENTER,
        LEFT,
        RIGHT,
    };

    class Text_Field : public Element{
        std::string Data = "";
        TEXT_LOCATION Text_Position = TEXT_LOCATION::LEFT;
        bool Allow_Text_Input = false;
        //if text can be inputted even when the text is outof bounds.
        bool Allow_Input_Overflow = false;
        bool Allow_Dynamic_Size = false;
        
    public:

        Text_Field(){}

        Text_Field(Style f){
            *((Style*)this) = f;
            Dirty.Dirty(STAIN_TYPE::TEXT);
        }

        Text_Field(std::string Text){
            Data = Text;

            std::pair<int, int> D = Get_Text_Dimensions(Data);
            Width = D.first;
            Height = D.second;
            Dirty.Dirty(STAIN_TYPE::TEXT);
        }

        Text_Field(std::string Text, TEXT_LOCATION Text_Position){
            Data = Text;
            this->Text_Position = Text_Position;
            
            std::pair<int, int> D = Get_Text_Dimensions(Text);
            Width = D.first;
            Height = D.second;
            Dirty.Dirty(STAIN_TYPE::TEXT);
        }
        
        Text_Field(std::string Text, Style f, TEXT_LOCATION Text_Position = TEXT_LOCATION::LEFT){
            Data = Text;
            *((Style*)this) = f;
            this->Text_Position = Text_Position;
            
            std::pair<unsigned int, unsigned int> D = Get_Text_Dimensions(Text);
            Width = D.first;
            Height = D.second;

            if (Border){
                Width += 2;
                Height += 2;
            }
            Dirty.Dirty(STAIN_TYPE::TEXT);
        }

        void Set_Data(std::string Data);

        std::string Get_Data();

        void Set_Text_Position(TEXT_LOCATION Text_Position);

        TEXT_LOCATION Get_Text_Position();
        
        void Show_Border(bool state) override;
        
        static std::pair<unsigned int, unsigned int> Get_Text_Dimensions(std::string& text); 

        std::vector<UTF> Render() override;
        
        bool Resize_To(Element* parent) override;

        std::string Get_Name() override;

        Element* Copy() override;

        //async function, 
        void Input(std::function<void(char)> Then);

        void Enable_Text_Input();

        void Disable_Text_Input();

        //Non visual updates dont need to update frame
        void Enable_Input_Overflow();

        //Non visual updates dont need to update frame
        void Disable_Input_Overflow();

        void Enable_Dynamic_Size();

        void Disable_Dynamic_Size();


        static std::vector<UTF> Center_Text(GGUI::Element* self, std::string Text, GGUI::Element* wrapper);
        static std::vector<UTF> Left_Text(GGUI::Element* self, std::string Text, GGUI::Element* wrapper);
        static std::vector<UTF> Right_Text(GGUI::Element* self, std::string Text, GGUI::Element* wrapper);
    };

    class Window : public Element{
        std::string Title = "";  //if this is empty then no title
    public:
        Window(){}

        Window(std::string title, Style f);

        void Set_Title(std::string t);

        std::string Get_Title();

        std::vector<UTF> Render() override;
        
        void Add_Overhead(Element* w, std::vector<UTF>& Result) override;

        std::string Get_Name() override;

        Element* Copy() override;
    };
    
    extern std::vector<UTF> Abstract_Frame_Buffer;               //2D clean vector whitout bold nor color
    extern std::string Frame_Buffer;                                 //string with bold and color, this what gets drawn to console.
    extern std::atomic_bool Pause_Render;                     //if true, the render will not be updated, good for window creation.
    extern std::atomic_bool Pause_Event_Thread;

    extern Window Main;                                   //Main window

    extern int Max_Width;
    extern int Max_Height;

    extern std::vector<Memory> Remember;

    extern std::vector<Action*> Event_Handlers;
    extern std::vector<Input*> Inputs;

    extern Element* Focused_On;

    extern Coordinates Mouse;    
    //move 1 by 1, or element by element.
    extern bool Mouse_Movement_Method;

    extern time_t UPDATE_SPEED_MIILISECONDS;
    extern int Inputs_Per_Second;
    extern int Inputs_Per_Query;


    extern bool Collides(GGUI::Element* a, GGUI::Element* b);

    extern bool Collides(GGUI::Element* a, GGUI::Coordinates b);

    extern Element* Get_Accurate_Element_From(Coordinates c, Element* Parent);

    extern bool Find_Upper_Element();

    extern bool Find_Lower_Element();

    extern bool Find_Left_Element();

    extern bool Find_Right_Element();


#if _WIN32
    #include <windows.h>
    
    #undef RGB
    #undef BOOL
    #undef NUMBER
    #undef min
    #undef max

    extern void ClearScreen();

    extern void Render_Frame();

    extern void Update_Max_Width_And_Height();

    void Update_Frame();
    //Is called on every cycle.
    extern void Query_Inputs();

    extern void Init_Platform_Stuff();

#else
    extern void ClearScreen();

    extern void Render_Frame();

    extern void Update_Max_Width_And_Height();
    //Is called on every cycle.
    extern void Query_Inputs();

#endif

    extern int Get_Max_Width();

    extern int Get_Max_Height();

    //Returns a char if given ASCII, or a short if given UNICODE
    extern GGUI::UTF* Get(GGUI::Coordinates Abselute_Position);

    extern std::string Liquify_UTF_Text(std::vector<GGUI::UTF> Text, int Width, int Height);

    extern void Update_Frame();
    
    extern void Pause_Renderer();

    extern void Resume_Renderer();

    extern void Recall_Memories();

    extern bool Is(unsigned long long f, unsigned long long Flag);

    extern void Un_Focus_Element();

    extern void Update_Focused_Element(GGUI::Element* new_candidate);

    extern void Event_Handler();

    //Inits GGUI and returns the main window.
    extern GGUI::Window* Init_Renderer();

    extern void Report(std::string Problem);

    extern void Nest_UTF_Text(GGUI::Element* Parent, GGUI::Element* child, std::vector<GGUI::UTF> Text, std::vector<GGUI::UTF>& Parent_Buffer);
}

#endif