#include "Style.h"
#include "../Elements/Element.h"
#include "../Elements/List_View.h"

namespace GGUI{

    // EVALs
    // -_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_

    void position::Evaluate(Element* owner){
        Value = Value.Evaluate(owner->Get_Position());

        Embed_Value(owner);
    }

    void width::Evaluate(Element* owner){
        Value = Value.Evaluate(owner->Get_Width());

        Embed_Value(owner);
    }

    void height::Evaluate(Element* owner){
        Value = Value.Evaluate(owner->Get_Height());

        Embed_Value(owner);
    }

    void text_color::Evaluate(Element* owner){
        Value = Value.Evaluate(owner->Get_Text_Color());

        Embed_Value(owner);
    }

    void background_color::Evaluate(Element* owner){
        Value = Value.Evaluate(owner->Get_Background_Color());

        Embed_Value(owner);
    }

    void border_color::Evaluate(Element* owner){
        Value = Value.Evaluate(owner->Get_Border_Color());

        Embed_Value(owner);
    }

    void border_background_color::Evaluate(Element* owner){
        Value = Value.Evaluate(owner->Get_Border_Background_Color());

        Embed_Value(owner);
    }

    void hover_border_color::Evaluate(Element* owner){
        Value = Value.Evaluate(owner->Get_Style().Hover_Background_Color.Value.Get<RGB>());

        Embed_Value(owner);
    }

    void hover_text_color::Evaluate(Element* owner){
        Value = Value.Evaluate(owner->Get_Style().Hover_Text_Color.Value.Get<RGB>());

        Embed_Value(owner);
    }

    void hover_background_color::Evaluate(Element* owner){
        Value = Value.Evaluate(owner->Get_Style().Hover_Background_Color.Value.Get<RGB>());

        Embed_Value(owner);
    }

    void hover_border_background_color::Evaluate(Element* owner){
        Value = Value.Evaluate(owner->Get_Style().Hover_Border_Background_Color.Value.Get<RGB>());

        Embed_Value(owner);
    }

    void focus_border_color::Evaluate(Element* owner){
        Value = Value.Evaluate(owner->Get_Style().Focus_Border_Color.Value.Get<RGB>());

        Embed_Value(owner);
    }

    void focus_text_color::Evaluate(Element* owner){
        Value = Value.Evaluate(owner->Get_Style().Focus_Text_Color.Value.Get<RGB>());

        Embed_Value(owner);
    }

    void focus_background_color::Evaluate(Element* owner){
        Value = Value.Evaluate(owner->Get_Style().Focus_Background_Color.Value.Get<RGB>());

        Embed_Value(owner);
    }

    void focus_border_background_color::Evaluate(Element* owner){
        Value = Value.Evaluate(owner->Get_Style().Focus_Border_Background_Color.Value.Get<RGB>());

        Embed_Value(owner);
    }

    void margin::Evaluate(Element* owner){
        Top = Top.Evaluate(owner->Get_Margin().Top.Get<unsigned int>());
        Bottom = Bottom.Evaluate(owner->Get_Margin().Bottom.Get<unsigned int>());
        Left = Left.Evaluate(owner->Get_Margin().Left.Get<unsigned int>());
        Right = Right.Evaluate(owner->Get_Margin().Right.Get<unsigned int>());

        Embed_Value(owner);
    }

    void shadow::Evaluate(Element* owner){
        shadow parents_shadow = owner->Get_Style().Shadow;

        Direction = Direction.Evaluate(parents_shadow.Direction.Get<FVector3>());
        Color = Color.Evaluate(parents_shadow.Color.Get<RGB>());

        Embed_Value(owner);
    }

    // Embeds
    // -_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_

    void position::Embed_Value(Element* host){
        host->Set_Position(Value.Get<IVector2>());
    }

    void position::Embed_Value(Styling* host){
        host->Position = *this;
    }

    void width::Embed_Value(Element* host){
        host->Set_Width(Value.Get<int>());
    }

    void width::Embed_Value(Styling* host){
        host->Width = *this;
    }

    void height::Embed_Value(Element* host){
        host->Set_Height(Value.Get<int>());
    }

    void height::Embed_Value(Styling* host){
        host->Height = *this;
    }

    void border_enabled::Embed_Value(Element* host){
        host->Show_Border(Value);
    }

    void border_enabled::Embed_Value(Styling* host){
        host->Border_Enabled = *this;
    }

    void text_color::Embed_Value(Element* host){
        host->Set_Text_Color(Value.Get<RGB>());
    }

    void text_color::Embed_Value(Styling* host){
        host->Text_Color = *this;
    }

    void background_color::Embed_Value(Element* host){
        host->Set_Background_Color(Value.Get<RGB>());
    }

    void background_color::Embed_Value(Styling* host){
        host->Background_Color = *this;
    }

    void border_color::Embed_Value(Element* host){
        host->Set_Border_Color(Value.Get<RGB>());
    }

    void border_color::Embed_Value(Styling* host){
        host->Border_Color = *this;
    }

    void border_background_color::Embed_Value(Element* host){
        host->Set_Border_Background_Color(Value.Get<RGB>());
    }

    void border_background_color::Embed_Value(Styling* host){
        host->Border_Background_Color = *this;
    }

    void hover_border_color::Embed_Value(Element* host){
        host->Set_Hover_Border_Color(Value.Get<RGB>());
    }

    void hover_border_color::Embed_Value(Styling* host){
        host->Hover_Border_Color = *this;
    }

    void hover_text_color::Embed_Value(Element* host){
        host->Set_Hover_Text_Color(Value.Get<RGB>());
    }

    void hover_text_color::Embed_Value(Styling* host){
        host->Hover_Text_Color = *this;
    }

    void hover_background_color::Embed_Value(Element* host){
        host->Set_Hover_Background_Color(Value.Get<RGB>());
    }

    void hover_background_color::Embed_Value(Styling* host){
        host->Hover_Background_Color = *this;
    }

    void hover_border_background_color::Embed_Value(Element* host){
        host->Set_Hover_Border_Background_Color(Value.Get<RGB>());
    }

    void hover_border_background_color::Embed_Value(Styling* host){
        host->Hover_Border_Background_Color = *this;
    }

    void focus_border_color::Embed_Value(Element* host){
        host->Set_Focus_Border_Color(Value.Get<RGB>());
    }

    void focus_border_color::Embed_Value(Styling* host){
        host->Focus_Border_Color = *this;
    }

    void focus_text_color::Embed_Value(Element* host){
        host->Set_Focus_Text_Color(Value.Get<RGB>());
    }

    void focus_text_color::Embed_Value(Styling* host){
        host->Focus_Text_Color = *this;
    }

    void focus_background_color::Embed_Value(Element* host){
        host->Set_Focus_Background_Color(Value.Get<RGB>());
    }

    void focus_background_color::Embed_Value(Styling* host){
        host->Focus_Background_Color = *this;
    }

    void focus_border_background_color::Embed_Value(Element* host){
        host->Set_Focus_Border_Background_Color(Value.Get<RGB>());
    }

    void focus_border_background_color::Embed_Value(Styling* host){
        host->Focus_Border_Background_Color = *this;
    }

    void styled_border::Embed_Value(Element* host){
        host->Set_Custom_Border_Style(*this);
    }

    void styled_border::Embed_Value(Styling* host){
        host->Border_Style = *this;
    }

    void flow_priority::Embed_Value(Element* host){
        host->Set_Flow_Priority(Value);
    }

    void flow_priority::Embed_Value(Styling* host){
        host->Flow_Priority = *this;
    }

    void wrap::Embed_Value(Element* host){
        host->Set_Wrap(Value);
    }

    void wrap::Embed_Value(Styling* host){
        host->Wrap = *this;
    }

    void allow_overflow::Embed_Value(Element* host){
        host->Allow_Overflow(Value);
    }

    void allow_overflow::Embed_Value(Styling* host){
        host->Allow_Overflow = *this;
    }

    void allow_dynamic_size::Embed_Value(Element* host){
        host->Allow_Dynamic_Size(Value);
    }

    void allow_dynamic_size::Embed_Value(Styling* host){
        host->Allow_Dynamic_Size = *this;
    }

    void margin::Embed_Value(Element* host){
        host->Set_Margin(*this);
    }

    void margin::Embed_Value(Styling* host){
        host->Margin = *this;
    }

    void shadow::Embed_Value(Element* host){
        host->Set_Shadow(*this);
    }

    void shadow::Embed_Value(Styling* host){
        host->Shadow = *this;
    }

    void opacity::Embed_Value(Element* host){
        host->Set_Opacity(Get());
    }

    void opacity::Embed_Value(Styling* host){
        host->Opacity = *this;
    }

    void allow_scrolling::Embed_Value(Element* host){
        // Since scroll enabling is only for Scroll_Views, we need to make an type check for the element to check if the host is an Scroll_View with dynamic_cast
        if (GGUI::Scroll_View* sv = dynamic_cast<Scroll_View*>(host)){
            sv->Allow_Scrolling(Value);
        }
    }

    void allow_scrolling::Embed_Value(Styling* host){
        host->Allow_Scrolling = *this;
    }

    void align::Embed_Value(Element* host){
        host->Set_Align(Value);
    }

    void align::Embed_Value(Styling* host){
        host->Align = *this;
    }

    void Styling::Evaluate_Dynamic_Attribute_Values(Element* owner){
        Position.Evaluate(owner);
        Width.Evaluate(owner);
        Height.Evaluate(owner);
        Border_Enabled.Evaluate(owner);
        Text_Color.Evaluate(owner);
        Background_Color.Evaluate(owner);
        Border_Color.Evaluate(owner);
        Border_Background_Color.Evaluate(owner);
        Hover_Border_Color.Evaluate(owner);
        Hover_Text_Color.Evaluate(owner);
        Hover_Background_Color.Evaluate(owner);
        Hover_Border_Background_Color.Evaluate(owner);
        Focus_Border_Color.Evaluate(owner);
        Focus_Text_Color.Evaluate(owner);
        Focus_Background_Color.Evaluate(owner);
        Focus_Border_Background_Color.Evaluate(owner);
        Margin.Evaluate(owner);
        Shadow.Evaluate(owner);
        Opacity.Evaluate(owner);
        Allow_Scrolling.Evaluate(owner);
        Align.Evaluate(owner);
    }

}