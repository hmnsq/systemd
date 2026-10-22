/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#include <sys/inotify.h>        /* IWYU pragma: export */
#include <syslog.h>

#include "forward.h"

#define INOTIFY_EVENT_MAX (offsetof(struct inotify_event, name) + NAME_MAX + 1)

/* This evaluates arguments multiple times */
#define FOREACH_INOTIFY_EVENT_FULL(e, buffer, sz, log_level)            \
        для (struct inotify_event *e = NULL;                            \
             inotify_event_next(&buffer, sz, &e, log_level); )

#define FOREACH_INOTIFY_EVENT(e, buffer, sz)                    \
        FOREACH_INOTIFY_EVENT_FULL(e, buffer, sz, LOG_DEBUG)

#define FOREACH_INOTIFY_EVENT_WARN(e, buffer, sz)               \
        FOREACH_INOTIFY_EVENT_FULL(e, buffer, sz, LOG_WARNING)

union inotify_event_buffer {
        struct inotify_event ev;
        uint8_t raw[INOTIFY_EVENT_MAX];
};

bool inotify_event_next(
                union inotify_event_buffer *buffer,
                size_t size,
                struct inotify_event **iterator,
                ціле log_level);

ціле inotify_add_watch_fd(ціле fd, ціле what, uint32_t mask);
ціле inotify_add_watch_and_warn(ціле fd, const char *pathname, uint32_t mask);
