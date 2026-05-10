#include <security/pam_modules.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "st_config.h"
#include "st_gate.h"
#include "st_messages.h"
#include "st_util.h"

#include <security/pam_appl.h>
#include <security/pam_modules.h>

static const struct pam_conv *st_get_conv(pam_handle_t *pamh)
{
    const void *item;

    if (pam_get_item(pamh, PAM_CONV, &item) != PAM_SUCCESS)
        return NULL;

    const struct pam_conv *conv = item;
    return conv && conv->conv ? conv : NULL;
}

int st_gate_run(pam_handle_t *pamh, const st_config_t *config)
{
    (void)config;

    const struct pam_conv *conv = st_get_conv(pamh);

    if (!conv)
        return PAM_IGNORE;

    char line[256];
    int message_i = st_rand_u32() % st_message_count;
    st_message_t message = st_messages[message_i];
    snprintf(line, sizeof line, "[therapist] %s", message.line);

    struct pam_message pam_message =
    {
        .msg_style = PAM_TEXT_INFO,
        .msg = line
    };
    const struct pam_message *ptr_pam_message = &pam_message;
    struct pam_response *pam_response = NULL;
    conv->conv(1, &ptr_pam_message, &pam_response, conv->appdata_ptr);

    if (pam_response)
    {
        if (pam_response->resp)
            free(pam_response->resp);
        free(pam_response);
    }

    return PAM_SUCCESS;
}
