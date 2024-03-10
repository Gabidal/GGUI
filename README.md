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
- **Scroll_View features**
    - Ability to add elements over boundaries and also scrolling vertically or horizontally, based on the Flow direction.
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
### Use `meson compile -C Build` to build the GGUI project.
### Builtin .vscode tasks.json and launch.json also useful for Dev.

# Building this project
```bash
g++ ./include/Build_Script.cpp
./a.exe
```

# Add GGUI to your project
```
g++ ... GGUI.lib
```