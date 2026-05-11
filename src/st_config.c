#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "st_color.h"
#include "st_config.h"
#include "st_util.h"

static void st_set_str(char *dst, size_t dstsz, const char *src)
{
    if (!dst || !dstsz)
        return;

    if (!src)
    {
        dst[0] = 0;
        return;
    }

    snprintf(dst, dstsz, "%s", src);
}

static void st_parse_color(const char *value, st_color_t *out)
{
    out->type = ST_COLOR_DEFAULT;

    if (!value || !*value)
        return;

    if (*value == '#')
    {
        char *endptr;
        unsigned long hex = strtoul(value + 1, &endptr, 16);

        if (!*endptr && endptr - value == 7)
        {
            out->type = ST_COLOR_RGB;
            out->r = (uint8_t)((hex >> 16) & 0xFF);
            out->g = (uint8_t)((hex >>  8) & 0xFF);
            out->b = (uint8_t)((hex >>  0) & 0xFF);
        }

        return;
    }

    for (int i = 0; i < st_basic_color_count; ++i)
    {
        if (strcasecmp(value, st_basic_color_map[i].name))
            continue;

        out->type = ST_COLOR_BASIC;
        out->code = st_basic_color_map[i].code;
        return;
    }

    char *next;
    long v1 = strtol(value, &next, 10);

    if (next == value)
        return;

    if (!*next)
    {
        if (v1 >= 0 && v1 <= 255)
        {
            out->type = ST_COLOR_INDEXED;
            out->index = (uint8_t)v1;
        }

        return;
    }

    if (*next == ',' || *next == ';')
    {
        char *p2 = next + 1;
        long v2 = strtol(p2, &next, 10);

        if (next != p2 && (*next == ',' || *next == ';'))
        {
            char *p3 = next + 1;
            long v3 = strtol(p3, &next, 10);

            if (next != p3 && !*next)
            {
                if (v1 >= 0 && v1 <= 255 && v2 >= 0 && v2 <= 255 && v3 >= 0 && v3 <= 255)
                {
                    out->type = ST_COLOR_RGB;
                    out->r = (uint8_t)v1;
                    out->g = (uint8_t)v2;
                    out->b = (uint8_t)v3;
                }
            }
        }
    }
}

static void st_apply_kv(st_config_t *config, const char *key, const char *value)
{
    if (!strcmp(key, "conf"))
        st_set_str(config->config_path, sizeof config->config_path, value);
    else if (!strcmp(key, "group"))
        st_set_str(config->group, sizeof config->group, value);
    else if (!strcmp(key, "tag_color"))
        st_parse_color(value, &config->tag_color);
    else if (!strcmp(key, "tag_background"))
        st_parse_color(value, &config->tag_background);
    else if (!strcmp(key, "text_color"))
        st_parse_color(value, &config->text_color);
    else if (!strcmp(key, "text_background"))
        st_parse_color(value, &config->text_background);
}

static void st_config_defaults(st_config_t *config)
{
    memset(config, 0, sizeof config);

    snprintf(config->config_path, sizeof config->config_path, "/etc/sudo-gambit.conf");
    config->group[0] = 0;
    config->tag_color.type = ST_COLOR_DEFAULT;
    config->tag_background.type = ST_COLOR_DEFAULT;
    config->text_color.type = ST_COLOR_DEFAULT;
    config->text_background.type = ST_COLOR_DEFAULT;
}

static void st_config_load_file(st_config_t *config, const char *path)
{
    FILE *f = fopen(path, "r");

    if (!f)
        return;

    char line[512];

    while (fgets(line, (int)sizeof line, f))
    {
        st_trim(line);

        if (!*line || *line == '#')
            continue;

        char *eq = strchr(line, '=');
        if (!eq)
            continue;

        char *key = line;
        char *value = eq + 1;

        *eq = 0;
        st_trim(key);
        st_trim(value);

        if (*key)
            st_apply_kv(config, key, value);
    }

    fclose(f);
}

static void st_config_apply_argv(st_config_t *config, int argc, const char **argv)
{
    for (int i = 0; i < argc; ++i)
    {
        const char *arg = argv[i];
        if (!argv[i])
            continue;

        const char *eq = strchr(arg, '=');
        if (!eq)
            continue;

        char *key = strdup(arg);
        char *value = key + (eq - arg + 1);

        value[-1] = 0;
        st_trim(key);
        st_trim(value);

        if (*key)
            st_apply_kv(config, key, value);

        free(key);
    }
}

static void st_config_validate(st_config_t *config)
{
    // restrict values of the parameters here
}

void st_config_load(st_config_t *config, int argc, const char **argv)
{
    st_config_defaults(config);

    st_config_apply_argv(config, argc, argv);

    st_config_load_file(config, config->config_path);
    st_config_apply_argv(config, argc, argv);

    st_config_validate(config);
}
