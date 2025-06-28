#include "ggui.h"

using namespace GGUI;

int main(){
    GGUI::GGUI(
        background_color(COLOR::WHITE) | // <-- This will be our base color
        
        // This element should look like Red + White = Pink
        node(new element(
            width(1.0f) | height(0.7f) |    // <-- 70% because we want little bit of overlap between the two elements
            background_color(COLOR::RED) |          // base color is red, but 50% of its value is given from its parent
            opacity(0.5f) | position(STYLES::top)   // positioned at the top of the parent element
        )) | 

        // <-- The overlap between the two elements will produce Pink + Cyan = Magenta.

        // This element should look like Blue + White = Cyan
        node(new element(
            width(1.0f) | height(0.7f) |    // <-- 70% because we want little bit of overlap between the two elements
            background_color(COLOR::BLUE) |      // base color is blue, but 50% of its value is given from its parent
            opacity(0.5f) | position(STYLES::bottom) // positioned at the bottom of the parent element
        ))
    );

    // You can also use the GGUI's internal sleep function to wait.
    GGUI::INTERNAL::SLEEP(INT32_MAX);
}