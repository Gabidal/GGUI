#include "ggui.h"
#include "./core/utils/utils.h"

using namespace GGUI;

int main() 
{
    GGUI::GGUI(styling(
        childs({
            new listView(styling(
                childs({
                    new button(styling(text("File") | on_click([]([[maybe_unused]] element* self){}))),
                    new button(styling(text("Edit") | on_click([]([[maybe_unused]] element* self){}))),
                    new button(styling(text("View") | on_click([]([[maybe_unused]] element* self){}))),
                    new button(styling(text("Help") | on_click([]([[maybe_unused]] element* self){})))
                }) |
                enable_border(true)
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
        enable_border(true)
    ));

    // GGUI::INTERNAL::SLEEP(1000);
    for (int i = 0; i < 100; i++){
        for (int j = 0; j < 10; j++){
            GGUI::report("[" + std::to_string(i) + std::to_string(j) + "]");
        }
    }

    GGUI::INTERNAL::SLEEP(INT32_MAX);

    // // Then exit properly
    GGUI::EXIT();
}