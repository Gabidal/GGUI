#include "ggui.h"

using namespace GGUI;

int main(){
    GGUI::GGUI(
        node(new listView(
            flow_priority(DIRECTION::COLUMN) | 

            // Instead of spamming node(new ...) everytime, you can make an childs which accepts an initializer list of element*
            childs({
                // Giving no width or height enables dynamic size.
                new listView(
                    flow_priority(DIRECTION::ROW) |     // <-- make this an horizontal list
        
                    node(new element(
                        width(20) | height(10) | 
                        text_color(COLOR::TEAL) | title("TEAL") |
                        background_color(COLOR::DARK_RED) |
                        enable_border(true)     // <-- enabling borders for two neighboring child nodes, will enable border optimization, saving space.
                    )) |
                    node(new element(
                        width(10) | height(15) | 
                        text_color(COLOR::LIGHT_GREEN) | title("L-GREEN") | 
                        background_color(COLOR::WHITE) | 
                        enable_border(true)     // <-- 
                    ))
                ),

                // If we have child elements that have borders enabled by default, we can give the parent also borders to also optimize those away if wanted.
                new listView(
                    flow_priority(DIRECTION::ROW) | enable_border(true) | 
        
                    node(new textField(
                        text("File") | enable_border(true)
                    )) |
                    node(new textField(
                        text("Edit") | enable_border(true)
                    )) | 
                    node(new textField(
                        text("Help") | enable_border(true)
                    )) |
                    node(new textField(
                        text("Exit") | enable_border(true) | text_color(COLOR::RED)
                    ))
                )
            })
        ))
    );

    while(true){ 
        //Your programm here :D
    }
}