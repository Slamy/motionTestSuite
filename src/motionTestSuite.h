/*
 * motionTestSuite.h
 *
 *  Created on: 11.06.2020
 *      Author: andre
 */

#ifndef SRC_MOTIONTESTSUITE_H_
#define SRC_MOTIONTESTSUITE_H_

#include <SDL2/SDL_ttf.h>

extern int screen_width;
extern int screen_height;

extern int pixels_per_frame;
extern int pixels_per_second;
extern int frame_cnt;

extern double frames_per_second;
extern int out_of_sync_cnt;

extern int moving_target_width;
extern int moving_target_height;

extern TTF_Font* font;
void drawUfo(int x, int y);
void drawText(int x, int y, const char* str);

#endif /* SRC_MOTIONTESTSUITE_H_ */
