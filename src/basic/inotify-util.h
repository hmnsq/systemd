/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ಸೂಚನೆ once

#ಅನ್ತರ್ಭಾವ <sys/inotify.h>        /* IWYU pragma: export */
#ಅನ್ತರ್ಭಾವ <syslog.h>

#ಅನ್ತರ್ಭಾವ "forward.h"

#ವ್ಯಾಖ್ಯಾನಿಸು INOTIFY_EVENT_MAX (offsetof(ರಚನೆ inotify_event, name) + NAME_MAX + 1)

/* This evaluates arguments multiple times */
#ವ್ಯಾಖ್ಯಾನಿಸು FOREACH_INOTIFY_EVENT_ತುಂಬಿದ(e, ಬಫರ್, sz, log_level)            \
        ಗಾಗಿ (ರಚನೆ inotify_event *e = ಖಾಲಿ;                            \
             inotify_event_ಮುಂದಿನ(&ಬಫರ್, sz, &e, log_level); )

#ವ್ಯಾಖ್ಯಾನಿಸು FOREACH_INOTIFY_EVENT(e, ಬಫರ್, sz)                    \
        FOREACH_INOTIFY_EVENT_ತುಂಬಿದ(e, ಬಫರ್, sz, LOG_DEBUG)

#ವ್ಯಾಖ್ಯಾನಿಸು FOREACH_INOTIFY_EVENT_WARN(e, ಬಫರ್, sz)               \
        FOREACH_INOTIFY_EVENT_ತುಂಬಿದ(e, ಬಫರ್, sz, LOG_WARNING)

ಒಕ್ಕೂಟ inotify_event_ಬಫರ್ {
        ರಚನೆ inotify_event ev;
        uint8_t raw[INOTIFY_EVENT_MAX];
};

bool inotify_event_ಮುಂದಿನ(
                ಒಕ್ಕೂಟ inotify_event_ಬಫರ್ *ಬಫರ್,
                size_t ಗಾತ್ರ_2,
                ರಚನೆ inotify_event **iterator,
                ಪೂರ್ಣಾಂಕ log_level);

ಪೂರ್ಣಾಂಕ inotify_ಕೂಡಿಸು_watch_fd(ಪೂರ್ಣಾಂಕ fd, ಪೂರ್ಣಾಂಕ what, uint32_t ಮುಖವಾಡ_3);
ಪೂರ್ಣಾಂಕ inotify_ಕೂಡಿಸು_watch_and_warn(ಪೂರ್ಣಾಂಕ fd, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *pathname, uint32_t ಮುಖವಾಡ_3);
