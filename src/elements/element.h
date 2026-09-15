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

        struct renderable {
            // Updated by element: Only fetch one parent UP, and own position +, then child repeat in Render pipeline.
            IVector3 absolutePositionCache;
    
            std::vector<terminal::cell> cellBuffer;
            std::vector<activeStyle> graphicalIdentityPool;
            std::vector<const activeStyle*> graphicalReflectionPool;
            
            // Returns list of minimum points of the deltas.
            std::vector<IVector2> getDeltaPoints();

            void compileActiveGraphics();
            
            void updateAbsolutePositionCache();
        };
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
        RGB borderGlyphColor, borderGlyphBackgroundColor;

        styledBorder borderStyle;
    public:
        // State machine for render pipeline only focus on changed aspects.
        stain::base flags = stain::types::FINALIZE;
        class element* parent = nullptr;
        
        element() = default;

        // Use copy() instead!
        element(const element&) = delete;
        element& operator=(const GGUI::element&) = delete;

        // Move is allowed
        element& operator=(element&&) = default;
        element(element&&) = default;

        virtual ~element();

        element* copy() const;

        constexpr const std::vector<converter::output::event::action>& getEventHandlers() const {
            return handlers;
        }

        void addEventhandler(const converter::output::event::action& handler);

        void processStateHandler(STATE s);

        void setOpacity(float Opacity) {
            // Only allow 0.0 - 1.0
            assert(Opacity >= 0.0f && Opacity <= 1.0f && "Opacity must be between 0.0 and 1.0");

            opacity = Opacity;

            flags |= stain::types::RESET;

            updateFrame();
        }

        float getOpacity() const {
            return opacity;
        }

        bool isTransparent() const {
            return opacity < 1.0f;
        }

        void setBorder(bool b) {
            if (b != showBorder) {
                flags |= stain::types::EDGE;
            }

            showBorder = b;
            updateFrame();
        }

        bool hasBorder() const {
            return showBorder;
        }

        // Notifies parent to refresh when display is changed.
        void setDisplay(bool d) {
            display = d;

            if (parent) {
                parent->flags |= stain::types::DEEP;
                updateFrame();
            }
        }

        bool getDisplay() const {
            return display;
        }

        void setDimensions(relativeNVector<int16_t, 2> dim) {
            if (dim != dimensions) {
                return;
            }

            dimensions = dim;

            flags |= stain::types::STRETCH;
            updateFrame();
        }

        auto getDimensions() const { return dimensions; }

        auto getWidth() const { return dimensions.x(); }
        auto getHeight() const { return dimensions.y(); }

        rectangle getAsRectangle() const { 
            return {
                {position.x().get(), position.y().get(), position.z().get()},
                {dimensions.x().get(), dimensions.y().get()}
            };
        }

        void setPosition(IVector3 c) {
            // Update the element's position in the style
            position = { c.x(), c.y(), c.z() };
            
            // Mark the element as dirty for movement updates
            this->flags |= (stain::types::MOVE);

            // Update the frame to reflect the position change
            updateFrame();
        }

        void updatePosition(IVector3 v){
            position += v;

            flags |= (stain::types::MOVE);

            updateFrame();
        }

        auto getPosition() const { return position; }

        auto getAbsolutePosition() const { return absolutePositionCache; }

        // Allocate the string somewhere else and give it as a view
        void setTitle(std::string_view t) {
            title = t;
        }

        std::string_view getTitle() const {
            return title;
        }

        void setBackgroundColor(RGB color) { backgroundColor = color; }

        RGB getBackgroundColor() const { return backgroundColor; }
        
        void setBorderColor(RGB color) { borderGlyphColor = color; }
        
        RGB getBorderColor() const { return borderGlyphBackgroundColor; }

        void setBorderBackgroundColor(RGB color) { borderGlyphBackgroundColor = color; }
        
        RGB getBorderBackgroundColor() const { return borderGlyphBackgroundColor; }
        
        void setTextColor(RGB color) { textColor = color; }

        RGB getTextColor() const { return textColor; }

        void setCustomBorderStyle(GGUI::styledBorder style);

        GGUI::styledBorder getCustomBorderStyle() const { return borderStyle; }

        virtual std::string getTypedName() const {
            return "element<" + ID + ">";
        }

        std::string_view getName() const { return ID; }

        bool hasEmptyName() { return ID.empty(); }

        void setName(const std::string& name);

        void remove();  TODO("need element::container types!")

        void onClick(std::function<bool(converter::output::event::base*)> action);

        /**
         * @brief A function that registers a lambda to be executed when the element is interacted with in any way.
         * @details The lambda is given a pointer to the Event object that triggered the call.
         *          The lambda is expected to return true if it was successful and false if it failed.
         * @param criteria The criteria to check for when deciding whether to execute the lambda.
         * @param action The lambda to be called when the element is interacted with.
         * @param GLOBAL Whether the lambda should be executed even if the element is not under the mouse.
         */
        void on(std::initializer_list<converter::input::key::types> criteria, std::function<bool(converter::output::event::base*)> job, bool GLOBAL = false);

        // Sets focus on this element
        void focus();

        // Add custom state handlers
        void onState(STATE s, void (*job)(element* self));
    protected:
    
        // Applies border glyphs into this render output
        void renderBorders(std::vector<terminal::cell>& Result);

        // Applies tittle into this render output
        void renderTitle(std::vector<terminal::cell>& Result);

        // Combines border glyphs where applicable
        void postProcessBorders(element* A, element* B, std::vector<terminal::cell>& Parent_Buffer);
        
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
