#ifndef GRAPHICS_H_
#define GRAPHICS_H_

#include <TXLib.h>

#define ANIMATION_CYCLES 1
#define NUMBER_OF_FRAMES 5
#define FRAME_DELAY 500
#define MAX_LENGTH_OF_TEXT 1024

enum game_state
{
    STATE_MAIN_MENU  = 0,
    STATE_PLAYING    = 1,
    STATE_SHOW_TREE  = 2,
    STATE_DEFINITION = 3,
    STATE_COMPARISON = 4,
    STATE_SAVING     = 5
};


extern bool graphics_initialized;
extern HDC background_frames[NUMBER_OF_FRAMES];

void set_game_state_background(int state);
bool create_main_window(int width, int height);
bool load_background_frames();
bool initialization_graphics();
void close_graphics();
void animate_question(const char* question_text);
void show_background(int frame_index);
void show_text(const char* text);

#endif // GRAPHICS_H_
