#include "ggui.h"

using namespace GGUI;

int main(){
    GGUI::GGUI(

        node(new listView(
            flow_priority(DIRECTION::ROW) | 

            node(new listView(
                flow_priority(DIRECTION::COLUMN) | enable_border(true) | 
                node(new listView(
                    flow_priority(DIRECTION::ROW) | enable_border(true) | 
                    node(new switchBox(
                        visualState("O", "X") | enable_border(true) | on_click([](element* self){ return true; })     // Enable clicking and enter functions
                    )) | 
                    node(new switchBox(
                        visualState("O", "X") | enable_border(true) | on_click([](element* self){ return true; })     // Enable clicking and enter functions
                    )) | 
                    node(new switchBox(
                        visualState("O", "X") | enable_border(true) | on_click([](element* self){ return true; })     // Enable clicking and enter functions
                    ))
                )) | 
                node(new listView(
                    flow_priority(DIRECTION::ROW) | enable_border(true) | 
                    node(new switchBox(
                        visualState("O", "X") | enable_border(true) | on_click([](element* self){ return true; })     // Enable clicking and enter functions
                    )) | 
                    node(new switchBox(
                        visualState("O", "X") | enable_border(true) | on_click([](element* self){ return true; })     // Enable clicking and enter functions
                    )) | 
                    node(new switchBox(
                        visualState("O", "X") | enable_border(true) | on_click([](element* self){ return true; })     // Enable clicking and enter functions
                    ))
                )) | 
                node(new listView(
                    flow_priority(DIRECTION::ROW) | enable_border(true) | 
                    node(new switchBox(
                        visualState("O", "X") | enable_border(true) | on_click([](element* self){ return true; })     // Enable clicking and enter functions
                    )) | 
                    node(new switchBox(
                        visualState("O", "X") | enable_border(true) | on_click([](element* self){ return true; })     // Enable clicking and enter functions
                    )) | 
                    node(new switchBox(
                        visualState("O", "X") | enable_border(true) | on_click([](element* self){ return true; })     // Enable clicking and enter functions
                    ))
                ))
            )) | 

            // We can make an vertical list where we can select only one of the given selecttables.
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