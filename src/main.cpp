#include "ggui.h"

using namespace GGUI;

int main() 
{
    GGUI::GGUI(styling(
        node(new button(styling(text("File") | STYLES::border)))
        // childs({
        //     new listView(styling(
        //         childs({
        //             new button(styling(text("File") | STYLES::border)),
        //             new button(styling(text("Edit") | STYLES::border)),
        //             new button(styling(text("View") | STYLES::border)),
        //             new button(styling(text("Help") | STYLES::border))
        //         }) |
        //         STYLES::border |
        //         background_color(COLOR::RED) | width(10) | height(10)
        //     )),
        //     new window(styling(
        //         title("A") | width(20) | height(10) | background_color(COLOR::MAGENTA) | text_color(COLOR::RED) | opacity(0.5f)
        //     )),
        //     new window(styling(
        //         title("B") | width(20) | height(10) | background_color(COLOR::YELLOW) | text_color(COLOR::GREEN) | opacity(0.5f)
        //     )),
        //     new window(styling(
        //         title("C") | width(20) | height(10) | background_color(COLOR::CYAN) | text_color(COLOR::BLUE) | opacity(0.5f)
        //     ))
        // }) | 
        // title("Your App UI") |
        // background_color(COLOR::WHITE) |
        // text_color(COLOR::BLACK) |
        // STYLES::border
    ), INT32_MAX);

    // // Then exit properly
    GGUI::EXIT();
}