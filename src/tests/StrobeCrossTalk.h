/*
 * StrobeCrossTalk.h
 *
 *  Created on: 11.06.2020
 *      Author: andre
 */

#ifndef SRC_TESTS_STROBECROSSTALK_H_
#define SRC_TESTS_STROBECROSSTALK_H_

#include "MotionTest.h"

class StrobeCrossTalk : public MotionTest
{
  public:
	virtual void draw();
	virtual ~StrobeCrossTalk() = default;

	bool MustBeJitterFree() override
	{
		return true;
	}
};

#endif /* SRC_TESTS_STROBECROSSTALK_H_ */
