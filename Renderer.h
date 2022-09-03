#ifndef _RENDERER_H_
#define _RENDERER_H_

#include <iostream>
#include <functional>
#include <thread>
#include <map>

#include "Elements/Window.h"
#include "Elements/Text_Field.h"
#include "Elements/List_View.h"

namespace GGUI{
    inline std::vector<GGUI::UTF> Abstract_Frame_Buffer;               //2D clean vector whitout bold nor color
    inline std::string Frame_Buffer;                                 //string with bold and color, this what gets drawn to console.
    inline bool Pause_Render = false;                              //if true, the render will not be updated, good for window creation.

    inline GGUI::Window Main;                                   //Main window

    inline int Max_Width = 0;
    inline int Max_Height = 0;

    inline std::vector<GGUI::Memory> Remember;

    inline std::vector<GGUI::Action*> Event_Handlers;
    inline std::vector<GGUI::Input*> Inputs;

    inline GGUI::Element* Focused_On = nullptr;

    inline Coordinates Mouse;

    inline const time_t UPDATE_SPEED_MIILISECONDS = TIME::MILLISECOND * 100;

    inline void Set_Cursor_At(GGUI::Coordinates C){
        std::cout << GGUI::Constants::ESC + std::to_string(C.X) + GGUI::Constants::SEPERATE + std::to_string(C.Y) + "H" << std::endl;
    }

#if _WIN32
    #include <windows.h>

    inline void ClearScreen()
    {
        HANDLE                     hStdOut;
        CONSOLE_SCREEN_BUFFER_INFO csbi;
        DWORD                      count;
        DWORD                      cellCount;
        COORD                      homeCoords = { 0, 0 };

        hStdOut = GetStdHandle( STD_OUTPUT_HANDLE );
        if (hStdOut == INVALID_HANDLE_VALUE) return;

        /* Get the number of cells in the current buffer */
        if (!GetConsoleScreenBufferInfo( hStdOut, &csbi )) return;
        cellCount = csbi.dwSize.X *csbi.dwSize.Y;

        /* Fill the entire buffer with spaces */
        if (!FillConsoleOutputCharacter(
            hStdOut,
            (TCHAR) ' ',
            cellCount,
            homeCoords,
            &count
            )) return;

        /* Fill the entire buffer with the current colors and attributes */
        if (!FillConsoleOutputAttribute(
            hStdOut,
            csbi.wAttributes,
            cellCount,
            homeCoords,
            &count
            )) return;

        /* Move the cursor home */
        SetConsoleCursorPosition( hStdOut, homeCoords );
    }

    inline void Render_Frame(){
        ClearScreen();

        unsigned long long tmp = 0;
        WriteFile(GetStdHandle(STD_OUTPUT_HANDLE), Frame_Buffer.data(), Frame_Buffer.size(), (LPDWORD)&tmp, NULL);
    }

    inline void Update_Max_Width_And_Height(){
        CONSOLE_SCREEN_BUFFER_INFO info;

        GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &info);

        Max_Width = info.srWindow.Right - info.srWindow.Left;
        Max_Height = info.srWindow.Bottom - info.srWindow.Top;
    }

    //Is called on every cycle.
    inline void Query_Inputs(){
        const int Inputs_Per_Second = 20;
        const int Inputs_Per_Query = Inputs_Per_Second / (TIME::SECOND / UPDATE_SPEED_MIILISECONDS);

        INPUT_RECORD Input[Inputs_Per_Query];

        int Buffer_Size = 0;

        ReadConsoleInput(GetStdHandle(STD_INPUT_HANDLE), Input, Inputs_Per_Query, (LPDWORD)&Buffer_Size);

        for (int i = 0; i < Buffer_Size; i++){
            if (Input[i].EventType == KEY_EVENT){
                if (Input[i].Event.KeyEvent.bKeyDown){
                    if (Input[i].Event.KeyEvent.wVirtualKeyCode == VK_UP){
                        GGUI::Mouse.Y--;
                    }
                    else if (Input[i].Event.KeyEvent.wVirtualKeyCode == VK_DOWN){
                        GGUI::Mouse.Y++;
                    }
                    else if (Input[i].Event.KeyEvent.wVirtualKeyCode == VK_LEFT){
                        GGUI::Mouse.X--;
                    }
                    else if (Input[i].Event.KeyEvent.wVirtualKeyCode == VK_RIGHT){
                        GGUI::Mouse.X++;
                    }
                    else if (Input[i].Event.KeyEvent.wVirtualKeyCode == VK_RETURN){
                        Inputs.push_back(new GGUI::Input(' ', Constants::ENTER));
                    }
                }
            }
        }
    }
#else
    inline void ClearScreen()
    {
        std::cout << GGUI::Constants::CLEAR_SCREEN;
    }

    inline void Render_Frame(){
        ClearScreen();

        unsigned long long tmp = 0;
        WriteFile(GetStdHandle(STD_OUTPUT_HANDLE), Frame_Buffer.data(), Frame_Buffer.size(), (LPDWORD)&tmp, NULL);
    }

    inline void Update_Max_Width_And_Height(){
        CONSOLE_SCREEN_BUFFER_INFO info;

        GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &info);

        Max_Width = info.srWindow.Right - info.srWindow.Left;
        Max_Height = info.srWindow.Bottom - info.srWindow.Top;
    }

    //Is called on every cycle.
    inline void Query_Inputs(){
        const int Inputs_Per_Second = 20;
        const int Inputs_Per_Query = Inputs_Per_Second / (TIME::SECOND / UPDATE_SPEED_MIILISECONDS);

        INPUT_RECORD Input[Inputs_Per_Query];

        int Buffer_Size = 0;

        ReadConsoleInput(GetStdHandle(STD_INPUT_HANDLE), Input, Inputs_Per_Query, (LPDWORD)&Buffer_Size);

        for (int i = 0; i < Buffer_Size; i++){
            if (Input[i].EventType == KEY_EVENT){
                if (Input[i].Event.KeyEvent.bKeyDown){
                    if (Input[i].Event.KeyEvent.wVirtualKeyCode == VK_UP){
                        GGUI::Mouse.Y--;
                    }
                    else if (Input[i].Event.KeyEvent.wVirtualKeyCode == VK_DOWN){
                        GGUI::Mouse.Y++;
                    }
                    else if (Input[i].Event.KeyEvent.wVirtualKeyCode == VK_LEFT){
                        GGUI::Mouse.X--;
                    }
                    else if (Input[i].Event.KeyEvent.wVirtualKeyCode == VK_RIGHT){
                        GGUI::Mouse.X++;
                    }
                    else if (Input[i].Event.KeyEvent.wVirtualKeyCode == VK_RETURN){
                        Inputs.push_back(new GGUI::Input(' ', Constants::ENTER));
                    }
                }
            }
        }
    }

#endif

    inline bool Collides(GGUI::Element* a, GGUI::Element* b){
        if (a == b)
            return false;
            
        int A_X = a->Position.X;
        int A_Y = a->Position.Y;

        int B_X = b->Position.X;
        int B_Y = b->Position.Y;
        return (A_X < B_X + b->Width && A_X + a->Width > B_X && A_Y < B_Y + b->Height && A_Y + a->Height > B_Y);
    }

    inline bool Collides(GGUI::Element* a, GGUI::Coordinates b){
        int A_X = a->Position.X;
        int A_Y = a->Position.Y;

        int B_X = b.X;
        int B_Y = b.Y;
        return (A_X < B_X + 1 && A_X + a->Width > B_X && A_Y < B_Y + 1 && A_Y + a->Height > B_Y);
    }

    inline int Get_Max_Width(){
        if (Max_Width == 0 && Max_Height == 0){
            Update_Max_Width_And_Height();
        }
        
        return Max_Width;
    }

    inline int Get_Max_Height(){
        if (Max_Width == 0 && Max_Height == 0){
            Update_Max_Width_And_Height();
        }

        return Max_Height;
    }

    //Returns a char if given ASCII, or a short if given UNICODE
    inline GGUI::UTF* Get(GGUI::Coordinates Abselute_Position){
        if (Abselute_Position.X >= Get_Max_Width() || 
            Abselute_Position.Y >= Get_Max_Height() ||
            Abselute_Position.X < 0 || 
            Abselute_Position.Y < 0)
        {
            return nullptr; //Later on make a 
        }
        else{
            return &Abstract_Frame_Buffer[Abselute_Position.Y * Get_Max_Width() + Abselute_Position.X];
        }
    }

    inline std::string Liquify_UTF_Text(std::vector<GGUI::UTF> Text, int Width, int Height){
        std::string Result = "";

        for (int y = 0; y < Height; y++){
            for (int x = 0; x < Width; x++){
                Result += Text[y * Width + x].To_String();
            }

            Result += "\n";
        }

        return Result;
    }

    inline void Update_Frame(){
        if (Pause_Render)
            return;

        Abstract_Frame_Buffer = Main.Render();

        Frame_Buffer = Liquify_UTF_Text(Abstract_Frame_Buffer, Main.Width, Main.Height);
    }
    
    inline void Pause_Renderer(){
        Pause_Render = true;
    }

    inline void Resume_Renderer(){
        Pause_Render = false;

        Update_Frame();
    }

    inline void Recall_Memories(){
        size_t Current_Time = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();

        for (int i = 0; i < Remember.size(); i++){
            //first calculate the time difference between the start if the task and the end task
            size_t Time_Difference = Current_Time - Remember[i].Start_Time;

            //if the time difference is greater than the time limit, then delete the memory
            if (Time_Difference > Remember[i].End_Time){
                Pause_Renderer();
                Remember[i].Job((Event*)&Remember[i]);

                Remember.erase(Remember.begin() + i);

                i--;
                
                Resume_Renderer();
            }
        }
    }

    inline bool Is(unsigned long long f, unsigned long long Flag){
        return (f & Flag) == Flag;
    }

    inline void Update_Focused_Element(GGUI::Element* new_candidate){
        if (Focused_On == new_candidate)
            return;

        //put the previus focused candidate into not-focus
        if (Focused_On)
            Focused_On->Focused = false;

        //switch the candidate
        Focused_On = new_candidate;
        
        //set the new candidate to focused.
        Focused_On->Focused = true;
        Focused_On->Dirty = true;

        Update_Frame();
    }

    inline void Un_Focus_Element(){
        Focused_On->Focused = false;
        Focused_On->Dirty = true;

        Focused_On = nullptr;

        Update_Frame();
    }

    inline void Event_Handler(){
        if (Focused_On && !Collides(Focused_On, GGUI::Mouse)){
            Un_Focus_Element();
        }

        Query_Inputs();
        for (auto& e : Event_Handlers){
            //update the focused
            if (Collides(e->Host, GGUI::Mouse)){
                Update_Focused_Element(e->Host);
            }
            
            for (int i = 0; i < Inputs.size(); i++){
                if (Is(e->Criteria, Inputs[i]->Criteria)){
                    e->Job(Inputs[i]);
                }
            }
        }
        Inputs.clear();
    }

    //Inits GGUI and returns the main window.
    inline GGUI::Window* Init_Renderer(){
        Update_Max_Width_And_Height();
        GGUI::Constants::Init();

        //now we need to allocate the buffer string by the width and height of the terminal
        Abstract_Frame_Buffer.resize(Max_Height * Max_Width);

        Main.Width = Get_Max_Width();
        Main.Height = Get_Max_Height();

        Abstract_Frame_Buffer = Main.Render();

        Frame_Buffer = Liquify_UTF_Text(Abstract_Frame_Buffer, Main.Width, Main.Height);

        std::thread Renderer([&](){
            while (true){
                if (Pause_Render)
                    continue;
                Render_Frame();
                
                std::this_thread::sleep_for(std::chrono::milliseconds(UPDATE_SPEED_MIILISECONDS)); 
            }
        });

        std::thread Job_Scheduler([&](){
            while (true){
                Recall_Memories();

                std::this_thread::sleep_for(std::chrono::milliseconds(UPDATE_SPEED_MIILISECONDS)); 
            }
        });
        
        std::thread Event_Scheduler([&](){
            while (true){
                Event_Handler();
                std::this_thread::sleep_for(std::chrono::milliseconds(UPDATE_SPEED_MIILISECONDS)); 
            }
        });
        

        Renderer.detach();

        Job_Scheduler.detach();

        Event_Scheduler.detach();

        return &Main;
    }

    inline void Report(std::string Problem){
        Pause_Renderer();

        GGUI::Text_Field* txt = new GGUI::Text_Field(Problem);

        bool Has_Border = true;

        int w = txt->Width + Has_Border * 2;
        int h = txt->Height + Has_Border * 2;

        int W_Center = (Max_Width - w) / 2;
        int H_Center = (Max_Height - h) / 2;

        GGUI::Window* tmp = new GGUI::Window("ERROR!", {
            {W_Center, H_Center, INT32_MAX}, w, h, Has_Border, 
            GGUI::COLOR::BLACK, 
            GGUI::COLOR::RED, 

            GGUI::COLOR::BLACK, 
            GGUI::COLOR::RED, 
        });

        tmp->Add_Child(txt);

        Main.Add_Child(tmp);

        Remember.push_back(Memory(
            TIME::SECOND * 10,
            [=](GGUI::Event* e){
                GGUI::Window* Right_tmp = nullptr;

                int i = 0;
                for (auto c : Main.Get_Childs()){
                    if (c == tmp){
                        Right_tmp = (GGUI::Window*)c;
                        break;
                    }
                    i++;
                }

                Main.Get_Childs().erase(Main.Get_Childs().begin() + i);

                delete Right_tmp;
            }
        ));

        Resume_Renderer();
    }

    inline void Nest_UTF_Text(GGUI::Element* Parent, GGUI::Element* child, std::vector<GGUI::UTF> Text, std::vector<GGUI::UTF>& Parent_Buffer){
        if (Parent == child){
            std::string R = 
                std::string("Cannot nest element to it self\n") +
                std::string("Element name: ") + Parent->Get_Name();

            if (Parent->Parent){
                R += std::string("\n") + 
                std::string("Inside of: ") + Parent->Parent->Get_Name();
            }

            Report(
                R
            );
        }

        GGUI::Coordinates C = child->Get_Position();

        int i = 0;
        for (int Parent_Y = 0; Parent_Y < Parent->Height; Parent_Y++){
            for (int Parent_X = 0; Parent_X < Parent->Width; Parent_X++){
                if (Parent_Y >= C.Y && Parent_X >= C.X &&
                    Parent_Y <= C.Y + child->Height && Parent_X <= C.X + child->Width)
                {
                    Parent_Buffer[Parent_Y * Parent->Width + Parent_X] = Text[i++];
                }
            }
        }
    }

}

#endif