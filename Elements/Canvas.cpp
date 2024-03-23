#include "Canvas.h"

#include "../Renderer.h"
#include <cmath>

namespace GGUI{

    Canvas::Canvas(unsigned int w, unsigned int h, Coordinates position) : Element(){
        Buffer.resize(w * h);

        Width = w;
        Height = h;

        Set_Position(position);

        //We dont need any other than the color.
        Dirty.Clean(STAIN_TYPE::DEEP);
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

    std::vector<UTF> Canvas::Render(){
        std::vector<GGUI::UTF> Result = Render_Buffer;
                
        if (Dirty.is(STAIN_TYPE::CLEAN))
            return Result;

        if (Dirty.is(STAIN_TYPE::CLASS)){
            Parse_Classes();

            Dirty.Clean(STAIN_TYPE::CLASS);
        }

        if (Dirty.is(STAIN_TYPE::STRECH)){
            Result.clear();
            Result.resize(Width * Height);
            Dirty.Clean(STAIN_TYPE::STRECH);

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

        Render_Buffer = Result;

        return Result;
    }

    Terminal_Canvas::Terminal_Canvas(unsigned int w, unsigned int h, Coordinates position) : Element(){
        Buffer.resize(w * h);

        Width = w;
        Height = h;

        Set_Position(position);

        //We dont need any other than the color.
        Dirty.Clean(STAIN_TYPE::DEEP);
    }

    void Terminal_Canvas::Set(unsigned int x, unsigned int y, Sprite sprite, bool Flush){
        unsigned int Actual_X = x + Has_Border();
        unsigned int Actual_Y = y + Has_Border();

        Buffer[Actual_X + Actual_Y * Width] = sprite;

        Dirty.Dirty(STAIN_TYPE::COLOR);

        if (Flush)
            Update_Frame();
    }

    void Terminal_Canvas::Set(unsigned int x, unsigned int y, UTF sprite, bool Flush){
        unsigned int Actual_X = x + Has_Border();
        unsigned int Actual_Y = y + Has_Border();

        Buffer[Actual_X + Actual_Y * Width].Texture = sprite;

        Dirty.Dirty(STAIN_TYPE::COLOR);

        if (Flush)
            Update_Frame();
    }

    void Terminal_Canvas::Flush(){
        Update_Frame();
    }

    std::vector<UTF> Terminal_Canvas::Render(){
        std::vector<GGUI::UTF> Result = Render_Buffer;
                
        if (Dirty.is(STAIN_TYPE::CLEAN))
            return Result;

        if (Dirty.is(STAIN_TYPE::CLASS)){
            Parse_Classes();

            Dirty.Clean(STAIN_TYPE::CLASS);
        }

        if (Dirty.is(STAIN_TYPE::STRECH)){
            Result.clear();
            Result.resize(Width * Height);
            Dirty.Clean(STAIN_TYPE::STRECH);

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
                    Result[x + y * Width] = Buffer[x + y * Width].Render();
                }
            }
        }

        //This will add the borders if nessesary and the title of the window.
        if (Dirty.is(STAIN_TYPE::EDGE))
            Add_Overhead(this, Result);

        Render_Buffer = Result;

        return Result;

    }

    UTF Sprite::Render(){
        UTF Result = Texture;

        Result.Set_Background(Background_Color);
        Result.Set_Foreground(Foreground_Color);
        
        return Result;
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

    namespace FONT{
        Font_Header Parse_Font_File(std::string File_Name){

            

        }
    }


}