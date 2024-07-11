#include "Renderer.h"
#include "Elements/File_Streamer.h"

#include <string>
#include <cassert>
#include <math.h>
#include <sstream>
#include <condition_variable>
#include <mutex>

namespace GGUI{
    std::vector<UTF> Abstract_Frame_Buffer;                     // 2D clean vector without bold nor color
    std::string Frame_Buffer;                                   // string with bold and color, this what gets drawn to console.
    
    // THEAD SYSTEM --
    bool Pause_Render = false;                                  // if true, the render will not be updated, good for window creation.
    bool Pause_Event_Thread = false;                            // if true, the event handler will pause.

    std::mutex Atomic_Mutex;                                    // Gives ownership of boolean for single thread at a time.
    std::condition_variable Atomic_Condition;                   // Will lock all other threads until the wanted boolean is true.
    // --

    std::vector<INTERNAL::BUFFER_CAPTURE*> Global_Buffer_Captures;

    int Max_Width = 0;
    int Max_Height = 0;

    std::vector<Memory> Remember;

    std::vector<Action*> Event_Handlers;
    std::vector<Input*> Inputs;
    std::chrono::system_clock::time_point Last_Input_Clear_Time;

    std::unordered_map<std::string, Element*> Element_Names;

    Element* Focused_On = nullptr;
    Element* Hovered_On = nullptr;

    bool Platform_Initialized = false;

    Coordinates Mouse;
    //move 1 by 1, or element by element.
    bool Mouse_Movement_Enabled = true;

    std::unordered_map<std::string, BUTTON_STATE> KEYBOARD_STATES;
    std::unordered_map<std::string, BUTTON_STATE> PREVIOUS_KEYBOARD_STATES;

    // Represents the update speed of each elapsed loop of passive events, which do NOT need user as an input.
    inline time_t MAX_UPDATE_SPEED = TIME::SECOND;
    inline time_t MIN_UPDATE_SPEED = TIME::MILLISECOND * 16;    // Close approximation to 60 fps.
    inline time_t CURRENT_UPDATE_SPEED = MAX_UPDATE_SPEED;
    inline float Event_Thread_Load = 0.0f;  // Describes the load of animation and events from 0.0 to 1.0. Will reduce the event thread pause.

    std::chrono::high_resolution_clock::time_point Previous_Time;
    std::chrono::high_resolution_clock::time_point Current_Time;
    unsigned long long Delta_Time;

    inline std::unordered_map<int, Styling> Classes;

    inline std::unordered_map<std::string, int> Class_Names;

    std::unordered_map<GGUI::Terminal_Canvas*, bool> Multi_Frame_Canvas;

    Window* Main = nullptr;
    
    const std::string ERROR_LOGGER = "_ERROR_LOGGER_";
    const std::string HISTORY = "_HISTORY_";

    bool Collides(GGUI::Coordinates A, GGUI::Coordinates B, int A_Width, int A_Height, int B_Width, int B_Height){
        return (
            A.X < B.X + B_Width &&
            A.X + A_Width > B.X &&
            A.Y < B.Y + B_Height &&
            A.Y + A_Height > B.Y
        );
    }

    bool Collides(GGUI::Element* a, GGUI::Element* b, bool Identity){
        if (a == b)
            return Identity;    // For custom purposes, defaults into true
            
        return Collides(a->Get_Absolute_Position(), b->Get_Absolute_Position(), a->Get_Width(), a->Get_Height(), b->Get_Width(), b->Get_Height());
    }

    bool Collides(GGUI::Element* a, GGUI::Coordinates b){
        return Collides(a->Get_Absolute_Position(), b, a->Get_Width(), a->Get_Height(), 1, 1);
    }

    Element* Get_Accurate_Element_From(Coordinates c, Element* Parent){
        
        //first check if the c is in bounds of Parent.
        if (!Collides(Parent, c)){
            return nullptr;
        }

        for (auto child : Parent->Get_Childs()){
            if (Collides(child, c)){
                return Get_Accurate_Element_From(c, child);
            }
        }

        return Parent;
    }

    Coordinates Find_Upper_Element(){

        //finds what element is upper relative to this element that the mouse is hovering on top of.
        //first get the current element.
        Element* Current_Element = Get_Accurate_Element_From(Mouse, Main);

        if (Current_Element == nullptr){
            return false;
        }

        Coordinates tmp_c = Current_Element->Get_Position();

        tmp_c.Y--;

        Element* Upper_Element = Get_Accurate_Element_From(tmp_c, Main);

        if (Upper_Element && Upper_Element != (Element*)&Main){
            return Upper_Element->Get_Position();
        }

        return Current_Element->Get_Position();
    }

    Coordinates Find_Lower_Element(){
        //finds what element is upper relative to this element that the mouse is hovering on top of.
        //first get the current element.
        Element* Current_Element = Get_Accurate_Element_From(Mouse, Main);

        if (Current_Element == nullptr){
            return false;
        }

        Coordinates tmp_c = Current_Element->Get_Position();

        tmp_c.Y += Current_Element->Get_Height();

        Element* Lower_Element = Get_Accurate_Element_From(tmp_c, Main);

        if (Lower_Element && Lower_Element != (Element*)&Main){
            return Lower_Element->Get_Position();
        }

        return Current_Element->Get_Position();
    }

    Coordinates Find_Left_Element(){
        //finds what element is upper relative to this element that the mouse is hovering on top of.
        //first get the current element.
        Element* Current_Element = Get_Accurate_Element_From(Mouse, Main);

        if (Current_Element == nullptr){
            return false;
        }

        Coordinates tmp_c = Current_Element->Get_Position();

        tmp_c.X--;

        Element* Left_Element = Get_Accurate_Element_From(tmp_c, Main);

        if (Left_Element && Left_Element != (Element*)&Main){
            return Left_Element->Get_Position();
        }

        return Current_Element->Get_Position();
    }

    Coordinates Find_Right_Element(){
        //finds what element is upper relative to this element that the mouse is hovering on top of.
        //first get the current element.
        Element* Current_Element = Get_Accurate_Element_From(Mouse, Main);

        if (Current_Element == nullptr){
            return false;
        }

        Coordinates tmp_c = Current_Element->Get_Position();

        tmp_c.X += Current_Element->Get_Width();

        Element* Right_Element = Get_Accurate_Element_From(tmp_c, Main);

        if (Right_Element && Right_Element != (Element*)&Main){
            return Right_Element->Get_Position();
        }

        return Current_Element->Get_Position();
    }

    Element* Find_Closest_Absolute_Element(Coordinates start, std::vector<Element*> Candidates){
        // Start from the position and check if the up, down, left, right are within the bounds of the renderable window.
        // If they are, check if they collide with any element.
        // cast "rays" to each four directions, and return the lenghts of each collision between the center of the rectangles and the start point.
        // return the smallest one.
        if (Candidates.size() == 0){
            Report("Missing Candidates!");
        }

        Element* Best_Candidate = nullptr;
        float Shortest_Distance = std::numeric_limits<float>::max();

        for (auto& candidate : Candidates){
            if (!candidate) 
                continue;   // Incase of event handlers with their stupid empty hosters.
            // Calculate the distance between the candidate position and the start position
            Coordinates CC = candidate->Get_Absolute_Position();
            float Distance = std::sqrt(std::pow(CC.X - start.X, 2) + std::pow(CC.Y - start.Y, 2));

            if (Distance < Shortest_Distance){
                Shortest_Distance = Distance;
                Best_Candidate = candidate;
            }
        }

        return Best_Candidate;
    }

    signed long long Min(signed long long a, signed long long b){
        return a < b ? a : b;
    }

    signed long long Max(signed long long a, signed long long b){
        return a > b ? a : b;
    }

    #if _WIN32
    #include <windows.h>
    #include <DbgHelp.h>

    void SLEEP(unsigned int mm){
        Sleep(mm);
    }

    GGUI::HANDLE GLOBAL_STD_OUTPUT_HANDLE;
    GGUI::HANDLE GLOBAL_STD_INPUT_HANDLE;

    DWORD PREVIOUS_CONSOLE_OUTPUT_STATE;
    DWORD PREVIOUS_CONSOLE_INPUT_STATE;

    CONSOLE_SCREEN_BUFFER_INFO Get_Console_Info();

    void Render_Frame(){
        unsigned long long tmp = 0;
        SetConsoleCursorPosition(GLOBAL_STD_OUTPUT_HANDLE, {0, 0});
        WriteFile(GLOBAL_STD_OUTPUT_HANDLE, Frame_Buffer.data(), Frame_Buffer.size(), reinterpret_cast<LPDWORD>(&tmp), NULL);
    }

    void Update_Max_Width_And_Height(){
        CONSOLE_SCREEN_BUFFER_INFO info = Get_Console_Info();

        Max_Width = info.srWindow.Right - info.srWindow.Left + 1;
        Max_Height = info.srWindow.Bottom - info.srWindow.Top + 1;  // this doesn't take into consideration of politics

        if (Max_Width == 0 || Max_Height == 0){
            Report("Failed to get console info!");
        }
    }

    void Update_Frame(bool Lock_Event_Thread);
    //Is called on every cycle.

    char Reverse_Engineer_Keybind(char keybind_value){
        // The current known keybinding table:
        /*
            CTRL+SHIFT+I => TAB

        */

        if (KEYBOARD_STATES[BUTTON_STATES::CONTROL].State && KEYBOARD_STATES[BUTTON_STATES::SHIFT].State){
            if (keybind_value == VK_TAB){
                return 'i';
            }
        }

        // return the normal value, if there is no key-binds detected.
        return keybind_value;
    }

    void Query_Inputs(){
        INPUT_RECORD Input[UINT8_MAX];

        int Buffer_Size = 0;

        // Clean the keyboard states.
        PREVIOUS_KEYBOARD_STATES = KEYBOARD_STATES;

        ReadConsoleInput(GLOBAL_STD_INPUT_HANDLE, Input, UINT8_MAX, (LPDWORD)&Buffer_Size);

        for (int i = 0; i < Buffer_Size; i++){
            if (Input[i].EventType == KEY_EVENT){

                bool Pressed = Input[i].Event.KeyEvent.bKeyDown;

                if (Input[i].Event.KeyEvent.wVirtualKeyCode == VK_UP){
                    Inputs.push_back(new GGUI::Input(0, Constants::UP));
                    KEYBOARD_STATES[BUTTON_STATES::UP] = BUTTON_STATE(Pressed);
                }
                else if (Input[i].Event.KeyEvent.wVirtualKeyCode == VK_DOWN){
                    Inputs.push_back(new GGUI::Input(0, Constants::DOWN));
                    KEYBOARD_STATES[BUTTON_STATES::DOWN] = BUTTON_STATE(Pressed);
                }
                else if (Input[i].Event.KeyEvent.wVirtualKeyCode == VK_LEFT){
                    Inputs.push_back(new GGUI::Input(0, Constants::LEFT));
                    KEYBOARD_STATES[BUTTON_STATES::LEFT] = BUTTON_STATE(Pressed);
                }
                else if (Input[i].Event.KeyEvent.wVirtualKeyCode == VK_RIGHT){
                    Inputs.push_back(new GGUI::Input(0, Constants::RIGHT));
                    KEYBOARD_STATES[BUTTON_STATES::RIGHT] = BUTTON_STATE(Pressed);
                }
                else if (Input[i].Event.KeyEvent.wVirtualKeyCode == VK_RETURN){
                    Inputs.push_back(new GGUI::Input('\n', Constants::ENTER));
                    KEYBOARD_STATES[BUTTON_STATES::ENTER] = BUTTON_STATE(Pressed);
                }
                else if (Input[i].Event.KeyEvent.wVirtualKeyCode == VK_SHIFT){
                    Inputs.push_back(new GGUI::Input(' ', Constants::SHIFT));
                    KEYBOARD_STATES[BUTTON_STATES::SHIFT] = BUTTON_STATE(Pressed);
                }
                else if (Input[i].Event.KeyEvent.wVirtualKeyCode == VK_CONTROL){
                    Inputs.push_back(new GGUI::Input(' ', Constants::CONTROL));
                    KEYBOARD_STATES[BUTTON_STATES::CONTROL] = BUTTON_STATE(Pressed);
                }
                else if (Input[i].Event.KeyEvent.wVirtualKeyCode == VK_BACK){
                    Inputs.push_back(new GGUI::Input(' ', Constants::BACKSPACE));
                    KEYBOARD_STATES[BUTTON_STATES::BACKSPACE] = BUTTON_STATE(Pressed);
                }
                else if (Input[i].Event.KeyEvent.wVirtualKeyCode == VK_ESCAPE){
                    Inputs.push_back(new GGUI::Input(' ', Constants::ESCAPE));
                    KEYBOARD_STATES[BUTTON_STATES::ESC] = BUTTON_STATE(Pressed);
                    Handle_Escape();
                }
                else if (Input[i].Event.KeyEvent.wVirtualKeyCode == VK_TAB){
                    Inputs.push_back(new GGUI::Input(' ', Constants::TAB));
                    KEYBOARD_STATES[BUTTON_STATES::TAB] = BUTTON_STATE(Pressed);
                    Handle_Tabulator();
                }
                else if (Input[i].Event.KeyEvent.uChar.AsciiChar != 0 && Pressed){
                    char Result = Reverse_Engineer_Keybind(Input[i].Event.KeyEvent.uChar.AsciiChar);

                    Inputs.push_back(new GGUI::Input(Result, Constants::KEY_PRESS));
                }
            }
            else if (Input[i].EventType == WINDOW_BUFFER_SIZE_EVENT){

                Update_Max_Width_And_Height();

                // For outbox use.
                if (Main)
                    Main->Set_Dimensions(Max_Width, Max_Height);
            }
            else if (Input[i].EventType == MOUSE_EVENT && Mouse_Movement_Enabled){
                if (Input[i].Event.MouseEvent.dwEventFlags == MOUSE_MOVED){
                    // Get mouse coordinates
                    COORD mousePos = Input[i].Event.MouseEvent.dwMousePosition;
                    // Handle cursor movement
                    Mouse.X = mousePos.X;
                    Mouse.Y = mousePos.Y;
                }
                // Handle mouse clicks
                // TODO: Windows doesn't give release events.
                // Yes it does you fucking moron!
                if ((Input[i].Event.MouseEvent.dwButtonState & FROM_LEFT_1ST_BUTTON_PRESSED) != 0) {
                    //PREVIOUS_KEYBOARD_STATES[BUTTON_STATES::MOUSE_LEFT].State = KEYBOARD_STATES[BUTTON_STATES::MOUSE_LEFT].State;
                    KEYBOARD_STATES[BUTTON_STATES::MOUSE_LEFT].State = true;
                    KEYBOARD_STATES[BUTTON_STATES::MOUSE_LEFT].Capture_Time = std::chrono::high_resolution_clock::now();
                }
                else if ((Input[i].Event.MouseEvent.dwButtonState & FROM_LEFT_1ST_BUTTON_PRESSED) == 0) {
                    //PREVIOUS_KEYBOARD_STATES[BUTTON_STATES::MOUSE_LEFT].State = KEYBOARD_STATES[BUTTON_STATES::MOUSE_LEFT].State;
                    KEYBOARD_STATES[BUTTON_STATES::MOUSE_LEFT].State = false;
                }

                if ((Input[i].Event.MouseEvent.dwButtonState & RIGHTMOST_BUTTON_PRESSED) != 0) {
                    //PREVIOUS_KEYBOARD_STATES[BUTTON_STATES::MOUSE_RIGHT].State = KEYBOARD_STATES[BUTTON_STATES::MOUSE_RIGHT].State;
                    KEYBOARD_STATES[BUTTON_STATES::MOUSE_RIGHT].State = true;
                    KEYBOARD_STATES[BUTTON_STATES::MOUSE_RIGHT].Capture_Time = std::chrono::high_resolution_clock::now();
                }
                else if ((Input[i].Event.MouseEvent.dwButtonState & RIGHTMOST_BUTTON_PRESSED) == 0) {
                    //PREVIOUS_KEYBOARD_STATES[BUTTON_STATES::MOUSE_RIGHT].State = KEYBOARD_STATES[BUTTON_STATES::MOUSE_RIGHT].State;
                    KEYBOARD_STATES[BUTTON_STATES::MOUSE_RIGHT].State = false;
                }
            
                // mouse scroll up
                if (Input[i].Event.MouseEvent.dwEventFlags == MOUSE_WHEELED){
                    // check if it has been wheeled up or down
                    int Scroll_Direction = GET_WHEEL_DELTA_WPARAM(Input[i].Event.MouseEvent.dwButtonState);

                    if (Scroll_Direction > 0){
                        KEYBOARD_STATES[BUTTON_STATES::MOUSE_SCROLL_UP].State = true;
                        KEYBOARD_STATES[BUTTON_STATES::MOUSE_SCROLL_DOWN].State = false;

                        KEYBOARD_STATES[BUTTON_STATES::MOUSE_SCROLL_UP].Capture_Time = std::chrono::high_resolution_clock::now();
                    }
                    else if (Scroll_Direction < 0){
                        KEYBOARD_STATES[BUTTON_STATES::MOUSE_SCROLL_DOWN].State = true;
                        KEYBOARD_STATES[BUTTON_STATES::MOUSE_SCROLL_UP].State = false;

                        KEYBOARD_STATES[BUTTON_STATES::MOUSE_SCROLL_DOWN].Capture_Time = std::chrono::high_resolution_clock::now();
                    }
                }
            }
        }

        SCROLL_API();

        MOUSE_API();
    }

    void Init_Platform_Stuff(){
        // Save the STD handles to prevent excess calls.
        GLOBAL_STD_OUTPUT_HANDLE = GetStdHandle(STD_OUTPUT_HANDLE);
        GLOBAL_STD_INPUT_HANDLE = GetStdHandle(STD_INPUT_HANDLE);

        GetConsoleMode(GLOBAL_STD_OUTPUT_HANDLE, &PREVIOUS_CONSOLE_OUTPUT_STATE);
        GetConsoleMode(GLOBAL_STD_INPUT_HANDLE, &PREVIOUS_CONSOLE_INPUT_STATE);

        SetConsoleMode(GLOBAL_STD_OUTPUT_HANDLE, -1);
        SetConsoleMode(GLOBAL_STD_INPUT_HANDLE, ENABLE_EXTENDED_FLAGS | ENABLE_MOUSE_INPUT | ENABLE_WINDOW_INPUT );

        std::cout << Constants::EnableFeature(Constants::REPORT_MOUSE_ALL_EVENTS) + Constants::DisableFeature(Constants::MOUSE_CURSOR);
        std::cout.flush();

        SetConsoleOutputCP(Constants::ENABLE_UTF8_MODE_FOR_WINDOWS);

        Platform_Initialized = true;
    }

    CONSOLE_SCREEN_BUFFER_INFO Get_Console_Info(){
        if (GLOBAL_STD_OUTPUT_HANDLE == 0){
            GLOBAL_STD_OUTPUT_HANDLE = GetStdHandle(STD_OUTPUT_HANDLE);
        }

        CONSOLE_SCREEN_BUFFER_INFO Result;

        // first get the size of the file
        if (!GetConsoleScreenBufferInfo(GLOBAL_STD_OUTPUT_HANDLE, &Result)){

            int Last_Error = GetLastError();

            Report("Failed to get console info: " + std::to_string(Last_Error));
        }

        return Result;
    }

    std::vector<char> Read_Console(){
        std::vector<char> Buffer;
        std::vector<CHAR_INFO> Fake_Buffer;

        CONSOLE_SCREEN_BUFFER_INFO Info = Get_Console_Info();

        Buffer.resize(Info.dwSize.X * Info.dwSize.Y);
        Fake_Buffer.resize(Info.dwSize.X * Info.dwSize.Y);

        // The area to read:
        SMALL_RECT rect {0,0, Info.dwSize.X-1, Info.dwSize.Y} ; // the console screen (buffer) region to read from (120x4)
        ReadConsoleOutput( GLOBAL_STD_OUTPUT_HANDLE, Fake_Buffer.data(), {Info.dwSize.X, Info.dwSize.Y} /*buffer size colsxrows*/, {0,0} /*buffer top,left*/, &rect );

        // now transform all the data from CHAR_INFO to char
        for (unsigned int i = 0; i < Fake_Buffer.size(); i++){
            Buffer[i] = Fake_Buffer[i].Char.AsciiChar;
        }

        return Buffer;
    }

    void De_Initialize(){
        // Also handles STD_COUT capture restorations.
        for (auto File_Handle : File_Streamer_Handles){
            File_Handle.second->~FILE_STREAM();
        }

        SetConsoleMode(GLOBAL_STD_OUTPUT_HANDLE, PREVIOUS_CONSOLE_OUTPUT_STATE);
        SetConsoleMode(GLOBAL_STD_INPUT_HANDLE, PREVIOUS_CONSOLE_INPUT_STATE);

        std::cout << Constants::EnableFeature(Constants::MOUSE_CURSOR);
        std::cout << Constants::DisableFeature(Constants::REPORT_MOUSE_ALL_EVENTS);
        std::cout << Constants::DisableFeature(Constants::SCREEN_CAPTURE);  // restores the screen.
        std::cout << std::flush;
    }

    void Exit(int signum){
        De_Initialize();

        exit(signum);
    }

    // For getting all font files:
    std::vector<std::string> Get_List_Of_Font_Files() {
        std::vector<std::string> Result;
        HKEY hKey;
        if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, "Software\\Microsoft\\Windows NT\\CurrentVersion\\Fonts", 0, KEY_READ, &hKey) == ERROR_SUCCESS) {
            DWORD cValues; 
            RegQueryInfoKey(hKey, NULL, NULL, NULL, NULL, NULL, NULL, &cValues, NULL, NULL, NULL, NULL);
            for (DWORD i = 0; i < cValues; i++) {
                char valueName[1024];
                DWORD valueNameSize = 1024; 
                BYTE valueData[1024];
                DWORD valueDataSize = 1024;
                DWORD valueType;
                if (RegEnumValue(hKey, i, valueName, &valueNameSize, NULL, &valueType, valueData, &valueDataSize) == ERROR_SUCCESS) {
                    // add the file to the Result list.
                    Result.push_back((char*)valueData);
                }
            }
            RegCloseKey(hKey);
        }

        return Result;
    }

    void Report_Stack(std::string Problem){
        const int Stack_Trace_Depth = 10;
        void* Ptr_Table[Stack_Trace_Depth];
        unsigned short Usable_Depth;
        SYMBOL_INFO* symbol;
        HANDLE process;

        process = GetCurrentProcess();

        SymInitialize(process, NULL, TRUE);
        Usable_Depth = CaptureStackBackTrace(0, Stack_Trace_Depth, Ptr_Table, NULL);
        symbol = (SYMBOL_INFO*)calloc(sizeof(SYMBOL_INFO) + 256 * sizeof(char), 1);
        symbol->MaxNameLen = 255;
        symbol->SizeOfStruct = sizeof(SYMBOL_INFO);

        if (Max_Width == 0){
            Update_Max_Width_And_Height();
        }

        std::string Result = "Stack Trace:\n";
        int Usable_Stack_Index = 0;
        bool Use_Indent = Usable_Depth < (Max_Width / 2);
        for (unsigned int Stack_Index = 0; Stack_Index < Usable_Depth; Stack_Index++){
            SymFromAddr(process, (DWORD64)(Ptr_Table[Stack_Index]), 0, symbol);

            // check if the symbol->name is empty, if so then skip
            if (symbol->Name[0] == 0)
                continue;

            std::string Branch_Start = SYMBOLS::VERTICAL_RIGHT_CONNECTOR;

            // For last branch use different branch start symbol
            if (Stack_Index == (unsigned int)Usable_Depth - 1)
                Branch_Start = SYMBOLS::BOTTOM_LEFT_CORNER;

            // now add indentation by the amount of index:
            std::string Indent = "";

            for (int i = 0; i < Usable_Stack_Index && Use_Indent; i++)
                Indent += SYMBOLS::HORIZONTAL_LINE;

            Result += Branch_Start + Indent + symbol->Name + "\n";

            Usable_Stack_Index++;
        }

        free(symbol);

        Result += "Problem: " + Problem;

        Report(Result);
    }

    #else
    #include <sys/ioctl.h>
    #include <signal.h>
    #include <termios.h>
    #include <execinfo.h>

    int Previus_Flags = 0;
    struct termios Previus_Raw;
    void De_Initialize(){
        // Also handles STD_COUT capture restorations.
        for (auto File_Handle : File_Streamer_Handles){
            File_Handle.second->~FILE_STREAM();
        }

        std::cout << Constants::EnableFeature(Constants::MOUSE_CURSOR);
        std::cout << Constants::DisableFeature(Constants::REPORT_MOUSE_ALL_EVENTS);
        std::cout << Constants::DisableFeature(Constants::SCREEN_CAPTURE);  // restores the screen.
        std::cout << std::flush;

        fcntl(STDIN_FILENO, F_SETFL, Previus_Flags); // set non-blocking flag
        tcsetattr(STDIN_FILENO, TCSAFLUSH, &Previus_Raw);
    }

    void Exit(int signum){

        De_Initialize();

        exit(signum);
    }

    void SLEEP(unsigned int mm){
        // use nanosleep
        struct timespec req = {0};
        time_t sec = (int)(mm / 1000);
        mm = mm - (sec * 1000);
        req.tv_sec = sec;
        req.tv_nsec = mm * 1000000L;
        while(nanosleep(&req, &req) == -1)
            continue;
    }


    void Render_Frame() {
        // Move cursor to top-left corner
        // Previously used: Constants::CLEAR_SCROLLBAR + As a prefix to clear the history, when it was accumulating, but lately tests point this to be useless.
        printf((Constants::SET_CURSOR_TO_START).c_str());

        // Flush the output to ensure it's written immediately
        fflush(stdout);

        int printed = 0;
        printed = write(1, Frame_Buffer.data(), Frame_Buffer.size());
    }

    void Update_Max_Width_And_Height(){
        struct winsize w;
        ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);

        Max_Width = w.ws_col;
        Max_Height = w.ws_row - 1;

        // Convenience sake :)
        if (Main)
            Main->Set_Dimensions(Max_Width, Max_Height);
    }

    // this global variable is only meant for unix use, since unix has ability to return non finished input events, so we need to store the first half for later use.
    std::vector<char> Input_Buffer;

    namespace Constants{
        constexpr char START_OF_CTRL = 1;
        constexpr char END_OF_CTRL = 26;
    };

    // Takes in an buffer with hex and octal values and transforms them into printable strings
    std::string To_String(char* buffer, int length, bool Obfuscate = false){
        std::string Result = "";

        for (int i = 0; i < length; i++){
            // This is used as an precession to avoid accidental ANSI escape sequences to trigger from printing the buffer contents.
            if (Obfuscate)
                Result += " ";

            if (isprint(buffer[i])){
                Result += buffer[i];
                continue;
            }

            // add base
            Result += std::to_string((unsigned char)buffer[i]);
        }

        return Result;
    }

    //Is called as soon as possible and gets stuck awaiting for the user input.
    void Query_Inputs(){
        // The code gets stuck here waiting for the user input, only proceed after required from user the input.
        // Since capturing an longer stream in an un-supervised situation might halt, we need to just choply take each character one by one and determining the rest ourselves smh.
        char Buffer[256];
        int Bytes_Read = read(STDIN_FILENO, Buffer, sizeof(Buffer));

        // All of the if-else statements could just make into a map, where each key of combination replaces the value of the keyboard state, but you know what?
        // haha code go brrrr -- 2024 gab here, nice joke, although who asked? 
        for (unsigned int i = 0; i < Bytes_Read; i++){

            // Check if SHIFT has been modifying the keys
            if ((Buffer[i] >= 'A' && Buffer[i] <= 'Z') || (Buffer[i] >= '!' && Buffer[i] <= '/')){
                // SHIFT key is pressed
                KEYBOARD_STATES[BUTTON_STATES::SHIFT] = BUTTON_STATE(true);
            }
            // We now can also check if the letter has been shifted down by CTRL key
            else if (Buffer[i] >= Constants::START_OF_CTRL && Buffer[i] <= Constants::END_OF_CTRL){
                // This is a CTRL key

                // The CTRL domain contains multiple useful keys to check for
                if (Buffer[i] == Constants::BACKSPACE){
                    // This is a backspace key
                    Inputs.push_back(new GGUI::Input(' ', Constants::BACKSPACE));
                    KEYBOARD_STATES[BUTTON_STATES::BACKSPACE] = BUTTON_STATE(true);
                }
                else if (Buffer[i] == Constants::TAB){
                    // This is a tab key
                    Inputs.push_back(new GGUI::Input(' ', Constants::TAB));
                    KEYBOARD_STATES[BUTTON_STATES::TAB] = BUTTON_STATE(true);
                }
                else if (Buffer[i] == Constants::ENTER){
                    // This is an enter key
                    Inputs.push_back(new GGUI::Input(' ', Constants::ENTER));
                    KEYBOARD_STATES[BUTTON_STATES::ENTER] = BUTTON_STATE(true);
                }

                // Shift the key back up
                char Offset = 'a' -1; // We remove one since the CTRL characters started from 1 and not 0
                Buffer[i] = Buffer[i] + Offset;
                KEYBOARD_STATES[BUTTON_STATES::CONTROL] = BUTTON_STATE(true);
            }

            if (Buffer[i] == Constants::ESC_CODE[0]){
                // check if there are stuff after this escape code
                if (i + 1 >= Bytes_Read){
                    // Clearly the escape key was invoked
                    Inputs.push_back(new GGUI::Input(' ', Constants::ESCAPE));
                    KEYBOARD_STATES[BUTTON_STATES::ESC] = BUTTON_STATE(true);
                    continue;
                }

                // GG go next
                i++;


                // The current data can either be an ALT key initiative or an escape sequence followed by '['
                if (Buffer[i] == Constants::ESC_CODE[1]){
                    // Escape sequence codes:

                    // UP, DOWN LEFT, RIGHT keys
                    if (Buffer[i + 1] == 'A'){
                        Inputs.push_back(new GGUI::Input(0, Constants::UP));
                        KEYBOARD_STATES[BUTTON_STATES::UP] = BUTTON_STATE(true);
                        i++;
                    }
                    else if (Buffer[i + 1] == 'B'){
                        Inputs.push_back(new GGUI::Input(0, Constants::DOWN));
                        KEYBOARD_STATES[BUTTON_STATES::DOWN] = BUTTON_STATE(true);
                        i++;
                    }
                    else if (Buffer[i + 1] == 'C'){
                        Inputs.push_back(new GGUI::Input(0, Constants::RIGHT));
                        KEYBOARD_STATES[BUTTON_STATES::RIGHT] = BUTTON_STATE(true);
                        i++;
                    }
                    else if (Buffer[i + 1] == 'D'){
                        Inputs.push_back(new GGUI::Input(0, Constants::LEFT));
                        KEYBOARD_STATES[BUTTON_STATES::LEFT] = BUTTON_STATE(true);
                        i++;
                    }

                    if (Buffer[i + 1] == 'M'){  // Decode Mouse handling
                        // Payload structure: '\e[Mbxy' where the b is bitmask representing the buttons, x and y representing the location of the mouse. 
                        char Bit_Mask = Buffer[i+2]; 
                        char X = Buffer[i+3];
                        char Y = Buffer[i+4];

                        Mouse.X = X;
                        Mouse.Y = Y;

                        // Bit_Mask & ~32 removes the 32'th bit, (Bit_Mask &= ~32, 0) removes the bit from the Bit_Mask variable and returns the number 0 for the OR.
                        KEYBOARD_STATES[BUTTON_STATES::CONTROL] = BUTTON_STATE(Bit_Mask & ~32 | (Bit_Mask &= ~32, 0));

                        if (Bit_Mask == 0){
                            KEYBOARD_STATES[BUTTON_STATES::MOUSE_LEFT] = BUTTON_STATE(true);
                        }
                        else if (Bit_Mask == 1){
                            KEYBOARD_STATES[BUTTON_STATES::MOUSE_MIDDLE] = BUTTON_STATE(true);
                        }
                        else if (Bit_Mask == 2){
                            KEYBOARD_STATES[BUTTON_STATES::MOUSE_RIGHT] = BUTTON_STATE(true);
                        }

                        i += 4;
                    }
                }
                else{
                    // This is an ALT key
                    Inputs.push_back(new GGUI::Input(Buffer[i], Constants::ALT));
                    KEYBOARD_STATES[BUTTON_STATES::ALT] = BUTTON_STATE(true);
                }


            }
            else{
                // Normal character data
                Inputs.push_back(new GGUI::Input(Buffer[i], Constants::KEY_PRESS));
            }

        }
    
        SCROLL_API();

        MOUSE_API();
    }

    void Init_Platform_Stuff(){
        std::cout << Constants::EnableFeature(Constants::REPORT_MOUSE_ALL_EVENTS);
        std::cout << Constants::DisableFeature(Constants::MOUSE_CURSOR);
        std::cout << Constants::EnableFeature(Constants::SCREEN_CAPTURE);   // for on exit to restore
        // std::cout << Constants::RESET_CONSOLE;
        // std::cout << Constants::EnableFeature(Constants::ALTERNATIVE_SCREEN_BUFFER);    // For double buffer if needed
        std::cout << std::flush;

        // Setup the new flags and take an snapshot of the flags before GGUI
        Previus_Flags = fcntl(STDIN_FILENO, F_GETFL, 0);
        int flags = O_RDONLY | O_CLOEXEC;
        fcntl(STDIN_FILENO, F_SETFL, flags); // set non-blocking flag

        struct termios Term_Handle;
        tcgetattr(STDIN_FILENO, &Term_Handle);

        Previus_Raw = Term_Handle;

        Term_Handle.c_lflag &= ~(ECHO | ICANON);
        Term_Handle.c_cc[VMIN] = 1;     // This dictates how many characters until the user input awaiting read() function will return the buffer.
        Term_Handle.c_cc[VTIME] = 0;    // Suppress automatic returning, since we want the Input_Query() to await until the user has given an input.  
        tcsetattr(STDIN_FILENO, TCSAFLUSH, &Term_Handle);

        /*
        SIGINT -> This is on by default, if then user makes an element to be focused with capabilities to capture this, then well remove it from the Exit codes.
        SIGILL
        SIGABRT
        SIGFPE
        SIGSEGV
        SIGTERM
        */

        struct sigaction* wrapper = new struct sigaction();
        wrapper->sa_handler = Exit;
        sigemptyset(&wrapper->sa_mask);
        wrapper->sa_flags = 0;

        for (
            auto i : {
                SIGINT,
                SIGILL,
                SIGABRT,
                SIGFPE,
                SIGSEGV,
                SIGTERM
            }){
            sigaction(i, wrapper, NULL);
        }
    }

    std::vector<std::string> Get_List_Of_Font_Files(){
        CMD Handle;

        // we will run the command and it will return us a list of all font file names.
        std::string Raw_Result = Handle.Run("fc-list -v | grep file");

        std::vector<std::string> File_Names;
        std::stringstream ss(Raw_Result);
        std::string temp;

        // Process the output line by line
        while(std::getline(ss, temp, '\n')) {
            // Extract the file name from each line and add it to the vector
            std::size_t pos = temp.find(": ");
            if(pos != std::string::npos) {
                std::string file_name = temp.substr(pos+2);
                File_Names.push_back(file_name);
            }
        }

        return File_Names;
    }

    void Report_Stack(std::string Problem){
        int Stack_Trace_Depth = 10;
        void *Ptr_Table[Stack_Trace_Depth];

        // Declare a pointer to an array of strings. This will hold the symbol names of the stack trace
        char **Name_Table;

        // Get the stack trace and store it in the array. The return value is the number of stack frames obtained
        size_t Usable_Depth = backtrace(Ptr_Table, Stack_Trace_Depth);

        // Convert the addresses in the stack trace into an array of strings that describe the addresses symbolically
        Name_Table = backtrace_symbols(Ptr_Table, Usable_Depth);

        if (Max_Width == 0){
            Update_Max_Width_And_Height();
        }

        // Now that we have the stack frame label names in the Name_Table list, we can construct an visually apleasing stack trace information:
        std::string Result = "Stack Trace:\n";

        bool Use_Indent = Usable_Depth < (Max_Width / 2);
        for (int Stack_Index = 0; Stack_Index < Usable_Depth; Stack_Index++){
            std::string Branch_Start = SYMBOLS::VERTICAL_RIGHT_CONNECTOR;

            // For last branch use different branch start symbol
            if (Stack_Index == Usable_Depth - 1)
                Branch_Start = SYMBOLS::BOTTOM_LEFT_CORNER;

            // now add indentation by the amount of index:
            std::string Indent = "";

            for (int i = 0; i < Stack_Index && Use_Indent; i++)
                Indent += SYMBOLS::HORIZONTAL_LINE;

            Result += Branch_Start + Indent + Name_Table[Stack_Index] + "\n";
        }

        // Free the memory allocated for the string array
        free(Name_Table);

        // now add the problem into the message
        Result += "Problem: " + Problem;

        Report(Result);
    }

    #endif

    // Since the query inputs wont populate the same inputs on every cycle we need to go through the key states and create new inputs for them :)
    void Populate_Inputs_For_Held_Down_Keys(){
        for (auto Key : KEYBOARD_STATES){

            // Check if the key is activated
            if (Key.second.State){

                if (BUTTON_STATES::MOUSE_LEFT == Key.first || BUTTON_STATES::MOUSE_RIGHT == Key.first || BUTTON_STATES::MOUSE_MIDDLE == Key.first)
                    continue;

                //Go through the already existing inputs and make sure that it doesn't already exist
                unsigned long long Constant_Key = BUTTON_STATES_TO_CONSTANTS_BRIDGE.at(Key.first);

                bool Found = false;
                for (auto input : Inputs){
                    if (input->Criteria == Constant_Key){
                        Found = true;
                        break;
                    }
                }

                if (!Found)
                    Inputs.push_back(new Input((char)0, Constant_Key));
            }
        }
    }

    void MOUSE_API(){
        unsigned long long Mouse_Left_Pressed_For = (unsigned long long)std::chrono::duration_cast<std::chrono::milliseconds>(abs(Current_Time - KEYBOARD_STATES[BUTTON_STATES::MOUSE_LEFT].Capture_Time)).count();

        if (KEYBOARD_STATES[BUTTON_STATES::MOUSE_LEFT].State && Mouse_Left_Pressed_For >= SETTINGS::Mouse_Press_Down_Cooldown){
            Inputs.push_back(new GGUI::Input(0, Constants::MOUSE_LEFT_PRESSED));
        }
        else if (!KEYBOARD_STATES[BUTTON_STATES::MOUSE_LEFT].State && PREVIOUS_KEYBOARD_STATES[BUTTON_STATES::MOUSE_LEFT].State != KEYBOARD_STATES[BUTTON_STATES::MOUSE_LEFT].State){
            Inputs.push_back(new GGUI::Input(0, Constants::MOUSE_LEFT_CLICKED));
        }

        unsigned long long Mouse_Right_Pressed_For = (unsigned long long)std::chrono::duration_cast<std::chrono::milliseconds>(abs(Current_Time - KEYBOARD_STATES[BUTTON_STATES::MOUSE_RIGHT].Capture_Time)).count();

        if (KEYBOARD_STATES[BUTTON_STATES::MOUSE_RIGHT].State && Mouse_Right_Pressed_For >= SETTINGS::Mouse_Press_Down_Cooldown){
            Inputs.push_back(new GGUI::Input(0, Constants::MOUSE_RIGHT_PRESSED));
        }
        else if (!KEYBOARD_STATES[BUTTON_STATES::MOUSE_RIGHT].State && PREVIOUS_KEYBOARD_STATES[BUTTON_STATES::MOUSE_RIGHT].State != KEYBOARD_STATES[BUTTON_STATES::MOUSE_RIGHT].State){
            Inputs.push_back(new GGUI::Input(0, Constants::MOUSE_RIGHT_CLICKED));
        }

        unsigned long long Mouse_Middle_Pressed_For = (unsigned long long)std::chrono::duration_cast<std::chrono::milliseconds>(abs(Current_Time - KEYBOARD_STATES[BUTTON_STATES::MOUSE_MIDDLE].Capture_Time)).count();

        if (KEYBOARD_STATES[BUTTON_STATES::MOUSE_MIDDLE].State && Mouse_Middle_Pressed_For >= SETTINGS::Mouse_Press_Down_Cooldown){
            Inputs.push_back(new GGUI::Input(0, Constants::MOUSE_MIDDLE_PRESSED));
        }
        else if (!KEYBOARD_STATES[BUTTON_STATES::MOUSE_MIDDLE].State && PREVIOUS_KEYBOARD_STATES[BUTTON_STATES::MOUSE_MIDDLE].State != KEYBOARD_STATES[BUTTON_STATES::MOUSE_MIDDLE].State){
            Inputs.push_back(new GGUI::Input(0, Constants::MOUSE_MIDDLE_CLICKED));
        }   
    }

    void SCROLL_API(){
        if (KEYBOARD_STATES[BUTTON_STATES::MOUSE_SCROLL_UP].State){

            if (Focused_On)
                Focused_On->Scroll_Up();
        }
        else if (KEYBOARD_STATES[BUTTON_STATES::MOUSE_SCROLL_DOWN].State){

            if (Focused_On)
                Focused_On->Scroll_Down();
        }
    }

    void Handle_Escape(){
        if (!KEYBOARD_STATES[BUTTON_STATES::ESC].State)
            return;

        if (Focused_On){
            Hovered_On = Focused_On;
            Un_Focus_Element();
        }
        else if (Hovered_On){
            Un_Hover_Element();
        }
    }

    // Will select the new tabbed element as Selected and NOT Hovered!
    void Handle_Tabulator(){
        if (!KEYBOARD_STATES[BUTTON_STATES::TAB].State)
            return;

        bool Shift_Is_Pressed = KEYBOARD_STATES[BUTTON_STATES::SHIFT].State;

        // Get the current element from the selected element 
        Element* Current = Focused_On;

        if (!Current)
            Current = Hovered_On;

        int Current_Index = 0;

        // If there has not been anything selected then then skip this phase and default to zero.
        if (Current)
            for (;Current_Index < Event_Handlers.size(); Current_Index++){
                if (Event_Handlers[Current_Index]->Host == Current)
                    break;
            }

        // Generalize index hopping, if shift is pressed then go backwards.
        Current_Index += 1 + (-2 * Shift_Is_Pressed);

        if (Current_Index < 0){
            Current_Index = Event_Handlers.size() - 1;
        }
        else if (Current_Index >= Event_Handlers.size()){
            Current_Index = 0;
        }

        // now incorporate the index into its corresponding element
        Un_Hover_Element();
        Update_Focused_Element(Event_Handlers[Current_Index]->Host);
    }

    bool Has_Bit_At(char val, int i){
        return ((val) & (1<<(i)));
    }

    //Returns 1 if it is not a unocode character.
    int Get_Unicode_Length(char first_char){
        //0xxxxxxx
        if (!Has_Bit_At(first_char, 7))
            return 1;
        //110xxxxx
        if (Has_Bit_At(first_char, 7) && Has_Bit_At(first_char, 6) && !Has_Bit_At(first_char, 5))
            return 2;
        //1110xxxx
        if (Has_Bit_At(first_char, 7) && Has_Bit_At(first_char, 6) && Has_Bit_At(first_char, 5) && !Has_Bit_At(first_char, 4))
            return 3;
        //11110xxx
        if (Has_Bit_At(first_char, 7) && Has_Bit_At(first_char, 6) && Has_Bit_At(first_char, 5) && Has_Bit_At(first_char, 4) && !Has_Bit_At(first_char, 3))
            return 4;
        
        return 1;
    }

    int Get_Max_Width(){
        if (Max_Width == 0 && Max_Height == 0){
            Update_Max_Width_And_Height();
        }
        
        return Max_Width;
    }

    int Get_Max_Height(){
        if (Max_Width == 0 && Max_Height == 0){
            Update_Max_Width_And_Height();
        }

        return Max_Height;
    }

    //Returns a char if given ASCII, or a short if given UNICODE
    GGUI::UTF* Get(GGUI::Coordinates Abselute_Position){
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

    GGUI::Super_String Liquify_UTF_Text(std::vector<GGUI::UTF> Text, int Width, int Height){
        Super_String Result(Width * Height * Constants::Maximum_Needed_Pre_Allocation_For_Encoded_Super_String + SETTINGS::Word_Wrapping * (Height - 1));

        Super_String tmp_container(Constants::Maximum_Needed_Pre_Allocation_For_Encoded_Super_String);  // We can expect the maximum size each can omit.
        Super_String Text_Overhead(Constants::Maximum_Needed_Pre_Allocation_For_Over_Head);
        Super_String Background_Overhead(Constants::Maximum_Needed_Pre_Allocation_For_Over_Head);
        Super_String Text_Colour(Constants::Maximum_Needed_Pre_Allocation_For_Color);
        Super_String Background_Colour(Constants::Maximum_Needed_Pre_Allocation_For_Color);

        for (int y = 0; y < Height; y++){
            for (int x = 0; x < Width; x++){
                Text[y * Width + x].To_Encoded_Super_String(&tmp_container, &Text_Overhead, &Background_Overhead, &Text_Colour, &Background_Colour);
                
                Result.Add(tmp_container, true);

                // now instead of emptying the Super_String.vector, we can reset the current index into 0 again.
                tmp_container.Clear();
                Text_Overhead.Clear();
                Background_Overhead.Clear();   
                Text_Colour.Clear();
                Background_Colour.Clear();
            }

            // the system doesn't have word wrapping enabled then, use newlines as replacement.
            if (!SETTINGS::Word_Wrapping){
                Result.Add("\n");   // the system is word wrapped.
            }
        }

        return Result;
    }

    void Update_Frame(){
        if (Pause_Render)
            return;

        bool Previous_Event_Thread = Pause_Event_Thread;
        bool Previous_Render = Pause_Render;

        Pause_GGUI();

        if (Main){
            Abstract_Frame_Buffer = Main->Render();

            // ENCODE for optimize
            Encode_Buffer(Abstract_Frame_Buffer);

            Frame_Buffer = Liquify_UTF_Text(Abstract_Frame_Buffer, Main->Get_Width(), Main->Get_Height()).To_String();
        }
        else{
            // Use OUTBOX rendering method.
            // Abstract_Frame_Buffer = Outbox.Render();

            // // ENCODE for optimize
            // Encode_Buffer(Abstract_Frame_Buffer);

            // Frame_Buffer = Liquify_UTF_Text(Abstract_Frame_Buffer, Outbox.Get_Width(), Outbox.Get_Height());
        }

        Render_Frame();
        //Unlock the event handler.
        Pause_Event_Thread = Previous_Event_Thread;
        Pause_Render = Previous_Render;

        if (!Pause_Event_Thread)
            Atomic_Condition.notify_one();
    }

    void Pause_GGUI(){
        Pause_Render = true;
        Pause_Event_Thread = true;
    }

    void Resume_GGUI(){
        // Dont give any chance of restarting the event thread, first un-pause Main thread and update, then continue.
        Pause_Render = false;

        Update_Frame();

        // Main thread is now complete, can continue to unlocking the secondary threads.
        Pause_Event_Thread = false;

        Atomic_Condition.notify_one();
    }

    void Clear_Inputs(){
        std::chrono::system_clock::time_point now = std::chrono::system_clock::now();

        unsigned long long Delta = std::chrono::duration_cast<std::chrono::milliseconds>(now - Last_Input_Clear_Time).count();

        if (Delta > SETTINGS::Input_Clear_Time){
            Inputs.clear();
            
            Last_Input_Clear_Time = now;
        }
    }

    void Recall_Memories(){
        std::chrono::high_resolution_clock::time_point Current_Time = std::chrono::high_resolution_clock::now();

        // Prolong prolongable memories.
        for (unsigned int i = 0; i < Remember.size(); i++){
            for (unsigned int j = 1; j < Remember.size(); j++){
                if (Remember[i].Is(MEMORY_FLAGS::PROLONG_MEMORY) && Remember[j].Is(MEMORY_FLAGS::PROLONG_MEMORY) && i != j)
                    if (Remember[i].Job.target<bool(*)(GGUI::Event* e)>() == Remember[j].Job.target<bool(*)(GGUI::Event* e)>()){
                        Remember[i].Start_Time = Remember[j].Start_Time;

                        Remember.erase(Remember.begin() + j--);
                        break;
                    }
            }
        }

        for (unsigned int i = 0; i < Remember.size(); i++){
            //first calculate the time difference between the start if the task and the end task
            size_t Time_Difference = std::chrono::duration_cast<std::chrono::milliseconds>(Current_Time - Remember[i].Start_Time).count();

            //if the time difference is greater than the time limit, then delete the memory
            if (Time_Difference > Remember[i].End_Time){
                //Pause_Renderer();

                try{
                    bool Success = Remember[i].Job((Event*)&Remember[i]);

                    // If job is a re-trigger it will ignore whether the job was successful or not.
                    if (Remember[i].Is(MEMORY_FLAGS::RETRIGGER)){

                        // May need to change this into more accurate version of time capturing.
                        Remember[i].Start_Time = Current_Time;

                    }
                    else if (Success){
                        Remember.erase(Remember.begin() + i);

                        i--;
                    }
                }
                catch (std::exception& e){
                    Report("In memory: '" + Remember[i].ID + "' Problem: " + std::string(e.what()));
                }
                
                //Resume_Renderer();
            }

        }
    }

    bool Is(unsigned long long f, unsigned long long Flag){
        return (f & Flag) == Flag;
    }

    bool Has(unsigned long long f, unsigned long long flag){
        return (f & flag) != 0;
    }

    bool Contains(unsigned long long big, unsigned long long smoll){
        return (smoll & big) == smoll;
    }

    void Un_Focus_Element(){
        if (!Focused_On)
            return;
        Focused_On->Set_Focus(false);
        Focused_On = nullptr;
    }

    void Un_Hover_Element(){
        if (!Hovered_On)
            return;
        Hovered_On->Set_Hover_State(false);
        Hovered_On = nullptr;
    }

    void Update_Focused_Element(GGUI::Element* new_candidate){
        if (Focused_On == new_candidate || new_candidate == Main)
            return;

        Pause_GGUI([new_candidate](){
            //put the previus focused candidate into not-focus
            if (Focused_On){
                Un_Focus_Element();
            }

            //switch the candidate
            Focused_On = new_candidate;
            
            //set the new candidate to focused.
            Focused_On->Set_Focus(true);
        });
    }

    void Update_Hovered_Element(GGUI::Element* new_candidate){
        if (Hovered_On == new_candidate || new_candidate == Main)
            return;

        Pause_GGUI([new_candidate](){
            //put the previus focused candidate into not-focus
            if (Hovered_On){
                Un_Hover_Element();
            }

            //switch the candidate
            Hovered_On = new_candidate;
            
            //set the new candidate to focused.
            Hovered_On->Set_Hover_State(true);
        });
    }

    // Events
    void Event_Handler(){
        // Disable hovered element if the mouse isn't on top of it anymore.
        if (Hovered_On && !Collides(Hovered_On, GGUI::Mouse)){
            Un_Hover_Element();
        }

        // Since some key events are piped to us at a different speed than others, we need to keep the older (un-used) inputs "alive" until their turn arrives.
        Populate_Inputs_For_Held_Down_Keys();


        for (auto& e : Event_Handlers){

            bool Has_Select_Event = false;

            for (unsigned int i = 0; i < Inputs.size(); i++){
                if (Has(Inputs[i]->Criteria, Constants::MOUSE_LEFT_CLICKED | Constants::ENTER))
                    Has_Select_Event = true;

                // Criteria must be identical for more accurate criteria listing.
                if (e->Criteria == Inputs[i]->Criteria){
                    try{
                        // Check if this job could be run successfully.
                        if (e->Job(Inputs[i])){
                            //dont let anyone else react to this event.
                            Inputs.erase(Inputs.begin() + i);
                        }
                        else{
                            // TODO: report miscarried event job.
                            Report_Stack("Job '" + e->ID + "' failed!");
                        }
                    }
                    catch(std::exception& problem){
                        Report("In event: '" + e->ID + "' Problem: " + std::string(problem.what()));
                    }
                }
            }

            // Hosted branches
            if (e->Host){
                if (!e->Host->Is_Displayed())
                    continue;

                //update the focused
                if (Collides(e->Host, GGUI::Mouse)){
                    if (Has_Select_Event){
                        Update_Focused_Element(e->Host);
                        Un_Hover_Element();
                    }
                    else{
                        Update_Hovered_Element(e->Host);
                    }
                }
            }
            // Unhosted branches
            else{

                // some code...

            }

            if (Inputs.size() <= 1)
                continue;

            // TODO: Do better you dum!
            // GO through the inputs and check if they contain all the flags required
            unsigned long long Remaining_Flags = e->Criteria;
            std::vector<GGUI::Input *> Accepted_Inputs;

            // if an input has flags that meet the criteria, then remove the criteria from the remaining flags and continue until the remaining flags are equal to zero.
            for (auto& i : Inputs){

                if (Contains(Remaining_Flags, i->Criteria)){
                    Remaining_Flags &= ~i->Criteria;
                    Accepted_Inputs.push_back(i);
                }

                if (Remaining_Flags == 0)
                    break;
            }

            if (Remaining_Flags == 0){
                // Now we need to find the information to send to the event handler.
                Input* Best_Candidate = Accepted_Inputs[0];

                for (auto i : Accepted_Inputs){
                    if (i->Data > Best_Candidate->Data){
                        Best_Candidate = i;
                    }
                }

                //check if this job could be runned succesfully.
                if (e->Job(Best_Candidate)){
                    // Now remove the candidates from the input
                    for (unsigned int i = 0; i < Inputs.size(); i++){
                        if (Inputs[i] == Best_Candidate){
                            Inputs.erase(Inputs.begin() + i);
                            i--;
                        }
                    }
                }
            }

        }
        //Clear_Inputs();
        Inputs.clear();
    }

    int Get_Free_Class_ID(std::string n){
        if (Class_Names.find(n) != Class_Names.end()){
            return Class_Names[n];
        }
        else{
            Class_Names[n] = Class_Names.size();

            return Class_Names[n];
        }
    }

    void Add_Class(std::string name, Styling Styling){
        int Class_ID = Get_Free_Class_ID(name);

        Classes[Class_ID] = Styling;
    }

    void Init_Classes(){
        // Add default class
        std::string DEFAULT_NAME = "default";
        Styling DEFAULT;
        DEFAULT.Text_Color = RGB_VALUE(COLOR::WHITE, VALUE_STATE::INITIALIZED);
        DEFAULT.Background_Color = RGB_VALUE(COLOR::BLACK, VALUE_STATE::INITIALIZED);

        DEFAULT.Border_Color = RGB_VALUE(COLOR::WHITE, VALUE_STATE::INITIALIZED);
        DEFAULT.Border_Background_Color = RGB_VALUE(COLOR::BLACK, VALUE_STATE::INITIALIZED);

        DEFAULT.Hover_Text_Color = RGB_VALUE(COLOR::WHITE, VALUE_STATE::INITIALIZED);
        DEFAULT.Hover_Background_Color = RGB_VALUE(COLOR::DARK_GRAY, VALUE_STATE::INITIALIZED);

        DEFAULT.Hover_Border_Color = RGB_VALUE(COLOR::WHITE, VALUE_STATE::INITIALIZED);
        DEFAULT.Hover_Border_Background_Color = RGB_VALUE(COLOR::BLACK, VALUE_STATE::INITIALIZED);
         
        DEFAULT.Focus_Text_Color = RGB_VALUE(COLOR::BLACK, VALUE_STATE::INITIALIZED);
        DEFAULT.Focus_Background_Color = RGB_VALUE(COLOR::WHITE, VALUE_STATE::INITIALIZED);

        DEFAULT.Focus_Border_Color = RGB_VALUE(COLOR::WHITE, VALUE_STATE::INITIALIZED);
        DEFAULT.Focus_Border_Background_Color = RGB_VALUE(COLOR::BLACK, VALUE_STATE::INITIALIZED);
        
        Add_Class(DEFAULT_NAME, DEFAULT);
    }

    void Go_Through_File_Streams(){
        for (auto& File_Handle : File_Streamer_Handles){
            if (!File_Handle.second->Is_Cout_Stream())  // Cout handlers get called whenever a new line is inserted.
                File_Handle.second->Changed();
        }
    }

    void Refresh_Multi_Frame_Canvas(){
        // Go through all animation needing elements (currently only for multi-frame canvases)
        for (auto i : Multi_Frame_Canvas){

            i.first->Flush(true);

        }

        if (Multi_Frame_Canvas.size() > 0){
            // Since the multi frame canvases need to be smooth we recon to go with 60fps realtime
            // First calculate the possible length of which our load can represent on.
            float Length_Of_Possible_Values = MAX_UPDATE_SPEED - MIN_UPDATE_SPEED;

            // Get the offset of which the wanted target fps deviates from the minimum value
            float Offset_Of_Our_Load = 16 - MIN_UPDATE_SPEED;

            // calculate the simple probability.
            Event_Thread_Load = 1 - Offset_Of_Our_Load / Length_Of_Possible_Values;
        }
    }

    //Inits GGUI and returns the main window.
    GGUI::Window* Init_Renderer(){
        Update_Max_Width_And_Height();
        
        if (Max_Height == 0 || Max_Width == 0){
            Report("Width/Height is zero!");
            return nullptr;
        }

        //Save the state before the init
        bool Default_Render_State = Pause_Render;
        bool Default_Event_Thread_State = Pause_Event_Thread;

        Current_Time = std::chrono::high_resolution_clock::now();
        Previous_Time = Current_Time;

        //pause the renderer
        Pause_GGUI();

        Init_Platform_Stuff();
        Init_Classes();

        // Set the Main to be anything but nullptr, since its own constructor will try anchor it otherwise.
        Main = (Window*)0xFFFFFF;
        Main = new Window("", Max_Width, Max_Height);

        if (!Pause_Event_Thread){
            Abstract_Frame_Buffer = Main->Render();

            Encode_Buffer(Abstract_Frame_Buffer);

            Frame_Buffer = Liquify_UTF_Text(Abstract_Frame_Buffer, Main->Get_Width(), Main->Get_Height()).To_String();
        }

        Init_Inspect_Tool();

        std::thread Passive_Scheduler([&](){
            while (true){
                std::unique_lock Current_Lock(Atomic_Mutex);
                Atomic_Condition.wait(Current_Lock, [](){ return !Pause_Event_Thread; });

                // Reset the thread load counter
                Event_Thread_Load = 0;
                Previous_Time = Current_Time;

                Pause_GGUI([](){
                    // First update Main size if needed.
                    Update_Max_Width_And_Height();

                    // Order independent --------------
                    Recall_Memories();
                    Go_Through_File_Streams();
                    Refresh_Multi_Frame_Canvas();
                    // --------------
                });

                Current_Time = std::chrono::high_resolution_clock::now();

                Atomic_Condition.notify_one();

                // Calculate the delta time.
                Delta_Time = std::chrono::duration_cast<std::chrono::milliseconds>(Current_Time - Previous_Time).count();

                CURRENT_UPDATE_SPEED = MIN_UPDATE_SPEED + (MAX_UPDATE_SPEED - MIN_UPDATE_SPEED) * (1 - Event_Thread_Load);

                std::this_thread::sleep_for(std::chrono::milliseconds(
                    Max(
                        CURRENT_UPDATE_SPEED - Delta_Time, 
                        MIN_UPDATE_SPEED
                    )
                ));
            }
        });

        std::thread Inquire_Scheduler([&](){
            while (true){
                std::unique_lock Current_Lock(Atomic_Mutex);
                Atomic_Condition.wait(Current_Lock, [](){ return !Pause_Event_Thread; });

                // First await for the user input outside the pause_renderer, so that awaiting for the user wont affect other processes.
                Query_Inputs();

                // Now if needed we can start reacting to the user input if given.
                Pause_GGUI([](){
                    // Since the user input queries are expected to stop and await for the user input, we dont need sleep functions here.
                    Event_Handler();
                });

                Atomic_Condition.notify_one();
            }
        });

        Passive_Scheduler.detach();
        Inquire_Scheduler.detach();

        Pause_Render = Default_Render_State;
        Pause_Event_Thread = Default_Event_Thread_State;

        if (!Pause_Event_Thread)
            Atomic_Condition.notify_one();

        return Main;
    }

    // NOTE: This is only a temporary function to be replaced when Date_Element is made.
    std::string Now(){
        //This function takes the current time and returns a string of the time.
        // Format: DD.MM.YYYY: SS.MM.HH
        std::time_t now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());

        std::string Result = std::ctime(&now);

        return Result.substr(0, Result.size() - 1);
    }

    void Report(std::string Problem){
        Pause_GGUI();

        Problem = " " + Problem + " ";

        // Error logger structure:
        /*
            <Window name="_ERROR_LOGGER_">
                <List name="_HISTORY_" type=vertical scrollable=true>
                    <List type="horizontal">
                        <TextField>Time</TextField>
                        <TextField>Problem a</TextField>
                        <TextField>[repetitions if any]</TextField>
                    </List>
                    ...
                </List>
            </Window>
        */

        if (Main && (Max_Width != 0 && Max_Height != 0)){
            bool Create_New_Line = true;

            // First check if there already is a report log.
            Window* Error_Logger = (Window*)Main->Get_Element(ERROR_LOGGER);

            if (Error_Logger){
                // Get the list
                Scroll_View* History = (Scroll_View*)Error_Logger->Get_Element(HISTORY);

                // This happens, when Error logger is kidnapped!
                if (!History){
                    // Now create the history lister
                    History = new Scroll_View(
                        Error_Logger->Get_Width() - 1,
                        Error_Logger->Get_Height() - 1,
                        GGUI::COLOR::RED,
                        GGUI::COLOR::BLACK
                    );
                    History->Set_Growth_Direction(DIRECTION::COLUMN);
                    History->Set_Name(HISTORY);

                    Error_Logger->Add_Child(History);
                }

                std::vector<List_View*>& Rows = (std::vector<List_View*>&)History->Get_Container()->Get_Childs(); 

                if (Rows.size() > 0){
                    //Text_Field* Previous_Date = Rows.back()->Get<Text_Field>(0);
                    Text_Field* Previous_Problem = Rows.back()->Get<Text_Field>(1);
                    Text_Field* Previous_Repetitions = Rows.back()->Get<Text_Field>(2);

                    //check if the previous problem was same problem
                    if (Previous_Problem->Get_Text() == Problem){
                        // increase the repetition count by one
                        if (!Previous_Repetitions){
                            Previous_Repetitions = new Text_Field("2");
                            Rows.back()->Add_Child(Previous_Repetitions);
                        }
                        else{
                            // translate the string to int
                            int Repetition = std::stoi(Previous_Repetitions->Get_Text()) + 1;
                            Previous_Repetitions->Set_Text(std::to_string(Repetition));
                        }

                        // We dont need to create a new line.
                        Create_New_Line = false;
                    }
                }
            }
            else{
                // create the error logger
                Error_Logger = new Window(
                    "LOG",
                    Main->Get_Width() / 4,
                    Main->Get_Height() / 2,
                    GGUI::COLOR::RED,
                    GGUI::COLOR::BLACK,
                    GGUI::COLOR::RED,
                    GGUI::COLOR::BLACK
                );
                Error_Logger->Set_Name(ERROR_LOGGER);
                Error_Logger->Set_Position({
                    (Max_Width - Error_Logger->Get_Width()) / 2,
                    (Max_Height - Error_Logger->Get_Height()) / 2,
                    INT32_MAX
                });
                Error_Logger->Show_Border(true);
                Error_Logger->Allow_Overflow(true);

                // Now create the history lister
                Scroll_View* History = new Scroll_View(
                    Error_Logger->Get_Width() - 1,
                    Error_Logger->Get_Height() - 1,
                    GGUI::COLOR::RED,
                    GGUI::COLOR::BLACK
                );
                History->Set_Growth_Direction(DIRECTION::COLUMN);
                History->Set_Name(HISTORY);

                Error_Logger->Add_Child(History);
                Main->Add_Child(Error_Logger);
            }

            if (Create_New_Line){
                // re-find the error_logger.
                Error_Logger = (Window*)Main->Get_Element(ERROR_LOGGER);
                Scroll_View* History = (Scroll_View*)Error_Logger->Get_Element(HISTORY);

                List_View* Row = new List_View(
                    History->Get_Width(),
                    1,
                    GGUI::COLOR::RED,
                    GGUI::COLOR::BLACK
                );
                Row->Set_Parent(History);
                Row->Set_Flow_Direction(DIRECTION::ROW);

                // TODO: replace the text_field into Date_Element !
                Text_Field* Date = new Text_Field(Now());
                Text_Field* Problem_Text = new Text_Field(Problem);

                Row->Add_Child(Date);
                Row->Add_Child(Problem_Text);

                History->Add_Child(Row);

                // Calculate the new x position for the Error_Logger
                if (Error_Logger->Get_Parent() == Main)
                    Error_Logger->Set_Position({
                        (Error_Logger->Get_Parent()->Get_Width() - History->Get_Width()) / 2,
                        (Error_Logger->Get_Parent()->Get_Height() - History->Get_Height()) / 2,
                        INT32_MAX
                    });

                // check if the Current rows amount makes the list new rows un-visible because of the of-limits.
                // We can assume that the singular error is at least one tall.
                if (GGUI::Min(History->Get_Container()->Get_Height(), (int)History->Get_Container()->Get_Childs().size()) >= Error_Logger->Get_Height()){
                    // Since the children are added asynchronously, we can assume the the order of childs list vector represents the actual visual childs.
                    // Element* First_Child = History->Get_Childs()[0];
                    // History->Remove(First_Child);

                    // TODO: Make this into a scroll action and not a remove action, since we want to see the previous errors :)
                    History->Scroll_Down();
                
                }
            }

            if (Error_Logger->Get_Parent() == Main){
                Error_Logger->Display(true);

                Remember.push_back(Memory(
                    TIME::SECOND * 30,
                    [=]([[maybe_unused]] GGUI::Event* e){
                        //delete tmp;
                        Error_Logger->Display(false);
                        //job successfully done
                        return true;
                    },
                    MEMORY_FLAGS::PROLONG_MEMORY,
                    "Report Logger Clearer"
                ));
            }

        }
        else{
            if (!Platform_Initialized){
                Init_Platform_Stuff();
            }

            // This is for the non GGUI space errors.
            UTF _error__tmp_ = UTF("ERROR: ", {COLOR::RED, {}});

            std::cout << _error__tmp_.To_String() + Problem << std::endl;
        }

        Resume_GGUI();
    }

    void Nest_UTF_Text(GGUI::Element* Parent, GGUI::Element* child, std::vector<GGUI::UTF> Text, std::vector<GGUI::UTF>& Parent_Buffer){
        if (Parent == child){
            std::string R = 
                std::string("Cannot nest element to it self\n") +
                std::string("Element name: ") + Parent->Get_Name();

            if (Parent->Get_Parent()){
                R += std::string("\n") + 
                std::string("Inside of: ") + Parent->Get_Parent()->Get_Name();
            }

            Report(
                R
            );
        }

        GGUI::Coordinates C = child->Get_Position();

        int i = 0;
        for (int Parent_Y = 0; Parent_Y < (signed)Parent->Get_Height(); Parent_Y++){
            for (int Parent_X = 0; Parent_X < (signed)Parent->Get_Width(); Parent_X++){
                if (
                    Parent_Y >= C.Y && Parent_X >= C.X &&
                    Parent_Y <= C.Y + (signed)child->Get_Height() &&
                    Parent_X <= C.X + (signed)child->Get_Width()
                )
                {
                    Parent_Buffer[Parent_Y * (signed)Parent->Get_Width() + Parent_X] = Text[i++];
                }
            }
        }
    }

    void Pause_GGUI(std::function<void()> f){
        bool Original_Value = Pause_Render;

        Pause_GGUI();

        f();

        // If another thread has been pausing this at the same time, then we need to check for it if it has already unpaused it.
        if (!Pause_Render && !Original_Value){
            Pause_Render = false;
            Original_Value = false;
        }

        if (!Original_Value)
            Resume_GGUI(); 
    }

    // Use this to use GGUI.
    void GGUI(std::function<void()> DOM, unsigned long long Sleep_For){
        Pause_GGUI([=](){

            Init_Renderer();

            DOM();
        });

        SLEEP(Sleep_For);
        // No need of un-initialization here or forced exit, since on process death the right exit codes will be initiated.
    }

    void Encode_Buffer(std::vector<GGUI::UTF>& Buffer){

        // There are three different encoded types: Start, Middle and End.

        Buffer[0].Set_Flag(UTF_FLAG::ENCODE_START);
        Buffer[Buffer.size() - 1].Set_Flag(UTF_FLAG::ENCODE_END);

        for (unsigned int Index = 1; Index < Buffer.size() - 1; Index++){

            bool Same_Colours_As_Previous = (RGB)Buffer[Index].Background == (RGB)Buffer[Index - 1].Background && (RGB)Buffer[Index].Foreground == (RGB)Buffer[Index - 1].Foreground;
            bool Same_Colours_As_Next = (RGB)Buffer[Index].Background == (RGB)Buffer[Index + 1].Background && (RGB)Buffer[Index].Foreground == (RGB)Buffer[Index + 1].Foreground;

            // if the current colours are same as the previous but not the next then the current is a end of a encode strip.
            if (!Same_Colours_As_Next){
                Buffer[Index].Set_Flag(UTF_FLAG::ENCODE_END);
            }

            // if the current colours are not the same as the previous but the same as the next then the current is a start of a encode strip.
            if (!Same_Colours_As_Previous){
                Buffer[Index].Set_Flag(UTF_FLAG::ENCODE_START);
            }
        }

        // Check if the second to last was also an ending node, if so then add the last node the start tag
        if (Buffer[Buffer.size() - 2].Is(UTF_FLAG::ENCODE_END)){
            Buffer[Buffer.size() - 1].Set_Flag(UTF_FLAG::ENCODE_START | UTF_FLAG::ENCODE_END);
        }
    }

    bool Update_Stats([[maybe_unused]] GGUI::Event* e){
        // Check if Inspect tool is displayed
        Element* Inspect_Tool = Main->Get_Element("Inspect");

        if (!Inspect_Tool || !Inspect_Tool->Is_Displayed())
            return false;

        // find stats
        Text_Field* Stats = (Text_Field*)Main->Get_Element("STATS");

        // Update the stats
        Stats->Set_Text(
            "Encode: " + std::to_string(Abstract_Frame_Buffer.size()) + "\n" + 
            "Decode: " + std::to_string(Frame_Buffer.size()) + "\n" +
            "Elements: " + std::to_string(Main->Get_All_Nested_Elements().size()) + "\n" +
            "FPS: " + std::to_string(GGUI::TIME::SECOND / Max(Delta_Time, 1))
        );

        // return success.
        return true;
    }

    void Init_Inspect_Tool(){
        GGUI::List_View* Inspect = new GGUI::List_View(
            Main->Get_Width() / 3,
            Main->Get_Height(),
            Main->Get_Text_Color(),
            Main->Get_Background_Color()
        );

        Inspect->Set_Flow_Direction(DIRECTION::COLUMN);
        Inspect->Show_Border(false);
        Inspect->Set_Position({
            Main->Get_Width() - (Main->Get_Width() / 3),
            0,
            INT32_MAX - 1,
        });
        Inspect->Set_Opacity(0.8f);
        Inspect->Set_Name("Inspect");

        Main->Add_Child(Inspect);
        
        // Add a count for how many UTF are being streamed.
        Text_Field* Stats = new Text_Field(
            "Encode: " + std::to_string(Abstract_Frame_Buffer.size()) + "\n" + 
            "Decode: " + std::to_string(Frame_Buffer.size()) + "\n" +
            "Elements: " + std::to_string(Main->Get_All_Nested_Elements().size()) + "\n" +
            "FPS: " + std::to_string(GGUI::TIME::SECOND / Max(Delta_Time, 1)),
            ALIGN::LEFT,
            Inspect->Get_Width(),
            4
        );
        Stats->Set_Name("STATS");

        Inspect->Add_Child(Stats);

        // Add the error logger kidnapper:
        Window* Error_Logger_Kidnapper = new Window(
            "LOG: ",
            Inspect->Get_Width(),
            Inspect->Get_Height() / 2,
            GGUI::COLOR::RED,
            GGUI::COLOR::BLACK,
            GGUI::COLOR::RED,
            GGUI::COLOR::BLACK
        );

        Error_Logger_Kidnapper->Set_Name(ERROR_LOGGER);
        Error_Logger_Kidnapper->Allow_Overflow(true);

        Inspect->Add_Child(Error_Logger_Kidnapper);
        Inspect->Display(false);

        GGUI::Main->On(Constants::SHIFT | Constants::CONTROL | Constants::KEY_PRESS, [=](GGUI::Event* e){
            GGUI::Input* input = (GGUI::Input*)e;

            if (!KEYBOARD_STATES[BUTTON_STATES::SHIFT].State && !KEYBOARD_STATES[BUTTON_STATES::CONTROL].State && input->Data != 'i' && input->Data != 'I') 
                return false;

            Inspect->Display(!Inspect->Is_Displayed());

            return true;
        }, true);
        
        Remember.push_back(
            GGUI::Memory(
                TIME::SECOND,
                Update_Stats,
                MEMORY_FLAGS::RETRIGGER,
                "Update Stats"
            )
        );
    }

    void Inform_All_Global_BUFFER_CAPTURES(INTERNAL::BUFFER_CAPTURE* informer){

        for (auto* capturer : Global_Buffer_Captures){
            if (!capturer->Is_Global)
                continue;

            // Give the capturers the latest row of captured buffer data
            if (capturer->Sync(informer)){
                // success
            }
            else{
                // fail, maybe try merge?
            }

        }

    }

}