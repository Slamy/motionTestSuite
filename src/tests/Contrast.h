/*
 * Contrast.h
 *
 *  Created on: 14.06.2020
 *      Author: andre
 */

#ifndef SRC_TESTS_CONTRAST_H_
#define SRC_TESTS_CONTRAST_H_

#include "MotionTest.h"

class Contrast : public MotionTest
{
  public:
	virtual void draw();
	virtual ~Contrast() = default;
};
#endif /* SRC_TESTS_CONTRAST_H_ */
