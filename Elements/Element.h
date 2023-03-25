#ifndef _ELEMENT_H_
#define _ELEMENT_H_

#include <string>
#include <vector>
#include <map>
#include <functional>
#include <chrono>
#include <atomic>

#include <iostream>

namespace GGUI{
    //GGUI uses the ANSI escape code
    //https://en.wikipedia.org/wiki/ANSI_escape_code

    namespace SYMBOLS{
        inline std::string TOP_LEFT_CORNER = "┌";//"\e(0\x6c\e(B";
        inline std::string BOTTOM_LEFT_CORNER = "└";//"\e(0\x6d\e(B";
        inline std::string TOP_RIGHT_CORNER = "┐";//"\e(0\x6b\e(B";
        inline std::string BOTTOM_RIGHT_CORNER = "┘";//"\e(0\x6a\e(B";
        inline std::string VERTICAL_LINE = "│";//"\e(0\x78\e(B";
        inline std::string HORIZONTAL_LINE = "─";//"\e(0\x71\e(B";
        inline std::string VERTICAL_RIGHT_CONNECTOR = "├";//"\e(0\x74\e(B";
        inline std::string VERTICAL_LEFT_CONNECTOR = "┤";//"\e(0\x75\e(B";
        inline std::string HORIZONTAL_BOTTOM_CONNECTOR = "┬";//"\e(0\x76\e(B";
        inline std::string HORIZONTAL_TOP_CONNECTOR = "┴";//"\e(0\x77\e(B";
        inline std::string CROSS_CONNECTOR = "┼";//"\e(0\x6e\e(B";

        inline std::string CENTERED_HORIZONTAL_LINE = "━";//"\e(0\x71\e(B";
        inline std::string FULL_BLOCK = "█";//"\e(0\xdb\e(B";

        inline unsigned int CONNECTS_UP = 1 << 0;
        inline unsigned int CONNECTS_DOWN = 1 << 1;
        inline unsigned int CONNECTS_LEFT = 1 << 2;
        inline unsigned int CONNECTS_RIGHT = 1 << 3;

        inline std::string RADIOBUTTON_OFF = "○";
        inline std::string RADIOBUTTON_ON = "◉";

        inline std::string EMPTY_CHECK_BOX = "☐";
        inline std::string CHECKED_CHECK_BOX = "☒";

        inline std::map<unsigned int, std::string> Border_Identifiers = {

            {CONNECTS_DOWN | CONNECTS_RIGHT, TOP_LEFT_CORNER},
            {CONNECTS_DOWN | CONNECTS_LEFT, TOP_RIGHT_CORNER},
            {CONNECTS_UP | CONNECTS_RIGHT, BOTTOM_LEFT_CORNER},
            {CONNECTS_UP | CONNECTS_LEFT, BOTTOM_RIGHT_CORNER},

            {CONNECTS_DOWN | CONNECTS_UP, VERTICAL_LINE},

            {CONNECTS_LEFT | CONNECTS_RIGHT, HORIZONTAL_LINE},

            {CONNECTS_DOWN | CONNECTS_UP | CONNECTS_RIGHT, VERTICAL_RIGHT_CONNECTOR},
            {CONNECTS_DOWN | CONNECTS_UP | CONNECTS_LEFT, VERTICAL_LEFT_CONNECTOR},

            {CONNECTS_LEFT | CONNECTS_RIGHT | CONNECTS_DOWN, HORIZONTAL_BOTTOM_CONNECTOR},
            {CONNECTS_LEFT | CONNECTS_RIGHT | CONNECTS_UP, HORIZONTAL_TOP_CONNECTOR},

        };

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

        // EASY MOUSE API
        inline unsigned long long MOUSE_LEFT_CLICKED = 1 << 12;
        inline unsigned long long MOUSE_MIDDLE_CLICKED = 1 << 13;
        inline unsigned long long MOUSE_RIGHT_CLICKED = 1 << 14;

        // NOTE: These will be spammed until it is not pressed anymore!
        inline unsigned long long MOUSE_LEFT_PRESSED = 1 << 15;
        inline unsigned long long MOUSE_MIDDLE_PRESSED = 1 << 16;
        inline unsigned long long MOUSE_RIGHT_PRESSED = 1 << 17;

        inline unsigned long long MOUSE_MIDDLE_SCROLL_UP = 1 << 18;
        inline unsigned long long MOUSE_MIDDLE_SCROLL_DOWN = 1 << 19;

        inline void Init();
    }

    namespace TIME{
        inline constexpr static  unsigned int MILLISECOND = 1; 
        inline constexpr static  unsigned int SECOND = MILLISECOND * 1000;
        inline constexpr static  unsigned int MINUTE = SECOND * 60;
        inline constexpr static  unsigned int HOUR = MINUTE * 60;
    }

    class BUTTON_STATE{
    public:
        bool State = false;
        std::chrono::system_clock::time_point Capture_Time;

        BUTTON_STATE(bool state = false){
            Capture_Time = std::chrono::system_clock::now();
            State = state;
        }
    };

    namespace BUTTON_STATES{
        inline std::string ESC = "ECS";
        inline std::string F1 = "F1";
        inline std::string F2 = "F2";
        inline std::string F3 = "F3";
        inline std::string F4 = "F4";
        inline std::string F5 = "F5";
        inline std::string F6 = "F6";
        inline std::string F7 = "F7";
        inline std::string F8 = "F8";
        inline std::string F9 = "F9";
        inline std::string F10 = "F10";
        inline std::string F11 = "F11";
        inline std::string F12 = "F12";
        inline std::string PRTSC = "PRTSC";
        inline std::string SCROLL_LOCK = "SCROLL_LOCK";
        inline std::string PAUSE = "PAUSE";
        inline std::string SECTION = "SECTION";
        inline std::string BACKSPACE = "BACKSPACE";
        inline std::string TAB = "TAB";
        inline std::string ENTER = "ENTER";
        inline std::string CAPS = "CAPS";
        inline std::string SHIFT = "SHIFT";
        inline std::string CTRL = "CTRL";
        inline std::string SUPER = "SUPER";
        inline std::string ALT = "ALT";
        inline std::string SPACE = "SPACE";
        inline std::string ALTGR = "ALTGR";
        inline std::string FN = "FN";
        inline std::string INS = "INS";
        inline std::string HOME = "HOME";
        inline std::string PAGE_UP = "PAGE_UP";
        inline std::string DELETE = "DELETE";
        inline std::string END = "END";
        inline std::string PAGE_DOWN = "PAGE_DOWN";

        inline std::string UP = "UP";
        inline std::string DOWN = "DOWN";
        inline std::string LEFT = "LEFT";
        inline std::string RIGHT = "RIGHT";

        inline std::string MOUSE_LEFT = "MOUSE_LEFT";
        inline std::string MOUSE_MIDDLE = "MOUSE_MIDDLE";
        inline std::string MOUSE_RIGHT = "MOUSE_RIGHT";
    };

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

    class RGBA : public RGB{
    private:
        float Fast_Alpha = 1;

        union{
            unsigned char Alpha = UCHAR_MAX;
            unsigned char A;
        };
    public:

        void Set_Alpha(unsigned char a){
            Alpha = a;
            Fast_Alpha = (float)Alpha / UCHAR_MAX;
        }

        void Set_Alpha(float a){
            Fast_Alpha = a;
            Alpha = (unsigned char)(a * UCHAR_MAX);
        }

        RGBA(unsigned char r, unsigned char g, unsigned char b, unsigned char a = 0){
            R = r;
            G = g;
            B = b;
            Set_Alpha(a);
        }

        RGBA(){}

        RGBA(RGB primal){
            R = primal.R;
            G = primal.G;
            B = primal.B;
        }

        float &Get_Float_Alpha(){
            return Fast_Alpha;
        }

        unsigned char &Get_Alpha(){
            return Alpha;
        }
    
        bool operator==(const RGBA& Other){
            return (Red == Other.Red) && (Green == Other.Green) && (Blue == Other.Blue) && (Alpha == Other.Alpha);
        }

        RGBA operator*(const RGBA& Other){
            return RGBA(Red * ((float)Other.Red * Other.Fast_Alpha), Green * ((float)Other.Green * Other.Fast_Alpha), Blue * ((float)Other.Blue * Other.Fast_Alpha), Fast_Alpha);
        }

        RGBA operator+(const RGBA& Other){
            return RGBA(Red + ((float)Other.Red * Other.Fast_Alpha), Green + ((float)Other.Green * Other.Fast_Alpha), Blue + ((float)Other.Blue * Other.Fast_Alpha), Fast_Alpha);
        }

        RGBA operator*=(const RGBA& Other){
            Red *= ((float)Other.Red * (float)Other.Fast_Alpha);
            Green *= ((float)Other.Green * (float)Other.Fast_Alpha);
            Blue *= ((float)Other.Blue * (float)Other.Fast_Alpha);
            return *this;
        }

        RGBA operator+=(const RGBA& Other){
            Red += ((float)Other.Red * (float)Other.Fast_Alpha);
            Green += ((float)Other.Green * (float)Other.Fast_Alpha);
            Blue += ((float)Other.Blue * (float)Other.Fast_Alpha);
            return *this;
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

    class Vector2{
    public:
        float X = 0;
        float Y = 0;

        Vector2(float x, float y){
            X = x;
            Y = y;
        }

        Vector2(){}
    };

    class Vector3 : public Vector2{
    public:
        float Z = 0;

        Vector3(float x, float y, float z){
            Z = z;
            X = x;
            Y = y;
        }

        Vector3(){}
    };

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
        bool Is_Unicode = false;

        char Ascii = ' ';
        std::string Unicode = " ";

        RGBA Foreground = {0, 0, 0}; 
        RGBA Background = {0, 0, 0};

        UTF(){}

        ~UTF(){}

        UTF(char data, std::pair<RGB, RGB> color = {{}, {}}){
            Ascii = data;
            Foreground = {color.first};
            Background = {color.second};
        }

        UTF(std::string data, std::pair<RGB, RGB> color = {{}, {}}){
            Unicode = data;
            Foreground = {color.first};
            Background = {color.second};
            Is_Unicode = true;
        }

        void Set_Foreground(RGB color){
            Foreground = {color};
        }

        void Set_Background(RGB color){
            Background = {color};
        }

        void Set_Color(std::pair<RGB, RGB> primals){
            Foreground = {primals.first};
            Background = {primals.second};
        }

        void Set_Text(std::string data){
            Unicode = data;
            Is_Unicode = true;
        }

        void Set_Text(char data){
            Ascii = data;
            Is_Unicode = false;
        }

        void Set_Text(UTF other){
            Ascii = other.Ascii;
            Unicode = other.Unicode;
            Is_Unicode = other.Is_Unicode;
        }

        std::string To_String();

        void operator=(char text){
            Set_Text(text);
        }

        void operator=(std::string text){
            Set_Text(text);
        }

        bool Has_Non_Default_Text(){
            return (Ascii != ' ') || (Unicode != " ");
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

    class Margin{
    public:
        unsigned int Top = 0;
        unsigned int Bottom = 0;
        unsigned int Left = 0;
        unsigned int Right = 0;

        Margin(unsigned int top = 0, unsigned int bottom = 0, unsigned int left = 0, unsigned int right = 0){
            Top = top;
            Bottom = bottom;
            Left = left;
            Right = right;
        }
    };

    enum class VALUE_TYPES{
        UNDEFINED,
        NUMBER,
        RGB,
        BOOL,
        COORDINATES,
        MARGIN,
    };

    class VALUE{
    public:
        VALUE_TYPES Type = VALUE_TYPES::UNDEFINED;

        VALUE(){}

        virtual VALUE* Copy() {
            return nullptr;
        };
    };

    class NUMBER_VALUE : public VALUE{
    public:
        int Value = 0;

        NUMBER_VALUE(int value){
            Value = value;
            Type = VALUE_TYPES::NUMBER;
        }

        NUMBER_VALUE(){
            Type = VALUE_TYPES::NUMBER;
        }

        VALUE* Copy() override {
            NUMBER_VALUE* copy = new NUMBER_VALUE(Value);
            return copy;
        } 
    };

    class RGB_VALUE : public VALUE{
    public:
        RGB Value = RGB();

        RGB_VALUE(RGB value){
            Value = value;
            Type = VALUE_TYPES::RGB;
        }

        RGB_VALUE(){
            Type = VALUE_TYPES::RGB;
        }

        VALUE* Copy() override {
            RGB_VALUE* copy = new RGB_VALUE(Value);
            return copy;
        } 
    };

    class BOOL_VALUE : public VALUE{
    public:
        bool Value = false;

        BOOL_VALUE(bool value){
            Value = value;
            Type = VALUE_TYPES::BOOL;
        }

        BOOL_VALUE(){
            Type = VALUE_TYPES::BOOL;
        }

        VALUE* Copy() override {
            BOOL_VALUE* copy = new BOOL_VALUE(Value);
            return copy;
        } 
    };

    class COORDINATES_VALUE : public VALUE{
    public:
        Coordinates Value = Coordinates();

        COORDINATES_VALUE(Coordinates value){
            Value = value;
            Type = VALUE_TYPES::COORDINATES;
        }

        COORDINATES_VALUE(){
            Type = VALUE_TYPES::COORDINATES;
        }

        VALUE* Copy() override {
            COORDINATES_VALUE* copy = new COORDINATES_VALUE(Value);
            return copy;
        } 
    };

    class MARGIN_VALUE : public VALUE{
    public:
        Margin Value = Margin();

        MARGIN_VALUE(){
            Type = VALUE_TYPES::MARGIN;
        }

        MARGIN_VALUE(Margin value){
            Value = value;
            Type = VALUE_TYPES::MARGIN;
        }

        VALUE* Copy() override {
            MARGIN_VALUE* copy = new MARGIN_VALUE(Value);
            return copy;
        } 
    };

    class SHADOW_VALUE : public VALUE{
    public:
        Vector3 Direction = {0, 0, 0.5};
        RGB Color = {};
        float Opacity = 1;

        SHADOW_VALUE(){}
    };

    namespace STYLES{
        inline std::string Border                          = "Border";
        inline std::string Text_Color                      = "Text_Color";
        inline std::string Background_Color               = "Background_Color";
        inline std::string Border_Colour                   = "Border_Colour";
        inline std::string Border_Background_Color        = "Border_Background_Color";

        inline std::string Hover_Border_Color               = "Hover_Border_Color";
        inline std::string Hover_Text_Color                 = "Hover_Text_Color";
        inline std::string Hover_Background_Color          = "Hover_Background_Color";
        inline std::string Hover_Border_Background_Color   = "Hover_Border_Background_Color";

        inline std::string Focus_Border_Color              = "Focus_Border_Color";
        inline std::string Focus_Text_Color                = "Focus_Text_Color";
        inline std::string Focus_Background_Color         = "Focus_Background_Color";
        inline std::string Focus_Border_Background_Color  = "Focus_Border_Background_Color";

        inline std::string Flow_Priority                   = "Flow_Priority";
        inline std::string Wrap                            = "Wrap";     
        
        inline std::string Text_Position                   = "Text_Position";
        inline std::string Allow_Input_Overflow            = "Allow_Input_Overflow";
        inline std::string Allow_Dynamic_Size              = "Allow_Dynamic_Size";     
        inline std::string Margin                          = "Margin";

        inline std::string Shadow                          = "Shadow";  // 0 - 100
        inline std::string Opacity                         = "Opacity"; // 0 - 100

        inline std::string Anchor                          = "Anchor";  // gives the line number in which the element is anchored.
    };

    enum class STAIN_TYPE{
        CLEAN = 0,        //No change
        COLOR = 1 << 0,  //BG and other color related changes
        EDGE = 1 << 1,   //title and border changes.
        DEEP = 1 << 2,   //children changes. Deep because the childs are connected via AST.
        STRECH = 1 << 3,  //width and or height changes.
        TEXT = 1 << 4,   //text changes, this is primarily for text_field
        CLASS = 1 << 5, //This is used to tell the renderer that there are still un_parsed classes.
        STATE = 1 << 6, // This is for Switches that based on their state display one symbol differently.
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
        STAIN_TYPE Type = (STAIN_TYPE)(STAIN_TYPE::COLOR | STAIN_TYPE::EDGE | STAIN_TYPE::DEEP | STAIN_TYPE::STRECH | STAIN_TYPE::CLASS);


        bool is(STAIN_TYPE f){
            if (f == STAIN_TYPE::CLEAN){
                return Type <= f;
            }
            return ((unsigned int)Type & (unsigned int)f) == (unsigned int)f;
        }

        void Clean(STAIN_TYPE f){
            Type = (STAIN_TYPE)((unsigned int)Type & ~(unsigned int)f);
        }

        void Clean(unsigned int f){
            Type = (STAIN_TYPE)((unsigned int)Type & ~f);
        }

        void Dirty(STAIN_TYPE f){
            Type = (STAIN_TYPE)((unsigned int)Type | (unsigned int)f);
        }

        void Dirty(unsigned int f){
            Type = (STAIN_TYPE)((unsigned int)Type | f);
        }

        void Stain_All(){
            Dirty(STAIN_TYPE::COLOR | STAIN_TYPE::EDGE | STAIN_TYPE::DEEP | STAIN_TYPE::STRECH | STAIN_TYPE::CLASS);
        }

    };

    enum class Flags{
        Empty = 0,
        Border = 1 << 0,
        Text_Input = 1 << 1,
        Overflow = 1 << 2,
        Dynamic = 1 << 3,
        Horizontal = 1 << 4,
        Vertical = 1 << 5,
        Align_Left = 1 << 6,
        Align_Right = 1 << 7,
        Align_Center = 1 << 8,
    };
    
    inline Flags operator|(Flags a, Flags b){
        return static_cast<Flags>(static_cast<int>(a) | static_cast<int>(b));
    }

    inline bool Is(Flags a, Flags b){
        return ((int)a & (int)b) == (int)b;
    }

    inline bool Has(Flags a, Flags b){
        return ((int)a & (int)b) != 0;
    }

    enum class State{

        RENDERED,
        HIDDEN

    };

    namespace SETTINGS{
        // How fast for a detection of hold down situation.
        inline unsigned long long Mouse_Press_Down_Cooldown = 365;

    };

    class Element{
    protected:
        Coordinates Position;

        unsigned int Width = 1;
        unsigned int Height = 1;

        unsigned int Post_Process_Width = 0;
        unsigned int Post_Process_Height = 0;

        //INTERNAL FLAGS
        class Element* Parent = nullptr;
        bool Show = true;
        
        std::vector<UTF> Render_Buffer;
        STAIN Dirty;
        
        std::vector<int> Classes;

        std::vector<Element*> Childs;

        bool Focused = false;
        bool Hovered = false;

        std::string Name = "";

        std::map<std::string, VALUE*> Style;

        std::map<State, std::function<void()>> State_Handlers;
    public:

        Element();

        Element(std::string Class, unsigned int width = 0, unsigned int height = 0, Element* parent = nullptr, Coordinates *position = nullptr);

        Element(std::map<std::string, VALUE*> css, unsigned int width = 0, unsigned int height = 0, Element* parent = nullptr, Coordinates *position = nullptr);

        Element(
            unsigned int width,
            unsigned int height,
            Coordinates position
        );

        //These next constructors are mainly for users to more easily create elements.
        Element(
            unsigned int width,
            unsigned int height
        );

        Element(
            unsigned int width,
            unsigned int height,
            RGB text_color,
            RGB background_color
        );

        Element(
            unsigned int width,
            unsigned int height,
            RGB text_color,
            RGB background_color,
            RGB border_color,
            RGB border_background_color
        );

        //End of user constructors.

        // If you want to make a representing element* that isnt the same as the Abstract one.
        // Then Remember to USE THIS!
        void Inherit_States_From(Element* abstract);

        template<typename T>
        T* At(std::string s){
            T* v = (T*)Style[s];

            if (v == nullptr){
                v = new T();

                Style[s] = v;
            }

            return v;
        }

        void Parse_Classes();

        STAIN& Get_Dirty(){
            return Dirty;
        }

        bool Is_Focused(){
            return Focused;
        }

        void Set_Focus(bool f);

        bool Is_Hovered(){
            return Hovered;
        }

        void Set_Hover_State(bool h);

        void Check(State s);

        std::map<std::string, VALUE*> Get_Style();

        void Set_Style(std::map<std::string, VALUE*> css);

        virtual Element* Handle_Or_Operator(Element* other){
            Set_Style(other->Get_Style());
        }

        void Add_Class(std::string class_name);

        RGB Get_RGB_Style(std::string style_name);

        int Get_Number_Style(std::string style_name);

        bool Get_Bool_Style(std::string style_name);

        VALUE* Get_Style(std::string style_name);

        void Set_Style(std::string style_name, VALUE* value);

        // Takes 0.0f to 1.0f
        void Set_Opacity(float Opacity);

        // RGBA - Alpha channel. 0 - 255
        void Set_Opacity(unsigned char Opacity);

        // return int as 0 - 100
        int Get_Opacity(); 

        bool Is_Transparent();

        bool Is_Anchored();

        int Get_Anchor_Location();

        void Set_Anchor_At_Current_Location();

        void Remove_Anchor();

        unsigned int Get_Processed_Width();
        unsigned int Get_Processed_Height();

        void Show_Shadow(Vector2 Direction, RGB Shadow_Color, float Opacity = 1, float Length = 0.5);

        Element* Get_Parent(){
            return Parent;
        }

        void Set_Parent(Element* parent);

        bool Has(std::string s);

        bool Has(int s){
            for (auto i : Classes){
                if (i == s)
                    return true;
            }
            return false;
        }

        //returns borders in mind.
        std::pair<unsigned int, unsigned int> Get_Fitting_Dimensions(Element* child);

        virtual void Show_Border(bool b);

        virtual void Show_Border(bool b, bool Previus_state);

        bool Has_Border();

        void Display(bool f);

        bool Is_Displayed();

        virtual void Add_Child(Element* Child);

        virtual void Set_Childs(std::vector<Element*> childs);

        bool Children_Changed();

        std::vector<Element*>& Get_Childs();

        virtual bool Remove(Element* handle);

        bool Remove(int index);

        void Set_Dimensions(int width, int height);

        int Get_Width();

        int Get_Height();

        void Set_Width(int width);

        void Set_Height(int height);

        void Set_Position(Coordinates c);
       
        void Set_Position(Coordinates* c);

        Coordinates Get_Position();

        Coordinates Get_Absolute_Position();

        void Set_Margin(Margin margin);

        Margin Get_Margin();

        void Set_Background_Color(RGB color);

        RGB Get_Background_Color();
        
        void Set_Border_Color(RGB color);
        
        RGB Get_Border_Color();

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

        void Compute_Alpha_To_Nesting(GGUI::UTF& Dest, GGUI::UTF Source);

        void Nest_Element(Element* Parent, Element* Child, std::vector<UTF>& Parent_Buffer, std::vector<UTF> Child_Buffer);

        void Post_Process_Borders(Element* A, Element* B, std::vector<UTF>& Parent_Buffer);

        std::pair<RGB, RGB>  Compose_All_Text_RGB_Values();

        RGB  Compose_Text_RGB_Values();
        RGB  Compose_Background_RGB_Values(bool Get_As_Foreground = false);

        std::pair<RGB, RGB>  Compose_All_Border_RGB_Values();

        virtual std::string Get_Name(){
            return "Element";
        }

        void Set_Name(std::string name);

        bool Has_Internal_Changes();

        virtual Element* Copy();

        //Makes suicide.
        void Remove();

        //Event handlers
        void On_Click(std::function<void(GGUI::Event* e)> action);

        //This function returns nullptr, if the element could not be found.
        Element* Get_Element(std::string name);

        //This function returns all child elements that have the same element type.
        template<typename T>
        std::vector<T*> Get_Elements(){

            //go throgh the child AST, and check if the element in question is same type as the template T.
            std::vector<T*> result;

            if (typeid(*this) == typeid(T)){
                result.push_back((T*)this);
            }

            for (auto& e : Childs){
                std::vector<T*> child_result = e->Get_Elements<T>();
                result.insert(result.end(), child_result.begin(), child_result.end());
            }

            return result;
        }

        void Re_Order_Childs();

        void Focus();

        void On_State(State s, std::function<void()> job);

        std::vector<GGUI::UTF> Process_Shadow(std::vector<GGUI::UTF> Current_Buffer);

        std::vector<GGUI::UTF> Process_Opacity(std::vector<GGUI::UTF> Current_Buffer);

        virtual std::vector<GGUI::UTF> Postprocess();

    };
}

#endif