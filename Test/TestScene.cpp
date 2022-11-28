//
// Created by Aleksei Lukianchuk on 27-Nov-22.
//

#include "Tester.h"
#include "Scene.h"
#include "Drawing/Latex.h"
#include "Drawing/Drawable.h"

#include "TBox.h"
#include "TObject.h"

using namespace std;
using namespace eft::utils::draw;

void TestDrawableCtorFromUniquePtr() {
    {
        auto box = make_unique<TBox>(0, 0, 1, 1);
        ASSERT_NO_THROW(Drawable::Create(std::move(box)));
    }
    {
        auto box = make_unique<TBox>(0, 0, 1, 1);
        auto box_drawable = Drawable::Create(std::move(box));
        ASSERT(box_drawable.get());
        ASSERT_EQUAL(box_drawable->name, "");
        ASSERT_EQUAL(box_drawable->draw_options, "");
        ASSERT_EQUAL(box_drawable->should_be_drawn, true);
    }
    {
        auto box = make_unique<TBox>(0, 0, 1, 1);
        auto box_drawable = Drawable::Create(std::move(box), "options");
        ASSERT(box_drawable.get());
        ASSERT_EQUAL(box_drawable->name, "");
        ASSERT_EQUAL(box_drawable->draw_options, "options");
        ASSERT_EQUAL(box_drawable->should_be_drawn, true);
    }
    {
        auto box = make_unique<TBox>(0, 0, 1, 1);
        auto box_drawable = Drawable::Create(std::move(box), "options_for_drawing", "name_box");
        ASSERT(box_drawable.get());
        ASSERT_EQUAL(box_drawable->name, "name_box");
        ASSERT_EQUAL(box_drawable->draw_options, "options_for_drawing");
        ASSERT_EQUAL(box_drawable->should_be_drawn, true);
    }
    {
        auto box = make_unique<TBox>(0, 0, 1, 1);
        auto box_drawable = Drawable::Create(std::move(box), "options_for_drawing", "name_box");
        ASSERT(box_drawable.get());
        box_drawable->should_be_drawn = false;
        ASSERT_EQUAL(box_drawable->name, "name_box");
        ASSERT_EQUAL(box_drawable->draw_options, "options_for_drawing");
        ASSERT_EQUAL(box_drawable->should_be_drawn, false);
    }
}

void TestDrawableCtor() {
    {
        auto box = new TBox(0, 0, 1, 1);
        ASSERT_NO_THROW(Drawable::Create(box));
    }
    {
        auto box = new TBox(0, 0, 1, 1);
        auto box_drawable = Drawable::Create(std::move(box));
        ASSERT(box_drawable.get());
        ASSERT_EQUAL(box_drawable->name, "");
        ASSERT_EQUAL(box_drawable->draw_options, "");
        ASSERT_EQUAL(box_drawable->should_be_drawn, true);
    }
    {
        auto box = new TBox(0, 0, 1, 1);
        auto box_drawable = Drawable::Create(std::move(box), "options");
        ASSERT(box_drawable.get());
        ASSERT_EQUAL(box_drawable->name, "");
        ASSERT_EQUAL(box_drawable->draw_options, "options");
        ASSERT_EQUAL(box_drawable->should_be_drawn, true);
    }
    {
        auto box = new TBox(0, 0, 1, 1);
        auto box_drawable = Drawable::Create(std::move(box), "options_for_drawing", "name_box");
        ASSERT(box_drawable.get());
        ASSERT_EQUAL(box_drawable->name, "name_box");
        ASSERT_EQUAL(box_drawable->draw_options, "options_for_drawing");
        ASSERT_EQUAL(box_drawable->should_be_drawn, true);
    }
    {
        auto box = new TBox(0, 0, 1, 1);
        auto box_drawable = Drawable::Create(std::move(box), "options_for_drawing", "name_box");
        ASSERT(box_drawable.get());
        box_drawable->should_be_drawn = false;
        ASSERT_EQUAL(box_drawable->name, "name_box");
        ASSERT_EQUAL(box_drawable->draw_options, "options_for_drawing");
        ASSERT_EQUAL(box_drawable->should_be_drawn, false);
    }
}

EFT_IMPLEMENT_TESTFILE(Scene) {
    EFT_ADD_TEST(TestDrawableCtorFromUniquePtr, "Drawable")
    EFT_ADD_TEST(TestDrawableCtor,              "Drawable")
}
EFT_END_IMPLEMENT_TESTFILE(Scene);
