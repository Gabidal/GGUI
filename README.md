# GGUI - GG-UI

## GGUI is a lightweight Terminal user interface
## GGUI tries to give the user single .lib and .h file to use for their projects.

# Features
- ## ANSI escape sequense support.
- ## Multiplatform support.
- ## Only one .lib and .h file
- ## All elements have
    - BG colours
    - FG colours
    - Border's and border colors
    - On focus BG & FG colors
    - Nesting
    - Event handling e.g On_Click()
- ## Window features
    - Title
- ## Text_Field features
    - Inner Text
    - Text alignment (center, left, right)
- ## List_View features
    - Flow direction

- ## Mouse movement features
    - Free move with arrow keys.
    - Element selection navigation toggle with shift key.

# For Dev build
### Use vscode .tasks & launch.json files.

# For Use Build
### cd include
### g++ .\Build_GGUI_To_LIB.cpp -O3
### ./a.exe

# Integrate GGUI to your project
- ### g++ some_other_project_main.cpp libGGUI.lib