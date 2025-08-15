#include "ggui.h"

using namespace GGUI;

int main(int argc, char* argv[])
{
    // This will enable whatever the user gave the args with GGUI
    GGUI::SETTINGS::parseCommandLineArguments(argc, argv);

    GGUI::GGUI(
        // Animated canvas
        node(new canvas(
            // Set the canvas to third of screen size. 30%
            width(0.33f) | height(0.5f) | position(STYLES::left) |

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
        )) | 

        // Non-animated canvas with unicode emojis
        node(new canvas(
            // Set the canvas to third of screen size. 30%
            width(0.33f) | height(0.5f) | position(STYLES::center) |

            // This is called when the render for canvas is ready to comb through the sprite cells in this canvas.
            onDraw([](unsigned int /*x*/, unsigned int /*y*/){
                // Giving more than one UTF to Sprite, will make the Sprite an animated Sprite, where the different colors are linearly interpolated.
                return GGUI::sprite(UTF(
                    "🗲",   // <-- Unicode characters are supported, but don't use too wide ones, since they currently break GGUI.
                    {
                        COLOR::WHITE,       // <-- Text color
                        COLOR::BLACK        // <-- back. 
                    }
                ));
            })
        )) | 

        node(new canvas(
            // Set the canvas to third of screen size. 30%
            width(0.33f) | height(0.5f) | position(STYLES::right) |

            // This is called when the render for canvas is ready to comb through the sprite cells in this canvas.
            onDraw([](unsigned int x, unsigned int y){
                // Giving more than one UTF to Sprite, will make the Sprite an animated Sprite, where the different colors are linearly interpolated.
                return GGUI::sprite(
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
            })
        ))
    );

    INTERNAL::SLEEP(1000);
    INTERNAL::EXIT();
}
