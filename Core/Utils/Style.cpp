#include "Style.h"
#include "../Elements/Element.h"

namespace GGUI{

    void position::Embed_Value(Element* host){
        host->Set_Position(Value.Get());
    }

    void width::Embed_Value(Element* host){
        host->Set_Width(Value.Get());
    }

    void height::Embed_Value(Element* host){
        host->Set_Height(Value.Get());
    }

    void text_color::Embed_Value(Element* host){
        host->Set_Text_Color(Value.Get());
    }

    void background_color::Embed_Value(Element* host){
        host->Set_Background_Color(Value.Get());
    }

    void border_color::Embed_Value(Element* host){
        host->Set_Border_Color(Value.Get());
    }

    void border_background_color::Embed_Value(Element* host){
        host->Set_Border_Background_Color(Value.Get());
    }

    void styled_border::Embed_Value(Element* host){
        host->Set_Custom_Border_Style(*this);
    }

    void margin::Embed_Value(Element* host){
        host->Set_Margin(*this);
    }

    void shadow::Embed_Value(Element* host){
        host->Set_Shadow(*this);
    }

    void opacity::Embed_Value(Element* host){
        host->Set_Opacity(Value.Get());
    }

    margin margin::Evaluate(Element* owner){
        return margin(
            Top.Evaluate(owner->Get_Margin().Top.Get()),
            Bottom.Evaluate(owner->Get_Margin().Bottom.Get()),
            Left.Evaluate(owner->Get_Margin().Left.Get()),
            Right.Evaluate(owner->Get_Margin().Right.Get()),
            Status
        );
    }

    shadow shadow::Evaluate(Element* owner){
        shadow parents_shadow = owner->Get_Style().Shadow;

        return shadow(
            Direction.Evaluate(parents_shadow.Direction.Get()),
            Color.Evaluate(parents_shadow.Color.Get()),
            Opacity,
            Enabled,
            Status
        );
    }

    void Styling::Evaluate_Dynamic_Attribute_Values(){

    }

}