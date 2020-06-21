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
	Contrast(int steps)
	{
		this->steps = steps;
	}
	virtual void draw();
	virtual ~Contrast() = default;
	void drawRect(int y, int rect_height, uint8_t r, uint8_t g, uint8_t b);

  private:
	int steps;
};
#endif /* SRC_TESTS_CONTRAST_H_ */
