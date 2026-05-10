#pragma once

#include <stddef.h>

typedef struct st_message
{
    const char *line;
} st_message_t;

extern const st_message_t st_messages[];

extern const size_t st_message_count;
