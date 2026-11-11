#include "elements/listView.h"
#include "elements/textField.h"
#include "core/core.h"

using namespace GGUI;

int main() {
    // As long as this object exists GGUI will be accessible
    gguiHandle app;
    
    pauseGGUI([&app](){
        app.setContent(
          (new listView{
            (new textField("Welcome to GGUI!"))->setTextAlignment(textField::alignments::CENTER),
          })->setVertical()->setPosition({0.5f, 0.5f})
        );
    });

    waitForTermination(); // Wait until user gives signal to close (ctrl+c)
}
