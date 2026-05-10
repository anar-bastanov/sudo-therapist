#pragma once

typedef struct st_config
{
    char config_path[512];
    char group[512];
} st_config_t;

void st_config_load(st_config_t *config, int argc, const char **argv);
