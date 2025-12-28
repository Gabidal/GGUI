#include <ggui.h>

#include <vector>

using namespace std;
using namespace GGUI;

void foo(element* self){
    GGUI::IVector3 A_velocity = {1, 2};
    GGUI::IVector3 B_velocity = {3, 1};
    GGUI::IVector3 C_velocity = {2, 3};

    element* A = self->getElement("A");
    element* B = self->getElement("B");
    element* C = self->getElement("C");

    // return;

    while (true){
        pauseGGUI();
        // Update the position of each window
        A->updatePosition(A_velocity);
        B->updatePosition(B_velocity);
        C->updatePosition(C_velocity);

        // Check if any window hits an edge and reverse its direction
        if (A->getPosition().x <= 0 || A->getPosition().x + A->getWidth() >= self->getWidth())
            A_velocity.x = -A_velocity.x;
        if (A->getPosition().y <= 0 || A->getPosition().y + A->getHeight() >= self->getHeight())
            A_velocity.y = -A_velocity.y;

        if (B->getPosition().x <= 0 || B->getPosition().x + B->getWidth() >= self->getWidth())
            B_velocity.x = -B_velocity.x;
        if (B->getPosition().y <= 0 || B->getPosition().y + B->getHeight() >= self->getHeight())
            B_velocity.y = -B_velocity.y;

        if (C->getPosition().x <= 0 || C->getPosition().x + C->getWidth() >= self->getWidth())
            C_velocity.x = -C_velocity.x;
        if (C->getPosition().y <= 0 || C->getPosition().y + C->getHeight() >= self->getHeight())
            C_velocity.y = -C_velocity.y;

        GGUI::report(to_string(A->getPosition().x));
        resumeGGUI();

        // press 'CTRL + SHIFT + I' to open the inspect window. 
        std::this_thread::sleep_for(std::chrono::milliseconds(16));
    }
}

int main(){
    GGUI::GGUI(
        childs({
            new element(
                title("A") |
                width(20) | height(10) |
                backgroundColor(COLOR::MAGENTA) | textColor(COLOR::RED) |
                opacity(0.5f) |
                position(10, 10)
            ),
            new element(
                title("B") |
                width(20) | height(10) |
                backgroundColor(COLOR::YELLOW) | textColor(COLOR::GREEN) |
                opacity(0.5f) |
                position(30, 10)
            ),
            new element( 
                title("C") |
                width(20) | height(10) |
                backgroundColor(COLOR::CYAN) | textColor(COLOR::BLUE) |
                opacity(0.5f) |
                position(20, 15)
            )
        }) | 

        onInit([](element* self){
            foo(self);
        }) | 

        name("Main window")
    );

    GGUI::waitForTermination();
}