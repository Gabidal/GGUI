#include "ggui.h"

using namespace GGUI;

int main() 
{
    GGUI::GGUI(
        title("Welcome to GGUI!") | 
        node(new textField( 
            position(STYLES::center) | 
            text("Hello World!")
        ))
    );

    while (true) {
        // ... 
    }
}