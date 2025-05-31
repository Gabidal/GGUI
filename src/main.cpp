#include "ggui.h"

#include <math.h>

using namespace GGUI;

int main() 
{
    GGUI::GGUI(styling(
        node(new terminalCanvas(styling(

            width(1.0f) | height(1.0f) |

            on_draw([]([[maybe_unused]] unsigned int x, [[maybe_unused]] unsigned int y){
                return GGUI::sprite(
                    {
                        {" ", {GGUI::COLOR::RED /*text color*/, GGUI::COLOR::RED /*background color*/}}, 
                        {" ", {GGUI::COLOR::BLUE, GGUI::COLOR::BLUE}}, 
                    },
                    0,  // Animation offset
                    1   // Animation speed
                );
            })
        )))
    ));

    GGUI::INTERNAL::SLEEP(200000);

    // // Then exit properly. After 0.1.8 this is unnecessary, unless user overrides exit handlers.
    GGUI::EXIT();
}
