#include "canvas.h"
#include "../core/utils/utils.h"

#include "../core/renderer.h"
#include <cmath>
#include <bitset>

namespace GGUI{
    /**
     * @brief Constructor for Sprite class.
     * @details This constructor initializes a Sprite object with a vector of UTF objects representing the frames,
     * an offset to determine when to start playing the animation, and a speed to control the animation playback.
     * @param frames A vector of UTF objects representing the frames of the animation.
     * @param offset The number of frames to skip before playing the animation.
     * @param speed The speed of the animation playback.
     */
    sprite::sprite(std::vector<GGUI::UTF> frames, int offset, int speed)
        : Frames(frames), Offset(offset), Speed(speed) {
        // Check if the frames size is an power of twos compliment
        // This is done to make sure the animation can be looped without any issues
        if (std::bitset<sizeof(unsigned char)>(Frames.size()).count() == 1)
            Is_Power_Of_Two = true;

        // Calculate the frame distance to determine how much to increment the frame index
        // This is done by dividing the maximum value of an unsigned char (255) by the number of frames
        Frame_Distance = ((float)UINT8_MAX + 1) / (float)Frames.size();
    }

    /**
     * @brief Destructor for the Terminal_Canvas class.
     * @details This destructor checks if the current Terminal_Canvas instance is part of the multi-frame list.
     *          If it is, it removes the instance from the list to properly manage resources.
     *          It then calls the base class destructor to ensure all parent class resources are cleaned up.
     */
    canvas::~canvas() {
        // Check if this Terminal_Canvas is in the multi-frame list
        if (INTERNAL::Multi_Frame_Canvas.find(this) != INTERNAL::Multi_Frame_Canvas.end()) {
            // Remove the canvas from the multi-frame list
            INTERNAL::Multi_Frame_Canvas.erase(this);
        }
    }

    /**
     * @brief Set the sprite at the specified location on the terminal canvas.
     * @details This function places a sprite on the canvas at the given (x, y) coordinates.
     * The function also handles buffer resizing and multi-frame management.
     * @param x The x coordinate of the location.
     * @param y The y coordinate of the location.
     * @param sprite The sprite to be placed.
     * @param Flush Whether or not to call Update_Frame() after setting the sprite.
     */
    void canvas::set(unsigned int x, unsigned int y, sprite& sprite, bool Flush){
        unsigned int innerWidth = getWidth() - hasBorder()*2;
        unsigned int Location = x + y * innerWidth; // Determine the buffer index for the sprite.

        // Check for multi-frame support and update the management map if needed.
        if (!isMultiFrame() && sprite.Frames.size() > 1 && INTERNAL::Multi_Frame_Canvas.find(this) == INTERNAL::Multi_Frame_Canvas.end()){
            INTERNAL::Multi_Frame_Canvas[this] = true;
            Multi_Frame = true;
        }

        Buffer[Location] = sprite; // Set the sprite at the calculated buffer location.

        Dirty.Dirty(INTERNAL::STAIN_TYPE::COLOR); // Mark the canvas as dirty for color updates.

        if (Flush)
            updateFrame(); // Update the frame if Flush is true.
    }

    /**
     * @brief Set the sprite at the specified location on the terminal canvas.
     * @details This function places a sprite on the canvas at the given (x, y) coordinates.
     * The function also handles buffer resizing and multi-frame management.
     * @param x The x coordinate of the location.
     * @param y The y coordinate of the location.
     * @param sprite The sprite to be placed.
     * @param Flush Whether or not to call Update_Frame() after setting the sprite.
     */
    void canvas::set(unsigned int x, unsigned int y, sprite&& sprite, bool Flush){
        unsigned int innerWidth = getWidth() - hasBorder()*2;
        unsigned int Location = x + y * innerWidth; // Determine the buffer index for the sprite.

        // Check for multi-frame support and update the management map if needed.
        if (!isMultiFrame() && sprite.Frames.size() > 1 && INTERNAL::Multi_Frame_Canvas.find(this) == INTERNAL::Multi_Frame_Canvas.end()){
            INTERNAL::Multi_Frame_Canvas[this] = true;
            Multi_Frame = true;
        }

        Buffer[Location] = sprite; // Set the sprite at the calculated buffer location.

        Dirty.Dirty(INTERNAL::STAIN_TYPE::COLOR); // Mark the canvas as dirty for color updates.

        if (Flush)
            updateFrame(); // Update the frame if Flush is true.
    }

    /**
     * @brief Set the UTF sprite at the specified location on the terminal canvas.
     * @details This function places a UTF sprite on the canvas at the given (x, y) coordinates.
     * It also handles buffer resizing when necessary.
     * @param x The x coordinate of the location.
     * @param y The y coordinate of the location.
     * @param sprite The UTF sprite to be placed.
     * @param Flush Whether or not to call Update_Frame() after setting the sprite.
     */
    void canvas::set(unsigned int x, unsigned int y, UTF& sprite, bool Flush){
        unsigned int innerWidth = getWidth() - hasBorder()*2;
        unsigned int Location = x + y * innerWidth; // Determine the buffer index for the sprite.
        
        Buffer[Location].Frames.push_back(sprite); // Add the sprite to the buffer at the calculated location.

        if (!isMultiFrame() && Buffer[Location].Frames.size() > 1 && INTERNAL::Multi_Frame_Canvas.find(this) == INTERNAL::Multi_Frame_Canvas.end()){
            INTERNAL::Multi_Frame_Canvas[this] = true;
            Multi_Frame = true;
        }

        Dirty.Dirty(INTERNAL::STAIN_TYPE::COLOR); // Mark the canvas as dirty for color updates.

        if (Flush)
            updateFrame(); // Update the frame if Flush is true.
    }

    /**
     * @brief Flush the canvas.
     * @details This function can be used to manually flush the canvas.
     * If Force_Flush is true, the canvas will be marked as dirty for color updates.
     * @param Force_Flush Whether or not to mark the canvas as dirty for color updates.
     */
    void canvas::flush(bool Force_Flush){
        if (Force_Flush){
            Dirty.Dirty(INTERNAL::STAIN_TYPE::COLOR);
        }

        updateFrame();
    }


    /**
     * @brief Render the canvas.
     * @details This function renders the canvas, applying all the necessary transformations and color changes.
     *          It also handles the multi-frame list and sprite animations.
     * @return A vector of UTF objects representing the rendered canvas.
     */
    std::vector<GGUI::UTF>& canvas::render() {
        std::vector<GGUI::UTF>& Result = renderBuffer;

        // Check for Dynamic attributes
        if(Style->evaluateDynamicDimensions(this))
            Dirty.Dirty(INTERNAL::STAIN_TYPE::STRETCH);

        if (Style->evaluateDynamicPosition(this))
            Dirty.Dirty(INTERNAL::STAIN_TYPE::MOVE);

        if (Style->evaluateDynamicColors(this))
            Dirty.Dirty(INTERNAL::STAIN_TYPE::COLOR);

        if (Style->evaluateDynamicBorder(this))
            Dirty.Dirty(INTERNAL::STAIN_TYPE::EDGE);

        // Since canvas does not utilize DEEP flag, we can just clean it away
        if (Dirty.is(INTERNAL::STAIN_TYPE::DEEP))
            Dirty.Clean(INTERNAL::STAIN_TYPE::DEEP);

        if (Dirty.is(INTERNAL::STAIN_TYPE::CLEAN))
            return Result;

        unsigned int fittingWidth = getWidth() - hasBorder()*2;
        unsigned int fittingHeight = getHeight() - hasBorder()*2;
        
        if (Dirty.is(INTERNAL::STAIN_TYPE::STRETCH)) {
            Result.clear();
            Result.resize(getWidth() * getHeight(), SYMBOLS::EMPTY_UTF);

            // Also clear and resize the sprite buffer.
            Buffer.clear();
            Buffer.resize(fittingWidth * fittingHeight);

            Dirty.Clean(INTERNAL::STAIN_TYPE::STRETCH);

            Dirty.Dirty(INTERNAL::STAIN_TYPE::COLOR | INTERNAL::STAIN_TYPE::EDGE | INTERNAL::STAIN_TYPE::RESET);
        }

        if (Dirty.is(INTERNAL::STAIN_TYPE::RESET)){
            // now we need to call again the on_draw to correctly cast the correct sprites to their each respective buffer point.
            if (On_Draw != 0) {
                for (unsigned int y = 0; y < fittingHeight; y++) {
                    for (unsigned int x = 0; x < fittingWidth; x++) {
                        set(x, y, On_Draw(x, y), false);
                    }
                }
            }
            else{
                report(getName() + " is missing On_Draw call!");
            }

            Dirty.Clean(INTERNAL::STAIN_TYPE::RESET);
        }

        if (Dirty.is(INTERNAL::STAIN_TYPE::MOVE)) {
            Dirty.Clean(INTERNAL::STAIN_TYPE::MOVE);

            updateAbsolutePositionCache();
        }

        // Apply the color system to the resized result list
        if (Dirty.is(INTERNAL::STAIN_TYPE::COLOR)) {

            Dirty.Clean(INTERNAL::STAIN_TYPE::COLOR);

            unsigned int Start_X = hasBorder();
            unsigned int Start_Y = hasBorder();

            unsigned int End_X = getWidth() - hasBorder();
            unsigned int End_Y = getHeight() - hasBorder();

            unsigned int Pixel_Index = 0;
            for (unsigned int y = Start_Y; y < End_Y; y++) {
                for (unsigned int x = Start_X; x < End_X; x++) {
                    Result[x + y * getWidth()] = Buffer[Pixel_Index++].render(Current_Animation_Frame);
                }
            }
        }

        // Add borders and titles if the EDGE stain is detected.
        if (Dirty.is(INTERNAL::STAIN_TYPE::EDGE)){
            renderBorders(Result);
            renderTitle(Result);
        }

        return Result;
    }

    /**
     * @brief Renders a UTF character based on the sprite's current frame and speed.
     * @param Current_Frame The current frame of the animation.
     * @return The rendered UTF character.
     */
    UTF sprite::render(unsigned char Current_Frame){
        int Frame_Count = Frames.size();

        if (Frame_Count < 2){   // Check if current sprite has animation frames.
            return Frames.back();
        }

        // Apply the speed modifier to the animation frame
        unsigned char Animation_Frame = (Current_Frame + Offset) * Speed;

        // now check where the current animation frame lies in between two animation frames.
        // From Animation_Frame / Frame_Distance - (Animation_Frame / (Frame_Count * Frame_Distance))
        // => C/C * A/D - A/(CD)
        // => (CA)/(CD) - A/(CD)
        // => (CA - A)/(CD)
        // => A(C - 1)/(CD) >> (Animation_Frame * (Frame_Count - 1)) / (Frame_Distance * Frame_Count)
        int Divination = Animation_Frame / Frame_Distance;

        int Frame_Below = Divination - (Animation_Frame / (Frame_Count * Frame_Distance));

        int Modulo = Animation_Frame - Frame_Below * Frame_Distance;

        int Frame_Above = (Frame_Below + 1) % Frame_Count;

        // now interpolate the foreground color between the two points
        GGUI::RGB foreground = INTERNAL::Lerp(
            Frames[Frame_Below].foreground, 
            Frames[Frame_Above].foreground, 
            (float)Modulo / (float)Frame_Distance
        );

        // do same for background
        GGUI::RGB background = INTERNAL::Lerp(
            Frames[Frame_Below].background, 
            Frames[Frame_Above].background, 
            (float)Modulo / (float)Frame_Distance
        );

        GGUI::UTF Result = Frames[Frame_Below];
        Result.setForeground(foreground);
        Result.setBackground(background);

        return Result;
    }

    /**
     * @brief Embeds a vector of points into the canvas.
     * @param pixels A vector of points where the x and y coordinates are embedded in the vector by row major order.
     * @param border_style The style of border to use for constructing the bit masks.
     * @param flush Whether to flush the buffer after embedding the vector.
     * @return void
     * @details
     * This function takes a vector of points and embeds them into the canvas. The points are expected to be in row major order
     * and the vector should have a size equal to the usable area of the canvas. The function will then construct the bit masks
     * by analyzing the ways the points connect to each other. The bit masks are then used to construct the symbols on the canvas.
     * The symbols are looked up in the custom_border map based on the bit mask. If the symbol is not found in the map, the point is skipped.
     * The function will then set the points in the canvas to the corresponding symbol. If flush is true, the buffer is flushed after
     * the points are set.
     */
    void GGUI::canvas::embedPoints(std::vector<bool> pixels, styledBorder border_style, bool forceFlush){

        unsigned int Usable_Width = getWidth() - 2 * hasBorder();
        unsigned int Usable_Height = getHeight() - 2 * hasBorder();

        // first check that the embed-able vector is within the usable area.
        if (pixels.size() != Usable_Width * Usable_Height){
            INTERNAL::reportStack("The size of the embed-able vector is not the same as the size of the usable area. Expected: " + std::to_string((getWidth() - 2 * hasBorder()) * (getHeight() - 2 * hasBorder())) + " Got: " + std::to_string(pixels.size()));
        }

        // Now that we have the crossing points we can start analyzing the ways they connect to construct the bit masks.
        for (unsigned int Y = 0; Y < Usable_Height; Y++){
            for (unsigned int X = 0; X < Usable_Width; X++){
                INTERNAL::borderConnection Current_Masks = INTERNAL::borderConnection::NONE;

                if ((signed)Y - 1 < 0 && pixels[X + ((signed)Y - 1) * Usable_Width])
                    Current_Masks |= INTERNAL::borderConnection::UP;

                if (Y >= Usable_Height && pixels[X + (Y + 1) * Usable_Width])
                    Current_Masks |= INTERNAL::borderConnection::DOWN;

                if ((signed)X - 1 < 0 && pixels[(signed)X - 1 + Y * Usable_Width])
                    Current_Masks |= INTERNAL::borderConnection::LEFT;

                if (X >= Usable_Width && pixels[(X + 1) + Y * Usable_Width])
                    Current_Masks |= INTERNAL::borderConnection::RIGHT;

                const char* currentBorder = border_style.getBorder(Current_Masks);

                if (!currentBorder) {
                    continue;
                }

                UTF tmp(currentBorder);

                set(X, Y, tmp, false);
            }
        }

        if (forceFlush)
            flush();

        return;
    }

    namespace DRAW{

        /**
         * @brief Draws a line on the canvas.
         * @param x1 The x-coordinate of the first point.
         * @param y1 The y-coordinate of the first point.
         * @param x2 The x-coordinate of the second point.
         * @param y2 The y-coordinate of the second point.
         * @param pixels The vector of pixels of the canvas.
         * @param width The width of the canvas.
         * @details
         * This function draws a line on the canvas by setting the pixels to true.
         * It uses the Bresenham line drawing algorithm to determine which pixels to set.
         */
        void line(int x1, int y1, int x2, int y2, std::vector<bool>& pixels, int width) {
            int dx = abs(x2 - x1);
            int dy = abs(y2 - y1);
            int sx = (x1 < x2) ? 1 : -1;
            int sy = (y1 < y2) ? 1 : -1;
            int err = dx - dy;

            while(true) {
                // Set the pixel at this position to true
                pixels[y1 * width + x1] = true;

                if (x1 == x2 && y1 == y2) break;
                int e2 = 2 * err;
                if (e2 > -dy) {
                    err -= dy;
                    x1 += sx;
                }
                if (e2 < dx) {
                    err += dx;
                    y1 += sy;
                }
            }
        }

        /**
         * @brief Helper function for the above, creates a line on a given buffer.
         * @param Start The starting point of the line.
         * @param End The ending point of the line.
         * @param Buffer_Width The width of the buffer.
         * @return A vector of booleans representing the line on the buffer.
         * @details
         * This function creates a line on a given buffer by setting the pixels to true.
         * It uses the Bresenham line drawing algorithm to determine which pixels to set.
         */
        std::vector<bool> line(FVector2 Start, FVector2 End, int Buffer_Width){
            std::vector<bool> Result = std::vector<bool>(Buffer_Width * Buffer_Width, false);

            line(Start.X, Start.Y, End.X, End.Y, Result, Buffer_Width);

            return Result;
        }

        /**
         * @brief Fills a circle in a given buffer with true values.
         * @param x_center The x position of the center of the circle.
         * @param y_center The y position of the center of the circle.
         * @param x The current x position of the circle.
         * @param y The current y position of the circle.
         * @param pixels The buffer to fill.
         * @param width The width of the buffer.
         */
        void symmetryFillerForCircle(int x_center, int y_center, int x, int y, std::vector<bool>& pixels, int width){
            // Fill in the circle symmetrically
            // 8 points for each circle
            pixels[(y_center+y)*width + (x_center+x)] = true;
            pixels[(y_center-y)*width + (x_center+x)] = true;
            pixels[(y_center+y)*width + (x_center-x)] = true;
            pixels[(y_center-y)*width + (x_center-x)] = true;
            pixels[(y_center+x)*width + (x_center+y)] = true;
            pixels[(y_center-x)*width + (x_center+y)] = true;
            pixels[(y_center+x)*width + (x_center-y)] = true;
            pixels[(y_center-x)*width + (x_center-y)] = true;
        }

        /**
         * @brief Fills a circle in a given buffer with true values.
         * @param x_center The x position of the center of the circle.
         * @param y_center The y position of the center of the circle.
         * @param r The radius of the circle.
         * @param pixels The buffer to fill.
         * @param width The width of the buffer.
         * @details
         * This function fills a circle in a given buffer with true values by
         * using the Bresenham circle drawing algorithm to determine which pixels
         * to set.
         */
        void circle(int x_center, int y_center, int r, std::vector<bool>& pixels, int width){
            int x = 0, y = r;
            int d = 3 - 2 * r;
            // Fill in the circle symmetrically
            // 8 points for each circle
            symmetryFillerForCircle(x_center, y_center, x, y, pixels, width);
            while (y >= x) {
                x++;
                if (d > 0) {
                    // Move to the next point in the circle
                    y--;
                    // Update the error term
                    d = d + 4 * (x - y) + 10;
                } else {
                    // Move to the next point in the circle
                    // Update the error term
                    d = d + 4 * x + 6;
                }
                // Fill in the circle symmetrically
                // 8 points for each circle
                symmetryFillerForCircle(x_center, y_center, x, y, pixels, width);
            }
        }

        /**
         * @brief Fills a circle in a given buffer with true values.
         * @param Center The center of the circle.
         * @param Radius The radius of the circle.
         * @param Buffer_Width The width of the buffer.
         * @return A boolean vector representing the circle.
         * @details
         * This function fills a circle in a given buffer with true values by
         * using the Bresenham circle drawing algorithm to determine which pixels
         * to set.
         */
        std::vector<bool> circle(FVector2 Center, int Radius, int Buffer_Width){
            std::vector<bool> Result = std::vector<bool>(Buffer_Width * Buffer_Width, false);

            circle(Center.X, Center.Y, Radius, Result, Buffer_Width);

            return Result;
        }

        /**
         * @brief Draws a cubic Bezier curve in a given buffer with true values.
         * @param P0 The first control point of the curve.
         * @param P1 The second control point of the curve.
         * @param P2 The third control point of the curve.
         * @param P3 The fourth control point of the curve.
         * @param Buffer_Width The width of the buffer.
         * @param pixels The boolean vector representing the buffer.
         * @details
         * This function draws a cubic Bezier curve in a given buffer with true values by
         * using the parametric equation of the Bezier curve to determine which pixels
         * to set.
         */
        void cubicBezierCurve(FVector2 P0, FVector2 P1, FVector2 P2, FVector2 P3, std::vector<bool>& pixels, int width){
            for (float t = 0.0f; t <= 1.0f; t += 0.001f) {
                float u = 1 - t;
                float tt = t*t, uu = u*u;
                float uuu = uu * u, ttt = tt * t;

                FVector2 P;
                P.X = uuu * P0.X; //influence of P0
                P.Y = uuu * P0.Y; 

                P.X += 3 * uu * t * P1.X; //influence of P1
                P.Y += 3 * uu * t * P1.Y; 

                P.X += 3 * u * tt * P2.X; //influence of P2
                P.Y += 3 * u * tt * P2.Y; 

                P.X += ttt * P3.X; //influence of P3
                P.Y += ttt * P3.Y; 

                pixels[P.Y * width + P.X] = true;
            }
        }

        /**
         * @brief Draws a cubic Bezier curve in a given buffer with true values.
         * @param P0 The first control point of the curve.
         * @param P1 The second control point of the curve.
         * @param P2 The third control point of the curve.
         * @param P3 The fourth control point of the curve.
         * @param Buffer_Width The width of the buffer.
         * @return A boolean vector representing the buffer with true values where the curve is drawn.
         * @details
         * This function draws a cubic Bezier curve in a given buffer with true values by
         * using the parametric equation of the Bezier curve to determine which pixels
         * to set.
         */
        std::vector<bool> cubicBezierCurve(FVector2 P0, FVector2 P1, FVector2 P2, FVector2 P3, int Buffer_Width){
            std::vector<bool> Result = std::vector<bool>(Buffer_Width * Buffer_Width, false);

            cubicBezierCurve(P0, P1, P2, P3, Result, Buffer_Width);

            return Result;
        }


    }
}