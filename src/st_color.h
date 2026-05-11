#pragma once

#include <stdbool.h>
#include <stdint.h>

#define ST_MAX_ANSI_COLOR_CODE_LENGTH 64

typedef struct st_color
{
    enum
    {
        ST_COLOR_DEFAULT = 0,
        ST_COLOR_BASIC,
        ST_COLOR_INDEXED,
        ST_COLOR_RGB
    } type;
    union
    {
        struct
        {
            int code;
        };
        struct
        {
            uint8_t index;
        };
        struct
        {
            uint8_t r;
            uint8_t g;
            uint8_t b;
        };
    };
} st_color_t;

typedef struct st_basic_color_kv
{
    const char *name;
    int code;
} st_basic_color_kv_t;

extern const st_basic_color_kv_t st_basic_color_map[];

extern const size_t st_basic_color_count;

extern void st_color_to_ansi_set_code(st_color_t color, bool is_background, char out[ST_MAX_ANSI_COLOR_CODE_LENGTH]);

extern void st_color_to_ansi_reset_code(st_color_t color, bool is_background, char out[ST_MAX_ANSI_COLOR_CODE_LENGTH]);
