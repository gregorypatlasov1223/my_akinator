#include <TXLib.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "graphics.h"

static HDC background = NULL;
static HDC frame1 = NULL, frame2 = NULL, frame3 = NULL, frame4 = NULL, frame5 = NULL;
static bool graphics_initialized = false;

bool initialization_graphics()
{
    if (graphics_initialized) return true;

    printf("Initializing graphics system...\n");

    // Создаем окно (только один раз во всей программе)
    if (txGetExtentX() == 0 && txGetExtentY() == 0)
    {
        txCreateWindow(800, 600);
    }

    // Создаем изображения программно
    background = txCreateCompatibleDC(800, 600);
    frame1     = txCreateCompatibleDC(800, 600);
    frame2     = txCreateCompatibleDC(800, 600);
    frame3     = txCreateCompatibleDC(800, 600);
    frame4     = txCreateCompatibleDC(800, 600);
    frame5     = txCreateCompatibleDC(800, 600);

    // Фон
    txSetFillColor(RGB(30, 30, 60));
    txRectangle(0, 0, 800, 600, background);

    // Текст на фоне
    txSetColor(RGB(200, 200, 255));
    txSelectFont("Arial", 48, 20, FW_BOLD);
    txSetTextAlign(TA_CENTER);
    txTextOut(400, 250, "AKINATOR", background);

    // Кадры анимации
    HDC frames[] = {frame1, frame2, frame3, frame4, frame5};
    COLORREF colors[] = {
        RGB(80, 40, 40),   // Темно-красный
        RGB(40, 80, 40),   // Темно-зеленый
        RGB(40, 40, 80),   // Темно-синий
        RGB(80, 80, 40),   // Оливковый
        RGB(80, 40, 80)    // Фиолетовый
    };

    for (int i = 0; i < NUMBER_OF_FRAMES; i++)
    {
        txSetFillColor(colors[i]);
        txRectangle(0, 0, 800, 600, frames[i]);

        txSetColor(RGB(255, 255, 255));
        txSelectFont("Arial", 36, 15, FW_BOLD);
        txSetTextAlign(TA_CENTER);

        char text[MAX_LENGTH_OF_TEXT] = {};
        sprintf(text, "Frame %d", i + 1);
        txTextOut(400, 250, text, frames[i]);

        txSetTextAlign(TA_LEFT);
    }

    graphics_initialized = true;
    printf("Graphics system initialized successfully\n");
    return true;
}

void close_graphics()
{
    if (background) txDeleteDC(background);
    if (frame1) txDeleteDC(frame1);
    if (frame2) txDeleteDC(frame2);
    if (frame3) txDeleteDC(frame3);
    if (frame4) txDeleteDC(frame4);
    if (frame5) txDeleteDC(frame5);

    background = NULL;
    frame1 = NULL;
    frame2 = NULL;
    frame3 = NULL;
    frame4 = NULL;
    frame5 = NULL;

    graphics_initialized = false;

    printf("Graphics system closed\n");
}

void animate_question(const char* question_text)
{
    if (!graphics_initialized)
    {
        init_graphics();
    }

    HDC frames[] = {frame1, frame2, frame3, frame4, frame5};

    for (int cycle = 0; cycle < ANIMATION_CYCLES; cycle++)
    {
        for (int i = 0; i < NUMBER_OF_FRAMES; i++)
        {
            txSetFillColor(TX_BLACK);
            txClear();

            // Отображаем кадр анимации
            if (frames[i] != NULL)
            {
                txBitBlt(txDC(), 0, 0, 800, 600, frames[i], 0, 0);
            }

            // Рисуем панель для текста
            txSetFillColor(RGB(0, 0, 0));
            txRectangle(0, 520, 800, 600);

            // Выводим текст вопроса
            txSetColor(TX_WHITE);
            txSelectFont("Arial", 20, 10, FW_BOLD);
            txSetTextAlign(TA_CENTER);
            txTextOut(400, 540, question_text);
            txSetTextAlign(TA_LEFT);

            // Обновляем окно
            txRedrawWindow();
            txSleep(FRAME_DELAY);
        }
    }
}
