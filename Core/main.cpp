#include "ggui.h"

#include <math.h>

using namespace GGUI;

int main() 
{
    GGUI::GGUI(Styling(
        node(new Terminal_Canvas(Styling(

            width(1.0f) | height(1.0f) |

            on_draw([]([[maybe_unused]] unsigned int x, [[maybe_unused]] unsigned int y){
                return GGUI::Sprite(
                    {
                        {"a", {GGUI::COLOR::RED /*text color*/, GGUI::COLOR::RED /*background color*/}}, 
                        {"b", {GGUI::COLOR::BLUE, GGUI::COLOR::BLUE}}, 
                    },
                    0,  // Animation offset
                    1   // Animation speed
                );
            })
        )))
    ), 10000);

    GGUI::Exit();
}
