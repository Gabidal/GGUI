# GGUI - Good GUI

## GGUI is a lightweight Terminal user interface
## GGUI tries to give the user single .lib and .h file to use for their projects.

# Features
- ## UFT-8 (Default ASCII)
- ## UTF-16 (Unicode), (if user terminal supports sutch thing ;) ).
- ## Infinite window nesting (as everybody else has it).
- ## Thats basically it atm :P, like what did you expect from a super light weight TUI bro?
- ## Oh almost forgot, coloring and bolding is a thing now.

# Build
## Just use .vscode/tasks.json file as an example.
## TL;DR 
- ### g++ main.cpp -o "whatever output folder/GGUI" -g [if you want for some reason debug my code? like there would be any problems hahaha ¦| ]
## No No for real this time
- ### g++ -c main.cpp -o GGUI.o
- ### ar rvs GGUI.lib GGUI.o

# Use
- ### g++ some_other_project_main.cpp GGUI.lib