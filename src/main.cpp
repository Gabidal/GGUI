#include "elements/listView.h"
#include "elements/textField.h"
#include "core/core.h"

using namespace GGUI;

int main(int argc, char* argv[]){
    
    // This will enable whatever the user gave the args with GGUI
    // SETTINGS::parseCommandLineArguments(argc, argv);

    gguiHandle app;
    GGUI::pauseGGUI([&app](){
        app.setContent(
            ((new textField(""))->input([](textField* self, const std::string& e){
                self->appendText(e);

                return action::responses::DONE;
            }))->setBackgroundColor(COLOR::GRAY)->setDimensions({1.f, 1.f})
        );
    });

    waitForTermination();
}
