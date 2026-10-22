/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#ಅನ್ತರ್ಭಾವ <sys/inotify.h>        /* IWYU pragma: export */
#ಅನ್ತರ್ಭಾವ <syslog.h>

#ಅನ್ತರ್ಭಾವ "forward.h"

#define INOTIFY_EVENT_MAX (offsetof(ಸಂರಚನಾ inotify_event, name) + NAME_MAX + 1)

/* This evaluates arguments multiple times */
#define FOREACH_INOTIFY_EVENT_FULL(e, buffer, sz, log_level)            \
        ಪ್ರತಿ (ಸಂರಚನಾ inotify_event *e = ರಿಕ್ತ;                            \
             inotify_event_next(&buffer, sz, &e, log_level); )

#define FOREACH_INOTIFY_EVENT(e, buffer, sz)                    \
        FOREACH_INOTIFY_EVENT_FULL(e, buffer, sz, LOG_DEBUG)

#define FOREACH_INOTIFY_EVENT_WARN(e, buffer, sz)               \
        FOREACH_INOTIFY_EVENT_FULL(e, buffer, sz, LOG_WARNING)

ಸಙ್ಘ inotify_event_buffer {
        ಸಂರಚನಾ inotify_event ev;
        uint8_t raw[INOTIFY_EVENT_MAX];
};

bool inotify_event_next(
                ಸಙ್ಘ inotify_event_buffer *buffer,
                size_t size,
                ಸಂರಚನಾ inotify_event **iterator,
                ಪೂರ್ಣಾಙ್ಕ log_level);

ಪೂರ್ಣಾಙ್ಕ inotify_add_watch_fd(ಪೂರ್ಣಾಙ್ಕ fd, ಪೂರ್ಣಾಙ್ಕ what, uint32_t mask);
ಪೂರ್ಣಾಙ್ಕ inotify_add_watch_and_warn(ಪೂರ್ಣಾಙ್ಕ fd, ಧ್ರುವ ವರ್ಣ *pathname, uint32_t mask);
