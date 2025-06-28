#include "ggui.h"

using namespace GGUI;

int main(){
    GGUI::GGUI(

        // To avoid drawing elements above each other
        node(new listView(
            flow_priority(DIRECTION::ROW) |     // horizontal list

            node(new listView(
                flow_priority(DIRECTION::COLUMN) |  // Vertical list
                childs({
                    new checkBox(
                        text("checkbox") | on_click([](element*){ return true; })   // Enable select events like mouse click and enter
                    ),
                    new checkBox(
                        text("another checkbox") | on_click([](element*){ return true; })   // Enable select events like mouse click and enter
                    )
                })
            )) | 

            node(new listView(
                flow_priority(DIRECTION::COLUMN) | enable_border(true) |    // vertical list with borders
                node(new listView(
                    flow_priority(DIRECTION::ROW) | enable_border(true) |   // horizontal list with border
                    node(new switchBox(
                        visualState("O", "X") | enable_border(true) | on_click([](element*){ return true; })     // Enable clicking and enter functions
                    )) | 
                    node(new switchBox(
                        visualState("O", "X") | enable_border(true) | on_click([](element*){ return true; })     // Enable clicking and enter functions
                    )) | 
                    node(new switchBox(
                        visualState("O", "X") | enable_border(true) | on_click([](element*){ return true; })     // Enable clicking and enter functions
                    ))
                )) | 
                node(new listView(
                    flow_priority(DIRECTION::ROW) | enable_border(true) |    // horizontal list with border
                    node(new switchBox(
                        visualState("O", "X") | enable_border(true) | on_click([](element*){ return true; })     // Enable clicking and enter functions
                    )) | 
                    node(new switchBox(
                        visualState("O", "X") | enable_border(true) | on_click([](element*){ return true; })     // Enable clicking and enter functions
                    )) | 
                    node(new switchBox(
                        visualState("O", "X") | enable_border(true) | on_click([](element*){ return true; })     // Enable clicking and enter functions
                    ))
                )) | 
                node(new listView(
                    flow_priority(DIRECTION::ROW) | enable_border(true) |    // horizontal list with border
                    node(new switchBox(
                        visualState("O", "X") | enable_border(true) | on_click([](element*){ return true; })     // Enable clicking and enter functions
                    )) | 
                    node(new switchBox(
                        visualState("O", "X") | enable_border(true) | on_click([](element*){ return true; })     // Enable clicking and enter functions
                    )) | 
                    node(new switchBox(
                        visualState("O", "X") | enable_border(true) | on_click([](element*){ return true; })     // Enable clicking and enter functions
                    ))
                ))
            )) | 

            // We can make an vertical list where we can select only one of the given selecttables.
            node(new listView(
                flow_priority(DIRECTION::COLUMN) | enable_border(true) |   // vertical list with border
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
                }) | 
                on_click([](element* self){
                    for (auto* switchable : self->getElements<switchBox>()){
                        if (!switchable->isSelected())
                            continue;
    
                        // Now we have the selected box, here do what you want :)
                    }
                    return false;   // Since this is one layer above, we need the input to be able to be passed to the inner childs.
                })
            ))
        ))
    );

    while(true){
        // ...
    }
}