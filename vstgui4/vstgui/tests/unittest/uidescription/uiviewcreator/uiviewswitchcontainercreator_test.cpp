// This file is part of VSTGUI. It is subject to the license terms
// in the LICENSE file found in the top-level directory of this
// distribution and at http://github.com/steinbergmedia/vstgui/LICENSE

#include "../../../../lib/cstring.h"
#include "../../../../uidescription/detail/uiviewcreatorattributes.h"
#include "../../../../uidescription/uiattributes.h"
#include "../../../../uidescription/uiviewfactory.h"
#include "../../../../uidescription/uiviewswitchcontainer.h"
#include "../../unittests.h"
#include "helpers.h"

namespace VSTGUI {
using namespace UIViewCreator;

TEST_CASE (UIViewSwitchContainerCreatorTest, TemplateNames)
{
	DummyUIDescription uidesc;
	testAttribute<UIViewSwitchContainer> (kUIViewSwitchContainer, kAttrTemplateNames, "temp1,temp2",
	                                      &uidesc, [] (UIViewSwitchContainer* v) {
		                                      auto controller =
		                                          dynamic_cast<UIDescriptionViewSwitchController*> (
		                                              v->getController ());
		                                      EXPECT (controller);
		                                      std::string str;
		                                      controller->getTemplateNames (str);
		                                      return str == "temp1,temp2";
	                                      });
}

TEST_CASE (UIViewSwitchContainerCreatorTest, TemplateSwitchControl)
{
	DummyUIDescription uidesc;
	uidesc.tag = 12345;
	testAttribute<UIViewSwitchContainer> (
	    kUIViewSwitchContainer, kAttrTemplateSwitchControl, kTagName, &uidesc,
	    [&] (UIViewSwitchContainer* v) {
		    auto controller =
		        dynamic_cast<UIDescriptionViewSwitchController*> (v->getController ());
		    EXPECT (controller);
		    return controller->getSwitchControlTag () == uidesc.tag;
	    },
	    true);
}

TEST_CASE (UIViewSwitchContainerCreatorTest, AnimationStyle)
{
	DummyUIDescription uidesc;
	testAttribute<UIViewSwitchContainer> (kUIViewSwitchContainer, kAttrAnimationStyle, "fade",
	                                      &uidesc, [] (UIViewSwitchContainer* v) {
		                                      return v->getAnimationStyle () ==
		                                             UIViewSwitchContainer::kFadeInOut;
	                                      });
	testAttribute<UIViewSwitchContainer> (kUIViewSwitchContainer, kAttrAnimationStyle, "move",
	                                      &uidesc, [] (UIViewSwitchContainer* v) {
		                                      return v->getAnimationStyle () ==
		                                             UIViewSwitchContainer::kMoveInOut;
	                                      });
	testAttribute<UIViewSwitchContainer> (kUIViewSwitchContainer, kAttrAnimationStyle, "push",
	                                      &uidesc, [] (UIViewSwitchContainer* v) {
		                                      return v->getAnimationStyle () ==
		                                             UIViewSwitchContainer::kPushInOut;
	                                      });
}

TEST_CASE (UIViewSwitchContainerCreatorTest, AnimationTime)
{
	DummyUIDescription uidesc;
	testAttribute<UIViewSwitchContainer> (
	    kUIViewSwitchContainer, kAttrAnimationTime, 1234, &uidesc,
	    [] (UIViewSwitchContainer* v) { return v->getAnimationTime () == 1234; });
}

TEST_CASE (UIViewSwitchContainerCreatorTest, AnimationStyleValues)
{
	DummyUIDescription uidesc;
	testPossibleValues (kUIViewSwitchContainer, kAttrAnimationStyle, &uidesc,
	                    {"fade", "move", "push"});
}

TEST_CASE (UIViewSwitchContainerCreatorTest, AnimationTimingFunction)
{
	DummyUIDescription uidesc;
	testAttribute<UIViewSwitchContainer> (kUIViewSwitchContainer, kAttrAnimationTimingFunction,
	                                      "linear", &uidesc, [] (UIViewSwitchContainer* v) {
		                                      return v->getTimingFunction () ==
		                                             UIViewSwitchContainer::kLinear;
	                                      });
	testAttribute<UIViewSwitchContainer> (kUIViewSwitchContainer, kAttrAnimationTimingFunction,
	                                      "easy-in", &uidesc, [] (UIViewSwitchContainer* v) {
		                                      return v->getTimingFunction () ==
		                                             UIViewSwitchContainer::kEasyIn;
	                                      });
	testAttribute<UIViewSwitchContainer> (kUIViewSwitchContainer, kAttrAnimationTimingFunction,
	                                      "easy-out", &uidesc, [] (UIViewSwitchContainer* v) {
		                                      return v->getTimingFunction () ==
		                                             UIViewSwitchContainer::kEasyOut;
	                                      });
	testAttribute<UIViewSwitchContainer> (kUIViewSwitchContainer, kAttrAnimationTimingFunction,
	                                      "easy-in-out", &uidesc, [] (UIViewSwitchContainer* v) {
		                                      return v->getTimingFunction () ==
		                                             UIViewSwitchContainer::kEasyInOut;
	                                      });
	testAttribute<UIViewSwitchContainer> (kUIViewSwitchContainer, kAttrAnimationTimingFunction,
	                                      "easy", &uidesc, [] (UIViewSwitchContainer* v) {
		                                      return v->getTimingFunction () ==
		                                             UIViewSwitchContainer::kEasy;
	                                      });
}

TEST_CASE (UIViewSwitchContainerCreatorTest, AnimationTimingFunctionValues)
{
	DummyUIDescription uidesc;
	testPossibleValues (kUIViewSwitchContainer, kAttrAnimationTimingFunction, &uidesc,
	                    {"linear", "easy-in", "easy-out", "easy-in-out", "easy"});
}

} // VSTGUI
