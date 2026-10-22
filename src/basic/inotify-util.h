/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#অন্তর্ভাব <sys/inotify.h>        /* IWYU pragma: export */
#অন্তর্ভাব <syslog.h>

#অন্তর্ভাব "forward.h"

#define INOTIFY_EVENT_MAX (offsetof(সংরচনা inotify_event, name) + NAME_MAX + 1)

/* This evaluates arguments multiple times */
#define FOREACH_INOTIFY_EVENT_FULL(e, buffer, sz, log_level)            \
        প্রতি (সংরচনা inotify_event *e = রিক্ত;                            \
             inotify_event_next(&buffer, sz, &e, log_level); )

#define FOREACH_INOTIFY_EVENT(e, buffer, sz)                    \
        FOREACH_INOTIFY_EVENT_FULL(e, buffer, sz, LOG_DEBUG)

#define FOREACH_INOTIFY_EVENT_WARN(e, buffer, sz)               \
        FOREACH_INOTIFY_EVENT_FULL(e, buffer, sz, LOG_WARNING)

সঙ্ঘ inotify_event_buffer {
        সংরচনা inotify_event ev;
        uint8_t raw[INOTIFY_EVENT_MAX];
};

bool inotify_event_next(
                সঙ্ঘ inotify_event_buffer *buffer,
                size_t size,
                সংরচনা inotify_event **iterator,
                পূর্ণাঙ্ক log_level);

পূর্ণাঙ্ক inotify_add_watch_fd(পূর্ণাঙ্ক fd, পূর্ণাঙ্ক what, uint32_t mask);
পূর্ণাঙ্ক inotify_add_watch_and_warn(পূর্ণাঙ্ক fd, ধ্রুব বর্ণ *pathname, uint32_t mask);
