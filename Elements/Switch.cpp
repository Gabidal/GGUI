#include "Switch.h"
#include "List_View.h"

#include "../Renderer.h"

namespace GGUI{

    Switch::Switch(std::string text, std::vector<std::string> states, std::function<void (Element* This)> event) : Element(){
        Pause_Renderer([=](){
            States = states;
            
            Text.Allow_Overflow(true);
            Set_Text(text);

            On_Click([=]([[maybe_unused]] Event* e){
                this->Toggle();

                event(this);

                Update_Frame();

                return true;
            });

            Width = Text.Get_Width();
            Height = Text.Get_Height();

            Dirty.Dirty(STAIN_TYPE::DEEP | STAIN_TYPE::STATE);
        });
    }

    void Switch::Set_Text(std::string text) { 
        Pause_Renderer([=](){
            std::string Symbol = " ";
            char Space = ' ';

            Dirty.Dirty(STAIN_TYPE::DEEP);
            Text.Set_Text(Symbol + Space + text);   // This will call the update_frame for us.

            Set_Width(Text.Get_Width() + Has_Border() * 2);
            Set_Height(Text.Get_Height() + Has_Border() * 2);
        });
    }

    std::vector<UTF> Switch::Render(){
        std::vector<GGUI::UTF> Result = Render_Buffer;
        
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
            Dirty.Clean(STAIN_TYPE::STRETCH);
            
            Dirty.Dirty(STAIN_TYPE::COLOR | STAIN_TYPE::EDGE | STAIN_TYPE::DEEP);
        }

        //Check if the text has been changed.
        if (Dirty.is(STAIN_TYPE::DEEP)){
            Nest_Element(this, &Text, Result, Text.Render());

            //Clean text update notice and state change notice.
            //NOTE: Cleaning STATE flag without checking it's existence might lead to unexpected results.
            Dirty.Clean(STAIN_TYPE::DEEP);

            Dirty.Dirty(STAIN_TYPE::COLOR);
        }

        if (Dirty.is(STAIN_TYPE::STATE)){
            int State_Location_X = Has_Border();
            int State_Location_Y = Has_Border();

            Result[State_Location_Y * Width + State_Location_X] = States[State];

            Dirty.Clean(STAIN_TYPE::STATE);
            Dirty.Dirty(STAIN_TYPE::COLOR);
        }

        //Apply the color system to the resized result list
        if (Dirty.is(STAIN_TYPE::COLOR))
            Apply_Colors(this, Result);

        //This will add the borders if nessesary and the title of the window.
        if (Dirty.is(STAIN_TYPE::EDGE))
            Add_Overhead(this, Result);

        Render_Buffer = Result;

        return Result;
    }

}