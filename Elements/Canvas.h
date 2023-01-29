#ifndef _CANVAS_H_
#define _CANVAS_H_

#include "Element.h"

#include <vector>

namespace GGUI{

    class Canvas : public Element{
    protected:
        std::vector<RGB> Buffer;
    public:
        Canvas(unsigned int w, unsigned int h, Coordinates position);
        
        // This is to set a color in the canvas, you can set it to not flush, if youre gonna set more than one pixel.
        void Set(unsigned int x, unsigned int y, RGB color, bool Flush = true);
        
        void Flush();

        std::vector<UTF> Render() override;
    };

    class Sprite{
    public:
        RGB Background_Color;
        RGB Foreground_Color;
        UTF Texture;

        Sprite(UTF t = UTF(""), RGB b = COLOR::BLACK, RGB f = COLOR::WHITE) : Texture(t), Background_Color(b), Foreground_Color(f){}

        UTF Render();
    };

    class Terminal_Canvas : public Element{
    protected:
        std::vector<Sprite> Buffer;
    public:
        Terminal_Canvas(unsigned int w, unsigned int h, Coordinates position);
        
        void Set(unsigned int x, unsigned int y, Sprite sprite, bool Flush = true);

        void Set(unsigned int x, unsigned int y, UTF sprite, bool Flush = true);
        
        void Flush();
        
        std::vector<UTF> Render() override;
    };

}

#endif
