#include "ggui.h"

using namespace std;
using namespace GGUI;
using namespace progress;

#include <random>

float getRandomFloat(float min, float max) {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_real_distribution<float> dist(min, max);
    return dist(gen);
}

void update(){
    while (true){
        for (auto* i : GGUI::INTERNAL::Main->getElements<Bar>()){
            i->updateProgress(getRandomFloat(0.005f, 0.00001f));
        }

        GGUI::INTERNAL::SLEEP(100);
    }
}

int main(){
    GGUI::GGUI(
        node(new listView(
            flow_priority(DIRECTION::COLUMN) | width(1.0f) | height(1.0f) | 

            // arrow like progressbar
            node(new Bar(
                width(1.0f) | 
                part(partType::TAIL, COLOR::LIGHT_YELLOW, '|') | 
                part(partType::BODY, COLOR::GREEN, '-') | 
                part(partType::HEAD, COLOR::YELLOW, '>')| 
                part(partType::EMPTY, COLOR::RED, '-')
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
            )) | 

            // weirdo
            node(new Bar(
                width(1.0f) | 
                part(partType::TAIL, COLOR::YELLOW, '<') | 
                part(partType::HEAD, COLOR::YELLOW, '>')
            )) | 
            
            // Blocky progressbar, with red back light
            node(new Bar(
                width(1.0f) |
                part(partType::HEAD, COLOR::GREEN, SYMBOLS::FULL_BLOCK) |
                part(partType::BODY, COLOR::GREEN, SYMBOLS::FULL_BLOCK) | 
                part(partType::TAIL, COLOR::GREEN, SYMBOLS::FULL_BLOCK) | 
                part(partType::EMPTY, COLOR::RED, SYMBOLS::FULL_BLOCK)
            ))
        ))
    );

    update();

    GGUI::INTERNAL::SLEEP(INT32_MAX);
}