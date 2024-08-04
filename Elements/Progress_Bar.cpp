#include "Progress_Bar.h"
#include <string>
#include <math.h>
#include <algorithm>
#include "../Renderer.h"

using namespace std;

namespace GGUI{

    namespace Progress_Bar_Styles{
        inline PROGRESS_STYLE Default = PROGRESS_STYLE();
        inline PROGRESS_STYLE Blocky = PROGRESS_STYLE(GGUI::SYMBOLS::FULL_BLOCK.data(), GGUI::SYMBOLS::FULL_BLOCK.data(), GGUI::SYMBOLS::FULL_BLOCK.data());
        inline PROGRESS_STYLE Arrow = PROGRESS_STYLE(">", "=", "=");
    }

    Progress_Bar::Progress_Bar(unsigned int width, unsigned int height, PROGRESS_STYLE style) : Element(width, height), Progress_Style(style){
        Content.resize(Width - Has_Border() * 2, UTF(Progress_Style.Body, { Progress_Style.Empty_Color , Get_Background_Color() }));
    }

    unsigned int Progress_Bar::Get_Index_of_Head(){
        return floor(Progress * (Width - Has_Border() * 2));
    }

    void Progress_Bar::Color_Bar(){
        // First color the empty_color
        for (unsigned int i = 0; i < Width - Has_Border() * 2; i++)
            Content[i] = UTF(Progress_Style.Body, { Progress_Style.Empty_Color , Get_Background_Color() });

        // Now fill in the progressed part.
        for (unsigned int i = 0; i < Get_Index_of_Head(); i++)
            Content[i].Foreground = Progress_Style.Body_Color;

        // now replace the head part.
        Content[Get_Index_of_Head()] = UTF(Progress_Style.Head, { Progress_Style.Head_Color, Get_Background_Color() });

        // now replace the tail part.
        Content[0] = UTF(Progress_Style.Tail, { Progress_Style.Tail_Color, Get_Background_Color() });

    }

    // Draws into the this.Render_Buffer nested buffer of AST window's
    std::vector<UTF>& Progress_Bar::Render(){
        std::vector<UTF>& Result = Render_Buffer;

        if (Dirty.is(STAIN_TYPE::CLEAN))
            return Result;

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
            
            Content.resize(Width - Has_Border() * 2, UTF(Progress_Style.Body, { Progress_Style.Empty_Color , Get_Background_Color() }));

            Color_Bar();

            Dirty.Clean(STAIN_TYPE::STRETCH);

            Dirty.Dirty(STAIN_TYPE::COLOR | STAIN_TYPE::EDGE | STAIN_TYPE::DEEP);
        }

        //Apply the color system to the resized result list
        if (Dirty.is(STAIN_TYPE::COLOR))
            Apply_Colors(this, Result);

        //This will add the child windows to the Result buffer
        if (Dirty.is(STAIN_TYPE::DEEP)){
            Dirty.Clean(STAIN_TYPE::DEEP);

            int Starting_Y = Has_Border();
            int Starting_X = Has_Border();

            int Ending_Y = Height - Has_Border();
            int Ending_X = Width - Has_Border();

            for (int y = Starting_Y; y < Ending_Y; y++)
                for (int x = Starting_X; x < Ending_X; x++)
                    Result[y * Width + x] = Content[x - Starting_X];
        }

        //This will add the borders if necessary and the title of the window.
        if (Dirty.is(STAIN_TYPE::EDGE))
            Add_Overhead(this, Result);

        return Result;
    }

    void Progress_Bar::Set_Progress(float New_Progress){
        if (New_Progress > 1.00){
            Report(Get_Name() + " got a percentage overflow!");

            return;
        }

        Progress = New_Progress;

        Color_Bar();

        Dirty.Dirty(STAIN_TYPE::DEEP);

        Update_Frame();
    }

    float Progress_Bar::Get_Progress(){
        return Progress;
    }

    void Progress_Bar::Show_Border(bool b){
        if (b != Style->Border_Enabled.Value){
            Style->Border_Enabled = b;

            if (b) Height += 2;
            else Height -= 2;

            Dirty.Dirty(STAIN_TYPE::EDGE);
            Update_Frame();
        }
    }
}
