#include "style.h"
#include "../../elements/element.h"
#include "../../elements/listView.h"
#include "../../elements/textField.h"
#include "../../elements/switch.h"
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
    STYLING_INTERNAL::styleBase::~styleBase(){
        // Check if the other is closer to an stack_starting address or to heap
        // [[gnu::assume(Other != nullptr || Other == nullptr)]]; // Hint that 'Other' is initialized
        // (void)Other;
        if (INTERNAL::getAllocationType(next) == INTERNAL::ALLOCATION_TYPE::HEAP)
            delete next;
    }

    // EVALs
    // -_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_

    void position::evaluate(const styling* self, const styling* parent){
        x.evaluate(INTERNAL::Max(
            parent->Width.get()                     // The base width of the parent
            - (parent->Border_Enabled.value * 2)    // Offset the width by 2 for both sides of border.
            - self->Width.get()                     // So that the relativity will take into count position - width. so that 1.0f doesn't put the element out of bound.
        , 1));
        y.evaluate(INTERNAL::Max(
            parent->Height.get()                    // The base height of the parent 
            - (parent->Border_Enabled.value * 2)    // Offset the height by 2 for both sides of border.
            - self->Height.get()                    // So that the relativity will take into count position - height. so that 1.0f doesn't put the element out of bound.
        , 1));
        // Z.evaluate(parent->Position.Get().Z);    // Since child as always parent.Z+1, there is no world where this is useful.
    }

    void width::evaluate([[maybe_unused]] const styling* self, const styling* parent){
        number.evaluate(INTERNAL::Max(parent->Width.get() - (parent->Border_Enabled.value * 2), 1));
    }

    void height::evaluate([[maybe_unused]] const styling* self, const styling* parent){
        number.evaluate(INTERNAL::Max(parent->Height.get() - (parent->Border_Enabled.value * 2), 1));
    }

    void textColor::evaluate([[maybe_unused]] const styling* self, const styling* parent){
        color.evaluate(parent->Text_Color.color.get<RGB>());
    }

    void backgroundColor::evaluate([[maybe_unused]] const styling* self, const styling* parent){
        color.evaluate(parent->Background_Color.color.get<RGB>());
    }

    void borderColor::evaluate([[maybe_unused]] const styling* self, const styling* parent){
        color.evaluate(parent->Border_Color.color.get<RGB>());
    }

    void borderBackgroundColor::evaluate([[maybe_unused]] const styling* self, const styling* parent){
        color.evaluate(parent->Border_Background_Color.color.get<RGB>());
    }

    void hoverBorderColor::evaluate([[maybe_unused]] const styling* self, const styling* parent){
        color.evaluate(parent->Hover_Border_Color.color.get<RGB>());
    }

    void hoverTextColor::evaluate([[maybe_unused]] const styling* self, const styling* parent){
        color.evaluate(parent->Hover_Text_Color.color.get<RGB>());
    }

    void hoverBackgroundColor::evaluate([[maybe_unused]] const styling* self, const styling* parent){
        color.evaluate(parent->Hover_Background_Color.color.get<RGB>());
    }

    void hoverBorderBackgroundColor::evaluate([[maybe_unused]] const styling* self, const styling* parent){
        color.evaluate(parent->Hover_Border_Background_Color.color.get<RGB>());
    }

    void focusBorderColor::evaluate([[maybe_unused]] const styling* self, const styling* parent){
        color.evaluate(parent->Focus_Border_Color.color.get<RGB>());
    }

    void focusTextColor::evaluate([[maybe_unused]] const styling* self, const styling* parent){
        color.evaluate(parent->Focus_Text_Color.color.get<RGB>());
    }

    void focusBackgroundColor::evaluate([[maybe_unused]] const styling* self, const styling* parent){
        color.evaluate(parent->Focus_Background_Color.color.get<RGB>());
    }

    void focusBorderBackgroundColor::evaluate([[maybe_unused]] const styling* self, const styling* parent){
        color.evaluate(parent->Focus_Border_Background_Color.color.get<RGB>());
    }

    void margin::evaluate([[maybe_unused]] const styling* self, const styling* parent){
        top.evaluate(parent->Margin.top.get<unsigned int>());
        bottom.evaluate(parent->Margin.bottom.get<unsigned int>());
        left.evaluate(parent->Margin.left.get<unsigned int>());
        right.evaluate(parent->Margin.right.get<unsigned int>());
    }

    // Embeds
    // -_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_

    /**
     * @brief Embeds the value of an RGB_VALUE object into a Styling object.
     * @param host The Styling object to embed the value into.
     * @param owner The Element that owns the Styling object.
     * @return A INTERNAL::STAIN_TYPE indicating the type of stain that was embedded.
     * @details This function does not actually embed any values and simply returns INTERNAL::STAIN_TYPE::CLEAN.
     */
    INTERNAL::STAIN_TYPE STYLING_INTERNAL::RGBValue::embedValue([[maybe_unused]] styling* host, [[maybe_unused]] element* owner) { return INTERNAL::STAIN_TYPE::CLEAN; }

    /**
     * @brief Embeds the value of a BOOL_VALUE object into a Styling object.
     * @param host The Styling object to embed the value into.
     * @param owner The Element that owns the Styling object.
     * @return A INTERNAL::STAIN_TYPE indicating the type of stain that was embedded.
     * @details This function does not actually embed any values and simply returns INTERNAL::STAIN_TYPE::CLEAN.
     */
    INTERNAL::STAIN_TYPE STYLING_INTERNAL::boolValue::embedValue([[maybe_unused]] styling* host, [[maybe_unused]] element* owner) { return INTERNAL::STAIN_TYPE::CLEAN; }

    /**
     * @brief Embeds the value of a NUMBER_VALUE object into a Styling object.
     * @param host The Styling object to embed the value into.
     * @param owner The Element that owns the Styling object.
     * @return A INTERNAL::STAIN_TYPE indicating the type of stain that was embedded.
     * @details This function does not actually embed any values and simply returns INTERNAL::STAIN_TYPE::CLEAN.
     */
    INTERNAL::STAIN_TYPE STYLING_INTERNAL::numberValue::embedValue([[maybe_unused]] styling* host, [[maybe_unused]] element* owner) { return INTERNAL::STAIN_TYPE::CLEAN; }

    /**
     * @brief Embeds the value of a Vector object into a Styling object.
     * @param host The Styling object to embed the value into.
     * @param owner The Element that owns the Styling object.
     * @return A INTERNAL::STAIN_TYPE indicating the type of stain that was embedded.
     * @details This function does not actually embed any values and simply returns INTERNAL::STAIN_TYPE::CLEAN.
     */
    INTERNAL::STAIN_TYPE STYLING_INTERNAL::vectorValue::embedValue([[maybe_unused]] styling* host, [[maybe_unused]] element* owner) { return INTERNAL::STAIN_TYPE::CLEAN; }

    INTERNAL::STAIN_TYPE position::embedValue(styling* host, [[maybe_unused]] element* owner){
        host->Position = *this;

        return INTERNAL::STAIN_TYPE::MOVE;
    }

    INTERNAL::STAIN_TYPE width::embedValue(styling* host, element* owner){
        host->Width = *this;

        // Since dynamic size and percentage based size are two incompatible systems.
        if (host->Width.number.getType() == INTERNAL::EVALUATION_TYPE::PERCENTAGE){
            allowDynamicSize tmp = allowDynamicSize(false);
            tmp.embedValue(host, owner);
        }

        return INTERNAL::STAIN_TYPE::STRETCH;
    }

    INTERNAL::STAIN_TYPE height::embedValue(styling* host, element* owner){
        host->Height = *this;

        // Since dynamic size and percentage based size are two incompatible systems.
        if (host->Width.number.getType() == INTERNAL::EVALUATION_TYPE::PERCENTAGE){
            allowDynamicSize tmp = allowDynamicSize(false);
            tmp.embedValue(host, owner);
        }
        
        return INTERNAL::STAIN_TYPE::STRETCH;
    }

    INTERNAL::STAIN_TYPE enableBorder::embedValue(styling* host, [[maybe_unused]] element* owner){
        owner->showBorder(this->value);

        // If border background value has not been given, then set this background color inverted.
        if (host->Border_Background_Color.status < VALUE_STATE::VALUE){
            host->Border_Background_Color.color = host->Background_Color.color;
            host->Border_Color.color.set(!host->Background_Color.color.get<RGB>());
        }

        return INTERNAL::STAIN_TYPE::EDGE;
    }

    INTERNAL::STAIN_TYPE textColor::embedValue(styling* host, [[maybe_unused]] element* owner){
        host->Text_Color = *this;

        if (host->Border_Color.status < VALUE_STATE::VALUE)
            host->Border_Color.color = this->color;

        return INTERNAL::STAIN_TYPE::COLOR;
    }

    INTERNAL::STAIN_TYPE backgroundColor::embedValue(styling* host, [[maybe_unused]] element* owner){
        host->Background_Color = *this;

        // If border background value has not been given, then set this background color as it.
        if (host->Border_Background_Color.status < VALUE_STATE::VALUE)
            host->Border_Background_Color.color = this->color;

        return INTERNAL::STAIN_TYPE::COLOR;
    }

    INTERNAL::STAIN_TYPE borderColor::embedValue(styling* host, [[maybe_unused]] element* owner){
        host->Border_Color = *this;

        return INTERNAL::STAIN_TYPE::COLOR;
    }

    INTERNAL::STAIN_TYPE borderBackgroundColor::embedValue(styling* host, [[maybe_unused]] element* owner){
        host->Border_Background_Color = *this;

        return INTERNAL::STAIN_TYPE::COLOR;
    }

    INTERNAL::STAIN_TYPE hoverBorderColor::embedValue(styling* host, [[maybe_unused]] element* owner){
        host->Hover_Border_Color = *this;

        return INTERNAL::STAIN_TYPE::COLOR;
    }

    INTERNAL::STAIN_TYPE hoverTextColor::embedValue(styling* host, [[maybe_unused]] element* owner){
        host->Hover_Text_Color = *this;

        return INTERNAL::STAIN_TYPE::COLOR;
    }

    INTERNAL::STAIN_TYPE hoverBackgroundColor::embedValue(styling* host, [[maybe_unused]] element* owner){
        host->Hover_Background_Color = *this;

        return INTERNAL::STAIN_TYPE::COLOR;
    }

    INTERNAL::STAIN_TYPE hoverBorderBackgroundColor::embedValue(styling* host, [[maybe_unused]] element* owner){
        host->Hover_Border_Background_Color = *this;

        return INTERNAL::STAIN_TYPE::COLOR;
    }

    INTERNAL::STAIN_TYPE focusBorderColor::embedValue(styling* host, [[maybe_unused]] element* owner){
        host->Focus_Border_Color = *this;

        return INTERNAL::STAIN_TYPE::COLOR;
    }

    INTERNAL::STAIN_TYPE focusTextColor::embedValue(styling* host, [[maybe_unused]] element* owner){
        host->Focus_Text_Color = *this;

        return INTERNAL::STAIN_TYPE::COLOR;
    }

    INTERNAL::STAIN_TYPE focusBackgroundColor::embedValue(styling* host, [[maybe_unused]] element* owner){
        host->Focus_Background_Color = *this;

        return INTERNAL::STAIN_TYPE::COLOR;
    }

    INTERNAL::STAIN_TYPE focusBorderBackgroundColor::embedValue(styling* host, [[maybe_unused]] element* owner){
        host->Focus_Border_Background_Color = *this;

        return INTERNAL::STAIN_TYPE::COLOR;
    }

    INTERNAL::STAIN_TYPE styledBorder::embedValue(styling* host, [[maybe_unused]] element* owner){
        host->Border_Style = *this;

        return INTERNAL::STAIN_TYPE::EDGE;
    }

    INTERNAL::STAIN_TYPE flowPriority::embedValue(styling* host, [[maybe_unused]] element* owner){
        host->Flow_Priority = *this;

        return INTERNAL::STAIN_TYPE::DEEP;
    }

    INTERNAL::STAIN_TYPE wrap::embedValue(styling* host, [[maybe_unused]] element* owner){
        host->Wrap = *this;

        return INTERNAL::STAIN_TYPE::DEEP;
    }

    INTERNAL::STAIN_TYPE allowOverflow::embedValue(styling* host, [[maybe_unused]] element* owner){
        host->Allow_Overflow = *this;

        return INTERNAL::STAIN_TYPE::CLEAN;
    }

    INTERNAL::STAIN_TYPE allowDynamicSize::embedValue(styling* host, [[maybe_unused]] element* owner){
        host->Allow_Dynamic_Size = *this;

        return INTERNAL::STAIN_TYPE::CLEAN;
    }

    INTERNAL::STAIN_TYPE margin::embedValue(styling* host, [[maybe_unused]] element* owner){
        host->Margin = *this;

        return INTERNAL::STAIN_TYPE::CLEAN;
    }

    INTERNAL::STAIN_TYPE opacity::embedValue(styling* host, [[maybe_unused]] element* owner){
        host->Opacity = *this;

        return INTERNAL::STAIN_TYPE::STRETCH;
    }

    INTERNAL::STAIN_TYPE allowScrolling::embedValue(styling* host, [[maybe_unused]] element* owner){
        host->Allow_Scrolling = *this;

        return INTERNAL::STAIN_TYPE::CLEAN;
    }

    INTERNAL::STAIN_TYPE anchor::embedValue(styling* host, [[maybe_unused]] element* owner){
        host->Align = *this;

        return INTERNAL::STAIN_TYPE::CLEAN;
    }

    INTERNAL::STAIN_TYPE node::embedValue(styling* host, [[maybe_unused]]  element* owner){
        // Since we need to put the value adding through the owner elements own custom process.
        // Since the value is typically given as an stack allocated local object, we need to transfer it into heap
        if (has(INTERNAL::getAllocationType(value), INTERNAL::ALLOCATION_TYPE::STACK))
            value = value->copy();

        host->Childs.push_back(value);

        return INTERNAL::STAIN_TYPE::DEEP;    // This also could just be a CLEAN value, since the Add_Child is determined to set the correct Stains.
    }
    
    INTERNAL::STAIN_TYPE childs::embedValue(styling* host, [[maybe_unused]]  element* owner){
        for (auto* c : *this){
            // Since the value is typically given as an stack allocated local object, we need to transfer it into heap
            if (has(INTERNAL::getAllocationType(c), INTERNAL::ALLOCATION_TYPE::STACK))
                c = c->copy();

            host->Childs.push_back(c);
        }

        return INTERNAL::STAIN_TYPE::DEEP;
    }

    INTERNAL::STAIN_TYPE onInit::embedValue([[maybe_unused]] styling* host, element* owner){
        owner->setOnInit(value);

        return INTERNAL::STAIN_TYPE::CLEAN;
    }

    INTERNAL::STAIN_TYPE onDestroy::embedValue([[maybe_unused]] styling* host, element* owner){
        owner->setOnDestroy(value);

        return INTERNAL::STAIN_TYPE::CLEAN;
    }

    INTERNAL::STAIN_TYPE onHide::embedValue([[maybe_unused]] styling* host, element* owner){
        owner->setOnHide(value);

        return INTERNAL::STAIN_TYPE::CLEAN;
    }

    INTERNAL::STAIN_TYPE onShow::embedValue([[maybe_unused]] styling* host, element* owner){
        owner->setOnShow(value);

        return INTERNAL::STAIN_TYPE::CLEAN;
    }

    INTERNAL::STAIN_TYPE onRender::embedValue([[maybe_unused]] styling* host, element* owner){
        owner->setOnRender(value);

        return INTERNAL::STAIN_TYPE::CLEAN;
    }

    INTERNAL::STAIN_TYPE name::embedValue([[maybe_unused]] styling* host, element* owner){
        // TODO: Remove this with 0.1.9, with de-standardization of std::string.
        std::string tmp = std::string(value.text, value.size);

        owner->setName(tmp);

        return INTERNAL::STAIN_TYPE::CLEAN;
    }

    INTERNAL::STAIN_TYPE title::embedValue([[maybe_unused]] styling* host, element* owner){
        owner->setTitle(value);

        if (owner->hasEmptyName()){
            // If the name is empty, then set the name to the title.
            owner->setName(INTERNAL::toString(value));
        }

        return INTERNAL::STAIN_TYPE::CLEAN;
    }

    INTERNAL::STAIN_TYPE display::embedValue([[maybe_unused]] styling* host, element* owner){
        owner->display(value);

        return INTERNAL::STAIN_TYPE::CLEAN;
    }

    INTERNAL::STAIN_TYPE onDraw::embedValue([[maybe_unused]] styling* host, element* owner){
        // first make sure that the element is an Terminal_Canvas element.
        if (dynamic_cast<canvas*>(owner))
            ((canvas*)owner)->setOnDraw(value);
        else
            throw std::runtime_error("The on_draw attribute can only be used on Terminal_Canvas type elements.");

        return INTERNAL::STAIN_TYPE::CLEAN;
    }

    INTERNAL::STAIN_TYPE text::embedValue([[maybe_unused]] styling* host, element* owner){
        // first make sure that the element is an Text_Field element.
        if (dynamic_cast<textField*>(owner))
            ((textField*)owner)->setText(value);
        else if (dynamic_cast<switchBox*>(owner))
            ((switchBox*)owner)->setText(value);
        else
            throw std::runtime_error("The text attribute can only be used on textField type elements.");

        return INTERNAL::STAIN_TYPE::CLEAN;
    }

    INTERNAL::STAIN_TYPE onClick::embedValue([[maybe_unused]] styling* host, element* owner){
        owner->onClick([this, owner](event*){
            // The default, on_click wont do anything.
            // It will call the provided lambda (if any) and return true (allowing the event to propagate).
            if (dynamic_cast<switchBox*>(owner))
                INTERNAL::DisableOthers((switchBox*)owner);

            return this->value(owner);
        });

        return INTERNAL::STAIN_TYPE::CLEAN;
    }

    INTERNAL::STAIN_TYPE onInput::embedValue([[maybe_unused]] styling* host, element* owner){
        if (dynamic_cast<textField*>(owner))
            ((textField*)owner)->input(value);
        else
            throw std::runtime_error("The on_input attribute can only be used on textField type elements.");

        return INTERNAL::STAIN_TYPE::CLEAN;
    }

    const char* styledBorder::getBorder(const INTERNAL::borderConnection flags){
        // Corners
        if (flags == (INTERNAL::borderConnection::DOWN | INTERNAL::borderConnection::RIGHT))
            return topLeftCorner;
        else if (flags == (INTERNAL::borderConnection::DOWN | INTERNAL::borderConnection::LEFT))
            return topRightCorner;
        else if (flags == (INTERNAL::borderConnection::UP | INTERNAL::borderConnection::RIGHT))
            return bottomLeftCorner;
        else if (flags == (INTERNAL::borderConnection::UP | INTERNAL::borderConnection::LEFT))
            return bottomRightCorner;
        // Vertical lines
        else if (flags == (INTERNAL::borderConnection::DOWN | INTERNAL::borderConnection::UP))
            return verticalLine;

        // Horizontal lines
        else if (flags == (INTERNAL::borderConnection::LEFT | INTERNAL::borderConnection::RIGHT))
            return horizontalLine;

        // connectors
        else if (flags == (INTERNAL::borderConnection::DOWN | INTERNAL::borderConnection::UP | INTERNAL::borderConnection::RIGHT))
            return verticalRightConnector;
        else if (flags == (INTERNAL::borderConnection::DOWN | INTERNAL::borderConnection::UP | INTERNAL::borderConnection::LEFT))
            return verticalLeftConnector;
        else if (flags == (INTERNAL::borderConnection::LEFT | INTERNAL::borderConnection::RIGHT | INTERNAL::borderConnection::DOWN))
            return horizontalBottomConnector;
        else if (flags == (INTERNAL::borderConnection::LEFT | INTERNAL::borderConnection::RIGHT | INTERNAL::borderConnection::UP))
            return horizontalTopConnector;

        // cross connectors
        else if (flags == (INTERNAL::borderConnection::LEFT | INTERNAL::borderConnection::RIGHT | INTERNAL::borderConnection::UP | INTERNAL::borderConnection::DOWN))
            return crossConnector;
        else
            return nullptr;
    }

    INTERNAL::borderConnection styledBorder::getBorderType(const char* border){
        if (border == topLeftCorner)
            return INTERNAL::borderConnection::DOWN | INTERNAL::borderConnection::RIGHT;
        else if (border == topRightCorner)
            return INTERNAL::borderConnection::DOWN | INTERNAL::borderConnection::LEFT;
        else if (border == bottomLeftCorner)
            return INTERNAL::borderConnection::UP | INTERNAL::borderConnection::RIGHT;
        else if (border == bottomRightCorner)
            return INTERNAL::borderConnection::UP | INTERNAL::borderConnection::LEFT;
        else if (border == verticalLine)
            return INTERNAL::borderConnection::DOWN | INTERNAL::borderConnection::UP;
        else if (border == horizontalLine)
            return INTERNAL::borderConnection::LEFT | INTERNAL::borderConnection::RIGHT;
        else if (border == verticalRightConnector)
            return INTERNAL::borderConnection::DOWN | INTERNAL::borderConnection::UP | INTERNAL::borderConnection::RIGHT;
        else if (border == verticalLeftConnector)
            return INTERNAL::borderConnection::DOWN | INTERNAL::borderConnection::UP | INTERNAL::borderConnection::LEFT;
        else if (border == horizontalBottomConnector)
            return INTERNAL::borderConnection::LEFT | INTERNAL::borderConnection::RIGHT | INTERNAL::borderConnection::DOWN;
        else if (border == horizontalTopConnector)
            return INTERNAL::borderConnection::LEFT | INTERNAL::borderConnection::RIGHT | INTERNAL::borderConnection::UP;
        else if (border == crossConnector)
            return INTERNAL::borderConnection::LEFT | INTERNAL::borderConnection::RIGHT | INTERNAL::borderConnection::UP | INTERNAL::borderConnection::DOWN;
        else return INTERNAL::borderConnection::NONE;
    }

    STYLING_INTERNAL::styleBase* node::copy() const {
        node* new_one = new node(*this);
        new_one->value = new_one->value->copy();
        return new_one;
    }
            
    STYLING_INTERNAL::styleBase* childs::copy() const {
        childs* new_one = new childs(*this);

        for (int i = 0; i < length(); i++){
            new_one->value[i] = this->value[i]->copy();
        }

        return new_one;
    }

    styling* styling::getReference(element* owner){
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
     * @brief evaluates dynamic attribute values for the given element.
     *
     * This function evaluates the dynamic attribute values of the styling associated
     * with the specified element. It determines the point of interest, which is
     * either the element's parent or the element itself if no parent exists,
     * and uses its style as a reference for evaluation.
     *
     * @param owner The element whose dynamic attributes are to be evaluated.
     * @return True if there wae changes in the attributes evaluated, false otherwise.
     */
    bool styling::evaluateDynamicAttributevalues(element* owner) {

        bool Changed_Attributes = false;

        // Use the retrieved style as a reference for evaluation
        styling* reference_style = getReference(owner);

        // evaluate each dynamic attribute against the reference style
        Changed_Attributes |= evaluateDynamicDimensions(owner, reference_style);
        Changed_Attributes |= evaluateDynamicPosition(owner, reference_style);
        Changed_Attributes |= evaluateDynamicBorder(owner, reference_style);
        Changed_Attributes |= evaluateDynamicColors(owner, reference_style);

        Margin.evaluate(owner->getDirectStyle(), reference_style);
        Opacity.evaluate(owner->getDirectStyle(), reference_style);
        Allow_Scrolling.evaluate(owner->getDirectStyle(), reference_style);
        Align.evaluate(owner->getDirectStyle(), reference_style);

        return Changed_Attributes;
    }

    bool styling::evaluateDynamicPosition(element* owner, styling* reference){
        if (!reference){
            reference = getReference(owner);
        }

        IVector3 previous_value = Position.get();

        Position.evaluate(owner->getDirectStyle(), reference);

        // check if position is still the same
        return previous_value != Position.get();
    }

    bool styling::evaluateDynamicDimensions(element* owner, styling* reference){
        if (!reference){
            reference = getReference(owner);
        }

        int previous_width = Width.get();
        int previous_height = Height.get();

        Width.evaluate(owner->getDirectStyle(), reference);
        Height.evaluate(owner->getDirectStyle(), reference);

        // check if width or height is still the same
        return previous_width != Width.get() || previous_height != Height.get();
    }

    bool styling::evaluateDynamicBorder(element* owner, styling* reference){
        if (!reference){
            reference = getReference(owner);
        }

        bool previous_value = Border_Enabled.value;

        Border_Enabled.evaluate(owner->getDirectStyle(), reference);

        // check if border is still the same
        return previous_value != Border_Enabled.value;
    }

    bool styling::evaluateDynamicColors(element* owner, styling* reference){
        if (!reference){
            reference = getReference(owner);
        }

        RGB previous_text_color = Text_Color.color.get<RGB>();
        RGB previous_background_color = Background_Color.color.get<RGB>();
        RGB previous_border_color = Border_Color.color.get<RGB>();
        RGB previous_border_background_color = Border_Background_Color.color.get<RGB>();
        RGB previous_hover_border_color = Hover_Border_Color.color.get<RGB>();
        RGB previous_hover_text_color = Hover_Text_Color.color.get<RGB>();
        RGB previous_hover_background_color = Hover_Background_Color.color.get<RGB>();
        RGB previous_hover_border_background_color = Hover_Border_Background_Color.color.get<RGB>();
        RGB previous_focus_border_color = Focus_Border_Color.color.get<RGB>();
        RGB previous_focus_text_color = Focus_Text_Color.color.get<RGB>();
        RGB previous_focus_background_color = Focus_Background_Color.color.get<RGB>();
        RGB previous_focus_border_background_color = Focus_Border_Background_Color.color.get<RGB>();

        Text_Color.evaluate(owner->getDirectStyle(), reference);
        Background_Color.evaluate(owner->getDirectStyle(), reference);
        Border_Color.evaluate(owner->getDirectStyle(), reference);
        Border_Background_Color.evaluate(owner->getDirectStyle(), reference);
        Hover_Border_Color.evaluate(owner->getDirectStyle(), reference);
        Hover_Text_Color.evaluate(owner->getDirectStyle(), reference);
        Hover_Background_Color.evaluate(owner->getDirectStyle(), reference);
        Hover_Border_Background_Color.evaluate(owner->getDirectStyle(), reference);
        Focus_Border_Color.evaluate(owner->getDirectStyle(), reference);
        Focus_Text_Color.evaluate(owner->getDirectStyle(), reference);
        Focus_Background_Color.evaluate(owner->getDirectStyle(), reference);
        Focus_Border_Background_Color.evaluate(owner->getDirectStyle(), reference);

        // check if any of the colors are still the same
        return previous_text_color                      != Text_Color.color.get<RGB>()                       ||
               previous_background_color                != Background_Color.color.get<RGB>()                 ||
               previous_border_color                    != Border_Color.color.get<RGB>()                     ||
               previous_border_background_color         != Border_Background_Color.color.get<RGB>()          ||
               previous_hover_border_color              != Hover_Border_Color.color.get<RGB>()               ||
               previous_hover_text_color                != Hover_Text_Color.color.get<RGB>()                 ||
               previous_hover_background_color          != Hover_Background_Color.color.get<RGB>()           ||
               previous_hover_border_background_color   != Hover_Border_Background_Color.color.get<RGB>()    ||
               previous_focus_border_color              != Focus_Border_Color.color.get<RGB>()               ||
               previous_focus_text_color                != Focus_Text_Color.color.get<RGB>()                 ||
               previous_focus_background_color          != Focus_Background_Color.color.get<RGB>()           || 
               previous_focus_border_background_color   != Focus_Border_Background_Color.color.get<RGB>();
    }

    /**
     * @brief Copies the values of the given Styling object to the current object.
     *
     * This will copy all the values of the given Styling object to the current object.
     *
     * @param other The Styling object to copy from.
     */
    void GGUI::styling::copy(const styling& other){
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
        Opacity = other.Opacity;
        Allow_Scrolling = other.Allow_Scrolling;
        Align = other.Align;

        // Add the new childs to the already existing ones
        Childs.insert(Childs.end(), other.Childs.begin(), other.Childs.end());
        
        // Copy the un_parsed_styles
        copyUnParsedStyles(&other);
    }

    void GGUI::styling::copyUnParsedStyles(){
        // deep copies the 'Other' members and uses their respective Copy virtual functions.
        STYLING_INTERNAL::styleBase* current_attribute = unParsedStyles;

        STYLING_INTERNAL::styleBase* previous_attribute = nullptr;

        while (current_attribute){
            // Shallow copy the current attribute from stack into heap.
            STYLING_INTERNAL::styleBase* anchor = current_attribute->copy();

            // Then set the current_attribute into the nested one
            if (previous_attribute)
                previous_attribute->next = anchor;
            else    // this means that this is the first occurrence, so set it as the new head
                unParsedStyles = anchor;

            // Then set the current_attribute into the nested one
            previous_attribute = anchor;

            // Then set the current_attribute into the nested one
            current_attribute = current_attribute->next;
        }
    }

    void styling::copyUnParsedStyles(const styling* other){
        STYLING_INTERNAL::styleBase* reader = other->unParsedStyles;
        STYLING_INTERNAL::styleBase* writer = unParsedStyles;

        if (reader){
            writer = reader->copy(); // Copy the first element from the other styling object
            reader = reader->next; // Move to the next element in the other styling object
        }

        // Anchor the first written as starting point
        unParsedStyles = writer;

        while (reader){
            writer->next = reader->copy(); // Copy the current element from the other styling object
            writer = writer->next; // Move to the next element in the writer styling object
            reader = reader->next; // Move to the next element in the other styling object
        }
    }

    /**
     * Embeds the styles of the current styling object into the element.
     * 
     * This function is used to embed the styles of the current styling object into the element.
     * It takes the element as a parameter and embeds the styles into it.
     * The styles are embedded by looping through the un_parsed_styles list and calling the Embed_value function on each attribute.
     * The Embed_value function is responsible for embedding the attribute into the element.
     * The changes to the element are recorded in the changes variable, which is of type STAIN.
     * The type of the changes is then added to the element's stains list.
     * The function returns nothing.
     * @param owner The element to which the styles will be embedded.
     */
    void styling::embedStyles(element* owner){
        STYLING_INTERNAL::styleBase* current_attribute = unParsedStyles;

        INTERNAL::STAIN changes;

        // This is the first pass for the INSTANT ordered style_bases:
        // Loop until no further nested attributes.
        while (current_attribute){

            if (current_attribute->order == INTERNAL::EMBED_ORDER::INSTANT)
                // First embed the current attribute
                changes.Dirty(current_attribute->embedValue(this, owner));

            // Then set the current_attribute into the nested one
            current_attribute = current_attribute->next;
        }

        // This is the second pass for the DELAYED ordered style_bases:
        current_attribute = unParsedStyles;
        
        // Loop until no further nested attributes.
        while (current_attribute){

            if (current_attribute->order == INTERNAL::EMBED_ORDER::DELAYED)
                // First embed the current attribute
                changes.Dirty(current_attribute->embedValue(this, owner));

            // Then set the current_attribute into the nested one
            current_attribute = current_attribute->next;
        }

        // evaluate itself
        bool Changes_After_Eval = evaluateDynamicAttributevalues(owner);

        if (Changes_After_Eval)
            owner->fullyStain();

        // now we need to first move all the childs first to an temporary list
        std::vector<element*> tmp_childs = Childs;

        Childs.clear();

        // For global variables to work, we need to clean each and every 'Other' member from the un_parsed_styles, so that the next user of global styles doesn't start parse unallocated stuff.
        current_attribute = unParsedStyles;

        // Wash dishes
        while (current_attribute){
            // add an anchor
            STYLING_INTERNAL::styleBase* dish = current_attribute;

            // Then set the current_attribute into the nested one
            current_attribute = current_attribute->next;

            // check if this was the last dish and so no 'Other's in it.
            if (dish)
                // now we can release the anchor
                dish->next = nullptr;
        }

        // Now we can one by one add them back via the official channel
        for (element* c : tmp_childs){
            owner->addChild(c);
        }

        owner->addStain(changes.Type);
    }

}