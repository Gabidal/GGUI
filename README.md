# **GGUI** - A Lightweight Terminal User Interface

### Full documentation [Here](https://gabidal.github.io/GGUI-Document/)

### Hello World example
```C++
#include <ggui.h>

using namespace GGUI;

int main() {
    GGUI::GGUI(
        node(new textField( 
            position(STYLES::center) | 
            text("Hello World!")
        ))
    );

    GGUI::waitForTermination(); // Wait until user gives signal to close (ctrl+c)
}
```

<img src="./bin/img/helloWorld.png">

### For more examples see [examples/README.md](./examples/README.md) folder. 

# Features
 - ### Cross platform (Windows, Linux, *"Android"*)
 - ### **Dynamic containers** with **horizontal** and **vertical** lists
 - ### **Text fields**, with left, center and right alignments
 - ### **Canvas** with builtin **sprite animation** handling
 - ### Fully customizable **progress bars** with even **multi line** support!
 - ### Customizable borders for all drawn elements
 - ### **Mouse support** with hover and focus effects
 - ### **Buttons** and **switches**
 - ### **Transparency** control
 - ### Custom **event handling** for ease of use.
 - ### *Should* contain everything you need to **interact with a terminal**
 - ### **Simple integration** to your projects with single `.h` and `.lib` file
    ```
    c++ -I./ggui.h -L./ggui.lib ...
    ```
 - ### **Optimized** for *Blazing fast* runtime with c++17 constexpr!
 - ### **Graceful exit** be it ctrl+c or normal exit, GGUI cleans up after itself 

# Building Manually
### Library:
```Bash
c++ -std=c++17 -O3 -I./src -c ./src/**/*.cpp -o ggui.lib   # This will compile all source files into a single library file.
```
### Header generation:
```Bash
c++ -std=c++17 -O3 -I./bin/export ./bin/export/buildGGUILib.cpp -o headerGenerator   # This will build the automated builder.
./headerGenerator --headers-only --source-root ./           # This will generate automatically ggui.h, you can also add '--include-internal' for building ggui_dev.h 
```

# Building Via Scripts
- ### Initialize project locally with the `init.sh` script
  - Will clean all existing build directories, exported binaries and generated headers
  - builds ggui_core archive and ggui.h for intellisense to work properly
- ### Build specific targets with `build.sh` script
  - give the build type first and then the target(s)
    ```bash
    ./bin/build.sh profile ggui     # Links ggui_core with main.cpp
    ./bin/build.sh release build_native_archive # builds a usable archive and auto generates headers under ./bin/export/
    ```
- ### Cross-platform libraries and headers from `export.sh` script
  - Cross-platform configs are located in `./bin/export/`
  - Build for specific architecture like this:
    ```
    ./bin/export.sh linux x86_64
    ```
  - Supported architectures are as follows:
    - linux arm32
    - linux arm64
    - linux x86_64
    - windows x86_64
  - Running `./bin/export.sh` with no arguments will build for all supported platforms.
- ### Run test suite with: `test.sh` script.
  - Takes build type as first argument
  - **Note**: The test suite does not recognize faulty rendering
  - Can run test for cross-platforms via Docker-QEMU, basically the same syntax as export.sh, but add the build type as first argument, like this:
    ```
    ./bin/test.sh release linux x86_64
    ```

## How i measure performance?
```bash
# Build types are: release, debug, profile
./bin/analytics/benchmark.sh -Full release      # Full CPU profiling
./bin/analytics/time.sh 5 120 release           # short duration 5s, long duration 120s; This is to check if opcodes explode with time growth or stay stabile.
./bin/analytics/assembly.sh profile             # Helper script to make a large asm file of the whole thing.
```
Note: scripts under analytics, do not support Docker cross-platform binary analysis yet!

### More about optimization and analytic scripts at [bin/analytics/README.md](./bin/analytics/README.md)
