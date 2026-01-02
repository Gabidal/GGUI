/*

This c++ code is an wrapper which is to link with GGUI library and initialize GGUI and then just wait until timeout stops this program from time.sh

*/

#include <ggui.h>

int main() {
    GGUI::GGUI(
        GGUI::backgroundColor(GGUI::COLOR::BLACK)
    );

    GGUI::waitForTermination();
}

