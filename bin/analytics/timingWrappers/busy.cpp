/*

This c++ code is a wrapper made to link with GGUI libraries and init GGUI and setup an heavy per frame caller to exhaust opcodes until timeout kills it from time.sh

*/

#include <GGUI.h>

using namespace GGUI;

int main() {
    GGUI::GGUI(
        // Animated canvas
        node(new canvas(
            // Set the canvas to third of screen size. 30%
            width(1.0f) | height(1.0f) | position(STYLES::left) |

            // This is called when the render for canvas is ready to comb through the sprite cells in this canvas.
            onDraw([](unsigned int x, unsigned int y){
                // Giving more than one UTF to Sprite, will make the Sprite an animated Sprite, where the different colors are linearly interpolated.
                return GGUI::sprite(
                    {
                        // UTF A
                        {' ', {GGUI::COLOR::RED /*text color*/, GGUI::COLOR::RED /*background color*/}}, 
                        // UTF B
                        {' ', {GGUI::COLOR::BLUE /*text color*/, GGUI::COLOR::BLUE /*background color*/}} 
                    },
                    x+y,  // <-- Animation offset, you can use perlin noise to make some nice wind styled animations
                    1   // <-- Animation speed of the linear interpolation
                );
            })
        ))
    );

    GGUI::SLEEP(UINT32_MAX);
}