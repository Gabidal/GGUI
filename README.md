# GGUI

## Lightweight Terminal User Interface

<img src="Banner.png"/>

## Hello World example
```C++
#include "GGUI.h"

using namespace GGUI;

int main() 
{
    // Initialize and startup GGUI
    Init_GGUI();

    // Give the main window an title
    Main->Set_Title("Your App UI");

    // Create an text field
    Text_Field* txt = new Text_Field("Hello World!");

    // Add the text field to the main container to be displayed
    Main->Add_Child(txt);

    // Your code here...

    // de-initialize console status.
    Exit();
}
```
### For more examples see `./Examples` folder. 

# Features
 - ### Scrollable horizontal and vertical lists
 - ### Canvas with builtin cell animations
 - ### Custom error log (CTRL+SHIFT+I)
 - ### Text fields, with left, center and right aligning
 - ### RGB with Transparency
 - ### Progress bars.
 - ### Switches with radio buttons
 - ### Continuous command line interaction support  
 - ### Cross platform (Windows, Linux, Android)

# Developing this project further
- ### Initialize project locally with: `Init.sh` or `Init.bat`.
- ### Exporting this project as library is documented in `./Export/README.md`.
- ### Add GGUI to your project
    ```
    g++ ... GGUI.lib
    ```