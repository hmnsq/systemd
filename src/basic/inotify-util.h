/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#அந்தர்பாவ <sys/inotify.h>        /* IWYU pragma: export */
#அந்தர்பாவ <syslog.h>

#அந்தர்பாவ "forward.h"

#define INOTIFY_EVENT_MAX (offsetof(கட்டமைப்பு inotify_event, name) + NAME_MAX + 1)

/* This evaluates arguments multiple times */
#define FOREACH_INOTIFY_EVENT_FULL(e, buffer, sz, log_level)            \
        க்கு (கட்டமைப்பு inotify_event *e = வெற்று;                            \
             inotify_event_next(&buffer, sz, &e, log_level); )

#define FOREACH_INOTIFY_EVENT(e, buffer, sz)                    \
        FOREACH_INOTIFY_EVENT_FULL(e, buffer, sz, LOG_DEBUG)

#define FOREACH_INOTIFY_EVENT_WARN(e, buffer, sz)               \
        FOREACH_INOTIFY_EVENT_FULL(e, buffer, sz, LOG_WARNING)

ஒன்றிப்பு inotify_event_buffer {
        கட்டமைப்பு inotify_event ev;
        uint8_t raw[INOTIFY_EVENT_MAX];
};

bool inotify_event_next(
                ஒன்றிப்பு inotify_event_buffer *buffer,
                size_t size,
                கட்டமைப்பு inotify_event **iterator,
                முழு log_level);

முழு inotify_add_watch_fd(முழு fd, முழு what, uint32_t mask);
முழு inotify_add_watch_and_warn(முழு fd, மாறிலி எழுத்து *pathname, uint32_t mask);
