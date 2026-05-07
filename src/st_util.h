#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

bool st_user_in_group(const char *user, const char *group);

void st_trim(char *s);

uint32_t st_rand_u32(void);
