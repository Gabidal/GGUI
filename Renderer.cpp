#include "Renderer.h"

#include <map>
#include <string>
#include <iostream>

#include <cassert>

namespace GGUI{
    std::vector<UTF> Abstract_Frame_Buffer;               //2D clean vector whitout bold nor color
    std::string Frame_Buffer;                                   //string with bold and color, this what gets drawn to console.
    std::atomic_bool Pause_Render = false;                      //if true, the render will not be updated, good for window creation.
    std::atomic_bool Pause_Event_Thread = false;                //if true, the event handler will pause.

    int Max_Width = 0;
    int Max_Height = 0;

    std::vector<Memory> Remember;

    std::vector<Action*> Event_Handlers;
    std::vector<Input*> Inputs;

    std::map<std::string, Element*> Element_Names;

    Element* Focused_On = nullptr;
    Element* Hovered_On = nullptr;

    Coordinates Mouse;
    //move 1 by 1, or element by element.
    bool Mouse_Movement_Enabled = true;

    std::map<std::string, BUTTON_STATE> KEYBOARD_STATES;
    std::map<std::string, BUTTON_STATE> PREVIOUS_KEYBOARD_STATES;

    // Time stuff
    inline time_t UPDATE_SPEED_MIILISECONDS = TIME::MILLISECOND * 16;
    inline int Inputs_Per_Second = INT16_MAX;
    inline int Inputs_Per_Query = Max(Inputs_Per_Second / (TIME::SECOND / UPDATE_SPEED_MIILISECONDS), (time_t)1);

    std::chrono::system_clock::time_point Previous_Time;
    std::chrono::system_clock::time_point Current_Time;
    unsigned long long Delta_Time;

    inline std::map<int, std::map<std::string, VALUE*>> Classes;

    inline std::map<std::string, int> Class_Names;

    Window* Main = nullptr;                                          //Main window

    bool Collides(GGUI::Element* a, GGUI::Element* b){
        if (a == b)
            return false;
            
        int A_X = a->Get_Position().X;
        int A_Y = a->Get_Position().Y;

        int B_X = b->Get_Position().X;
        int B_Y = b->Get_Position().Y;

        return (A_X < B_X + b->Get_Width() && A_X + a->Get_Width() > B_X && A_Y < B_Y + b->Get_Height() && A_Y + a->Get_Height() > B_Y);
    }

    bool Collides(GGUI::Element* a, GGUI::Coordinates b){
        int A_X = a->Get_Absolute_Position().X;
        int A_Y = a->Get_Absolute_Position().Y;

        int B_X = b.X;
        int B_Y = b.Y;
        return (A_X < B_X + 1 && A_X + a->Get_Width() > B_X && A_Y < B_Y + 1 && A_Y + a->Get_Height() > B_Y);
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

    signed long long Min(signed long long a, signed long long b){
        return a < b ? a : b;
    }

    signed long long Max(signed long long a, signed long long b){
        return a > b ? a : b;
    }

    #if _WIN32
    #include <windows.h>

    void SLEEP(unsigned int mm){
        _sleep(mm);
    }

    GGUI::HANDLE GLOBAL_STD_HANDLE;

    void Render_Frame(){
        unsigned long long tmp = 0;
        SetConsoleCursorPosition(GLOBAL_STD_HANDLE, {0, 0});
        WriteFile(GLOBAL_STD_HANDLE, Frame_Buffer.data(), Frame_Buffer.size(), reinterpret_cast<LPDWORD>(&tmp), NULL);
    }

    void Update_Max_Width_And_Height(){
        CONSOLE_SCREEN_BUFFER_INFO info;

        GetConsoleScreenBufferInfo(GLOBAL_STD_HANDLE, &info);

        Max_Width = info.srWindow.Right - info.srWindow.Left;
        Max_Height = info.srWindow.Bottom - info.srWindow.Top;

        assert(("Terminal Size non-existant!", Max_Width > 0 && Max_Height > 0));
    }

    void Update_Frame(bool Lock_Event_Thread);
    //Is called on every cycle.

    void Query_Inputs(){
        std::vector<INPUT_RECORD> Input;

        int Buffer_Size = 0;

        if (GetNumberOfConsoleInputEvents(GetStdHandle(STD_INPUT_HANDLE), (LPDWORD)&Buffer_Size) && Buffer_Size > 0){

            Input.resize(Buffer_Size);

            ReadConsoleInput(GetStdHandle(STD_INPUT_HANDLE), Input.data(), Buffer_Size, (LPDWORD)&Buffer_Size);
        }

        for (int i = 0; i < Buffer_Size; i++){
            if (Input[i].EventType == KEY_EVENT){
                if (Input[i].Event.KeyEvent.bKeyDown){
                    if (Input[i].Event.KeyEvent.wVirtualKeyCode == VK_UP){
                        Inputs.push_back(new GGUI::Input(0, Constants::UP));
                        KEYBOARD_STATES[BUTTON_STATES::UP] = BUTTON_STATE(true);
                    }
                    else if (Input[i].Event.KeyEvent.wVirtualKeyCode == VK_DOWN){
                        Inputs.push_back(new GGUI::Input(0, Constants::DOWN));
                        KEYBOARD_STATES[BUTTON_STATES::DOWN] = BUTTON_STATE(true);
                    }
                    else if (Input[i].Event.KeyEvent.wVirtualKeyCode == VK_LEFT){
                        Inputs.push_back(new GGUI::Input(0, Constants::LEFT));
                        KEYBOARD_STATES[BUTTON_STATES::LEFT] = BUTTON_STATE(true);
                    }
                    else if (Input[i].Event.KeyEvent.wVirtualKeyCode == VK_RIGHT){
                        Inputs.push_back(new GGUI::Input(0, Constants::RIGHT));
                        KEYBOARD_STATES[BUTTON_STATES::RIGHT] = BUTTON_STATE(true);
                    }
                    else if (Input[i].Event.KeyEvent.wVirtualKeyCode == VK_RETURN){
                        Inputs.push_back(new GGUI::Input('\n', Constants::ENTER));
                        KEYBOARD_STATES[BUTTON_STATES::ENTER] = BUTTON_STATE(true);
                    }
                    else if (Input[i].Event.KeyEvent.wVirtualKeyCode == VK_SHIFT){
                        Inputs.push_back(new GGUI::Input(' ', Constants::SHIFT));
                        KEYBOARD_STATES[BUTTON_STATES::SHIFT] = BUTTON_STATE(true);
                    }
                    else if (Input[i].Event.KeyEvent.wVirtualKeyCode == VK_BACK){
                        Inputs.push_back(new GGUI::Input(' ', Constants::BACKSPACE));
                        KEYBOARD_STATES[BUTTON_STATES::BACKSPACE] = BUTTON_STATE(true);
                    }
                    else if (Input[i].Event.KeyEvent.wVirtualKeyCode == VK_ESCAPE){
                        Inputs.push_back(new GGUI::Input(' ', Constants::ESCAPE));
                        KEYBOARD_STATES[BUTTON_STATES::ESC] = BUTTON_STATE(true);
                        Handle_Escape();
                    }
                    else if (Input[i].Event.KeyEvent.wVirtualKeyCode == VK_TAB){
                        Inputs.push_back(new GGUI::Input(' ', Constants::TAB));
                        KEYBOARD_STATES[BUTTON_STATES::TAB] = BUTTON_STATE(true);
                        Handle_Tabulator();
                    }
                    else if (Input[i].Event.KeyEvent.uChar.AsciiChar != 0){
                        Inputs.push_back(new GGUI::Input(Input[i].Event.KeyEvent.uChar.AsciiChar, Constants::KEY_PRESS));
                    }
                }
            }
            else if (Input[i].EventType == WINDOW_BUFFER_SIZE_EVENT){

                Update_Max_Width_And_Height();

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
                if ((Input[i].Event.MouseEvent.dwButtonState & FROM_LEFT_1ST_BUTTON_PRESSED) != 0) {
                    PREVIOUS_KEYBOARD_STATES[BUTTON_STATES::MOUSE_LEFT].State = KEYBOARD_STATES[BUTTON_STATES::MOUSE_LEFT].State;
                    KEYBOARD_STATES[BUTTON_STATES::MOUSE_LEFT].State = true;
                    KEYBOARD_STATES[BUTTON_STATES::MOUSE_LEFT].Capture_Time = std::chrono::high_resolution_clock::now();
                }
                if ((Input[i].Event.MouseEvent.dwButtonState & FROM_LEFT_1ST_BUTTON_PRESSED) == 0) {
                    PREVIOUS_KEYBOARD_STATES[BUTTON_STATES::MOUSE_LEFT].State = KEYBOARD_STATES[BUTTON_STATES::MOUSE_LEFT].State;
                    KEYBOARD_STATES[BUTTON_STATES::MOUSE_LEFT].State = false;
                }

                if ((Input[i].Event.MouseEvent.dwButtonState & RIGHTMOST_BUTTON_PRESSED) != 0) {
                    PREVIOUS_KEYBOARD_STATES[BUTTON_STATES::MOUSE_RIGHT].State = KEYBOARD_STATES[BUTTON_STATES::MOUSE_RIGHT].State;
                    KEYBOARD_STATES[BUTTON_STATES::MOUSE_RIGHT].State = true;
                    KEYBOARD_STATES[BUTTON_STATES::MOUSE_RIGHT].Capture_Time = std::chrono::high_resolution_clock::now();
                }
                if ((Input[i].Event.MouseEvent.dwButtonState & RIGHTMOST_BUTTON_PRESSED) == 0) {
                    PREVIOUS_KEYBOARD_STATES[BUTTON_STATES::MOUSE_RIGHT].State = KEYBOARD_STATES[BUTTON_STATES::MOUSE_RIGHT].State;
                    KEYBOARD_STATES[BUTTON_STATES::MOUSE_RIGHT].State = false;
                }
            }
        }

        
        MOUSE_API();
    }

    void Init_Platform_Stuff(){
        GLOBAL_STD_HANDLE = GetStdHandle(STD_OUTPUT_HANDLE);
        SetConsoleMode(GLOBAL_STD_HANDLE, -1);
        SetConsoleMode(GetStdHandle(STD_INPUT_HANDLE), ENABLE_EXTENDED_FLAGS | ENABLE_MOUSE_INPUT | ENABLE_WINDOW_INPUT);

        std::cout << "\033[?1003h";
        std::cout.flush();

        SetConsoleOutputCP(65001);
    }

    void Exit(){

    }

    #else
    #include <unistd.h>
    #include <termios.h>
    #include <sys/ioctl.h>
    #include <unistd.h>
    #include <fcntl.h>
    #include <signal.h>

    int Previus_Flags = 0;
    struct termios Previus_Raw;
    void Exit(int signum){
        if (signum == SIGINT){
            printf("\e[?1003l\e[?25h");

            fcntl(STDIN_FILENO, F_SETFL, Previus_Flags); // set non-blocking flag
            tcsetattr(STDIN_FILENO, TCSAFLUSH, &Previus_Raw);

            exit(0);
        }
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
        printf("\033[H");
        // Write the frame buffer to the console
        fwrite(Frame_Buffer.data(), 1, Frame_Buffer.size(), stdout);
        // Flush the output to ensure it's written immediately
        fflush(stdout);
    }

    void Update_Max_Width_And_Height(){
        struct winsize w;
        ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);

        Max_Width = w.ws_col;
        Max_Height = w.ws_row - 1;
    }

    //Is called on every cycle.
    void Query_Inputs(){
        return;

        // For keyboard input handling.
        char buffer[256];
        int bytes_read = read(STDIN_FILENO, buffer, sizeof(buffer));
        
        for (int i = 0; i < bytes_read; i++) {
            char c = buffer[i];
            if (c == '\x1B') { // handle arrow keys
                i++;
                if (i < bytes_read && buffer[i] == '[') {
                    i++;
                    if (i < bytes_read && buffer[i] == 'A') {
                        Inputs.push_back(new GGUI::Input(0, Constants::UP));
                    }
                    else if (i < bytes_read && buffer[i] == 'B') {
                        Inputs.push_back(new GGUI::Input(0, Constants::DOWN));
                    }
                    else if (i < bytes_read && buffer[i] == 'C') {
                        Inputs.push_back(new GGUI::Input(0, Constants::RIGHT));
                    }
                    else if (i < bytes_read && buffer[i] == 'D') {
                        Inputs.push_back(new GGUI::Input(0, Constants::LEFT));
                    }
                }
            }
            else if (c == '\033') {
                i++;
                if (i < bytes_read && buffer[i] == '['){
                    i++;
                    if (i + 3 < bytes_read && buffer[i] == 'M') {
                        int button = buffer[i + 1] & 3;
                        int release = (buffer[i + 1] >> 2) & 3;

                        int x = buffer[i + 2] - 32;
                        int y = buffer[i + 3] - 32;


                        if (button == 0) {

                            if (release == 3) Inputs.push_back(new GGUI::Input(0, Constants::MOUSE_LEFT_PRESS));
                            else Inputs.push_back(new GGUI::Input(0, Constants::MOUSE_LEFT_RELEASE));

                        } else if (button == 1) {

                            if (release == 3) Inputs.push_back(new GGUI::Input(0, Constants::MOUSE_MIDDLE_PRESS));
                            else Inputs.push_back(new GGUI::Input(0, Constants::MOUSE_MIDDLE_RELEASE));

                        } else if (button == 2) {

                            if (release == 3) Inputs.push_back(new GGUI::Input(0, Constants::MOUSE_RIGHT_PRESS));
                            else Inputs.push_back(new GGUI::Input(0, Constants::MOUSE_RIGHT_RELEASE));

                        } else if (button == 3) {
                            // handle scroll up event
                            Inputs.push_back(new GGUI::Input(0, Constants::MOUSE_MIDDLE_SCROLL_UP));

                        } else if (button == 4) {
                            // handle scroll down event
                            Inputs.push_back(new GGUI::Input(0, Constants::MOUSE_MIDDLE_SCROLL_DOWN));

                        }
                    }
                }
            }
            else if (c == '\r') { // handle enter key
                Inputs.push_back(new GGUI::Input('\n', Constants::ENTER));
            }
            else if (c == '\b') { // handle backspace key
                Inputs.push_back(new GGUI::Input(' ', Constants::BACKSPACE));
            }
            else if (c == ' ') { // handle shift key
                Inputs.push_back(new GGUI::Input(' ', Constants::SHIFT));
            }
            else { // handle any other key presses
                Inputs.push_back(new GGUI::Input(c, Constants::KEY_PRESS));
            }
        }
    
    }

    void Init_Platform_Stuff(){
        std::cout << "\e[?1003h\e[?25l" << std::flush;
        Previus_Flags = fcntl(STDIN_FILENO, F_GETFL, 0);
        int flags = -1; //fcntl(STDIN_FILENO, F_GETFL, 0); // get current flags for stdin
        fcntl(STDIN_FILENO, F_SETFL, flags); // set non-blocking flag

        signal(SIGINT, Exit);

        struct termios raw;
        tcgetattr(STDIN_FILENO, &raw);

        Previus_Raw = raw;

        raw.c_lflag &= ~(ECHO | ICANON);
        tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
    }

    #endif

    void MOUSE_API(){
        auto Mouse_Left_Pressed_For = std::chrono::duration_cast<std::chrono::milliseconds>(Current_Time - KEYBOARD_STATES[BUTTON_STATES::MOUSE_LEFT].Capture_Time).count();

        if (KEYBOARD_STATES[BUTTON_STATES::MOUSE_LEFT].State && Mouse_Left_Pressed_For >= (long long)SETTINGS::Mouse_Press_Down_Cooldown){
            Inputs.push_back(new GGUI::Input(0, Constants::MOUSE_LEFT_PRESSED));
        }
        else if (!KEYBOARD_STATES[BUTTON_STATES::MOUSE_LEFT].State && PREVIOUS_KEYBOARD_STATES[BUTTON_STATES::MOUSE_LEFT].State != KEYBOARD_STATES[BUTTON_STATES::MOUSE_LEFT].State){
            Inputs.push_back(new GGUI::Input(0, Constants::MOUSE_LEFT_CLICKED));
        }

        auto Mouse_Right_Pressed_For = std::chrono::duration_cast<std::chrono::milliseconds>(Current_Time - KEYBOARD_STATES[BUTTON_STATES::MOUSE_RIGHT].Capture_Time).count();

        if (KEYBOARD_STATES[BUTTON_STATES::MOUSE_RIGHT].State && Mouse_Right_Pressed_For >= SETTINGS::Mouse_Press_Down_Cooldown){
            Inputs.push_back(new GGUI::Input(0, Constants::MOUSE_RIGHT_PRESSED));
        }
        else if (!KEYBOARD_STATES[BUTTON_STATES::MOUSE_RIGHT].State && PREVIOUS_KEYBOARD_STATES[BUTTON_STATES::MOUSE_RIGHT].State != KEYBOARD_STATES[BUTTON_STATES::MOUSE_RIGHT].State){
            Inputs.push_back(new GGUI::Input(0, Constants::MOUSE_RIGHT_CLICKED));
        }

        auto Mouse_Middle_Pressed_For = std::chrono::duration_cast<std::chrono::milliseconds>(Current_Time - KEYBOARD_STATES[BUTTON_STATES::MOUSE_MIDDLE].Capture_Time).count();

        if (KEYBOARD_STATES[BUTTON_STATES::MOUSE_MIDDLE].State && Mouse_Middle_Pressed_For >= SETTINGS::Mouse_Press_Down_Cooldown){
            Inputs.push_back(new GGUI::Input(0, Constants::MOUSE_MIDDLE_PRESSED));
        }
        else if (!KEYBOARD_STATES[BUTTON_STATES::MOUSE_MIDDLE].State && PREVIOUS_KEYBOARD_STATES[BUTTON_STATES::MOUSE_MIDDLE].State != KEYBOARD_STATES[BUTTON_STATES::MOUSE_MIDDLE].State){
            Inputs.push_back(new GGUI::Input(0, Constants::MOUSE_MIDDLE_CLICKED));
        }   
    }

    void Handle_Escape(){
        Element* Current = Main;

        for (auto e : Event_Handlers){
            if (!Collides(e->Host, Mouse))
                continue;

            Current = e->Host;
            break;
        }

        if (!Current)
            return;

        if (!Current->Get_Parent())
            return;

        Mouse = Current->Get_Parent()->Get_Absolute_Position();
        Mouse.X += Current->Get_Parent()->Has_Border();
        Mouse.Y += Current->Get_Parent()->Has_Border();
    }

    void Handle_Tabulator(){
        bool Shift_Is_Pressed = KEYBOARD_STATES[BUTTON_STATES::SHIFT].State;

        Element* Current = nullptr;

        for (auto e : Event_Handlers){
            if (!Collides(e->Host, Mouse))
                continue;

            Current = e->Host;
            break;
        }

        if (!Current)
            return;

        if (Current->Get_Childs().size() > 0){
            int Start_index = 0;
            int End_Index = Current->Get_Childs().size() - 1;

            if (Shift_Is_Pressed){
                Start_index = Current->Get_Childs().size() - 1;
                End_Index = 0;
            }

            // Find the last child.
            for (int i = Current->Get_Childs().size() - 1; i >= 0;){
                if (!Current->Get_Childs()[i]->Is_Displayed())
                    goto CONTINUE;

                // try to find if this child has a event handler.
                for (auto e : Event_Handlers){
                    if (e->Host != Current->Get_Childs()[i])
                        continue;

                    Mouse = Current->Get_Childs()[i]->Get_Absolute_Position();
                    Mouse.X += Current->Get_Childs()[i]->Has_Border();
                    Mouse.Y += Current->Get_Childs()[i]->Has_Border();
                }

                CONTINUE:;
                if (Shift_Is_Pressed)
                    i--;
                else
                    i++;
            }
        }
        else{
            if (!Current->Get_Parent())
                return;

            // Find this current elements index in the parents child list.
            int Index = -1;

            for (int i = 0; i < Current->Get_Parent()->Get_Childs().size(); i++){
                if (Current->Get_Parent()->Get_Childs()[i] == Current){
                    Index = i;
                    break;
                }
            }

            if (Index == -1)
                Report("Child " + Current->Get_Name() + " could not be found from it's parent!");

            if (Shift_Is_Pressed){
                Index--;
                if (Index < 0)
                    Index = Current->Get_Parent()->Get_Childs().size() - 1;
            }
            else{
                Index++;
                if (Index >= Current->Get_Parent()->Get_Childs().size())
                    Index = 0;
            }

            Element* new_Current = Current->Get_Parent()->Get_Childs()[Index];
            Mouse = new_Current->Get_Absolute_Position();
            Mouse.X += new_Current->Has_Border();
            Mouse.Y += new_Current->Has_Border();
        }
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

    std::string Liquify_UTF_Text(std::vector<GGUI::UTF> Text, int Width, int Height){
        std::string Result = "";

        for (int y = 0; y < Height; y++){
            for (int x = 0; x < Width; x++){
                Result += Text[y * Width + x].To_String();
            }

            Result += "\n";
        }

        return Result;
    }

    void Update_Frame(){
        if (Pause_Render.load())
            return;

        Pause_Event_Thread = true;

        Abstract_Frame_Buffer = Main->Render();

        Frame_Buffer = Liquify_UTF_Text(Abstract_Frame_Buffer, Main->Get_Width(), Main->Get_Height());

        //Unlock the event handler.
        Pause_Event_Thread = false;
        Render_Frame();
    }

    void Pause_Renderer(){
        Pause_Render = true;
    }

    void Resume_Renderer(){
        Pause_Render = false;

        Update_Frame();
    }

    void Recall_Memories(){
        std::chrono::high_resolution_clock::time_point Current_Time = std::chrono::high_resolution_clock::now();

        for (int i = 0; i < Remember.size(); i++){
            //first calculate the time difference between the start if the task and the end task
            size_t Time_Difference = std::chrono::duration_cast<std::chrono::milliseconds>(Current_Time - Remember[i].Start_Time).count();

            //if the time difference is greater than the time limit, then delete the memory
            if (Time_Difference > Remember[i].End_Time){
                //Pause_Renderer();
                Remember[i].Job((Event*)&Remember[i]);

                Remember.erase(Remember.begin() + i);

                i--;
                
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

    void Un_Focus_Element(){
        if (!Focused_On)
            return;
        Focused_On->Get_Dirty().Dirty(STAIN_TYPE::COLOR | STAIN_TYPE::EDGE);
        Focused_On->Set_Focus(false);
        Focused_On = nullptr;
    }

    void Un_Hover_Element(){
        if (!Hovered_On)
            return;
        Hovered_On->Get_Dirty().Dirty(STAIN_TYPE::COLOR | STAIN_TYPE::EDGE);
        Hovered_On->Set_Hover_State(false);
        Hovered_On = nullptr;
    }

    void Update_Focused_Element(GGUI::Element* new_candidate){
        if (Focused_On == new_candidate || new_candidate == Main)
            return;

        //put the previus focused candidate into not-focus
        if (Focused_On){
            Un_Focus_Element();
        }

        //switch the candidate
        Focused_On = new_candidate;
        
        //set the new candidate to focused.
        Focused_On->Set_Focus(true);
        Focused_On->Get_Dirty().Dirty(STAIN_TYPE::COLOR | STAIN_TYPE::EDGE);
        Update_Frame();
    }

    void Update_Hovered_Element(GGUI::Element* new_candidate){
        if (Hovered_On == new_candidate || new_candidate == Main)
            return;

        //put the previus focused candidate into not-focus
        if (Hovered_On){
            Un_Hover_Element();
        }

        //switch the candidate
        Hovered_On = new_candidate;
        
        //set the new candidate to focused.
        Hovered_On->Set_Hover_State(true);
        Hovered_On->Get_Dirty().Dirty(STAIN_TYPE::COLOR | STAIN_TYPE::EDGE);
        Update_Frame();
    }


    // Events
    void Event_Handler(){
        if (Hovered_On && !Collides(Hovered_On, GGUI::Mouse)){
            Un_Hover_Element();
            Update_Frame();
        }

        Query_Inputs();
        for (auto& e : Event_Handlers){

            bool Has_Select_Event = false;

            for (int i = 0; i < Inputs.size(); i++){
                if (Has(Inputs[i]->Criteria, Constants::MOUSE_LEFT_CLICKED | Constants::ENTER))
                    Has_Select_Event = true;

                if (Is(e->Criteria, Inputs[i]->Criteria)){
                    //check if this job could be runned succesfully.
                    if (e->Job(Inputs[i])){
                        //dont let anyone else react to this event.
                        Inputs.erase(Inputs.begin() + i);
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
        }
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

    void Add_Class(std::string name, std::map<std::string, VALUE*> Styling){
        int Class_ID = Get_Free_Class_ID(name);

        Classes[Class_ID] = Styling;
    }

    void Init_Classes(){
        // Add default class
        std::string DEFAULT_NAME = "default";
        std::map<std::string, VALUE*> DEFAULT = {
            {STYLES::Text_Color, new RGB_VALUE(COLOR::WHITE)},
            {STYLES::Background_Color, new RGB_VALUE(COLOR::BLACK)},

            {STYLES::Border_Colour, new RGB_VALUE(COLOR::WHITE)},
            {STYLES::Border_Background_Color, new RGB_VALUE(COLOR::BLACK)},


            {STYLES::Hover_Text_Color, new RGB_VALUE(COLOR::WHITE)},
            {STYLES::Hover_Background_Color, new RGB_VALUE(COLOR::DARK_GRAY)},

            {STYLES::Hover_Border_Color, new RGB_VALUE(COLOR::WHITE)},
            {STYLES::Hover_Border_Background_Color, new RGB_VALUE(COLOR::BLACK)},


            {STYLES::Focus_Text_Color, new RGB_VALUE(COLOR::BLACK)},
            {STYLES::Focus_Background_Color, new RGB_VALUE(COLOR::WHITE)},

            {STYLES::Focus_Border_Color, new RGB_VALUE(COLOR::WHITE)},
            {STYLES::Focus_Border_Background_Color, new RGB_VALUE(COLOR::BLACK)},
        };

        Add_Class(DEFAULT_NAME, DEFAULT);
    }

    //Inits GGUI and returns the main window.
    GGUI::Window* Init_Renderer(){
        //Save the state before the init
        bool Default_Render_State = Pause_Render;
        bool Default_Event_State = Pause_Event_Thread;

        Current_Time = std::chrono::high_resolution_clock::now();
        Previous_Time = Current_Time;

        //pause the renderer
        Pause_Render = true;
        Pause_Event_Thread = true;

        Init_Platform_Stuff();
        Update_Max_Width_And_Height();
        GGUI::Constants::Init();
        Init_Classes();


        //now we need to allocate the buffer string by the width and height of the terminal
        Abstract_Frame_Buffer.resize(Max_Height * Max_Width);

        Main = new Window("", Max_Width, Max_Height);

        Abstract_Frame_Buffer = Main->Render();

        Frame_Buffer = Liquify_UTF_Text(Abstract_Frame_Buffer, Main->Get_Width(), Main->Get_Height());

        std::thread Job_Scheduler([&](){
            int i = 0;
            while (true){
                if (Pause_Event_Thread.load())
                    continue;

                Current_Time = std::chrono::high_resolution_clock::now();

                // Calculate the delta time.
                Delta_Time = std::chrono::duration_cast<std::chrono::milliseconds>(Current_Time - Previous_Time).count();

                Recall_Memories();
                Event_Handler();

                Previous_Time = Current_Time;
                std::this_thread::sleep_for(std::chrono::milliseconds(UPDATE_SPEED_MIILISECONDS)); 
            }
        });

        Job_Scheduler.detach();

        Pause_Render = Default_Render_State;

        return Main;
    }

    void Report(std::string Problem){
        Pause_Renderer();

        GGUI::Text_Field* txt = new GGUI::Text_Field(Problem);

        bool Has_Border = true;

        unsigned int w = txt->Get_Width() + Has_Border * 2;
        unsigned int h = txt->Get_Height() + Has_Border * 2;

        unsigned int W_Center = (Max_Width - w) / 2;
        unsigned int H_Center = (Max_Height - h) / 2;

        GGUI::Window* tmp = new GGUI::Window(
            "ERROR!",
            w, h,
            GGUI::COLOR::RED,
            GGUI::COLOR::BLACK,
            GGUI::COLOR::RED,
            GGUI::COLOR::BLACK
        );

        tmp->Set_Parent(Main);
        tmp->Set_Position({W_Center, H_Center, INT32_MAX});

        tmp->Add_Child(txt);

        Main->Add_Child(tmp);

        Remember.push_back(Memory(
            TIME::SECOND * 10,
            [=](GGUI::Event* e){
                //delete tmp;
                tmp->Remove();
                //job succesfully done
                return true;
            }
        ));

        Resume_Renderer();
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
        for (int Parent_Y = 0; Parent_Y < Parent->Get_Height(); Parent_Y++){
            for (int Parent_X = 0; Parent_X < Parent->Get_Width(); Parent_X++){
                if (Parent_Y >= C.Y && Parent_X >= C.X &&
                    Parent_Y <= C.Y + child->Get_Height() && Parent_X <= C.X + child->Get_Width())
                {
                    Parent_Buffer[Parent_Y * Parent->Get_Width() + Parent_X] = Text[i++];
                }
            }
        }
    }

    void Pause_Renderer(std::function<void()> f){
        bool Original_Value = Pause_Render;

        Pause_Renderer();

        f();

        if (!Original_Value)
            Resume_Renderer(); 
    }

    // Use this to use GGUI.
    void GGUI(std::function<void()> DOM, unsigned long long Sleep_For){
        bool Previud_Event_Value = Pause_Event_Thread;
        Pause_Event_Thread = true;

        Pause_Renderer([=](){

            Init_Renderer();

            DOM();
        });

        Pause_Event_Thread = Previud_Event_Value;
        SLEEP(Sleep_For);
    }
}