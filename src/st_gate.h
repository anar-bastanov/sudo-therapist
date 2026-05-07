#pragma once

#include <security/pam_appl.h>

#include "st_config.h"

int st_gate_run(pam_handle_t *pamh, const st_config_t *config);
