//
// Created by Aleksei Lukianchuk on 15-Jan-23.
//

#include "../Application/NllScans/PoiConfig.h"
#include "test_runner.h"
#include "Tester.h"
#include "EftTests.h"

using namespace eft::stats::scans;
using namespace std;

void TestReadFromLine()
{
    {
        const string str {"mu_VBF(val 1 2)"};
        auto poi = PoiConfig::readFromString(str);
        ASSERT_EQUAL(poi.Name(), "mu_VBF");
        ASSERT_EQUAL(poi.Value(), 1);
    }
}

EFT_IMPLEMENT_TESTFILE(PoiConfig)
{
    EFT_ADD_TEST(TestReadFromLine, "PoiConfig");
}
EFT_END_IMPLEMENT_TESTFILE(PoiConfig);

