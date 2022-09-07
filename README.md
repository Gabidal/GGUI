# GGUI

## Lightweight Terminal User Interface

<img src="Banner.png"/>

## Hello World example
```C++
#include "GGUI.h"

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
    - Nesting
    - Event handling e.g On_Click()
- **Window features**
    - Title
- **Text_Field features**
    - Inner Text
    - Text alignment (center, left, right)
- **List_View features**
    - Flow direction

- **Mouse movement features**
    - Free move with arrow keys.
    - Element selection navigation toggle with shift key.

# Developing this project further
### Use vscode "tasks.json" and "launch.json" files.

# Building this project
```bash
cd include
g++ ./Build_Script.cpp -O3
./a
```

# Add GGUI to your project
```
g++ ... GGUI.lib
```