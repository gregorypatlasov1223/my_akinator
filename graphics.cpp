#include <TXLib.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "graphics.h"

bool graphics_initialized = false;
HDC background_frames[NUMBER_OF_FRAMES] = {NULL, NULL, NULL, NULL, NULL};


void set_game_state_background(int state)
{
    if (!graphics_initialized) return;

    switch(state)
    {
        case STATE_MAIN_MENU:
            show_background(0);
            show_text("Main Menu - Choose an option");
            break;

        case STATE_PLAYING:
            show_background(1);
            show_text("Game in progress - Think of something!");
            break;

        case STATE_SHOW_TREE:
            show_background(2);
            show_text("Showing tree structure");
            break;

        case STATE_DEFINITION:
            show_background(3);
            show_text("Finding object definition");
            break;

        case STATE_COMPARISON:
            show_background(4);
            show_text("Comparing two objects");
            break;

        case STATE_SAVING:
            show_background(0);
            show_text("Saving database...");
            break;

        default:
            show_background(0);
            show_text("Akinator Game");
            break;
    }
}


bool create_main_window(int width, int height)
{
    printf("Creating main window...\n");
    txCreateWindow(width, height);
    txSleep(FRAME_DELAY);

    if (txGetExtentX() == 0 || txGetExtentY() == 0)
    {
        printf("ERROR: Window creation failed!\n");
        return false;
    }

    printf("Window created successfully: %dx%d\n", txGetExtentX(), txGetExtentY());

    return true;
}


bool load_background_frames()
{
    background_frames[0] = txLoadImage("frame1.bmp");
    background_frames[1] = txLoadImage("frame2.bmp");
    background_frames[2] = txLoadImage("frame3.bmp");
    background_frames[3] = txLoadImage("frame4.bmp");
    background_frames[4] = txLoadImage("frame5.bmp");

    bool all_loaded = true;
    for (int i = 0; i < NUMBER_OF_FRAMES; i++)
    {
        if (background_frames[i] == NULL)
        {
            printf("ERROR: Failed to load frame%d.bmp\n", i + 1);
            all_loaded = false;
        }
    }
    set_game_state_background(STATE_MAIN_MENU);

    return all_loaded;
}


bool initialization_graphics()
{
    if (graphics_initialized) return true;

    if (!create_main_window(800, 600)) return false;

    if(!load_background_frames())
        printf("Some background frames failed to load\n");

    graphics_initialized = true;

    printf("Graphics system initialized successfully\n");
    return true;
}


void close_graphics()
{
    for (int i = 0; i < NUMBER_OF_FRAMES; i++)
    {
        if (background_frames[i] != NULL)
        {
            txDeleteDC(background_frames[i]);
            background_frames[i] = NULL;
        }
    }

    graphics_initialized = false;
    printf("Graphics system closed\n");
}


void show_background(int frame_index)
{
    if (!graphics_initialized) return;

    if (frame_index >= 0 && frame_index < NUMBER_OF_FRAMES && background_frames[frame_index] != NULL)
    {
        txSetFillColor(RGB(0, 0, 0));
        txClear();
        txBitBlt(0, 0, background_frames[frame_index]);
        txRedrawWindow(); // гарантируем смену фона
    }
}


void show_text(const char* text)
{
    if (!graphics_initialized) return;

    // панель для текста
    txSetFillColor(TX_BLACK);
    txRectangle(0, 520, 800, 600);

    // Выводим текст
    txSetColor(TX_WHITE);
    txSelectFont("Arial", 20, 10, FW_BOLD);
    txSetTextAlign(TA_CENTER);
    txTextOut(400, 540, text);

    txRedrawWindow();
}


void animate_question(const char* question_text)
{
    if (!graphics_initialized)
    {
        if (!initialization_graphics())
        {
            printf("ERROR: Failed to initialize graphics for animation!\n");
            return;
        }
    }

    for (int cycle = 0; cycle < ANIMATION_CYCLES; cycle++)
    {
        for (int i = 0; i < NUMBER_OF_FRAMES; i++)
        {
            show_background(i);

            show_text(question_text);

            txSleep(FRAME_DELAY);
        }
    }

    set_game_state_background(STATE_MAIN_MENU);
}

