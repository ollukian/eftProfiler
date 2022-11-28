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
        auto box_drawable = Drawable::Create(box);
        ASSERT(box_drawable.get());
        ASSERT_EQUAL(box_drawable->name, "");
        ASSERT_EQUAL(box_drawable->draw_options, "");
        ASSERT_EQUAL(box_drawable->should_be_drawn, true);
    }
    {
        auto box = new TBox(0, 0, 1, 1);
        auto box_drawable = Drawable::Create(box, "options");
        ASSERT(box_drawable.get());
        ASSERT_EQUAL(box_drawable->name, "");
        ASSERT_EQUAL(box_drawable->draw_options, "options");
        ASSERT_EQUAL(box_drawable->should_be_drawn, true);
    }
    {
        auto box = new TBox(0, 0, 1, 1);
        auto box_drawable = Drawable::Create(box, "options_for_drawing", "name_box");
        ASSERT(box_drawable.get());
        ASSERT_EQUAL(box_drawable->name, "name_box");
        ASSERT_EQUAL(box_drawable->draw_options, "options_for_drawing");
        ASSERT_EQUAL(box_drawable->should_be_drawn, true);
    }
    {
        auto box = new TBox(0, 0, 1, 1);
        auto box_drawable = Drawable::Create(box, "options_for_drawing", "name_box");
        ASSERT(box_drawable.get());
        box_drawable->should_be_drawn = false;
        ASSERT_EQUAL(box_drawable->name, "name_box");
        ASSERT_EQUAL(box_drawable->draw_options, "options_for_drawing");
        ASSERT_EQUAL(box_drawable->should_be_drawn, false);
    }
    {
        auto box_1 = new TBox(0, 0, 1, 1);
        auto box_2 = new TBox(1, 2, 3, 4);
        auto box_drawable_1 = Drawable::Create(box_1, "options_for_drawing", "name_box");
        auto box_drawable_2 = Drawable::Create(box_2, "options_for_drawing", "name_box");
        ASSERT(box_drawable_1.get());
        ASSERT(box_drawable_2.get());
    }
    {
        auto box_1 = Drawable::Create(new TBox{1, 2, 3, 4});
        auto box_2 = Drawable::Create(new TBox{0, 0, 1, 1});
        ASSERT_NOT_EQUAL(box_1->As<TBox>()->GetX1(), box_2->As<TBox>()->GetX1());
        ASSERT_NOT_EQUAL(box_1->As<TBox>()->GetX2(), box_2->As<TBox>()->GetX2());
        ASSERT_NOT_EQUAL(box_1->As<TBox>()->GetY1(), box_2->As<TBox>()->GetY1());
        ASSERT_NOT_EQUAL(box_1->As<TBox>()->GetY2(), box_2->As<TBox>()->GetY2());

    }
}

//void TestDrawableTemplateCreation() {
//    {
//        Drawable::Create(new TBox{1, 2, 3, 4});
//        //Drawable::Create<TBox>({1., 2., 3., 4.});
//        //Drawable::Create<TBox>(std::initializer_list<float>{1, 2, 3, 4});
//        //Drawable::Create<std::initializer_list<TCanvas>>(std::initializer_list<TCanvas>{"c", "c", 1200, 800});
//        //Drawable::Create<TBox>({1, 2, 3, 4});
//        //auto box = Drawable::Create<TBox, float>({1, 2, 3, 4});
//    }
//}

void TestSceneBasicDrawableRegistering() {
    {
        eft::stats::Logger::SetFullPrinting();
        EFT_PROF_INFO("TestSceneBasicDrawableRegistering");
        auto canvas = Scene::Create(1200, 800);
        ASSERT(canvas);
        auto box1 = Drawable::Create(new TBox(1, 2, 3, 4), "opt", "box1");
        Scene::Register(box1);
        const auto& reg = Scene::GetRegistry();
        ASSERT_EQUAL(reg.size(), 1u);
        ASSERT_EQUAL(reg[0]->name, "box1");
        ASSERT_EQUAL(reg[0]->draw_options, "opt");
        ASSERT_EQUAL(reg[0]->should_be_drawn, true);
    }
    {
        auto canvas = Scene::Create(1200, 800);
        ASSERT(canvas);
        auto box1 = Drawable::Create(new TBox(1, 2, 3, 4), "opt", "box1");
        auto box2 = Drawable::Create(new TBox(1, 1, 1, 1), "", "box2");
        Scene::Register(box1);
        Scene::Register(box2);
        const auto& reg = Scene::GetRegistry();
        ASSERT_EQUAL(reg.size(), 2u);
        ASSERT_EQUAL(reg[0]->name, "box1");
        ASSERT_EQUAL(reg[0]->draw_options, "opt");
        ASSERT_EQUAL(reg[0]->should_be_drawn, true);
        ASSERT_EQUAL(reg[1]->name, "box2");
        ASSERT_EQUAL(reg[1]->draw_options, "");
        ASSERT_EQUAL(reg[1]->should_be_drawn, true);
    }
    {
        auto canvas = Scene::Create(1200, 800);
        ASSERT(canvas);
        auto box_raw1 = Scene::AddBox(1, 2, 3, 4);
        auto box_raw2 = Scene::AddBox(5, 6, 7, 8);
        auto box_active_ptr_1 = Scene::Register(box_raw1)->As<TBox>();
        auto box_active_ptr_2 = Scene::Register(box_raw2)->As<TBox>();

        ASSERT_EQUAL(box_raw1, nullptr);
        ASSERT_EQUAL(box_raw2, nullptr);
        ASSERT_NOT_EQUAL(box_active_ptr_1, nullptr);
        ASSERT_NOT_EQUAL(box_active_ptr_1, nullptr);

        ASSERT_EQUAL(Scene::GetRegistry().size(), 2u);
        ASSERT_EQUAL(box_active_ptr_1->GetX1(), 1);
        ASSERT_EQUAL(box_active_ptr_1->GetY1(), 2);
        ASSERT_EQUAL(box_active_ptr_1->GetX2(), 3);
        ASSERT_EQUAL(box_active_ptr_1->GetY2(), 4);

        ASSERT_EQUAL(box_active_ptr_2->GetX1(), 5);
        ASSERT_EQUAL(box_active_ptr_2->GetY1(), 6);
        ASSERT_EQUAL(box_active_ptr_2->GetX2(), 7);
        ASSERT_EQUAL(box_active_ptr_2->GetY2(), 8);
    }
}

EFT_IMPLEMENT_TESTFILE(Scene) {
    EFT_ADD_TEST(TestDrawableCtorFromUniquePtr, "Drawable")
    EFT_ADD_TEST(TestDrawableCtor,              "Drawable")
    EFT_ADD_TEST(TestSceneBasicDrawableRegistering, "Drawable")
}
EFT_END_IMPLEMENT_TESTFILE(Scene);
