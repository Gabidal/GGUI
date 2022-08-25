#ifndef _RENDERER_H_
#define _RENDERER_H_

#include "Window.h"
#include "Constants.h"

namespace Utils{
    string (*Bold)(string text);
    string (*Color)(string text, string color);

        #if _WIN32
            #include <windows.h>

            void Init(){
                Bold = [](string Text){
                    return string("\033[1m") + Text;
                };

                Color = [](string Text, string Color){
                    return Color + Text + COLOR::RESET;
                };
            }
        #else            
            void Init();
        #endif
}

namespace RENDERER{
    vector<Text> Frame_Buffer;                   //2D clean vector whitout bold nor color

    int Max_Width = 0;
    int Max_Height = 0;

    #if _WIN32
        void ClearScreen()
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

        void Render_Frame(string& Frame){
            ClearScreen();
            printf(Frame.data());
        }
    
        void Set_Cursor_At(Coordinates C){
            using namespace Utils;

            COORD c;
            c.X = C.X;
            c.Y = C.Y;

            SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), c);
        }

        void Update_Max_Width_And_Height(){
            using namespace Utils;

            CONSOLE_SCREEN_BUFFER_INFO info;

            GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &info);

            Max_Width = info.dwSize.X;
            Max_Height = info.dwSize.Y;
        }
    
    #else
        void ClearScreen();

        void Render_Frame(string& Frame);
    
        void Set_Cursor_At(Coordinates C);

        void Update_Max_Width_And_Height();
    #endif

    int Get_Max_Width(){
        if (Max_Width == 0 && Max_Height == 0){
            Update_Max_Width_And_Height();
        }
        else{
            return Max_Width;
        }
    }

    int Get_Max_Height(){
        if (Max_Width == 0 && Max_Height == 0){
            Update_Max_Width_And_Height();
        }
        else{
            return Max_Height;
        }
    }

    //Returns a char if given ASCII, or a short if given UNICODE
    Text* Get(Coordinates Abselute_Position){
        if (Abselute_Position.X >= Get_Max_Width() || 
            Abselute_Position.Y >= Get_Max_Height() ||
            Abselute_Position.X < 0 || 
            Abselute_Position.Y < 0)
        {
            return nullptr; //Later on make a 
        }
        else{
            return &Frame_Buffer[Abselute_Position.Y * Get_Max_Width() + Abselute_Position.X];
        }
    }

    void Init_Renderer(){
        Update_Max_Width_And_Height();
        Constants::Init();
        Utils::Init();

        //now we need to allocate the buffer string by the width and height of the terminal
        Frame_Buffer.resize(Max_Height * Max_Width);
    }

    //This function inserts the window into the screen buffer
    void Render_Window(Window* w){

        

    }

}

#endif