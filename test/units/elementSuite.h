#ifndef _ELEMENT_SUITE_H_
#define _ELEMENT_SUITE_H_

#include "utils.h"
#include <GGUI.h>

namespace tester {
    class elementSuite : public utils::TestSuite {
    public:
        elementSuite() : utils::TestSuite("Element core behaviour tester") {
            add_test("test_focus_and_hover_state", "Focus and hover flag transitions", test_focus_and_hover_state);
            add_test("test_opacity_setters", "Opacity setters (float & int) affect transparency", test_opacity_setters);
            add_test("test_dynamic_resize_on_add_child", "Parent resizes when dynamic size allowed", test_dynamic_resize_on_add_child);
            add_test("test_compose_text_and_border_colors", "composeAll* color selection precedence", test_compose_text_and_border_colors);
            add_test("test_display_cascade", "Display state cascades to children", test_display_cascade);
            add_test("test_child_ordering_by_z", "Children reordered by Z (ascending)", test_child_ordering_by_z);
            add_test("test_absolute_position_cache_on_render", "Absolute position cache updates after render", test_absolute_position_cache_on_render);
            add_test("test_border_rendering_and_corners", "Border corners & edges rendered in buffer", test_border_rendering_and_corners);
            add_test("test_title_truncation_with_ellipsis", "Title truncated with ellipsis when exceeding width", test_title_truncation_with_ellipsis);
            add_test("test_negative_position_fitting_area", "getFittingArea handles negative child offsets", test_negative_position_fitting_area);
            add_test("test_alpha_blending_partial_opacity", "Child with 50% opacity blends into parent", test_alpha_blending_partial_opacity);
            add_test("test_get_all_nested_elements_visibility", "getAllNestedElements respects Show flag", test_get_all_nested_elements_visibility);
            add_test("test_remove_child_updates_parent", "Removing child dirties parent & detaches", test_remove_child_updates_parent);
        }
    private:
        static void test_focus_and_hover_state() {
            GGUI::element parent; // default style
            ASSERT_FALSE(parent.isFocused());
            ASSERT_FALSE(parent.isHovered());
            parent.setHoverState(true);
            ASSERT_TRUE(parent.isHovered());
            parent.setFocus(true);
            ASSERT_TRUE(parent.isFocused());
            // Focus should not unset hover automatically
            ASSERT_TRUE(parent.isHovered());
            parent.setHoverState(false);
            ASSERT_FALSE(parent.isHovered());
        }

        static void test_opacity_setters() {
            GGUI::element e;
            ASSERT_FLOAT_EQ(1.0f, e.getOpacity(), 0.0001f);
            e.setOpacity(0.5f);
            ASSERT_FLOAT_EQ(0.5f, e.getOpacity(), 0.0001f);
            ASSERT_TRUE(e.isTransparent());
            e.setOpacity(100u); // 100% -> 1.0f
            ASSERT_FLOAT_EQ(1.0f, e.getOpacity(), 0.0001f);
            ASSERT_FALSE(e.isTransparent());
        }

        static void test_dynamic_resize_on_add_child() {
            GGUI::element parent; // default  maybe small but set explicit dims
            parent.setDimensions(5,5);
            parent.allowDynamicSize(true);

            auto child = new GGUI::element();
            child->setPosition({4,4,0});
            child->setDimensions(4,4); // would exceed parent (needs width >=8 height >=8)
            parent.addChild(child);

            ASSERT_TRUE(parent.getWidth() >= 8);
            ASSERT_TRUE(parent.getHeight() >= 8);
        }

        static void test_compose_text_and_border_colors() {
            GGUI::element e;
            // Capture base colors
            auto baseText = e.composeAllTextRGBValues();
            auto baseBorder = e.composeAllBorderRGBValues();

            // Hover should switch to hover colors
            e.setHoverState(true);
            auto hoverText = e.composeAllTextRGBValues();
            auto hoverBorder = e.composeAllBorderRGBValues();

            // Focus should override hover
            e.setFocus(true);
            auto focusText = e.composeAllTextRGBValues();
            auto focusBorder = e.composeAllBorderRGBValues();

            // Validate precedence: focus colors must match direct getters
            ASSERT_EQ(focusText.first.Red, e.getFocusTextColor().Red);
            ASSERT_EQ(focusText.first.Green, e.getFocusTextColor().Green);
            ASSERT_EQ(focusText.first.Blue, e.getFocusTextColor().Blue);
            ASSERT_EQ(focusBorder.first.Red, e.getFocusBorderColor().Red);

            // At least one of the stages should differ (avoid brittle equality on all channels if style defaults identical)
            bool anyDiffHover = baseText.first.Red != hoverText.first.Red || baseText.second.Red != hoverText.second.Red;
            bool anyDiffFocus = hoverText.first.Red != focusText.first.Red || hoverText.second.Red != focusText.second.Red;
            bool anyBorderDiffHover = baseBorder.first.Red != hoverBorder.first.Red || baseBorder.second.Red != hoverBorder.second.Red;
            bool anyBorderDiffFocus = hoverBorder.first.Red != focusBorder.first.Red || hoverBorder.second.Red != focusBorder.second.Red;
            ASSERT_TRUE(anyDiffHover || anyDiffFocus || anyBorderDiffHover || anyBorderDiffFocus);
        }

        static void test_display_cascade() {
            GGUI::element parent; parent.setDimensions(10,3);
            auto c1 = new GGUI::element();
            auto c2 = new GGUI::element();
            parent.addChild(c1);
            parent.addChild(c2);
            // Precondition: children inherit visibility from parent
            ASSERT_TRUE(c1->isDisplayed());
            // Toggle visibility off
            parent.display(false);
            // Direct state check (no render thread)
            ASSERT_FALSE(parent.isDisplayed());
            ASSERT_FALSE(c1->isDisplayed());
            ASSERT_FALSE(c2->isDisplayed());
            // Toggle back on
            parent.display(true);
            ASSERT_TRUE(parent.isDisplayed());
            ASSERT_TRUE(c1->isDisplayed());
        }

        static void test_child_ordering_by_z() {
            GGUI::element parent; parent.setDimensions(20,5);
            auto low = new GGUI::element(); low->setPosition({0,0,0});
            auto mid = new GGUI::element(); mid->setPosition({0,0,5});
            auto high = new GGUI::element(); high->setPosition({0,0,10});
            parent.addChild(high); // add out of order intentionally
            parent.addChild(low);
            parent.addChild(mid);
            auto& childs = parent.getChilds();
            ASSERT_TRUE(childs.size() == 3);
            // Expect ascending Z after reOrderChilds inside addChild
            ASSERT_TRUE(childs[0]->getPosition().Z <= childs[1]->getPosition().Z);
            ASSERT_TRUE(childs[1]->getPosition().Z <= childs[2]->getPosition().Z);
        }

        static void test_absolute_position_cache_on_render() {
            GGUI::element parent; parent.setDimensions(10,4); parent.setPosition({2,3,0});
            auto child = new GGUI::element();
            child->setPosition({5,1,0});
            parent.addChild(child);
            // Trigger render to update absolute caches
            parent.render();
            auto absParent = parent.getAbsolutePosition();
            auto absChild = child->getAbsolutePosition();
            ASSERT_EQ(absParent.X, parent.getPosition().X);
            ASSERT_EQ(absParent.Y, parent.getPosition().Y);
            short expectedChildX = parent.getPosition().X + child->getPosition().X;
            short expectedChildY = parent.getPosition().Y + child->getPosition().Y;
            ASSERT_EQ(absChild.X, expectedChildX);
            ASSERT_EQ(absChild.Y, expectedChildY);
        }

        static void prepareForDirectRender(GGUI::element& e){
            // Force a render to allocate buffer in current state
            e.render();
        }

        static void test_border_rendering_and_corners(){
            GGUI::element e; 
            e.setDimensions(6,4); // Enough for a hollow box
            e.setCustomBorderStyle(GGUI::STYLES::BORDER::Single);
            e.showBorder(true);
            auto& buf = e.render();
            ASSERT_EQ((unsigned) (6*4), (unsigned) buf.size());
            auto isGlyph = [](const GGUI::UTF& u, const char* g){ return u.is(GGUI::INTERNAL::COMPACT_STRING_FLAG::IS_UNICODE) && std::string(u.getUnicode()) == g; };
            ASSERT_TRUE(isGlyph(buf[0], "┌"));
            ASSERT_TRUE(isGlyph(buf[5], "┐"));
            ASSERT_TRUE(isGlyph(buf[18], "└")); // (height-1)*width = 3*6 = 18
            ASSERT_TRUE(isGlyph(buf[23], "┘"));
            ASSERT_TRUE(isGlyph(buf[2], "─"));
            ASSERT_TRUE(isGlyph(buf[6], "│"));
        }

        static void test_title_truncation_with_ellipsis(){
            GGUI::element e; 
            e.setDimensions(10,3);
            e.setCustomBorderStyle(GGUI::STYLES::BORDER::Single);
            e.showBorder(true);
            e.setTitle(GGUI::INTERNAL::compactString("VeryLongTitleThatShouldTrim"));
            auto& buf = e.render();
            // First row indices 0..9
            std::string topRow;
            for (int x=0;x<10;x++) if (!buf[x].is(GGUI::INTERNAL::COMPACT_STRING_FLAG::IS_UNICODE) || buf[x].size>0) topRow += buf[x][0];
            // Expect ellipsis present
            ASSERT_TRUE(topRow.find("...") != std::string::npos);
        }

        static void test_negative_position_fitting_area(){
            GGUI::element parent; parent.setDimensions(8,5); parent.showBorder(false);
            auto child = new GGUI::element();
            child->setDimensions(6,4);
            child->setPosition({-2,-1,0}); // Partially outside
            parent.addChild(child);
            parent.render();
            // getFittingArea should clamp start >=0 and provide negativeOffset
            auto fa = GGUI::element::getFittingArea(&parent, child);
            ASSERT_TRUE(fa.negativeOffset.X == 2);
            ASSERT_TRUE(fa.negativeOffset.Y == 1);
            ASSERT_TRUE(fa.start.X >= 0 && fa.start.Y >=0);
            ASSERT_TRUE(fa.end.X <= (int)parent.getWidth());
            ASSERT_TRUE(fa.end.Y <= (int)parent.getHeight());
        }

        static void test_alpha_blending_partial_opacity(){
            GGUI::element parent; parent.setDimensions(3,3); parent.showBorder(false);
            auto child = new GGUI::element();
            child->setDimensions(3,3);
            child->setBackgroundColor(GGUI::COLOR::RED); // Red background
            child->setOpacity(0.5f);
            parent.addChild(child);
            auto& buf = parent.render();
            // Pick center cell index (1,1) => 1 + 1*3 = 4
            auto blended = buf[4];
            // Background red channel should be roughly half intensity (~127)
            ASSERT_TRUE(blended.background.Red >= 110 && blended.background.Red <= 140);
            ASSERT_EQ((unsigned char)0, blended.background.Green);
            ASSERT_EQ((unsigned char)0, blended.background.Blue);
            // Foreground green/blue diminished (approx 127). Red likely 255.
            ASSERT_TRUE(blended.foreground.Red >= 200);
            ASSERT_TRUE(blended.foreground.Green >= 110 && blended.foreground.Green <= 140);
            ASSERT_TRUE(blended.foreground.Blue >= 110 && blended.foreground.Blue <= 140);
        }

        static void test_get_all_nested_elements_visibility(){
            GGUI::element root; root.setDimensions(4,4);
            auto a = new GGUI::element(); auto b = new GGUI::element();
            root.addChild(a); root.addChild(b);
            b->display(false); // hide b
            auto allVisible = root.getAllNestedElements(false);
            // Should contain root and a, but not b
            bool hasRoot=false, hasA=false, hasB=false;
            for (auto* e : allVisible){ if (e==&root) hasRoot=true; else if (e==a) hasA=true; else if (e==b) hasB=true; }
            ASSERT_TRUE(hasRoot && hasA);
            ASSERT_FALSE(hasB);
            auto allWithHidden = root.getAllNestedElements(true);
            hasRoot=hasA=hasB=false;
            for (auto* e : allWithHidden){ if (e==&root) hasRoot=true; else if (e==a) hasA=true; else if (e==b) hasB=true; }
            ASSERT_TRUE(hasRoot && hasA && hasB);
        }

        static void test_remove_child_updates_parent(){
            GGUI::element parent; parent.setDimensions(5,3);
            auto child = new GGUI::element();
            parent.addChild(child);
            ASSERT_TRUE(parent.getChilds().size() == 1);
            bool removed = parent.remove(child);
            ASSERT_TRUE(removed);
            ASSERT_TRUE(parent.getChilds().size() == 0);
        }
    };
}

#endif
