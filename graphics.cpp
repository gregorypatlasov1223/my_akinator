#include <TXLib.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "graphics.h"

static bool graphics_initialized = false;
static HDC background_frames[NUMBER_OF_FRAMES] = {NULL, NULL, NULL};

bool initialization_graphics()
{
    if (graphics_initialized) return true;

    printf("Initializing graphics system...\n");

    printf("Creating main window...\n");
    txCreateWindow(800, 600);
    txSleep(200);

    if (txGetExtentX() == 0 || txGetExtentY() == 0)
    {
        printf("ERROR: Window creation failed!\n");
        return false;
    }

    printf("Window created successfully: %dx%d\n", txGetExtentX(), txGetExtentY());

    // Загружаем картинки для фона
    background_frames[0] = txLoadImage("frame1.bmp");
    background_frames[1] = txLoadImage("frame2.bmp");
    background_frames[2] = txLoadImage("frame3.bmp");

    for (int i = 0; i < NUMBER_OF_FRAMES; i++)
    {
        if (background_frames[i] == NULL)
        {
            printf("ERROR: Failed to load frame%d.bmp\n", i + 1);
        }
    }

    // Показываем первую картинку как начальный фон
    show_background(0);

    graphics_initialized = true;
    printf("Graphics system initialized successfully\n");
    return true;
}

void close_graphics()
{
    // Освобождаем память от загруженных картинок
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

// Функция для показа фона
void show_background(int frame_index)
{
    if (!graphics_initialized) return;

    if (frame_index >= 0 && frame_index < NUMBER_OF_FRAMES && background_frames[frame_index] != NULL)
    {
        // Очищаем экран и отображаем картинку
        txSetFillColor(TX_BLACK);
        txClear();
        txBitBlt(0, 0, background_frames[frame_index]);
        txRedrawWindow();
    }
}

// Функция для показа текста поверх фона
void show_text(const char* text)
{
    if (!graphics_initialized) return;

    // Рисуем панель для текста
    txSetFillColor(RGB(0, 0, 0));
    txRectangle(0, 520, 800, 600);

    // Выводим текст
    txSetColor(TX_WHITE);
    txSelectFont("Arial", 20, 10, FW_BOLD);
    txSetTextAlign(TA_CENTER);
    txTextOut(400, 540, text);

    txRedrawWindow();
}


void set_game_state_background(int state)
{
    if (!graphics_initialized) return;

    switch(state)
    {
        case STATE_MAIN_MENU:
            show_background(0);  // Основной фон для меню
            show_text("Main Menu - Choose an option");
            break;

        case STATE_PLAYING:
            show_background(1);  // Фон для игры
            show_text("Game in progress - Think of something!");
            break;

        case STATE_SHOW_TREE:
            show_background(2);  // Фон для показа дерева
            show_text("Showing tree structure");
            break;

        case STATE_DEFINITION:
            show_background(1);  // Фон для определения объекта
            show_text("Finding object definition");
            break;

        case STATE_COMPARISON:
            show_background(2);  // Фон для сравнения объектов
            show_text("Comparing two objects");
            break;

        case STATE_SAVING:
            show_background(0);  // Фон для сохранения
            show_text("Saving database...");
            break;

        default:
            show_background(0);  // Фон по умолчанию
            show_text("Akinator Game");
            break;
    }
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

    printf("Starting animation with custom images...\n");

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

