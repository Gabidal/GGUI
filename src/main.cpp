#include "ggui.h"
#include "./core/utils/utils.h"

using namespace GGUI;

int main() 
{
    GGUI::GGUI(styling(
        childs({
            new listView(styling(
                childs({
                    new button(styling(text("File") | STYLES::border)),
                    new button(styling(text("Edit") | STYLES::border)),
                    new button(styling(text("View") | STYLES::border)),
                    new button(styling(text("Help") | STYLES::border))
                }) |
                STYLES::border
            )),
            new window(styling(
                title("A") | width(20) | height(10) | background_color(COLOR::MAGENTA) | text_color(COLOR::RED) | opacity(0.5f) | position(10, 10)
            )),
            new window(styling(
                title("B") | width(20) | height(10) | background_color(COLOR::YELLOW) | text_color(COLOR::GREEN) | opacity(0.5f) | position(30, 10)
            )),
            new window(styling(
                title("C") | width(20) | height(10) | background_color(COLOR::CYAN) | text_color(COLOR::BLUE) | opacity(0.5f) | position(20, 15)
            ))
        }) |
        title("Your App UI") |
        background_color(COLOR::WHITE) |
        text_color(COLOR::BLACK) |
        STYLES::border
    ), INT32_MAX);

    // // Then exit properly
    GGUI::EXIT();
}