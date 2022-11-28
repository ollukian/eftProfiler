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

void TestDrawableCtor() {
    {
        Drawable d;
        TBox box;
    }
}

EFT_IMPLEMENT_TESTFILE(Scene) {
    EFT_ADD_TEST(TestDrawableCtor,"Scene")
}
EFT_END_IMPLEMENT_TESTFILE(Scene);
