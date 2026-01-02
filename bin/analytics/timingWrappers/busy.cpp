/*

This c++ code is a wrapper made to link with GGUI libraries and init GGUI and setup an heavy per frame caller to exhaust opcodes until timeout kills it from time.sh

*/

#include <ggui.h>

using namespace GGUI;

int main() {
    GGUI::GGUI(
        // Animated canvas
        node(new canvas(
            width(1.0f) | height(1.0f) | position(STYLES::left) |

            onRender([](element* self) {
                canvas* actualSelf = dynamic_cast<canvas*>(self);

                for (int y = 0; y < actualSelf->getHeight(); y++) {
                    for (int x = 0; x < actualSelf->getWidth(); x++) {
                        // Set an initial sprite at each position
                        GGUI::sprite initialSprite(
                            {
                                // UTF A
                                {' ', {GGUI::COLOR::RED /*text color*/, GGUI::COLOR::RED /*background color*/}}, 
                                // UTF B
                                {' ', {GGUI::COLOR::BLUE /*text color*/, GGUI::COLOR::BLUE /*background color*/}} 
                            },
                            x+y,  // <-- Animation offset, you can use perlin noise to make some nice wind styled animations
                            1   // <-- Animation speed of the linear interpolation
                        );

                        actualSelf->set(x, y, initialSprite, false);
                    }
                }
            })
        ))
    );

    GGUI::waitForTermination();
}