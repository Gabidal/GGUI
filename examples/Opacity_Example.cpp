#include "ggui.h"

using namespace std;
using namespace GGUI;

int main(){
    GGUI::GGUI(
        node(new element(
            name("A") | width(20) | height(10) | 
            text_color(COLOR::MAGENTA) | background_color(COLOR::RED) | 
            opacity(0.5f) | position(10, 20)
        )) | 
        node(new element(
            name("B") | width(20) | height(10) | 
            text_color(COLOR::YELLOW) | background_color(COLOR::GREEN) | 
            opacity(0.5f) | position(20, 12)
        )) | 
        node(new element(
            name("C") | width(20) | height(10) | 
            text_color(COLOR::CYAN) | background_color(COLOR::BLUE) | 
            opacity(0.5f) | position(15, 17)
        )) | 
        background_color(COLOR::WHITE)
    );

    GGUI::INTERNAL::SLEEP(INT32_MAX);
}