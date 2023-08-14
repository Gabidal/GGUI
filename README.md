# GGUI

## Lightweight Terminal User Interface

<img src="Banner.png"/>

## Hello World example
```C++
#include "ggui.h"

using namespace GGUI;

int main() 
{
    Window* Main = Init_Renderer();

    Main->Set_Title("Your App UI");

    Text_Field* Txt = new Text_Field("Hello World!");

    Main->Add_Child(Txt);

    // Your code here...
}
```
### For more examples see Examples folder. 

# Features
- **ANSI escape sequence support.**
- **Multiplatform support.**
- **Only one .lib and .h file**
- **All elements have**
    - Background colours
    - Foreground colours
    - Border's and border colors
    - On focus Background & Foreground colors
    - On Hover Background & Foreground colors
    - Element Nesting
    - Event handling e.g On_Click(), (for more comprehensive function use: 'On()')
    - Alpha channel transparency support. 
- **Window features**
    - Title
- **Text_Field features**
    - Inner Text
    - Text alignment (center, left, right)
    - Text input
- **List_View features**
    - Flow direction
- **Canvas features**
    - Pixel per Pixel rendering
- **Progress_Bar**
    - Epic progress bars
- **Switch**
    - Radio Buttons
    - Checkboxes
- **Button**
    - Button stuff.
- **Mouse movement features**
    - Integrated mouse support (only windows atm)
    - Tabulator navigation, switch between elements with tabulator and shift+tabulator


# Developing this project further
### This project uses 'meson' so use `meson setup Build` to setup the meson environment if you have not yet done so.
### Use 'launch.json' and 'tasks.json' to build and debug this project if your in VSCode.

# Building this project
```bash
cd include
g++ ./Build_Script.cpp -O3
./a.exe
```

# Add GGUI to your project
```
g++ ... GGUI.lib
```