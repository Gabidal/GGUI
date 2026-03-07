#include <ggui.h>

using namespace GGUI;

int main(int argc, char* argv[]) {

    // This will enable whatever the user gave the args with GGUI
    GGUI::SETTINGS::parseCommandLineArguments(argc, argv);

    GGUI::GGUI(
        node(new textField( 
            position(STYLES::center) | 
            text("Hello World!")
        ))
    );

    GGUI::waitForTermination(); // Wait until user gives signal to close (ctrl+c)
}