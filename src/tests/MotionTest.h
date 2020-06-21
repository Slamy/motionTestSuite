/*
 * MotionTest.h
 *
 *  Created on: 11.06.2020
 *      Author: andre
 */

#ifndef SRC_TESTS_MOTIONTEST_H_
#define SRC_TESTS_MOTIONTEST_H_

#include "motionTestSuite.h"

class MotionTest
{
  public:
	virtual void screenResized(){};
	virtual void draw()	  = 0;
	virtual ~MotionTest() = default;
};

#endif /* SRC_TESTS_MOTIONTEST_H_ */
