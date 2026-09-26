#ifndef _LIST_VIEW_H_
#define _LIST_VIEW_H_

#include "element.h"

namespace GGUI{
    class listView : public element {
        enum class containerFlags : uint8_t {
            __min       = 0,
            DEFAULT     = __min,
            
            dynamic     = 1,
            overflow    = 2,
            vertical    = 3,        // when off, horizontal by default
        
            __max
        };

        IVector2 lastAddedElementInfo = {};
        std::vector<element*> content;

        rectangle innerBounds;  // Cached inner bounds from border offset and margins
    public:
        linearMask<containerFlags> containerFlag;

        template<typename... elements>
        listView(elements*... e) : element() {
            (add(e), ...);
        }

        listView* add(element* e);

        template<typename T>
        listView* add(T& e) {
            add(e.copy());

            return this;
        }
        
        std::string getTypedName() const override;

        bool remove(element* e);

        bool remove(size_t i) {
            if (i >= content.size())
                return false;

            return remove(content[i]);
        }

        bool detach(element* e);

        bool isVertical() const {
            return containerFlag.has(containerFlags::vertical);
        }

        bool isHorizontal() const {
            return !containerFlag.has(containerFlags::vertical);
        }

        listView* setHorizontal() {
            if (containerFlag.has(containerFlags::vertical)) {
                containerFlag.set(containerFlags::vertical, false);

                updateFrame();
            }

            return this;
        }

        listView* setVertical() {
            if (!containerFlag.has(containerFlags::vertical)) {
                containerFlag.set(containerFlags::vertical, true);
            
                updateFrame();
            }

            return this;
        }

        template<typename T>
        T* get(int index){
            if (index > (signed)content.size() - 1)
                return nullptr;

            if (index < 0)
                index = (signed)content.size() + index - 1;

            return (T*)this->content[index];
        }

        template<typename T = element>
        T* getElement(std::string_view Name) const {
            for (auto* c : content){
                if (c->getName() == Name)
                    return c;

                if (dynamic_cast<const listView*>(c)) {
                    element* tmp = static_cast<listView*>(c)->getElement<T>(Name);
                    
                    if (tmp) return tmp;
                }
            }

            return nullptr;
        }

        template<typename T = element>
        std::vector<T*> getElements() const {
            std::vector<T*> Result;

            for (auto* c : content){
                if (dynamic_cast<T*>(c))
                    Result.push_back(static_cast<T*>(c));

                if (dynamic_cast<const listView*>(c)) {
                    auto tmp = static_cast<listView*>(c)->getElements<T>();
                    
                    Result.insert(Result.end(), tmp.begin(), tmp.end());
                }
            }

            return Result;
        }

        std::vector<element*> getVisibleContent() const;

        const std::vector<element*> getContent();

        listView* setDisplay(bool d);

        bool contentIsShown(element* other) const;

        // Expect [innerBounds.position, innerBounds.dimensions]
        rectangle getInnerBounds() const { return innerBounds; }

        element* copy() const override;
    protected:
        element* createInstance() const override {
            return new listView();
        }

        // Combines border glyphs where applicable
        void postProcessBorders(element* A, element* B, std::vector<terminal::cell>& Parent_Buffer);

        IVector2 getDimensionLimit() const; TODO("Deprecate")

        IVector2 getOuterBounds() const;    TODO("Deprecate")

        void updateInnerBounds();

        rectangle getBoundLimits();

        void computeDynamicSize();

        bool contentChanged() const;

        bool hasTransparentContent() const;

        std::vector<terminal::cell>& render() override;

        // Give access to Last_Child
        friend class scrollView;
    };

    class scrollView : public listView {
    protected:
        int scrollIndex = 0;  // Render based on the offset of the scrollIndex by flow direction.
    public:

        scrollView() : listView() { containerFlag.add(containerFlags::overflow); }

        void scrollUp() { scrollIndex++; };

        void scrollDown() { scrollIndex--; };

        scrollView* setScrolling(bool allow);

        std::string getTypedName() const override;
    protected:
        std::vector<terminal::cell>& render() override;
    
        element* createInstance() const override {
            return new scrollView();
        }
    };
}

#endif