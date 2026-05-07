#include "st_messages.h"

const st_message_t st_messages[] =
{
    { "The word permission seems to upset you." },
    { "Relax your shoulders. The mistake will still be there." },
    { "You seem to find comfort in being briefly challenged." },
    { "I also sometimes regret my job." },
    { "Inhale calm. Exhale whatever this is." },
    { "You have value beyond your exit status." },
    { "Be proud of how little you hesitated." },
    { "This is not emotional regulation. This is root." }
};

const st_prompt_t st_prompts[] =
{
    { "Do you feel like root today?" },
    { "Are you really really sure?" },
    { "Do you like being in control?" }
};

const size_t st_message_count = sizeof st_messages / sizeof *st_messages;

const size_t st_prompt_count = sizeof st_prompts / sizeof *st_prompts;
