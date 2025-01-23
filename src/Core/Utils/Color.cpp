#include "color.h"

namespace GGUI{
    GGUI::RGB Lerp(GGUI::RGB A, GGUI::RGB B, float Distance) {
        if (SETTINGS::ENABLE_GAMMA_CORRECTION) {
            // Apply gamma correction to input values
            A.Red = Interpolate(A.Red, B.Red, Distance);
            A.Green = Interpolate(A.Green, B.Green, Distance);
            A.Blue = Interpolate(A.Blue, B.Blue, Distance);
        } else {
            // Perform linear interpolation on input values
            A.Red = static_cast<unsigned char>(lerp<float>(A.Red, B.Red, Distance));
            A.Green = static_cast<unsigned char>(lerp<float>(A.Green, B.Green, Distance));
            A.Blue = static_cast<unsigned char>(lerp<float>(A.Blue, B.Blue, Distance));
        }
        return A;
    }
}