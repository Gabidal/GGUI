#include "ggui.h"

#include <vector>

using namespace std;
using namespace GGUI;

int main(){

    GGUI::GGUI(
        node(new scrollView(
            flow_priority(DIRECTION::ROW) | width(1.5f) | name("vertical scroll view") | background_color(COLOR::WHITE) | position(STYLES::center) | 
            on_init([](element* self){
                for (int i = 0; i < self->getWidth(); i++){
                    element* tmp = new element(background_color(RGB(rand()%255,rand()%255,rand()%255)));
                    self->addChild(tmp);
                }
            })
        )) | 
        
        node(new scrollView(
            flow_priority(DIRECTION::COLUMN)| height(1.5f) | name("horizontal scroll view") | background_color(COLOR::WHITE) | position(STYLES::center) | 
            on_init([](element* self){
                for (int i = 0; i < self->getHeight(); i++){
                    element* tmp = new element(background_color(RGB(rand()%255,rand()%255,rand()%255)));
                    self->addChild(tmp);
                }
            })
        ))
    );

    while (true){
        for (int i = 0; i < 10; i++){
            for (auto* scrollable : GGUI::INTERNAL::Main->getElements<scrollView>()){
                scrollable->scrollUp();
            }
            
            GGUI::INTERNAL::SLEEP(16);
        }

        for (int i = 0; i < 10; i++){
            for (auto* scrollable : GGUI::INTERNAL::Main->getElements<scrollView>()){
                scrollable->scrollDown();
            }

            GGUI::INTERNAL::SLEEP(16);
        }
    }

    GGUI::INTERNAL::SLEEP(INT32_MAX);
}