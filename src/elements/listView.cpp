#include "listView.h"
#include "../core/core.h"
#include "../core/utils/utils.h"

#include "../core/utils/logger.h"

//undefine these before algorithm.h is included

#undef RGB
#undef BOOL
#undef NUMBER

namespace GGUI {
    IVector2 listView::getDimensionLimit() const {
        // Overflow does not have bounds, and is thus unbounded.
        if (containerFlag.has(containerFlags::overflow)) {
            return {INT16_MAX, INT16_MAX};
        }

        if (containerFlag.has(containerFlags::dynamic)) {
            IVector2 Start_Address = getPosition().surjection<IVector2::dimensions>();

            IVector2 End_Address = getOuterBounds().surjection<IVector2::dimensions>();

            return End_Address - Start_Address;
        }

        return {getWidth(), getHeight()};
    }

    IVector2 listView::getOuterBounds() const {
        if (containerFlag.has(containerFlags::overflow)) {
            return {INT16_MAX, INT16_MAX};
        }

        IVector2 End_Address = {getWidth(), getHeight()};

        // We can check if the container allows some flexibility.
        if (container && container->containerFlag.has(containerFlags::dynamic)) {
            End_Address = container->getOuterBounds();
        }

        return End_Address;
    }

    bool listView::contentIsShown(element* other) const {
        rectangle self = {
            getPosition(),
            { getWidth(), getHeight() }
        };

        rectangle otherArea = {
            other->getPosition(),
            { other->getWidth(), other->getHeight() }
        };

        rectangle result = self.intersection(otherArea);

        if (result.empty()) return false;
        else return true;
    }

    element* listView::copy() const {
        // Compile time check
        //static_assert(std::is_same<T&, decltype(*this)>::value, "T must be the same as the type of the object");
        listView* new_element = static_cast<listView*>(element::copy());
        
        // copy the contents over.
        for (unsigned int i = 0; i < this->content.size(); i++){
            new_element->content[i] = this->content[i]->copy();
        }

        return new_element;
    }

    const std::vector<element*> listView::getContent() { return content; }

    std::vector<element*> listView::getVisibleContent() const {
        std::vector<element*> result;
        result.reserve(content.size());

        for (auto* c : content) {
            if (!c->showBorder || !contentIsShown(c))
                    continue;

            result.push_back(c);
        }

        // sort result by z-priority, higher z is first
        std::sort(result.begin(), result.end(), [](const element* a, const element* b) {
            return a->getPosition().z() > b->getPosition().z();
        });

        return result;
    }

    listView* listView::setDisplay(bool f) {
        pauseGGUI([this, f]() {
            // Check if the to be displayed is true and the element wasn't already displayed.
            if (f != display){
                element::setDisplay(f);
    
                for (auto* c : this->getContent()){
                    c->setDisplay(f);
                }

                flags |= stain::types::DEEP;
            }
        });

        return this;
    }

    void listView::updateInnerBounds() {
        rectangle result = { {}, {getWidth(), getHeight()} };

        TODO("Add dynamic border thickness")
        if (showBorder) {
            result.position += {1, 1};
            result.size -= {1, 1};
        }
    }

    rectangle listView::getBoundLimits() {
        rectangle result;

        updateInnerBounds();

        result = getInnerBounds();

        // Get the bound limits from the container container
        if (containerFlag.has(containerFlags::dynamic) && container) {
            result = container->getBoundLimits();
        }

        // overflown containers have no bound limits
        if (containerFlag.has(containerFlags::overflow)) {
            result = {
                {},
                {INT16_MAX, INT16_MAX}
            };
        }

        return result;
    }

    listView* listView::add(element* e) {
        pauseGGUI([this, e]() {
            // Update dynamic attributes to align with this container
            e->container = this;
            e->evaluateDynamicAttributes();

            IVector2 growthDirection = {1, 0};  // Default to horizontal growth direction

            if (containerFlag.has(containerFlags::vertical)) {
                growthDirection = {0, 1};
            }

            TODO("Add here reverse growth direction.");

            rectangle boundLimits = getBoundLimits();

            rectangle remainingSpace = {
                // Zero the non-growing direction
                { lastAddedElementInfo.x() * growthDirection.x(), lastAddedElementInfo.y() * growthDirection.y() },
                // Relativize the dimensions
                { boundLimits.size.x() - lastAddedElementInfo.x(), boundLimits.size.y() - lastAddedElementInfo.y() }
            };

            // Check if the size of the incoming content can fit inside this container
            if (e->getAsRectangle().size > remainingSpace.size) {
                // If not
                logger::log("Element " + std::string(e->getName()) + " cannot fit inside " + std::string(getName()) + " container. Skipping addition.");
                return;
            }

            // Set the contents position
            e->setPosition({
                lastAddedElementInfo.x() * growthDirection.x(),
                lastAddedElementInfo.y() * growthDirection.y(),
                0
            });

            // Set the lastAddedElementInfo to point to the end of the newly added content
            lastAddedElementInfo = {
                e->getWidth() + e->getPosition().x(),
                e->getHeight() + e->getPosition().y()
            };

            // Update the parents size opposite of growth direction
            if (containerFlag.has(containerFlags::dynamic)) {
                setDimensions({
                    std::max(getWidth(), (short)(e->getWidth() + e->getPosition().x())),
                    std::max(getHeight(), (short)(e->getHeight() + e->getPosition().y()))
                });
            }

            flags |= stain::types::DEEP;
            content.push_back(e);
        });

        return this;
    }

    bool listView::contentChanged() const {
        for (const element* e : content){
            assert(e != nullptr);
            assert(reinterpret_cast<uintptr_t>(e) % alignof(element) == 0);

            assert(e->flags.has(stain::types::FINALIZE) && "Content element passthrough Finalization stage!");

            // Not counting State machine, if element is not being drawn return always false.
            if (!e->getDisplay())
                return false;

            if (!e->flags.isEmpty())
                return true;

            if (dynamic_cast<const listView*>(e) && static_cast<const listView*>(e)->contentChanged())
                return true;
        }

        return false;
    }

    bool listView::hasTransparentContent() const {
        // Recursively check each content element for transparency.
        for (const element* e : content) {
            if (!e->getDisplay())
                continue;

            if (e->isTransparent())
                return true;

            if (dynamic_cast<const listView*>(e) && static_cast<const listView*>(e)->hasTransparentContent())
                return true;
        }

        // No transparent content found.
        return false;
    }

    void listView::computeDynamicSize() {
        // Observe the minimum required container size
        if (containerFlag.has(containerFlags::dynamic)) {
            rectangle required = {};

            // Utils:
            IVector2 growthDirection = containerFlag.has(containerFlags::vertical) ? IVector2{0, 1} : IVector2{1, 0};
            IVector2 secondaryGrowthDirection = IVector2{1, 1} - growthDirection;

            for (element* c : content) {
                if (!c->getDisplay()) continue;

                // If the content is also a dynamic container such as a listView, then we need to process that first
                if (dynamic_cast<listView*>(c)) static_cast<listView*>(c)->computeDynamicSize();

                // Compare the required space end
                if (c->getPosition() != required.position) {
                    c->setPosition(required.position);
                    required.position = c->getPosition() + c->getAsRectangle().size * growthDirection;  // The secondary direction can just stay zero
                }

                if (c->getAsRectangle().size * secondaryGrowthDirection > required.size * secondaryGrowthDirection) {
                    required.size = (
                        c->getAsRectangle().size * secondaryGrowthDirection +   // Updates only the non growing direction
                        required.size * growthDirection   // Preserves the growth direction dimension
                    );
                }
            }

            // compare the requirements to the current state, if vary; Then update
            setDimensions(required.size);
        }

        return;     // The rest is computed at content[i]::render::evaluateDynamicAttributes() to update their relative values to the proposed dimensions of this container.
    }

    std::vector<terminal::cell>& listView::render() {
        // Check for Dynamic attributes
        evaluateDynamicAttributes();

        if (contentChanged()) {
            computeDynamicSize();
        }

        //if inned children have changed without this changing, then this will trigger.
        if (!flags.has(stain::base(stain::types::STRETCH) | stain::types::RESET)){
            bool tmp = contentChanged();

            if (!tmp && flags.isEmpty()){
                return cellBuffer;
            }
            else if (tmp || hasTransparentContent()){
                flags |= (stain::types::RESET);
            }
        }

        // This is to tell the rendering thread that some or no changes were made to the rendering buffer.
        if (this == getRoot() && !flags.isEmpty()){
            thread::identicalFrame = false;
        }

        if (flags.isEmpty())
            return cellBuffer;

        if (flags.has(stain::types::MOVE)){
            flags ^= stain::types::MOVE;

            updateAbsolutePositionCache();
        }

        if (flags.has(stain::types::RESET)){
            flags ^= (stain::types::RESET);

            std::fill(cellBuffer.begin(), cellBuffer.end(), ' ');
            
            flags |= (stain::base(stain::types::GRAPHICS) | stain::types::EDGE | stain::types::DEEP);
        }

        if (flags.has(stain::types::STRETCH)){
            flags ^= (stain::types::STRETCH);
            
            cellBuffer.clear();
            cellBuffer.resize(getWidth() * getHeight(), ' ');

            flags |= (stain::base(stain::types::GRAPHICS) | stain::types::EDGE | stain::types::DEEP | stain::types::NOT_RENDERED);
        }

        if (flags.has(stain::types::NOT_RENDERED)) {
            if (onRender) onRender(this);

            // Clean regardless of On_Render existing or not.
            flags ^= (stain::types::NOT_RENDERED);
        }

        //This will add the content windows to the Result buffer
        if (flags.has(stain::types::DEEP)){
            flags ^= (stain::types::DEEP);

            // clean reflection pool
            graphicalReflectionPool.clear();
            graphicalIdentityPool.clear();
            // Resets baked graphics and reserves for content*2+2, for the incoming deep stains.
            graphicalReflectionPool.reserve(content.size() * 2 + 2); 
            flags |= (stain::types::GRAPHICS);

            for (element* c : content){
                // check if the content is within the rendering area.
                if (!c || !c->getDisplay()|| !contentIsShown(c))
                    continue;

                if (c->hasBorder())
                    flags |= stain::types::COMBINE_BORDERS;

                const std::vector<terminal::cell>& tmp = c->render();

                // compile graphical reflection pool
                graphicalReflectionPool.insert(graphicalReflectionPool.end(), c->graphicalReflectionPool.begin(), c->graphicalReflectionPool.end());

                core::nestElement(this, c, cellBuffer, tmp);
            }
        }

        if (flags.has(stain::types::GRAPHICS)) {     
            flags ^= (stain::types::GRAPHICS);

            compileActiveGraphics();    // compiles identifying graphics pools
        }

        // DEEP wont trigger this if this container does not have borders
        if (flags.has(stain::types::COMBINE_BORDERS) && hasBorder())
            flags |= (stain::types::EDGE);

        //This will add the borders if necessary and the title of the window.
        if (flags.has(stain::types::EDGE)){
            flags ^= (stain::types::EDGE);

            flags |= stain::types::COMBINE_BORDERS;

            renderBorders(cellBuffer);
            renderTitle(cellBuffer);
        }

        // This will calculate the connecting borders.
        if (flags.has(stain::types::COMBINE_BORDERS)){
            flags ^= (stain::types::COMBINE_BORDERS);

            for (auto A : content){
                for (auto B : content){
                    if (A == B)
                        continue;

                    if (!A->getDisplay() || !A->hasBorder() || !B->getDisplay() || !B->hasBorder())
                        continue;

                    postProcessBorders(A, B, cellBuffer);
                }

                postProcessBorders(this, A, cellBuffer);
            }
        }

        return cellBuffer;
    }

    void listView::postProcessBorders(element* A, element* B, std::vector<terminal::cell>& Parent_Buffer){
        // We only need to calculate the contents points in which they intersect with the container borders.
        // At these intersecting points of border we will construct a bit mask that portraits the connections the middle point has.
        // With the calculated bit mask we can fetch from the 'SYMBOLS::Border_Identifiers' the right border string.

        // First calculate if the contents borders even touch the parents borders.
        // If not, there is no need to calculate anything.

        // First calculate if the content is outside the container.
        if (
            B->getPosition().x() + B->getWidth() < A->getPosition().x() ||
            B->getPosition().x() > A->getPosition().x() + A->getWidth() ||
            B->getPosition().y() + B->getHeight() < A->getPosition().y() ||
            B->getPosition().y() > A->getPosition().y() + A->getHeight()
        )
            return;

        // Now calculate if the content is inside the container.
        if (
            B->getPosition().x() > A->getPosition().x() &&
            B->getPosition().x() + B->getWidth() < A->getPosition().x() + A->getWidth() &&
            B->getPosition().y() > A->getPosition().y() &&
            B->getPosition().y() + B->getHeight() < A->getPosition().y() + A->getHeight()
        )
            return;

        // Now that we are here it means the both boxes interlace each other.
        // We will calculate the hitting points by drawing segments from corner to corner and then comparing one segments x to other segments y, and so forth.

        // two nested loops rotating the x and y usages.
        // store the line x,y into a array for the nested loops to access.
        std::vector<int> Vertical_Line_X_Coordinates = {
            
            B->getPosition().x(),
            A->getPosition().x(),
            B->getPosition().x() + B->getWidth() - 1,
            A->getPosition().x() + A->getWidth() - 1,

                    
            // A->getPosition().X,
            // B->getPosition().X,
            // A->getPosition().X + A->Width - 1,
            // B->getPosition().X + B->Width - 1

        };

        std::vector<int> Horizontal_Line_Y_Coordinates = {
            
            A->getPosition().y(),
            B->getPosition().y() + B->getHeight() - 1,
            A->getPosition().y(),
            B->getPosition().y() + B->getHeight() - 1,

            // B->Position.Y,
            // A->Position.Y + A->Height - 1,
            // B->Position.Y,
            // A->Position.Y + A->Height - 1,

        };

        std::vector<IVector3> Crossing_Indicies;

        // Go through singular box
        for (size_t Box_Index = 0; Box_Index < Horizontal_Line_Y_Coordinates.size(); Box_Index++){
            // Now just pair the indicies from the two lists.
            Crossing_Indicies.push_back(
                // First pair
                IVector3(
                    Vertical_Line_X_Coordinates[Box_Index],
                    Horizontal_Line_Y_Coordinates[Box_Index]
                )
            );
        }

        // Now that we have the crossing points we can start analyzing the ways they connect to construct the bit masks.
        for (auto c : Crossing_Indicies){

            IVector3 Above = { c.x(), c.y() - 1 };
            IVector3 Below = { c.x(), c.y() + 1 };
            IVector3 Left = { c.x() - 1, c.y() };
            IVector3 Right = { c.x() + 1, c.y() };

            bitMask<styledBorder::connectionTypes> Current_Masks = styledBorder::connectionTypes::NONE;

            auto Is_In_Bounds = [](IVector3 index, listView* container) {
                // checks if the index is out of bounds
                if (index.x() < 0 || index.y() < 0 || index.x() >= container->getWidth() || index.y() >= container->getHeight())
                    return false;

                return true;
            };

            auto From = [](IVector3 index, std::vector<terminal::cell>& Parent_Buffer, listView* Container) {
                return &Parent_Buffer[index.y() * Container->getWidth() + index.x()];
            };

            // These selected coordinates can only contain something related to the borders and if the current UTF is unicode then it is an border.
            if (Is_In_Bounds(Above, this)){
                // Since the border above can be already processed we need to check all possible border variations.
                bitMask<styledBorder::connectionTypes> borderAbove = A->getCustomBorderStyle().getBorderType(From(Above, Parent_Buffer, this)->getGlyphs()) | 
                                                         B->getCustomBorderStyle().getBorderType(From(Above, Parent_Buffer, this)->getGlyphs());
                
                if (borderAbove != styledBorder::connectionTypes::NONE)
                    Current_Masks |= styledBorder::connectionTypes::UP;
            }

            if (Is_In_Bounds(Below, this)){
                // Since the border below can be already processed we need to check all possible border variations.
                bitMask<styledBorder::connectionTypes> borderBelow = A->getCustomBorderStyle().getBorderType(From(Below, Parent_Buffer, this)->getGlyphs()) | 
                                                         B->getCustomBorderStyle().getBorderType(From(Below, Parent_Buffer, this)->getGlyphs());
                
                if (borderBelow != styledBorder::connectionTypes::NONE)
                    Current_Masks |= styledBorder::connectionTypes::DOWN;
            }

            if (Is_In_Bounds(Left, this)){
                // Since the border left can be already processed we need to check all possible border variations.
                bitMask<styledBorder::connectionTypes> borderLeft = A->getCustomBorderStyle().getBorderType(From(Left, Parent_Buffer, this)->getGlyphs()) | 
                                                        B->getCustomBorderStyle().getBorderType(From(Left, Parent_Buffer, this)->getGlyphs());
                
                if (borderLeft != styledBorder::connectionTypes::NONE)
                    Current_Masks |= styledBorder::connectionTypes::LEFT;
            }

            if (Is_In_Bounds(Right, this)){
                // Since the border right can be already processed we need to check all possible border variations.
                bitMask<styledBorder::connectionTypes> borderRight = A->getCustomBorderStyle().getBorderType(From(Right, Parent_Buffer, this)->getGlyphs()) | 
                                                         B->getCustomBorderStyle().getBorderType(From(Right, Parent_Buffer, this)->getGlyphs());
                
                if (borderRight != styledBorder::connectionTypes::NONE)
                    Current_Masks |= styledBorder::connectionTypes::RIGHT;
            }

            std::string_view finalBorder = A->borderStyle.getBorder(Current_Masks);

            if (finalBorder.empty()){
                continue;
            }

            *From(c, Parent_Buffer, this) = finalBorder;
        }
    }

    std::string listView::getTypedName() const {
        return "listView<" + ID + ">";
    }

    bool listView::detach(element* detachable) {
        bool result;
        pauseGGUI([this, detachable, &result]() {
            unsigned int Index = 0;

            //first find the detachable element index.
            for (;Index < content.size() && content[Index] != detachable; Index++);
            
            // Check if there was no element by that ptr value.
            if (Index == content.size()){
                logger::log("Internal: no element with ptr value: " + detachable->getTypedName() + " was found in the list view: " + getTypedName());
                
                // Removal action failed.
                result = false;
                return;
            }

            content.erase(content.begin() + Index);

            result = true;
        });

        return result;
    }

    bool listView::remove(element* removable) {
        bool result = detach(removable);

        // Only remove this element if it truly existed in the list view
        if (result) delete removable;

        return result;
    }

    TODO("This does not account for scroll index currently!")
    std::vector<terminal::cell>& scrollView::render() {
        // Check for Dynamic attributes
        evaluateDynamicAttributes();

        if (contentChanged()) {
            computeDynamicSize();
        }

        //if inned children have changed without this changing, then this will trigger.
        if (!flags.has(stain::base(stain::types::STRETCH) | stain::types::RESET)){
            bool tmp = contentChanged();

            if (!tmp && flags.isEmpty()){
                return cellBuffer;
            }
            else if (tmp || hasTransparentContent()){
                flags |= (stain::types::RESET);
            }
        }

        // This is to tell the rendering thread that some or no changes were made to the rendering buffer.
        if (this == getRoot() && !flags.isEmpty()){
            thread::identicalFrame = false;
        }

        if (flags.isEmpty())
            return cellBuffer;

        if (flags.has(stain::types::MOVE)){
            flags ^= stain::types::MOVE;

            updateAbsolutePositionCache();
        }

        if (flags.has(stain::types::RESET)){
            flags ^= (stain::types::RESET);

            std::fill(cellBuffer.begin(), cellBuffer.end(), ' ');
            
            flags |= (stain::base(stain::types::GRAPHICS) | stain::types::EDGE | stain::types::DEEP);
        }

        if (flags.has(stain::types::STRETCH)){
            flags ^= (stain::types::STRETCH);
            
            cellBuffer.clear();
            cellBuffer.resize(getWidth() * getHeight(), ' ');

            flags |= (stain::base(stain::types::GRAPHICS) | stain::types::EDGE | stain::types::DEEP | stain::types::NOT_RENDERED);
        }

        if (flags.has(stain::types::NOT_RENDERED)) {
            if (onRender) onRender(this);

            // Clean regardless of On_Render existing or not.
            flags ^= (stain::types::NOT_RENDERED);
        }

        //This will add the content windows to the Result buffer
        if (flags.has(stain::types::DEEP)){
            flags ^= (stain::types::DEEP);

            // clean reflection pool
            graphicalReflectionPool.clear();
            graphicalIdentityPool.clear();
            // Resets baked graphics and reserves for content*2+2, for the incoming deep stains.
            graphicalReflectionPool.reserve(content.size() * 2 + 2); 
            flags |= (stain::types::GRAPHICS);

            for (element* c : content){
                // check if the content is within the rendering area.
                if (!c || !c->getDisplay()|| !contentIsShown(c))
                    continue;

                if (c->hasBorder())
                    flags |= stain::types::COMBINE_BORDERS;

                const std::vector<terminal::cell>& tmp = c->render();

                // compile graphical reflection pool
                graphicalReflectionPool.insert(graphicalReflectionPool.end(), c->graphicalReflectionPool.begin(), c->graphicalReflectionPool.end());

                core::nestElement(this, c, cellBuffer, tmp);
            }
        }

        if (flags.has(stain::types::GRAPHICS)) {     
            flags ^= (stain::types::GRAPHICS);

            compileActiveGraphics();    // compiles identifying graphics pools
        }

        // DEEP wont trigger this if this container does not have borders
        if (flags.has(stain::types::COMBINE_BORDERS) && hasBorder())
            flags |= (stain::types::EDGE);

        //This will add the borders if necessary and the title of the window.
        if (flags.has(stain::types::EDGE)){
            flags ^= (stain::types::EDGE);

            flags |= stain::types::COMBINE_BORDERS;

            renderBorders(cellBuffer);
            renderTitle(cellBuffer);
        }

        // This will calculate the connecting borders.
        if (flags.has(stain::types::COMBINE_BORDERS)){
            flags ^= (stain::types::COMBINE_BORDERS);

            for (auto A : content){
                for (auto B : content){
                    if (A == B)
                        continue;

                    if (!A->getDisplay() || !A->hasBorder() || !B->getDisplay() || !B->hasBorder())
                        continue;

                    postProcessBorders(A, B, cellBuffer);
                }

                postProcessBorders(this, A, cellBuffer);
            }
        }

        return cellBuffer;
    }

    scrollView* scrollView::setScrolling(bool allow) {
        bool scrollingEventsExists = false;
        size_t scrollUpEventHandlerIndex = 0;
        size_t scrollDownEventHandlerIndex = 0;

        const std::vector<converter::output::event::action>& localEventHandlers = getEventHandlers();

        // Check if scrolling events already exist for this ScrollView
        for (unsigned int i = 0; i < localEventHandlers.size(); i++) {
            if (localEventHandlers[i].has(converter::input::key::types::SCROLL_UP)) {
                scrollUpEventHandlerIndex = i;
                scrollingEventsExists = true;
            }
            else if (localEventHandlers[i].has(converter::input::key::types::SCROLL_DOWN)) {
                scrollDownEventHandlerIndex = i;
                scrollingEventsExists = true;
            }
        }

        // If scrolling events dont exist but we allow to, then create them:
        if (!scrollingEventsExists == allow) {
            this->on({converter::input::key::types::SCROLL_UP}, [this](converter::output::event::base*) {
                this->scrollUp();
                return true;
            });

            this->on({converter::input::key::types::SCROLL_DOWN}, [this](converter::output::event::base*) {
                this->scrollDown();
                return true;
            });
        } else if (scrollingEventsExists == !allow) {   // If scrolling events exist, but we dont allow, then:
            removeEventHandler(std::max(scrollUpEventHandlerIndex, scrollDownEventHandlerIndex));   // remove the later one so the prior ones index does not get shifted
            removeEventHandler(std::min(scrollUpEventHandlerIndex, scrollDownEventHandlerIndex));   // remove the prior one
        }

        return this;
    }

    std::string scrollView::getTypedName() const {
        return "scrollView<" + ID + ">";
    }

}
