#include <GGUI.h>

using namespace GGUI;

int main(){
    GGUI::GGUI(
        node(new listView(
            flowPriority(DIRECTION::COLUMN) | 

            // Instead of spamming node(new ...) everytime, you can make an childs which accepts an initializer list of element*
            childs({
                // Giving no width or height enables dynamic size.
                new listView(
                    flowPriority(DIRECTION::ROW) |     // <-- make this an horizontal list
        
                    node(new element(
                        width(20) | height(10) | 
                        textColor(COLOR::TEAL) | title("TEAL") |
                        backgroundColor(COLOR::DARK_RED) |
                        enableBorder(true)     // <-- enabling borders for two neighboring child nodes, will enable border optimization, saving space.
                    )) |
                    node(new element(
                        width(10) | height(15) | 
                        textColor(COLOR::LIGHT_GREEN) | title("L-GREEN") | 
                        backgroundColor(COLOR::WHITE) | 
                        enableBorder(true)     // <-- 
                    ))
                ),

                // If we have child elements that have borders enabled by default, we can give the parent also borders to also optimize those away if wanted.
                new listView(
                    flowPriority(DIRECTION::ROW) | enableBorder(true) | 
        
                    node(new textField(
                        text("File") | enableBorder(true)
                    )) |
                    node(new textField(
                        text("Edit") | enableBorder(true)
                    )) | 
                    node(new textField(
                        text("Help") | enableBorder(true)
                    )) |
                    node(new textField(
                        text("Exit") | enableBorder(true) | textColor(COLOR::RED)
                    ))
                )
            })
        ))
    );

    while(true){ 
        //Your programm here :D
    }
}