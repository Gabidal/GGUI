#include "Canvas.h"

#include "../Core/Renderer.h"
#include <cmath>
#include <bitset>

#include "../Core/SIMD/SIMD.h"

namespace GGUI{
    
    /**
     * @brief Set the value of a pixel on the canvas.
     * @details
     * This function sets the value of a pixel on the canvas. The coordinates are in pixels relative to the top left corner of the canvas.
     * The color is an RGB object. The Flush argument determines whether or not to call Update_Frame() after setting the pixel.
     * @param x The x coordinate of the pixel.
     * @param y The y coordinate of the pixel.
     * @param color The color of the pixel.
     * @param Flush Whether or not to call Update_Frame() after setting the pixel.
     */
    void Canvas::Set(unsigned int x, unsigned int y, RGB color, bool Flush){
        unsigned int Actual_X = x + Has_Border(); // Add the border offset to the x coordinate.
        unsigned int Actual_Y = y + Has_Border(); // Add the border offset to the y coordinate.

        Buffer[Actual_X + Actual_Y * Get_Width()] = color; // Set the color of the pixel at the calculated coordinates.

        Dirty.Dirty(STAIN_TYPE::COLOR); // Mark the canvas as dirty so that it will be updated the next time Update_Frame() is called.

        if (Flush)
            Update_Frame(); // If Flush is true, call Update_Frame() to update the canvas.
    }


    /**
     * @brief Flushes the canvas.
     * @details
     * This function flushes the canvas by calling Update_Frame().
     * It is used to update the canvas immediately after making changes to it.
     */
    void Canvas::Flush(){
        // Call Update_Frame() to update the canvas.
        Update_Frame();
    }

    /**
     * @brief Renders the canvas and returns the result.
     * @details This function processes the canvas to generate a vector of UTF objects representing the current state.
     * It handles different stains such as CLASS, STRETCH, COLOR, and EDGE to ensure the canvas is rendered correctly.
     * @return A vector of UTF objects representing the rendered canvas.
     */
    std::vector<GGUI::UTF>& Canvas::Render() {
        std::vector<GGUI::UTF>& Result = Render_Buffer;

        // If the canvas is clean, return the current render buffer.
        if (Dirty.is(STAIN_TYPE::CLEAN))
            return Result;

        // Parse classes if the CLASS stain is detected.
        if (Dirty.is(STAIN_TYPE::CLASS)) {
            Parse_Classes();
            Dirty.Clean(STAIN_TYPE::CLASS);
        }

        // Handle the STRETCH stain by evaluating dynamic attributes and resizing the result buffer.
        if (Dirty.is(STAIN_TYPE::STRETCH)) {
            Style->Evaluate_Dynamic_Attribute_Values(this);
            Result.clear();
            Result.resize(Get_Width() * Get_Height());
            Dirty.Clean(STAIN_TYPE::STRETCH);
            Dirty.Dirty(STAIN_TYPE::COLOR | STAIN_TYPE::EDGE);
        }

        // Apply the color system if the COLOR stain is detected.
        if (Dirty.is(STAIN_TYPE::COLOR)) {
            Dirty.Clean(STAIN_TYPE::COLOR);

            unsigned int Start_X = Has_Border();
            unsigned int Start_Y = Has_Border();
            unsigned int End_X = Get_Width() - Has_Border();
            unsigned int End_Y = Get_Height() - Has_Border();

            // Loop over each pixel within the border to set the background color.
            for (unsigned int y = Start_Y; y < End_Y; y++) {
                for (unsigned int x = Start_X; x < End_X; x++) {
                    UTF Current_Pixel;
                    Current_Pixel.Set_Background(Buffer[x + y * Get_Width()]);
                    Result[x + y * Get_Width()] = Current_Pixel;
                }
            }
        }

        // Add borders and titles if the EDGE stain is detected.
        if (Dirty.is(STAIN_TYPE::EDGE))
            Add_Overhead(this, Result);

        return Result;
    }

    /**
     * @brief Constructor for Sprite class.
     * @details This constructor initializes a Sprite object with a vector of UTF objects representing the frames,
     * an offset to determine when to start playing the animation, and a speed to control the animation playback.
     * @param frames A vector of UTF objects representing the frames of the animation.
     * @param offset The number of frames to skip before playing the animation.
     * @param speed The speed of the animation playback.
     */
    Sprite::Sprite(std::vector<GGUI::UTF> frames, int offset, int speed)
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
    Terminal_Canvas::~Terminal_Canvas() {
        // Check if this Terminal_Canvas is in the multi-frame list
        if (Multi_Frame_Canvas.find(this) != Multi_Frame_Canvas.end()) {
            // Remove the canvas from the multi-frame list
            Multi_Frame_Canvas.erase(this);
        }

        // Call the base class destructor to clean up parent resources
        Element::~Element();
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
    void Terminal_Canvas::Set(unsigned int x, unsigned int y, Sprite& sprite, bool Flush){
        unsigned int Actual_X = x + Has_Border(); // Calculate the actual x coordinate, accounting for border offset.
        unsigned int Actual_Y = y + Has_Border(); // Calculate the actual y coordinate, accounting for border offset.

        unsigned int Location = Actual_X + Actual_Y * Get_Width(); // Determine the buffer index for the sprite.

        // Resize buffer if necessary to match the width x height of the canvas.
        if (Buffer.size() == 0 || (Location > Buffer.size() && Location <= Get_Width() * Get_Height())){
            Buffer.resize(Get_Width() * Get_Height());
        }

        // Check for multi-frame support and update the management map if needed.
        if (!Is_Multi_Frame() && sprite.Frames.size() > 1 && Multi_Frame_Canvas.find(this) == Multi_Frame_Canvas.end()){
            Multi_Frame_Canvas[this] = true;
            Multi_Frame = true;
        }

        Buffer[Location] = sprite; // Set the sprite at the calculated buffer location.

        Dirty.Dirty(STAIN_TYPE::COLOR); // Mark the canvas as dirty for color updates.

        if (Flush)
            Update_Frame(); // Update the frame if Flush is true.
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
    void Terminal_Canvas::Set(unsigned int x, unsigned int y, UTF& sprite, bool Flush){
        unsigned int Actual_X = x + Has_Border(); // Calculate the actual x coordinate, accounting for border offset.
        unsigned int Actual_Y = y + Has_Border(); // Calculate the actual y coordinate, accounting for border offset.

        unsigned int Location = Actual_X + Actual_Y * Get_Width(); // Determine the buffer index for the sprite.

        // Resize buffer if necessary to match the width x height of the canvas.
        if (Buffer.size() == 0 || (Location > Buffer.size() && Location <= Get_Width() * Get_Height())){
            Buffer.resize(Get_Width() * Get_Height());
        }

        Buffer[Location].Frames.push_back(sprite); // Add the sprite to the buffer at the calculated location.

        Dirty.Dirty(STAIN_TYPE::COLOR); // Mark the canvas as dirty for color updates.

        if (Flush)
            Update_Frame(); // Update the frame if Flush is true.
    }

    /**
     * @brief Flush the canvas.
     * @details This function can be used to manually flush the canvas.
     * If Force_Flush is true, the canvas will be marked as dirty for color updates.
     * @param Force_Flush Whether or not to mark the canvas as dirty for color updates.
     */
    void Terminal_Canvas::Flush(bool Force_Flush){
        if (Force_Flush){
            Dirty.Dirty(STAIN_TYPE::COLOR);
        }

        Update_Frame();
    }


    /**
     * @brief Render the canvas.
     * @details This function renders the canvas, applying all the necessary transformations and color changes.
     *          It also handles the multi-frame list and sprite animations.
     * @return A vector of UTF objects representing the rendered canvas.
     */
    std::vector<GGUI::UTF>& Terminal_Canvas::Render() {
        std::vector<GGUI::UTF>& Result = Render_Buffer;

        if (Dirty.is(STAIN_TYPE::CLEAN))
            return Result;

        if (Dirty.is(STAIN_TYPE::CLASS)) {
            Parse_Classes();

            Dirty.Clean(STAIN_TYPE::CLASS);
        }

        if (Dirty.is(STAIN_TYPE::STRETCH)) {
            // This needs to be called before the actual stretch, since the actual Width and Height have already been modified to the new state, and we need to make sure that is correct according to the percentile of the dynamic attributes that follow the parents diction.
            Style->Evaluate_Dynamic_Attribute_Values(this);

            Result.clear();
            Result.resize(Get_Width() * Get_Height(), SYMBOLS::EMPTY_UTF);
            Dirty.Clean(STAIN_TYPE::STRETCH);

            Dirty.Dirty(STAIN_TYPE::COLOR | STAIN_TYPE::EDGE);
        }

        if (Dirty.is(STAIN_TYPE::MOVE)) {
            Dirty.Clean(STAIN_TYPE::MOVE);

            Update_Absolute_Position_Cache();
        }

        // Apply the color system to the resized result list
        if (Dirty.is(STAIN_TYPE::COLOR)) {

            Dirty.Clean(STAIN_TYPE::COLOR);

            unsigned int Start_X = Has_Border();
            unsigned int Start_Y = Has_Border();

            unsigned int End_X = Get_Width() - Has_Border();
            unsigned int End_Y = Get_Height() - Has_Border();

            unsigned int Pixel_Index = 0;
            for (unsigned int y = Start_Y; y < End_Y; y++) {
                for (unsigned int x = Start_X; x < End_X; x++) {
                    Result[x + y * Get_Width()] = Buffer[Pixel_Index++].Render(Current_Animation_Frame);
                }
            }
        }

        // This will add the borders if necessary.
        if (Dirty.is(STAIN_TYPE::EDGE))
            Add_Overhead(this, Result);

        return Result;
    }

    /**
     * @brief Renders a UTF character based on the sprite's current frame and speed.
     * @param Current_Frame The current frame of the animation.
     * @return The rendered UTF character.
     */
    UTF Sprite::Render(unsigned char Current_Frame){
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
        GGUI::RGB foreground = Lerp(
            Frames[Frame_Below].Foreground, 
            Frames[Frame_Above].Foreground, 
            (float)Modulo / (float)Frame_Distance
        );

        // do same for background
        GGUI::RGB background = Lerp(
            Frames[Frame_Below].Background, 
            Frames[Frame_Above].Background, 
            (float)Modulo / (float)Frame_Distance
        );

        GGUI::UTF Result = Frames[Frame_Below];
        Result.Set_Foreground(foreground);
        Result.Set_Background(background);

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
    void GGUI::Terminal_Canvas::Embed_Points(std::vector<bool> pixels, styled_border border_style, bool flush){

        unsigned int Usable_Width = Get_Width() - 2 * Has_Border();
        unsigned int Usable_Height = Get_Height() - 2 * Has_Border();

        // first check that the embed-able vector is within the usable area.
        if (pixels.size() != Usable_Width * Usable_Height){
            Report_Stack("The size of the embed-able vector is not the same as the size of the usable area. Expected: " + std::to_string((Get_Width() - 2 * Has_Border()) * (Get_Height() - 2 * Has_Border())) + " Got: " + std::to_string(pixels.size()));
        }

        std::unordered_map<unsigned int, const char*> custom_border = Get_Custom_Border_Map(border_style);

        // Now that we have the crossing points we can start analyzing the ways they connect to construct the bit masks.
        for (unsigned int Y = 0; Y < Usable_Height; Y++){
            for (unsigned int X = 0; X < Usable_Width; X++){
                unsigned int Current_Masks = 0;

                if ((signed)Y - 1 < 0 && pixels[X + ((signed)Y - 1) * Usable_Width])
                    Current_Masks |= SYMBOLS::CONNECTS_UP;

                if (Y >= Usable_Height && pixels[X + (Y + 1) * Usable_Width])
                    Current_Masks |= SYMBOLS::CONNECTS_DOWN;

                if ((signed)X - 1 < 0 && pixels[(signed)X - 1 + Y * Usable_Width])
                    Current_Masks |= SYMBOLS::CONNECTS_LEFT;

                if (X >= Usable_Width && pixels[(X + 1) + Y * Usable_Width])
                    Current_Masks |= SYMBOLS::CONNECTS_RIGHT;

                if (custom_border.find(Current_Masks) == custom_border.end())
                    continue;

                UTF tmp(custom_border[Current_Masks]);

                Set(X, Y, tmp, false);
            }
        }

        if (flush)
            Flush();

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
        void Line(int x1, int y1, int x2, int y2, std::vector<bool>& pixels, int width) {
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
        std::vector<bool> Line(FVector2 Start, FVector2 End, int Buffer_Width){
            std::vector<bool> Result = std::vector<bool>(Buffer_Width * Buffer_Width, false);

            Line(Start.X, Start.Y, End.X, End.Y, Result, Buffer_Width);

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
        void Symmetry_Filler_For_Circle(int x_center, int y_center, int x, int y, std::vector<bool>& pixels, int width){
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
        void Circle(int x_center, int y_center, int r, std::vector<bool>& pixels, int width){
            int x = 0, y = r;
            int d = 3 - 2 * r;
            // Fill in the circle symmetrically
            // 8 points for each circle
            Symmetry_Filler_For_Circle(x_center, y_center, x, y, pixels, width);
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
                Symmetry_Filler_For_Circle(x_center, y_center, x, y, pixels, width);
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
        std::vector<bool> Circle(FVector2 Center, int Radius, int Buffer_Width){
            std::vector<bool> Result = std::vector<bool>(Buffer_Width * Buffer_Width, false);

            Circle(Center.X, Center.Y, Radius, Result, Buffer_Width);

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
        void Cubic_Bezier_Curve(FVector2 P0, FVector2 P1, FVector2 P2, FVector2 P3, std::vector<bool>& pixels, int width){
            for (double t = 0.0; t <= 1.0; t += 0.001) {
                double u = 1 - t;
                double tt = t*t, uu = u*u;
                double uuu = uu * u, ttt = tt * t;

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
        std::vector<bool> Cubic_Bezier_Curve(FVector2 P0, FVector2 P1, FVector2 P2, FVector2 P3, int Buffer_Width){
            std::vector<bool> Result = std::vector<bool>(Buffer_Width * Buffer_Width, false);

            Cubic_Bezier_Curve(P0, P1, P2, P3, Result, Buffer_Width);

            return Result;
        }


    }

    // namespace FONT{
    //     Font_Header Parse_Font_File(std::string File_Name){

            

    //     }
    // }


}