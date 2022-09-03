#include "ggui.h"

using namespace std;

int main(){
    GGUI::Pause_Renderer();
    
    GGUI::Window* Main = GGUI::Init_Renderer();
    Main->Show_Border(true);

    GGUI::Resume_Renderer();

    while (true){
    }

    return 0;
}