#include "ggui.h"

using namespace GGUI;

int main() 
{
    GGUI::GGUI(
        childs({
            new listView(
                childs({
                    new button(text("File")),
                    new button(text("Edit") | onClick([]([[maybe_unused]] element* self){ return true; })),
                    new button(text("View") | onClick([]([[maybe_unused]] element* self){ return true; })),
                    new button(text("Help") | onClick([]([[maybe_unused]] element* self){ return true; }))
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

    GGUI::INTERNAL::SLEEP(UINT32_MAX);
}
