//
// Created by Aleksei Lukianchuk on 15-Jan-23.
//

#include "nlohmann/json.hpp"
#include "../Application/NllScans/PoiConfig.h"
#include "test_runner.h"
#include "Tester.h"
#include "EftTests.h"
#include "Logger.h"

using namespace eft::stats::scans;
using namespace std;

void TestReadCentralValueFromLine()
{
    {
        eft::stats::Logger::SetFullPrinting();
        const string str {"mu_VBF(val 1)"};
        ASSERT_THROW(PoiConfig::readFromString(str), std::logic_error);
    }
    {
        const string str {"mu_VBF(val 1 1)"};
        auto poi = PoiConfig::readFromString(str);
        ASSERT_EQUAL(poi.Name(), "mu_VBF");
        ASSERT_EQUAL(poi.CentralValue(), 1);
    }
    {
        const string str {"mu_VBF(val -1 1)"};
        auto poi = PoiConfig::readFromString(str);
        ASSERT_EQUAL(poi.Name(), "mu_VBF");
        ASSERT_EQUAL(poi.CentralValue(), -1);
    }
    {
        const string str {"mu_VBF( val 1 25)"};
        auto poi = PoiConfig::readFromString(str);
        ASSERT_EQUAL(poi.Name(), "mu_VBF");
        ASSERT_EQUAL(poi.CentralValue(), 1);
    }
    {
        const string str {" mu_VBF ( val  1 0.1    )"};
        auto poi = PoiConfig::readFromString(str);
        ASSERT_EQUAL(poi.Name(), "mu_VBF");
        ASSERT_EQUAL(poi.CentralValue(), 1);
    }
    {
        const string str {" mu_VBF ( val  1    2)"};
        auto poi = PoiConfig::readFromString(str);
        ASSERT_EQUAL(poi.Name(), "mu_VBF");
        ASSERT_EQUAL(poi.CentralValue(), 1);
        ASSERT_EQUAL(poi.CentralError(), 2);
    }
    {
        const string str {"mu_VBF (val 1 0.1)"};
        auto poi = PoiConfig::readFromString(str);
        ASSERT_EQUAL(poi.Name(), "mu_VBF");
        ASSERT_EQUAL(poi.CentralValue(), 1.f);
        ASSERT_EQUAL(poi.CentralError(), 0.1f);
    }
    {
        const string str {"mu_VBF (val 1 0.1 )"};
        auto poi = PoiConfig::readFromString(str);
        ASSERT_EQUAL(poi.Name(), "mu_VBF");
        ASSERT_EQUAL(poi.CentralValue(), 1.f);
        ASSERT_EQUAL(poi.CentralError(), 0.1f);
    }
    {
        const string str {"    mu_VBF     (    val 1         0.1)"};
        auto poi = PoiConfig::readFromString(str);
        ASSERT_EQUAL(poi.Name(), "mu_VBF");
        ASSERT_EQUAL(poi.CentralValue(), 1.f);
        ASSERT_EQUAL(poi.CentralError(), 0.1f);
    }
    {
        const string str {"mu_VBF (val 1 1 )"};
        auto poi = PoiConfig::readFromString(str);
        ASSERT_EQUAL(poi.Name(), "mu_VBF");
        ASSERT_EQUAL(poi.CentralValue(), 1.f);
        ASSERT_EQUAL(poi.CentralError(), 1.f);
    }
    {
        const string str {"mu_VBF (val 1 )"};
        auto poi = PoiConfig::readFromString(str);
        ASSERT_EQUAL(poi.Name(), "mu_VBF");
        ASSERT_EQUAL(poi.CentralValue(), 1.f);
        ASSERT_EQUAL(poi.CentralError(), 0.f);
    }
    {
        const string str {"mu_VBF (val 1 :)"};
        auto poi = PoiConfig::readFromString(str);
        ASSERT_EQUAL(poi.Name(), "mu_VBF");
        ASSERT_EQUAL(poi.CentralValue(), 1);
        ASSERT_EQUAL(poi.CentralError(), 0);
    }
    {
        const string str {"mu_VBF (val 1 : )"};
        auto poi = PoiConfig::readFromString(str);
        ASSERT_EQUAL(poi.Name(), "mu_VBF");
        ASSERT_EQUAL(poi.CentralValue(), 1);
        ASSERT_EQUAL(poi.CentralError(), 0);
    }
}

void TestReadGridFromLine() {
    {
        eft::stats::Logger::SetFullPrinting();
        const string str {"mu_VBF (val 1 0.1 )"};
        auto poi = PoiConfig::readFromString(str);
        ASSERT_EQUAL(poi.Name(), "mu_VBF");
        ASSERT_EQUAL(poi.CentralValue(), 1.f);
        ASSERT_EQUAL(poi.CentralError(), 0.1f);
        ASSERT_EQUAL(poi.GridSize(), 1u);
    }
    {
        const string str {"mu_VBF (val 1 0.1 : grid 100)"};
        auto poi = PoiConfig::readFromString(str);
        ASSERT_EQUAL(poi.Name(), "mu_VBF");
        ASSERT_EQUAL(poi.CentralValue(), 1.f);
        ASSERT_EQUAL(poi.CentralError(), 0.1f);
        ASSERT_EQUAL(poi.GridSize(), 100u);
    }
    {
        const string str {"mu_VBF (val 1 0.1 : grid 100 )"};
        auto poi = PoiConfig::readFromString(str);
        ASSERT_EQUAL(poi.Name(), "mu_VBF");
        ASSERT_EQUAL(poi.CentralValue(), 1.f);
        ASSERT_EQUAL(poi.CentralError(), 0.1f);
        ASSERT_EQUAL(poi.GridSize(), 100u);
    }
    {
        const string str {"mu_VBF (val 1 0.1 : grid 100 : )"};
        auto poi = PoiConfig::readFromString(str);
        ASSERT_EQUAL(poi.Name(), "mu_VBF");
        ASSERT_EQUAL(poi.CentralValue(), 1.f);
        ASSERT_EQUAL(poi.CentralError(), 0.1f);
        ASSERT_EQUAL(poi.GridSize(), 100u);
    }
}

void TestReadRangeFromLine() {
    eft::stats::Logger::SetFullPrinting();
    {
        const string str {"mu_VBF (val 1 0.1 : grid 100 : range -1 1 )"};
        auto poi = PoiConfig::readFromString(str);
        ASSERT_EQUAL(poi.Name(), "mu_VBF");
        ASSERT_EQUAL(poi.CentralValue(), 1.f);
        ASSERT_EQUAL(poi.CentralError(), 0.1f);
        ASSERT_EQUAL(poi.GridSize(), 100u);

        ASSERT_EQUAL(poi.ScanRangeHigh(), 1.f);
        ASSERT_EQUAL(poi.ScanRangeLow(), -1.f);
    }
    {
        const string str {"mu_VBF (val 1 0.1 : grid 100 : range -1 1)"};
        auto poi = PoiConfig::readFromString(str);
        ASSERT_EQUAL(poi.Name(), "mu_VBF");
        ASSERT_EQUAL(poi.CentralValue(), 1.f);
        ASSERT_EQUAL(poi.CentralError(), 0.1f);
        ASSERT_EQUAL(poi.GridSize(), 100u);

        ASSERT_EQUAL(poi.ScanRangeHigh(), 1.f);
        ASSERT_EQUAL(poi.ScanRangeLow(), -1.f);
    }
    {
        const string str {"mu_VBF (val 1 0.1 : range -1 1)"};
        auto poi = PoiConfig::readFromString(str);
        ASSERT_EQUAL(poi.Name(), "mu_VBF");
        ASSERT_EQUAL(poi.CentralValue(), 1.f);
        ASSERT_EQUAL(poi.CentralError(), 0.1f);

        ASSERT_EQUAL(poi.ScanRangeHigh(), 1.f);
        ASSERT_EQUAL(poi.ScanRangeLow(), -1.f);
    }
    {
        const string str {"mu_VBF (val 1 0.1 : range 1s 1)"};
        auto poi = PoiConfig::readFromString(str);
        ASSERT_EQUAL(poi.Name(), "mu_VBF");
        ASSERT_EQUAL(poi.CentralValue(), 1.f);
        ASSERT_EQUAL(poi.CentralError(), 0.1f);

        ASSERT_EQUAL(poi.ScanRangeHigh(), 1.f);
        ASSERT_EQUAL(poi.ScanRangeLow(),  0.9f);
        ASSERT(poi.IsRangeInSigmas());
    }
    {
        const string str {"mu_VBF (val 1 0.1 : range 1s 1s)"};
        auto poi = PoiConfig::readFromString(str);
        ASSERT_EQUAL(poi.Name(), "mu_VBF");
        ASSERT_EQUAL(poi.CentralValue(), 1.f);
        ASSERT_EQUAL(poi.CentralError(), 0.1f);

        ASSERT_EQUAL(poi.ScanRangeHigh(), 1.1f);
        ASSERT_EQUAL(poi.ScanRangeLow(),  0.9f);
        ASSERT(poi.IsRangeInSigmas());
    }
    {
        const string str {"mu_VBF (val 1 0.1 : range 1 1s)"};
        auto poi = PoiConfig::readFromString(str);
        ASSERT_EQUAL(poi.Name(), "mu_VBF");
        ASSERT_EQUAL(poi.CentralValue(), 1.f);
        ASSERT_EQUAL(poi.CentralError(), 0.1f);

        ASSERT_EQUAL(poi.ScanRangeHigh(), 1.1f);
        ASSERT_EQUAL(poi.ScanRangeLow(),  1.0f);
        ASSERT(poi.IsRangeInSigmas());
    }
    {
        const string str {"mu_VBF (val 1 0.1 : range 5s 5s)"};
        auto poi = PoiConfig::readFromString(str);
        ASSERT_EQUAL(poi.Name(), "mu_VBF");
        ASSERT_EQUAL(poi.CentralValue(), 1.f);
        ASSERT_EQUAL(poi.CentralError(), 0.1f);

        ASSERT_EQUAL(poi.ScanRangeHigh(), 0.5f);
        ASSERT_EQUAL(poi.ScanRangeLow(),  1.5f);
        ASSERT(poi.IsRangeInSigmas());
    }
    {
        const string str {"mu_VBF (val 1 0.1 : range 5s 5s  )"};
        auto poi = PoiConfig::readFromString(str);
        ASSERT_EQUAL(poi.Name(), "mu_VBF");
        ASSERT_EQUAL(poi.CentralValue(), 1.f);
        ASSERT_EQUAL(poi.CentralError(), 0.1f);

        ASSERT_EQUAL(poi.ScanRangeHigh(), 0.5f);
        ASSERT_EQUAL(poi.ScanRangeLow(),  1.5f);
        ASSERT(poi.IsRangeInSigmas());
    }
    {
        const string str {"mu_VBF (val 1 0.1 : range -5s 5s  )"};
        auto poi = PoiConfig::readFromString(str);
        ASSERT_EQUAL(poi.Name(), "mu_VBF");
        ASSERT_EQUAL(poi.CentralValue(), 1.f);
        ASSERT_EQUAL(poi.CentralError(), 0.1f);

        ASSERT_EQUAL(poi.ScanRangeHigh(), 0.5f);
        ASSERT_EQUAL(poi.ScanRangeLow(),  1.5f);
        ASSERT(poi.IsRangeInSigmas());
    }
    {
        const string str {"mu_VBF (val 1 0.1 : range -5s -5s  )"};
        auto poi = PoiConfig::readFromString(str);
        ASSERT_EQUAL(poi.Name(), "mu_VBF");
        ASSERT_EQUAL(poi.CentralValue(), 1.f);
        ASSERT_EQUAL(poi.CentralError(), 0.1f);

        ASSERT_EQUAL(poi.ScanRangeHigh(), 0.5f);
        ASSERT_EQUAL(poi.ScanRangeLow(),  1.5f);
        ASSERT(poi.IsRangeInSigmas());
    }
    {
        const string str {"mu_VBF (val 1 0.1 : range -5 5  )"};
        auto poi = PoiConfig::readFromString(str);
        ASSERT_EQUAL(poi.Name(), "mu_VBF");
        ASSERT_EQUAL(poi.CentralValue(), 1.f);
        ASSERT_EQUAL(poi.CentralError(), 0.1f);

        ASSERT_EQUAL(poi.ScanRangeHigh(), -5.f);
        ASSERT_EQUAL(poi.ScanRangeLow(),   5.f);
        ASSERT_NOT(poi.IsRangeInSigmas());
        ASSERT_NOT(poi.IsGridPointKnown());
    }
    {
        const string str {"mu_VBF (val 1 0.1 : range -1 5  )"};
        auto poi = PoiConfig::readFromString(str);
        ASSERT_EQUAL(poi.Name(), "mu_VBF");
        ASSERT_EQUAL(poi.CentralValue(), 1.f);
        ASSERT_EQUAL(poi.CentralError(), 0.1f);

        ASSERT_EQUAL(poi.ScanRangeHigh(), -1.f);
        ASSERT_EQUAL(poi.ScanRangeLow(),   5.f);
        ASSERT_NOT(poi.IsRangeInSigmas());
        ASSERT_NOT(poi.IsGridPointKnown());
    }
    {
        const string str {"mu_VBF (val 1 0.1 : range -1s 2s  )"};
        auto poi = PoiConfig::readFromString(str);
        ASSERT_EQUAL(poi.Name(), "mu_VBF");
        ASSERT_EQUAL(poi.CentralValue(), 1.f);
        ASSERT_EQUAL(poi.CentralError(), 0.1f);

        ASSERT_EQUAL(poi.ScanRangeHigh(), 0.9f);
        ASSERT_EQUAL(poi.ScanRangeLow(),  1.2f);
        ASSERT(poi.IsRangeInSigmas());
        ASSERT_NOT(poi.IsGridPointKnown());
    }
    {
        const string str {"mu_VBF (val 1 0.1 : range -1 2s  )"};
        auto poi = PoiConfig::readFromString(str);
        ASSERT_EQUAL(poi.Name(), "mu_VBF");
        ASSERT_EQUAL(poi.CentralValue(), 1.f);
        ASSERT_EQUAL(poi.CentralError(), 0.1f);

        ASSERT_EQUAL(poi.ScanRangeHigh(), -1.f);
        ASSERT_EQUAL(poi.ScanRangeLow(),   1.2f);
        ASSERT(poi.IsRangeInSigmas());
        ASSERT_NOT(poi.IsGridPointKnown());
    }
    {
        const string str {"mu_VBF (val 1 0.1 : range -1s 2  )"};
        auto poi = PoiConfig::readFromString(str);
        ASSERT_EQUAL(poi.Name(), "mu_VBF");
        ASSERT_EQUAL(poi.CentralValue(), 1.f);
        ASSERT_EQUAL(poi.CentralError(), 0.1f);

        ASSERT_EQUAL(poi.ScanRangeHigh(), 0.9f);
        ASSERT_EQUAL(poi.ScanRangeLow(),  2.0f);
        ASSERT(poi.IsRangeInSigmas());
        ASSERT_NOT(poi.IsGridPointKnown());
    }
    {
        const string str {"mu_VBF (val 1 0.1 : range 2s  )"};
        auto poi = PoiConfig::readFromString(str);
        ASSERT_EQUAL(poi.Name(), "mu_VBF");
        ASSERT_EQUAL(poi.CentralValue(), 1.f);
        ASSERT_EQUAL(poi.CentralError(), 0.1f);

        ASSERT_EQUAL(poi.ScanRangeHigh(), 0.8f);
        ASSERT_EQUAL(poi.ScanRangeLow(),  1.2f);

        ASSERT(poi.IsRangeInSigmas());
        ASSERT_NOT(poi.IsGridPointKnown());
    }
    {
        const string str {"mu_VBF (val 1 0.1:range -1s 2)"};
        auto poi = PoiConfig::readFromString(str);
        ASSERT_EQUAL(poi.Name(), "mu_VBF");
        ASSERT_EQUAL(poi.CentralValue(), 1.f);
        ASSERT_EQUAL(poi.CentralError(), 0.1f);

        ASSERT_EQUAL(poi.ScanRangeHigh(), 0.9f);
        ASSERT_EQUAL(poi.ScanRangeLow(),  2.0f);
        ASSERT(poi.IsRangeInSigmas());
        ASSERT_NOT(poi.IsGridPointKnown());
    }
    {
        const string str {"mu_VBF (val 1 0.1:range 2s)"};
        auto poi = PoiConfig::readFromString(str);
        ASSERT_EQUAL(poi.Name(), "mu_VBF");
        ASSERT_EQUAL(poi.CentralValue(), 1.f);
        ASSERT_EQUAL(poi.CentralError(), 0.1f);

        ASSERT_EQUAL(poi.ScanRangeHigh(), 0.8f);
        ASSERT_EQUAL(poi.ScanRangeLow(),  1.2f);

        ASSERT(poi.IsRangeInSigmas());
        ASSERT_NOT(poi.IsGridPointKnown());
    }
}

void TestReadingValueToTestAt() {
    {
        eft::stats::Logger::SetFullPrinting();
        const string str {"mu_VBF (val 1 0.1 : range -5 5  : at 4.07)"};
        auto poi = PoiConfig::readFromString(str);
        ASSERT_EQUAL(poi.Name(), "mu_VBF");
        ASSERT_EQUAL(poi.CentralValue(), 1.f);
        ASSERT_EQUAL(poi.CentralError(), 0.1f);

        ASSERT_EQUAL(poi.ScanRangeHigh(), -5.f);
        ASSERT_EQUAL(poi.ScanRangeLow(),   5.f);

        ASSERT_EQUAL(poi.Value(),   4.07f);
        ASSERT(poi.IsGridPointKnown());
    }
    {
        const string str {"mu_VBF (at 4.07)"};
        auto poi = PoiConfig::readFromString(str);
        ASSERT_EQUAL(poi.Name(), "mu_VBF");
        ASSERT_EQUAL(poi.Value(),   4.07f);
        ASSERT(poi.IsGridPointKnown());

    }
    {
        const string str {"mu_VBF ()"};
        auto poi = PoiConfig::readFromString(str);
        ASSERT_EQUAL(poi.Name(), "mu_VBF");
        ASSERT_EQUAL(poi.Value(),   4.07f);
        ASSERT_NOT(poi.IsGridPointKnown());
    }
    {
        const string str {"mu_VBF(val 1 0.1 : range -1s 2 : grid 100 : at 1.23)"};
        auto poi = PoiConfig::readFromString(str);
        ASSERT_EQUAL(poi.Name(), "mu_VBF");
        ASSERT_EQUAL(poi.CentralValue(), 1.f);
        ASSERT_EQUAL(poi.CentralError(), 0.1f);

        ASSERT_EQUAL(poi.ScanRangeHigh(), 0.9f);
        ASSERT_EQUAL(poi.ScanRangeLow(),  2.0f);

        ASSERT_EQUAL(poi.GridSize(), 100u);
        ASSERT_EQUAL(poi.Value(), 1.23f);
        ASSERT(poi.IsRangeInSigmas());
        ASSERT_NOT(poi.IsGridPointKnown());
    }
}

void TestSetters() {
        const string name {"my_cool_poi"};
        const string cfg_version {"v1"};
        const float r_low {-1.f};
        const float r_high {2.f};
        const size_t grid_size {300u};
        const float central_val {4.f};
        const float central_err {5.f};

    {
        auto poi = PoiConfig()
                .WithRangeLow(r_low)
                .WithRangeHigh(r_high)
                .WithName(name)
                .WithVersionOfConfig(cfg_version)
                .WithGridSize(grid_size)
                .WithCentralVal(central_val)
                .WithCentralErr(central_err);

        ASSERT_EQUAL(poi.Name(), name);
        ASSERT_EQUAL(poi.ScanRangeHigh(), r_high);
        ASSERT_EQUAL(poi.ScanRangeLow(), r_low);
        ASSERT_EQUAL(poi.GridSize(), grid_size);
        ASSERT_EQUAL(poi.CentralValue(), central_val);
        ASSERT_EQUAL(poi.CentralError(), central_err);
        ASSERT_NOT(poi.IsRangeInSigmas());
        ASSERT_NOT(poi.IsGridPointKnown());
    }
    {
        auto poi = PoiConfig()
                .WithRangeLow(r_low)
                .WithRangeHigh(r_high)
                .WithName(name)
                .WithVersionOfConfig(cfg_version)
                .WithGridSize(grid_size)
                .WithCentralVal(central_val)
                .WithCentralErr(central_err)
                .WithRangeSigmas(2);

        ASSERT_EQUAL(poi.Name(), name);
        ASSERT_EQUAL(poi.ScanRangeHigh(), central_val + 2 * central_err);
        ASSERT_EQUAL(poi.ScanRangeLow(), central_val - 2 * central_err);
        ASSERT_EQUAL(poi.GridSize(), grid_size);
        ASSERT_EQUAL(poi.CentralValue(), central_val);
        ASSERT_EQUAL(poi.CentralError(), central_err);

        ASSERT(poi.IsRangeInSigmas());
        ASSERT_NOT(poi.IsGridPointKnown());
    }
    {
        auto poi = PoiConfig()
                .WithRangeLow(r_low)
                .WithRangeHigh(r_high)
                .WithName(name)
                .WithVersionOfConfig(cfg_version)
                .WithGridSize(grid_size)
                .WithCentralVal(central_val)
                .WithCentralErr(central_err)
                .WithRangeSigmasHigh(2)
                .WithRangeSigmasLow(3);

        ASSERT_EQUAL(poi.Name(), name);
        ASSERT_EQUAL(poi.ScanRangeHigh(), central_val + 2 * central_err);
        ASSERT_EQUAL(poi.ScanRangeLow(),  central_val - 3 * central_err);
        ASSERT_EQUAL(poi.GridSize(), grid_size);
        ASSERT_EQUAL(poi.CentralValue(), central_val);
        ASSERT_EQUAL(poi.CentralError(), central_err);

        ASSERT(poi.IsRangeInSigmas());
        ASSERT_NOT(poi.IsGridPointKnown());
    }
}

void TestJSONIO() {
    {

    }
}

EFT_IMPLEMENT_TESTFILE(PoiConfig)
{
    EFT_ADD_TEST(TestSetters,                   "PoiConfig");
    EFT_ADD_TEST(TestReadCentralValueFromLine,  "PoiConfig");
    EFT_ADD_TEST(TestReadGridFromLine,          "PoiConfig");
    EFT_ADD_TEST(TestReadRangeFromLine,         "PoiConfig");
    EFT_ADD_TEST(TestReadingValueToTestAt,      "PoiConfig");
    EFT_ADD_TEST(TestJSONIO,                    "PoiConfig");
}
EFT_END_IMPLEMENT_TESTFILE(PoiConfig);

