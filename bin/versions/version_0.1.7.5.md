# GGUI 0.1.7.5 Update
```
            _____ _____ _____ _____ 
            |   __|   __|  |  |     |
            |  |  |  |  |  |  |-   -|
            |_____|_____|_____|_____|              
                ___   _  _____
                / _ \ / ||___  |
                | | | || |   / / 
                | |_| || |_ / /  
                \___(_)_(_)_/           
```

# Build and Configuration
- ### Updated `Build_Body.cpp` to automatically compile as a single header file.
- ### Update `meson.build` to include AVX support for SIMD.

# Command Line and Platform Support
### **New CMD Class for Command Line Operations:** 
### Added CMD class to handle continuous command line operations.
### **Usage:**
```Cpp
GGUI::CMD cmd;
cmd.Run("cd ./somewhere")
cmd.Run("python this_is_in_somewhere.py")
```

# Border Styling and Customization
### **Custom Border Styles:**
### Introduced support for custom border styles in UI elements.
### **Usage:**
```Cpp
GGUI::Element tmp = new GGUI::Element(10, 10);
tmp->Show_Border(true);
tmp->Set_Custom_Border_Style(GGUI::STYLES::BORDER::Double);
```

# Terminal Canvas and Sprite Animation
### **Sprite Animations.**
### Added support for sprite animations on `Terminal_Canvas`, with customizable characters and colors. Colors are interpolated for smooth transitions.
### **Usage:**
```Cpp
// Single frame Sprite:
GGUI::Sprite s("a");

// Single frame Sprite with colors:
GGUI::Sprite s(GGUI::UTF("a", {
    GGUI::COLOR::RED /*text color*/, 
    GGUI::COLOR::BLUE /*background color*/
}));

// Multi frame Sprite:
GGUI::Sprite s(
    {
        {"a", {GGUI::COLOR::RED /*text color*/, GGUI::COLOR::BLUE /*background color*/}}, 
        {"b", {GGUI::COLOR::BLUE, GGUI::COLOR::RED}}, 
    },
    0,  // [Optional] Animation offset
    1   // [Optional] Animation speed
);

// apply the sprite to the canvas at (x, y).
example_canvas->Set(x, y, s);
```

# Performance Improvements:
### Implemented various optimizations to enhance performance and reduce memory usage.
- ### Overhauled thread scheduler systems for improved task management.
- ### Optimized hitbox updater, job scheduler, and buffer encoder.
- ### Streamlined shadow and opacity processing.
- ### Improved UTF symbol initialization, transitioning from strings to `Compact_String`.
- ### Enhanced styling system by replacing `unordered_maps` with class member access.
- ### Changed RGBA alpha channel from float to unsigned char, optimizing to fit within a 32-bit register.
- ### Secondary threads now avoid spin-locking when paused.
- ### Dedicated a separate thread for the rendering pipeline, preventing conflicts with the main thread.

# Refactoring and Cleanup
### **Significant refactoring to improve code structure and resolve issues.**
- ### Refactored sleep functions and fixed lambda capture issues.
- ### Addressed missing default arguments.
- ### Removed OUTBOX from development listings and refined styling merging.
- ### Updated constructors to set element names from strings.
- ### Rewrote the `Text_Field` and `Progress_Bar` classes.
- ### Enhanced `List_View` to re-adjust contents upon resizing.
- ### Ensured GGUI compiles without warnings

# Bug Fixes and Tweaks
### **Fixed various bugs and made minor improvements.**
- ### Resolved issues with Unix-side key inputs and mouse support.
- ### Improved RGBA alpha channel handling and memory management.
- ### Addressed bugs in key handling and system stability.
- ### Corrected issues where certain element class flags were not cleared properly.
- ### Fixed hover not displaying on buttons, since the content was filling the whole area and border are not affected by hover nor focus.

# Known Issues
- ### Shadows are not functioning correctly.
- ### Adding multiple identical child elements to `List_View` only displays the last added.

# Compact_String And Super_String
### **New String Handling Mechanism:**
### Implemented a new system for handling characters and string literals, optimizing the display of repeating ANSI Escape Sequence codes.
- ### Compact_String: Contains a const char* and its size, representing a string efficiently.
- ### Super_String: A collection of `Compact_String`s that is transformed into a normal string at the end of the rendering pipeline for display.

# Side Notes
- ### For large projects or serious usage, consider waiting for version 0.1.8, which will significantly overhaul element creation and styling management.
- ### Inlining the GGUI namespace is not recommended in larger projects.
- ### Make sure GGUI always exists through: `GGUI::Exit();`.
- ### Batch changes inside: `GGUI::Pause_GGUI(Function/Lambda)`.
    ```Cpp
    // This will avoid re-rendering for all changes. Instead flush all the changes once exiting Pause_GGUI();
    GGUI::Pause_GGUI([a, b, c](){
        a->Set_Background_Color(GGUI::RGB(1, 2, 3));
        b->Set_Position({10, 10});
        c->Add_Child(a);
        c->Add_Child(b);
    });
    ```
- ### Recommended to initialize GGUI with: `GGUI::GGUI(function/lambda)`.
- ### Before setting up meson, set environment variable: `CXX=g++`.

# Next Update (0.1.8) Preview
### **Upcoming Features:**
- ### Pipelining output from other processes into GGUI for enhanced display.
- ### Reworking element construction and styling.
- ### Adding Unicode support to `Text_Fields`.
- ### Simple HTML support.
- ### Drag & Drop functionality.
- ### Dynamic stylization updates like percentage values and corner anchoring.
- ### `List_View` content line-wrap.
- ### Font file rendering support.
- ### Post-process shaders for applying sprite animations to other elements.
- ### Replacing GGUI DOM with a vector-based system for better content management.
- ### Optimize data types, for better memory footprint.