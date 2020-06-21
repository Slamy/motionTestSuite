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
	GridPattern(bool focus);

	virtual void screenResized()
	{
		calculateGridSize();
	};

	void setColor(float r, float g, float b)
	{
		red	  = r;
		green = g;
		blue  = b;
	}

  private:
	void calculateGridSize();
	void convergenceGrid();
	void focusGrid();

	int gridCellSize;
	int cellsHorizontal;
	int cellsVertical;
	int offsetX;
	int offsetY;

	bool focus;

	float red	= 1;
	float green = 1;
	float blue	= 1;
};

#endif /* SRC_TESTS_GRIDPATTERN_H_ */
