/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#ضمن <sys/inotify.h>        /* IWYU pragma: export */
#ضمن <syslog.h>

#ضمن "forward.h"

#define INOTIFY_EVENT_MAX (offsetof(بنية inotify_event, name) + NAME_MAX + 1)

/* This evaluates arguments multiple times */
#define FOREACH_INOTIFY_EVENT_FULL(e, buffer, sz, log_level)            \
        עבור (بنية inotify_event *e = عدم;                            \
             inotify_event_next(&buffer, sz, &e, log_level); )

#define FOREACH_INOTIFY_EVENT(e, buffer, sz)                    \
        FOREACH_INOTIFY_EVENT_FULL(e, buffer, sz, LOG_DEBUG)

#define FOREACH_INOTIFY_EVENT_WARN(e, buffer, sz)               \
        FOREACH_INOTIFY_EVENT_FULL(e, buffer, sz, LOG_WARNING)

اتحاد inotify_event_buffer {
        بنية inotify_event ev;
        uint8_t raw[INOTIFY_EVENT_MAX];
};

bool inotify_event_next(
                اتحاد inotify_event_buffer *buffer,
                size_t size,
                بنية inotify_event **iterator,
                صحيح log_level);

صحيح inotify_add_watch_fd(صحيح fd, صحيح what, uint32_t mask);
صحيح inotify_add_watch_and_warn(صحيح fd, ثابت حرف *pathname, uint32_t mask);
