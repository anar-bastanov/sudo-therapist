#include <security/pam_modules.h>
#include <signal.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>

#include "st_config.h"
#include "st_gate.h"
#include "st_messages.h"
#include "st_util.h"

#include <security/pam_appl.h>
#include <security/pam_modules.h>

static volatile sig_atomic_t got_sigint = 0;

static void handle_sigint(int sig)
{
    got_sigint = 1;
}

static const struct pam_conv *st_get_conv(pam_handle_t *pamh)
{
    const void *item;

    if (pam_get_item(pamh, PAM_CONV, &item) != PAM_SUCCESS)
        return NULL;

    const struct pam_conv *conv = item;
    return conv && conv->conv ? conv : NULL;
}

static void st_fail()
{
    /*
        We use `_exit(1)` here because sudo ignores return values such as
        `PAM_PERM_DENIED`/`PAM_AUTH_ERR` and insists on printing its own
        error messages instead of terminating silently. Though normally
        a bad practice, this ensures a clean exit for the user.
    */
    _exit(1);
}

int st_gate_run(pam_handle_t *pamh, const st_config_t *config)
{
    const struct pam_conv *conv = st_get_conv(pamh);

    if (!conv)
        return PAM_IGNORE;

    struct pam_message pam_message;
    const struct pam_message *ptr_pam_message = &pam_message;
    struct pam_response *pam_response = NULL;
    char line[256];

    if (!config->prompt_chance || st_rand_u32() % config->prompt_chance)
    {
        int message_i = st_rand_u32() % st_message_count;
        st_message_t message = st_messages[message_i];
        snprintf(line, sizeof line, "[therapist] %s", message.line);

        pam_message.msg_style = PAM_TEXT_INFO;
        pam_message.msg = line;
        conv->conv(1, &ptr_pam_message, &pam_response, conv->appdata_ptr);

        goto out;
    }

    int prompt_i = st_rand_u32() % st_prompt_count;
    st_prompt_t prompt = st_prompts[prompt_i];
    snprintf(line, sizeof line, "[therapist] %s: ", prompt.line);

    void (*old_int_handler)(int) = signal(SIGINT, handle_sigint);

    struct pam_message pam_prompt;
    pam_prompt.msg_style = PAM_PROMPT_ECHO_ON;
    pam_prompt.msg = line;

    const struct pam_message *ptr_pam_prompt = &pam_prompt;
    int rc = conv->conv(1, &ptr_pam_prompt, &pam_response, conv->appdata_ptr);

    signal(SIGINT, old_int_handler);

    if (rc != PAM_SUCCESS || !pam_response || !pam_response->resp)
    {
        if (!got_sigint)
            goto out;

        pam_message.msg_style = PAM_ERROR_MSG;
        pam_message.msg = "[therapist] cancelled";
        conv->conv(1, &ptr_pam_message, &pam_response, conv->appdata_ptr);

        st_fail();
    }

    if (strcasecmp(pam_response->resp, "yes"))
    {
        pam_message.msg_style = PAM_ERROR_MSG;
        pam_message.msg = "[therapist] wrong";
        conv->conv(1, &ptr_pam_message, &pam_response, conv->appdata_ptr);

        st_fail();
    }

    pam_message.msg_style = PAM_TEXT_INFO;
    pam_message.msg = "[therapist] correct";
    conv->conv(1, &ptr_pam_message, &pam_response, conv->appdata_ptr);

out:
    if (pam_response)
    {
        if (pam_response->resp)
            free(pam_response->resp);
        free(pam_response);
    }

    return PAM_SUCCESS;
}
