#include "element.h"
#include "listView.h"

#include "../core/core.h"
#include "../core/utils/utils.h"
#include "../core/utils/logger.h"

#include <algorithm>
#include <vector>

#undef min
#undef max

namespace GGUI {
    element::~element(){
        // Call handler for on destroying moment.
        processStateHandler(STATE::DESTROYED);

        // Make sure this element is not listed in the container element.
        if (container) {
            container->remove(this);
        }

        //now also update the event handlers.
        for (size_t i = 0; i < core::inputConverter->handlers.size(); i++) {
            if (core::inputConverter->handlers[i] == this) {
                core::inputConverter->handlers.erase(core::inputConverter->handlers.begin() + i);
                // don't increment i, since elements shifted left

                break;
            }
        }

        // Now make sure that if the Focused_On element points to this element, then set it to nullptr
        if (core::focusedOn == this)
            core::focusedOn.unlink();

        // Now make sure that if the Hovered_On element points to this element, then set it to nullptr
        if (core::hoveredOn == this)
            core::hoveredOn.unlink();
    }   

    void element::evaluateDynamicAttributes() {
        if (container) {        // Dynamic require container container to exist
            IVector2 previousDimensions = { getWidth(), getHeight() };
            IVector3 previousPosition = getPosition();

            dimensions.x().evaluate(container->dimensions.x().get());
            dimensions.y().evaluate(container->dimensions.y().get());

            // position = container->dimensions - this->dimensions to get the actual usable relative positions otherwise the positions could clip outside 
            position.x().evaluate(container->dimensions.x().get() - dimensions.x().get());
            position.y().evaluate(container->dimensions.y().get() - dimensions.y().get());
    
            if (previousDimensions != IVector2{ getWidth(), getHeight() })
                flags |= stain::types::STRETCH;

            if (previousPosition != getPosition())
                flags |= stain::types::MOVE;
        }

        // For delayed initialization this is here instead inside the constructor
        if (flags.has(stain::types::FINALIZE)) {
            flags ^= stain::types::FINALIZE;

            // Regardless of present container state handlers should be called
            processStateHandler(STATE::INIT);
        }
    }

    std::vector<terminal::cell>& element::render() {
        // Check for Dynamic attributes
        evaluateDynamicAttributes();

        // This is to tell the rendering thread that some or no changes were made to the rendering buffer.
        if (this == getRoot() && !flags.isEmpty()){
            thread::identicalFrame = false;
        }
        
        // NULLOP
        flags ^= stain::types::DEEP;
        flags ^= stain::types::COMBINE_BORDERS;

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


        if (flags.has(stain::types::GRAPHICS)) {     // Resets baked graphics
            flags ^= (stain::types::GRAPHICS);

            // clean reflection pool
            graphicalReflectionPool.clear();
            graphicalIdentityPool.clear();

            compileActiveGraphics();    // compiles identifying graphics pools
        }

        //This will add the borders if necessary and the title of the window.
        if (flags.has(stain::types::EDGE)){
            flags ^= (stain::types::EDGE);

            renderBorders(cellBuffer);
            renderTitle(cellBuffer);
        }

        return cellBuffer;
    }

    void element::setFocusState(bool f) {
        if (f == (core::focusedOn == this)){
            // If the focus state has changed, dirty the element and update the frame.
            flags |= (stain::base(stain::types::GRAPHICS) | stain::types::EDGE);

            core::focusedOn.link(this);

            updateFrame();
        }
    }

    void element::setHoverState(bool h) {
        if (h == (core::hoveredOn == this)){
            // If the hover state has changed, dirty the element and update the frame.
            flags |= (stain::base(stain::types::GRAPHICS) | stain::types::EDGE);

            core::hoveredOn.link(this);

            updateFrame();
        }
    }

    void element::processStateHandler(STATE s){
        if      (s == STATE::INIT       && onInit)      onInit(this);
        else if (s == STATE::DESTROYED  && onDestroy)   onDestroy(this);
        else if (s == STATE::HIDDEN     && onHide)      onHide(this);
        else if (s == STATE::SHOWN      && onShow)      onShow(this);
    }

    void element::setDisplay(bool f) {
        pauseGGUI([this, f]() {
            // Check if the to be displayed is true and the element wasn't already displayed.
            if (f != display){
                display = f;
                
                if (f){
                    processStateHandler(STATE::SHOWN);
                }
                else{
                    processStateHandler(STATE::HIDDEN);
                    
                    // Ask the container to flush its buffer from this.
                    if (container){
                        container->flags |= (stain::types::RESET);
                    }
                }
            }
        });
    }

    void element::remove() {
        if (container){
            // Tell the container what is about to happen.
            // You need to update the container before removing the content, otherwise the code cannot erase it when it is not found!
            container->remove(this);
        }
        else{
            logger::log("Cannot remove " + getTypedName() + ", with no container\n");
        }
    }

    element* element::copy() const {
        // Compile time check
        //static_assert(std::is_same<T&, decltype(*this)>::value, "T must be the same as the type of the object");
        element* new_element = createInstance();

        // Make sure the name is also renewed to represent the memory.
        if (!hasEmptyName())
            new_element->setName(std::string(getName()) + "_copy");
        else 
            new_element->ID = "";

        // reset the container info.
        new_element->container = nullptr;

        // now also update the event handlers.
        // NOTE: We don't have enough power to update the lambda captures of the this ptr value, so please use the self->host ptr instead!
        for (auto& e : this->handlers){
            //add the new action to the event handlers list
            new_element->handlers.push_back(e);
        }

        if (!new_element->handlers.empty()) {
            core::inputConverter->handlers.push_back(new_element);
        }

        // Clear the Focused on bool
        if (core::focusedOn == this) {
            core::focusedOn.unlink();
        }

        // Clear the Hovered on bool
        if (core::hoveredOn == this) {
            core::hoveredOn.unlink();
        }

        return new_element;
    }

    void element::addEventhandler(const converter::output::event::action& handler) {
        handlers.push_back(handler);

        // Check if this element has been added to the INTERNAL::eventHandlers, if not, then append this into it.
        bool found = false;
        for (const auto& h : core::inputConverter->handlers){
            if (h == this){
                found = true;
                break;
            }
        }

        if (!found) core::inputConverter->handlers.push_back(this);
    }

    void element::removeEventHandler(size_t i) {
        if (i >= handlers.size()) {
            logger::log("Error: Attempted to remove an event handler at index " + std::to_string(i) + ", but the size of the handlers vector is " + std::to_string(handlers.size()));
            return;
        }

        handlers.erase(handlers.begin() + i);

        // If there are no more event handlers, remove this element from the INTERNAL::eventHandlers
        if (handlers.empty()) {
            auto& internalHandlers = core::inputConverter->handlers;
            internalHandlers.erase(std::remove(internalHandlers.begin(), internalHandlers.end(), this), internalHandlers.end());
        }
    }

    void element::renderBorders(std::vector<terminal::cell>& Result){
        if (!hasBorder()) return;

        unsigned int Width  = getWidth();
        unsigned int Height = getHeight();
        const auto& Border  = borderStyle;

        // Corners
        Result[0] = Border.data[0];
        Result[(Height - 1) * Width] = Border.data[1];
        Result[Width - 1] = Border.data[2];
        Result[(Height * Width) - 1] = Border.data[3];

        // Top and Bottom horizontal borders
        for (unsigned int x = 1; x < Width - 1; ++x) {
            Result[x] = Border.data[4];                          // Top row
            Result[(Height - 1) * Width + x] = Border.data[5];   // Bottom row
        }

        // Left and Right vertical borders
        for (unsigned int y = 1; y < Height - 1; ++y) {
            Result[y * Width] = Border.data[4];                 // Left column
            Result[y * Width + (Width - 1)] = Border.data[5];   // Right column
        }
    }

    void element::renderTitle(std::vector<terminal::cell>& Result){
        if (title.empty())
            return;

        size_t Title_Length = title.size(); // +1 for trailing, since Compact_Strings do not include trailing characters in their size.
        size_t Horizontal_Offset = (int)hasBorder();
        static constexpr std::string Ellipsis = "...";
        bool Enable_Ellipsis = false;

        size_t Writable_Length = std::min(Title_Length, (size_t)(getWidth() - Horizontal_Offset - (int)Ellipsis.size() - 1));

        if (Writable_Length < Title_Length)
            Enable_Ellipsis = true;

        // Now we'll write what we can
        for (size_t x = Horizontal_Offset; x < Writable_Length + Horizontal_Offset; x++){
            Result[x] = title[x - Horizontal_Offset];
        }

        // And then we'll add the ellipsis
        if (Enable_Ellipsis){
            size_t Ellipsis_Offset = Writable_Length + Horizontal_Offset;
            for (size_t x = 0; x < Ellipsis.size(); x++){
                if (Ellipsis_Offset + x < (size_t)(getWidth() - Horizontal_Offset)){
                    Result[Ellipsis_Offset + x] = Ellipsis[x];
                }
            }
        }
    }

    void element::setCustomBorderStyle(styledBorder Style) {
        // Set the border style of the element
        borderStyle = Style;
        
        // Mark the border as needing an update
        flags |= (stain::types::EDGE);

        // Ensure the border is visible
        setBorder(true);
    }

    void element::onClick(std::function<bool(converter::output::event::base*)> job){
        auto wrapper = [this, job](converter::output::event::base* e){
            // As os 0.1.8 no need to check for mouse collision with current element, since mouse collision is already checked at the eventHandler scheduler.

            // Construct an Action from the Event obj
            auto* event2actionWrapper = new converter::output::event::action(e->criteria, job, getTypedName() + "::onClick");

            //action successfully executed.
            return job(event2actionWrapper);
        };
        
        auto mouse = converter::output::event::action(
            {converter::input::key::types::LEFT_CLICK},
            wrapper,
            getTypedName() + "::onClick::wrapper::mouse"
        );

        auto enter = converter::output::event::action(
            {converter::input::key::types::ENTER},
            wrapper,
            getTypedName() + "::onClick::wrapper::enter"
        );

        addEventhandler(mouse);
        addEventhandler(enter);
    }

    void element::on(std::initializer_list<converter::input::key::types> criteria, std::function<bool(converter::output::event::base*)> job, bool GLOBAL){
        addEventhandler(converter::output::event::action(
            criteria,
            [this, job, GLOBAL](converter::output::event::base* e){
                if (core::focusedOn == this || GLOBAL){
                    // action successfully executed.
                    return job(e);
                }
                // action failed.
                return false;
            },
            getTypedName() + "::on::"
        ));
    }

    void element::setName(const std::string& Name){
        // Set the name of the element.
        ID = Name;

        // Store the element in the global Element_Names map.
        core::elementNames[ID] = this;
    }

    void element::focus() {
        // Set the mouse position to the element's position.
        currentMouse.position = absolutePositionCache.surjection<IVector2::dimensions>();
        // Update the focused element.
        core::focusedOn.link(this);
    }

    /**
    * @brief Adds a handler function to the state handlers map.
    * @details This function takes a state and a handler function as arguments.
    *          The handler function is stored in the State_Handlers map with the given state as the key.
    * @param s The state for which the handler should be executed.
    * @param job The handler function to be executed
    */
    void element::onState(STATE s, void (*job)(element* self)){
        if (s == STATE::INIT)
            onInit = job;
        else if (s == STATE::DESTROYED)
            onDestroy = job;
        else if (s == STATE::HIDDEN)
            onHide = job;
        else if (s == STATE::SHOWN)
            onShow = job;
    }

    bool Is_Signed(int x){
        return x < 0;
    }

    int Get_Sign(int x){
        return Is_Signed(x) ? -1 : 1;
    }

    // Constructs two squares one 2 steps larger on width and height, and given the different indicies.
    std::vector<IVector3> Get_Surrounding_Indicies(int Width, int Height, IVector3 start_offset, FVector2 Offset){

        std::vector<IVector3> Result;

        // First construct the first square.
        int Bigger_Square_Start_X = start_offset.x() - 1;
        int Bigger_Square_Start_Y = start_offset.y() - 1;

        int Bigger_Square_End_X = start_offset.x() + Width + 1;
        int Bigger_Square_End_Y = start_offset.y() + Height + 1;

        int Smaller_Square_Start_X = start_offset.x() + (Offset.x() * std::min(0, (int)Offset.x()));
        int Smaller_Square_Start_Y = start_offset.y() + (Offset.y() * std::min(0, (int)Offset.y()));

        int Smaller_Square_End_X = start_offset.x() + Width - (Offset.x() * std::max(0, (int)Offset.x()));
        int Smaller_Square_End_Y = start_offset.y() + Height - (Offset.y() * std::max(0, (int)Offset.y()));

        for (int y = Bigger_Square_Start_Y; y < Bigger_Square_End_Y; y++){
            for (int x = Bigger_Square_Start_X; x < Bigger_Square_End_X; x++){

                bool Is_Inside_Smaller_Square = x >= Smaller_Square_Start_X && x < Smaller_Square_End_X && y >= Smaller_Square_Start_Y && y < Smaller_Square_End_Y;

                // Check if the current coordinates are outside the smaller square.
                if (!Is_Inside_Smaller_Square)
                    Result.push_back({ x, y });
            }
        }

        return Result;

    }
}