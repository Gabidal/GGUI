#include "ggui.h"

#include <math.h>

using namespace GGUI;

int main() 
{
    GGUI::GGUI(Styling(
        node(Terminal_Canvas(Styling(

            width(1.0f) | height(1.0f) |

            on_init([](Element* raw_self){
                Terminal_Canvas* self = (Terminal_Canvas*)raw_self;

                for (int x = 0; x < self->Get_Width(); x++)
                    for (int y = 0; y < self->Get_Height(); y++){
                        GGUI::Sprite s(
                            {
                                {"a", {GGUI::COLOR::RED /*text color*/, GGUI::COLOR::RED /*background color*/}}, 
                                {"b", {GGUI::COLOR::BLUE, GGUI::COLOR::BLUE}}, 
                            },
                            0,  // Animation offset
                            1   // Animation speed
                        );

                        self->Set(x, y, s, false);
                    }

                self->Flush(true);
            })
        )))
    ), INT32_MAX);

    GGUI::Exit();
}
