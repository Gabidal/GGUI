#ifndef _ELEMENT_SUITE_H_
#define _ELEMENT_SUITE_H_

#include "utils.h"
#include <ggui_dev.h>

// element class testing, no styling from style class, only internal styling member testing instead.

namespace tester {
    class elementSuite : public utils::TestSuite {
    public:
        elementSuite() : utils::TestSuite("Element node behaviour") {
            add_test("default_construction", "Verify default element state", test_default_construction);
            add_test("dimensions_set", "Width/Height and STRETCH staining", test_dimensions_set);
            add_test("individual_width_height", "Independent width / height setters", test_individual_width_height);
            // add_test("position_and_absolute", "Position + absolute cache incl. border offset", test_position_and_absolute);
            add_test("add_child_parent_relationship", "addChild establishes parent & ordering", test_add_child_parent_relationship);
            add_test("remove_child_by_pointer", "remove(element*) deletes child entry", test_remove_child_by_pointer);
            add_test("remove_child_by_index", "remove(index) deletes child entry", test_remove_child_by_index);
            add_test("display_toggle_propagation", "display(false/true) cascades to children", test_display_toggle_propagation);
            add_test("dynamic_size_allowed", "Parent grows when Allow_Dynamic_Size true", test_dynamic_size_allowed);
            add_test("dynamic_size_disallowed", "Parent remains static when dynamic size off", test_dynamic_size_disallowed);
            add_test("overflow_and_wrap_flags", "allowOverflow / wrap flags", test_overflow_and_wrap_flags);
            add_test("anchor_and_flow_priority", "Anchor and flow priority setters", test_anchor_and_flow_priority);
            add_test("color_setters", "Background / Text / Border color setters", test_color_setters);
            add_test("hover_focus_color_composition", "composeAllTextRGBvalues precedence", test_hover_focus_color_composition);
            add_test("margin_setter", "setMargin copies values", test_margin_setter);
            add_test("name_and_lookup", "setName & getElement recursive search", test_name_and_lookup);
            // add_test("fitting_dimensions_basic", "getFittingDimensions base cases", test_fitting_dimensions_basic);
            add_test("reorder_childs_z", "reOrderChilds sorts by Z", test_reorder_childs_z);
            add_test("mouse_on_hover", "Mouse hover detection and onHover callback", test_mouse_on_hover);
        }
    private:
        // Helper: access STRETCH flag quickly
        static bool hasFlag(GGUI::element* e, GGUI::INTERNAL::STAIN_TYPE f){
            return e->getDirty().has(static_cast<unsigned int>(f));
        }

        static void test_default_construction(){
            GGUI::element e; // root-like element (no parent)
            ASSERT_EQ((unsigned)1, e.getWidth());
            ASSERT_EQ((unsigned)1, e.getHeight());
            ASSERT_FLOAT_EQ(1.0f, e.getOpacity(), 0.0001f);
            ASSERT_TRUE(e.isDisplayed());
            ASSERT_FALSE(e.hasBorder());
            ASSERT_TRUE(e.getParent() == nullptr);
        }

        static void test_dimensions_set(){
            GGUI::element e;
            e.setDimensions(5,4);
            ASSERT_EQ((unsigned)5, e.getWidth());
            ASSERT_EQ((unsigned)4, e.getHeight());
            ASSERT_TRUE(hasFlag(&e, GGUI::INTERNAL::STAIN_TYPE::STRETCH) || hasFlag(&e, GGUI::INTERNAL::STAIN_TYPE::COLOR)); // fullyStain sets multiple
        }

        static void test_individual_width_height(){
            GGUI::element e; 
            e.setWidth(7);
            ASSERT_EQ((unsigned)7, e.getWidth());
            e.setHeight(9);
            ASSERT_EQ((unsigned)9, e.getHeight());
        }

        // static void test_position_and_absolute(){
        //     GGUI::element parent; parent.setDimensions(20,10);
        //     auto child = new GGUI::element();
        //     child->setPosition({2,3,0});
        //     parent.addChild(child);
        //     child->updateAbsolutePositionCache();
        //     ASSERT_TRUE(child->getAbsolutePosition().X == (short)2);
        //     ASSERT_TRUE(child->getAbsolutePosition().Y == (short)3);
        //     parent.showBorder(true);
        //     child->updateAbsolutePositionCache();
        //     ASSERT_TRUE(child->getAbsolutePosition().X == (short)3); // +1 border offset
        //     ASSERT_TRUE(child->getAbsolutePosition().Y == (short)4);
        // }

        static void test_add_child_parent_relationship(){
            GGUI::element parent; parent.setDimensions(10,10);
            auto child = new GGUI::element();
            parent.addChild(child);
            ASSERT_TRUE(child->getParent() == &parent);
            ASSERT_EQ((size_t)1, parent.getChilds().size());
        }

        static void test_remove_child_by_pointer(){
            GGUI::element parent; parent.setDimensions(10,10);
            auto child = new GGUI::element();
            parent.addChild(child);
            ASSERT_EQ((size_t)1, parent.getChilds().size());
            bool removed = parent.remove(child); // deletes child
            ASSERT_TRUE(removed);
            ASSERT_EQ((size_t)0, parent.getChilds().size());
        }

        static void test_remove_child_by_index(){
            GGUI::element parent; parent.setDimensions(10,10);
            auto c1 = new GGUI::element();
            auto c2 = new GGUI::element();
            parent.addChild(c1);
            parent.addChild(c2);
            ASSERT_EQ((size_t)2, parent.getChilds().size());
            bool ok = parent.remove(0u);
            ASSERT_TRUE(ok);
            ASSERT_EQ((size_t)1, parent.getChilds().size());
        }

        static void test_display_toggle_propagation(){
            GGUI::element parent; parent.setDimensions(5,5);
            auto child = new GGUI::element();
            parent.addChild(child);
            ASSERT_TRUE(child->isDisplayed());
            parent.display(false);
            ASSERT_FALSE(parent.isDisplayed());
            ASSERT_FALSE(child->isDisplayed());
            parent.display(true);
            ASSERT_TRUE(parent.isDisplayed());
            ASSERT_TRUE(child->isDisplayed());
        }

        static void test_dynamic_size_allowed(){
            GGUI::element parent; parent.setDimensions(1,1); parent.allowDynamicSize(true);
            auto child = new GGUI::element(); child->setDimensions(6,4);
            parent.addChild(child);
            ASSERT_TRUE(parent.getWidth() >= child->getWidth());
            ASSERT_TRUE(parent.getHeight() >= child->getHeight());
        }

        static void test_dynamic_size_disallowed(){
            GGUI::element parent; parent.setDimensions(1,1); parent.allowDynamicSize(false);
            auto child = new GGUI::element(); child->setDimensions(6,4);
            parent.addChild(child);
            ASSERT_EQ((unsigned)1, parent.getWidth());
            ASSERT_EQ((unsigned)1, parent.getHeight());
        }

        static void test_overflow_and_wrap_flags(){
            GGUI::element e;
            e.allowOverflow(true);
            ASSERT_TRUE(e.isOverflowAllowed());
            e.allowOverflow(false);
            ASSERT_FALSE(e.isOverflowAllowed());
            e.setWrap(true);
            ASSERT_TRUE(e.getWrap());
            e.setWrap(false);
            ASSERT_FALSE(e.getWrap());
        }

        static void test_anchor_and_flow_priority(){
            GGUI::element e;
            e.setAnchor(GGUI::ANCHOR::CENTER);
            ASSERT_EQ((int)GGUI::ANCHOR::CENTER, (int)e.getAlign());
            e.setFlowPriority(GGUI::DIRECTION::COLUMN);
            ASSERT_EQ((int)GGUI::DIRECTION::COLUMN, (int)e.getFlowPriority());
        }

        static void test_color_setters(){
            GGUI::element e;
            e.setBackgroundColor(GGUI::COLOR::BLUE);
            ASSERT_EQ(GGUI::COLOR::BLUE, e.getBackgroundColor());
            e.setTextColor(GGUI::COLOR::GREEN);
            ASSERT_EQ(GGUI::COLOR::GREEN, e.getTextColor());
            e.setBorderColor(GGUI::COLOR::RED);
            ASSERT_EQ(GGUI::COLOR::RED, e.getBorderColor());
            e.setHoverBackgroundColor(GGUI::COLOR::YELLOW);
            ASSERT_EQ(GGUI::COLOR::YELLOW, e.getHoverBackgroundColor());
            e.setFocusBackgroundColor(GGUI::COLOR::MAGENTA);
            ASSERT_EQ(GGUI::COLOR::MAGENTA, e.getFocusBackgroundColor());
        }

        static void test_hover_focus_color_composition(){
            GGUI::element e; 
            e.setTextColor({10,20,30});
            e.setBackgroundColor({1,2,3});
            e.setHoverTextColor({40,50,60});
            e.setHoverBackgroundColor({4,5,6});
            e.setFocusTextColor({70,80,90});
            e.setFocusBackgroundColor({7,8,9});
            auto base = e.composeAllTextRGBvalues();
            ASSERT_EQ(GGUI::RGB(10,20,30), base.first);
            e.setHoverState(true);
            auto hov = e.composeAllTextRGBvalues();
            ASSERT_EQ(GGUI::RGB(40,50,60), hov.first);
            e.setFocus(true); // focus should override hover
            auto foc = e.composeAllTextRGBvalues();
            ASSERT_EQ(GGUI::RGB(70,80,90), foc.first);
        }

        static void test_margin_setter(){
            GGUI::element e; 
            GGUI::margin m(1,2,3,4); e.setMargin(m);
            auto gm = e.getMargin();
            ASSERT_EQ(1u, gm.top.get<unsigned int>());
            ASSERT_EQ(2u, gm.bottom.get<unsigned int>());
            ASSERT_EQ(3u, gm.left.get<unsigned int>());
            ASSERT_EQ(4u, gm.right.get<unsigned int>());
        }

        static void test_name_and_lookup(){
            GGUI::element root; root.setDimensions(20,10);
            auto c1 = new GGUI::element(); c1->setName("child1");
            auto c2 = new GGUI::element(); c2->setName("child2");
            root.addChild(c1); root.addChild(c2);
            ASSERT_TRUE(root.getElement("child2") == c2);
        }

        // static void test_fitting_dimensions_basic(){
        //     GGUI::element parent; parent.setDimensions(10,5); parent.showBorder(false);
        //     auto c = new GGUI::element(); c->setDimensions(3,2); parent.addChild(c);
        //     auto fit = parent.getFittingDimensions(c);
        //     ASSERT_TRUE(fit.first  <= parent.getWidth());
        //     ASSERT_TRUE(fit.second <= parent.getHeight());
        // }

        static void test_reorder_childs_z(){
            GGUI::element parent; parent.setDimensions(10,5);
            auto a = new GGUI::element(); a->setPosition({0,0,5});
            auto b = new GGUI::element(); b->setPosition({0,0,1});
            auto c = new GGUI::element(); c->setPosition({0,0,3});
            parent.addChild(a); parent.addChild(b); parent.addChild(c);
            // After addChild calls reOrderChilds, order should be by Z ascending
            auto& childs = parent.getChilds();
            ASSERT_TRUE(childs[0]->getPosition().z <= childs[1]->getPosition().z);
            ASSERT_TRUE(childs[1]->getPosition().z <= childs[2]->getPosition().z);
        }

        static void test_mouse_on_hover() {
            using namespace GGUI;
            
            int w = 100, h = 100;

            element hoverable(
                width(w) | height(h) | onClick([](element*){
                    return true;
                }),
                true
            );

            INTERNAL::mouse = {0, 0};                   // Let's test it on all four corners
            INTERNAL::eventHandler();                   // run pipeline 
            ASSERT_TRUE(hoverable.isHovered());         // now let's see if the hoverable has onHover enabled

            INTERNAL::mouse = {w + 1, h + 1};           // Let's move the mouse outside the perimeter to disable hover
            INTERNAL::eventHandler();
            ASSERT_FALSE(hoverable.isHovered());        // now let's see if the hoverable has onHover disabled
            
            INTERNAL::mouse = {w - 1, 0};               // now let's try top right corner next
            INTERNAL::eventHandler();                   // run pipeline
            ASSERT_TRUE(hoverable.isHovered());         // now let's see if the hoverable has onHover enabled

            INTERNAL::mouse = {w + 1, h + 1};           // now clear hover
            INTERNAL::eventHandler();                   // run pipeline
            ASSERT_FALSE(hoverable.isHovered());        // now let's see if the hoverable has onHover disabled

            INTERNAL::mouse = {0, h - 1};               // now let's try bottom left corner next
            INTERNAL::eventHandler();                   // run pipeline
            ASSERT_TRUE(hoverable.isHovered());         // now let's see if the hoverable has

            INTERNAL::mouse = {w + 1, h + 1};           // now clear hover
            INTERNAL::eventHandler();                   // run pipeline
            ASSERT_FALSE(hoverable.isHovered());        // now let's see if the hoverable has

            INTERNAL::mouse = {w - 1, h - 1};           // now let's try bottom right corner next
            INTERNAL::eventHandler();                   // run pipeline
            ASSERT_TRUE(hoverable.isHovered());         // now let's see if the hoverable has
        }
    };
}

#endif
