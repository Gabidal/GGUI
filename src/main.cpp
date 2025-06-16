#include "ggui.h"

using namespace GGUI;

int main() 
{
    GGUI::GGUI(
        title("Your App UI") | 
        node(new textField( 
            position(STYLES::center) | 
            text("Hello World!")
        ))
    );

    while (true) {
        // ... 
    }
}