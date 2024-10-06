#include "Style.h"
#include "../Elements/Element.h"

namespace GGUI{

    void position::Embed_Value(Element* host){
        host->Set_Position(Value.Get<IVector2>());
    }

    void width::Embed_Value(Element* host){
        host->Set_Width(Value.Get<int>());
    }

    void height::Embed_Value(Element* host){
        host->Set_Height(Value.Get<int>());
    }

    void text_color::Embed_Value(Element* host){
        host->Set_Text_Color(Value.Get<RGB>());
    }

    void background_color::Embed_Value(Element* host){
        host->Set_Background_Color(Value.Get<RGB>());
    }

    void border_color::Embed_Value(Element* host){
        host->Set_Border_Color(Value.Get<RGB>());
    }

    void border_background_color::Embed_Value(Element* host){
        host->Set_Border_Background_Color(Value.Get<RGB>());
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
        host->Set_Opacity(Get());
    }

    margin margin::Evaluate(Element* owner){
        return margin(
            Top.Evaluate(owner->Get_Margin().Top.Get<unsigned int>()),
            Bottom.Evaluate(owner->Get_Margin().Bottom.Get<unsigned int>()),
            Left.Evaluate(owner->Get_Margin().Left.Get<unsigned int>()),
            Right.Evaluate(owner->Get_Margin().Right.Get<unsigned int>()),
            Status
        );
    }

    shadow shadow::Evaluate(Element* owner){
        shadow parents_shadow = owner->Get_Style().Shadow;

        return shadow(
            Direction.Evaluate(parents_shadow.Direction.Get<FVector3>()),
            Color.Evaluate(parents_shadow.Color.Get<RGB>()),
            Opacity,
            Enabled,
            Status
        );
    }

    void Styling::Evaluate_Dynamic_Attribute_Values(){

    }

}