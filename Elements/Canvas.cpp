#include "Canvas.h"

#include "../Renderer.h"
#include <cmath>
#include <bitset>

#include "../SIMD/SIMD.h"

namespace GGUI{

    Canvas::Canvas(unsigned int w, unsigned int h, Coordinates position) : Element(){
        Pause_GGUI([=](){
            Buffer.resize(w * h);

            Width = w;
            Height = h;

            Set_Position(position);

            //We dont need any other than the color.
            Dirty.Clean(STAIN_TYPE::DEEP);
        });
    }
    
    void Canvas::Set(unsigned int x, unsigned int y, RGB color, bool Flush){
        unsigned int Actual_X = x + Has_Border();
        unsigned int Actual_Y = y + Has_Border();

        Buffer[Actual_X + Actual_Y * Width] = color;

        Dirty.Dirty(STAIN_TYPE::COLOR);

        if (Flush)
            Update_Frame();
    }

    void Canvas::Flush(){
        Update_Frame();
    }

    void Canvas::Render(){
        std::vector<GGUI::UTF>& Result = Render_Buffer;
                
        if (Dirty.is(STAIN_TYPE::CLEAN))
            return;

        if (Dirty.is(STAIN_TYPE::CLASS)){
            Parse_Classes();

            Dirty.Clean(STAIN_TYPE::CLASS);
        }

        if (Dirty.is(STAIN_TYPE::STRETCH)){
            Result.clear();
            Result.resize(Width * Height);
            Dirty.Clean(STAIN_TYPE::STRETCH);

            Dirty.Dirty(STAIN_TYPE::COLOR | STAIN_TYPE::EDGE);
        }

        //Apply the color system to the resized result list
        if (Dirty.is(STAIN_TYPE::COLOR)){

            Dirty.Clean(STAIN_TYPE::COLOR);

            unsigned int Start_X = Has_Border();
            unsigned int Start_Y = Has_Border();

            unsigned int End_X = Width - Has_Border();
            unsigned int End_Y = Height - Has_Border();

            for (unsigned int y = Start_Y; y < End_Y; y++){
                for (unsigned int x = Start_X; x < End_X; x++){
                    UTF Current_Pixel;

                    Current_Pixel.Set_Background(Buffer[x + y * Width]);

                    Result[x + y * Width] = Current_Pixel;
                }
            }
        }

        //This will add the borders if nessesary and the title of the window.
        if (Dirty.is(STAIN_TYPE::EDGE))
            Add_Overhead(this, Result);

        return;
    }

    Sprite::Sprite(std::vector<GGUI::UTF> frames, int offset, int speed) : Frames(frames), Offset(offset), Speed(speed) {
        // check if the frames size if an power of twos compliment
        if (std::bitset<sizeof(unsigned char)>(Frames.size()).count() == 1)
            Is_Power_Of_Two = true;

        Frame_Distance = ((float)UINT8_MAX + 1) / (float)Frames.size();
    }

    Terminal_Canvas::Terminal_Canvas(unsigned int w, unsigned int h, Coordinates position) : Element(){
        Buffer.resize(w * h);

        Width = w;
        Height = h;

        Set_Position(position);
    }

    Terminal_Canvas::~Terminal_Canvas(){
        // Check if this is added to the multiframe list and if so then remove it from there.
        if (Multi_Frame_Canvas.find(this) != Multi_Frame_Canvas.end()){
            Multi_Frame_Canvas.erase(this);
        }

        // Call the base class destructor
        Element::~Element();
    }

    void Terminal_Canvas::Set(unsigned int x, unsigned int y, Sprite& sprite, bool Flush){
        unsigned int Actual_X = x + Has_Border();
        unsigned int Actual_Y = y + Has_Border();

        if (!Is_Multi_Frame() && sprite.Frames.size() > 1 && Multi_Frame_Canvas.find(this) == Multi_Frame_Canvas.end()){
            Multi_Frame_Canvas[this] = true;
            Multi_Frame = true;
        }

        Buffer[Actual_X + Actual_Y * Width] = sprite;

        Dirty.Dirty(STAIN_TYPE::COLOR);

        if (Flush)
            Update_Frame();
    }

    void Terminal_Canvas::Set(unsigned int x, unsigned int y, UTF& sprite, bool Flush){
        unsigned int Actual_X = x + Has_Border();
        unsigned int Actual_Y = y + Has_Border();

        Buffer[Actual_X + Actual_Y * Width].Frames.push_back(sprite);

        Dirty.Dirty(STAIN_TYPE::COLOR);

        if (Flush)
            Update_Frame();
    }

    void Terminal_Canvas::Flush(bool Force_Flush){
        if (Force_Flush){
            Dirty.Dirty(STAIN_TYPE::COLOR);
        }

        Update_Frame();
    }

    void Terminal_Canvas::Render(){
        std::vector<GGUI::UTF>& Result = Render_Buffer;
                
        Current_Animation_Frame++;

        if (Dirty.is(STAIN_TYPE::CLEAN))
            return;

        if (Dirty.is(STAIN_TYPE::CLASS)){
            Parse_Classes();

            Dirty.Clean(STAIN_TYPE::CLASS);
        }

        if (Dirty.is(STAIN_TYPE::MOVE)){
            Dirty.Clean(STAIN_TYPE::MOVE);

            Update_Absolute_Position_Cache();
        }

        if (Dirty.is(STAIN_TYPE::STRETCH)){
            Result.clear();
            Result.resize(Width * Height, SYMBOLS::EMPTY_UTF);
            Dirty.Clean(STAIN_TYPE::STRETCH);

            Dirty.Dirty(STAIN_TYPE::COLOR | STAIN_TYPE::EDGE);
        }

        //Apply the color system to the resized result list
        if (Dirty.is(STAIN_TYPE::COLOR)){

            Dirty.Clean(STAIN_TYPE::COLOR);

            unsigned int Start_X = Has_Border();
            unsigned int Start_Y = Has_Border();

            unsigned int End_X = Width - Has_Border();
            unsigned int End_Y = Height - Has_Border();

            unsigned int Pixel_Index = 0;
            for (unsigned int y = Start_Y; y < End_Y; y++){
                for (unsigned int x = Start_X; x < End_X; x++){
                    Result[x + y * Width] = Buffer[Pixel_Index++].Render(Current_Animation_Frame);
                }
            }
        }
        
        //This will add the borders if necessary.
        if (Dirty.is(STAIN_TYPE::EDGE))
            Add_Overhead(this, Result);

        return;
    }

    UTF Sprite::Render(unsigned char Current_Frame){
        int Frame_Count = Frames.size();

        if (Frame_Count < 2){   // Check if current sprite has animation frames.
            return Frames.back();
        }

        // Apply the speed modifier 
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

        // now interpolate the foreground color between he two points
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

    void GGUI::Terminal_Canvas::Embed_Points(std::vector<bool> pixels, BORDER_STYLE_VALUE border_style, bool flush){

        unsigned int Usable_Width = Width - 2 * Has_Border();
        unsigned int Usable_Height = Height - 2 * Has_Border();

        // first check that the embed-able vector is within the usable area.
        if (pixels.size() != Usable_Width * Usable_Height){
            Report_Stack("The size of the embed-able vector is not the same as the size of the usable area. Expected: " + std::to_string((Width - 2 * Has_Border()) * (Height - 2 * Has_Border())) + " Got: " + std::to_string(pixels.size()));
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

        // Helper function for the above
        std::vector<bool> Line(Vector2 Start, Vector2 End, int Buffer_Width){
            std::vector<bool> Result = std::vector<bool>(Buffer_Width * Buffer_Width, false);

            Line(Start.X, Start.Y, End.X, End.Y, Result, Buffer_Width);

            return Result;
        }

        void Symmetry_Filler_For_Circle(int x_center, int y_center, int x, int y, std::vector<bool>& pixels, int width){
            pixels[(y_center+y)*width + (x_center+x)] = true;
            pixels[(y_center-y)*width + (x_center+x)] = true;
            pixels[(y_center+y)*width + (x_center-x)] = true;
            pixels[(y_center-y)*width + (x_center-x)] = true;
            pixels[(y_center+x)*width + (x_center+y)] = true;
            pixels[(y_center-x)*width + (x_center+y)] = true;
            pixels[(y_center+x)*width + (x_center-y)] = true;
            pixels[(y_center-x)*width + (x_center-y)] = true;
        }

        void Circle(int x_center, int y_center, int r, std::vector<bool>& pixels, int width){
            int x = 0, y = r;
            int d = 3 - 2 * r;
            Symmetry_Filler_For_Circle(x_center, y_center, x, y, pixels, width);
            while (y >= x) {
                x++;
                if (d > 0) {
                    y--;
                    d = d + 4 * (x - y) + 10;
                } else {
                    d = d + 4 * x + 6;
                }
                Symmetry_Filler_For_Circle(x_center, y_center, x, y, pixels, width);
            }
        }

        std::vector<bool> Circle(Vector2 Center, int Radius, int Buffer_Width){
            std::vector<bool> Result = std::vector<bool>(Buffer_Width * Buffer_Width, false);

            Circle(Center.X, Center.Y, Radius, Result, Buffer_Width);

            return Result;
        }

        void Cubic_Bezier_Curve(Vector2 P0, Vector2 P1, Vector2 P2, Vector2 P3, std::vector<bool>& pixels, int width){
            for (double t = 0.0; t <= 1.0; t += 0.001) {
                double u = 1 - t;
                double tt = t*t, uu = u*u;
                double uuu = uu * u, ttt = tt * t;

                Vector2 P;
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

        std::vector<bool> Cubic_Bezier_Curve(Vector2 P0, Vector2 P1, Vector2 P2, Vector2 P3, int Buffer_Width){
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