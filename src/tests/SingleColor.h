/*
 * SingleColor.h
 *
 *  Created on: 14.06.2020
 *      Author: andre
 */

#ifndef SRC_TESTS_SINGLECOLOR_H_
#define SRC_TESTS_SINGLECOLOR_H_

#include "MotionTest.h"

class SingleColor : public MotionTest
{
  public:
	virtual void draw();
	virtual ~SingleColor() = default;
	SingleColor(float r, float g, float b)
	{
		red	  = r;
		green = g;
		blue  = b;
	}

  private:
	float red	= 0;
	float green = 0;
	float blue	= 0;
};

#endif /* SRC_TESTS_SINGLECOLOR_H_ */
