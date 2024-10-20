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
        Canvas(Styling s) : Element(s){}
        
        // This is to set a color in the canvas, you can set it to not flush, if youre gonna set more than one pixel.
        void Set(unsigned int x, unsigned int y, RGB color, bool Flush = true);
        
        void Flush();

        std::vector<GGUI::UTF>&  Render() override;

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
        std::vector<GGUI::UTF> Frames;

        int Offset = 0;     // This is for more beautiful mass animation systems
        int Speed = 1;      // Using decimals too slow hmmm...

        int Frame_Distance = 0;

        bool Is_Power_Of_Two = false;

        Sprite(std::vector<GGUI::UTF> frames, int offset = 0, int speed = 1);

        Sprite(GGUI::UTF frame, int offset = 0, int speed = 1) : Offset(offset), Speed(speed), Frame_Distance(UINT8_MAX) {
            Frames.push_back(frame);
        }

        Sprite() : Frame_Distance(UINT8_MAX){
            Frames.push_back(GGUI::UTF(' '));
            
            Offset = 0;
            Speed = 1;
            
            Is_Power_Of_Two = false;
        }

        UTF Render(unsigned char Current_Time);
    };

    namespace GROUP_TYPE{
        // Defines the group sizes for Sprite group optimizing.
        inline unsigned char QUAD = 1 << 2;
        inline unsigned char HEX = 1 << 3;
        inline unsigned char OCTAL = 1 << 4;
    }

    class Terminal_Canvas : public Element{
    private:
        // DONT GIVE THIS TO USER!!!
        Terminal_Canvas(){}
    protected:
        std::vector<Sprite> Buffer;

        unsigned char Current_Animation_Frame = 0;

        // For speeding up sprite sets, to avoid redundant checks in unordered_maps.
        bool Multi_Frame = false;
    public:
        Terminal_Canvas(Styling s) : Element(s){}
        
        ~Terminal_Canvas() override;

        void Set_Next_Animation_Frame() { Current_Animation_Frame++; }

        void Set(unsigned int x, unsigned int y, Sprite& sprite, bool Flush = true);

        void Set(unsigned int x, unsigned int y, UTF& sprite, bool Flush = true);
        
        void Flush(bool Force_Flush = false);
        
        std::vector<GGUI::UTF>&  Render() override;
        
        void Group_Heuristics();

        void Group(unsigned int Start_Index, int length);

        bool Is_Multi_Frame(){ return Multi_Frame; }

        Element* Safe_Move() override {
            Terminal_Canvas* new_Terminal_Canvas = new Terminal_Canvas();
            *new_Terminal_Canvas = *(Terminal_Canvas*)this;

            return new_Terminal_Canvas;
        }

        std::string Get_Name() const override{
            return "Terminal_Canvas<" + Name + ">";
        }
    
        void Embed_Points(std::vector<bool> pixels, styled_border border_style = GGUI::STYLES::BORDER::Single, bool Flush = true);
    };

    namespace DRAW{

        // Expects fully initialized 2D list of booleans, which it will put the result.
        void Line(int x1, int y1, int x2, int y2, std::vector<bool>& pixels, int width);

        std::vector<bool> Line(FVector2 Start, FVector2 End, int Buffer_Width);

        // Symmetrical circle draw helper:
        void Symmetry_Filler_For_Circle(int x_center, int y_center, int x, int y, std::vector<bool>& pixels, int width);

        void Circle(int x_center, int y_center, int r, std::vector<bool>& pixels, int width);

        std::vector<bool> Circle(FVector2 Center, int Radius, int Buffer_Width);

        void Cubic_Bezier_Curve(FVector2 P0, FVector2 P1, FVector2 P2, FVector2 P3, std::vector<bool>& pixels, int width);
        
        std::vector<bool> Cubic_Bezier_Curve(FVector2 P0, FVector2 P1, FVector2 P2, FVector2 P3, int Buffer_Width);

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
