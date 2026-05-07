#include <ctype.h>
#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "st_config.h"
#include "st_util.h"

static void st_parse_uint(const char *value, int *out)
{
    if (!value || !*value)
        return;

    errno = 0;
    char *end = NULL;
    long res = strtol(value, &end, 10);

    if (errno)
        return;

    while (*end && isspace((unsigned char)*end))
        ++end;

    if (*end || res < 0 || res > INT_MAX)
        return;

    *out = (int)res;
}

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

static void st_apply_kv(st_config_t *config, const char *key, const char *value)
{
    if (!strcmp(key, "conf"))
        st_set_str(config->config_path, sizeof config->config_path, value);
    else if (!strcmp(key, "group"))
        st_set_str(config->group, sizeof config->group, value);
    else if (!strcmp(key, "prompt_chance"))
        st_parse_uint(value, &config->prompt_chance);
}

static void st_config_defaults(st_config_t *config)
{
    snprintf(config->config_path, sizeof config->config_path, "/etc/sudo-gambit.conf");

    config->group[0] = 0;

    config->prompt_chance = 5;
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
