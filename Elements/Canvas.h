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

}

#endif
