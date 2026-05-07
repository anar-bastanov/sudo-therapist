#pragma once

#include <stddef.h>

typedef struct st_message
{
    const char *line;
} st_message_t;

typedef struct st_prompt
{
    const char *line;
} st_prompt_t;

extern const st_message_t st_messages[];

extern const st_prompt_t st_prompts[];

extern const size_t st_message_count;

extern const size_t st_prompt_count;
