#include <TXLib.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "graphics.h"

static bool graphics_initialized = false;

bool initialization_graphics()
{
    if (graphics_initialized) return true;

    printf("Initializing graphics system...\n");

    // Создаем окно
    txCreateWindow(800, 600);
    txSleep(200);

    if (txGetExtentX() == 0 || txGetExtentY() == 0)
    {
        printf("ERROR: Window creation failed!\n");
        return false;
    }

    printf("Window created successfully: %dx%d\n", txGetExtentX(), txGetExtentY());

    // Рисуем начальный экран
    txSetFillColor(RGB(30, 30, 60));
    txClear();
    txSetColor(RGB(200, 200, 255), 3);
    txSelectFont("Arial", 48, 20, FW_BOLD);
    txSetTextAlign(TA_CENTER);
    txTextOut(400, 250, "AKINATOR");
    txRedrawWindow();

    graphics_initialized = true;
    printf("Graphics system initialized successfully\n");
    return true;
}

void close_graphics()
{
    graphics_initialized = false;
    printf("Graphics system closed\n");
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

    // Цвета для анимации
    COLORREF colors[] = {
        RGB(80, 40, 40),   // Темно-красный
        RGB(40, 80, 40),   // Темно-зеленый
        RGB(40, 40, 80),   // Темно-синий
        RGB(80, 80, 40),   // Оливковый
        RGB(80, 40, 80)    // Фиолетовый
    };

    for (int cycle = 0; cycle < ANIMATION_CYCLES; cycle++)
    {
        for (int i = 0; i < NUMBER_OF_FRAMES; i++)
        {
            // Очищаем экран и устанавливаем цвет фона
            txSetFillColor(colors[i]);
            txClear();

            // Рисуем заголовок
            txSetColor(RGB(255, 255, 255), 3);
            txSelectFont("Arial", 36, 15, FW_BOLD);
            txSetTextAlign(TA_CENTER);

            char text[MAX_LENGTH_OF_TEXT] = {};
            sprintf(text, "Frame %d", i + 1);
            txTextOut(400, 250, text);

            // Рисуем панель для текста вопроса
            txSetFillColor(RGB(0, 0, 0));
            txRectangle(0, 520, 800, 600);

            // Выводим текст вопроса
            txSetColor(TX_WHITE);
            txSelectFont("Arial", 20, 10, FW_BOLD);
            txSetTextAlign(TA_CENTER);
            txTextOut(400, 540, question_text);

            // Обновляем окно
            txRedrawWindow();
            txSleep(FRAME_DELAY);
        }
    }

    // После анимации возвращаем исходный фон
    txSetFillColor(RGB(30, 30, 60));
    txClear();
    txSetColor(RGB(200, 200, 255), 3);
    txSelectFont("Arial", 48, 20, FW_BOLD);
    txSetTextAlign(TA_CENTER);
    txTextOut(400, 250, "AKINATOR");
    txRedrawWindow();
}

bool is_graphics_initialized()
{
    return graphics_initialized;
}
