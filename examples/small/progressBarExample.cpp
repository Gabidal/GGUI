#include <GGUI.h>

using namespace GGUI;
using namespace progress;

#include <random>

// Simple random generator
float getRandomFloat(float min, float max) {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_real_distribution<float> dist(min, max);
    return dist(gen);
}

void update(){
    while (true){
        for (auto* i : GGUI::getRoot()->getElements<Bar>()){
            i->updateProgress(getRandomFloat(0.00005f, 0.05f));     // use negative percentage to go backwards if for some reason needed.
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

int main(){
    GGUI::GGUI(
        node(new listView(
            flowPriority(DIRECTION::COLUMN) | width(1.0f) | height(1.0f) |     // Fullscreen, vertical list

            // arrow like progressbar
            node(new progress::Bar(
                width(1.0f) | 
                part(partType::TAIL, COLOR::LIGHT_YELLOW, '|') | 
                part(partType::BODY, COLOR::GREEN, '-') | 
                part(partType::HEAD, COLOR::YELLOW, '>')| 
                part(partType::EMPTY, COLOR::RED, '-')          // The empty part of the gauge is colored red of '-'
            )) | 
            
            // Default progressbar
            node(new Bar(
                width(1.0f) // Faults to the default style.
            )) | 
            
            // Blocky progressbar
            node(new Bar(
                width(1.0f) | height(3) | 
                part(partType::HEAD, COLOR::BLUE, SYMBOLS::FULL_BLOCK) |    // Since the color is for text color we can't just put ' ' space, we need to use a full character
                part(partType::BODY, COLOR::BLUE, SYMBOLS::FULL_BLOCK) | 
                part(partType::TAIL, COLOR::BLUE, SYMBOLS::FULL_BLOCK)
            )) | 

            // weirdo
            node(new Bar(
                width(1.0f) | 
                part(partType::TAIL, COLOR::YELLOW, '<') |                  // Not giving parts of a progress bar, will use the default values
                part(partType::HEAD, COLOR::YELLOW, '>')
            )) | 
            
            // Blocky progressbar, with red back light
            node(new Bar(
                width(1.0f) |
                part(partType::HEAD, COLOR::GREEN, SYMBOLS::FULL_BLOCK) |
                part(partType::BODY, COLOR::GREEN, SYMBOLS::FULL_BLOCK) | 
                part(partType::TAIL, COLOR::GREEN, SYMBOLS::FULL_BLOCK) | 
                part(partType::EMPTY, COLOR::RED, SYMBOLS::FULL_BLOCK)          // Empty part of the gauge 
            )) | 

            // reverse progressbar
            node(new Bar(
                width(1.0f) |
                part(partType::HEAD, COLOR::GREEN, '<') |
                part(partType::BODY, COLOR::BLACK, ' ') |       // Make the filled part invisible, the color can be anything, since space cannot be colored.
                part(partType::TAIL, COLOR::BLACK, ' ') | 
                part(partType::EMPTY, COLOR::GREEN, '-')
            ))
        ))
    );

    // call our own totally not realistic custom code
    update();
}