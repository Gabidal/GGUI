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
                listView::containerFlags::vertical,
                (new listView{
                    listView::containerFlags::DEFAULT,
                    (new textField("File"))->setBorder(true)->onClick([](converter::output::event::base*){ return true; }),
                    (new textField("Edit"))->setBorder(true)->onClick([](converter::output::event::base*){ return true; }),
                    (new textField("View"))->setBorder(true)->onClick([](converter::output::event::base*){ return true; }),
                    (new textField("Help"))->setBorder(true)->onClick([](converter::output::event::base*){ return true; }),
                })->setBorder(true)->setName("first list"),
                
                (new listView{
                    listView::containerFlags::DEFAULT,
                    (new element())->setTitle("A")->setDimensions({25, 12})->setPosition({0.50f, 0.5f})->setBackgroundColor(COLOR::MAGENTA)->setTextColor(COLOR::RED)->setOpacity(0.5f)->setBorder(true),
                    (new element())->setTitle("B")->setDimensions({25, 12})->setPosition({0.25f, 0.65f})->setBackgroundColor(COLOR::YELLOW)->setTextColor(COLOR::GREEN)->setOpacity(0.5f)->setBorder(true),
                    (new element())->setTitle("C")->setDimensions({25, 12})->setPosition({0.75f, 0.65f})->setBackgroundColor(COLOR::CYAN)->setTextColor(COLOR::BLUE)->setOpacity(0.5f)->setBorder(true)
                })->setName("second list")->setDimensions({1.0f, 1.0f})
            })->setBorder(true)->setTitle("Your App UI")->setDimensions({1.0f, 1.0f})
        );
    });

    sleep(10);
}
