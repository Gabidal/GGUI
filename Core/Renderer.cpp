#include "Renderer.h"
#include "../Elements/File_Streamer.h"

#include <string>
#include <cassert>
#include <math.h>
#include <sstream>

namespace GGUI{
    std::vector<UTF> SAFE_MIRROR;                               // Only used for references to be initalized to point at.
    std::vector<UTF>& Abstract_Frame_Buffer = SAFE_MIRROR;      // 2D clean vector without bold nor color
    std::string Frame_Buffer;                                   // string with bold and color, this what gets drawn to console.

    // For threading system
    namespace Atomic{
        std::mutex Mutex;
        std::condition_variable Condition;

        Status Pause_Render_Thread = Status::RESUMED;
    }

    std::vector<INTERNAL::BUFFER_CAPTURE*> Global_Buffer_Captures;

    unsigned int Max_Width = 0;
    unsigned int Max_Height = 0;

    Atomic::Guard<std::vector<Memory>> Remember;

    std::vector<Action*> Event_Handlers;
    std::vector<Input*> Inputs;
    std::chrono::system_clock::time_point Last_Input_Clear_Time;

    std::unordered_map<std::string, Element*> Element_Names;

    Element* Focused_On = nullptr;
    Element* Hovered_On = nullptr;

    bool Platform_Initialized = false;

    IVector3 Mouse;
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

    unsigned long long Render_Delay;    // describes how long previous render cycle took in ms
    unsigned long long Event_Delay;    // describes how long previous memory tasks took in ms

    inline Atomic::Guard<std::unordered_map<int, Styling>> Classes;

    inline std::unordered_map<std::string, int> Class_Names;

    std::unordered_map<GGUI::Terminal_Canvas*, bool> Multi_Frame_Canvas;

    void* Stack_Start_Address = 0;
    void* Heap_Start_Address = 0;

    Window* Main = nullptr;
    
    const std::string ERROR_LOGGER = "_ERROR_LOGGER_";
    const std::string HISTORY = "_HISTORY_";

    // This class contains carry flags from previous cycle cross-thread, if another thread had some un-finished things when another thread was already running.
    class Carry{
    public:
        bool Resize = false;
    };

    Atomic::Guard<Carry> Carry_Flags; 

    bool Collides(GGUI::IVector3 A, GGUI::IVector3 B, int A_Width, int A_Height, int B_Width, int B_Height) {
        // Check if the rectangles overlap in the x-axis and y-axis
        return (
            A.X < B.X + B_Width &&   // A's right edge is beyond B's left edge
            A.X + A_Width > B.X &&   // A's left edge is before B's right edge
            A.Y < B.Y + B_Height &&  // A's bottom edge is below B's top edge
            A.Y + A_Height > B.Y     // A's top edge is above B's bottom edge
        );
    }

    bool Collides(GGUI::Element* a, GGUI::Element* b, bool Identity = true) {
        if (a == b)
            return Identity;    // For custom purposes, defaults into true

        return Collides(
            a->Get_Absolute_Position(), b->Get_Absolute_Position(),
            a->Get_Width(), a->Get_Height(), b->Get_Width(), b->Get_Height()
        );
    }

    bool Collides(GGUI::Element* a, GGUI::IVector3 b) {
        // Call the Collides function with the element's position and dimensions, and the point with assumed dimensions of 1x1.
        return Collides(a->Get_Absolute_Position(), b, a->Get_Width(), a->Get_Height(), 1, 1);
    }

    Element* Get_Accurate_Element_From(IVector3 c, Element* Parent){
        //first check if the c is in bounds of Parent.
        if (!Collides(Parent, c)){
            return nullptr;
        }

        // Check all the child elements of the parent.
        for (auto child : Parent->Get_Childs()){
            if (Collides(child, c)){
                // If a child element contains the position, search in the child element.
                return Get_Accurate_Element_From(c, child);
            }
        }

        // If no child element contains the position, return the parent element.
        return Parent;
    }

    IVector3 Find_Upper_Element(){
        //first get the current element.
        Element* Current_Element = Get_Accurate_Element_From(Mouse, Main);

        if (Current_Element == nullptr){
            return false;
        }

        IVector3 tmp_c = Current_Element->Get_Position();

        tmp_c.Y--;  // Move one pixel up

        Element* Upper_Element = Get_Accurate_Element_From(tmp_c, Main);

        if (Upper_Element && Upper_Element != (Element*)&Main){
            return Upper_Element->Get_Position();
        }

        return Current_Element->Get_Position();
    }

    IVector3 Find_Lower_Element(){
        //first get the current element.
        Element* Current_Element = Get_Accurate_Element_From(Mouse, Main);

        if (Current_Element == nullptr){
            return false;
        }

        IVector3 tmp_c = Current_Element->Get_Position();

        tmp_c.Y += Current_Element->Get_Height();

        Element* Lower_Element = Get_Accurate_Element_From(tmp_c, Main);

        if (Lower_Element && Lower_Element != (Element*)&Main){
            return Lower_Element->Get_Position();
        }

        return Current_Element->Get_Position();
    }

    IVector3 Find_Left_Element(){
        //first get the current element.
        Element* Current_Element = Get_Accurate_Element_From(Mouse, Main);

        if (Current_Element == nullptr){
            return false;
        }

        IVector3 tmp_c = Current_Element->Get_Position();

        // Move one pixel to the left
        tmp_c.X--;

        Element* Left_Element = Get_Accurate_Element_From(tmp_c, Main);

        if (Left_Element && Left_Element != (Element*)&Main){
            // If a left element is found, return its position
            return Left_Element->Get_Position();
        }

        // If no left element is found, return the current element's position
        return Current_Element->Get_Position();
    }

    IVector3 Find_Right_Element(){
        //first get the current element.
        Element* Current_Element = Get_Accurate_Element_From(Mouse, Main);

        if (Current_Element == nullptr){
            return false;
        }

        IVector3 tmp_c = Current_Element->Get_Position();

        // Move one pixel to the right
        tmp_c.X += Current_Element->Get_Width();

        Element* Right_Element = Get_Accurate_Element_From(tmp_c, Main);

        if (Right_Element && Right_Element != (Element*)&Main){
            // If a right element is found, return its position
            return Right_Element->Get_Position();
        }

        // If no right element is found, return the current element's position
        return Current_Element->Get_Position();
    }

    /// @brief Finds the closest element to the given position from the given candidates.
    /// @param start The position to start from.
    /// @param Candidates The list of elements to check from.
    /// @return The closest element to the given position.
    Element* Find_Closest_Absolute_Element(IVector3 start, std::vector<Element*> Candidates){
        // Start from the position and check if the up, down, left, right are within the bounds of the renderable window.
        // If they are, check if they collide with any element.
        // cast "rays" to each four directions, and return the lengths of each collision between the center of the rectangles and the start point.
        // return the smallest one.
        if (Candidates.size() == 0){
            Report("Missing Candidates!");
        }

        Element* Best_Candidate = nullptr;
        float Shortest_Distance = std::numeric_limits<float>::max();
        IVector3 CC; // Center of Candidate

        for (auto& candidate : Candidates){
            if (!candidate) 
                continue;   // Incase of event handlers with their stupid empty host.

            // Calculate the distance between the candidate position and the start position
            CC = candidate->Get_Absolute_Position();
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
        /// Sleep for the specified amount of milliseconds.
        Sleep(mm);
    }

    GGUI::HANDLE GLOBAL_STD_OUTPUT_HANDLE;
    GGUI::HANDLE GLOBAL_STD_INPUT_HANDLE;

    DWORD PREVIOUS_CONSOLE_OUTPUT_STATE;
    DWORD PREVIOUS_CONSOLE_INPUT_STATE;

    CONSOLE_SCREEN_BUFFER_INFO Get_Console_Info();

    // This is here out from the Query_Inputs, so that we can differentiate querying and translation of said input.
    const unsigned int Raw_Input_Capacity = UINT8_MAX * 10;
    INPUT_RECORD Raw_Input[Raw_Input_Capacity];
    int Raw_Input_Size = 0;

    /// @brief A function to render a frame.
    /// @details This function is called from the event loop. It renders the frame by writing the Frame_Buffer data to the console.
    ///          It also moves the cursor to the top left corner of the screen.
    void Render_Frame(){
        // The number of bytes written to the console, not used anywhere else.
        unsigned long long tmp = 0;
        // Move the cursor to the top left corner of the screen.
        SetConsoleCursorPosition(GLOBAL_STD_OUTPUT_HANDLE, {0, 0});
        // Write the Frame_Buffer data to the console.
        WriteFile(GLOBAL_STD_OUTPUT_HANDLE, Frame_Buffer.data(), Frame_Buffer.size(), reinterpret_cast<LPDWORD>(&tmp), NULL);
    }

    /// @brief Updates the maximum width and height of the console.
    /// @details This function is used to get the maximum width and height of the console.
    ///          It is called from the Query_Inputs function.
    void Update_Max_Width_And_Height(){
        // Get the console information.
        CONSOLE_SCREEN_BUFFER_INFO info = Get_Console_Info();

        // Update the maximum width and height.
        Max_Width = info.srWindow.Right - info.srWindow.Left + 1;
        Max_Height = info.srWindow.Bottom - info.srWindow.Top + 1;

        // Check if we got the console information correctly.
        if (Max_Width == 0 || Max_Height == 0){
            Report("Failed to get console info!");
        }

        // Check that the main window is not active and if so, set its dimensions.
        if (Main)
            Main->Set_Dimensions(Max_Width, Max_Height);
    }

    void Update_Frame(bool Lock_Event_Thread);
    //Is called on every cycle.

    /// @brief A function to reverse-engineer keybinds.
    /// @param keybind_value The value of the key to reverse-engineer.
    /// @return The reversed keybind value.
    /// @details This function is used to reverse-engineer keybinds. It checks if the keybind is in the known keybinding table.
    ///          If it is, it returns the reversed keybind value. If not, it returns the normal value.
    char Reverse_Engineer_Keybinds(char keybind_value){
        // The current known keybinding table:

        /*
            CTRL+SHIFT+I => TAB
            // TODO: Add more keybinds to the table
        */

        if (KEYBOARD_STATES[BUTTON_STATES::CONTROL].State && KEYBOARD_STATES[BUTTON_STATES::SHIFT].State){
            if (keybind_value == VK_TAB){
                return 'i';
            }
        }

        // return the normal value, if there is no key-binds detected.
        return keybind_value;
    }

    /// @brief Waits for user input, will not translate, use Translate_Inputs for that.
    /// @details This function waits for user input and stores it in the Raw_Input array.
    ///          It is called from the event loop.
    void Query_Inputs(){
        // For appending to already existing buffered input which has not yet been processed, we can use the previous Raw_Input_Size to deduce the new starting point.
        INPUT_RECORD* Current_Starting_Address = Raw_Input + Raw_Input_Size;

        // Ceil the value so that negative numbers wont create overflows.
        unsigned int Current_Usable_Capacity = Max(Raw_Input_Capacity - Raw_Input_Size, Raw_Input_Capacity);

        // Read the console input and store it in Raw_Input.
        ReadConsoleInput(
            GLOBAL_STD_INPUT_HANDLE,
            Current_Starting_Address,
            Current_Usable_Capacity,
            (LPDWORD)&Raw_Input_Size
        );
    }

    // Continuation of Query_Input, while differentiate the execute timings.
    /// @brief Translate the input events stored in Raw_Input into Input objects.
    /// @details This function is used to translate the input events stored in Raw_Input into Input objects. It checks if the event is a key event, and if so, it checks if the key is a special key (up, down, left, right, enter, shift, control, backspace, escape, tab) and if so, it creates an Input object with the corresponding Constants:: value. If the key is not a special key, it creates an Input object with the key's ASCII value and Constants::KEY_PRESS. If the event is not a key event, it checks if the event is a mouse event and if so, it checks if the mouse event is a movement, click or scroll event and if so, it creates an Input object with the corresponding Constants:: value. Finally, it resets the Raw_Input_Size to 0.
    void Translate_Inputs(){
        // Clean the keyboard states.
        PREVIOUS_KEYBOARD_STATES = KEYBOARD_STATES;

        for (int i = 0; i < Raw_Input_Size; i++){
            if (Raw_Input[i].EventType == KEY_EVENT){

                bool Pressed = Raw_Input[i].Event.KeyEvent.bKeyDown;

                if (Raw_Input[i].Event.KeyEvent.wVirtualKeyCode == VK_UP){
                    Inputs.push_back(new GGUI::Input(0, Constants::UP));
                    KEYBOARD_STATES[BUTTON_STATES::UP] = BUTTON_STATE(Pressed);
                }
                else if (Raw_Input[i].Event.KeyEvent.wVirtualKeyCode == VK_DOWN){
                    Inputs.push_back(new GGUI::Input(0, Constants::DOWN));
                    KEYBOARD_STATES[BUTTON_STATES::DOWN] = BUTTON_STATE(Pressed);
                }
                else if (Raw_Input[i].Event.KeyEvent.wVirtualKeyCode == VK_LEFT){
                    Inputs.push_back(new GGUI::Input(0, Constants::LEFT));
                    KEYBOARD_STATES[BUTTON_STATES::LEFT] = BUTTON_STATE(Pressed);
                }
                else if (Raw_Input[i].Event.KeyEvent.wVirtualKeyCode == VK_RIGHT){
                    Inputs.push_back(new GGUI::Input(0, Constants::RIGHT));
                    KEYBOARD_STATES[BUTTON_STATES::RIGHT] = BUTTON_STATE(Pressed);
                }
                else if (Raw_Input[i].Event.KeyEvent.wVirtualKeyCode == VK_RETURN){
                    Inputs.push_back(new GGUI::Input('\n', Constants::ENTER));
                    KEYBOARD_STATES[BUTTON_STATES::ENTER] = BUTTON_STATE(Pressed);
                }
                else if (Raw_Input[i].Event.KeyEvent.wVirtualKeyCode == VK_SHIFT){
                    Inputs.push_back(new GGUI::Input(' ', Constants::SHIFT));
                    KEYBOARD_STATES[BUTTON_STATES::SHIFT] = BUTTON_STATE(Pressed);
                }
                else if (Raw_Input[i].Event.KeyEvent.wVirtualKeyCode == VK_CONTROL){
                    Inputs.push_back(new GGUI::Input(' ', Constants::CONTROL));
                    KEYBOARD_STATES[BUTTON_STATES::CONTROL] = BUTTON_STATE(Pressed);
                }
                else if (Raw_Input[i].Event.KeyEvent.wVirtualKeyCode == VK_BACK){
                    Inputs.push_back(new GGUI::Input(' ', Constants::BACKSPACE));
                    KEYBOARD_STATES[BUTTON_STATES::BACKSPACE] = BUTTON_STATE(Pressed);
                }
                else if (Raw_Input[i].Event.KeyEvent.wVirtualKeyCode == VK_ESCAPE){
                    Inputs.push_back(new GGUI::Input(' ', Constants::ESCAPE));
                    KEYBOARD_STATES[BUTTON_STATES::ESC] = BUTTON_STATE(Pressed);
                    Handle_Escape();
                }
                else if (Raw_Input[i].Event.KeyEvent.wVirtualKeyCode == VK_TAB){
                    Inputs.push_back(new GGUI::Input(' ', Constants::TAB));
                    KEYBOARD_STATES[BUTTON_STATES::TAB] = BUTTON_STATE(Pressed);
                    Handle_Tabulator();
                }
                else if (Raw_Input[i].Event.KeyEvent.uChar.AsciiChar != 0 && Pressed){
                    char Result = Reverse_Engineer_Keybinds(Raw_Input[i].Event.KeyEvent.uChar.AsciiChar);

                    Inputs.push_back(new GGUI::Input(Result, Constants::KEY_PRESS));
                }
            }
            else if (Raw_Input[i].EventType == WINDOW_BUFFER_SIZE_EVENT){
                Carry_Flags([](GGUI::Carry& current_carry){
                    current_carry.Resize = true;    // Tell the render thread that an resize is needed to be performed.
                });
            }
            else if (Raw_Input[i].EventType == MOUSE_EVENT && Mouse_Movement_Enabled){
                if (Raw_Input[i].Event.MouseEvent.dwEventFlags == MOUSE_MOVED){
                    // Get mouse coordinates
                    COORD mousePos = Raw_Input[i].Event.MouseEvent.dwMousePosition;
                    // Handle cursor movement
                    Mouse.X = mousePos.X;
                    Mouse.Y = mousePos.Y;
                }
                // Handle mouse clicks
                // TODO: Windows doesn't give release events.
                // Yes it does you fucking moron!
                if ((Raw_Input[i].Event.MouseEvent.dwButtonState & FROM_LEFT_1ST_BUTTON_PRESSED) != 0) {
                    //PREVIOUS_KEYBOARD_STATES[BUTTON_STATES::MOUSE_LEFT].State = KEYBOARD_STATES[BUTTON_STATES::MOUSE_LEFT].State;
                    KEYBOARD_STATES[BUTTON_STATES::MOUSE_LEFT].State = true;
                    KEYBOARD_STATES[BUTTON_STATES::MOUSE_LEFT].Capture_Time = std::chrono::high_resolution_clock::now();
                }
                else if ((Raw_Input[i].Event.MouseEvent.dwButtonState & FROM_LEFT_1ST_BUTTON_PRESSED) == 0) {
                    //PREVIOUS_KEYBOARD_STATES[BUTTON_STATES::MOUSE_LEFT].State = KEYBOARD_STATES[BUTTON_STATES::MOUSE_LEFT].State;
                    KEYBOARD_STATES[BUTTON_STATES::MOUSE_LEFT].State = false;
                }

                if ((Raw_Input[i].Event.MouseEvent.dwButtonState & RIGHTMOST_BUTTON_PRESSED) != 0) {
                    //PREVIOUS_KEYBOARD_STATES[BUTTON_STATES::MOUSE_RIGHT].State = KEYBOARD_STATES[BUTTON_STATES::MOUSE_RIGHT].State;
                    KEYBOARD_STATES[BUTTON_STATES::MOUSE_RIGHT].State = true;
                    KEYBOARD_STATES[BUTTON_STATES::MOUSE_RIGHT].Capture_Time = std::chrono::high_resolution_clock::now();
                }
                else if ((Raw_Input[i].Event.MouseEvent.dwButtonState & RIGHTMOST_BUTTON_PRESSED) == 0) {
                    //PREVIOUS_KEYBOARD_STATES[BUTTON_STATES::MOUSE_RIGHT].State = KEYBOARD_STATES[BUTTON_STATES::MOUSE_RIGHT].State;
                    KEYBOARD_STATES[BUTTON_STATES::MOUSE_RIGHT].State = false;
                }
            
                // mouse scroll up
                if (Raw_Input[i].Event.MouseEvent.dwEventFlags == MOUSE_WHEELED){
                    // check if it has been wheeled up or down
                    int Scroll_Direction = GET_WHEEL_DELTA_WPARAM(Raw_Input[i].Event.MouseEvent.dwButtonState);

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
   
        // We can assume that the Raw_Input buffer will be fully translated by this point, if not, then something is wrong!!!
        // We can now also restart the Raw_Input_Size.
        Raw_Input_Size = 0;
    }


    namespace Constants{
        namespace ANSI{
            inline int ENABLE_UTF8_MODE_FOR_WINDOWS = 65001;
        }
    }

    /// @brief Initializes platform-specific settings for console handling.
    /// @details This function sets up the console handles and modes required for input and output operations.
    ///          It enables mouse and window input, sets UTF-8 mode for output, and prepares the console for
    ///          handling specific ANSI features.
    void Init_Platform_Stuff(){
        // Save the STD handles to prevent excess calls.
        GLOBAL_STD_OUTPUT_HANDLE = GetStdHandle(STD_OUTPUT_HANDLE);
        GLOBAL_STD_INPUT_HANDLE = GetStdHandle(STD_INPUT_HANDLE);

        // Retrieve and store previous console modes for restoration upon exit.
        GetConsoleMode(GLOBAL_STD_OUTPUT_HANDLE, &PREVIOUS_CONSOLE_OUTPUT_STATE);
        GetConsoleMode(GLOBAL_STD_INPUT_HANDLE, &PREVIOUS_CONSOLE_INPUT_STATE);

        // Set new console modes with extended flags, mouse, and window input enabled.
        SetConsoleMode(GLOBAL_STD_OUTPUT_HANDLE, -1);
        SetConsoleMode(GLOBAL_STD_INPUT_HANDLE, ENABLE_EXTENDED_FLAGS | ENABLE_MOUSE_INPUT | ENABLE_WINDOW_INPUT);

        // Enable specific ANSI features for mouse event reporting and hide the mouse cursor.
        std::cout << Constants::ANSI::Enable_Private_SGR_Feature(Constants::ANSI::REPORT_MOUSE_ALL_EVENTS).To_String() 
                  << Constants::ANSI::Enable_Private_SGR_Feature(Constants::ANSI::MOUSE_CURSOR, false).To_String();
        std::cout.flush();

        // Set the console output code page to UTF-8 mode.
        SetConsoleOutputCP(Constants::ANSI::ENABLE_UTF8_MODE_FOR_WINDOWS);

        // Mark the platform as initialized.
        Platform_Initialized = true;
    }

    /// @brief Retrieves the console screen buffer information.
    /// @details This function retrieves the console screen buffer information using the GetConsoleScreenBufferInfo function.
    ///          It first checks if the GLOBAL_STD_OUTPUT_HANDLE has been set, and if not, it sets it by calling GetStdHandle.
    ///          It then calls GetConsoleScreenBufferInfo and if the call fails, it will report the error.
    /// @return The console screen buffer information.
    CONSOLE_SCREEN_BUFFER_INFO Get_Console_Info() {
        if (GLOBAL_STD_OUTPUT_HANDLE == 0) {
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

    /// @brief Reads the entire console screen buffer.
    /// @details This function reads the entire console screen buffer and returns it as a vector of characters.
    ///          It first gets the console screen buffer information using GetConsoleScreenBufferInfo.
    ///          It then allocates a vector of CHAR_INFO structures with the same size as the screen buffer.
    ///          It then calls ReadConsoleOutput to read the console screen buffer into the vector of CHAR_INFO structures.
    ///          Finally, it loops through the vector of CHAR_INFO structures and copies the AsciiChar member of each structure
    ///          into the corresponding position in the output vector.
    /// @return The entire console screen buffer as a vector of characters.
    std::vector<char> Read_Console(){
        // Get the console screen buffer information
        CONSOLE_SCREEN_BUFFER_INFO Info = Get_Console_Info();

        // Allocate a vector of CHAR_INFO structures with the same size as the screen buffer
        std::vector<char> Buffer(Info.dwSize.X * Info.dwSize.Y);
        std::vector<CHAR_INFO> Fake_Buffer(Info.dwSize.X * Info.dwSize.Y);

        // The area to read
        SMALL_RECT rect {0,0, Info.dwSize.X-1, Info.dwSize.Y} ; // the console screen (buffer) region to read from (120x4)

        // Read the console screen buffer into the vector of CHAR_INFO structures
        ReadConsoleOutput( GLOBAL_STD_OUTPUT_HANDLE, Fake_Buffer.data(), {Info.dwSize.X, Info.dwSize.Y} /*buffer size cols x rows*/, {0,0} /*buffer top,left*/, &rect );

        // Copy the AsciiChar member of each CHAR_INFO structure into the corresponding position in the output vector
        for (unsigned int i = 0; i < Fake_Buffer.size(); i++){
            Buffer[i] = Fake_Buffer[i].Char.AsciiChar;
        }

        return Buffer;
    }

    /// @brief De-initializes platform-specific settings and resources.
    /// @details This function restores console modes to their previous states, cleans up file stream handles,
    ///          and disables specific ANSI features. It ensures that any platform-specific settings are reset
    ///          before the application exits.
    void De_Initialize(){
        // Clean up file stream handles
        for (auto File_Handle : File_Streamer_Handles){
            File_Handle.second->~FILE_STREAM(); // Manually call destructor to release resources
        }

        // Restore previous console modes
        SetConsoleMode(GLOBAL_STD_OUTPUT_HANDLE, PREVIOUS_CONSOLE_OUTPUT_STATE);
        SetConsoleMode(GLOBAL_STD_INPUT_HANDLE, PREVIOUS_CONSOLE_INPUT_STATE);

        // Disable specific ANSI features and restore screen
        std::cout << Constants::ANSI::Enable_Private_SGR_Feature(Constants::ANSI::MOUSE_CURSOR).To_String();
        std::cout << Constants::ANSI::Enable_Private_SGR_Feature(Constants::ANSI::REPORT_MOUSE_ALL_EVENTS, false).To_String();
        std::cout << Constants::ANSI::Enable_Private_SGR_Feature(Constants::ANSI::SCREEN_CAPTURE, false).To_String();
        std::cout << std::flush; // Ensure all output is flushed to console
    }

    /// @brief Cleanly exits the GGUI library.
    /// @details This function is called automatically when the application exits, or can be called manually to exit the library at any time.
    ///          It ensures that any platform-specific settings are reset before the application exits.
    /// @param signum The exit code to return to the operating system.
    void Exit(int signum){
        // Clean up platform-specific resources and settings
        De_Initialize();

        // Exit the application with the specified exit code
        exit(signum);
    }


    /// @brief Retrieves a list of font files from the Windows registry.
    /// @details This function retrieves a list of font files from the Windows registry.
    ///          It first opens the "Software\\Microsoft\\Windows NT\\CurrentVersion\\Fonts" key in the registry,
    ///          and then enumerates all the values in the key.
    ///          Each value is a string containing the path to a font file.
    ///          The function returns a vector of strings containing all the font files found in the registry.
    std::vector<std::string> Get_List_Of_Font_Files() {
        std::vector<std::string> Result;

        // Open the "Software\\Microsoft\\Windows NT\\CurrentVersion\\Fonts" key in the registry
        HKEY hKey;
        if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, "Software\\Microsoft\\Windows NT\\CurrentVersion\\Fonts", 0, KEY_READ, &hKey) == ERROR_SUCCESS) {
            DWORD cValues; 
            // Get the number of values in the key
            RegQueryInfoKey(hKey, NULL, NULL, NULL, NULL, NULL, NULL, &cValues, NULL, NULL, NULL, NULL);
            for (DWORD i = 0; i < cValues; i++) {
                char valueName[1024];
                DWORD valueNameSize = 1024; 
                BYTE valueData[1024];
                DWORD valueDataSize = 1024;
                DWORD valueType;
                // Enumerate the values in the key
                if (RegEnumValue(hKey, i, valueName, &valueNameSize, NULL, &valueType, valueData, &valueDataSize) == ERROR_SUCCESS) {
                    // Add the file to the Result list
                    Result.push_back((char*)valueData);
                }
            }
            // Close the key
            RegCloseKey(hKey);
        }

        return Result;
    }

    /// @brief Reports the current stack trace along with a specified problem description.
    /// @details This function captures the call stack, symbolically resolves the addresses, and formats the stack trace.
    ///          The resulting formatted trace is then reported along with the provided problem description.
    /// @param Problem A description of the problem to be reported with the stack trace.
    void Report_Stack(std::string Problem) {
        const int Stack_Trace_Depth = 10;
        void* Ptr_Table[Stack_Trace_Depth];
        unsigned short Usable_Depth;
        SYMBOL_INFO* symbol;
        HANDLE process;

        // Get the current process handle
        process = GetCurrentProcess();

        // Initialize the symbol handler for the process
        SymInitialize(process, NULL, TRUE);

        // Capture the stack backtrace
        Usable_Depth = CaptureStackBackTrace(0, Stack_Trace_Depth, Ptr_Table, NULL);

        // Allocate memory for a SYMBOL_INFO structure with space for a name
        symbol = (SYMBOL_INFO*)calloc(sizeof(SYMBOL_INFO) + 256 * sizeof(char), 1);
        symbol->MaxNameLen = 255;
        symbol->SizeOfStruct = sizeof(SYMBOL_INFO);

        // Update maximum width and height if not already set
        if (Max_Width == 0) {
            Update_Max_Width_And_Height();
        }

        // Initialize result string with a header
        std::string Result = "Stack Trace:\n";
        int Usable_Stack_Index = 0;
        bool Use_Indent = Usable_Depth < (Max_Width / 2);

        // Iterate over the captured stack frames
        for (unsigned int Stack_Index = 0; Stack_Index < Usable_Depth; Stack_Index++) {
            // Resolve the symbol from the address
            SymFromAddr(process, (DWORD64)(Ptr_Table[Stack_Index]), 0, symbol);

            // Skip empty symbol names
            if (symbol->Name[0] == 0)
                continue;

            // Determine the branch start symbol
            std::string Branch_Start = "|";
            if (Stack_Index == (unsigned int)Usable_Depth - 1)
                Branch_Start = "\\";

            // Create indentation for the current stack level
            std::string Indent = "";
            for (int i = 0; i < Usable_Stack_Index && Use_Indent; i++)
                Indent += "-";

            // Append the formatted stack frame info to the result
            Result += Branch_Start + Indent + " " + symbol->Name + "\n";
            Usable_Stack_Index++;
        }

        // Free allocated memory for symbol
        free(symbol);

        // Append the problem description to the result
        Result += "Problem: " + Problem;

        // Report the final result
        Report(Result);
    }

    #else
    #include <sys/ioctl.h>
    #include <signal.h>
    #include <termios.h>
    #include <execinfo.h>

    int Previous_Flags = 0;
    struct termios Previous_Raw;

    // Stored globally, so that translation and inquiry can be separate proccess.
    const unsigned int Raw_Input_Capacity = UINT8_MAX * 2;
    unsigned char Raw_Input[Raw_Input_Capacity];
    unsigned int Raw_Input_Size = 0;

    /**
     * @brief De-initializes platform-specific settings and resources.
     * @details This function restores console modes to their previous states, cleans up file stream handles,
     *          and disables specific ANSI features. It ensures that any platform-specific settings are reset
     *          before the application exits.
     */
    void De_Initialize(){
        // Restore previous console modes
        for (auto File_Handle : File_Streamer_Handles){
            File_Handle.second->~FILE_STREAM();
        }

        // Restore default cursor visibility
        std::cout << Constants::ANSI::Enable_Private_SGR_Feature(Constants::ANSI::MOUSE_CURSOR).To_String();

        // Disable mouse event reporting
        std::cout << Constants::ANSI::Enable_Private_SGR_Feature(Constants::ANSI::REPORT_MOUSE_ALL_EVENTS, false).To_String();

        // Disable screen capture
        std::cout << Constants::ANSI::Enable_Private_SGR_Feature(Constants::ANSI::SCREEN_CAPTURE, false).To_String();  // restores the screen.
        std::cout << std::flush;

        // Restore previous file descriptor flags
        fcntl(STDIN_FILENO, F_SETFL, Previous_Flags); // set non-blocking flag

        // Restore previous terminal attributes
        tcsetattr(STDIN_FILENO, TCSAFLUSH, &Previous_Raw);
    }

    /**
     * @brief De-initializes platform-specific settings and resources and exits the application.
     * @details This function is called by the Exit function to de-initialize platform-specific settings and resources.
     *          It ensures that any platform-specific settings are reset before the application exits.
     * @param signum The exit code for the application.
     */
    void Exit(int signum){

        De_Initialize();

        // Exit the application with the specified exit code
        exit(signum);
    }

    /// @brief A cross-platform sleep function using nanosleep.
    /// @param mm The number of milliseconds to sleep for.
    /// @details This function pauses the execution of the program for a specified amount of time using nanosleep.
    ///          It breaks down the milliseconds into seconds and nanoseconds for the timespec structure and handles
    ///          any interruptions by retrying the nanosleep with the remaining time.
    void SLEEP(unsigned int mm){
        // Define timespec structure for the required sleep duration
        struct timespec req = {0, 0};
        // Calculate seconds from milliseconds
        time_t sec = (int)(mm / 1000);
        // Calculate remaining milliseconds
        mm = mm - (sec * 1000);
        // Set the seconds and nanoseconds in the timespec structure
        req.tv_sec = sec;
        req.tv_nsec = mm * 1000000L;
        // Repeatedly call nanosleep until the sleep duration is fully elapsed
        while(nanosleep(&req, &req) == -1)
            continue;
    }

    /**
     * @brief Renders the contents of the Frame_Buffer to the standard output (STDOUT).
     * @details This function moves the cursor to the top-left corner of the terminal, flushes the output
     *          buffer to ensure immediate writing, and writes the contents of the Frame_Buffer to STDOUT.
     *          If the write operation fails or writes fewer bytes than expected, an error message is reported.
     */
    void Render_Frame() {
        // Move the cursor to the top-left corner of the terminal
        printf("%s", Constants::ANSI::SET_CURSOR_TO_START.c_str());

        // Flush the output buffer to ensure it's written immediately
        fflush(stdout);

        // Write the contents of Frame_Buffer to STDOUT
        int Error = write(STDOUT_FILENO, Frame_Buffer.data(), Frame_Buffer.size());

        // Check for write errors or incomplete writes
        if (Error != (signed)Frame_Buffer.size()) {
            Report("Failed to write to STDOUT: " + std::to_string(Error));
        }
    }

    /**
     * @brief Updates the maximum width and height of the terminal.
     * @details This function updates the Max_Width and Max_Height variables by calling ioctl to get the current
     *          width and height of the terminal. If the call fails, a report message is sent.
     * @note The height is reduced by 1 to account for the one line of console space taken by the GGUI status bar.
     */
    void Update_Max_Width_And_Height(){
        struct winsize w;
        if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &w) == -1){
            Report("Failed to get console info!");
            return;
        }

        Max_Width = w.ws_col;
        Max_Height = w.ws_row - 1;

        // Convenience sake :)
        if (Main)
            Main->Set_Dimensions(Max_Width, Max_Height);
    }

    /**
     * @brief Adds a signal handler to automatically update the terminal size whenever a SIGWINCH signal is received.
     * @details This function sets up a signal handler to detect when the terminal size changes and updates the
     *          Carry_Flags to indicate that a resize is needed. This is necessary because the render thread needs
     *          to update the maximum width and height of the terminal whenever a resize occurs.
     */
    void Add_Automatic_Terminal_Size_Update_Handler(){
        // To automatically update GGUI max dimensions, we need to make an WINCH Signal handler.
        struct sigaction Handler;
        
        // Setup the function handler with a lambda
        Handler.sa_handler = [](int signum){
            // When the signal is received, update the carry flags to indicate that a resize is needed
            Carry_Flags([](GGUI::Carry& current_carry){
                current_carry.Resize = true;    // Tell the render thread that an resize is needed to be performed.
            });
        };

        // Clears any other handler which could potentially hinder this handler.
        sigemptyset(&Handler.sa_mask);

        // Since sigaction flags does not get auto constructed, we need to clean it.
        Handler.sa_flags = 0;

        // Now set this handler up.
        sigaction(SIGWINCH, &Handler, nullptr);
    }

    namespace Constants{
        namespace ANSI{
            constexpr char START_OF_CTRL = 1;
            constexpr char END_OF_CTRL = 26;
        }
    };

    /**
     * @brief Takes in an buffer with hex and octal values and transforms them into printable strings
     * @details This function takes in a buffer with hex and octal values and transforms them into printable strings
     *          for better visualization of the buffer contents.
     *
     * @param buffer The buffer to transform into a printable string
     * @param length The length of the buffer
     * @param Obfuscate A boolean flag to control whether the buffer contents should be obfuscated or not. If set to true,
     *                  any non-printable characters in the buffer will be replaced with a space.
     *
     * @return A string containing the transformed buffer contents
     */
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


    /**
     * @brief Reverse-engineers termios raw terminal keybinds.
     * @param keybind_value The value of the key to reverse-engineer.
     * @return The reversed keybind value.
     * @details This function is used to reverse-engineer termios raw terminal keybinds. It checks if the keybind is in the known keybinding table.
     *          If it is, it returns the reversed keybind value. If not, it returns the normal value.
     */
    char Reverse_Engineer_Keybinds(char keybind_value){

        // SHIFT + TAB => \e[Z
        if (keybind_value == 'Z'){
            // Set the shift key state to true
            KEYBOARD_STATES[BUTTON_STATES::SHIFT] = BUTTON_STATE(true);
            // Set the tab key state to true
            KEYBOARD_STATES[BUTTON_STATES::TAB] = BUTTON_STATE(true);

            // Set the keybind value to 0, to indicate that it has been reversed
            keybind_value = 0;
        }

        return keybind_value;
    }


    /**
     * @brief Waits for user input and stores it in the Raw_Input array.
     * @details This function waits for user input and stores it in the Raw_Input array. It is called from the event loop.
     *          It is also the function that is called as soon as possible and gets stuck awaiting for the user input.
     */
    void Query_Inputs(){
        // Add the previous input size into the current offset for cumulative reading.
        unsigned char* Current_Input_Buffer_Location = Raw_Input + Raw_Input_Size;

        // Ceil the result, so that negative numbers wont start overflow.
        int Current_Input_Buffer_Capacity = Max(Raw_Input_Capacity - Raw_Input_Size, Raw_Input_Capacity);

        Raw_Input_Size = read(
            STDIN_FILENO,
            Current_Input_Buffer_Location,
            Current_Input_Buffer_Capacity
        );
    }

    /**
     * @brief Translate the input events stored in Raw_Input into Input objects.
     * @details This function is used to translate the input events stored in Raw_Input into Input objects. It checks if the event is a key event, and if so, it checks if the key is a special key (up, down, left, right, enter, shift, control, backspace, escape, tab) and if so, it creates an Input object with the corresponding Constants:: value. If the key is not a special key, it creates an Input object with the key's ASCII value and Constants::KEY_PRESS. If the event is not a key event, it checks if the event is a mouse event and if so, it checks if the mouse event is a movement, click or scroll event and if so, it creates an Input object with the corresponding Constants:: value. Finally, it resets the Raw_Input_Size to 0.
     */
    void Translate_Inputs(){
        // Clean the keyboard states.
        PREVIOUS_KEYBOARD_STATES = KEYBOARD_STATES;

        // Unlike in Windows we wont be getting an indication per Key information, whether it was pressed in or out.
        KEYBOARD_STATES.clear();

        // All of the if-else statements could just make into a map, where each key of combination replaces the value of the keyboard state, but you know what?
        // haha code go brrrr -- 2024 gab here, nice joke, although who asked? 
        for (unsigned int i = 0; i < Raw_Input_Size; i++) {

            // Check if SHIFT has been modifying the keys
            if ((Raw_Input[i] >= 'A' && Raw_Input[i] <= 'Z') || (Raw_Input[i] >= '!' && Raw_Input[i] <= '/')) {
                // SHIFT key is pressed
                Inputs.push_back(new GGUI::Input(' ', Constants::SHIFT));
                KEYBOARD_STATES[BUTTON_STATES::SHIFT] = BUTTON_STATE(true);
            }

            // We now can also check if the letter has been shifted down by CTRL key
            else if (Raw_Input[i] >= Constants::ANSI::START_OF_CTRL && Raw_Input[i] <= Constants::ANSI::END_OF_CTRL) {
                // This is a CTRL key

                // The CTRL domain contains multiple useful keys to check for
                if (Raw_Input[i] == Constants::ANSI::BACKSPACE) {
                    // This is a backspace key
                    Inputs.push_back(new GGUI::Input(' ', Constants::BACKSPACE));
                    KEYBOARD_STATES[BUTTON_STATES::BACKSPACE] = BUTTON_STATE(true);
                }
                else if (Raw_Input[i] == Constants::ANSI::HORIZONTAL_TAB) {
                    // This is a tab key
                    Inputs.push_back(new GGUI::Input(' ', Constants::TAB));
                    KEYBOARD_STATES[BUTTON_STATES::TAB] = BUTTON_STATE(true);
                    Handle_Tabulator();
                }
                else if (Raw_Input[i] == Constants::ANSI::LINE_FEED) {
                    // This is an enter key
                    Inputs.push_back(new GGUI::Input(' ', Constants::ENTER));
                    KEYBOARD_STATES[BUTTON_STATES::ENTER] = BUTTON_STATE(true);
                }

                // Shift the key back up
                char Offset = 'a' - 1; // We remove one since the CTRL characters started from 1 and not 0
                Raw_Input[i] = Raw_Input[i] + Offset;
                KEYBOARD_STATES[BUTTON_STATES::CONTROL] = BUTTON_STATE(true);
            }

            if (Raw_Input[i] == Constants::ANSI::ESC_CODE[0]) {
                // check if there are stuff after this escape code
                if (i + 1 >= Raw_Input_Size) {
                    // Clearly the escape key was invoked
                    Inputs.push_back(new GGUI::Input(' ', Constants::ESCAPE));
                    KEYBOARD_STATES[BUTTON_STATES::ESC] = BUTTON_STATE(true);
                    Handle_Escape();
                    continue;
                }

                // GG go next
                i++;

                // The current data can either be an ALT key initiative or an escape sequence followed by '['
                if (Raw_Input[i] == Constants::ANSI::ESC_CODE[1]) {
                    // Escape sequence codes:

                    // UP, DOWN LEFT, RIGHT keys
                    if (Raw_Input[i + 1] == 'A') {
                        Inputs.push_back(new GGUI::Input(0, Constants::UP));
                        KEYBOARD_STATES[BUTTON_STATES::UP] = BUTTON_STATE(true);
                        i++;
                    }
                    else if (Raw_Input[i + 1] == 'B') {
                        Inputs.push_back(new GGUI::Input(0, Constants::DOWN));
                        KEYBOARD_STATES[BUTTON_STATES::DOWN] = BUTTON_STATE(true);
                        i++;
                    }
                    else if (Raw_Input[i + 1] == 'C') {
                        Inputs.push_back(new GGUI::Input(0, Constants::RIGHT));
                        KEYBOARD_STATES[BUTTON_STATES::RIGHT] = BUTTON_STATE(true);
                        i++;
                    }
                    else if (Raw_Input[i + 1] == 'D') {
                        Inputs.push_back(new GGUI::Input(0, Constants::LEFT));
                        KEYBOARD_STATES[BUTTON_STATES::LEFT] = BUTTON_STATE(true);
                        i++;
                    }
                    else if (Raw_Input[i + 1] == 'M') {  // Decode Mouse handling
                        // Payload structure: '\e[Mbxy' where the b is bitmask representing the buttons, x and y representing the location of the mouse. 
                        char Bit_Mask = Raw_Input[i + 2];

                        // Check if the bit 2'rd has been set, is so then the SHIFT has been pressed
                        if (Bit_Mask & 4) {
                            Inputs.push_back(new GGUI::Input(' ', Constants::SHIFT));
                            KEYBOARD_STATES[BUTTON_STATES::SHIFT] = BUTTON_STATE(true);
                            // also remove the bit from the bitmask
                            Bit_Mask &= ~4;
                        }

                        // Check if the 3'th bit has been set, is so then the SUPER has been pressed
                        if (Bit_Mask & 8) {
                            Inputs.push_back(new GGUI::Input(' ', Constants::SUPER));
                            KEYBOARD_STATES[BUTTON_STATES::SUPER] = BUTTON_STATE(true);
                            // also remove the bit from the bitmask
                            Bit_Mask &= ~8;
                        }

                        // Check if the 4'th bit has been set, is so then the CTRL has been pressed
                        if (Bit_Mask & 16) {
                            Inputs.push_back(new GGUI::Input(' ', Constants::CONTROL));
                            KEYBOARD_STATES[BUTTON_STATES::CONTROL] = BUTTON_STATE(true);
                            // also remove the bit from the bitmask
                            Bit_Mask &= ~16;
                        }

                        // Bit 5'th is not widely supported so remove it in case.
                        Bit_Mask &= ~32;

                        // Check if the 6'th bit has been set, is so then there is a movement event.
                        if (Bit_Mask & 64) {
                            char X = Raw_Input[i + 3];
                            char Y = Raw_Input[i + 4];

                            // XTERM will normally shift its X and Y coordinates by 32, so that it skips all the control characters in ASCII.
                            Mouse.X = X - 32;
                            Mouse.Y = Y - 32;

                            Bit_Mask &= ~64;
                        }

                        // Bits 7'th are not widely supported. But clear this bit just in case
                        Bit_Mask &= ~(128);

                        if (Bit_Mask == 0) {
                            KEYBOARD_STATES[BUTTON_STATES::MOUSE_LEFT] = BUTTON_STATE(true);
                            KEYBOARD_STATES[BUTTON_STATES::MOUSE_LEFT].Capture_Time = std::chrono::high_resolution_clock::now();
                        }
                        else if (Bit_Mask == 1) {
                            KEYBOARD_STATES[BUTTON_STATES::MOUSE_MIDDLE] = BUTTON_STATE(true);
                            KEYBOARD_STATES[BUTTON_STATES::MOUSE_MIDDLE].Capture_Time = std::chrono::high_resolution_clock::now();
                        }
                        else if (Bit_Mask == 2) {
                            KEYBOARD_STATES[BUTTON_STATES::MOUSE_RIGHT] = BUTTON_STATE(true);
                            KEYBOARD_STATES[BUTTON_STATES::MOUSE_RIGHT].Capture_Time = std::chrono::high_resolution_clock::now();
                        }
                        else if (Bit_Mask == 3) {
                            KEYBOARD_STATES[BUTTON_STATES::MOUSE_LEFT].State = false;
                            KEYBOARD_STATES[BUTTON_STATES::MOUSE_MIDDLE].State = false;
                            KEYBOARD_STATES[BUTTON_STATES::MOUSE_RIGHT].State = false;
                        }

                        i += 4;
                    }
                    else if (Raw_Input[i + 1] == 'Z') {
                        // SHIFT + TAB => Z
                        Inputs.push_back(new GGUI::Input(' ', Constants::SHIFT));
                        Inputs.push_back(new GGUI::Input(' ', Constants::TAB));

                        KEYBOARD_STATES[BUTTON_STATES::SHIFT] = BUTTON_STATE(true);
                        KEYBOARD_STATES[BUTTON_STATES::TAB] = BUTTON_STATE(true);

                        Handle_Tabulator();

                        i++;
                    }

                }
                else {
                    // This is an ALT key
                    Inputs.push_back(new GGUI::Input(Raw_Input[i], Constants::ALT));
                    KEYBOARD_STATES[BUTTON_STATES::ALT] = BUTTON_STATE(true);
                }
            }
            else {
                // Normal character data
                Inputs.push_back(new GGUI::Input(Raw_Input[i], Constants::KEY_PRESS));
            }

        }

        // We can assume that at the end of user input translation, all buffered inputs are hereby translate and no need to store, so reset offset.
        Raw_Input_Size = 0;
    }

    /**
     * @brief Initializes platform-specific settings for console handling.
     * @details This function sets up the console handles and modes required for input and output operations.
     *          It enables mouse and window input, sets UTF-8 mode for output, and prepares the console for
     *          handling specific ANSI features.
     */
    void Init_Platform_Stuff(){
        // Initialize the console for mouse and window input, and set UTF-8 mode for output.
        std::cout << Constants::ANSI::Enable_Private_SGR_Feature(Constants::ANSI::REPORT_MOUSE_ALL_EVENTS).To_String();
        std::cout << Constants::ANSI::Enable_Private_SGR_Feature(Constants::ANSI::MOUSE_CURSOR, false).To_String();
        std::cout << Constants::ANSI::Enable_Private_SGR_Feature(Constants::ANSI::SCREEN_CAPTURE).To_String();   // for on exit to restore
        // std::cout << Constants::RESET_CONSOLE;
        // std::cout << Constants::EnableFeature(Constants::ALTERNATIVE_SCREEN_BUFFER);    // For double buffer if needed
        std::cout << std::flush;

        // Save the current flags and take an snapshot of the flags before GGUI
        Previous_Flags = fcntl(STDIN_FILENO, F_GETFL, 0);

        // Set non-blocking flag
        int flags = O_RDONLY | O_CLOEXEC;
        fcntl(STDIN_FILENO, F_SETFL, flags);

        // Save the current terminal settings
        struct termios Term_Handle;
        tcgetattr(STDIN_FILENO, &Term_Handle);

        Previous_Raw = Term_Handle;

        // Set the terminal to raw mode
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

        // Register the exit handler for the following signals
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
    
        // Add a signal handler to automatically update the terminal size whenever a SIGWINCH signal is received.
        Add_Automatic_Terminal_Size_Update_Handler();
    }


    /**
     * @brief Get a list of all font files in the system.
     *        This function uses the "fc-list" command to get a list of all font files in the system.
     *        The output is then processed to extract the file names and return them as a vector.
     * @return A vector of strings containing all the font file names.
     */
    std::vector<std::string> Get_List_Of_Font_Files() {
        CMD Handle;

        // Run the command and store the output in Raw_Result
        std::string Raw_Result = Handle.Run("fc-list -v | grep file");

        std::vector<std::string> File_Names;
        std::stringstream ss(Raw_Result);
        std::string temp;

        // Process the output line by line
        while (std::getline(ss, temp, '\n')) {
            // Extract the file name from each line and add it to the vector
            std::size_t pos = temp.find(": ");
            if (pos != std::string::npos) {
                std::string file_name = temp.substr(pos + 2);
                File_Names.push_back(file_name);
            }
        }

        return File_Names;
    }

    /**
     * @brief Reports the current stack trace along with a specified problem description.
     *        This function captures the call stack, symbolically resolves the addresses, and formats the stack trace.
     *        The resulting formatted trace is then reported along with the provided problem description.
     * @param Problem A description of the problem to be reported with the stack trace.
     */
    void Report_Stack(std::string Problem) {
        // This is required for GGUI to work in Android, since Androids own STD is primitive in terms of stack tracing.
        // TODO: use Evie to dismantle the running exec stack with name table.
        #ifndef __ANDROID__
            const int Stack_Trace_Depth = 10;
            size_t *Ptr_Table[Stack_Trace_Depth];


            // Declare a pointer to an array of strings. This will hold the symbol names of the stack trace
            char **Name_Table;

            // Get the stack trace and store it in the array. The return value is the number of stack frames obtained
            size_t Usable_Depth = backtrace(reinterpret_cast<void**>(Ptr_Table), Stack_Trace_Depth);

            // Convert the addresses in the stack trace into an array of strings that describe the addresses symbolically
            Name_Table = backtrace_symbols(reinterpret_cast<void**>(Ptr_Table), Usable_Depth);

            if (Max_Width == 0) {
                Update_Max_Width_And_Height();
            }

            // Now that we have the stack frame label names in the Name_Table list, we can construct an visually appeasing stack trace information:
            std::string Result = "Stack Trace:\n";

            bool Use_Indent = Usable_Depth < (Max_Width / 2);
            for (unsigned int Stack_Index = 0; Stack_Index < Usable_Depth; Stack_Index++) {
                std::string Branch_Start = "|"; //SYMBOLS::VERTICAL_RIGHT_CONNECTOR;                // No UNICODE support ATM

                // For last branch use different branch start symbol
                if (Stack_Index == Usable_Depth - 1)
                    Branch_Start = "\\"; //SYMBOLS::BOTTOM_LEFT_CORNER;                             // No UNICODE support ATM

                // now add indentation by the amount of index:
                std::string Indent = "";

                for (unsigned int i = 0; i < Stack_Index && Use_Indent; i++)
                    Indent += "-"; //SYMBOLS::HORIZONTAL_LINE;                                      // No UNICODE support ATM

                Result += Branch_Start + Indent + " " + Name_Table[Stack_Index] + "\n";
            }

            // Free the memory allocated for the string array
            free(Name_Table);

            // now add the problem into the message
            Result += "Problem: " + Problem;

            Report(Result);
        #endif

        Report(Problem);
    }

    #endif

    /**
     * @brief Populate inputs for keys that are held down.
     * @details This function iterates over the current keyboard states and creates new input objects
     *          for keys that are held down and not already present in the inputs list. It skips mouse button keys.
     */
    void Populate_Inputs_For_Held_Down_Keys() {
        for (auto Key : KEYBOARD_STATES) {

            // Check if the key is activated
            if (Key.second.State) {

                // Skip mouse button keys
                if (BUTTON_STATES::MOUSE_LEFT == Key.first || BUTTON_STATES::MOUSE_RIGHT == Key.first || BUTTON_STATES::MOUSE_MIDDLE == Key.first)
                    continue;

                // Get the constant associated with the key
                unsigned long long Constant_Key = BUTTON_STATES_TO_CONSTANTS_BRIDGE.at(Key.first);

                // Check if the input already exists
                bool Found = false;
                for (auto input : Inputs) {
                    if (input->Criteria == Constant_Key) {
                        Found = true;
                        break;
                    }
                }

                // If not found, create a new input
                if (!Found)
                    Inputs.push_back(new Input((char)0, Constant_Key));
            }
        }
    }

    /**
 * @brief Processes mouse input events and updates the input list.
 * @details This function checks the state of mouse buttons (left, right, and middle)
 *          and determines if they have been pressed or clicked. It compares the current
 *          state with the previous state and the duration the button has been pressed.
 *          Based on these checks, it creates corresponding input objects and adds them
 *          to the Inputs list.
 */
void MOUSE_API() {
    // Get the duration the left mouse button has been pressed
    unsigned long long Mouse_Left_Pressed_For = (unsigned long long)std::chrono::duration_cast<std::chrono::milliseconds>(
        abs(Current_Time - KEYBOARD_STATES[BUTTON_STATES::MOUSE_LEFT].Capture_Time)).count();

    // Check if the left mouse button is pressed and for how long
    if (KEYBOARD_STATES[BUTTON_STATES::MOUSE_LEFT].State && Mouse_Left_Pressed_For >= SETTINGS::Mouse_Press_Down_Cooldown) {
        Inputs.push_back(new GGUI::Input(0, Constants::MOUSE_LEFT_PRESSED));
    } 
    // Check if the left mouse button was previously pressed and now released
    else if (!KEYBOARD_STATES[BUTTON_STATES::MOUSE_LEFT].State && PREVIOUS_KEYBOARD_STATES[BUTTON_STATES::MOUSE_LEFT].State != KEYBOARD_STATES[BUTTON_STATES::MOUSE_LEFT].State) {
        Inputs.push_back(new GGUI::Input(0, Constants::MOUSE_LEFT_CLICKED));
    }

    // Get the duration the right mouse button has been pressed
    unsigned long long Mouse_Right_Pressed_For = (unsigned long long)std::chrono::duration_cast<std::chrono::milliseconds>(
        abs(Current_Time - KEYBOARD_STATES[BUTTON_STATES::MOUSE_RIGHT].Capture_Time)).count();

    // Check if the right mouse button is pressed and for how long
    if (KEYBOARD_STATES[BUTTON_STATES::MOUSE_RIGHT].State && Mouse_Right_Pressed_For >= SETTINGS::Mouse_Press_Down_Cooldown) {
        Inputs.push_back(new GGUI::Input(0, Constants::MOUSE_RIGHT_PRESSED));
    }
    // Check if the right mouse button was previously pressed and now released
    else if (!KEYBOARD_STATES[BUTTON_STATES::MOUSE_RIGHT].State && PREVIOUS_KEYBOARD_STATES[BUTTON_STATES::MOUSE_RIGHT].State != KEYBOARD_STATES[BUTTON_STATES::MOUSE_RIGHT].State) {
        Inputs.push_back(new GGUI::Input(0, Constants::MOUSE_RIGHT_CLICKED));
    }

    // Get the duration the middle mouse button has been pressed
    unsigned long long Mouse_Middle_Pressed_For = (unsigned long long)std::chrono::duration_cast<std::chrono::milliseconds>(
        abs(Current_Time - KEYBOARD_STATES[BUTTON_STATES::MOUSE_MIDDLE].Capture_Time)).count();

    // Check if the middle mouse button is pressed and for how long
    if (KEYBOARD_STATES[BUTTON_STATES::MOUSE_MIDDLE].State && Mouse_Middle_Pressed_For >= SETTINGS::Mouse_Press_Down_Cooldown) {
        Inputs.push_back(new GGUI::Input(0, Constants::MOUSE_MIDDLE_PRESSED));
    }
    // Check if the middle mouse button was previously pressed and now released
    else if (!KEYBOARD_STATES[BUTTON_STATES::MOUSE_MIDDLE].State && PREVIOUS_KEYBOARD_STATES[BUTTON_STATES::MOUSE_MIDDLE].State != KEYBOARD_STATES[BUTTON_STATES::MOUSE_MIDDLE].State) {
        Inputs.push_back(new GGUI::Input(0, Constants::MOUSE_MIDDLE_CLICKED));
    }
}

    /**
     * @brief Handles mouse scroll events.
     * @details This function checks if the mouse scroll up or down button has been pressed and if the focused element is not null.
     *          If the focused element is not null, it calls the scroll up or down function on the focused element.
     */
    void SCROLL_API(){
        // Check if the mouse scroll up button has been pressed
        if (KEYBOARD_STATES[BUTTON_STATES::MOUSE_SCROLL_UP].State){

            // If the focused element is not null, call the scroll up function
            if (Focused_On)
                Focused_On->Scroll_Up();
        }
        // Check if the mouse scroll down button has been pressed
        else if (KEYBOARD_STATES[BUTTON_STATES::MOUSE_SCROLL_DOWN].State){

            // If the focused element is not null, call the scroll down function
            if (Focused_On)
                Focused_On->Scroll_Down();
        }
    }

    /**
     * @brief Handles escape key press events.
     * @details This function checks if the escape key has been pressed and if the focused element is not null.
     *          If the focused element is not null, it calls the Un_Focus_Element function to remove the focus.
     *          If the focused element is null but the hovered element is not null, it calls the Un_Hover_Element
     *          function to remove the hover.
     */
    void Handle_Escape(){
        // Check if the escape key has been pressed
        if (!KEYBOARD_STATES[BUTTON_STATES::ESC].State)
            return;

        // If the focused element is not null, remove the focus
        if (Focused_On){
            Hovered_On = Focused_On;
            Un_Focus_Element();
        }
        // If the focused element is null but the hovered element is not null, remove the hover
        else if (Hovered_On){
            Un_Hover_Element();
        }
    }

    /**
     * @brief Handles the pressing of the tab key.
     * @details This function selects the next tabbed element as focused and not hovered.
     *          If the shift key is pressed, it goes backwards in the list of tabbed elements.
     */
    void Handle_Tabulator(){
        // Check if the tab key has been pressed
        if (!KEYBOARD_STATES[BUTTON_STATES::TAB].State)
            return;

        // Check if the shift key is pressed
        bool Shift_Is_Pressed = KEYBOARD_STATES[BUTTON_STATES::SHIFT].State;

        // Get the current element from the selected element
        Element* Current = Focused_On;

        // If there has not been anything selected then then skip this phase and default to zero.
        if (!Current)
            Current = Hovered_On;

        int Current_Index = 0;

        // Find the index of the current element in the list of event handlers
        if (Current)
            for (;(unsigned int)Current_Index < Event_Handlers.size(); Current_Index++){
                if (Event_Handlers[Current_Index]->Host == Current)
                    break;
            }

        // Generalize index hopping, if shift is pressed then go backwards.
        Current_Index += 1 + (-2 * Shift_Is_Pressed);

        // If the index is out of bounds, wrap it around to the other side of the list
        if (Current_Index < 0){
            Current_Index = Event_Handlers.size() - 1;
        }
        else if ((unsigned int)Current_Index >= Event_Handlers.size()){
            Current_Index = 0;
        }

        // Now update the focused element with the new index
        Un_Hover_Element();
        Update_Focused_Element(Event_Handlers[Current_Index]->Host);
    }

    /**
     * @brief Checks if a bit is set in a char.
     * @details This function takes a char and an index as input and checks if the bit at the specified index is set.
     *          It returns true if the bit is set and false if it is not.
     *
     * @param val The char to check the bit in.
     * @param i The index of the bit to check.
     *
     * @return True if the bit is set, false if it is not.
     */
    bool Has_Bit_At(char val, int i){
        // Shift the bit to the right i times and then check if the bit is set.
        return ((val) & (1<<(i))) != 0;
    }

    // Returns the length of a Unicode character based on the first byte.
    // @param first_char The first byte of the character.
    // @return The length of the character in bytes. Returns 1 if it is not a Unicode character.
    int Get_Unicode_Length(char first_char) {
        // Check if the character is an ASCII character (0xxxxxxx)
        if (!Has_Bit_At(first_char, 7)) // ASCII characters have the most significant bit as 0
            return 1;

        // Check if the character is a 2-byte Unicode character (110xxxxx)
        if (Has_Bit_At(first_char, 7) && Has_Bit_At(first_char, 6) && !Has_Bit_At(first_char, 5))
            return 2;

        // Check if the character is a 3-byte Unicode character (1110xxxx)
        if (Has_Bit_At(first_char, 7) && Has_Bit_At(first_char, 6) && Has_Bit_At(first_char, 5) && !Has_Bit_At(first_char, 4))
            return 3;

        // Check if the character is a 4-byte Unicode character (11110xxx)
        if (Has_Bit_At(first_char, 7) && Has_Bit_At(first_char, 6) && Has_Bit_At(first_char, 5) && Has_Bit_At(first_char, 4) && !Has_Bit_At(first_char, 3))
            return 4;

        // Default case: return 1 for invalid or unexpected byte patterns
        return 1;
    }

    /**
     * @brief Gets the current maximum width of the terminal.
     * @details This function returns the current maximum width of the terminal. If the width is 0, it will set the carry flag to indicate that a resize is needed to be performed.
     *
     * @return The current maximum width of the terminal.
     */
    int Get_Max_Width(){
        if (Max_Width == 0 && Max_Height == 0){
            Carry_Flags([](GGUI::Carry& current_carry){
                current_carry.Resize = true;    // Tell the render thread that an resize is needed to be performed.
            });
        }
        
        return Max_Width;
    }


    /**
     * @brief Gets the current maximum height of the terminal.
     * @details This function returns the current maximum height of the terminal. If the height is 0, it will set the carry flag to indicate that a resize is needed to be performed.
     *
     * @return The current maximum height of the terminal.
     */
    int Get_Max_Height(){
        if (Max_Width == 0 && Max_Height == 0){
            Carry_Flags([](GGUI::Carry& current_carry){
                current_carry.Resize = true;    // Tell the render thread that an resize is needed to be performed.
            });
        }

        return Max_Height;
    }

    /**
     * @brief Gets the contents of a given position in the buffer.
     * @details This function takes a position in the buffer and returns the contents of that position. If the position is out of bounds, it will return nullptr.
     * @param Absolute_Position The position to get the contents of.
     * @return The contents of the given position, or nullptr if the position is out of bounds.
     */
    GGUI::UTF* Get(GGUI::IVector3 Absolute_Position){
        if (Absolute_Position.X >= Get_Max_Width() || 
            Absolute_Position.Y >= Get_Max_Height() ||
            Absolute_Position.X < 0 || 
            Absolute_Position.Y < 0)
        {
            // The position is out of bounds, return nullptr
            return nullptr;
        }
        else{
            // The position is in bounds, return the contents of that position
            return &Abstract_Frame_Buffer[Absolute_Position.Y * Get_Max_Width() + Absolute_Position.X];
        }
    }

    namespace INTERNAL{
        static Super_String LIQUIFY_UTF_TEXT_RESULT_CACHE;
        static Super_String LIQUIFY_UTF_TEXT_TMP_CONTAINER(Constants::ANSI::Maximum_Needed_Pre_Allocation_For_Encoded_Super_String);
        static Super_String LIQUIFY_UTF_TEXT_TEXT_OVERHEAD(Constants::ANSI::Maximum_Needed_Pre_Allocation_For_Over_Head);
        static Super_String LIQUIFY_UTF_TEXT_BACKGROUND_OVERHEAD(Constants::ANSI::Maximum_Needed_Pre_Allocation_For_Over_Head);
        static Super_String LIQUIFY_UTF_TEXT_TEXT_COLOUR(Constants::ANSI::Maximum_Needed_Pre_Allocation_For_Color);
        static Super_String LIQUIFY_UTF_TEXT_BACKGROUND_COLOUR(Constants::ANSI::Maximum_Needed_Pre_Allocation_For_Color);
    }

    /**
     * @brief Converts a vector of UTFs into a Super_String.
     * @details This function takes a vector of UTFs, and converts it into a Super_String. The resulting Super_String is stored in a cache, and the cache is resized if the window size has changed.
     * @param Text The vector of UTFs to convert.
     * @param Width The width of the window.
     * @param Height The height of the window.
     * @return A pointer to the resulting Super_String.
     */
    GGUI::Super_String* Liquify_UTF_Text(std::vector<GGUI::UTF>& Text, int Width, int Height){
        const unsigned int Maximum_Needed_Pre_Allocation_For_Whole_Cache_Buffer = (Width * Height * Constants::ANSI::Maximum_Needed_Pre_Allocation_For_Encoded_Super_String + SETTINGS::Word_Wrapping * (Height - 1));
        
        // Since they are located as globals we need to remember to restart the starting offset.
        INTERNAL::LIQUIFY_UTF_TEXT_RESULT_CACHE.Clear();
        INTERNAL::LIQUIFY_UTF_TEXT_TMP_CONTAINER.Clear();
        INTERNAL::LIQUIFY_UTF_TEXT_TEXT_OVERHEAD.Clear();
        INTERNAL::LIQUIFY_UTF_TEXT_BACKGROUND_OVERHEAD.Clear();
        INTERNAL::LIQUIFY_UTF_TEXT_TEXT_COLOUR.Clear();
        INTERNAL::LIQUIFY_UTF_TEXT_BACKGROUND_COLOUR.Clear();
        
        // We need to dynamically resize this, since the window size will be potentially re-sized.
        if (INTERNAL::LIQUIFY_UTF_TEXT_RESULT_CACHE.Data.capacity() != Maximum_Needed_Pre_Allocation_For_Whole_Cache_Buffer){
            INTERNAL::LIQUIFY_UTF_TEXT_RESULT_CACHE = Super_String(Maximum_Needed_Pre_Allocation_For_Whole_Cache_Buffer);
        }

        Super_String* Result = &INTERNAL::LIQUIFY_UTF_TEXT_RESULT_CACHE;
 
        for (int y = 0; y < Height; y++){
            for (int x = 0; x < Width; x++){
                Text[y * Width + x].To_Encoded_Super_String(
                    &INTERNAL::LIQUIFY_UTF_TEXT_TMP_CONTAINER,
                    &INTERNAL::LIQUIFY_UTF_TEXT_TEXT_OVERHEAD,
                    &INTERNAL::LIQUIFY_UTF_TEXT_BACKGROUND_OVERHEAD,
                    &INTERNAL::LIQUIFY_UTF_TEXT_TEXT_COLOUR,
                    &INTERNAL::LIQUIFY_UTF_TEXT_BACKGROUND_COLOUR
                );
                
                Result->Add(INTERNAL::LIQUIFY_UTF_TEXT_TMP_CONTAINER, true);

                // now instead of emptying the Super_String.vector, we can reset the current index into 0 again.
                INTERNAL::LIQUIFY_UTF_TEXT_TMP_CONTAINER.Clear();
                INTERNAL::LIQUIFY_UTF_TEXT_TEXT_OVERHEAD.Clear();
                INTERNAL::LIQUIFY_UTF_TEXT_BACKGROUND_OVERHEAD.Clear();   
                INTERNAL::LIQUIFY_UTF_TEXT_TEXT_COLOUR.Clear();
                INTERNAL::LIQUIFY_UTF_TEXT_BACKGROUND_COLOUR.Clear();
            }

            // the system doesn't have word wrapping enabled then, use newlines as replacement.
            if (!SETTINGS::Word_Wrapping){
                Result->Add('\n');   // the system is word wrapped.
            }
        }

        return Result;
    }

    /**
     * @brief Updates the frame.
     * @details This function updates the frame. It's the main entry point for the rendering thread.
     * @note This function will return immediately if the rendering thread is paused.
     */
    void Update_Frame(){
        std::unique_lock lock(Atomic::Mutex);

        // Check if the rendering thread is paused.
        if (Atomic::Pause_Render_Thread == Atomic::Status::LOCKED)
            return;

        // Give the rendering thread one ticket.
        Atomic::Pause_Render_Thread = Atomic::Status::RESUMED;

        // Notify all waiting threads that the frame has been updated.
        Atomic::Condition.notify_all();
    }

    /**
     * @brief Pauses the rendering thread.
     * @details This function pauses the rendering thread. The thread will wait until the rendering thread is resumed.
     */
    void Pause_GGUI(){
        std::unique_lock lock(Atomic::Mutex);

        // Set the render status to locked.
        Atomic::Pause_Render_Thread = Atomic::Status::LOCKED;

        // Wait for the rendering thread to become available.
        Atomic::Condition.wait_for(lock, GGUI::SETTINGS::Thread_Timeout, []{
            // If the rendering thread is not locked, then the wait is over.
            return Atomic::Pause_Render_Thread == Atomic::Status::LOCKED;
        });
    }

    /**
     * @brief Resumes the rendering thread.
     * @details This function resumes the rendering thread after it has been paused.
     * @param restore_render_to The status to restore the rendering thread to.
     */
    void Resume_GGUI(Atomic::Status restore_render_to){
        {
            // Local scope to set the new render status.
            std::unique_lock lock(Atomic::Mutex);
            // Set the render status to the given status.
            Atomic::Pause_Render_Thread = restore_render_to;
        }

        // Check if the rendering status is anything but locked.
        if (restore_render_to < Atomic::Status::LOCKED){
            // If it's not locked, then update the frame.
            Update_Frame();
        }
    }

    /**
     * @brief Calculates the current load of the GGUI thread based on the given current position.
     * @param Min The minimum value the load can have.
     * @param Max The maximum value the load can have.
     * @param Position The current position of the load.
     * @return The current load of the GGUI thread from 0 to 1.
     */
    float Lerp(int Min, int Max, int Position){
        // First calculate the possible length of which our load can represent on.
        float Length_Of_Possible_Values = Max - Min;

        // Get the offset of which the wanted target fps deviates from the minimum value
        float Offset_Of_Our_Load = GGUI::Max(Position - Min, 0);

        // Calculate the simple probability.
        return 1 - Offset_Of_Our_Load / Length_Of_Possible_Values;
    }

    /**
     * @brief This function is a helper for the smart memory system to recall which tasks should be prolonged, and which should be deleted.
     * @details This function is a lambda function that is used by the Atomic::Guard class to prolong or delete memories in the smart memory system.
     *          It takes a pointer to a vector of Memory objects and prolongs or deletes the memories in the vector based on the time difference between the current time and the memory's start time.
     */
    void Recall_Memories(){
        Remember([](std::vector<Memory>& rememberable){
            std::chrono::high_resolution_clock::time_point Current_Time = std::chrono::high_resolution_clock::now();

            // For smart memory system to shorten the next sleep time to arrive at the perfect time for the nearest memory.
            size_t Shortest_Time = MAX_UPDATE_SPEED;
            // Prolong prolongable memories.
            for (unsigned int i = 0; i < rememberable.size(); i++){
                for (unsigned int j = i + 1; j < rememberable.size(); j++){
                    if (rememberable[i].Is(MEMORY_FLAGS::PROLONG_MEMORY) && rememberable[j].Is(MEMORY_FLAGS::PROLONG_MEMORY) && i != j)
                        // Check if the Job at I is same as the one at J.
                        if (rememberable[i].Job.target<bool(*)(GGUI::Event*)>() == rememberable[j].Job.target<bool(*)(GGUI::Event*)>()){
                            // Since J will always be one later than I, J will contain the prolonging memory if there is one. 
                            rememberable[i].Start_Time = rememberable[j].Start_Time;

                            rememberable.erase(rememberable.begin() + j--);
                            break;
                        }
                }
            }

            for (unsigned int i = 0; i < rememberable.size(); i++){
                //first calculate the time difference between the start if the task and the end task
                size_t Time_Difference = std::chrono::duration_cast<std::chrono::milliseconds>(Current_Time - rememberable[i].Start_Time).count();

                size_t Time_Left = rememberable[i].End_Time - Time_Difference;

                if (Time_Left < Shortest_Time)
                    Shortest_Time = Time_Left;

                //if the time difference is greater than the time limit, then delete the memory
                if (Time_Difference > rememberable[i].End_Time){
                    try{
                        bool Success = rememberable[i].Job((Event*)&rememberable[i]);

                        // If job is a re-trigger it will ignore whether the job was successful or not.
                        if (rememberable[i].Is(MEMORY_FLAGS::RETRIGGER)){

                            // May need to change this into more accurate version of time capturing.
                            rememberable[i].Start_Time = Current_Time;

                        }
                        else if (Success){
                            rememberable.erase(rememberable.begin() + i);

                            i--;
                        }
                    }
                    catch (std::exception& e){
                        Report("In memory: '" + rememberable[i].ID + "' Problem: " + std::string(e.what()));
                    }
                }

            }

            Event_Thread_Load = Lerp(MIN_UPDATE_SPEED, MAX_UPDATE_SPEED, Shortest_Time);
        });
    }


    /**
     * @brief Checks if the given flag is set in the given flags.
     * @details This function takes two unsigned long long parameters, one for the flags and one for the flag to check. It returns true if the flag is set in the flags, otherwise it returns false.
     *
     * @param f The flags to check.
     * @param Flag The flag to check for.
     * @return True if the flag is set, otherwise false.
     */
    bool Is(unsigned long long f, unsigned long long Flag){
        return (f & Flag) == Flag;
    }

    /**
     * @brief Checks if a flag is set in a set of flags.
     * @details This function takes two unsigned long long parameters, one for the flags and one for the flag to check. It returns true if the flag is set in the flags, otherwise it returns false.
     *
     * @param f The flags to check.
     * @param flag The flag to check for.
     * @return True if the flag is set, otherwise false.
     */
    bool Has(unsigned long long f, unsigned long long flag){
        return (f & flag) != 0;
    }


    /**
     * @brief Checks if all flags in small are set in big.
     * @details This function takes two unsigned long long parameters, one for the flags to check and one for the flags to check against. It returns true if all flags in small are set in big, otherwise it returns false.
     *
     * @param big The flags to check against.
     * @param small The flags to check.
     * @return True if all flags in small are set in big, otherwise false.
     */
    bool Contains(unsigned long long big, unsigned long long Small) {
        return (Small & big) == Small;
    }


    /**
     * @brief Recursively applies or removes focus on an element and its children.
     * @details This function checks if the current element is an event handler.
     *          If not, it sets the focus state on the element and recurses on its children.
     *          Focus is only applied if the element's current focus state differs from the desired state.
     * 
     * @param current The current element to apply or remove focus.
     * @param Focus The desired focus state.
     */
    void Recursively_Apply_Focus(Element* current, bool Focus){
        // Flag to determine if the current element is an event handler
        bool Is_An_Event_handler = false;

        // Check if the current element is an event handler
        for (auto i : Event_Handlers){
            if (i->Host == current){
                Is_An_Event_handler = true;
                break;
            }
        } 

        // If the element is an event handler and the focus state is unchanged, return
        if (Is_An_Event_handler && current->Is_Focused() != Focus)
            return;

        // Set the focus state on the current element
        current->Set_Focus(Focus);

        // Recurse on all child elements
        for (auto c : current->Get_Childs()){
            Recursively_Apply_Focus(c, Focus);
        }
    }

    /**
     * @brief Recursively applies or removes hover state on an element and its children.
     * @details This function checks if the current element is an event handler.
     *          If not, it sets the hover state on the element and recurses on its children.
     *          Hover is only applied if the element's current hover state differs from the desired state.
     * 
     * @param current The current element to apply or remove hover.
     * @param Hover The desired hover state.
     */
    void Recursively_Apply_Hover(Element* current, bool Hover){
        // check if the current element is one of the Event handlers, if not, then apply the focus buff.
        bool Is_An_Event_handler = false;

        for (auto i : Event_Handlers){
            if (i->Host == current){
                Is_An_Event_handler = true;
                break;
            }
        }

        if (Is_An_Event_handler && current->Is_Hovered() != Hover)
            return;

        current->Set_Hover_State(Hover);

        // Recurse on all child elements
        for (auto c : current->Get_Childs()){
            Recursively_Apply_Hover(c, Hover);
        }
    }

    /**
     * @brief Removes focus from the currently focused element and its children.
     * @details This function checks if there is a currently focused element.
     *          If there is, it sets the focus state on the element and its children to false.
     *          Focus is only removed if the element's current focus state differs from the desired state.
     */
    void Un_Focus_Element(){
        if (!Focused_On)
            return;

        Focused_On->Set_Focus(false);

        // Recursively remove focus from all child elements
        Recursively_Apply_Focus(Focused_On, false);

        Focused_On = nullptr;
    }

    /**
     * @brief Removes the hover state from the currently hovered element and its children.
     * @details This function checks if there is a currently hovered element.
     *          If there is, it sets the hover state on the element and its children to false.
     *          Hover is only removed if the element's current hover state differs from the desired state.
     */
    void Un_Hover_Element(){
        if (!Hovered_On)
            return;

        // Set the hover state to false on the currently hovered element
        Hovered_On->Set_Hover_State(false);

        // Recursively remove the hover state from all child elements
        Recursively_Apply_Hover(Hovered_On, false);

        // Set the hovered element to nullptr to indicate there is no currently hovered element
        Hovered_On = nullptr;
    }


    /**
     * @brief Updates the currently focused element to a new candidate.
     * @details This function checks if the new candidate is the same as the current focused element.
     *          If not, it removes the focus from the current element and all its children.
     *          Then, it sets the focus on the new candidate element and all its children.
     * @param new_candidate The new element to focus on.
     */
    void Update_Focused_Element(GGUI::Element* new_candidate){
        if (Focused_On == new_candidate || new_candidate == Main)
            return;

        // Unfocus the previous focused element and its children
        if (Focused_On){
            Un_Focus_Element();
        }

        // Set the focus on the new element and all its children
        Focused_On = new_candidate;

        // Set the focus state on the new element to true
        Focused_On->Set_Focus(true);

        // Recursively set the focus state on all child elements to true
        Recursively_Apply_Focus(Focused_On, true);
    }

    /**
     * @brief Updates the currently hovered element to a new candidate.
     * @details This function checks if the new candidate is the same as the current hovered element.
     *          If not, it removes the hover state from the current element and all its children.
     *          Then, it sets the hover state on the new candidate element and all its children.
     * @param new_candidate The new element to hover on.
     */
    void Update_Hovered_Element(GGUI::Element* new_candidate){
        if (Hovered_On == new_candidate || new_candidate == Main)
            return;

        // Remove the hover state from the previous hovered element and its children
        if (Hovered_On){
            Un_Hover_Element();
        }

        // Set the hover state on the new element and all its children
        Hovered_On = new_candidate;

        // Set the hover state on the new element to true
        Hovered_On->Set_Hover_State(true);

        // Recursively set the hover state on all child elements to true
        Recursively_Apply_Hover(Hovered_On, true);
    }

    /**
     * @brief Handles all events in the system.
     * @details This function goes through all event handlers and checks if the event criteria matches any of the inputs.
     *          If a match is found, it calls the event handler job with the input as an argument.
     *          If the job is successful, it removes the input from the list of inputs.
     *          If the job is unsuccessful, it reports an error.
     */
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
            // Un-hosted branches
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

                //check if this job could be run successfully.
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

    /**
     * Get the ID of a class by name, assigning a new ID if it doesn't exist.
     * 
     * @param n The name of the class.
     * @return The ID of the class.
     */
    int Get_Free_Class_ID(std::string n){
        // Check if the class name is already in the map
        if (Class_Names.find(n) != Class_Names.end()){
            // Return the existing class ID
            return Class_Names[n];
        }
        else{
            // Assign a new class ID as the current size of the map
            Class_Names[n] = Class_Names.size();

            // Return the newly assigned class ID
            return Class_Names[n];
        }
    }

    /**
     * @brief Adds a new class with the specified name and styling.
     * @details This function retrieves a unique class ID for the given name.
     *          It then associates the provided styling with this class ID 
     *          in the `Classes` map.
     * 
     * @param name The name of the class.
     * @param Styling The styling to be associated with the class.
     */
    void Add_Class(std::string name, Styling Styling){
        Classes([name, Styling](auto& classes){
            // Obtain a unique class ID for the given class name
            int Class_ID = Get_Free_Class_ID(name);

            // Associate the styling with the obtained class ID
            classes[Class_ID] = Styling;
        }); 
    }

    /**
     * @brief Iterates through all file stream handles and triggers change events.
     * @details This function goes through each file stream handle in the `File_Streamer_Handles` map.
     *          It checks if the handle is not a standard output stream, and if so, calls the `Changed` method
     *          on the file stream to trigger any associated change events.
     */
    void Go_Through_File_Streams(){
        for (auto& File_Handle : File_Streamer_Handles){
            // Check if the file handle is not a standard output stream
            if (!File_Handle.second->Is_Cout_Stream()) {
                // Trigger change event for the file stream
                File_Handle.second->Changed();
            }
        }
    }

    /// @brief Refreshes all multi-frame canvases by updating their animation frames.
    /// @details This function iterates over all registered multi-frame canvases,
    ///          advances their animation to the next frame, and flushes their state.
    ///          Additionally, it adjusts the event thread load based on the number
    ///          of canvases that require updates.
    void Refresh_Multi_Frame_Canvas() {
        // Iterate over each multi-frame canvas
        for (auto i : Multi_Frame_Canvas) {
            // Advance the animation to the next frame
            i.first->Set_Next_Animation_Frame();

            // Flush the updated state of the canvas
            i.first->Flush(true);
        }

        // Adjust the event thread load if there are canvases to update
        if (Multi_Frame_Canvas.size() > 0) {
            Event_Thread_Load = Lerp(MIN_UPDATE_SPEED, MAX_UPDATE_SPEED, TIME::MILLISECOND * 16);
        }
    }

    /**
     * @brief Initializes the start addresses for stack and heap.
     * 
     * This function is made extern to prevent inlining. It is responsible
     * for capturing and initializing the nearest stack and heap addresses 
     * and assigning them to the respective global variables.
     */
    extern void Init_Start_Addresses();

    /**
     * @brief Initializes the start addresses for stack and heap.
     * 
     * This function is made extern to prevent inlining. It is responsible
     * for capturing and initializing the nearest stack and heap addresses 
     * and assigning them to the respective global variables.
     */
    extern void Init_Start_Addresses();

    /**
     * @brief Initializes the GGUI system and returns the main window.
     * 
     * @return The main window of the GGUI system.
     */
    GGUI::Window* Init_GGUI(){
        Init_Start_Addresses();

        Update_Max_Width_And_Height();
        
        if (Max_Height == 0 || Max_Width == 0){
            Report("Width/Height is zero!");
            return nullptr;
        }

        // Save the state before the init
        Current_Time = std::chrono::high_resolution_clock::now();
        Previous_Time = Current_Time;

        Init_Platform_Stuff();

        // Set the Main to be anything but nullptr, since its own constructor will try anchor it otherwise.
        Main = (Window*)0xFFFFFFFF;
        Main = new Window("", Styling(width(Max_Width) | height(Max_Height)));

        std::thread Rendering_Scheduler([&](){
            while (true){
                {
                    std::unique_lock lock(Atomic::Mutex);
                    Atomic::Condition.wait(lock, [&](){ return Atomic::Pause_Render_Thread == Atomic::Status::RESUMED; });

                    Atomic::Pause_Render_Thread = Atomic::Status::LOCKED;
                }

                // Save current time, we have the right to overwrite unto the other thread, since they always run after each other and not at same time.
                Previous_Time = std::chrono::high_resolution_clock::now();

                if (Main){

                    // Process the previous carry flags
                    Carry_Flags([](GGUI::Carry& previous_carry){
                        if (previous_carry.Resize){
                            // Clear the previous carry flag
                            previous_carry.Resize = false;

                            Update_Max_Width_And_Height();
                        }
                    });

                    Abstract_Frame_Buffer = Main->Render();

                    // ENCODE for optimize
                    Encode_Buffer(Abstract_Frame_Buffer);

                    Frame_Buffer = Liquify_UTF_Text(Abstract_Frame_Buffer, Main->Get_Width(), Main->Get_Height())->To_String();
                    
                    Render_Frame();
                }

                // Check the difference of the time captured before render and now after render
                Current_Time = std::chrono::high_resolution_clock::now();

                Render_Delay = std::chrono::duration_cast<std::chrono::milliseconds>(Current_Time - Previous_Time).count();

                {
                    std::unique_lock lock(Atomic::Mutex);
                    // Now for itself set it to sleep.
                    Atomic::Pause_Render_Thread = Atomic::Status::PAUSED;
                    Atomic::Condition.notify_all();
                }
            }
        });

        Init_Inspect_Tool();

        std::thread Event_Scheduler([&](){
            while (true){
                Pause_GGUI();

                // Reset the thread load counter
                Event_Thread_Load = 0;
                Previous_Time = std::chrono::high_resolution_clock::now();

                // Order independent --------------
                Recall_Memories();
                Go_Through_File_Streams();
                Refresh_Multi_Frame_Canvas();
                // --------------
                
                /* 
                    Notice: Since the Rendering thread will use its own access to render as tickets, so every time it is "RESUMED" it will after its own run set itself to PAUSED.
                    This is what Tickets are.
                    So in other words, if there is MUST use of rendering pipeline, use Update_Frame().
                */  
                Resume_GGUI();

                Current_Time = std::chrono::high_resolution_clock::now();

                // Calculate the delta time.
                Event_Delay = std::chrono::duration_cast<std::chrono::milliseconds>(Current_Time - Previous_Time).count();

                CURRENT_UPDATE_SPEED = MIN_UPDATE_SPEED + (MAX_UPDATE_SPEED - MIN_UPDATE_SPEED) * (1 - Event_Thread_Load);

                // If ya want uncapped FPS, disable this sleep code:
                std::this_thread::sleep_for(std::chrono::milliseconds(
                    Max(
                        CURRENT_UPDATE_SPEED - Event_Delay, 
                        MIN_UPDATE_SPEED
                    )
                ));
            }
        });

        std::thread Inquire_Scheduler([&](){
            while (true){
                // Wait for user input.
                Query_Inputs();

                Pause_GGUI();

                // Translate the Queried inputs.
                Translate_Inputs();

                // Translate the movements thingies to better usable for user.
                SCROLL_API();
                MOUSE_API();

                // Now call upon event handlers which may react to the parsed input.
                Event_Handler();

                Resume_GGUI();
            }
        });

        Rendering_Scheduler.detach();
        Event_Scheduler.detach();
        Inquire_Scheduler.detach();

        return Main;
    }

    /**
     * @brief Temporary function to return the current date and time in a string.
     * @return A string of the current date and time in the format "DD.MM.YYYY: SS.MM.HH"
     * @note This function will be replaced when the Date_Element is implemented.
     */
    std::string Now(){
        // This function takes the current time and returns a string of the time.
        // Format: DD.MM.YYYY: SS.MM.HH
        std::time_t now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());

        std::string Result = std::ctime(&now);

        // Remove the newline at the end of the string
        return Result.substr(0, Result.size() - 1);
    }

    /**
     * @brief Reports an error to the user.
     * @param Problem The error message to display.
     * @note If the main window is not created yet, the error will be printed to the console.
     * @note This function is thread safe.
     */
    void Report(std::string Problem){
        Pause_GGUI([&Problem]{
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
                        History = new Scroll_View(Styling(
                            width(Error_Logger->Get_Width() - 1) | height(Error_Logger->Get_Height() - 1) |
                            text_color(GGUI::COLOR::RED) | background_color(GGUI::COLOR::BLACK)
                        ));
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
                        Styling(
                            width(Main->Get_Width() / 4) | height(Main->Get_Height() / 2) |
                            text_color(GGUI::COLOR::RED) | background_color(GGUI::COLOR::BLACK) |
                            border_color(GGUI::COLOR::RED) | border_background_color(GGUI::COLOR::BLACK)
                        )
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
                    Scroll_View* History = new Scroll_View(Styling(
                        width(Error_Logger->Get_Width() - 1) | height(Error_Logger->Get_Height() - 1) |
                        text_color(GGUI::COLOR::RED) | background_color(GGUI::COLOR::BLACK)
                    ));
                    History->Set_Growth_Direction(DIRECTION::COLUMN);
                    History->Set_Name(HISTORY);

                    Error_Logger->Add_Child(History);
                    Main->Add_Child(Error_Logger);
                }

                if (Create_New_Line){
                    // re-find the error_logger.
                    Error_Logger = (Window*)Main->Get_Element(ERROR_LOGGER);
                    Scroll_View* History = (Scroll_View*)Error_Logger->Get_Element(HISTORY);

                    List_View* Row = new List_View(Styling(
                        width(History->Get_Width() - 1) | height(1) | text_color(GGUI::COLOR::RED) | background_color(GGUI::COLOR::BLACK)
                    ));
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

                    Remember([Error_Logger](std::vector<Memory>& rememberable){
                        rememberable.push_back(Memory(
                            TIME::SECOND * 30,
                            [Error_Logger](GGUI::Event*){
                                //delete tmp;
                                Error_Logger->Display(false);
                                //job successfully done
                                return true;
                            },
                            MEMORY_FLAGS::PROLONG_MEMORY,
                            "Report Logger Clearer"
                        ));
                    });
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

        });
    }

    /**
     * @brief Nests a text buffer into a parent buffer while considering the childs position and size.
     * 
     * @param Parent The parent element which the text is being nested into.
     * @param child The child element which's text is being nested.
     * @param Text The text buffer to be nested.
     * @param Parent_Buffer The parent buffer which the text is being nested into.
     */
    void Nest_UTF_Text(GGUI::Element* Parent, GGUI::Element* child, std::vector<GGUI::UTF> Text, std::vector<GGUI::UTF>& Parent_Buffer)
    {
        if (Parent == child)
        {
            std::string R = 
                std::string("Cannot nest element to it self\n") +
                std::string("Element name: ") + Parent->Get_Name();

            if (Parent->Get_Parent())
            {
                R += std::string("\n") + 
                std::string("Inside of: ") + Parent->Get_Parent()->Get_Name();
            }

            Report(
                R
            );
        }

        // Get the position of the child element in the parent buffer.
        GGUI::IVector3 C = child->Get_Position();

        int i = 0;
        // Iterate over the parent buffer and copy the text buffer into the parent buffer at the correct position.
        for (int Parent_Y = 0; Parent_Y < (signed)Parent->Get_Height(); Parent_Y++)
        {
            for (int Parent_X = 0; Parent_X < (signed)Parent->Get_Width(); Parent_X++)
            {
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

    /**
     * @brief Pauses all other GGUI internal threads and calls the given function.
     * @details This function will pause all other GGUI internal threads and call the given function.
     * @param f The function to call.
     */
    void Pause_GGUI(std::function<void()> f){

        // Save the current render status.
        Atomic::Status Previous_Render_Status;

        // Make an virtual local scope to temporary own the mutex.
        {
            // Lock the mutex to make sure we are the only one that can change the render status.
            std::unique_lock lock(Atomic::Mutex);

            // Save the current render status.
            Previous_Render_Status = Atomic::Pause_Render_Thread;
        }

        Pause_GGUI();

        try{
            // Call the given function.
            f();
        }
        catch(std::exception& e){
            // If an exception is thrown, report the stack trace and the exception message.
            Report_Stack("In Pause_GGUI: " + std::string(e.what()));
        }

        // Resume the render thread with the previous render status.
        Resume_GGUI(
            Previous_Render_Status
        );
    }

    /**
     * @brief Use GGUI in a simple way.
     * @details This is a simple way to use GGUI. It will pause all other GGUI internal threads, initialize GGUI, call the given function, sleep for the given amount of milliseconds, and then exit GGUI.
     * @param DOM The function that will add all the elements to the root window.
     * @param Sleep_For The amount of milliseconds to sleep after calling the given function.
     */
    void GGUI(std::function<void()> DOM, unsigned long long Sleep_For){
        Init_Start_Addresses();

        Pause_GGUI([DOM](){
            Init_GGUI();

            DOM();
        });

        SLEEP(Sleep_For);
        // No need of un-initialization here or forced exit, since on process death the right exit codes will be initiated.
    }

    /**
     * @brief Use GGUI in a simple way.
     * @details This is a simple way to use GGUI. It will pause all other GGUI internal threads, initialize GGUI, add all the elements to the root window, sleep for the given amount of milliseconds, and then exit GGUI.
     * @param DOM The elements to add to the root window.
     * @param Sleep_For The amount of milliseconds to sleep after calling the given function.
     */
    void GGUI(std::vector<Element*> DOM, unsigned long long Sleep_For){
        Init_Start_Addresses();

        Pause_GGUI([DOM](){
            Init_GGUI();

            // Add all the elements to the root window.
            for (auto* e : DOM){
                Main->Add_Child(e);
            }
        });

        // Sleep for the given amount of milliseconds.
        SLEEP(Sleep_For);
    }

    /**
     * @brief Encodes a buffer of UTF elements by setting start and end flags based on color changes.
     * 
     * @param Buffer A vector of UTF elements to be encoded.
     * @details The function marks the beginning and end of color strips within the buffer. 
     *          It checks each UTF element's foreground and background colors with its adjacent elements
     *          to determine where encoding strips start and end.
     */
    void Encode_Buffer(std::vector<GGUI::UTF>& Buffer) {
        
        // Initialize the first and last elements with start and end flags respectively.
        Buffer[0].Set_Flag(UTF_FLAG::ENCODE_START);
        Buffer[Buffer.size() - 1].Set_Flag(UTF_FLAG::ENCODE_END);

        // Iterate through the buffer to determine encoding strip positions.
        for (unsigned int Index = 1; Index < Buffer.size() - 1; Index++) {

            bool Same_Colours_As_Previous = Buffer[Index].Background == Buffer[Index - 1].Background &&
                                            Buffer[Index].Foreground == Buffer[Index - 1].Foreground;
            bool Same_Colours_As_Next = Buffer[Index].Background == Buffer[Index + 1].Background &&
                                        Buffer[Index].Foreground == Buffer[Index + 1].Foreground;

            // Set end flag if current colors differ from the next.
            if (!Same_Colours_As_Next) {
                Buffer[Index].Set_Flag(UTF_FLAG::ENCODE_END);
            }

            // Set start flag if current colors differ from the previous.
            if (!Same_Colours_As_Previous) {
                Buffer[Index].Set_Flag(UTF_FLAG::ENCODE_START);
            }
        }

        // Ensure the last element is marked correctly if the second to last was an ending node.
        if (Buffer[Buffer.size() - 2].Is(UTF_FLAG::ENCODE_END)) {
            Buffer[Buffer.size() - 1].Set_Flag(UTF_FLAG::ENCODE_START | UTF_FLAG::ENCODE_END);
        }
    }

    /**
     * @brief Updates the stats panel with the number of elements, render time, and event time.
     * @param Event The event that triggered the update.
     * @return True if the update was successful, false otherwise.
     * @details This function should be called by the main event loop to update the stats panel.
     */
    bool Update_Stats(GGUI::Event* Event){
        // Check if the inspect tool is displayed
        Element* Inspect_Tool = Main->Get_Element("Inspect");

        if (!Inspect_Tool || !Inspect_Tool->Is_Displayed())
            return false;

        // find the stats element
        Text_Field* Stats = (Text_Field*)Main->Get_Element("STATS");

        // Update the stats
        Stats->Set_Text(
            "Encode: " + std::to_string(Abstract_Frame_Buffer.size()) + "\n" + 
            "Decode: " + std::to_string(Frame_Buffer.size()) + "\n" +
            "Elements: " + std::to_string(Main->Get_All_Nested_Elements().size()) + "\n" +
            "Render delay: " + std::to_string(Render_Delay) + "ms\n" +
            "Event delay: " + std::to_string(Event_Delay) + "ms"
        );

        // return success
        return true;
    }

    /**
     * @brief Initializes the inspect tool.
     * @details This function initializes the inspect tool which is a debug tool that displays the number of elements, render time, and event time.
     * @see GGUI::Update_Stats
     */
    void Init_Inspect_Tool(){
        // Create a list view that will contain the inspect tool elements
        GGUI::List_View* Inspect = new GGUI::List_View(Styling(
            width(Main->Get_Width() / 2) | height(Main->Get_Height()) | 
            text_color(Main->Get_Text_Color()) | background_color(Main->Get_Background_Color()) 
        ));

        // Set the flow direction to column so the elements stack vertically
        Inspect->Set_Flow_Direction(DIRECTION::COLUMN);
        // Hide the border of the list view
        Inspect->Show_Border(false);
        // Set the position of the list view to the right side of the main window
        Inspect->Set_Position({
            Main->Get_Width() - (Main->Get_Width() / 2),
            0,
            INT32_MAX - 1,
        });
        // Set the opacity of the list view to 0.8
        Inspect->Set_Opacity(0.8f);
        // Set the name of the list view to "Inspect"
        Inspect->Set_Name("Inspect");

        // Add the list view to the main window
        Main->Add_Child(Inspect);
        
        // Add a count for how many UTF are being streamed.
        Text_Field* Stats = new Text_Field(
            "Encode: " + std::to_string(Abstract_Frame_Buffer.size()) + "\n" + 
            "Decode: " + std::to_string(Frame_Buffer.size()) + "\n" +
            "Elements: " + std::to_string(Main->Get_All_Nested_Elements().size()) + "\n" +
            "Render delay: " + std::to_string(Render_Delay) + "ms\n" +
            "Event delay: " + std::to_string(Event_Delay) + "ms",
            Styling(
                align(ALIGN::LEFT) | width(Inspect->Get_Width()) | height(5)
            )
        );
        // Set the name of the text field to "STATS"
        Stats->Set_Name("STATS");

        // Add the text field to the list view
        Inspect->Add_Child(Stats);

        // Add the error logger kidnapper:
        Window* Error_Logger_Kidnapper = new Window(
            "LOG: ",
            Styling(
                width(Inspect->Get_Width()) | height(Inspect->Get_Height() / 2) |
                text_color(GGUI::COLOR::RED) | background_color(GGUI::COLOR::BLACK) |
                border_color(GGUI::COLOR::RED) | border_background_color(GGUI::COLOR::BLACK) | 
                STYLES::border
            )
        );

        // Set the name of the window to "LOG"
        Error_Logger_Kidnapper->Set_Name(ERROR_LOGGER);
        // Allow the window to overflow, so that the text can be seen even if it is longer than the window
        Error_Logger_Kidnapper->Allow_Overflow(true);

        // Add the window to the list view
        Inspect->Add_Child(Error_Logger_Kidnapper);
        // Hide the inspect tool by default
        Inspect->Display(false);

        // Register an event handler to toggle the inspect tool on and off
        GGUI::Main->On(Constants::SHIFT | Constants::CONTROL | Constants::KEY_PRESS, [Inspect](GGUI::Event* e){
            GGUI::Input* input = (GGUI::Input*)e;

            // If the shift key or control key is pressed and the 'i' key is pressed, toggle the inspect tool
            if (!KEYBOARD_STATES[BUTTON_STATES::SHIFT].State && !KEYBOARD_STATES[BUTTON_STATES::CONTROL].State && input->Data != 'i' && input->Data != 'I') 
                return false;

            // Toggle the inspect tool, so if it is hidden, show it and if it is shown, hide it
            Inspect->Display(!Inspect->Is_Displayed());

            // Return true to indicate that the event was handled
            return true;
        }, true);
        
        // Remember the inspect tool, so it will be updated every second
        Remember([](std::vector<Memory>& rememberable){
            rememberable.push_back(
                GGUI::Memory(
                    TIME::SECOND,
                    Update_Stats,
                    MEMORY_FLAGS::RETRIGGER,
                    "Update Stats"
                )
            );
        });
    }

    /**
     * @brief Notifies all global buffer capturers about the latest data to be captured.
     *
     * This function is used to inform all global buffer capturers about the latest data to be captured.
     * It iterates over all global buffer capturers and calls their Sync() method to update their data.
     *
     * @param informer Pointer to the buffer capturer with the latest data.
     */
    void Inform_All_Global_BUFFER_CAPTURES(INTERNAL::BUFFER_CAPTURE* informer){

        // Iterate over all global buffer capturers
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

    /**
     * @brief Determines if a given pointer is likely deletable (heap-allocated).
     *
     * This function assesses whether a pointer may belong to the heap by comparing its
     * position relative to known memory sections such as the stack, heap, and data segments.
     *
     * @param ptr Pointer to be evaluated.
     * @return True if the pointer is likely deletable (heap-allocated), false otherwise.
     */
    bool Is_Deletable(void* ptr) {
        if (ptr == nullptr) {
            return false;  // Null pointer can't be valid
        }

        static void* Start_Of_BSS = nullptr;  // Placeholder for BSS segment start

        constexpr int MiB = 0x100000;
        static signed long long Somewhere_In_DATA = 100 * MiB;  // Arbitrary location in the data section

        // Check if ptr is above BSS, indicating potential data section location
        bool Ptr_Is_Above_BSS = ptr >= Start_Of_BSS;

        // Calculate if ptr is within range of the data section
        bool Ptr_Is_In_Range_Of_DATA_Section = ((signed long long)ptr - (signed long long)&Somewhere_In_DATA) <= Somewhere_In_DATA;

        // Check if ptr is smaller than the stack start address
        bool Lower_Than_Stack = (uintptr_t)ptr < (uintptr_t)Stack_Start_Address;

        // Try to allocate memory on the heap for comparison
        size_t* new_heap = new(std::nothrow) size_t;
        if (new_heap == nullptr) {
            Report_Stack("Failed to allocate new heap for stack pointer check!");
            exit(1);  // FATAL error if heap allocation fails
        }

        // Check if the new heap address is below the stack start address
        bool Heap_Is_Lower_Than_Stack = (uintptr_t)new_heap < (uintptr_t)Stack_Start_Address;

        // Calculate distance from ptr to the stack start address
        uintptr_t ptr_distance_to_stack = (uintptr_t)Stack_Start_Address - (uintptr_t)ptr;

        // Calculate distance from ptr to the closest heap address
        uintptr_t heap_min_address = Min((uintptr_t)new_heap, (uintptr_t)Heap_Start_Address);
        uintptr_t ptr_distance_to_heap = heap_min_address - (uintptr_t)ptr;

        // Determine if ptr is closer to the stack than the heap and below the stack start
        bool Stack_Is_Closer = ptr_distance_to_stack < ptr_distance_to_heap && Lower_Than_Stack;

        // Clean up the heap allocation
        delete new_heap;

        // Assess likelihood of pointer being in heap, stack, or data section
        int Points_To_DATA_Section = Ptr_Is_Above_BSS + Ptr_Is_In_Range_Of_DATA_Section;  // 2pts for data section
        int Points_To_Stack = Lower_Than_Stack + Stack_Is_Closer;  // 2pts for stack
        int Points_To_Heap = !Lower_Than_Stack + !Stack_Is_Closer + Heap_Is_Lower_Than_Stack - Points_To_DATA_Section;  // 3pts for heap

        // Return true if pointer is more likely heap-allocated
        return Points_To_Heap > Points_To_Stack && Points_To_Heap > Points_To_DATA_Section;
    }

}
