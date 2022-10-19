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
        });

        int Symbol_Lenght = 1;

        std::pair<unsigned int, unsigned int> Text_Dimensions = Text_Field::Get_Text_Dimensions(Text);

        Width = Symbol_Lenght + Text_Dimensions.first;
        Height = Text_Dimensions.second;
    }

    std::vector<UTF> Switch::Render(){
        if (Dirty.is(STAIN_TYPE::CLEAN))
            return Render_Buffer;

        // States: {O, X} Where: State = 0/1
        Text_Field* Container = new Text_Field(States[State] + " " + Text);

        Container->Inherit_States_From(this);

        Render_Buffer = Container->Render();

        return Render_Buffer;
    }

}