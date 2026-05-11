#pragma once

#include "st_color.h"

typedef struct st_config
{
    char config_path[512];
    char group[256];
    st_color_t tag_color;
    st_color_t tag_background;
    st_color_t text_color;
    st_color_t text_background;
} st_config_t;

void st_config_load(st_config_t *config, int argc, const char **argv);
