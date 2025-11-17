#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <TXLib.h>

#define ANIMATION_CYCLES 3
#define NUMBER_OF_FRAMES 3
#define FRAME_DELAY 500
#define MAX_LENGTH_OF_TEXT 1024

bool initialization_graphics();
void close_graphics();
void animate_question(const char* question_text);
bool is_graphics_initialized();

#endif // GRAPHICS_H_
