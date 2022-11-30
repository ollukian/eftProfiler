//
// Created by Aleksei Lukianchuk on 27-Nov-22.
//

#include "Tester.h"
#include "Scene.h"
#include "Drawing/Latex.h"
#include "Drawing/Drawable.h"

#include "TBox.h"
#include "TObject.h"
#include "TH2D.h"
#include "TH1D.h"

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
    {
        auto box = std::make_shared<TBox>(1, 3, 2, 4);
        auto drawable = Drawable(box.get());
        ASSERT_EQUAL(drawable.As<TBox>()->GetX1(), 1);
        ASSERT_EQUAL(drawable.As<TBox>()->GetX2(), 2);
        ASSERT_EQUAL(drawable.As<TBox>()->GetY1(), 3);
        ASSERT_EQUAL(drawable.As<TBox>()->GetY2(), 4);

        box->SetX1(10);
        box->SetX2(20);
        box->SetY1(30);
        box->SetY2(40);

        ASSERT_EQUAL(box->GetX1(), 10);
        ASSERT_EQUAL(box->GetX2(), 20);
        ASSERT_EQUAL(box->GetY1(), 30);
        ASSERT_EQUAL(box->GetY2(), 40);

        ASSERT_EQUAL(drawable.As<TBox>()->GetX1(), 10);
        ASSERT_EQUAL(drawable.As<TBox>()->GetX2(), 20);
        ASSERT_EQUAL(drawable.As<TBox>()->GetY1(), 30);
        ASSERT_EQUAL(drawable.As<TBox>()->GetY2(), 40);

        drawable.As<TBox>()->SetX1(100);
        drawable.As<TBox>()->SetX2(200);
        drawable.As<TBox>()->SetY1(300);
        drawable.As<TBox>()->SetY2(400);

        ASSERT_EQUAL(box->GetX1(), 100);
        ASSERT_EQUAL(box->GetX2(), 200);
        ASSERT_EQUAL(box->GetY1(), 300);
        ASSERT_EQUAL(box->GetY2(), 400);

        ASSERT_EQUAL(drawable.As<TBox>()->GetX1(), 100);
        ASSERT_EQUAL(drawable.As<TBox>()->GetX2(), 200);
        ASSERT_EQUAL(drawable.As<TBox>()->GetY1(), 300);
        ASSERT_EQUAL(drawable.As<TBox>()->GetY2(), 400);
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
        EFT_PROF_CRITICAL("test #1");
        eft::stats::Logger::SetFullPrinting();
        EFT_PROF_INFO("TestSceneBasicDrawableRegistering");
        auto canvas = Scene::Create(1200, 800);
        ASSERT(canvas);
        auto box1 = Drawable::Create(new TBox(1, 2, 3, 4), "opt", "box1");
        Scene::Register(box1.get());
        const auto& reg = Scene::GetRegistry();
        ASSERT_EQUAL(reg.size(), 1u);
        ASSERT_EQUAL(reg[0]->name, "box1");
        ASSERT_EQUAL(reg[0]->draw_options, "opt");
        ASSERT_EQUAL(reg[0]->should_be_drawn, true);
        Scene::Clear();
    }
    {
        EFT_PROF_CRITICAL("test #2");
        auto canvas = Scene::Create(1200, 800);
        ASSERT(canvas);
        EFT_PROF_CRITICAL("test #3  create box #1");
        auto box1 = Drawable::Create(new TBox(1, 2, 3, 4), "opt", "box1");
        EFT_PROF_CRITICAL("test #3  create box #2");
        auto box2 = Drawable::Create(new TBox(1, 1, 1, 1), "", "box2");
        EFT_PROF_CRITICAL("test #3  register box #1");
        Scene::Register(box1.get());
        EFT_PROF_CRITICAL("test #3  register box #2");
        Scene::Register(box2.get());
        EFT_PROF_CRITICAL("test #3  get reg");
        const auto& reg = Scene::GetRegistry();
        ASSERT_EQUAL(reg.size(), 2u);
        ASSERT_EQUAL(reg[0]->name, "box1");
        ASSERT_EQUAL(reg[0]->draw_options, "opt");
        ASSERT_EQUAL(reg[0]->should_be_drawn, true);
        ASSERT_EQUAL(reg[1]->name, "box2");
        ASSERT_EQUAL(reg[1]->draw_options, "");
        ASSERT_EQUAL(reg[1]->should_be_drawn, true);
        Scene::Clear();
    }
    {
        EFT_PROF_CRITICAL("test #4");
        auto canvas = Scene::Create(1200, 800);
        ASSERT(canvas);
        auto box_raw1 = Scene::AddBox(1, 2, 3, 4)->As<TBox>();
        auto box_raw2 = Scene::AddBox(5, 6, 7, 8)->As<TBox>();
        //auto box_active_ptr_1 = Scene::Register(box_raw1)->As<TBox>();
        //auto box_active_ptr_2 = Scene::Register(box_raw2)->As<TBox>();

        ASSERT_NOT_EQUAL(box_raw1, nullptr);
        ASSERT_NOT_EQUAL(box_raw2, nullptr);

        ASSERT_EQUAL(Scene::GetRegistry().size(), 2u);
        ASSERT_EQUAL(box_raw1->GetX1(), 1);
        ASSERT_EQUAL(box_raw1->GetY1(), 2);
        ASSERT_EQUAL(box_raw1->GetX2(), 3);
        ASSERT_EQUAL(box_raw1->GetY2(), 4);

        ASSERT_EQUAL(box_raw2->GetX1(), 5);
        ASSERT_EQUAL(box_raw2->GetY1(), 6);
        ASSERT_EQUAL(box_raw2->GetX2(), 7);
        ASSERT_EQUAL(box_raw2->GetY2(), 8);
        Scene::Clear();
    }
    {
        EFT_PROF_CRITICAL("test #5");
        auto canvas = Scene::Create(1200, 800);
        ASSERT(canvas);

        shared_ptr<TH2D> cov = make_shared<TH2D>("test", "tsts", 10, 0, 10, 10, 0, 10);

        std::random_device rd;
        std::mt19937 rng(rd());
        std::uniform_int_distribution<std::mt19937::result_type> dist256(0, 255);
        for (size_t idx_x {0}; idx_x < 50; ++idx_x) {
            for (size_t idx_y {0}; idx_y < 50; ++idx_y) {
                cov->SetBinContent(idx_x + 1, idx_y + 1, dist256(rng));
            }
        }

        EFT_PROF_INFO("register hist");
        auto hist_drawable_handle = Scene::Register(cov);
        ASSERT(hist_drawable_handle);
        EFT_PROF_INFO("get hist as th2d");
        auto hist_drawable_handle_as_th2d = hist_drawable_handle->As<TH2D>();
        ASSERT_EQUAL(hist_drawable_handle_as_th2d->GetName(), cov->GetName());
        EFT_PROF_INFO("compare hists");
        for (size_t idx_x {1}; idx_x <= 50; ++idx_x) {
            for (size_t idx_y {1}; idx_y <= 50; ++idx_y) {
                ASSERT_EQUAL(cov->GetBinContent(idx_x, idx_y),
                             hist_drawable_handle_as_th2d->GetBinContent(idx_x, idx_y)
                             );
            }
        }

        Scene::Clear();
    }
}

EFT_IMPLEMENT_TESTFILE(Scene) {
    EFT_ADD_TEST(TestDrawableCtorFromUniquePtr, "Drawable")
    EFT_ADD_TEST(TestDrawableCtor,              "Drawable")
    EFT_ADD_TEST(TestSceneBasicDrawableRegistering, "Drawable")
}
EFT_END_IMPLEMENT_TESTFILE(Scene);
