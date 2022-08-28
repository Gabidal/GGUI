#ifndef _RENDERER_H_
#define _RENDERER_H_

#include <iostream>
#include <functional>
#include <thread>

#include "Elements/Window.h"
#include "Elements/Text_Field.h"



namespace GGUI{
    inline std::vector<GGUI::UTF> Abstract_Frame_Buffer;               //2D clean vector whitout bold nor color
    inline std::string Frame_Buffer;                                 //string with bold and color, this what gets drawn to console.
    inline bool Pause_Render = false;                              //if true, the render will not be updated, good for window creation.

    inline GGUI::Window Main;                                   //Main window

    inline int Max_Width = 0;
    inline int Max_Height = 0;

    inline std::vector<GGUI::Memory> Remember;

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
#else
    inline void Render_Frame(){}
    inline void Update_Max_Width_And_Height(){}
    inline void ClearScreen()
    {
        std::cout << GGUI::Constants::CLEAR_SCREEN;
    }
#endif

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

    inline void Nest_UTF_Text(GGUI::Window* Parent, std::vector<GGUI::UTF>& Text, std::vector<GGUI::UTF>& Parent_Buffer){
        for (int Parent_Y = 0; Parent_Y < Parent->Height; Parent_Y++){

            if (Parent_Y + Parent->Has_Border() >= Parent->Height - Parent->Has_Border()){
                return;
            }

            for (int Parent_X = 0; Parent_X < Parent->Width; Parent_X++){
                if (Parent_X + Parent->Has_Border() >= Parent->Width - Parent->Has_Border()){
                    break; //last row cannot have text when border occupies it.
                }
                if (Parent_Y * Parent->Width + Parent_X < Text.size()){
                    Parent_Buffer[(Parent_Y + Parent->Has_Border()) * Parent->Width + (Parent_X + Parent->Has_Border())] = Text[Parent_Y * Parent->Width + Parent_X];
                }
            }
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
                Remember[i].Job();

                Remember.erase(Remember.begin() + i);

                i--;
                
                Resume_Renderer();
            }
        }
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
        

        Renderer.detach();

        Job_Scheduler.detach();

        return &Main;
    }

    inline void Report(std::string Problem){
        Pause_Renderer();

        std::pair<int, int> size = GGUI::Text_Field::Get_Text_Dimensions(Problem);

        GGUI::Window* tmp = new GGUI::Window("ERROR!", {
            {0, 0, INT32_MAX}, size.first, size.second, true, 
            GGUI::COLOR::BLACK, 
            GGUI::COLOR::RED, 

            GGUI::COLOR::BLACK, 
            GGUI::COLOR::RED, 
        });

        Main.Add_Child(tmp);

        Remember.push_back(Memory(
            TIME::SECOND * 5,
            [=](){
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

}

#endif