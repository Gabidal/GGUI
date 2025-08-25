#include <GGUI.h>

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
        if (A->getPosition().X <= 0 || A->getPosition().X + A->getWidth() >= self->getWidth())
            A_velocity.X = -A_velocity.X;
        if (A->getPosition().Y <= 0 || A->getPosition().Y + A->getHeight() >= self->getHeight())
            A_velocity.Y = -A_velocity.Y;

        if (B->getPosition().X <= 0 || B->getPosition().X + B->getWidth() >= self->getWidth())
            B_velocity.X = -B_velocity.X;
        if (B->getPosition().Y <= 0 || B->getPosition().Y + B->getHeight() >= self->getHeight())
            B_velocity.Y = -B_velocity.Y;

        if (C->getPosition().X <= 0 || C->getPosition().X + C->getWidth() >= self->getWidth())
            C_velocity.X = -C_velocity.X;
        if (C->getPosition().Y <= 0 || C->getPosition().Y + C->getHeight() >= self->getHeight())
            C_velocity.Y = -C_velocity.Y;

        GGUI::report(to_string(A->getPosition().X));
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

    std::this_thread::sleep_for(std::chrono::milliseconds(UINT32_MAX));
}