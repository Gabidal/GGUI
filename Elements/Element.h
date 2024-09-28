#ifndef _ELEMENT_H_
#define _ELEMENT_H_

#include <string>
#include <sstream>
#include <cstring>
#include <vector>
#include <unordered_map>
#include <functional>
#include <chrono>
#include <atomic>
#include <limits>

#include <iostream>

#include "../Core/Utils/Super_String.h"
#include "../Core/Utils/Constants.h"
#include "../Core/Utils/Color.h"
#include "../Core/Utils/Units.h"
#include "../Core/Utils/Event.h"
#include "../Core/Utils/Style.h"

namespace GGUI{
    //GGUI uses the ANSI escape code
    //https://en.wikipedia.org/wiki/ANSI_escape_code

    // Inits with 'NOW()' when created
    class BUTTON_STATE{
    public:
        bool State = false;
        std::chrono::high_resolution_clock::time_point Capture_Time;

        BUTTON_STATE(bool state = false){
            Capture_Time = std::chrono::high_resolution_clock::now();
            State = state;
        }
    };

    enum class STAIN_TYPE{
        CLEAN = 0,              // No change
        COLOR = 1 << 0,         // BG and other color related changes
        EDGE = 1 << 1,          // Title and border changes.
        DEEP = 1 << 2,          // Children changes. Deep because the childs are connected via AST.
        STRETCH = 1 << 3,       // Width and or height changes.
        CLASS = 1 << 5,         // This is used to tell the renderer that there are still un_parsed classes.
        STATE = 1 << 6,         // This is for Switches that based on their state display one symbol differently. And also for state handlers.
        MOVE = 1 << 7,          // Enabled, to signal absolute position caching.
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
        STAIN_TYPE Type = STAIN_TYPE::CLEAN; //(STAIN_TYPE)(STAIN_TYPE::COLOR | STAIN_TYPE::EDGE | STAIN_TYPE::DEEP | STAIN_TYPE::STRETCH | STAIN_TYPE::CLASS | STAIN_TYPE::MOVE);


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

        // void Stain_All(){
        //     Dirty(STAIN_TYPE::COLOR | STAIN_TYPE::EDGE | STAIN_TYPE::DEEP | STAIN_TYPE::STRETCH | STAIN_TYPE::CLASS | STAIN_TYPE::MOVE);
        // }

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
        UNKNOWN,

        RENDERED,
        HIDDEN

    };

    // For templates.
    extern std::vector<Action*> Event_Handlers;

    class Element{
    protected:
        unsigned int Post_Process_Width = 0;
        unsigned int Post_Process_Height = 0;

        //INTERNAL FLAGS
        class Element* Parent = nullptr;
        bool Show = true;
        
        std::vector<UTF> Render_Buffer;
        std::vector<UTF> Post_Process_Buffer;
        STAIN Dirty;
        
        std::vector<int> Classes;

        std::vector<Element*> Childs;

        bool Focused = false;
        bool Hovered = false;

        std::string Name = "";

        // NOTE: do NOT set the .VALUEs manually set each member straight with the operator= overload.
        Styling* Style = nullptr;

        std::unordered_map<State, std::function<void()>> State_Handlers;
    public:

        Element();

        Element(std::string Class, unsigned int width = 0, unsigned int height = 0, Element* parent = nullptr, IVector2 *position = nullptr);

        Element(Styling css, unsigned int width = 0, unsigned int height = 0, Element* parent = nullptr, IVector2 *position = nullptr);

        Element(
            unsigned int width,
            unsigned int height,
            IVector2 position
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

        // Disable Copy constructor
        Element(const Element&);

        Element& operator=(const GGUI::Element&) = default;

        //Start of destructors.
        //-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_

        virtual ~Element();

        //
        //-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_

        virtual Element* Safe_Move(){
            Element* new_element = new Element();
            *new_element = *(Element*)this;

            return new_element;
        }

        // Use this when you want to duplicate the same element with its properties safely.
        Element* Copy();

        virtual void Fully_Stain();

        // If you want to make a representing element* that isn't the same as the Abstract one.
        // Then Remember to USE THIS!
        void Inherit_States_From(Element* abstract);

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

        Styling Get_Style();

        void Set_Style(Styling css);

        virtual void Calculate_Childs_Hitboxes([[maybe_unused]] unsigned int Starting_Offset = 0){}

        virtual Element* Handle_Or_Operator(Element* other){
            Set_Style(other->Get_Style());

            return this;
        }

        void Add_Class(std::string class_name);

        // Takes 0.0f to 1.0f
        void Set_Opacity(float Opacity);

        // Alpha channel. 0 to 100
        void Set_Opacity(int Opacity);

        styled_border Get_Border_Style(){
            return Style->Border_Style;
        }

        // return int as 0 - 100
        int Get_Opacity(); 

        bool Is_Transparent();

        unsigned int Get_Processed_Width();
        unsigned int Get_Processed_Height();

        void Show_Shadow(FVector2 Direction, RGB Shadow_Color, float Opacity = 1, float Length = 0.5);

        void Show_Shadow(RGB Shadow_Color, float Opacity = 1, float Length = 0.5);

        void Set_Shadow(shadow s);

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

        //returns the area which a new element could be fitted in.
        std::pair<unsigned int, unsigned int> Get_Fitting_Dimensions(Element* child);

        // returns the maximum area of width and height which an element could be fit in.
        // basically same as the Get_Fitting_Dimensions(), but with some extra safe checks, so use this.
        std::pair<unsigned int, unsigned int> Get_Limit_Dimensions();

        virtual void Show_Border(bool b);

        virtual void Show_Border(bool b, bool Previous_state);

        bool Has_Border();

        // NOTE: This will also HIDE ALL children in the AST beneath this element!!!
        void Display(bool f);

        bool Is_Displayed();

        virtual void Add_Child(Element* Child);

        virtual void Set_Childs(std::vector<Element*> childs);

        bool Children_Changed();
        
        bool Has_Transparent_Children();    

        virtual std::vector<Element*>& Get_Childs();

        virtual bool Remove(Element* handle);

        virtual bool Remove(unsigned int index);

        void Set_Dimensions(unsigned int width, unsigned int height);

        unsigned int Get_Width();

        unsigned int Get_Height();

        void Set_Width(unsigned int width);

        void Set_Height(unsigned int height);

        void Set_Position(IVector2 c);
       
        void Set_Position(IVector2* c);

        IVector2 Get_Position();

        IVector2 Get_Absolute_Position();

        void Update_Absolute_Position_Cache();

        void Set_Margin(margin margin);

        margin Get_Margin();

        virtual void Set_Background_Color(RGB color);

        RGB Get_Background_Color();
        
        virtual void Set_Border_Color(RGB color);
        
        RGB Get_Border_Color();

        virtual void Set_Border_Background_Color(RGB color);
        
        RGB Get_Border_Background_Color();
        
        virtual void Set_Text_Color(RGB color);

        void Allow_Dynamic_Size(bool True);

        bool Is_Dynamic_Size_Allowed();

        // Allows by default hidden overflow, so that child elements can exceed the parent element dimension limits, whiteout resizing parent.  
        void Allow_Overflow(bool True);

        bool Is_Overflow_Allowed();
        
        RGB Get_Text_Color();

        static std::pair<std::pair<unsigned int, unsigned int> ,std::pair<std::pair<unsigned int, unsigned int>, std::pair<unsigned int, unsigned int>>> Get_Fitting_Area(GGUI::Element* Parent, GGUI::Element* Child);

        void Compute_Dynamic_Size();

        virtual std::vector<GGUI::UTF>& Render();

        // Used to update the parent when the child cannot update on itself, for an example on removal of an element.
        virtual void Update_Parent(Element* New_Element);

        virtual void Add_Overhead(Element* w, std::vector<UTF>& Result);

        virtual void Apply_Colors(Element* w, std::vector<UTF>& Result);

        virtual bool Resize_To([[maybe_unused]] Element* parent){
            return false;
        }

        void Compute_Alpha_To_Nesting(GGUI::UTF& Dest, GGUI::UTF Source);

        void Nest_Element(Element* Parent, Element* Child, std::vector<UTF>& Parent_Buffer, std::vector<UTF>& Child_Buffer);

        std::unordered_map<unsigned int, const char*> Get_Custom_Border_Map(Element* e);

        std::unordered_map<unsigned int, const char*> Get_Custom_Border_Map(GGUI::styled_border custom_border_style);

        void Set_Custom_Border_Style(GGUI::styled_border style);

        GGUI::styled_border Get_Custom_Border_Style();

        void Post_Process_Borders(Element* A, Element* B, std::vector<UTF>& Parent_Buffer);

        std::pair<RGB, RGB>  Compose_All_Text_RGB_Values();

        RGB  Compose_Text_RGB_Values();
        RGB  Compose_Background_RGB_Values();

        std::pair<RGB, RGB>  Compose_All_Border_RGB_Values();

        virtual std::string Get_Name() const {
            return "Element<" + Name + ">";
        }

        void Set_Name(std::string name);

        bool Has_Internal_Changes();

        //Makes suicide.
        void Remove();

        //Event handlers
        void On_Click(std::function<bool(GGUI::Event*)> action);

        void On(unsigned long long criteria, std::function<bool(GGUI::Event*)> action, bool GLOBAL = false);

        //This function returns nullptr, if the element could not be found.
        Element* Get_Element(std::string name);

        // TEMPLATES
        //-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_

        //This function returns all child elements that have the same element type.
        template<typename T>
        std::vector<T*> Get_Elements(){

            //go through the child AST, and check if the element in question is same type as the template T.
            std::vector<T*> result;

            if (typeid(*this) == typeid(T)){
                result.push_back((T*)this);
            }

            for (auto e : Childs){
                std::vector<T*> child_result = e->Get_Elements<T>();
                result.insert(result.end(), child_result.begin(), child_result.end());
            }

            return result;
        }

        std::vector<Element*> Get_All_Nested_Elements(bool Show_Hidden = false){
            std::vector<Element*> result;

            if (!Show && !Show_Hidden)
                return {};
            
            result.push_back(this);

            for (auto e : Childs){
                std::vector<Element*> child_result = e->Get_All_Nested_Elements(Show_Hidden);
                result.insert(result.end(), child_result.begin(), child_result.end());
            }

            return result;
        }

        //-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_

        // By default elements do not have inherent scrolling abilities.
        virtual void Scroll_Up() {}
        virtual void Scroll_Down() {}

        void Re_Order_Childs();

        void Focus();

        void On_State(State s, std::function<void()> job);

        bool Has_Postprocessing_To_Do();

        void Process_Shadow(std::vector<GGUI::UTF>& Current_Buffer);

        void Process_Opacity(std::vector<GGUI::UTF>& Current_Buffer);

        virtual std::vector<GGUI::UTF>& Postprocess();

        // Uses the post_processed widths and height values
        bool Child_Is_Shown(Element* other);
    };
}

#endif
