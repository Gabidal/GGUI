#ifndef _RENDERER_H_
#define _RENDERER_H_

#undef min
#undef max

#include <iostream>
#include <functional>
#include <thread>
#include <map>
#include <atomic>

#include "Elements/Window.h"
#include "Elements/Text_Field.h"
#include "Elements/List_View.h"

namespace GGUI{
    extern std::vector<UTF> Abstract_Frame_Buffer;               //2D clean vector whitout bold nor color
    extern std::string Frame_Buffer;                                 //string with bold and color, this what gets drawn to console.
    extern std::atomic_bool Pause_Render;                     //if true, the render will not be updated, good for window creation.

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