#ifndef _RENDERER_H_
#define _RENDERER_H_

#include "Window.h"
#include "Constants.h"

namespace Utils{
    std::string (*Bold)(std::string text);
    std::string (*Color)(std::string text, std::string color);

        #if _WIN32
            #include <windows.h>

            void Init(){
                Bold = [](std::string Text){
                    return std::string("\033[1m") + Text;
                };

                Color = [](std::string Text, std::string Color){
                    return Color + Text + COLOR::RESET;
                };
            }
        #else            
            void Init();
        #endif
}

namespace RENDERER{
    std::vector<WINDOW::UTF> Frame_Buffer;                   //2D clean vector whitout bold nor color

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

        void Render_Frame(std::string& Frame){
            ClearScreen();
            printf(Frame.data());
        }
    
        void Set_Cursor_At(WINDOW::Coordinates C){
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
    WINDOW::UTF* Get(WINDOW::Coordinates Abselute_Position){
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

    std::vector<WINDOW::UTF> Add_Overhead(WINDOW::Window* w){
        if (!w->Show_Border)
            return;

        std::vector<WINDOW::UTF> Result;
        Result.resize(w->Height * w->Width);

        for (int y = 0; y < w->Height; y++){
            for (int x = 0; x < w->Width; x++){
                //top left corner
                if (y == 0 && x == 0){
                    Result[y * w->Width + x] = SYMBOLS::TOP_LEFT_CORNER;
                }
                //top right corner
                else if (y == 0 && x == w->Width - 1){
                    Result[y * w->Width + x] = SYMBOLS::TOP_RIGHT_CORNER;
                }
                //bottom left corner
                else if (y == w->Height - 1 && x == 0){
                    Result[y * w->Width + x] = SYMBOLS::BOTTOM_LEFT_CORNER;
                }
                //bottom right corner
                else if (y == w->Height - 1 && x == w->Width - 1){
                    Result[y * w->Width + x] = SYMBOLS::BOTTOM_RIGHT_CORNER;
                }
                //The title will only be written after the top left corner symbol until top right corner symbol and will NOT overflow
                else if (y == 0 && x < w->Title.size()){
                    Result[y * w->Width + x] = w->Title[x];
                }
                //The roof border
                else if (y == 0){
                    Result[y * w->Width + x] = SYMBOLS::TOP;
                }
                //The floor border
                else if (y == w->Height - 1){
                    Result[y * w->Width + x] = SYMBOLS::BOTTOM;
                }
                //The left border
                else if (x == 0){
                    Result[y * w->Width + x] = SYMBOLS::LEFT;
                }
                //The right border
                else if (x == w->Width - 1){
                    Result[y * w->Width + x] = SYMBOLS::RIGHT;
                }
            }
        }

        return Result;
    }

    void Nest_Window(WINDOW::Window* Parent, WINDOW::Window* Child, std::vector<WINDOW::UTF>& Parent_Buffer, std::vector<WINDOW::UTF> Child_Buffer){
        for (int Child_Y = 0; Child_Y < Child->Height; Child_Y++){
            for (int Child_X = 0; Child_X < Child->Width; Child_X++){
                Parent_Buffer[(Child->Y + Child_Y) * Parent->Width + Child->X + Child_X] = Child_Buffer[Child_Y * Child->Width + Child_X];
            }
        }
    }

    void Nest_UTF_Text(WINDOW::Window* Parent, vector<WINDOW::UTF>& Text, std::vector<WINDOW::UTF>& Parent_Buffer){
        for (int Parent_Y = 0; Parent_Y < Parent->Width; Parent_Y++){
            for (int Parent_X = 0; Parent_X < Parent->Height; Parent_X++){
                if (Parent_Y * Parent->Width + Parent_X < Text.size()){
                    Parent_Buffer[Parent_Y * Parent->Width + Parent_X] = Text[Parent_Y * Parent->Width + Parent_X];
                }
            }
        }
    }

    //Returns nested buffer of AST window's
    std::vector<WINDOW::UTF> Render_Window(WINDOW::Window* w){
        std::vector<WINDOW::UTF> Result;
        Result.resize(w->Width * w->Height);

        //This will add the borders if nessesary and the title of the window.
        Result = Add_Overhead(w);


        //This will add the content of the window to the Result buffer
        if (w->Content.size() > 0){
            Nest_UTF_Text(w, w->Content, Result);
        }

        //This will add the child windows to the Result buffer
        for (auto& c : w->Childs){
            Nest_Window(w, c, Result, Render_Window(c));
        }

        return Result;
    }

    std::string Liquify_UTF_Text(std::vector<WINDOW::UTF> Text, int Width, int Height){
        std::string Result;
        Result.resize(Width * Height);


        for (int y = 0; y < Height; y++){
            for (int x = 0; x < Width; x++){
                Result += Text[y * Width + x].To_String();
            }

            Result += "\n";
        }

        return Result;
    }
}

#endif