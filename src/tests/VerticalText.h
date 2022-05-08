/*
 * VerticalText.h
 *
 *  Created on: 11.06.2020
 *      Author: andre
 */

#ifndef SRC_TESTS_VERTICALTEXT_H_
#define SRC_TESTS_VERTICALTEXT_H_

#include <GL/gl.h>
#include <GL/glu.h>

#include "MotionTest.h"

class VerticalText : public MotionTest
{
  public:
	void draw() override;
	VerticalText();
	virtual ~VerticalText();

	bool MustBeJitterFree() override
	{
		return true;
	}

  private:
	SDL_Surface* surfaceMessage = nullptr;
	GLuint texture_text;
};

#endif /* SRC_TESTS_VERTICALTEXT_H_ */
