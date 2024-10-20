#include "Switch.h"
#include "List_View.h"

#include "../Core/Renderer.h"

namespace GGUI{

    Switch::Switch(std::string text, std::vector<std::string> states, std::function<void (Element* This)> event, Styling s) : Element(s){
        Pause_GGUI([this, text, states, event](){
            States = states;
            
            Text.Allow_Overflow(true);
            Set_Text(text);

            On_Click([=]([[maybe_unused]] Event* e){
                this->Toggle();

                event(this);

                Update_Frame();

                return true;
            });

            Set_Width(Text.Get_Width());
            Set_Height(Text.Get_Height());

            Dirty.Dirty(STAIN_TYPE::DEEP | STAIN_TYPE::STATE);
        });
    }

    void Switch::Set_Text(std::string text) { 
        Pause_GGUI([this, text](){
            std::string Symbol = " ";
            char Space = ' ';

            Dirty.Dirty(STAIN_TYPE::DEEP);
            Text.Set_Text(Symbol + Space + text);   // This will call the update_frame for us.

            Set_Width(Text.Get_Width() + Has_Border() * 2);
            Set_Height(Text.Get_Height() + Has_Border() * 2);
        });
    }

    std::vector<GGUI::UTF>& Switch::Render(){
        std::vector<GGUI::UTF>& Result = Render_Buffer;
        
        if (Dirty.is(STAIN_TYPE::CLEAN))
            return Result;

        if (Dirty.is(STAIN_TYPE::CLASS)){
            Parse_Classes();

            Dirty.Clean(STAIN_TYPE::CLASS);
        }

        if (Dirty.is(STAIN_TYPE::STRETCH)){
            // This needs to be called before the actual stretch, since the actual Width and Height have already been modified to the new state, and we need to make sure that is correct according to the percentile of the dynamic attributes that follow the parents diction.
            Style->Evaluate_Dynamic_Attribute_Values(this);
            
            Result.clear();
            Result.resize(Get_Width() * Get_Height(), SYMBOLS::EMPTY_UTF);
            Dirty.Clean(STAIN_TYPE::STRETCH);
            
            Dirty.Dirty(STAIN_TYPE::COLOR | STAIN_TYPE::EDGE | STAIN_TYPE::DEEP);
        }

        if (Dirty.is(STAIN_TYPE::MOVE)){
            Dirty.Clean(STAIN_TYPE::MOVE);

            Update_Absolute_Position_Cache();
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

            Result[State_Location_Y * Get_Width() + State_Location_X] = States[State];

            Dirty.Clean(STAIN_TYPE::STATE);
            Dirty.Dirty(STAIN_TYPE::COLOR);
        }

        //Apply the color system to the resized result list
        if (Dirty.is(STAIN_TYPE::COLOR))
            Apply_Colors(this, Result);

        //This will add the borders if nessesary and the title of the window.
        if (Dirty.is(STAIN_TYPE::EDGE))
            Add_Overhead(this, Result);

        return Result;
    }

}