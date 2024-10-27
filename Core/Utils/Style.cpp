#include "Style.h"
#include "../Elements/Element.h"
#include "../Elements/List_View.h"
#include "../Renderer.h"

namespace GGUI{

    STYLING_INTERNAL::style_base::~style_base(){
        if (Other){
            // Check if the other is closer to an stack_starting address or to heap
            if (Is_Deletable(Other))
                delete Other;
        }
    }

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

    STAIN_TYPE STYLING_INTERNAL::RGB_VALUE::Embed_Value([[maybe_unused]] Styling* host, [[maybe_unused]] Element* owner) { return STAIN_TYPE::CLEAN; };

    STAIN_TYPE STYLING_INTERNAL::BOOL_VALUE::Embed_Value([[maybe_unused]] Styling* host, [[maybe_unused]] Element* owner) { return STAIN_TYPE::CLEAN; };

    STAIN_TYPE STYLING_INTERNAL::NUMBER_VALUE::Embed_Value([[maybe_unused]] Styling* host, [[maybe_unused]] Element* owner) { return STAIN_TYPE::CLEAN; };

    STAIN_TYPE STYLING_INTERNAL::Vector::Embed_Value([[maybe_unused]] Styling* host, [[maybe_unused]] Element* owner) { return STAIN_TYPE::CLEAN; };

    STAIN_TYPE position::Embed_Value(Styling* host, [[maybe_unused]] Element* owner){
        host->Position = *this;

        return STAIN_TYPE::MOVE;
    }

    STAIN_TYPE width::Embed_Value(Styling* host, [[maybe_unused]] Element* owner){
        host->Width = *this;

        return STAIN_TYPE::STRETCH;
    }

    STAIN_TYPE height::Embed_Value(Styling* host, [[maybe_unused]] Element* owner){
        host->Height = *this;
        
        return STAIN_TYPE::STRETCH;
    }

    STAIN_TYPE enable_border::Embed_Value(Styling* host, [[maybe_unused]] Element* owner){
        host->Border_Enabled = *this;

        return STAIN_TYPE::EDGE;
    }

    STAIN_TYPE text_color::Embed_Value(Styling* host, [[maybe_unused]] Element* owner){
        host->Text_Color = *this;

        return STAIN_TYPE::COLOR;
    }

    STAIN_TYPE background_color::Embed_Value(Styling* host, [[maybe_unused]] Element* owner){
        host->Background_Color = *this;

        return STAIN_TYPE::COLOR;
    }

    STAIN_TYPE border_color::Embed_Value(Styling* host, [[maybe_unused]] Element* owner){
        host->Border_Color = *this;

        return STAIN_TYPE::COLOR;
    }

    STAIN_TYPE border_background_color::Embed_Value(Styling* host, [[maybe_unused]] Element* owner){
        host->Border_Background_Color = *this;

        return STAIN_TYPE::COLOR;
    }

    STAIN_TYPE hover_border_color::Embed_Value(Styling* host, [[maybe_unused]] Element* owner){
        host->Hover_Border_Color = *this;

        return STAIN_TYPE::COLOR;
    }

    STAIN_TYPE hover_text_color::Embed_Value(Styling* host, [[maybe_unused]] Element* owner){
        host->Hover_Text_Color = *this;

        return STAIN_TYPE::COLOR;
    }

    STAIN_TYPE hover_background_color::Embed_Value(Styling* host, [[maybe_unused]] Element* owner){
        host->Hover_Background_Color = *this;

        return STAIN_TYPE::COLOR;
    }

    STAIN_TYPE hover_border_background_color::Embed_Value(Styling* host, [[maybe_unused]] Element* owner){
        host->Hover_Border_Background_Color = *this;

        return STAIN_TYPE::COLOR;
    }

    STAIN_TYPE focus_border_color::Embed_Value(Styling* host, [[maybe_unused]] Element* owner){
        host->Focus_Border_Color = *this;

        return STAIN_TYPE::COLOR;
    }

    STAIN_TYPE focus_text_color::Embed_Value(Styling* host, [[maybe_unused]] Element* owner){
        host->Focus_Text_Color = *this;

        return STAIN_TYPE::COLOR;
    }

    STAIN_TYPE focus_background_color::Embed_Value(Styling* host, [[maybe_unused]] Element* owner){
        host->Focus_Background_Color = *this;

        return STAIN_TYPE::COLOR;
    }

    STAIN_TYPE focus_border_background_color::Embed_Value(Styling* host, [[maybe_unused]] Element* owner){
        host->Focus_Border_Background_Color = *this;

        return STAIN_TYPE::COLOR;
    }

    STAIN_TYPE styled_border::Embed_Value(Styling* host, [[maybe_unused]] Element* owner){
        host->Border_Style = *this;

        return STAIN_TYPE::EDGE;
    }

    STAIN_TYPE flow_priority::Embed_Value(Styling* host, [[maybe_unused]] Element* owner){
        host->Flow_Priority = *this;

        return STAIN_TYPE::DEEP;
    }

    STAIN_TYPE wrap::Embed_Value(Styling* host, [[maybe_unused]] Element* owner){
        host->Wrap = *this;

        return STAIN_TYPE::DEEP;
    }

    STAIN_TYPE allow_overflow::Embed_Value(Styling* host, [[maybe_unused]] Element* owner){
        host->Allow_Overflow = *this;

        return STAIN_TYPE::CLEAN;
    }

    STAIN_TYPE allow_dynamic_size::Embed_Value(Styling* host, [[maybe_unused]] Element* owner){
        host->Allow_Dynamic_Size = *this;

        return STAIN_TYPE::CLEAN;
    }

    STAIN_TYPE margin::Embed_Value(Styling* host, [[maybe_unused]] Element* owner){
        host->Margin = *this;

        return STAIN_TYPE::CLEAN;
    }

    STAIN_TYPE shadow::Embed_Value(Styling* host, [[maybe_unused]] Element* owner){
        host->Shadow = *this;

        return STAIN_TYPE::EDGE;
    }

    STAIN_TYPE opacity::Embed_Value(Styling* host, [[maybe_unused]] Element* owner){
        host->Opacity = *this;

        return STAIN_TYPE::STRETCH;
    }

    STAIN_TYPE allow_scrolling::Embed_Value(Styling* host, [[maybe_unused]] Element* owner){
        host->Allow_Scrolling = *this;

        return STAIN_TYPE::CLEAN;
    }

    STAIN_TYPE align::Embed_Value(Styling* host, [[maybe_unused]] Element* owner){
        host->Align = *this;

        return STAIN_TYPE::CLEAN;
    }

    STAIN_TYPE node::Embed_Value([[maybe_unused]] Styling* host, Element* owner){
        // Since we need to put the value adding through the owner elements own custom process.
        owner->Add_Child(Value); 

        return STAIN_TYPE::DEEP;    // This also could just be a CLEAN value, since the Add_Child is determined to set the correct Stains.
    }

    STAIN_TYPE childs::Embed_Value([[maybe_unused]] Styling* host, Element* owner){
        for (auto* c : Value){
            owner->Add_Child(c);
        }

        return STAIN_TYPE::DEEP;
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

    void Styling::Embed_Styles(Element* owner){
        STYLING_INTERNAL::style_base* current_attribute = un_parsed_styles;

        STAIN changes;

        // Loop until no further nested attributes.
        while (current_attribute){

            // First embed the current attribute
            changes.Dirty(current_attribute->Embed_Value(this, owner));

            // Then set the current_attribute into the nested one
            current_attribute = current_attribute->Other;
        }

        owner->Add_Stain(changes.Type);
    }
}