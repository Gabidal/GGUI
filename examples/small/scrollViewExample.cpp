#include "ggui.h"

using namespace GGUI;

int main(){
    GGUI::GGUI(
        node(new scrollView(
            flowPriority(DIRECTION::ROW) |     // horizontal listView container that enabled scrolling
            width(1.5f) |   // 150% of the parents width
            name("vertical scroll view") | backgroundColor(COLOR::WHITE) | position(STYLES::center) | 

            // When this element has done its styling configuration and is ready to be pre-rendered, this will be run
            onInit([](element* self){
                for (unsigned int i = 0; i < self->getWidth(); i++){

                    // Since elements default width and height is 1, we can just add a new element with a random background color.
                    element* tmp = new element(backgroundColor(RGB(rand()%255,rand()%255,rand()%255)));
                    self->addChild(tmp);
                }
            })
        )) | 
        
        node(new scrollView(
            flowPriority(DIRECTION::COLUMN)|   // vertical listView container that enabled scrolling
            height(1.5f) |  // 150% of the parents height
            name("horizontal scroll view") | backgroundColor(COLOR::WHITE) | position(STYLES::center) | 
            
            // When this element has done its styling configuration and is ready to be pre-rendered, this will be run
            onInit([](element* self){
                for (unsigned int i = 0; i < self->getHeight(); i++){

                    // Since elements default width and height is 1, we can just add a new element with a random background color.
                    element* tmp = new element(backgroundColor(RGB(rand()%255,rand()%255,rand()%255)));
                    self->addChild(tmp);
                }
            })
        ))
    );

    while (true){
        for (int i = 0; i < 10; i++){

            // getElements<>() will return all elements of the specified class type in the main window.
            for (auto* scrollable : GGUI::getRoot()->getElements<scrollView>()){
                scrollable->scrollUp();
            }
            
            GGUI::SLEEP(16);
        }

        for (int i = 0; i < 10; i++){

            // getElements<>() will return all elements of the specified class type in the main window.
            for (auto* scrollable : GGUI::getRoot()->getElements<scrollView>()){
                scrollable->scrollDown();
            }

            GGUI::SLEEP(16);
        }
    }
}