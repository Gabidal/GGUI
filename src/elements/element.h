#ifndef _ELEMENT_H_
#define _ELEMENT_H_

#include <string>
#include <cstring>
#include <vector>
#include <functional>

#include "../core/utils/color.h"
#include "../core/utils/types.h"
#include "../core/backend/terminal.h"

#include "../core/converter.h"

namespace GGUI{
    namespace thread {
        extern void renderer();
    }

    extern void updateFrame();

    namespace terminal {
        class outputCapture;
    }

    class element : public terminal::renderable {
    protected:
        relativeNVector<int16_t, 2> dimensions = {1, 1};
        relativeNVector<int16_t, 3> position = {0, 0, 0};

        std::vector<converter::output::event::action> handlers;
        
        // Determines if the element is rendered or not.
        bool display = true;
        bool showBorder = false;
        
        // Human readable ID.
        std::string ID = "";
        std::string_view title = "";
        
        void (*onInit)(element*) = nullptr;
        void (*onDestroy)(element*) = nullptr;
        void (*onHide)(element*) = nullptr;
        void (*onShow)(element*) = nullptr;
        void (*onRender)(element*) = nullptr;
        
        float opacity = 1.0f;

        RGB textColor, backgroundColor;
        RGB borderGlyphColor, borderBackgroundColor;

        styledBorder borderStyle;
    public:
        // State machine for render pipeline only focus on changed aspects.
        stain::base flags = stain::types::FINALIZE;
        class listView* container = nullptr;
        
        element() = default;

        // Use copy() instead!
        element(const element&) = delete;
        element& operator=(const GGUI::element&) = delete;

        // Move is allowed
        element& operator=(element&&) = default;
        element(element&&) = default;

        virtual ~element();

        virtual element* copy() const;

        constexpr const std::vector<converter::output::event::action>& getEventHandlers() const {
            return handlers;
        }

        void addEventhandler(const converter::output::event::action& handler);

        void removeEventHandler(size_t i);

        void processStateHandler(STATE s);

        element* setOpacity(float Opacity) {
            // Only allow 0.0 - 1.0
            assert(Opacity >= 0.0f && Opacity <= 1.0f && "Opacity must be between 0.0 and 1.0");

            opacity = Opacity;

            flags |= stain::types::RESET;

            updateFrame();

            return this;
        }

        float getOpacity() const {
            return opacity;
        }

        bool isTransparent() const {
            return opacity < 1.0f;
        }

        element* setBorder(bool b) {
            if (b != showBorder) {
                flags |= stain::types::EDGE;
            }

            showBorder = b;
            updateFrame();

            return this;
        }

        bool hasBorder() const {
            return showBorder;
        }

        // Notifies container to refresh when display is changed.
        element* setDisplay(bool d);

        bool getDisplay() const {
            return display;
        }

        element* setDimensions(relativeNVector<int16_t, 2> dim) {
            if (dim != dimensions) {
                return this;
            }

            dimensions = dim;

            flags |= stain::types::STRETCH;
            updateFrame();

            return this;
        }

        auto getDimensions() const { return dimensions; }

        auto getWidth() const { return dimensions.x().get(); }
        auto getHeight() const { return dimensions.y().get(); }

        rectangle getAsRectangle() const { 
            return {
                {position.x().get(), position.y().get(), position.z().get()},
                {dimensions.x().get(), dimensions.y().get()}
            };
        }

        element* setPosition(IVector3 c) {
            if (c == getPosition()) return this;

            // Update the element's position in the style
            position = { c.x(), c.y(), c.z() };
            
            // Mark the element as dirty for movement updates
            this->flags |= (stain::types::MOVE);

            // Update the frame to reflect the position change
            updateFrame();

            return this;
        }

        void updatePosition(IVector3 v){
            position += v;

            flags |= (stain::types::MOVE);

            updateFrame();
        }

        IVector3 getPosition() const { return IVector3{position.x().get(), position.y().get(), position.z().get()}; }

        auto getAbsolutePosition() const { return absolutePositionCache; }

        // Allocate the string somewhere else and give it as a view
        element* setTitle(std::string_view t) {
            title = t;

            return this;
        }

        std::string_view getTitle() const {
            return title;
        }

        element* setBackgroundColor(RGB color) { backgroundColor = color; return this; }

        RGB getBackgroundColor() const { return backgroundColor; }
        
        element* setBorderColor(RGB color) { borderGlyphColor = color; return this; }
        
        RGB getBorderGlyphColor() const { return borderBackgroundColor; }

        element* setBorderBackgroundColor(RGB color) { borderBackgroundColor = color; return this; }
        
        RGB getBorderBackgroundColor() const { return borderBackgroundColor; }
        
        element* setTextColor(RGB color) { textColor = color; return this; }

        RGB getTextColor() const { return textColor; }

        element* setCustomBorderStyle(GGUI::styledBorder style);

        GGUI::styledBorder getCustomBorderStyle() const { return borderStyle; }

        virtual std::string getTypedName() const {
            return "element<" + ID + ">";
        }

        std::string_view getName() const { return ID; }

        bool hasEmptyName() const { return ID.empty(); }

        element* setName(const std::string& name);

        void remove();

        element* onClick(std::function<bool(converter::output::event::base*)> action);

        /**
         * @brief A function that registers a lambda to be executed when the element is interacted with in any way.
         * @details The lambda is given a pointer to the Event object that triggered the call.
         *          The lambda is expected to return true if it was successful and false if it failed.
         * @param criteria The criteria to check for when deciding whether to execute the lambda.
         * @param action The lambda to be called when the element is interacted with.
         * @param GLOBAL Whether the lambda should be executed even if the element is not under the mouse.
         */
        element* on(std::initializer_list<converter::input::key::types> criteria, std::function<bool(converter::output::event::base*)> job, bool GLOBAL = false);

        // Sets focus on this element
        void focus();

        element* setFocusState(bool f);

        element* setHoverState(bool h); 

        // Add custom state handlers
        void onState(STATE s, void (*job)(element* self));
    protected:
    
        // Applies border glyphs into this render output
        void renderBorders(std::vector<terminal::cell>& Result);

        // Applies tittle into this render output
        void renderTitle(std::vector<terminal::cell>& Result);
        
        constexpr void fullyStain(){
            // Mark the element as dirty for all possible stain types to ensure
            // complete re-evaluation and rendering.
            flags |= (
                stain::base(stain::types::STRETCH) | 
                stain::types::GRAPHICS | stain::types::DEEP | 
                stain::types::EDGE | stain::types::MOVE
                // stain::types::FINALIZE // <- only constructors have the right to set this flag!
                | stain::types::NOT_RENDERED
            );
        }

        void evaluateDynamicAttributes();
        
        /**
         * @brief Creates a new instance of the element class.
         * 
         * This virtual function is intended to be overridden by derived classes
         * to provide a mechanism for creating instances of the respective class.
         * By default, it creates and returns a new instance of the base `element` class.
         * 
         * @return A pointer to a newly created instance of the `element` class.
         */
        virtual element* createInstance() const {
            return new element();
        }
        
        // The main rendering pipeline for each element and its derived classes
        virtual std::vector<terminal::cell>& render();

        // Give thread::renderer() access to our private render method.
        friend void thread::renderer();

        // Since protected methods can be accessed via the derived class only if it is as "this" pointer, so we need to give it access.
        friend class listView;
        friend class scrollView;

        friend class terminal::outputCapture;
    
    // Some of the above mentioned functions be of help
    public:
        
        /**
         * @brief Provides access to embedStyles().
         *
         * This is a temporary workaround for managing RTTI during construction time.
         * The base class element performs style embedding if forced, which means the
         * virtual table (VT) has not yet been set to the derived class that is calling
         * the base class constructor.
         */
        void compile() {  }
        
    };

    namespace utils {
        constexpr bool collides(const element* a, IVector3 b) {
            if (!a) return false;   // Safe guard
            return a->getAsRectangle().hits(b.surjection<2>());
        }
    }
}

#endif
