#include "ggui.h"

using namespace GGUI;

int main()
{
    GGUI::GGUI(
        GGUI::backgroundColor(GGUI::COLOR::BLACK)
    );

    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    EXIT();
}
