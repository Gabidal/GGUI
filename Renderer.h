#ifndef _RENDERER_H_
#define _RENDERER_H_

#include <iostream>

#include "Window.h"
#include "Constants.h"

#include <thread>

namespace Utils{
    inline std::string (*Bold)(std::string text);
    inline std::string (*Color)(std::string text, std::string color);

        #if _WIN32
            #include <windows.h>

            inline void Init(){
                Bold = [](std::string Text){
                    return std::string("\033[1m") + Text;
                };

                Color = [](std::string Text, std::string Color){
                    return Color + Text + COLOR::RESET;
                };
            }
        #else            
            inline void Init();
        #endif
}

namespace RENDERER{
    inline std::vector<GGUI::UTF> Abstract_Frame_Buffer;                   //2D clean vector whitout bold nor color
    inline std::string Frame_Buffer;                                       //string with bold and color, this what gets drawn to console.
    inline bool Pause_Render = false;                              //if true, the render will not be updated, good for window creation.

    inline GGUI::Window Main;                                     //Main window

    inline int Max_Width = 0;
    inline int Max_Height = 0;

    #if _WIN32
        inline void ClearScreen()
        {
            using namespace Utils;

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

        inline void Set_Cursor_At(GGUI::Coordinates C){
            using namespace Utils;

            COORD c;
            c.X = C.X;
            c.Y = C.Y;

            SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), c);
        }

        inline void Render_Frame(){
            using namespace Utils;

            std::this_thread::sleep_for(std::chrono::milliseconds(100)); 
            ClearScreen();

            unsigned long long tmp = 0;
            WriteFile(GetStdHandle(STD_OUTPUT_HANDLE), Frame_Buffer.data(), Frame_Buffer.size(), (LPDWORD)&tmp, NULL);
        }
    
        inline void Update_Max_Width_And_Height(){
            using namespace Utils;

            CONSOLE_SCREEN_BUFFER_INFO info;

            GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &info);

            Max_Width = info.srWindow.Right - info.srWindow.Left;
            Max_Height = info.srWindow.Bottom - info.srWindow.Top;
        }
    
    #else
        void ClearScreen();

        void Render_Frame(string& Frame);
    
        void Set_Cursor_At(Coordinates C);

        void Update_Max_Width_And_Height();
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

    inline void Pause_Renderer(){
        Pause_Render = true;
    }

    inline void Resume_Renderer(){
        Pause_Render = false;
    }

    inline void Update_Frame(){
        if (Pause_Render)
            return;

        Abstract_Frame_Buffer = Main.Render();

        Frame_Buffer = Liquify_UTF_Text(Abstract_Frame_Buffer, Main.Width, Main.Height);
    }

    //Inits GGUI and returns the main window.
    inline GGUI::Window* Init_Renderer(){
        Update_Max_Width_And_Height();
        Constants::Init();
        Utils::Init();

        //now we need to allocate the buffer string by the width and height of the terminal
        Abstract_Frame_Buffer.resize(Max_Height * Max_Width);

        Main.Width = Get_Max_Width();
        Main.Height = Get_Max_Height();

        Abstract_Frame_Buffer = Main.Render();

        Frame_Buffer = Liquify_UTF_Text(Abstract_Frame_Buffer, Main.Width, Main.Height);

        std::thread Renderer([](){
            while (true){
                if (Pause_Render)
                    continue;
                Render_Frame();
            }
        });

        Renderer.detach();

        return &Main;
    }

    inline void Report(std::string Problem){
        //calculate the most longest row of text and set it as the width
        int longest_row = 0;
        int Current_Row = 0;

        int longest_Height = 0;
        for (auto& c : Problem){
            if (c == '\n'){
                if (Current_Row > longest_row)
                    longest_row = Current_Row;
                
                Current_Row = 0;
                longest_Height++;
            }
            else{
                Current_Row++;
            }
        }

        Pause_Renderer();
        //Main = GGUI::Window("ERROR!", {0, 0, INT32_MAX}, Problem, false, longest_row, longest_Height, COLOR::RED, COLOR::WHITE, COLOR::YELLOW);
        Resume_Renderer();

        Update_Frame();

        //sleep
        std::this_thread::sleep_for(std::chrono::milliseconds(5000));
        //std::exit(1);
    }


}

#endif