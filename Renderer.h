#ifndef _RENDERER_H_
#define _RENDERER_H_

#undef min
#undef max

#include <functional>
#include <thread>
#include <atomic>

#include "Elements/Window.h"
#include "Elements/Text_Field.h"
#include "Elements/List_View.h"
#include "Elements/Canvas.h"

namespace GGUI{
    
    namespace INTERNAL{
        class BUFFER_CAPTURE;
    }

    extern std::vector<UTF> Abstract_Frame_Buffer;               //2D clean vector whitout bold nor color
    extern std::string Frame_Buffer;                                 //string with bold and color, this what gets drawn to console.
    extern std::atomic_bool Pause_Render;                     //if true, the render will not be updated, good for window creation.
    extern std::atomic_bool Pause_Event_Thread;                                 //Main window
    
    extern std::vector<INTERNAL::BUFFER_CAPTURE*> Global_Buffer_Captures;

    extern int Max_Width;
    extern int Max_Height;

    extern std::vector<Memory> Remember;

    extern std::vector<Action*> Event_Handlers;
    extern std::vector<Input*> Inputs;
    
    extern std::map<std::string, Element*> Element_Names;

    extern Element* Focused_On;
    extern Element* Hovered_On;

    extern Coordinates Mouse;    
    extern bool Mouse_Movement_Enabled;

    extern std::map<std::string, BUTTON_STATE> KEYBOARD_STATES;

    extern time_t UPDATE_SPEED_MIILISECONDS;
    extern int Inputs_Per_Second;
    extern int Inputs_Per_Query;

    extern std::map<int, std::map<std::string, VALUE*>> Classes;
    extern std::map<std::string, int> Class_Names;

    extern Window* Main;  

    extern std::unordered_map<GGUI::Terminal_Canvas*, bool> Multi_Frame_Canvas;

    void SLEEP(unsigned int milliseconds);

    extern bool Collides(GGUI::Element* a, GGUI::Element* b);

    extern bool Collides(GGUI::Element* a, GGUI::Coordinates b);

    extern bool Collides(GGUI::Element* a, GGUI::Coordinates c, unsigned int Width, unsigned int Height);

    extern Element* Get_Accurate_Element_From(Coordinates c, Element* Parent);

    extern Coordinates Find_Upper_Element();

    extern Coordinates Find_Lower_Element();

    extern Coordinates Find_Left_Element();

    extern Coordinates Find_Right_Element();

    extern signed long long Min(signed long long a, signed long long b);

    extern signed long long Max(signed long long a, signed long long b);

    extern void ClearScreen();

    extern void Render_Frame();

    extern void Update_Max_Width_And_Height();

    void Update_Frame();
    //Is called on every cycle.
    extern void Query_Inputs();

    extern void MOUSE_API();

    // Handles also UP and DOWN buttons
    extern void SCROLL_API();

    extern void Init_Platform_Stuff();

    extern int Get_Unicode_Length(char first_char);

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

    extern void Un_Hover_Element();

    extern void Update_Focused_Element(GGUI::Element* new_candidate);

    extern void Update_Hovered_Element(GGUI::Element* new_candidate);

    extern void Event_Handler();

    extern int Get_Free_Class_ID(std::string n);

    extern void Add_Class(std::string name, std::map<std::string, VALUE*> Styling);

    extern void Init_Classes();

    //Inits GGUI and returns the main window.
    extern GGUI::Window* Init_Renderer();

    extern void Report(std::string Problem);

    extern void Report_Stack(std::string Problem);

    extern void Nest_UTF_Text(GGUI::Element* Parent, GGUI::Element* child, std::vector<GGUI::UTF> Text, std::vector<GGUI::UTF>& Parent_Buffer);

    extern void Pause_Renderer(std::function<void()> f);

    // Use this to access GGUI.
    extern void GGUI(std::function<void()> DOM, unsigned long long Sleep_For = 0);

    extern void Exit();

    // Also handles shift tabs!
    extern void Handle_Tabulator();

    extern void Handle_Escape();

    extern void Encode_Buffer(std::vector<GGUI::UTF>& Buffer);

    extern void Init_Inspect_Tool();

    extern void Inform_All_Global_BUFFER_CAPTURES(INTERNAL::BUFFER_CAPTURE* informer);
}

#endif