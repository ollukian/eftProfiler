//
// Created by Aleksei Lukianchuk on 21-Nov-22.
//

#include "../Utils/PlotterUtils.h"
#include "test_runner.h"
#include "Tester.h"

using namespace std;
using eft::utils::PlotterUtils;

void TestFormName() {
    {
        auto setting = make_unique<eft::plot::RankingPlotterSettings>();
        setting->poi = "mu";
        setting->top = 2;
        const string expected {"Impact_mu_2_nps"};
        ASSERT_NO_THROW( PlotterUtils::FormName(setting) );
        auto res = PlotterUtils::FormName(setting);
        ASSERT_EQUAL(res, expected);
    }
    {
        auto setting = make_unique<eft::plot::RankingPlotterSettings>();
        setting->poi = "mu";
        setting->top = 2;
        setting->replacements = {{"this", "that"}};
        setting->ignore_name = {"privet"};
        setting->match_names = {"privit"};
        const string expected {"Impact_mu_2_nps__Select_1_patterns__privit__Ignore_1_patterns__privet"};
        ASSERT_NO_THROW( PlotterUtils::FormName(setting) );
        auto res = PlotterUtils::FormName(setting);
        ASSERT_EQUAL(res, expected);
    }
}

void TestGetLabelNormal()
{
    {
        auto setting = make_unique<eft::plot::RankingPlotterSettings>();
        setting->poi = "mu";
        setting->top = 2;
        setting->np_names = {"first", "second", "third"};

        ASSERT_NO_THROW( PlotterUtils::GetLabel(setting, 0, "init_name"));
        ASSERT_NO_THROW( PlotterUtils::GetLabel(setting, 1, "init_name"));
        ASSERT_NO_THROW( PlotterUtils::GetLabel(setting, 2, "init_name"));

        const auto res1 = PlotterUtils::GetLabel(setting, 0, "init_name");
        const auto res2 = PlotterUtils::GetLabel(setting, 1, "init_name");
        const auto res3 = PlotterUtils::GetLabel(setting, 2, "init_name");

        ASSERT_EQUAL("first",   res1);
        ASSERT_EQUAL("second",  res2);
        ASSERT_EQUAL("third",   res3);
    }
}

void TestGetLabelNothingSet()
{
    {
        auto setting = make_unique<eft::plot::RankingPlotterSettings>();
        setting->poi = "mu";
        setting->top = 2;

        ASSERT_NO_THROW( PlotterUtils::GetLabel(setting, 0, "init_name_1"));
        ASSERT_NO_THROW( PlotterUtils::GetLabel(setting, 1, "init_name_2"));
        ASSERT_NO_THROW( PlotterUtils::GetLabel(setting, 2, "init_name_3"));

        const auto res1 = PlotterUtils::GetLabel(setting, 0, "init_name_1");
        const auto res2 = PlotterUtils::GetLabel(setting, 1, "init_name_2");
        const auto res3 = PlotterUtils::GetLabel(setting, 2, "init_name_3");

        ASSERT_EQUAL("init_name_1",   res1);
        ASSERT_EQUAL("init_name_2",  res2);
        ASSERT_EQUAL("init_name_3",   res3);
    }
}

void TestGetLabelRemovePrefix()
{
    {
        auto setting = make_unique<eft::plot::RankingPlotterSettings>();
        setting->poi = "mu";
        setting->top = 2;
        setting->remove_prefix = {"aaa"};

        string init_name_1 {"aaa_bcdef"};
        string init_name_2 {"bcdef"};
        string init_name_3 {"bcdef_aaa"};

        const string expected_1 {"_bcdef"};
        const string expected_2 = init_name_2;
        const string expected_3 = init_name_3;

        ASSERT_NO_THROW( PlotterUtils::GetLabel(setting, 0, init_name_1));
        ASSERT_NO_THROW( PlotterUtils::GetLabel(setting, 1, init_name_2));
        ASSERT_NO_THROW( PlotterUtils::GetLabel(setting, 2, init_name_3));

        const auto res1 = PlotterUtils::GetLabel(setting, 0, init_name_1);
        const auto res2 = PlotterUtils::GetLabel(setting, 1, init_name_2);
        const auto res3 = PlotterUtils::GetLabel(setting, 2, init_name_3);

        ASSERT_EQUAL(expected_1,   res1);
        ASSERT_EQUAL(expected_2,   res2);
        ASSERT_EQUAL(expected_3,   res3);
    }
    { // should be removed nothing
        auto setting = make_unique<eft::plot::RankingPlotterSettings>();
        setting->poi = "mu";
        setting->top = 2;
        setting->remove_prefix = {""};

        string init_name_1 {"aaa_bcdef"};
        string init_name_2 {"bcdef"};
        string init_name_3 {"bcdef_aaa"};

        const string expected_1 = init_name_1;
        const string expected_2 = init_name_2;
        const string expected_3 = init_name_3;

        ASSERT_NO_THROW( PlotterUtils::GetLabel(setting, 0, init_name_1));
        ASSERT_NO_THROW( PlotterUtils::GetLabel(setting, 1, init_name_2));
        ASSERT_NO_THROW( PlotterUtils::GetLabel(setting, 2, init_name_3));

        const auto res1 = PlotterUtils::GetLabel(setting, 0, init_name_1);
        const auto res2 = PlotterUtils::GetLabel(setting, 1, init_name_2);
        const auto res3 = PlotterUtils::GetLabel(setting, 2, init_name_3);

        ASSERT_EQUAL(expected_1,   res1);
        ASSERT_EQUAL(expected_2,   res2);
        ASSERT_EQUAL(expected_3,   res3);
    }
    {
        auto setting = make_unique<eft::plot::RankingPlotterSettings>();
        setting->poi = "mu";
        setting->top = 2;
        setting->remove_prefix = {"a", "bc"};

        string init_name_1 {"abc_this_is_init_bca"};
        string init_name_2 {"nothing_remove"};
        string init_name_3 {"a_only_one_a"};
        string init_name_4 {"bc_only_one_bc"};

        const string expected_1 {"_this_is_init_bca"};
        const string expected_2 {"nothing_remove"};
        const string expected_3 {"_only_one_a"};
        const string expected_4 {"_only_one_bc"};

        ASSERT_NO_THROW( PlotterUtils::GetLabel(setting, 0, init_name_1));
        ASSERT_NO_THROW( PlotterUtils::GetLabel(setting, 1, init_name_2));
        ASSERT_NO_THROW( PlotterUtils::GetLabel(setting, 2, init_name_3));
        ASSERT_NO_THROW( PlotterUtils::GetLabel(setting, 2, init_name_4));

        const auto res1 = PlotterUtils::GetLabel(setting, 0, init_name_1);
        const auto res2 = PlotterUtils::GetLabel(setting, 1, init_name_2);
        const auto res3 = PlotterUtils::GetLabel(setting, 2, init_name_3);
        const auto res4 = PlotterUtils::GetLabel(setting, 3, init_name_4);

        ASSERT_EQUAL(expected_1,   res1);
        ASSERT_EQUAL(expected_2,   res2);
        ASSERT_EQUAL(expected_3,   res3);
        ASSERT_EQUAL(expected_4,   res4);
    }
}

void TestGetLabelRemoveSuffix()
{
    {
        auto setting = make_unique<eft::plot::RankingPlotterSettings>();
        setting->poi = "mu";
        setting->top = 2;
        setting->remove_suffix = {"ef"};

        string init_name_1 {"aaa_bcdef"};
        string init_name_2 {"bcdef"};
        string init_name_3 {"bcdef_aaa"};

        const string expected_1 {"aaa_bcd"};
        const string expected_2 = {"bcd"};
        const string expected_3 = {"bcdef_aaa"};

        ASSERT_NO_THROW( PlotterUtils::GetLabel(setting, 0, init_name_1));
        ASSERT_NO_THROW( PlotterUtils::GetLabel(setting, 1, init_name_2));
        ASSERT_NO_THROW( PlotterUtils::GetLabel(setting, 2, init_name_3));

        const auto res1 = PlotterUtils::GetLabel(setting, 0, init_name_1);
        const auto res2 = PlotterUtils::GetLabel(setting, 1, init_name_2);
        const auto res3 = PlotterUtils::GetLabel(setting, 2, init_name_3);

        ASSERT_EQUAL(expected_1,   res1);
        ASSERT_EQUAL(expected_2,   res2);
        ASSERT_EQUAL(expected_3,   res3);
    }
    { // should be removed nothing
        auto setting = make_unique<eft::plot::RankingPlotterSettings>();
        setting->poi = "mu";
        setting->top = 2;
        setting->remove_suffix = {""};

        string init_name_1 {"aaa_bcdef"};
        string init_name_2 {"bcdef"};
        string init_name_3 {"bcdef_aaa"};

        const string expected_1 = init_name_1;
        const string expected_2 = init_name_2;
        const string expected_3 = init_name_3;

        ASSERT_NO_THROW( PlotterUtils::GetLabel(setting, 0, init_name_1));
        ASSERT_NO_THROW( PlotterUtils::GetLabel(setting, 1, init_name_2));
        ASSERT_NO_THROW( PlotterUtils::GetLabel(setting, 2, init_name_3));

        const auto res1 = PlotterUtils::GetLabel(setting, 0, init_name_1);
        const auto res2 = PlotterUtils::GetLabel(setting, 1, init_name_2);
        const auto res3 = PlotterUtils::GetLabel(setting, 2, init_name_3);

        ASSERT_EQUAL(expected_1,   res1);
        ASSERT_EQUAL(expected_2,   res2);
        ASSERT_EQUAL(expected_3,   res3);
    }
    {
        auto setting = make_unique<eft::plot::RankingPlotterSettings>();
        setting->poi = "mu";
        setting->top = 2;
        setting->remove_suffix = {"a", "bc"};

        string init_name_1 {"this_is_init_bca"};
        string init_name_2 {"nothing_remove"};
        string init_name_3 {"only_one_a"};
        string init_name_4 {"only_one_bc"};

        const string expected_1 {"this_is_init_"};
        const string expected_2 {"nothing_remove"};
        const string expected_3 {"only_one_"};
        const string expected_4 {"only_one_"};

        ASSERT_NO_THROW( PlotterUtils::GetLabel(setting, 0, init_name_1));
        ASSERT_NO_THROW( PlotterUtils::GetLabel(setting, 1, init_name_2));
        ASSERT_NO_THROW( PlotterUtils::GetLabel(setting, 2, init_name_3));
        ASSERT_NO_THROW( PlotterUtils::GetLabel(setting, 2, init_name_4));

        const auto res1 = PlotterUtils::GetLabel(setting, 0, init_name_1);
        const auto res2 = PlotterUtils::GetLabel(setting, 1, init_name_2);
        const auto res3 = PlotterUtils::GetLabel(setting, 2, init_name_3);
        const auto res4 = PlotterUtils::GetLabel(setting, 3, init_name_4);

        ASSERT_EQUAL(expected_1,   res1);
        ASSERT_EQUAL(expected_2,   res2);
        ASSERT_EQUAL(expected_3,   res3);
        ASSERT_EQUAL(expected_4,   res4);
    }
}

EFT_IMPLEMENT_TESTFILE(PlotterUtils) {
        EFT_ADD_TEST(TestFormName,          "PlotterUtils");
        EFT_ADD_TEST(TestGetLabelNormal,    "PlotterUtils");
        EFT_ADD_TEST(TestGetLabelNothingSet,  "PlotterUtils");
        EFT_ADD_TEST(TestGetLabelRemovePrefix,  "PlotterUtils");
        EFT_ADD_TEST(TestGetLabelRemoveSuffix,  "PlotterUtils");

}
EFT_END_IMPLEMENT_TESTFILE(PlotterUtils);