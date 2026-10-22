/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#ଅନ୍ତର୍ଭାଵ <sys/inotify.h>        /* IWYU pragma: export */
#ଅନ୍ତର୍ଭାଵ <syslog.h>

#ଅନ୍ତର୍ଭାଵ "forward.h"

#define INOTIFY_EVENT_MAX (offsetof(ସଂରଚନା inotify_event, name) + NAME_MAX + 1)

/* This evaluates arguments multiple times */
#define FOREACH_INOTIFY_EVENT_FULL(e, buffer, sz, log_level)            \
        ପାଇଁ (ସଂରଚନା inotify_event *e = ଖାଲି;                            \
             inotify_event_next(&buffer, sz, &e, log_level); )

#define FOREACH_INOTIFY_EVENT(e, buffer, sz)                    \
        FOREACH_INOTIFY_EVENT_FULL(e, buffer, sz, LOG_DEBUG)

#define FOREACH_INOTIFY_EVENT_WARN(e, buffer, sz)               \
        FOREACH_INOTIFY_EVENT_FULL(e, buffer, sz, LOG_WARNING)

ସଂଘ inotify_event_buffer {
        ସଂରଚନା inotify_event ev;
        uint8_t raw[INOTIFY_EVENT_MAX];
};

bool inotify_event_next(
                ସଂଘ inotify_event_buffer *buffer,
                size_t size,
                ସଂରଚନା inotify_event **iterator,
                ପୂର୍ଣ୍ଣସଂଖ୍ୟା log_level);

ପୂର୍ଣ୍ଣସଂଖ୍ୟା inotify_add_watch_fd(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd, ପୂର୍ଣ୍ଣସଂଖ୍ୟା what, uint32_t mask);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା inotify_add_watch_and_warn(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd, ଅଚଳ ଅକ୍ଷର *pathname, uint32_t mask);
