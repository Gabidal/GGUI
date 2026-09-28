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
            (new listView{
                (new listView{
                    (new textField("File"))->onClick([](converter::output::event::base*){ return true; }),
                    (new textField("Edit"))->onClick([](converter::output::event::base*){ return true; }),
                    (new textField("View"))->onClick([](converter::output::event::base*){ return true; }),
                    (new textField("Help"))->onClick([](converter::output::event::base*){ return true; }),
                })->setBorder(true),
                
                (new listView{
                    (new element())->setTitle("A")->setDimensions({20, 10})->setBackgroundColor(COLOR::MAGENTA)->setTextColor(COLOR::RED)->setOpacity(0.5f)->setPosition({10, 10}),
                    (new element())->setTitle("B")->setDimensions({20, 10})->setBackgroundColor(COLOR::YELLOW)->setTextColor(COLOR::GREEN)->setOpacity(0.5f)->setPosition({30, 10}),
                    (new element())->setTitle("C")->setDimensions({20, 10})->setBackgroundColor(COLOR::CYAN)->setTextColor(COLOR::BLUE)->setOpacity(0.5f)->setPosition({20, 15})
                })->setBorder(true)
            })->setVertical()->setBorder(true)->setTitle("Your App UI")->setDimensions({1.0f, 1.0f})->setBackgroundColor(COLOR::WHITE)->setTextColor(COLOR::BLACK)
        );
    });

    waitForTermination();
}
