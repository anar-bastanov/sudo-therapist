#define PAM_SM_AUTH
#define PAM_SM_ACCOUNT

#include <security/pam_appl.h>
#include <security/pam_modules.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "st_config.h"
#include "st_gate.h"
#include "st_util.h"

static const char *st_mark_key = "sudo-therapist.authed";

static void st_mark_cleanup(pam_handle_t *pamh, void *data, int error_status)
{
    (void)pamh;
    (void)error_status;

    free(data);
}

static void st_mark_set(pam_handle_t *pamh)
{
    int *data = malloc(sizeof *data);

    if (data)
    {
        *data = 1;

        if (pam_set_data(pamh, st_mark_key, data, st_mark_cleanup) != PAM_SUCCESS)
            free(data);
    }
}

static bool st_mark_exists(const pam_handle_t *pamh)
{
    const void *data;
    return pam_get_data(pamh, st_mark_key, &data) == PAM_SUCCESS && data;
}

static bool st_should_apply(const pam_handle_t *pamh)
{
    const void *item;
    if (pam_get_item(pamh, PAM_SERVICE, &item) != PAM_SUCCESS || !item)
        return false;

    const char *service = item;
    return !strcmp(service, "sudo");
}

static const char *st_get_user(pam_handle_t *pamh)
{
    const char *user;
    if (pam_get_user(pamh, &user, NULL) != PAM_SUCCESS)
        return NULL;

    return user;
}

PAM_EXTERN int pam_sm_authenticate(pam_handle_t *pamh, int flags, int argc, const char **argv)
{
    (void)flags;

    if (st_should_apply(pamh))
        st_mark_set(pamh);

    return PAM_SUCCESS;
}

PAM_EXTERN int pam_sm_setcred(pam_handle_t *pamh, int flags, int argc, const char **argv)
{
    (void)pamh;
    (void)flags;
    (void)argc;
    (void)argv;

    return PAM_SUCCESS;
}

PAM_EXTERN int pam_sm_acct_mgmt(pam_handle_t *pamh, int flags, int argc, const char **argv)
{
    (void)flags;

    if (!st_should_apply(pamh))
        return PAM_SUCCESS;

    if (!st_mark_exists(pamh))
        return PAM_SUCCESS;

    st_config_t config;
    st_config_load(&config, argc, argv);

    const char *user = st_get_user(pamh);

    if (!st_user_in_group(user, config.group))
        return PAM_SUCCESS;

    return st_gate_run(pamh, &config);
}

#ifdef PAM_MODULE_ENTRY
PAM_MODULE_ENTRY("pam_sudo_therapist");
#endif
