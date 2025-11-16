#define TX_USE_SPEAK
#include <TXLib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <assert.h>

#include "speech.h"

#define MAX_LENGTH_OF_ANSWER 256

void speak_print_with_variable_number_of_parameters(const char* format, ...)
{
    assert(format != NULL);

    if (format[0] == '\0')
        return;

    char buffer[MAX_LENGTH_OF_ANSWER];
    va_list args;

    va_start(args, format);
    int length = vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);

    if (length <= 0 || buffer[0] == '\0')
        return;

    char final_buffer[MAX_LENGTH_OF_ANSWER];
    snprintf(final_buffer, sizeof(final_buffer), "\v%s", buffer);

    txSpeak(final_buffer);
}
