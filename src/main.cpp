#include "ggui.h"

using namespace GGUI;

int main(int argc, char* argv[]){
    
    // This will enable whatever the user gave the args with GGUI
    GGUI::SETTINGS::parseCommandLineArguments(argc, argv);

    GGUI::GGUI(
        backgroundColor(COLOR::YELLOW) | 
        node(new textField(
            allowOverflow(true) | width(0.5f) | height(10) | 
            onInput([](textField* self, char c){
                self->setText(self->getText() + c);
            })
        ))
    );

    std::this_thread::sleep_for(std::chrono::milliseconds(UINT32_MAX));
    EXIT();
}
