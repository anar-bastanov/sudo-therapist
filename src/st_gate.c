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

#define MAX_LINE_LENGTH 1024

static const struct pam_conv *st_get_conv(pam_handle_t *pamh)
{
    const void *item;

    if (pam_get_item(pamh, PAM_CONV, &item) != PAM_SUCCESS)
        return NULL;

    const struct pam_conv *conv = item;
    return conv && conv->conv ? conv : NULL;
}

static void st_format_message(char *out_line, st_message_t message, const st_config_t *config)
{
    char tag_fg_set[ST_MAX_ANSI_COLOR_CODE_LENGTH], tag_fg_reset[ST_MAX_ANSI_COLOR_CODE_LENGTH];
    char tag_bg_set[ST_MAX_ANSI_COLOR_CODE_LENGTH], tag_bg_reset[ST_MAX_ANSI_COLOR_CODE_LENGTH];
    char text_fg_set[ST_MAX_ANSI_COLOR_CODE_LENGTH], text_fg_reset[ST_MAX_ANSI_COLOR_CODE_LENGTH];
    char text_bg_set[ST_MAX_ANSI_COLOR_CODE_LENGTH], text_bg_reset[ST_MAX_ANSI_COLOR_CODE_LENGTH];
    st_color_to_ansi_set_code(config->tag_color, false, tag_fg_set);
    st_color_to_ansi_reset_code(config->tag_color, false, tag_fg_reset);
    st_color_to_ansi_set_code(config->tag_background, true, tag_bg_set);
    st_color_to_ansi_reset_code(config->tag_background, true, tag_bg_reset);
    st_color_to_ansi_set_code(config->text_color, false, text_fg_set);
    st_color_to_ansi_reset_code(config->text_color, false, text_fg_reset);
    st_color_to_ansi_set_code(config->text_background, true, text_bg_set);
    st_color_to_ansi_reset_code(config->text_background, true, text_bg_reset);

    snprintf(out_line, MAX_LINE_LENGTH, "%s%s[therapist]%s%s%s%s %s%s%s",
        tag_fg_set, tag_bg_set,
        tag_fg_reset, tag_bg_reset,
        text_fg_set, text_bg_set,
        message.line,
        text_fg_reset, text_bg_reset);
}

int st_gate_run(pam_handle_t *pamh, const st_config_t *config)
{
    (void)config;

    const struct pam_conv *conv = st_get_conv(pamh);

    if (!conv)
        return PAM_IGNORE;

    char line[MAX_LINE_LENGTH];
    int message_i = st_rand_u32() % st_message_count;
    st_message_t message = st_messages[message_i];
    st_format_message(line, message, config);

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
