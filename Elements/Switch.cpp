#include "Switch.h"
#include "List_View.h"

#include "../Renderer.h"

namespace GGUI{

    Switch::Switch(std::string text, std::vector<std::string> states, std::function<void (Element* This)> event) : Element(){
        States = states;

        auto Togler = [=](){
            this->Toggle();

            event(this);
        };

        Text = text;

        On_Click([=](Event* e){
            Togler();

            Update_Frame();

            return true;
        });

        int Symbol_Lenght = 1;

        int Space_Lenght = 1;

        std::pair<unsigned int, unsigned int> Text_Dimensions = Text_Field::Get_Text_Dimensions(Text);

        Width = Symbol_Lenght + Space_Lenght + Text_Dimensions.first;
        Height = Text_Dimensions.second;

        Dirty.Dirty(STAIN_TYPE::DEEP | STAIN_TYPE::STATE);
    }

    std::vector<UTF> Switch::Render(){
        std::vector<GGUI::UTF> Result = Render_Buffer;
        
        if (Dirty.is(STAIN_TYPE::CLEAN))
            return Result;

        if (Dirty.is(STAIN_TYPE::CLASS)){
            Parse_Classes();

            Dirty.Clean(STAIN_TYPE::CLASS);
        }

        if (Dirty.is(STAIN_TYPE::STRETCH)){
            Result.clear();
            Result.resize(Width * Height, SYMBOLS::EMPTY_UTF);
            Dirty.Clean(STAIN_TYPE::STRETCH);
            
            Dirty.Dirty(STAIN_TYPE::COLOR | STAIN_TYPE::EDGE | STAIN_TYPE::DEEP);
        }

        //Check if the text has been changed.
        if (Dirty.is(STAIN_TYPE::DEEP)){
            std::string Display_Text = States[State] + " " + Text;

            Text_Field::Left_Text(this, Display_Text, Parent, Result);

            //Clean text update notice and state change notice.
            //NOTE: Cleaning STATE flag whitout checking it's existance might lead to unexpecte results.
            Dirty.Clean(STAIN_TYPE::DEEP | STAIN_TYPE::STATE);

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