#ifndef _PROGRESS_BAR_H_
#define _PROGRESS_BAR_H_

#include "element.h"

#include "../core/utils/color.h"

namespace GGUI{

    namespace progress{
        enum class partType : uint8_t {
            EMPTY,
            HEAD,
            BODY,
            TAIL
        };

        struct part {
            terminal::cell character = terminal::cell(' ');
            RGB color = COLOR::GRAY;
            partType type = partType::EMPTY;

            constexpr part(partType t, RGB fillColor = COLOR::GREEN, terminal::cell cs = terminal::cell(' ')) { type = t; color = fillColor; character = cs; }
        };

        class Bar : public element {
        protected:
            float Progress = 0; // 0.0 - 1.0

            terminal::cell Head = terminal::cell('>');
            terminal::cell Body = terminal::cell('-');
            terminal::cell Tail = terminal::cell('|');
            terminal::cell Empty = terminal::cell(' ');

            RGB Head_Color = GGUI::COLOR::LIGHT_GRAY;
            RGB Body_Color = GGUI::COLOR::GRAY;
            RGB Tail_Color = GGUI::COLOR::GRAY;
            RGB Empty_Color = GGUI::COLOR::DARK_GRAY;
        public:

            Bar() = default;

            Bar* setHeadCharacter(terminal::cell cs) { Head = cs; return this; }
            Bar* setBodyCharacter(terminal::cell cs) { Body = cs; return this; }
            Bar* setTailCharacter(terminal::cell cs) { Tail = cs; return this; }
            Bar* setEmptyCharacter(terminal::cell cs) { Empty = cs; return this; }

            Bar* setHeadColor(RGB color) { Head_Color = color; return this; }
            Bar* setBodyColor(RGB color) { Body_Color = color; return this; }
            Bar* setTailColor(RGB color) { Tail_Color = color; return this; }
            Bar* setEmptyColor(RGB color) { Empty_Color = color; return this; }

            unsigned int getIndexofHead() const { return floor(Progress * (getWidth() - hasBorder() * 2)); }

            void colorBar();

            Bar* setProgress(float New_Progress);
            
            float getProgress() const { return Progress; }

            void updateProgress(float add);

            ~Bar() override = default;

            std::string getTypedName() const override {
                return "progressBar<" + ID + ">";
            }
        protected:
            std::vector<terminal::cell>& render() override;
            
            element* createInstance() const override {
                return new Bar();
            }
        };
    }
}

#endif