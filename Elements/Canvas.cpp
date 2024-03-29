#include "Canvas.h"

#include "../Renderer.h"

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


}