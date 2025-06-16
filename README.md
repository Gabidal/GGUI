# **GGUI** - A Lightweight Terminal User Interface

### Hello World example
```C++
#include "ggui.h"

using namespace GGUI;

int main() 
{
    GGUI::GGUI(
        title("Welcome to GGUI!") | 
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

<img src="./bin/img/helloWorld.png">

### For more examples see [examples/README.md](./examples/README.md) folder. 

# Features
 - ### Cross platform (Windows, Linux, *Android*)
 - ### **Dynamic containers** with **horizontal** and **vertical** lists
 - ### **Text fields**, with left, center and right aligning
 - ### **Canvas** with builtin **sprite animation** handling.
 - ### **Transparency** control
 - ### Fully customizable **progress bars** with even **multi line** support!.
 - ### *Should* contain everything you need to **interact with a terminal**
 - ### **Simple integration** to your projects with single header and lib file
    ```
    g++ ... GGUI.lib
    ```

--- 
# Contributing to development of **GGUI**
- ### Initialize project locally with: `init.sh` or `init.bat`.
- ### Exporting this project as library is documented in `./export/README.md`.