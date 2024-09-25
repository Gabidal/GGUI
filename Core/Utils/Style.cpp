#include "Style.h"
#include "../Elements/Element.h"

namespace GGUI{

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

}