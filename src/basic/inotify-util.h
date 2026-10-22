/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#شامل <sys/inotify.h>        /* IWYU pragma: export */
#شامل <syslog.h>

#شامل "forward.h"

#define INOTIFY_EVENT_MAX (offsetof(ساختار inotify_event, name) + NAME_MAX + 1)

/* This evaluates arguments multiple times */
#define FOREACH_INOTIFY_EVENT_FULL(e, buffer, sz, log_level)            \
        برای (ساختار inotify_event *e = پوچ;                            \
             inotify_event_next(&buffer, sz, &e, log_level); )

#define FOREACH_INOTIFY_EVENT(e, buffer, sz)                    \
        FOREACH_INOTIFY_EVENT_FULL(e, buffer, sz, LOG_DEBUG)

#define FOREACH_INOTIFY_EVENT_WARN(e, buffer, sz)               \
        FOREACH_INOTIFY_EVENT_FULL(e, buffer, sz, LOG_WARNING)

اجتماع inotify_event_buffer {
        ساختار inotify_event ev;
        uint8_t raw[INOTIFY_EVENT_MAX];
};

bool inotify_event_next(
                اجتماع inotify_event_buffer *buffer,
                size_t size,
                ساختار inotify_event **iterator,
                صحیح log_level);

صحیح inotify_add_watch_fd(صحیح fd, صحیح what, uint32_t mask);
صحیح inotify_add_watch_and_warn(صحیح fd, ثابت نویسه *pathname, uint32_t mask);
