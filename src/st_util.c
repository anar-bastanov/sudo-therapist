#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <grp.h>
#include <limits.h>
#include <pwd.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <sys/random.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

#include "st_util.h"

bool st_user_in_group(const char *user, const char *group)
{
    if (!group || !*group)
        return true;

    if (!user || !*user)
        return false;

    const struct group *gr = getgrnam(group);
    if (!gr)
        return false;

    const struct passwd *pw = getpwnam(user);
    if (!pw)
        return false;

    int count = 16;
    gid_t *groups = malloc((size_t)count * sizeof *groups);
    if (!groups)
        return false;

    while (getgrouplist(user, pw->pw_gid, groups, &count) == -1)
    {
        gid_t *tmp = realloc(groups, (size_t)count * sizeof *tmp);
        if (!tmp)
        {
            free(groups);
            return false;
        }

        groups = tmp;
    }

    for (int i = 0; i < count; ++i)
    {
        if (groups[i] == gr->gr_gid)
        {
            free(groups);
            return true;
        }
    }

    free(groups);
    return false;
}

char *st_skip_spaces(const char *s)
{
    while (isspace((unsigned char)*s))
        ++s;

    return (char *)s;
}

void st_trim(char *s)
{
    if (!s)
        return;

    char *p = st_skip_spaces(s);
    size_t n = strlen(p);

    while (n && isspace((unsigned char)p[n - 1]))
        --n;

    if (p != s)
        memmove(s, p, n);

    s[n] = 0;
}

uint32_t st_rand_u32(void)
{
    uint32_t x;
    ssize_t r;

    do
    {
        r = getrandom(&x, sizeof x, 0);
        if (r == (ssize_t)sizeof x)
            return x;
    }
    while (r < 0 && errno == EINTR);

    int fd = open("/dev/urandom", O_RDONLY);
    if (fd >= 0)
    {
        r = read(fd, &x, sizeof x);
        close(fd);

        if (r == (ssize_t)sizeof x)
            return x;
    }

    x = (uint32_t)getpid() ^ (uint32_t)getppid() ^ (uint32_t)time(NULL);
    x ^= x << 13;
    x ^= x >> 17;
    x ^= x << 5;

    return x;
}
