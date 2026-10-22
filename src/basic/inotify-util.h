/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#include <sys/inotify.h>        /* IWYU pragma: export */
#include <syslog.h>

#include "forward.h"

#define INOTIFY_EVENT_MAX (offsetof(sa_nracanaa inotify_event, name) + NAME_MAX + 1)

/* This evaluates arguments multiple times */
#define FOREACH_INOTIFY_EVENT_FULL(e, buffer, sz, log_level)            \
        សម្រាប់ (sa_nracanaa inotify_event *e = NULL;                            \
             inotify_event_next(&buffer, sz, &e, log_level); )

#define FOREACH_INOTIFY_EVENT(e, buffer, sz)                    \
        FOREACH_INOTIFY_EVENT_FULL(e, buffer, sz, LOG_DEBUG)

#define FOREACH_INOTIFY_EVENT_WARN(e, buffer, sz)               \
        FOREACH_INOTIFY_EVENT_FULL(e, buffer, sz, LOG_WARNING)

sa_ngha inotify_event_buffer {
        sa_nracanaa inotify_event ev;
        uint8_t raw[INOTIFY_EVENT_MAX];
};

bool inotify_event_next(
                sa_ngha inotify_event_buffer *buffer,
                size_t size,
                sa_nracanaa inotify_event **iterator,
                ចំនួនគត់ log_level);

ចំនួនគត់ inotify_add_watch_fd(ចំនួនគត់ fd, ចំនួនគត់ what, uint32_t mask);
ចំនួនគត់ inotify_add_watch_and_warn(ចំនួនគត់ fd, const var_na *pathname, uint32_t mask);
