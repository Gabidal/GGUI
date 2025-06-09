#include "ggui.h"

using namespace GGUI;

int main() 
{
    GGUI::GGUI(
        childs({
            new listView(
                childs({
                    new button(text("File")),
                    new button(text("Edit") | on_click([]([[maybe_unused]] element* self){ return true; })),
                    new button(text("View") | on_click([]([[maybe_unused]] element* self){ return true; })),
                    new button(text("Help") | on_click([]([[maybe_unused]] element* self){ return true; }))
                }) |
                enable_border(true)
            ),
            new element(
                title("A") | width(20) | height(10) | background_color(COLOR::MAGENTA) | text_color(COLOR::RED) | opacity(0.5f) | position(10, 10) | enable_border(true)
            ),
            new element(
                title("B") | width(20) | height(10) | background_color(COLOR::YELLOW) | text_color(COLOR::GREEN) | opacity(0.5f) | position(30, 10) | enable_border(true)
            ),
            new element(
                title("C") | width(20) | height(10) | background_color(COLOR::CYAN) | text_color(COLOR::BLUE) | opacity(0.5f) | position(20, 15) | enable_border(true)
            )
        }) |

        title("Your App UI") |
        background_color(COLOR::WHITE) |
        text_color(COLOR::BLACK) |
        enable_border(true) 
    );

    GGUI::INTERNAL::SLEEP(UINT32_MAX);

    // // Then exit properly. After 0.1.8 this is unnecessary, unless user overrides exit handlers.
    GGUI::EXIT();
}
