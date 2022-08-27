#include <iostream>
#include <io.h>
#include <fcntl.h>

#include "Renderer.h"

using namespace std;


int main(){
    GGUI::Window* Main = RENDERER::Init_Renderer();
    Main->Show_Border(true);
    
    // GGUI::Window* Text = new GGUI::Window("", {10, 10, 0}, "Hello World!", true, 13, 3);
    // Main->Add_Child(Text);

    // Text = new GGUI::Window("", {25, 5, 0}, "Hello World!", true, 13, 3);
    // Main->Add_Child(Text);

    // Text = new GGUI::Window("", {40, 30, 0}, "Hello World!", true, 13, 3);
    // Main->Add_Child(Text);

    while(true){ 
    }

}