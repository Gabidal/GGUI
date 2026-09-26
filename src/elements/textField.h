#ifndef _TEXT_FIELD_H_
#define _TEXT_FIELD_H_

#include "element.h"

namespace GGUI{
    class textField : public element {
    public:
        enum class alignments : uint8_t {
            LEFT, CENTER, RIGHT
        };
    protected:
        // Used to describe line snipped from the text
        struct line {
            uint32_t start, end;

            constexpr size_t getSize() const { return end - start; }
        };

        std::string text = "";

        // This will hold the text by lines, and does not re-allocate memory for whole text, only for indicies.
        std::vector<line> textLineCache;

        alignments alignment = alignments::LEFT;

        void updateTextCache();
    public:
        textField() = default;

        ~textField() override = default;

        void setText(std::string& text);

        std::string_view getText() {
            return text;
        }
        
        void input(std::function<void(textField*, char)> Then);
        
    protected:
        void alignTextLeft(std::vector<terminal::cell>& Result);
        
        void alignTextRight(std::vector<terminal::cell>& Result);
        
        void alignTextCenter(std::vector<terminal::cell>& Result);

        std::vector<terminal::cell>& render() override;

        element* createInstance() const override {
            return new textField();
        }

        // switchBox uses textField::render() so lets give it some access.
        friend class switchBox;
    };
}

#endif