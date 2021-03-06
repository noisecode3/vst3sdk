// This file is part of VSTGUI. It is subject to the license terms
// in the LICENSE file found in the top-level directory of this
// distribution and at http://github.com/steinbergmedia/vstgui/LICENSE

#include "../../../../lib/controls/cxypad.h"
#include "../../../../lib/cstring.h"
#include "../../../../uidescription/detail/uiviewcreatorattributes.h"
#include "../../../../uidescription/uiattributes.h"
#include "../../../../uidescription/uiviewfactory.h"
#include "../../unittests.h"
#include "helpers.h"

namespace VSTGUI {
using namespace UIViewCreator;

TEST_CASE (CXYPadCreatorTest, Create)
{
	DummyUIDescription uidesc;
	UIViewFactory factory;
	UIAttributes a;
	a.setAttribute (kAttrClass, kCXYPad);

	auto view = owned (factory.createView (a, &uidesc));
	auto control = view.cast<CXYPad> ();
	EXPECT (control);
	UIAttributes a2;
	EXPECT (factory.getAttributesForView (view, &uidesc, a2));
}

} // VSTGUI
