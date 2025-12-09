#include "ggui.h"

using namespace GGUI;

int main() 
{
    GGUI::GGUI(
        // Animated canvas
        node(new canvas(
            // Set the canvas to third of screen size. 30%
            width(0.33f) | height(0.5f) | position(STYLES::left) |

            // This is called when the render for canvas is ready to comb through the sprite cells in this canvas.
            onRender([](element* self) {
                canvas* actualSelf = dynamic_cast<canvas*>(self);

                for (unsigned int y = 0; y < actualSelf->getHeight(); y++) {
                    for (unsigned int x = 0; x < actualSelf->getWidth(); x++) {
                        // Giving more than one UTF to Sprite, will make the Sprite an animated Sprite, where the different colors are linearly interpolated.
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
        )) | 

        // Non-animated canvas with unicode emojis
        node(new canvas(
            // Set the canvas to third of screen size. 30%
            width(0.33f) | height(0.5f) | position(STYLES::center) |

            // This is called when the render for canvas is ready to comb through the sprite cells in this canvas.
            onRender([](element* self) {
                canvas* actualSelf = dynamic_cast<canvas*>(self);

                for (unsigned int y = 0; y < actualSelf->getHeight(); y++) {
                    for (unsigned int x = 0; x < actualSelf->getWidth(); x++) {
                        // Giving more than one UTF to Sprite, will make the Sprite an animated Sprite, where the different colors are linearly interpolated.
                        GGUI::sprite initialSprite(UTF(
                            "🗲",   // <-- Unicode characters are supported, but don't use too wide ones, since they currently break GGUI.
                            {
                                COLOR::WHITE,       // <-- Text color
                                COLOR::BLACK        // <-- back. 
                            }
                        ));

                        actualSelf->set(x, y, initialSprite, false);
                    }
                }
            })
        )) | 

        node(new canvas(
            // Set the canvas to third of screen size. 30%
            width(0.33f) | height(0.5f) | position(STYLES::right) |

            // This is called when the render for canvas is ready to comb through the sprite cells in this canvas.
            onRender([](element* self) {
                canvas* actualSelf = dynamic_cast<canvas*>(self);

                for (unsigned int y = 0; y < actualSelf->getHeight(); y++) {
                    for (unsigned int x = 0; x < actualSelf->getWidth(); x++) {
                        // Giving more than one UTF to Sprite, will make the Sprite an animated Sprite, where the different colors are linearly interpolated.
                        GGUI::sprite initialSprite(
                            {
                                // UTF A
                                {' ', {COLOR::BLACK /*text color*/, COLOR::BLACK /*background color*/}}, 
                                // UTF B
                                {' ', {COLOR::LIGHT_GRAY /*text color*/, COLOR::LIGHT_GRAY /*background color*/}}, 

                                {"A", {COLOR::LIGHT_RED /*text color*/, COLOR::LIGHT_GREEN /*background color*/}}, 

                                {' ', {COLOR::BLACK /*text color*/, COLOR::BLACK /*background color*/}}
                            },
                            x-y*y,  // <-- Animation offset, you can use perlin noise to make some nice wind styled animations
                            1+x   // <-- Animation speed of the linear interpolation
                        );

                        actualSelf->set(x, y, initialSprite, false);
                    }
                }
            })
        ))
    );

    while (true) {
        // ... 
    }
}
