#include "canvas.h"

#include "../core/core.h"

namespace GGUI{
    namespace core {
        extern std::unordered_map<GGUI::canvas*, bool> multiFrameCanvas;
    }

    /**
     * @brief Destructor for the Terminal_Canvas class.
     * @details This destructor checks if the current Terminal_Canvas instance is part of the multi-frame list.
     *          If it is, it removes the instance from the list to properly manage resources.
     *          It then calls the base class destructor to ensure all parent class resources are cleaned up.
     */
    canvas::~canvas() {
        // Check if this Terminal_Canvas is in the multi-frame list
        if (core::multiFrameCanvas.find(this) != core::multiFrameCanvas.end()) {
            // Remove the canvas from the multi-frame list
            core::multiFrameCanvas.erase(this);
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
    void canvas::set(unsigned int x, unsigned int y, animationSprite& sprite, bool Flush){
        unsigned int innerWidth = getWidth() - hasBorder()*2;
        unsigned int Location = x + y * innerWidth; // Determine the buffer index for the sprite.

        // Check for multi-frame support and update the management map if needed.
        if (!isMultiFrame() && sprite.frames.size() > 1 && core::multiFrameCanvas.find(this) == core::multiFrameCanvas.end()){
            core::multiFrameCanvas[this] = true;
            multiFrame = true;
        }

        buffer[Location] = sprite; // Set the sprite at the calculated buffer location.

        flags |= (stain::types::GRAPHICS); // Mark the canvas as dirty for color updates.

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
    void canvas::set(unsigned int x, unsigned int y, animationSprite&& sprite, bool Flush){
        unsigned int innerWidth = getWidth() - hasBorder()*2;
        unsigned int Location = x + y * innerWidth; // Determine the buffer index for the sprite.

        // Check for multi-frame support and update the management map if needed.
        if (!isMultiFrame() && sprite.frames.size() > 1 && core::multiFrameCanvas.find(this) == core::multiFrameCanvas.end()){
            core::multiFrameCanvas[this] = true;
            multiFrame = true;
        }

        buffer[Location] = sprite; // Set the sprite at the calculated buffer location.

        flags |= (stain::types::GRAPHICS); // Mark the canvas as dirty for color updates.

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
    void canvas::set(unsigned int x, unsigned int y, const sprite& sprite, bool Flush){
        unsigned int innerWidth = getWidth() - hasBorder()*2;
        unsigned int Location = x + y * innerWidth; // Determine the buffer index for the sprite.
        
        buffer[Location].frames.push_back(sprite); // Add the sprite to the buffer at the calculated location.

        if (!isMultiFrame() && buffer[Location].frames.size() > 1 && core::multiFrameCanvas.find(this) == core::multiFrameCanvas.end()){
            core::multiFrameCanvas[this] = true;
            multiFrame = true;
        }

        flags |= (stain::types::GRAPHICS); // Mark the canvas as dirty for color updates.

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
            flags |= (stain::types::GRAPHICS);
        }

        updateFrame();
    }


    /**
     * @brief Render the canvas.
     * @details This function renders the canvas, applying all the necessary transformations and color changes.
     *          It also handles the multi-frame list and sprite animations.
     * @return A vector of UTF objects representing the rendered canvas.
     */
    std::vector<terminal::cell>& canvas::render() {
        std::vector<terminal::cell>& Result = cellBuffer;

        // Check for Dynamic attributes
        if(style->evaluateDynamicDimensions(this))
            flags |= (stain::types::STRETCH);

        if (style->evaluateDynamicPosition(this))
            flags |= (stain::types::MOVE);

        if (style->evaluateDynamicGraphics(this))
            flags |= (stain::types::GRAPHICS);

        if (style->evaluateDynamicBorder(this))
            flags |= (stain::types::EDGE);

        // Since canvas does not utilize DEEP flag, we can just clean it away
        if (flags.has(stain::types::DEEP))
            flags ^= (stain::types::DEEP);

        if (flags.isEmpty())
            return Result;

        unsigned int fittingWidth = getWidth() - hasBorder()*2;
        unsigned int fittingHeight = getHeight() - hasBorder()*2;
        
        if (flags.has(stain::types::STRETCH)) {
            Result.clear();
            Result.resize(getWidth() * getHeight(), ' ');

            // Also clear and resize the sprite buffer.
            buffer.clear();
            buffer.resize(fittingWidth * fittingHeight);

            flags ^= (stain::types::STRETCH);

            flags |= (stain::base(stain::types::GRAPHICS) | stain::types::EDGE | stain::types::RESET | stain::types::NOT_RENDERED);
        }

        if (flags.has(stain::types::NOT_RENDERED)) {
            if (onRender) onRender(this);

            // Clean regardless of On_Render existing or not.
            flags ^= (stain::types::NOT_RENDERED);
        }

        if (flags.has(stain::types::RESET)){
            flags ^= (stain::types::RESET);

            // now we need to call again the on_draw to correctly cast the correct sprites to their each respective buffer point.
            if (On_Draw != 0) {
                for (unsigned int y = 0; y < fittingHeight; y++) {
                    for (unsigned int x = 0; x < fittingWidth; x++) {
                        set(x, y, On_Draw(x, y), false);
                    }
                }
            }
        }

        if (flags.has(stain::types::MOVE)) {
            flags ^= (stain::types::MOVE);

            updateAbsolutePositionCache();
        }

        // Apply the color system to the resized result list
        if (flags.has(stain::types::GRAPHICS)) {
            flags ^= (stain::types::GRAPHICS);
            
            graphicalIdentityPool.clear();

            unsigned int startX = hasBorder();
            unsigned int startY = hasBorder();

            unsigned int endX = getWidth() - hasBorder();
            unsigned int endY = getHeight() - hasBorder();

            unsigned int pixelIndex = 0;
            for (unsigned int y = startY; y < endY; y++) {
                for (unsigned int x = startX; x < endX; x++) {
                    auto [glyph, styling] = buffer[pixelIndex++].render(currentAnimationFrame).render();
                    Result[x + y * getWidth()] = glyph;

                    // before we can add the styling to the identity pool
                    styling.area.position = getAbsolutePosition() + IVector2{x, y};
                    styling.origin = this;

                    graphicalIdentityPool.push_back(styling);
                }
            }

            compileActiveGraphics();    // compiles identifying graphics pools
        }

        // Add borders and titles if the EDGE stain is detected.
        if (flags.has(stain::types::EDGE)){
            flags ^= (stain::types::EDGE);

            renderBorders(Result);
            renderTitle(Result);
        }

        return Result;
    }

    /**
     * @brief Renders a UTF character based on the sprite's current frame and speed.
     * @param currentFrame The current frame of the animation.
     * @return The rendered UTF character.
     */
    sprite animationSprite::render(unsigned char currentFrame){
        int frameCount = frames.size();

        if (frameCount < 2){   // Check if current sprite has animation frames.
            return frames.back();
        }

        unsigned char animationFrame = (currentFrame + offset) * speed;     // This is our current animation frame

        int currentFrameIndex = animationFrame / frameDistance;            // This gives us the closest frame to the current animation frame.

        int frameBelow = currentFrameIndex % frameCount;      // This gives us the how many animations we have passed

        int frameIndexRemainder = animationFrame - frameBelow * frameDistance;  // This first transforms the frame below index back into non index based and then tells you how far are you from an "checkpoint" of a actual frame index.

        int Frame_Above = (frameBelow + 1) % frameCount;    // only +1, because the currentFrameIndex isn't a real index.
        
        sprite result = frames[frameBelow];

        // now interpolate the glyph color color between the two points
        result.glyphColor = utils::lerp(
            frames[frameBelow].glyphColor, 
            frames[Frame_Above].glyphColor, 
            frameIndexRemainder,
            frameDistance
        );

        // do same for background
        result.backgroundColor = utils::lerp(
            frames[frameBelow].backgroundColor, 
            frames[Frame_Above].backgroundColor, 
            frameIndexRemainder,
            frameDistance
        );

        return result;
    }
}