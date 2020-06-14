/*
 * GridPattern.h
 *
 *  Created on: 14.06.2020
 *      Author: andre
 */

#ifndef SRC_TESTS_GRIDPATTERN_H_
#define SRC_TESTS_GRIDPATTERN_H_

#include "MotionTest.h"

class GridPattern : public MotionTest
{
  public:
	virtual void draw();
	virtual ~GridPattern() = default;
	GridPattern();

  private:
	int gridCellSize;
	int cellsHorizontal;
	int cellsVertical;
	int offsetX;
	int offsetY;
};

#endif /* SRC_TESTS_GRIDPATTERN_H_ */
