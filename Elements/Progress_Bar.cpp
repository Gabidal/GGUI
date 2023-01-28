#include "Progress_Bar.h"
#include <string>
#include <math.h>
#include <algorithm>
#include "../Renderer.h"

using namespace std;

namespace GGUI{

    std::vector<UTF> Progress_Bar::Render(){
        std::vector<GGUI::UTF> Result = Render_Buffer;

        //if inned children have changed whitout this changing, then this will trigger.
        if (Children_Changed()){
            Dirty.Dirty(STAIN_TYPE::DEEP);
        }
                
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

            Dirty.Dirty(STAIN_TYPE::COLOR | STAIN_TYPE::EDGE | STAIN_TYPE::DEEP);
        }

        bool Connect_Borders_With_Parent = Has_Border();

        //This is for the bar content
        if (Dirty.is(STAIN_TYPE::DEEP)){
            Dirty.Clean(STAIN_TYPE::DEEP);

            Add_Horizontal_Lines(Result);
        }

        //Apply the color to the progress bar content.
        if (Dirty.is(STAIN_TYPE::COLOR))
            Apply_Colors(this, Result);


        if (Connect_Borders_With_Parent)
            Dirty.Dirty(STAIN_TYPE::EDGE);

        //This will add the borders if nessesary.
        if (Dirty.is(STAIN_TYPE::EDGE))
            Add_Overhead(this, Result);

        Render_Buffer = Result;

        return Result;
    }

    void Progress_Bar::Apply_Colors(Element* w, std::vector<UTF>& Result){

        // Call the default Apple_Color for the border control.
        Element::Apply_Colors(w, Result);

        // Now calculate the progress bars content.
        int Start_X = Has_Border();
        int Start_Y = Has_Border();

        int End_X = Width - Has_Border();
        int End_Y = Height - Has_Border();

        float FILL = Progress * (End_X - Start_X);

        for (int y = Start_Y; y < End_Y; y++){
            for (int x = Start_X; x < End_X; x++){
                int Index = x + y * Width;

                // Clear the default Apply_Color system
                Result[Index].Pre_Fix = "";
                Result[Index].Post_Fix = "";

                if (x >= floor(FILL)){
                    Result[Index].Pre_Fix = Compose_Background_RGB_Values(true);
                }
                else{
                    Result[Index].Pre_Fix = Compose_Text_RGB_Values();
                }
 
                if (Result[Index].Pre_Fix != "")
                    Result[Index].Post_Fix = Constants::RESET_Text_Color + Constants::RESET_Back_Ground_Color;
            }
        }

    }


    void Progress_Bar::Add_Horizontal_Lines(std::vector<UTF>& buffer){
        int Start_X = Has_Border();
        int Start_Y = Has_Border();

        int End_X = Width - Has_Border();
        int End_Y = Height - Has_Border();

        UTF FILL_TYPE = SYMBOLS::CENTERED_HORIZONTAL_LINE;

        if (End_Y - Start_Y > 1){
            FILL_TYPE = SYMBOLS::FULL_BLOCK;
        }

        for (int y = Start_Y; y < End_Y; y++){
            for (int x = Start_X; x < End_X; x++){
                int Index = x + y * Width;

                buffer[Index] = FILL_TYPE;
            }
        }

    }


    void Progress_Bar::Set_Progress(float New_Progress){
        if (New_Progress > 1.01){
            Report("Percentage overflow!");
        }

        Progress = New_Progress;
        Dirty.Dirty(STAIN_TYPE::COLOR);

        Update_Frame();
    }

    float Progress_Bar::Get_Progress(){
        return Progress;
    }

    Progress_Bar::Progress_Bar() : Element(){
        Progress = 0;
    }

    Progress_Bar::Progress_Bar(unsigned int Width, unsigned int Height) : Element(Width, Height){
        Progress = 0;
    }

    
    Progress_Bar::Progress_Bar(RGB Fill_Color, RGB Empty_Color) : Element(){
        Progress = 0;
        Set_Fill_Color(Fill_Color);
        Set_Empty_Color(Empty_Color);
    }

    
    Progress_Bar::Progress_Bar(RGB Fill_COlor, RGB Empty_Color, unsigned int Width, unsigned int Height) : Element(Width, Height){
        Progress = 0;
        Set_Fill_Color(Fill_COlor);
        Set_Empty_Color(Empty_Color);
    }

    void Progress_Bar::Set_Fill_Color(RGB value){
        At<RGB_VALUE>(STYLES::Text_Color)->Value = value;
        Dirty.Dirty(STAIN_TYPE::COLOR);
    }

    void Progress_Bar::Set_Empty_Color(RGB value){
        At<RGB_VALUE>(STYLES::Background_Color)->Value = value;
        Dirty.Dirty(STAIN_TYPE::COLOR);
    }

    void Progress_Bar::Show_Border(bool b){
        if (b != At<BOOL_VALUE>(STYLES::Border)->Value){
            At<BOOL_VALUE>(STYLES::Border)->Value = b;

            if (b) Height += 2;
            else Height -= 2;

            Dirty.Dirty(STAIN_TYPE::EDGE);
            Update_Frame();
        }
    }

}
