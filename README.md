# GGUI

## Lightweight Terminal User Interface

<img src="banner.png"/>

## Hello World example
```C++
#include "ggui.h"

using namespace GGUI;

int main() 
{
    GGUI::GGUI(
        title("Your App UI") | 
        node(new textField( 
            position(STYLES::center) | 
            text("Hello World!")
        ))
    );

    while (true) {
        // ... 
    }
}
```

<img src="./bin/img/helloWorld.jpg">

### For more examples see `./examples` folder. 

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
- ### Initialize project locally with: `init.sh` or `init.bat`.
- ### Exporting this project as library is documented in `./Export/README.md`.
- ### Add GGUI to your project
    ```
    g++ ... GGUI.lib
    ```