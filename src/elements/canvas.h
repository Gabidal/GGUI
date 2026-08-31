#ifndef _CANVAS_H_
#define _CANVAS_H_

#include "element.h"

#include "../core/utils/style.h"
#include "../core/backend/utils.h"

#include <vector>
#include <bitset>

namespace GGUI{
    struct sprite {
        terminal::cell glyph  = ' ';           // terminal::cell for unicode characters
        unsigned char opacity = UINT8_MAX;
        RGB glyphColor        = {};
        RGB backgroundColor   = {};
        linearMask<textAttributeTypes, uint64_t> textAttributes = textAttributeTypes::DEFAULT;
    protected:
        constexpr std::pair<terminal::cell, ActiveStyle> render() {
            ActiveStyle styling;
            styling.area = {
                {}, {1, 1}
            };
            styling.activeTextColor = glyphColor;
            styling.activeBackgroundColor = backgroundColor;
            styling.opacity = opacity;
            styling.activeTextAttributes = textAttributes;

            return {glyph, styling};
        }

        friend class canvas;
    };

    struct animationSprite{
    protected:
        bool isPowerOfTwo = false;
    public:
        std::vector<sprite> frames;     
        
        int offset;     // This is for more beautiful mass animation systems, like wildfires and ocean waves.
        int speed;      // Animation speed scalar, 1x, 2x, 3x, ...
        
        int frameDistance = 1; // Interpolation steps between two frames

        constexpr animationSprite(std::vector<sprite> Frames = {}, int Offset = 0, int Speed = 1) : frames(Frames), offset(Offset), speed(Speed) {
            // Check if the frames size is an power of twos compliment
            // This is done to make sure the animation can be looped without any issues
            if (std::bitset<sizeof(unsigned char)>(Frames.size()).count() == 1)
                isPowerOfTwo = true;

            // Calculate the frame distance to determine how much to increment the frame index
            // This is done by dividing the maximum value of an unsigned char (255) by the number of frames
            frameDistance = ((float)UINT8_MAX + 1) / (float)Frames.size();
        }

    protected:
        sprite render(unsigned char Current_Time);

        friend class canvas;
    };

    class canvas : public element{
    private:
        // DONT GIVE THIS TO USER!!!
        canvas(){}
    protected:
        std::vector<animationSprite> buffer;

        unsigned char currentAnimationFrame = 0;

        // For speeding up sprite sets, to avoid redundant checks in unordered_maps.
        bool multiFrame = false;

        // Per-frame, use On_Render for an single pass use.
        GGUI::animationSprite (*On_Draw)(unsigned int x, unsigned int y) = 0;
    public:
        /**
         * @brief Constructs a canvas element with the specified style and optional embedding of styles.
         * 
         * @param s A reference to a `STYLING_INTERNAL::styleBase` object that defines the style for the canvas.
         * @param Embed_Styles_On_Construct A boolean flag indicating whether styles should be embedded during construction. 
         *        Defaults to `false`.
         */
        canvas(STYLING_INTERNAL::styleBase& s, bool Embed_Styles_On_Construct = false) : element(s, Embed_Styles_On_Construct){}

        /**
         * @brief Constructs a canvas object with the specified style and optional embedding of styles.
         * 
         * @param s A rvalue reference to a `STYLING_INTERNAL::styleBase` object that defines the style for the canvas.
         * @param Embed_Styles_On_Construct A boolean flag indicating whether to embed styles during construction. 
         *        Defaults to false.
         */
        canvas(STYLING_INTERNAL::styleBase&& s, bool Embed_Styles_On_Construct = false) : canvas(s, Embed_Styles_On_Construct){}
        
        /**
         * @brief Destructor for the Terminal_Canvas class.
         * @details This destructor checks if the current Terminal_Canvas instance is part of the multi-frame list.
         *          If it is, it removes the instance from the list to properly manage resources.
         *          It then calls the base class destructor to ensure all parent class resources are cleaned up.
         */
        ~canvas() override;

        /**
         * @brief Advances the animation to the next frame.
         * 
         * This function increments the current animation frame counter,
         * allowing the animation to progress to the next frame in the sequence.
         */
        void setNextAnimationFrame() { currentAnimationFrame++; }

        /**
         * @brief Set the sprite at the specified location on the terminal canvas.
         * @details This function places a sprite on the canvas at the given (x, y) coordinates.
         * The function also handles buffer resizing and multi-frame management.
         * @param x The x coordinate of the location.
         * @param y The y coordinate of the location.
         * @param sprite The sprite to be placed.
         * @param Flush Whether or not to call Update_Frame() after setting the sprite.
         */
        void set(unsigned int x, unsigned int y, animationSprite& sprite, bool Flush = true);

        /**
         * @brief Set the sprite at the specified location on the terminal canvas.
         * @details This function places a sprite on the canvas at the given (x, y) coordinates.
         * The function also handles buffer resizing and multi-frame management.
         * @param x The x coordinate of the location.
         * @param y The y coordinate of the location.
         * @param sprite The sprite to be placed.
         * @param Flush Whether or not to call Update_Frame() after setting the sprite.
         */
        void set(unsigned int x, unsigned int y, animationSprite&& sprite, bool Flush = true);

        /**
         * @brief Set the UTF sprite at the specified location on the terminal canvas.
         * @details This function places a UTF sprite on the canvas at the given (x, y) coordinates.
         * It also handles buffer resizing when necessary.
         * @param x The x coordinate of the location.
         * @param y The y coordinate of the location.
         * @param sprite The UTF sprite to be placed.
         * @param Flush Whether or not to call Update_Frame() after setting the sprite.
         */
        void set(unsigned int x, unsigned int y, const sprite& sprite, bool Flush = true);
        
        /**
         * @brief Flush the canvas.
         * @details This function can be used to manually flush the canvas.
         * If Force_Flush is true, the canvas will be marked as dirty for color updates.
         * @param Force_Flush Whether or not to mark the canvas as dirty for color updates.
         */
        void flush(bool Force_Flush = false);

        /**
         * @brief Checks if the canvas is in a multi-frame state.
         * 
         * @return true if the canvas is in a multi-frame state, false otherwise.
         */
        bool isMultiFrame(){ return multiFrame; }

        /**
         * @brief Returns the name of the Terminal_Canvas as a string.
         * @details The returned string is a combination of the class name and the Name property.
         * @return A string representing the name of the Terminal_Canvas.
         */
        std::string getName() const override {
            // Concatenate class name and Name property to form the full name.
            return "canvas<" + Name + ">";
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
        void embedPoints(std::vector<bool> pixels, styledBorder border_style = GGUI::STYLES::BORDER::Single, bool Flush = true);

        /**
         * @brief Sets the callback function to be invoked during the drawing process.
         * 
         * This function allows you to specify a custom callback that will be called
         * for each pixel or coordinate during the drawing operation. The callback
         * should return a GGUI::sprite object based on the provided x and y coordinates.
         * 
         * @param on_draw A pointer to a function that takes two unsigned integers 
         *                (x and y coordinates) as input and returns a GGUI::sprite object.
         */
        void setOnDraw(GGUI::animationSprite (*on_draw)(unsigned int x, unsigned int y)){
            this->On_Draw = on_draw;
        }

    protected:
        /**
         * @brief Creates a deep copy of the Terminal_Canvas and returns it as a movable Element.
         * @return A deep copy of the Terminal_Canvas as a movable Element.
         */
        element* createInstance() const override {
            return new canvas();
        }
        
        /**
         * @brief Render the canvas.
         * @details This function renders the canvas, applying all the necessary transformations and color changes.
         *          It also handles the multi-frame list and sprite animations.
         * @return A vector of UTF objects representing the rendered canvas.
         */
        std::vector<terminal::cell>& render() override;
    };

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
        void line(int x1, int y1, int x2, int y2, std::vector<bool>& pixels, int width);

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
        std::vector<bool> line(FVector2 Start, FVector2 End, int Buffer_Width);

        /**
         * @brief Symmetrical circle draw helper.
         * @param x_center The x position of the center of the circle.
         * @param y_center The y position of the center of the circle.
         * @param x The current x position of the circle.
         * @param y The current y position of the circle.
         * @param pixels The vector of pixels of the canvas.
         * @param width The width of the canvas.
         * @details
         * This function is a helper function for drawing a circle on the canvas.
         * It fills in the circle symmetrically by setting the pixels to true.
         */
        void symmetryFillerForCircle(int x_center, int y_center, int x, int y, std::vector<bool>& pixels, int width);

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
        void circle(int x_center, int y_center, int r, std::vector<bool>& pixels, int width);

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
        std::vector<bool> circle(FVector2 Center, int Radius, int Buffer_Width);

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
        void cubicBezierCurve(FVector2 P0, FVector2 P1, FVector2 P2, FVector2 P3, std::vector<bool>& pixels, int width);
        
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
        std::vector<bool> cubicBezierCurve(FVector2 P0, FVector2 P1, FVector2 P2, FVector2 P3, int Buffer_Width);

    }

    namespace FONT{
        // Based on: https://learn.microsoft.com/en-us/typography/opentype/spec/otff
        class fontHeader{
        public:
        };

        fontHeader parseFontFile(std::string File_Name);
    }

}

#endif
