// This file is part of VSTGUI. It is subject to the license terms
// in the LICENSE file found in the top-level directory of this
// distribution and at http://github.com/steinbergmedia/vstgui/LICENSE

#include "../../../lib/cpoint.h"
#include "../unittests.h"

namespace VSTGUI {

TEST_CASE (CPointTest, Unequal)
{
	EXPECT (CPoint (0, 0) != CPoint (1, 1));
	EXPECT (CPoint (0, 0) != CPoint (0, 1));
	EXPECT (CPoint (0, 0) != CPoint (1, 0));
}

TEST_CASE (CPointTest, Equal)
{
	EXPECT (CPoint (0, 0) == CPoint (0, 0));
}

TEST_CASE (CPointTest, OperatorAddAssign)
{
	CPoint p (1, 1);
	p += CPoint (1, 1);
	EXPECT (p == CPoint (2, 2));
}

TEST_CASE (CPointTest, OperatorSubtractAssign)
{
	CPoint p (2, 2);
	p -= CPoint (1, 1);
	EXPECT (p == CPoint (1, 1));
}

TEST_CASE (CPointTest, OperatorAdd)
{
	CPoint p (2, 2);
	auto p2 = p + CPoint (1, 1);
	EXPECT (p2 == CPoint (3, 3));
	EXPECT (p == CPoint (2, 2));
}

TEST_CASE (CPointTest, OperatorSubtract)
{
	CPoint p (2, 2);
	auto p2 = p - CPoint (1, 1);
	EXPECT (p2 == CPoint (1, 1));
	EXPECT (p == CPoint (2, 2));
}

TEST_CASE (CPointTest, OperatorInverse)
{
	CPoint p (2, 2);
	auto p2 = -p;
	EXPECT (p2 == CPoint (-2, -2));
	EXPECT (p == CPoint (2, 2));
}

TEST_CASE (CPointTest, OffsetCoords)
{
	CPoint p (1, 2);
	p.offset (1, 2);
	EXPECT (p == CPoint (2, 4));
}

TEST_CASE (CPointTest, OffsetPoint)
{
	CPoint p (1, 2);
	p.offset (CPoint (2, 3));
	EXPECT (p == CPoint (3, 5));
}

TEST_CASE (CPointTest, OffsetInverse)
{
	CPoint p (5, 3);
	p.offsetInverse (CPoint (2, 1));
	EXPECT (p == CPoint (3, 2));
}

TEST_CASE (CPointTest, MakeIntegral)
{
	CPoint p (5.3, 4.2);
	p.makeIntegral ();
	EXPECT (p == CPoint (5, 4));
	p (5.5, 4.5);
	p.makeIntegral ();
	EXPECT (p == CPoint (6, 5));
	p (5.9, 4.1);
	p.makeIntegral ();
	EXPECT (p == CPoint (6, 4));
	p (5.1, 4.501);
	p.makeIntegral ();
	EXPECT (p == CPoint (5, 5));
}

} // VSTGUI
