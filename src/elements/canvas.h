#ifndef _CANVAS_H_
#define _CANVAS_H_

#include "element.h"

#include "../core/backend/utils.h"

#include <vector>
#include <bitset>

namespace GGUI{
    struct sprite {
        terminal::cell glyph  = ' ';           // terminal::cell for unicode characters
        unsigned char opacity = UINT8_MAX;
        RGB glyphColor        = {};
        RGB backgroundColor   = {};
        linearMask<terminal::textAttributeTypes, uint64_t> textAttributes = terminal::textAttributeTypes::DEFAULT;
    protected:
        constexpr std::pair<terminal::cell, activeStyle> render() {
            activeStyle styling;
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
        GGUI::animationSprite (*On_Draw)(IVector2 point) = 0;
    public:
        ~canvas() override;

        void setNextAnimationFrame() { currentAnimationFrame++; }

        void set(IVector2 point, animationSprite& sprite, bool Flush = true);

        void set(IVector2 point, animationSprite&& sprite, bool Flush = true);

        void set(IVector2 point, const sprite& sprite, bool Flush = true);
        
        void flush(bool Force_Flush = false);

        bool isMultiFrame(){ return multiFrame; }

        std::string getTypedName() const override {
            // Concatenate class name and Name property to form the full name.
            return "canvas<" + ID + ">";
        }

        void setOnDraw(GGUI::animationSprite (*on_draw)(IVector2 point)){
            this->On_Draw = on_draw;
        }

    protected:
        element* createInstance() const override {
            return new canvas();
        }

        rectangle getInnerBounds() const {
            IVector2 borderOffset = {hasBorder(), hasBorder()};

            return {
                borderOffset,
                getAsRectangle().size - borderOffset
            };
        }
        
        std::vector<terminal::cell>& render() override;
    };

    namespace FONT{
        // Based on: https://learn.microsoft.com/en-us/typography/opentype/spec/otff
        class fontHeader{
        public:
        };

        fontHeader parseFontFile(std::string File_Name);
    }

}

#endif
