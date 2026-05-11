#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

#include "st_color.h"

const st_basic_color_kv_t st_basic_color_map[] =
{
    { "black",   0 },
    { "red",     1 },
    { "green",   2 },
    { "yellow",  3 },
    { "blue",    4 },
    { "magenta", 5 },
    { "cyan",    6 },
    { "white",   7 },
    { "default", 9 }
};

const size_t st_basic_color_count = sizeof st_basic_color_map / sizeof *st_basic_color_map;

void st_color_to_ansi_set_code(st_color_t color, bool is_background, char out[ST_MAX_ANSI_COLOR_CODE_LENGTH])
{
    char layer = is_background ? '4' : '3';

    switch (color.type)
    {
    case ST_COLOR_BASIC:
        snprintf(out, ST_MAX_ANSI_COLOR_CODE_LENGTH, "\x1b[%c%im", layer, color.code);
        return;
    case ST_COLOR_INDEXED:
        snprintf(out, ST_MAX_ANSI_COLOR_CODE_LENGTH, "\x1b[%c8;5;%im", layer, (int)color.index);
        return;
    case ST_COLOR_RGB:
        snprintf(out, ST_MAX_ANSI_COLOR_CODE_LENGTH, "\x1b[%c8;2;%i;%i;%im", layer, (int)color.r, (int)color.g, (int)color.b);
        return;
    case ST_COLOR_DEFAULT:
    default:
        out[0] = '\0';
        return;
    }
}

void st_color_to_ansi_reset_code(st_color_t color, bool is_background, char out[ST_MAX_ANSI_COLOR_CODE_LENGTH])
{
    if (color.type == ST_COLOR_DEFAULT ||
        (color.type == ST_COLOR_BASIC && color.code == 9))
    {
        out[0] = '\0';
        return;
    }

    char layer = is_background ? '4' : '3';
    snprintf(out, ST_MAX_ANSI_COLOR_CODE_LENGTH, "\x1b[%c9m", layer);
}
