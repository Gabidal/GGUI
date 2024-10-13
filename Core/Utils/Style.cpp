#include "Style.h"
#include "../Elements/Element.h"
#include "../Elements/List_View.h"

namespace GGUI{

    // EVALs
    // -_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_

    void position::Evaluate(Styling* parent){
        Value = Value.Evaluate(parent->Position.Get());
    }

    void width::Evaluate(Styling* parent){
        Value = Value.Evaluate(parent->Width.Get());
    }

    void height::Evaluate(Styling* parent){
        Value = Value.Evaluate(parent->Height.Get());
    }

    void text_color::Evaluate(Styling* parent){
        Value = Value.Evaluate(parent->Text_Color.Value.Get<RGB>());
    }

    void background_color::Evaluate(Styling* parent){
        Value = Value.Evaluate(parent->Background_Color.Value.Get<RGB>());
    }

    void border_color::Evaluate(Styling* parent){
        Value = Value.Evaluate(parent->Border_Color.Value.Get<RGB>());
    }

    void border_background_color::Evaluate(Styling* parent){
        Value = Value.Evaluate(parent->Border_Background_Color.Value.Get<RGB>());
    }

    void hover_border_color::Evaluate(Styling* parent){
        Value = Value.Evaluate(parent->Hover_Border_Color.Value.Get<RGB>());
    }

    void hover_text_color::Evaluate(Styling* parent){
        Value = Value.Evaluate(parent->Hover_Text_Color.Value.Get<RGB>());
    }

    void hover_background_color::Evaluate(Styling* parent){
        Value = Value.Evaluate(parent->Hover_Background_Color.Value.Get<RGB>());
    }

    void hover_border_background_color::Evaluate(Styling* parent){
        Value = Value.Evaluate(parent->Hover_Border_Background_Color.Value.Get<RGB>());
    }

    void focus_border_color::Evaluate(Styling* parent){
        Value = Value.Evaluate(parent->Focus_Border_Color.Value.Get<RGB>());
    }

    void focus_text_color::Evaluate(Styling* parent){
        Value = Value.Evaluate(parent->Focus_Text_Color.Value.Get<RGB>());
    }

    void focus_background_color::Evaluate(Styling* parent){
        Value = Value.Evaluate(parent->Focus_Background_Color.Value.Get<RGB>());
    }

    void focus_border_background_color::Evaluate(Styling* parent){
        Value = Value.Evaluate(parent->Focus_Border_Background_Color.Value.Get<RGB>());
    }

    void margin::Evaluate(Styling* parent){
        Top = Top.Evaluate(parent->Margin.Top.Get<unsigned int>());
        Bottom = Bottom.Evaluate(parent->Margin.Bottom.Get<unsigned int>());
        Left = Left.Evaluate(parent->Margin.Left.Get<unsigned int>());
        Right = Right.Evaluate(parent->Margin.Right.Get<unsigned int>());
    }

    void shadow::Evaluate(Styling* parent){
        Direction = Direction.Evaluate(parent->Shadow.Direction.Get<FVector3>());
        Color = Color.Evaluate(parent->Shadow.Color.Get<RGB>());
    }

    // Embeds
    // -_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_
    void position::Embed_Value(Styling* host){
        host->Position = *this;
    }

    void width::Embed_Value(Styling* host){
        host->Width = *this;
    }

    void height::Embed_Value(Styling* host){
        host->Height = *this;
    }

    void border_enabled::Embed_Value(Styling* host){
        host->Border_Enabled = *this;
    }

    void text_color::Embed_Value(Styling* host){
        host->Text_Color = *this;
    }

    void background_color::Embed_Value(Styling* host){
        host->Background_Color = *this;
    }

    void border_color::Embed_Value(Styling* host){
        host->Border_Color = *this;
    }

    void border_background_color::Embed_Value(Styling* host){
        host->Border_Background_Color = *this;
    }

    void hover_border_color::Embed_Value(Styling* host){
        host->Hover_Border_Color = *this;
    }

    void hover_text_color::Embed_Value(Styling* host){
        host->Hover_Text_Color = *this;
    }

    void hover_background_color::Embed_Value(Styling* host){
        host->Hover_Background_Color = *this;
    }

    void hover_border_background_color::Embed_Value(Styling* host){
        host->Hover_Border_Background_Color = *this;
    }

    void focus_border_color::Embed_Value(Styling* host){
        host->Focus_Border_Color = *this;
    }

    void focus_text_color::Embed_Value(Styling* host){
        host->Focus_Text_Color = *this;
    }

    void focus_background_color::Embed_Value(Styling* host){
        host->Focus_Background_Color = *this;
    }

    void focus_border_background_color::Embed_Value(Styling* host){
        host->Focus_Border_Background_Color = *this;
    }

    void styled_border::Embed_Value(Styling* host){
        host->Border_Style = *this;
    }

    void flow_priority::Embed_Value(Styling* host){
        host->Flow_Priority = *this;
    }

    void wrap::Embed_Value(Styling* host){
        host->Wrap = *this;
    }

    void allow_overflow::Embed_Value(Styling* host){
        host->Allow_Overflow = *this;
    }

    void allow_dynamic_size::Embed_Value(Styling* host){
        host->Allow_Dynamic_Size = *this;
    }

    void margin::Embed_Value(Styling* host){
        host->Margin = *this;
    }

    void shadow::Embed_Value(Styling* host){
        host->Shadow = *this;
    }

    void opacity::Embed_Value(Styling* host){
        host->Opacity = *this;
    }

    void allow_scrolling::Embed_Value(Styling* host){
        host->Allow_Scrolling = *this;
    }

    void align::Embed_Value(Styling* host){
        host->Align = *this;
    }

    void node::Embed_Value(Styling* host){
        host->Childs.push_back(Value);
    }

    void Styling::Evaluate_Dynamic_Attribute_Values(Element* owner){

        Element* point_of_interest = owner;

        if (owner->Get_Parent()){
            point_of_interest = owner->Get_Parent();
        }

        Styling tmp = point_of_interest->Get_Style();

        Styling& reference_style = tmp;

        Position.Evaluate(&reference_style);
        Width.Evaluate(&reference_style);
        Height.Evaluate(&reference_style);
        Border_Enabled.Evaluate(&reference_style);
        Text_Color.Evaluate(&reference_style);
        Background_Color.Evaluate(&reference_style);
        Border_Color.Evaluate(&reference_style);
        Border_Background_Color.Evaluate(&reference_style);
        Hover_Border_Color.Evaluate(&reference_style);
        Hover_Text_Color.Evaluate(&reference_style);
        Hover_Background_Color.Evaluate(&reference_style);
        Hover_Border_Background_Color.Evaluate(&reference_style);
        Focus_Border_Color.Evaluate(&reference_style);
        Focus_Text_Color.Evaluate(&reference_style);
        Focus_Background_Color.Evaluate(&reference_style);
        Focus_Border_Background_Color.Evaluate(&reference_style);
        Margin.Evaluate(&reference_style);
        Shadow.Evaluate(&reference_style);
        Opacity.Evaluate(&reference_style);
        Allow_Scrolling.Evaluate(&reference_style);
        Align.Evaluate(&reference_style);
    }

}