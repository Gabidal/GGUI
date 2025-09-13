# GGUI 0.1.8 Update
```
  /$$$$$$   /$$$$$$  /$$   /$$ /$$$$$$
 /$$__  $$ /$$__  $$| $$  | $$|_  $$_/
| $$  \__/| $$  \__/| $$  | $$  | $$  
| $$ /$$$$| $$ /$$$$| $$  | $$  | $$  
| $$|_  $$| $$|_  $$| $$  | $$  | $$  
| $$  \ $$| $$  \ $$| $$  | $$  | $$  
|  $$$$$$/|  $$$$$$/|  $$$$$$/ /$$$$$$
 \______/  \______/  \______/ |______/
  /$$$$$$       /$$        /$$$$$$    
 /$$$_  $$    /$$$$       /$$__  $$   
| $$$$\ $$   |_  $$      | $$  \ $$   
| $$ $$ $$     | $$      |  $$$$$$/   
| $$\ $$$$     | $$       >$$__  $$   
| $$ \ $$$     | $$      | $$  \ $$   
|  $$$$$$//$$ /$$$$$$ /$$|  $$$$$$/   
 \______/|__/|______/|__/ \______/
```
### Overview
Major restructuring and modernization of the project. Heaps of optimization and custom containers made for most optimal results. 
Whole new styling system for more HTML/XML like styling and node tree construction.
Clipped off internal components from getting into auto generated header on export.
Since this update almost re-wrote the whole project many commits which focused on fixing issues, were about issues brought by the new re-write.

### API Changes
- Input events now use IVector2 instead of Coordinate struct
- Many if not all classes are now written in lower camelCase
- Vector2 is now correctly named FVector2 for containing floating point coordinates
- GGUI::mouse is now an IVector2 type
- You can now give percentage floating point values for almost any value based styling, like width or color channel and so on...
  - You can use these as follows:
    ```Cpp
    element(
        width(100) | height(100) |   // set the parent element as 100x100
        node(new element(
            width(0.50f) | height(0.50f)    // this child element is now 50x50 and updates as its parents dimensions changes
        ))
    )
    ```
- GGUI does no longer have RGBA, instead opacity is given as a single floating value per element node
- RGB lerp can use Gamma color correct compute for much higher cost or use cheap simple lerp formula
  - You can configure this via the settings or via the cmd arg:
    ```CMD
    enableGammaCorrection
    ```
  - GGUI accepts many of configurable settings at call time, via the args. GGUI removes hyphens so you can add single hyphen, double hyphen or no hyphens.
- Since most of the node structure is now given as an HTML/XML like, GGUI provides `on_init()` and other `on()` state change function handlers the user can customize
  - Since JS and other types of similar products use underscores for their `on_*` state handlers GGUI uses this naming convention as well.
- `GGUI::Main` is not accessible anymore, since it was moved under `GGUI::INTERNAL::Main`, and intended for user to access through `GGUI::getRoot()`
- Titles are now native to element class so window class was removed
- Pixel based canvas has been removed, since only terminal cell based canvas is needed
  - For pixel perfect canvas look up GGDirect for GGUI with DRM as backend support. 
- Button class has been removed, since normal textField with `on_click()` brings out the same behavior
- SwitchBoxes have now `singleSelect(true)` like styling which disables all other marked switchBoxes in the same parent, giving user simple way to make single select choice lists
- Shadow styling has been removed, to be replaced later on with full shaders like customizations

### Fixes
- Fixed where transparency wasn't propagated properly to transparent layers of text, not obtaining color from layers below

### Optimizations
- To prevent unnecessary string concatenations GGUI implements its own string_view and string concatenation, these are called:
  - compactString: Holds as a std::variant either a char or a unicode letter
  - superString: Holds a predetermined list of compactStrings, capable of concatenating smaller superStrings to itself
  - The whole rendering pipeline is based on these two classes.
- Since GGUI has a varying and sometimes deeply nested functions of getters, many of them have been made constexpr to hopefully ease inlining and runtime
- To prevent weird std::vector behavior and total control in niche use case of it, GGUI now implements its own fastVector for more cache-like behavior between render passes

### For Maintainers
- Added proper scripts for linux and windows side for init, build, export, test and benchmarking with various kinds of tools
- GGUI can take now -DGGUI_DEBUG, -DGGUI_RELEASE or -DGGUI_PROFILE, which are automatically managed via the meson build system, but could be manually set for fine control
  - Enabling -DGGUI_DEBUG, makes GGUI behave like `--verbose` in other programs.

### Internal 
- Added new multithread safety guard wrapper for:
  - Memory events
- Distributed to multithreading these following tasks:
  - Rendering pipeline
  - Memory and timed events handling
  - User input handling
- 0.1.8 introduces new internally used `reportStacktrace()`, which utilizes symbol table lookup and stack to print out helpful stacktraces
- GGUI utilizes some constexpr operator to default in all stylings, which is why GGUI currently only works on GCC v13 and higher and not in Clang!
- The GGUI lib now links statically with libc and libc++

### Small additional notes
- GitHub Actions CI workflows
- New analytics & benchmarking scripts
  - `./bin/test.sh`
  - `./bin/analytics/benchmark.sh`
  - `./bin/analytics/time.sh 5 10`  <- runs for 5 second and 10 seconds and displays the growth in opcode count.
- Examples in more structured sub folders
  - small       <- single feature focused examples
  - medium      <- feature combinatorial examples
  - large       <- applicative examples
- Developer and contribution tooling (.github, guidelines)
- When you use the new styling position with percentages, note that the percentage already takes into account the parent.width or height minus the current node elements own width or height
  - For an example:
    ```Cpp
    element(
        width(100) | height(100) | 

        node(new element(
            width(10) | height(10) |
            position(STYLES::bottom + STYLES::left)     // This will force this element to be on the bottom left corner of the parent offset by the 10x10, so that it does not go out of bounds.
        ))
    )
    ```