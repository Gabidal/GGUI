#include "style.h"
#include "../../elements/element.h"
#include "../../elements/listView.h"
#include "../../elements/textField.h"
#include "../../elements/button.h"
#include "../renderer.h"

#include "./utils.h"

namespace GGUI{

    /**
     * @brief Destructor of the style_base class.
     *
     * This destructor takes care of deleting the "Other" pointer if it is a heap-allocated object.
     * It does this by calling the Is_Deletable() function to check if the pointer is likely to be
     * a heap-allocated object. If it is, it deletes the object using the delete keyword.
     */
    STYLING_INTERNAL::style_base::~style_base(){
        // Check if the other is closer to an stack_starting address or to heap
        // [[gnu::assume(Other != nullptr || Other == nullptr)]]; // Hint that 'Other' is initialized
        // (void)Other;
        if (getAllocationType(Other) == ALLOCATION_TYPE::HEAP)
            delete Other;
    }

    // EVALs
    // -_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_

    void position::Evaluate(styling* parent){
        X.Evaluate(Max(parent->Width.Get() - (parent->Border_Enabled.Value * 2), 1));
        Y.Evaluate(Max(parent->Height.Get() - (parent->Border_Enabled.Value * 2), 1));
        // Z.Evaluate(parent->Position.Get().Z);    // Since child as always parent.Z+1, there is no world where this is useful.
    }

    void width::Evaluate(styling* parent){
        Value.Evaluate(Max(parent->Width.Get() - (parent->Border_Enabled.Value * 2), 1));
    }

    void height::Evaluate(styling* parent){
        Value.Evaluate(Max(parent->Height.Get() - (parent->Border_Enabled.Value * 2), 1));
    }

    void text_color::Evaluate(styling* parent){
        Value.Evaluate(parent->Text_Color.Value.Get<RGB>());
    }

    void background_color::Evaluate(styling* parent){
        Value.Evaluate(parent->Background_Color.Value.Get<RGB>());
    }

    void border_color::Evaluate(styling* parent){
        Value.Evaluate(parent->Border_Color.Value.Get<RGB>());
    }

    void border_background_color::Evaluate(styling* parent){
        Value.Evaluate(parent->Border_Background_Color.Value.Get<RGB>());
    }

    void hover_border_color::Evaluate(styling* parent){
        Value.Evaluate(parent->Hover_Border_Color.Value.Get<RGB>());
    }

    void hover_text_color::Evaluate(styling* parent){
        Value.Evaluate(parent->Hover_Text_Color.Value.Get<RGB>());
    }

    void hover_background_color::Evaluate(styling* parent){
        Value.Evaluate(parent->Hover_Background_Color.Value.Get<RGB>());
    }

    void hover_border_background_color::Evaluate(styling* parent){
        Value.Evaluate(parent->Hover_Border_Background_Color.Value.Get<RGB>());
    }

    void focus_border_color::Evaluate(styling* parent){
        Value.Evaluate(parent->Focus_Border_Color.Value.Get<RGB>());
    }

    void focus_text_color::Evaluate(styling* parent){
        Value.Evaluate(parent->Focus_Text_Color.Value.Get<RGB>());
    }

    void focus_background_color::Evaluate(styling* parent){
        Value.Evaluate(parent->Focus_Background_Color.Value.Get<RGB>());
    }

    void focus_border_background_color::Evaluate(styling* parent){
        Value.Evaluate(parent->Focus_Border_Background_Color.Value.Get<RGB>());
    }

    void margin::Evaluate(styling* parent){
        Top.Evaluate(parent->Margin.Top.Get<unsigned int>());
        Bottom.Evaluate(parent->Margin.Bottom.Get<unsigned int>());
        Left.Evaluate(parent->Margin.Left.Get<unsigned int>());
        Right.Evaluate(parent->Margin.Right.Get<unsigned int>());
    }

    void shadow::Evaluate(styling* parent){
        Direction.Evaluate(parent->Shadow.Direction.Get<FVector3>());
        Color.Evaluate(parent->Shadow.Color.Get<RGB>());
    }

    // Embeds
    // -_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_

    /**
     * @brief Embeds the value of an RGB_VALUE object into a Styling object.
     * @param host The Styling object to embed the value into.
     * @param owner The Element that owns the Styling object.
     * @return A STAIN_TYPE indicating the type of stain that was embedded.
     * @details This function does not actually embed any values and simply returns STAIN_TYPE::CLEAN.
     */
    STAIN_TYPE STYLING_INTERNAL::RGB_VALUE::Embed_Value([[maybe_unused]] styling* host, [[maybe_unused]] element* owner) { return STAIN_TYPE::CLEAN; };

    /**
     * @brief Embeds the value of a BOOL_VALUE object into a Styling object.
     * @param host The Styling object to embed the value into.
     * @param owner The Element that owns the Styling object.
     * @return A STAIN_TYPE indicating the type of stain that was embedded.
     * @details This function does not actually embed any values and simply returns STAIN_TYPE::CLEAN.
     */
    STAIN_TYPE STYLING_INTERNAL::BOOL_VALUE::Embed_Value([[maybe_unused]] styling* host, [[maybe_unused]] element* owner) { return STAIN_TYPE::CLEAN; };

    /**
     * @brief Embeds the value of a NUMBER_VALUE object into a Styling object.
     * @param host The Styling object to embed the value into.
     * @param owner The Element that owns the Styling object.
     * @return A STAIN_TYPE indicating the type of stain that was embedded.
     * @details This function does not actually embed any values and simply returns STAIN_TYPE::CLEAN.
     */
    STAIN_TYPE STYLING_INTERNAL::NUMBER_VALUE::Embed_Value([[maybe_unused]] styling* host, [[maybe_unused]] element* owner) { return STAIN_TYPE::CLEAN; };

    /**
     * @brief Embeds the value of a Vector object into a Styling object.
     * @param host The Styling object to embed the value into.
     * @param owner The Element that owns the Styling object.
     * @return A STAIN_TYPE indicating the type of stain that was embedded.
     * @details This function does not actually embed any values and simply returns STAIN_TYPE::CLEAN.
     */
    STAIN_TYPE STYLING_INTERNAL::Vector::Embed_Value([[maybe_unused]] styling* host, [[maybe_unused]] element* owner) { return STAIN_TYPE::CLEAN; };

    STAIN_TYPE position::Embed_Value(styling* host, [[maybe_unused]] element* owner){
        host->Position = *this;

        return STAIN_TYPE::MOVE;
    }

    STAIN_TYPE width::Embed_Value(styling* host, element* owner){
        host->Width = *this;

        // Since dynamic size and percentage based size are two incompatible systems.
        if (host->Width.Value.Get_Type() == EVALUATION_TYPE::PERCENTAGE){
            allow_dynamic_size tmp = allow_dynamic_size(false);
            tmp.Embed_Value(host, owner);
        }

        return STAIN_TYPE::STRETCH;
    }

    STAIN_TYPE height::Embed_Value(styling* host, element* owner){
        host->Height = *this;

        // Since dynamic size and percentage based size are two incompatible systems.
        if (host->Width.Value.Get_Type() == EVALUATION_TYPE::PERCENTAGE){
            allow_dynamic_size tmp = allow_dynamic_size(false);
            tmp.Embed_Value(host, owner);
        }
        
        return STAIN_TYPE::STRETCH;
    }

    STAIN_TYPE enable_border::Embed_Value(styling* host, [[maybe_unused]] element* owner){
        host->Border_Enabled = *this;

        return STAIN_TYPE::EDGE;
    }

    STAIN_TYPE text_color::Embed_Value(styling* host, [[maybe_unused]] element* owner){
        host->Text_Color = *this;

        return STAIN_TYPE::COLOR;
    }

    STAIN_TYPE background_color::Embed_Value(styling* host, [[maybe_unused]] element* owner){
        host->Background_Color = *this;

        return STAIN_TYPE::COLOR;
    }

    STAIN_TYPE border_color::Embed_Value(styling* host, [[maybe_unused]] element* owner){
        host->Border_Color = *this;

        return STAIN_TYPE::COLOR;
    }

    STAIN_TYPE border_background_color::Embed_Value(styling* host, [[maybe_unused]] element* owner){
        host->Border_Background_Color = *this;

        return STAIN_TYPE::COLOR;
    }

    STAIN_TYPE hover_border_color::Embed_Value(styling* host, [[maybe_unused]] element* owner){
        host->Hover_Border_Color = *this;

        return STAIN_TYPE::COLOR;
    }

    STAIN_TYPE hover_text_color::Embed_Value(styling* host, [[maybe_unused]] element* owner){
        host->Hover_Text_Color = *this;

        return STAIN_TYPE::COLOR;
    }

    STAIN_TYPE hover_background_color::Embed_Value(styling* host, [[maybe_unused]] element* owner){
        host->Hover_Background_Color = *this;

        return STAIN_TYPE::COLOR;
    }

    STAIN_TYPE hover_border_background_color::Embed_Value(styling* host, [[maybe_unused]] element* owner){
        host->Hover_Border_Background_Color = *this;

        return STAIN_TYPE::COLOR;
    }

    STAIN_TYPE focus_border_color::Embed_Value(styling* host, [[maybe_unused]] element* owner){
        host->Focus_Border_Color = *this;

        return STAIN_TYPE::COLOR;
    }

    STAIN_TYPE focus_text_color::Embed_Value(styling* host, [[maybe_unused]] element* owner){
        host->Focus_Text_Color = *this;

        return STAIN_TYPE::COLOR;
    }

    STAIN_TYPE focus_background_color::Embed_Value(styling* host, [[maybe_unused]] element* owner){
        host->Focus_Background_Color = *this;

        return STAIN_TYPE::COLOR;
    }

    STAIN_TYPE focus_border_background_color::Embed_Value(styling* host, [[maybe_unused]] element* owner){
        host->Focus_Border_Background_Color = *this;

        return STAIN_TYPE::COLOR;
    }

    STAIN_TYPE styled_border::Embed_Value(styling* host, [[maybe_unused]] element* owner){
        host->Border_Style = *this;

        return STAIN_TYPE::EDGE;
    }

    STAIN_TYPE flow_priority::Embed_Value(styling* host, [[maybe_unused]] element* owner){
        host->Flow_Priority = *this;

        return STAIN_TYPE::DEEP;
    }

    STAIN_TYPE wrap::Embed_Value(styling* host, [[maybe_unused]] element* owner){
        host->Wrap = *this;

        return STAIN_TYPE::DEEP;
    }

    STAIN_TYPE allow_overflow::Embed_Value(styling* host, [[maybe_unused]] element* owner){
        host->Allow_Overflow = *this;

        return STAIN_TYPE::CLEAN;
    }

    STAIN_TYPE allow_dynamic_size::Embed_Value(styling* host, [[maybe_unused]] element* owner){
        host->Allow_Dynamic_Size = *this;

        return STAIN_TYPE::CLEAN;
    }

    STAIN_TYPE margin::Embed_Value(styling* host, [[maybe_unused]] element* owner){
        host->Margin = *this;

        return STAIN_TYPE::CLEAN;
    }

    STAIN_TYPE shadow::Embed_Value(styling* host, [[maybe_unused]] element* owner){
        host->Shadow = *this;

        return STAIN_TYPE::EDGE;
    }

    STAIN_TYPE opacity::Embed_Value(styling* host, [[maybe_unused]] element* owner){
        host->Opacity = *this;

        return STAIN_TYPE::STRETCH;
    }

    STAIN_TYPE allow_scrolling::Embed_Value(styling* host, [[maybe_unused]] element* owner){
        host->Allow_Scrolling = *this;

        return STAIN_TYPE::CLEAN;
    }

    STAIN_TYPE align::Embed_Value(styling* host, [[maybe_unused]] element* owner){
        host->Align = *this;

        return STAIN_TYPE::CLEAN;
    }

    STAIN_TYPE node::Embed_Value(styling* host, [[maybe_unused]]  element* owner){
        // Since we need to put the value adding through the owner elements own custom process.
        // Since the Value is typically given as an stack allocated local object, we need to transfer it into heap
        if (Has(getAllocationType(Value), ALLOCATION_TYPE::STACK))
            Value = Value->copy();

        host->Childs.push_back(Value);

        return STAIN_TYPE::DEEP;    // This also could just be a CLEAN value, since the Add_Child is determined to set the correct Stains.
    }
    
    STAIN_TYPE childs::Embed_Value(styling* host, [[maybe_unused]]  element* owner){
        for (auto* c : *this){
            // Since the Value is typically given as an stack allocated local object, we need to transfer it into heap
            if (Has(getAllocationType(c), ALLOCATION_TYPE::STACK))
                c = c->copy();

            host->Childs.push_back(c);
        }

        return STAIN_TYPE::DEEP;
    }

    STAIN_TYPE on_init::Embed_Value([[maybe_unused]] styling* host, element* owner){
        owner->setOnInit(Value);

        return STAIN_TYPE::CLEAN;
    }

    STAIN_TYPE on_destroy::Embed_Value([[maybe_unused]] styling* host, element* owner){
        owner->setOnDestroy(Value);

        return STAIN_TYPE::CLEAN;
    }

    STAIN_TYPE on_hide::Embed_Value([[maybe_unused]] styling* host, element* owner){
        owner->setOnHide(Value);

        return STAIN_TYPE::CLEAN;
    }

    STAIN_TYPE on_show::Embed_Value([[maybe_unused]] styling* host, element* owner){
        owner->setOnShow(Value);

        return STAIN_TYPE::CLEAN;
    }

    STAIN_TYPE name::Embed_Value([[maybe_unused]] styling* host, element* owner){
        owner->setName(Value);

        return STAIN_TYPE::CLEAN;
    }

    STAIN_TYPE title::Embed_Value([[maybe_unused]] styling* host, element* owner){
        // first make sure that the element is an Window type element.
        if (dynamic_cast<window*>(owner))
            ((window*)owner)->setTitle(Value);
        else
            throw std::runtime_error("The title attribute can only be used on Window type elements.");

        return STAIN_TYPE::CLEAN;
    }

    STAIN_TYPE display::Embed_Value([[maybe_unused]] styling* host, element* owner){
        owner->display(Value);

        return STAIN_TYPE::CLEAN;
    }

    STAIN_TYPE on_draw::Embed_Value([[maybe_unused]] styling* host, element* owner){
        // first make sure that the element is an Terminal_Canvas element.
        if (dynamic_cast<terminalCanvas*>(owner))
            ((terminalCanvas*)owner)->setOnDraw(Value);
        else
            throw std::runtime_error("The on_draw attribute can only be used on Terminal_Canvas type elements.");

        return STAIN_TYPE::CLEAN;
    }

    STAIN_TYPE text::Embed_Value([[maybe_unused]] styling* host, element* owner){
        // first make sure that the element is an Text_Field element.
        if (dynamic_cast<textField*>(owner))
            ((textField*)owner)->setText(Value);
        else if (dynamic_cast<button*>(owner))
            ((button*)owner)->setText(Value);
        else
            throw std::runtime_error("The text attribute can only be used on textField type elements.");

        return STAIN_TYPE::CLEAN;
    }

    STAIN_TYPE on_click::Embed_Value([[maybe_unused]] styling* host, element* owner){
        owner->onClick([this, owner](Event*){
            // The default, on_click wont do anything.
            // It will call the provided lambda (if any) and return true (allowing the event to propagate).

            this->Value(owner);

            return true;
        });

        return STAIN_TYPE::CLEAN;
    }

    const char* styled_border::get_border(const Border_Connection flags){
        // Corners
        if (flags == (Border_Connection::DOWN | Border_Connection::RIGHT))
            return TOP_LEFT_CORNER;
        else if (flags == (Border_Connection::DOWN | Border_Connection::LEFT))
            return TOP_RIGHT_CORNER;
        else if (flags == (Border_Connection::UP | Border_Connection::RIGHT))
            return BOTTOM_LEFT_CORNER;
        else if (flags == (Border_Connection::UP | Border_Connection::LEFT))
            return BOTTOM_RIGHT_CORNER;
        // Vertical lines
        else if (flags == (Border_Connection::DOWN | Border_Connection::UP))
            return VERTICAL_LINE;

        // Horizontal lines
        else if (flags == (Border_Connection::LEFT | Border_Connection::RIGHT))
            return HORIZONTAL_LINE;

        // connectors
        else if (flags == (Border_Connection::DOWN | Border_Connection::UP | Border_Connection::RIGHT))
            return VERTICAL_RIGHT_CONNECTOR;
        else if (flags == (Border_Connection::DOWN | Border_Connection::UP | Border_Connection::LEFT))
            return VERTICAL_LEFT_CONNECTOR;
        else if (flags == (Border_Connection::LEFT | Border_Connection::RIGHT | Border_Connection::DOWN))
            return HORIZONTAL_BOTTOM_CONNECTOR;
        else if (flags == (Border_Connection::LEFT | Border_Connection::RIGHT | Border_Connection::UP))
            return HORIZONTAL_TOP_CONNECTOR;

        // cross connectors
        else if (flags == (Border_Connection::LEFT | Border_Connection::RIGHT | Border_Connection::UP | Border_Connection::DOWN))
            return CROSS_CONNECTOR;
        else
            return nullptr;
    }

    Border_Connection styled_border::get_border_type(const char* border){
        if (border == TOP_LEFT_CORNER)
            return Border_Connection::DOWN | Border_Connection::RIGHT;
        else if (border == TOP_RIGHT_CORNER)
            return Border_Connection::DOWN | Border_Connection::LEFT;
        else if (border == BOTTOM_LEFT_CORNER)
            return Border_Connection::UP | Border_Connection::RIGHT;
        else if (border == BOTTOM_RIGHT_CORNER)
            return Border_Connection::UP | Border_Connection::LEFT;
        else if (border == VERTICAL_LINE)
            return Border_Connection::DOWN | Border_Connection::UP;
        else if (border == HORIZONTAL_LINE)
            return Border_Connection::LEFT | Border_Connection::RIGHT;
        else if (border == VERTICAL_RIGHT_CONNECTOR)
            return Border_Connection::DOWN | Border_Connection::UP | Border_Connection::RIGHT;
        else if (border == VERTICAL_LEFT_CONNECTOR)
            return Border_Connection::DOWN | Border_Connection::UP | Border_Connection::LEFT;
        else if (border == HORIZONTAL_BOTTOM_CONNECTOR)
            return Border_Connection::LEFT | Border_Connection::RIGHT | Border_Connection::DOWN;
        else if (border == HORIZONTAL_TOP_CONNECTOR)
            return Border_Connection::LEFT | Border_Connection::RIGHT | Border_Connection::UP;
        else if (border == CROSS_CONNECTOR)
            return Border_Connection::LEFT | Border_Connection::RIGHT | Border_Connection::UP | Border_Connection::DOWN;
        else return Border_Connection::NONE;
    }

    STYLING_INTERNAL::style_base* node::Copy() const {
        node* new_one = new node(*this);
        new_one->Value = new_one->Value->copy();
        return new_one;
    }
            
    STYLING_INTERNAL::style_base* childs::Copy() const {
        childs* new_one = new childs(*this);

        for (int i = 0; i < length(); i++){
            new_one->Value[i] = this->Value[i]->copy();
        }

        return new_one;
    }

    styling* styling::Get_Reference(element* owner){
        // Determine the point of interest for style evaluation
        element* point_of_interest = owner;

        // If the element has a parent, use the parent's style for evaluation
        if (owner->getParent()) {
            point_of_interest = owner->getParent();
        }

        // Get the style of the point of interest
        return point_of_interest->getDirectStyle();
    }

    /**
     * @brief Evaluates dynamic attribute values for the given element.
     *
     * This function evaluates the dynamic attribute values of the styling associated
     * with the specified element. It determines the point of interest, which is
     * either the element's parent or the element itself if no parent exists,
     * and uses its style as a reference for evaluation.
     *
     * @param owner The element whose dynamic attributes are to be evaluated.
     * @return True if there wae changes in the attributes evaluated, false otherwise.
     */
    bool styling::Evaluate_Dynamic_Attribute_Values(element* owner) {

        bool Changed_Attributes = false;

        // Use the retrieved style as a reference for evaluation
        styling* reference_style = Get_Reference(owner);

        // Evaluate each dynamic attribute against the reference style
        Changed_Attributes |= Evaluate_Dynamic_Position(owner, reference_style);
        Changed_Attributes |= Evaluate_Dynamic_Dimensions(owner, reference_style);
        Changed_Attributes |= Evaluate_Dynamic_Border(owner, reference_style);
        Changed_Attributes |= Evaluate_Dynamic_Colors(owner, reference_style);

        Margin.Evaluate(reference_style);
        Shadow.Evaluate(reference_style);
        Opacity.Evaluate(reference_style);
        Allow_Scrolling.Evaluate(reference_style);
        Align.Evaluate(reference_style);

        return Changed_Attributes;
    }

    bool styling::Evaluate_Dynamic_Position(element* owner, styling* reference){
        if (!reference){
            reference = Get_Reference(owner);
        }

        position previous_value = Position;

        Position.Evaluate(reference);

        // check if position is still the same
        return previous_value != Position;
    }

    bool styling::Evaluate_Dynamic_Dimensions(element* owner, styling* reference){
        if (!reference){
            reference = Get_Reference(owner);
        }

        width previous_width = Width;
        height previous_height = Height;

        Width.Evaluate(reference);
        Height.Evaluate(reference);

        // check if width or height is still the same
        return previous_width != Width || previous_height != Height;
    }

    bool styling::Evaluate_Dynamic_Border(element* owner, styling* reference){
        if (!reference){
            reference = Get_Reference(owner);
        }

        enable_border previous_value = Border_Enabled;

        Border_Enabled.Evaluate(reference);

        // check if border is still the same
        return previous_value != Border_Enabled;
    }

    bool styling::Evaluate_Dynamic_Colors(element* owner, styling* reference){
        if (!reference){
            reference = Get_Reference(owner);
        }

        text_color previous_text_color = Text_Color;
        background_color previous_background_color = Background_Color;
        border_color previous_border_color = Border_Color;
        border_background_color previous_border_background_color = Border_Background_Color;
        hover_border_color previous_hover_border_color = Hover_Border_Color;
        hover_text_color previous_hover_text_color = Hover_Text_Color;
        hover_background_color previous_hover_background_color = Hover_Background_Color;
        hover_border_background_color previous_hover_border_background_color = Hover_Border_Background_Color;
        focus_border_color previous_focus_border_color = Focus_Border_Color;
        focus_text_color previous_focus_text_color = Focus_Text_Color;
        focus_background_color previous_focus_background_color = Focus_Background_Color;
        focus_border_background_color previous_focus_border_background_color = Focus_Border_Background_Color;

        Text_Color.Evaluate(reference);
        Background_Color.Evaluate(reference);
        Border_Color.Evaluate(reference);
        Border_Background_Color.Evaluate(reference);
        Hover_Border_Color.Evaluate(reference);
        Hover_Text_Color.Evaluate(reference);
        Hover_Background_Color.Evaluate(reference);
        Hover_Border_Background_Color.Evaluate(reference);
        Focus_Border_Color.Evaluate(reference);
        Focus_Text_Color.Evaluate(reference);
        Focus_Background_Color.Evaluate(reference);
        Focus_Border_Background_Color.Evaluate(reference);

        // check if any of the colors are still the same
        return previous_text_color != Text_Color ||
               previous_background_color != Background_Color ||
               previous_border_color != Border_Color ||
               previous_border_background_color != Border_Background_Color ||
               previous_hover_border_color != Hover_Border_Color ||
               previous_hover_text_color != Hover_Text_Color ||
               previous_hover_background_color != Hover_Background_Color ||
               previous_hover_border_background_color != Hover_Border_Background_Color ||
               previous_focus_border_color != Focus_Border_Color ||
               previous_focus_text_color != Focus_Text_Color ||
               previous_focus_background_color != Focus_Background_Color || 
               previous_focus_border_background_color != Focus_Border_Background_Color;
    }

    /**
     * @brief Copies the values of the given Styling object to the current object.
     *
     * This will copy all the values of the given Styling object to the current object.
     *
     * @param other The Styling object to copy from.
     */
    void GGUI::styling::Copy(const styling& other){
        Position = other.Position;
        Width = other.Width;
        Height = other.Height;
        Border_Enabled = other.Border_Enabled;
        Text_Color = other.Text_Color;
        Background_Color = other.Background_Color;
        Border_Color = other.Border_Color;
        Border_Background_Color = other.Border_Background_Color;
        Hover_Border_Color = other.Hover_Border_Color;
        Hover_Text_Color = other.Hover_Text_Color;
        Hover_Background_Color = other.Hover_Background_Color;
        Hover_Border_Background_Color = other.Hover_Border_Background_Color;
        Focus_Border_Color = other.Focus_Border_Color;
        Focus_Text_Color = other.Focus_Text_Color;
        Focus_Background_Color = other.Focus_Background_Color;
        Focus_Border_Background_Color = other.Focus_Border_Background_Color;
        Border_Style = other.Border_Style;
        Flow_Priority = other.Flow_Priority;
        Wrap = other.Wrap;
        Allow_Overflow = other.Allow_Overflow;
        Allow_Dynamic_Size = other.Allow_Dynamic_Size;
        Margin = other.Margin;
        Shadow = other.Shadow;
        Opacity = other.Opacity;
        Allow_Scrolling = other.Allow_Scrolling;
        Align = other.Align;

        // Add the new childs to the already existing ones
        Childs.insert(Childs.end(), other.Childs.begin(), other.Childs.end());
        
        // Copy the un_parsed_styles
        un_parsed_styles = other.un_parsed_styles;
    }

    void GGUI::styling::Copy_Un_Parsed_Styles(){
        // for the cleaning afterwards
        STYLING_INTERNAL::style_base* cleaning_handle = un_parsed_styles;

        // deep copies the 'Other' members and uses their respective Copy virtual functions.
        STYLING_INTERNAL::style_base* current_attribute = un_parsed_styles;

        STYLING_INTERNAL::style_base* previous_attribute = nullptr;

        while (current_attribute){
            // Shallow copy the current attribute from stack into heap.
            STYLING_INTERNAL::style_base* anchor = current_attribute->Copy();

            // Then set the current_attribute into the nested one
            if (previous_attribute)
                previous_attribute->Other = anchor;
            else    // this means that this is the first occurrence, so set it as the new head
                un_parsed_styles = anchor;

            // Then set the current_attribute into the nested one
            previous_attribute = anchor;

            // Then set the current_attribute into the nested one
            current_attribute = current_attribute->Other;
        }

        // now we can clean the 'Others' from the clean_handle
        while (cleaning_handle){
            // add an anchor
            STYLING_INTERNAL::style_base* dish = cleaning_handle;

            // Then set the current_attribute into the nested one
            cleaning_handle = cleaning_handle->Other;

            // check if this was the last dish and so no 'Other's in it.
            if (dish)
                // now we can release the anchor
                dish->Other = nullptr;
        }
    }

    /**
     * Embeds the styles of the current styling object into the element.
     * 
     * This function is used to embed the styles of the current styling object into the element.
     * It takes the element as a parameter and embeds the styles into it.
     * The styles are embedded by looping through the un_parsed_styles list and calling the Embed_Value function on each attribute.
     * The Embed_Value function is responsible for embedding the attribute into the element.
     * The changes to the element are recorded in the changes variable, which is of type STAIN.
     * The type of the changes is then added to the element's stains list.
     * The function returns nothing.
     * @param owner The element to which the styles will be embedded.
     */
    void styling::Embed_Styles(element* owner){
        STYLING_INTERNAL::style_base* current_attribute = un_parsed_styles;

        STAIN changes;

        // This is the first pass for the INSTANT ordered style_bases:
        // Loop until no further nested attributes.
        while (current_attribute){

            if (current_attribute->Order == EMBED_ORDER::INSTANT)
                // First embed the current attribute
                changes.Dirty(current_attribute->Embed_Value(this, owner));

            // Then set the current_attribute into the nested one
            current_attribute = current_attribute->Other;
        }

        // This is the second pass for the DELAYED ordered style_bases:
        current_attribute = un_parsed_styles;
        
        // Loop until no further nested attributes.
        while (current_attribute){

            if (current_attribute->Order == EMBED_ORDER::DELAYED)
                // First embed the current attribute
                changes.Dirty(current_attribute->Embed_Value(this, owner));

            // Then set the current_attribute into the nested one
            current_attribute = current_attribute->Other;
        }

        // Evaluate itself
        bool Changes_After_Eval = Evaluate_Dynamic_Attribute_Values(owner);

        if (Changes_After_Eval)
            owner->fullyStain();

        // now we need to first move all the childs first to an temporary list
        std::vector<element*> tmp_childs = Childs;

        Childs.clear();

        // For global variables to work, we need to clean each and every 'Other' member from the un_parsed_styles, so that the next user of global styles doesn't start parse unallocated stuff.
        current_attribute = un_parsed_styles;

        // Wash dishes
        while (current_attribute){
            // add an anchor
            STYLING_INTERNAL::style_base* dish = current_attribute;

            // Then set the current_attribute into the nested one
            current_attribute = current_attribute->Other;

            // check if this was the last dish and so no 'Other's in it.
            if (dish)
                // now we can release the anchor
                dish->Other = nullptr;
        }

        // Now we can one by one add them back via the official channel
        for (element* c : tmp_childs){
            owner->addChild(c);
        }

        owner->addStain(changes.Type);
    }

}