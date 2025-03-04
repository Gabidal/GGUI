#include "ggui.h"

#include <vector>

using namespace std;
using namespace GGUI;

int main(int Argument_Count, char** Arguments){
    GGUI::GGUI(styling(
        childs({
            new window(styling(
                title("A") |
                width(20) | height(10) |
                background_color(COLOR::MAGENTA) | text_color(COLOR::RED) |
                opacity(0.5f) |
                position(10, 10)
            )),
            new window(styling(
                title("B") |
                width(20) | height(10) |
                background_color(COLOR::YELLOW) | text_color(COLOR::GREEN) |
                opacity(0.5f) |
                position(30, 10)
            )),
            new window(styling(
                title("C") |
                width(20) | height(10) |
                background_color(COLOR::CYAN) | text_color(COLOR::BLUE) |
                opacity(0.5f) |
                position(20, 15)
            ))
        })

        // B->Show_Shadow(GGUI::COLOR::BLUE, 1, 3.0f);
    ));

    GGUI::IVector3 A_velocity = {1, 2};
    GGUI::IVector3 B_velocity = {3, 1};
    GGUI::IVector3 C_velocity = {1, 1};

    window* A = (window*)GGUI::INTERNAL::Main->getElement("A");
    window* B = (window*)GGUI::INTERNAL::Main->getElement("B");
    window* C = (window*)GGUI::INTERNAL::Main->getElement("C");

    while (true){
        GGUI::pauseGGUI();
        // Update the position of each window
        A->setPosition(A->getPosition() + A_velocity);
        B->setPosition(B->getPosition() + B_velocity);
        C->setPosition(C->getPosition() + C_velocity);

        // Check if any window hits an edge and reverse its direction
        if (A->getPosition().X <= 0 || A->getPosition().X + A->getProcessedWidth() >= GGUI::INTERNAL::Main->getProcessedWidth())
            A_velocity.X = -A_velocity.X;
        if (A->getPosition().Y <= 0 || A->getPosition().Y + A->getProcessedHeight() >= GGUI::INTERNAL::Main->getProcessedHeight())
            A_velocity.Y = -A_velocity.Y;

        if (B->getPosition().X <= 0 || B->getPosition().X + B->getProcessedWidth() >= GGUI::INTERNAL::Main->getProcessedWidth())
            B_velocity.X = -B_velocity.X;
        if (B->getPosition().Y <= 0 || B->getPosition().Y + B->getProcessedHeight() >= GGUI::INTERNAL::Main->getProcessedHeight())
            B_velocity.Y = -B_velocity.Y;

        if (C->getPosition().X <= 0 || C->getPosition().X + C->getProcessedWidth() >= GGUI::INTERNAL::Main->getProcessedWidth())
            C_velocity.X = -C_velocity.X;
        if (C->getPosition().Y <= 0 || C->getPosition().Y + C->getProcessedHeight() >= GGUI::INTERNAL::Main->getProcessedHeight())
            C_velocity.Y = -C_velocity.Y;
        
        // press 'CTRL + SHIFT + I' to open the inspect window. 
        GGUI::report(to_string(A->getPosition().X));
        GGUI::resumeGGUI();
        GGUI::INTERNAL::SLEEP(16);
    }

    GGUI::INTERNAL::SLEEP(INT32_MAX);
    
    // Then exit properly
    GGUI::EXIT();
}