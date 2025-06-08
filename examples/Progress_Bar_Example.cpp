#include "ggui.h"

using namespace std;
using namespace GGUI;
using namespace progress;

void update(){
    while (true){
        for (auto* i : GGUI::INTERNAL::Main->getElements<Bar>()){
            i->updateProgress(0.01);
        }

        GGUI::INTERNAL::SLEEP(100);
    }
}

int main(){
    GGUI::GGUI(
        // arrow like progressbar
        node(new Bar(
            width(1.0f) | 
            part(partType::TAIL, COLOR::GREEN, '#') | 
            part(partType::BODY, COLOR::GREEN, '=') | 
            part(partType::HEAD, COLOR::GREEN, '>')| 
            part(partType::EMPTY, COLOR::GRAY)
        )) | 

        // Default progressbar
        node(new Bar(
            width(1.0f) // Faults to the default style.
        )) | 

        // Blocky progressbar
        node(new Bar(
            width(1.0f) | height(3) | 
            part(partType::HEAD, COLOR::BLUE, SYMBOLS::FULL_BLOCK) |
            part(partType::BODY, COLOR::BLUE, SYMBOLS::FULL_BLOCK) | 
            part(partType::TAIL, COLOR::BLUE, SYMBOLS::FULL_BLOCK) | 
            part(partType::EMPTY, COLOR::RED)
        ))
    );

    update();

    GGUI::INTERNAL::SLEEP(INT32_MAX);
}