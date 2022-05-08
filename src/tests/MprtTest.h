/*
 * MprtTest.h
 *
 *  Created on: 15.06.2020
 *      Author: andre
 */

#ifndef SRC_TESTS_MPRTTEST_H_
#define SRC_TESTS_MPRTTEST_H_

#include "MotionTest.h"

class MprtTest : public MotionTest
{
  public:
	virtual void draw();
	virtual ~MprtTest() = default;

	bool MustBeJitterFree() override
	{
		return true;
	}

	void alterCheckerBoardSize(int incDec)
	{
		checkerBoardSize += incDec;
		if (checkerBoardSize < 1)
			checkerBoardSize = 1;
	};
	void alterThickness(int incDec)
	{
		thickness += incDec;
		if (thickness < 1)
			thickness = 1;
	};
	void alterPixelsPerFrame(int incDec)
	{
		pixelsPerFrame += incDec;
		if (pixelsPerFrame < 0)
			pixelsPerFrame = 0;
	};

  private:
	int checkerBoardSize = 12;
	int thickness		 = 2;
	int pixelsPerFrame	 = 9;
};

#endif /* SRC_TESTS_MPRTTEST_H_ */
