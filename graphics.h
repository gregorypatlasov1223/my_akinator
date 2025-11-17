#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <TXLib.h>

#define ANIMATION_CYCLES 2
#define NUMBER_OF_FRAMES 5
#define NUMBER_OF_IMAGES 5
#define FRAME_DELAY 100
#define MAX_LENGTH_OF_TEXT 1024

bool initialization_graphics();
void close_graphics();
void animate_question(const char* question_text);

#endif // GRAPHICS_H_
