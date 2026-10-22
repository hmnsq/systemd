/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#അന്തര്ഭാവ <sys/inotify.h>        /* IWYU pragma: export */
#അന്തര്ഭാവ <syslog.h>

#അന്തര്ഭാവ "forward.h"

#define INOTIFY_EVENT_MAX (offsetof(ഘടന inotify_event, name) + NAME_MAX + 1)

/* This evaluates arguments multiple times */
#define FOREACH_INOTIFY_EVENT_FULL(e, buffer, sz, log_level)            \
        വേണ്ടി (ഘടന inotify_event *e = ശൂന്യ;                            \
             inotify_event_next(&buffer, sz, &e, log_level); )

#define FOREACH_INOTIFY_EVENT(e, buffer, sz)                    \
        FOREACH_INOTIFY_EVENT_FULL(e, buffer, sz, LOG_DEBUG)

#define FOREACH_INOTIFY_EVENT_WARN(e, buffer, sz)               \
        FOREACH_INOTIFY_EVENT_FULL(e, buffer, sz, LOG_WARNING)

സംഘം inotify_event_buffer {
        ഘടന inotify_event ev;
        uint8_t raw[INOTIFY_EVENT_MAX];
};

bool inotify_event_next(
                സംഘം inotify_event_buffer *buffer,
                size_t size,
                ഘടന inotify_event **iterator,
                പൂർണ്ണസംഖ്യ log_level);

പൂർണ്ണസംഖ്യ inotify_add_watch_fd(പൂർണ്ണസംഖ്യ fd, പൂർണ്ണസംഖ്യ what, uint32_t mask);
പൂർണ്ണസംഖ്യ inotify_add_watch_and_warn(പൂർണ്ണസംഖ്യ fd, സ്ഥിരം അക്ഷരം *pathname, uint32_t mask);
