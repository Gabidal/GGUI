#include "ggui.h"

using namespace GGUI;

int main(int argc, char* argv[])
{
    GGUI::GGUI(
        GGUI::backgroundColor(GGUI::COLOR::BLACK)
    );

    INTERNAL::SLEEP(10000);
    INTERNAL::EXIT();
}
