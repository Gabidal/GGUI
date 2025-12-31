#include <ggui.h>

using namespace GGUI;

int main(int argc, char* argv[]){
    
    // This will enable whatever the user gave the args with GGUI
    GGUI::SETTINGS::parseCommandLineArguments(argc, argv);

    GGUI::GGUI(
        childs({
            new listView(
                childs({
                    new textField(text("File") | onClick([]([[maybe_unused]] element* self){ return true; }) | enableBorder(true)),
                    new textField(text("Edit") | onClick([]([[maybe_unused]] element* self){ return true; }) | enableBorder(true)),
                    new textField(text("View") | onClick([]([[maybe_unused]] element* self){ return true; }) | enableBorder(true)),
                    new textField(text("Help") | onClick([]([[maybe_unused]] element* self){ return true; }) | enableBorder(true))
                }) |
                enableBorder(true)
            ),
            new element(
                title("A") | width(20) | height(10) | backgroundColor(COLOR::MAGENTA) | textColor(COLOR::RED) | opacity(0.5f) | position(10, 10) | enableBorder(true)
            ),
            new element(
                title("B") | width(20) | height(10) | backgroundColor(COLOR::YELLOW) | textColor(COLOR::GREEN) | opacity(0.5f) | position(30, 10) | enableBorder(true)
            ),
            new element(
                title("C") | width(20) | height(10) | backgroundColor(COLOR::CYAN) | textColor(COLOR::BLUE) | opacity(0.5f) | position(20, 15) | enableBorder(true)
            )
        }) |

        title("Your App UI") |
        backgroundColor(COLOR::WHITE) |
        textColor(COLOR::BLACK) |
        enableBorder(true) 
    );

    GGUI::waitForTermination();
}
