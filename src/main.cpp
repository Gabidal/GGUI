#include "ggui.h"

#include <math.h>

using namespace GGUI;

int main()
{
    GGUI::GGUI(styling(
        node(new element(styling(
            width(10) | height(10) | background_color(COLOR::RED) | name("A") | opacity(0.5f)
        )))
    ));

    GGUI::INTERNAL::SLEEP(1000);
    element* a = GGUI::getElement("A");

    a->setPosition({10, 10});

    GGUI::INTERNAL::SLEEP(INT16_MAX);

    GGUI::EXIT();
}
