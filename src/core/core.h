#ifndef _CORE_H_
#define _CORE_H_

#undef min
#undef max

#include <functional>

#include "utils/utils.h"

#include "thread.h"

#include "../elements/element.h"
#include "../elements/listView.h"

#include "converter.h"

namespace GGUI{
    static struct mouse {
        enum class states : uint8_t {
            DISABLE,
            ENABLE
        } state = states::DISABLE;
        
        IVector2 position = {};

        constexpr bool collides(element* other) {
            IVector3 tmp(position);
            return utils::collides(other, tmp);
        }
    } currentMouse;

    namespace core{
        class bufferCapture;

        extern thread::guard<std::vector<converter::output::event::memory>> remember;
        
        extern std::unordered_map<std::string_view, element*> elementNames;

        class selectable {
            element* selected = nullptr;

            RGB textColor, backgroundColor;
            RGB borderGlyphColor, borderGlyphBackgroundColor;
        public:
            
            void link(element* newLinkable) {
                assert(newLinkable && "Improper linkage with nullptr object!");

                selected = newLinkable;

                updateFrame();
            }

            void unlink() {
                if (selected) {
                    selected = nullptr;

                    updateFrame();
                }
            }

            bool operator==(const element* other) const {
                return selected == other;
            }
        
            element* operate() const { return selected; }
        };

        extern selectable focusedOn;
        extern selectable hoveredOn;

        extern std::chrono::steady_clock::duration CURRENT_UPDATE_SPEED; // dynamic depending on load

        extern converter::input::base*  inputManager;
        extern converter::output::base* inputConverter; 

        extern listView* main;

        extern std::chrono::steady_clock::duration renderDelay;    // describes how long previous render cycle took in ms

        extern std::string now();

        extern std::string constructLoggerFileName();

        extern void SignalThreadTermination();

        extern void recallMemories();

        extern void eventHandler();

        extern int getFreeClassID(std::string n);

        extern void init();

        extern void deinit();

        extern void handleTabulator();

        extern void handleEscape();

        std::pair<rectangle, rectangle> getFittingArea(GGUI::element* Container, GGUI::element* Content);

        void nestElement(element* container, element* content, std::vector<terminal::cell>& Parent_Buffer, const std::vector<terminal::cell>& Child_Buffer);
    }
    
    extern void registerCleanupCallback(std::function<void()> Callback);

    extern void EXIT(int Signum = 0);
    
    extern void waitForTermination();

    extern listView* getRoot();
    
    extern void registerCleanupCallback(std::function<void()> Callback);

    extern void updateFrame();
    
    extern void pauseGGUI();

    extern void resumeGGUI();

    extern void pauseGGUI(std::function<void()> f);

    extern void GGUI(listView&& App, unsigned long long Sleep_For = 0);

    extern element* getElement(std::string_view name);
}

#endif