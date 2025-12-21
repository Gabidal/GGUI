#include <ggui.h>

using namespace GGUI;

int main() 
{
    GGUI::GGUI(
        // You want press this one, since it does not have an on_click handle
        node(new textField(
            position(STYLES::left) |    // <-- anchor the text to the left side
            text("You Can't press me") | 
            anchor(ANCHOR::CENTER) |    // <-- make the text centered
            enableBorder(true)
        )) |

        // This one you can press because of the on_click event present
        node(new textField(
            position(STYLES::right) |   // <-- anchor the text to right side
            text("Press me") | 
            anchor(ANCHOR::CENTER) |    // <-- make the text centered
            enableBorder(true) | 
            onClick([](element*){      // <-- Self parameter
                return true;            // <-- Stop propagation of event
            })
        ))
    );

    // Your program...
    std::this_thread::sleep_for(std::chrono::milliseconds(UINT32_MAX));
}