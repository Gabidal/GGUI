#include "ggui.h"

using namespace GGUI;

int main(){
    GGUI::GGUI(
        node(new listView(
            flow_priority(DIRECTION::COLUMN) | enable_border(true) |
            childs({
                new radioButton(
                    text("A") | 
                    singleSelect() |     // This will disable all other grouped selecttables
                    on_click([](element* self){ return true; })     // Enable clicking and enter functions
                ),
                new radioButton(
                    text("B") | 
                    singleSelect() | 
                    on_click([](element* self){ return true; })
                ),
                new radioButton(
                    text("C") | 
                    singleSelect() | 
                    on_click([](element* self){ return true; })
                )
            })
        ))
    );

    while(true){
        // ...
    }
}