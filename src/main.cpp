#include <ggui.h>

using namespace GGUI;

canvas* canvas1 = new canvas(
    width(0.33f) | height(0.5f) | position(STYLES::center)
);

void directSetToCanvas() {
    for (int y = 0; y < canvas1->getHeight(); y++) {
        for (int x = 0; x < canvas1->getWidth(); x++) {
            // Giving more than one UTF to Sprite, will make the Sprite an animated Sprite, where the different colors are linearly interpolated.
            GGUI::sprite initialSprite(
                {
                    // UTF A
                    {"∆", {GGUI::COLOR::RED /*text color*/, GGUI::COLOR::RED /*background color*/}}, 
                    // UTF B
                    {"∇", {GGUI::COLOR::YELLOW /*text color*/, GGUI::COLOR::RED /*background color*/}}, 
                },
                x+y,  // <-- Animation offset, you can use perlin noise to make some nice wind styled animations
                1   // <-- Animation speed of the linear interpolation
            );

            canvas1->set(x, y, initialSprite, false);
        }
    }

    // canvas1->flush();    // No need, since this is called inside onInit().
}

struct block {
    unsigned int ID = 0;
};

sprite getSpriteFromId(block b) {
    if (b.ID == 0) {    // water
        return sprite({'~', {COLOR::LIGHT_BLUE, COLOR::BLUE}});
    } else if (b.ID == 1) { // sand
        return sprite({'.', {COLOR::ORANGE, COLOR::DARK_YELLOW}});
    } else if (b.ID == 2) { // grass
        return sprite({',', {COLOR::DARK_GREEN, COLOR::GREEN}});
    } else if (b.ID == 3) { // mountain
        return sprite({'^', {COLOR::LIGHT_GRAY, COLOR::GRAY}});
    } else {    // default
        return sprite({'#', {COLOR::WHITE, COLOR::BLACK}});
    }
}

IVector2 playerPosition = {0, 0};
void addMovementKeybindsBasedOnArrowKeys(element* self);
void addMovementKeybindsBasedOnWASD(element* self);

canvas* canvas2 = new canvas(
    width(0.33f) | height(0.5f) | position(STYLES::right) | 

    onDraw([](unsigned int x, unsigned int y){
        srand((playerPosition.y + y) * 256 + (playerPosition.x + x));

        return getSpriteFromId(block{static_cast<unsigned int>(rand() % 4)});
    }) | 

    onInit([](element* self){
        addMovementKeybindsBasedOnWASD(self);
    })
);

void addMovementKeybindsBasedOnArrowKeys(element* self) {
    self->on(constants::UP, [](event*){
        playerPosition.y -= 1;
        return true;
    });

    self->on(constants::DOWN, [](event*){
        playerPosition.y += 1;
        return true;
    });

    self->on(constants::LEFT, [](event*){
        playerPosition.x -= 1;
        return true;
    });

    self->on(constants::RIGHT, [](event*){
        playerPosition.x += 1;
        return true;
    });
}

void addMovementKeybindsBasedOnWASD(element* self) {
    self->on(constants::KEY_PRESS, [](event* e){
        auto* key = static_cast<input*>(e);

        if (key->data == 'w' || key->data == 'W') {
            playerPosition.y -= 1;
        } else if (key->data == 's' || key->data == 'S') {
            playerPosition.y += 1;
        } else if (key->data == 'a' || key->data == 'A') {
            playerPosition.x -= 1;
        } else if (key->data == 'd' || key->data == 'D') {
            playerPosition.x += 1;
        } else {
            return false;   // unknown input.
        }

        return true;
    });
}


int main() 
{
    GGUI::GGUI(
        // Animated canvas
        node(new canvas(    // canvas0
            // Set the canvas to third of screen size. 30%
            width(0.33f) | height(0.5f) | position(STYLES::left) |

            // This is called when the render for canvas is ready to comb through the sprite cells in this canvas.
            onRender([](element* self) {
                canvas* actualSelf = dynamic_cast<canvas*>(self);

                for (int y = 0; y < actualSelf->getHeight(); y++) {
                    for (int x = 0; x < actualSelf->getWidth(); x++) {
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

        node(canvas1) |
        onInit([](element*){
            std::thread t([](){
                directSetToCanvas();
            });

            t.detach();
        }) |

        node(canvas2)
    );
    
    // Your program...
    std::this_thread::sleep_for(std::chrono::seconds(INT32_MAX));
}
