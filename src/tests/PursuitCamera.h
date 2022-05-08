/*
 * PursuitCamera.h
 *
 *  Created on: 11.06.2020
 *      Author: andre
 */

#ifndef SRC_TESTS_PURSUITCAMERA_H_
#define SRC_TESTS_PURSUITCAMERA_H_

#include "MotionTest.h"

class PursuitCamera : public MotionTest
{
  public:
	virtual void draw();
	virtual ~PursuitCamera() = default;

	bool MustBeJitterFree() override
	{
		return true;
	}
};

#endif /* SRC_TESTS_PURSUITCAMERA_H_ */
