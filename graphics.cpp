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

    printf("Creating main window...\n");
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

    // Загружаем ваши 3 картинки
    HDC frames[] = {
        txLoadImage("frame1.bmp"),
        txLoadImage("frame2.bmp"),
        txLoadImage("frame3.bmp")
    };

    // Проверяем, что картинки загрузились
    bool images_loaded = true;
    for (int i = 0; i < NUMBER_OF_FRAMES; i++)
    {
        if (frames[i] == NULL)
        {
            printf("ERROR: Failed to load frame%d.bmp\n", i + 1);
            images_loaded = false;
        }
    }

    // Если картинки не загрузились, используем резервную анимацию
    if (!images_loaded)
    {
        printf("Using backup animation...\n");

        for (int cycle = 0; cycle < ANIMATION_CYCLES; cycle++)
        {
            for (int i = 0; i < NUMBER_OF_FRAMES; i++)
            {
                // Резервная анимация с цветами
                COLORREF colors[] = {RGB(80, 40, 40), RGB(40, 80, 40), RGB(40, 40, 80)};

                txSetFillColor(colors[i]);
                txClear();

                txSetColor(TX_WHITE, 3);
                txSelectFont("Arial", 36, 15, FW_BOLD);
                txSetTextAlign(TA_CENTER);
                txTextOut(400, 250, "AKINATOR");

                // Рисуем панель для текста
                txSetFillColor(RGB(0, 0, 0));
                txRectangle(0, 520, 800, 600);

                // Выводим текст вопроса
                txSetColor(TX_WHITE);
                txSelectFont("Arial", 20, 10, FW_BOLD);
                txSetTextAlign(TA_CENTER);
                txTextOut(400, 540, question_text);

                txRedrawWindow();
                txSleep(FRAME_DELAY);
            }
        }
    }
    else
    {
        // Используем загруженные картинки
        printf("Using custom images for animation...\n");

        for (int cycle = 0; cycle < ANIMATION_CYCLES; cycle++)
        {
            for (int i = 0; i < NUMBER_OF_FRAMES; i++)
            {
                // Очищаем экран
                txSetFillColor(TX_BLACK);
                txClear();

                // Отображаем картинку
                txBitBlt(0, 0, frames[i]);

                // Рисуем полупрозрачную панель для текста
                txSetFillColor(RGB(0, 0, 0)); // Полупрозрачная черная панель
                txRectangle(0, 520, 800, 600);

                // Выводим текст вопроса
                txSetColor(TX_WHITE);
                txSelectFont("Arial", 20, 10, FW_BOLD);
                txSetTextAlign(TA_CENTER);
                txTextOut(400, 540, question_text);

                txRedrawWindow();
                txSleep(FRAME_DELAY);
            }
        }

        // Освобождаем память от загруженных картинок
        for (int i = 0; i < NUMBER_OF_FRAMES; i++)
        {
            if (frames[i] != NULL)
            {
                txDeleteDC(frames[i]);
            }
        }
    }

    // Возвращаем исходный экран
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
