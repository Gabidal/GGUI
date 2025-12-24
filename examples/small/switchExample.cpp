#include <ggui.h>

using namespace GGUI;

int main(){
    GGUI::GGUI(

        // To avoid drawing elements above each other
        node(new listView(
            flowPriority(DIRECTION::ROW) |     // horizontal list

            node(new listView(
                flowPriority(DIRECTION::COLUMN) |  // Vertical list
                childs({
                    new checkBox(
                        text("checkbox") | onClick([](element*){ return true; })   // Enable select events like mouse click and enter
                    ),
                    new checkBox(
                        text("another checkbox") | onClick([](element*){ return true; })   // Enable select events like mouse click and enter
                    )
                })
            )) | 

            node(new listView(
                flowPriority(DIRECTION::COLUMN) | enableBorder(true) |    // vertical list with borders
                node(new listView(
                    flowPriority(DIRECTION::ROW) | enableBorder(true) |   // horizontal list with border
                    node(new switchBox(
                        visualState("O", "X") | enableBorder(true) | onClick([](element*){ return true; })     // Enable clicking and enter functions
                    )) | 
                    node(new switchBox(
                        visualState("O", "X") | enableBorder(true) | onClick([](element*){ return true; })     // Enable clicking and enter functions
                    )) | 
                    node(new switchBox(
                        visualState("O", "X") | enableBorder(true) | onClick([](element*){ return true; })     // Enable clicking and enter functions
                    ))
                )) | 
                node(new listView(
                    flowPriority(DIRECTION::ROW) | enableBorder(true) |    // horizontal list with border
                    node(new switchBox(
                        visualState("O", "X") | enableBorder(true) | onClick([](element*){ return true; })     // Enable clicking and enter functions
                    )) | 
                    node(new switchBox(
                        visualState("O", "X") | enableBorder(true) | onClick([](element*){ return true; })     // Enable clicking and enter functions
                    )) | 
                    node(new switchBox(
                        visualState("O", "X") | enableBorder(true) | onClick([](element*){ return true; })     // Enable clicking and enter functions
                    ))
                )) | 
                node(new listView(
                    flowPriority(DIRECTION::ROW) | enableBorder(true) |    // horizontal list with border
                    node(new switchBox(
                        visualState("O", "X") | enableBorder(true) | onClick([](element*){ return true; })     // Enable clicking and enter functions
                    )) | 
                    node(new switchBox(
                        visualState("O", "X") | enableBorder(true) | onClick([](element*){ return true; })     // Enable clicking and enter functions
                    )) | 
                    node(new switchBox(
                        visualState("O", "X") | enableBorder(true) | onClick([](element*){ return true; })     // Enable clicking and enter functions
                    ))
                ))
            )) | 

            // We can make an vertical list where we can select only one of the given selecttables.
            node(new listView(
                flowPriority(DIRECTION::COLUMN) | enableBorder(true) |   // vertical list with border
                childs({
                    new radioButton(
                        text("A") | 
                        singleSelect() |     // This will disable all other grouped selecttables
                        onClick([](element* self){ return true; })     // Enable clicking and enter functions
                    ),
                    new radioButton(
                        text("B") | 
                        singleSelect() | 
                        onClick([](element* self){ return true; })
                    ),
                    new radioButton(
                        text("C") | 
                        singleSelect() | 
                        onClick([](element* self){ return true; })
                    )
                }) | 
                onClick([](element* self){
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

    std::this_thread::sleep_for(std::chrono::seconds(INT32_MAX));
}