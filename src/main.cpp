#include "ggui.h"

using namespace GGUI;

int main()
{
    GGUI::GGUI(
        GGUI::backgroundColor(GGUI::COLOR::BLACK)
    );

    INTERNAL::SLEEP(10000);
    INTERNAL::EXIT();
}
