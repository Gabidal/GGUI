#ifndef _CANVAS_H_
#define _CANVAS_H_

#include "Element.h"

#include <vector>

namespace GGUI{

    class Canvas : public Element{
    private:
        // DONT GIVE THIS TO USER!
        Canvas(){}
    protected:
        std::vector<RGB> Buffer;
    public:
        Canvas(unsigned int w, unsigned int h, Coordinates position);
        
        // This is to set a color in the canvas, you can set it to not flush, if youre gonna set more than one pixel.
        void Set(unsigned int x, unsigned int y, RGB color, bool Flush = true);
        
        void Flush();

        std::vector<UTF> Render() override;

        Element* Safe_Move() override {
            Canvas* new_Canvas = new Canvas();
            *new_Canvas = *(Canvas*)this;

            return new_Canvas;
        }

        std::string Get_Name() const override{
            return "Canvas<" + Name + ">";
        }
    };

    class Sprite{
    public:
        RGB Background_Color;
        RGB Foreground_Color;
        UTF Texture;

        Sprite(UTF t = UTF(" "), RGB b = COLOR::BLACK, RGB f = COLOR::WHITE) : Texture(t), Background_Color(b), Foreground_Color(f){}

        UTF Render();
    };

    class Terminal_Canvas : public Element{
    private:
        // DONT GIVE THIS TO USER!!!
        Terminal_Canvas(){}
    protected:
        std::vector<Sprite> Buffer;
    public:
        Terminal_Canvas(unsigned int w, unsigned int h, Coordinates position);
        
        void Set(unsigned int x, unsigned int y, Sprite sprite, bool Flush = true);

        void Set(unsigned int x, unsigned int y, UTF sprite, bool Flush = true);
        
        void Flush();
        
        std::vector<UTF> Render() override;
        
        Element* Safe_Move() override {
            Terminal_Canvas* new_Terminal_Canvas = new Terminal_Canvas();
            *new_Terminal_Canvas = *(Terminal_Canvas*)this;

            return new_Terminal_Canvas;
        }

        std::string Get_Name() const override{
            return "Terminal_Canvas<" + Name + ">";
        }
    
        
    };

    namespace DRAW{

        // Expects fully initialized 2D list of booleans, which it will put the result.
        void Line(int x1, int y1, int x2, int y2, std::vector<bool>& pixels, int width);

        std::vector<bool> Line(Vector2 Start, Vector2 End, int Buffer_Width);

        // Symmetrical circle draw helper:
        void Symmetry_Filler_For_Circle(int x_center, int y_center, int x, int y, std::vector<bool>& pixels, int width);

        void Circle(int x_center, int y_center, int r, std::vector<bool>& pixels, int width);

        std::vector<bool> Circle(Vector2 Center, int Radius, int Buffer_Width);

        void Cubic_Bezier_Curve(Vector2 P0, Vector2 P1, Vector2 P2, Vector2 P3, std::vector<bool>& pixels, int width);
        
        std::vector<bool> Cubic_Bezier_Curve(Vector2 P0, Vector2 P1, Vector2 P2, Vector2 P3, int Buffer_Width);

    }

    namespace FONT{
        // Based on: https://learn.microsoft.com/en-us/typography/opentype/spec/otff
        class Font_Header{
        public:
        };

        Font_Header Parse_Font_File(std::string File_Name);
    }

}

#endif
