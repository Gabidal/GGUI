#include "ggui.h"

using namespace GGUI;

int main() 
{
    GGUI::GGUI(
        // You want press this one, since it does not have an on_click handle
        node(new textField(
            position(STYLES::left) | 
            text("You Can't press me") | 
            anchor(ANCHOR::CENTER) | // <-- make the text centered
            enable_border(true)
        )) |

        // This one you can press because of the on_click event present
        node(new textField(
            position(STYLES::right) | 
            text("Press me") | 
            anchor(ANCHOR::CENTER) | // <-- make the text centered
            enable_border(true) | 
            on_click([](element* self){     // <-- Parameter 'self' is in this case the button object, you can safely static_cast it to an object*.
                return true;    // <-- Tell the GGUI event handler that this event completed successfully.
            })
        ))
    );

    while (true) {
        // ... 
    }
}