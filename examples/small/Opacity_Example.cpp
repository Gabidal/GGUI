#include "ggui.h"

using namespace std;
using namespace GGUI;

int main(){
    GGUI::GGUI(
        node(new element(
            width(1.0f) | height(0.7f) | 
            background_color(COLOR::RED) | 
            opacity(0.5f) | position(STYLES::top)
        )) | 
        node(new element(
            width(1.0f) | height(0.7f) | 
            background_color(COLOR::BLUE) | 
            opacity(0.5f) | position(STYLES::bottom)
        )) | 
        background_color(COLOR::WHITE)
    );

    GGUI::INTERNAL::SLEEP(INT32_MAX);
}