/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#અન્તર્ભાવ <sys/inotify.h>        /* IWYU pragma: export */
#અન્તર્ભાવ <syslog.h>

#અન્તર્ભાવ "forward.h"

#define INOTIFY_EVENT_MAX (offsetof(સંરચના inotify_event, name) + NAME_MAX + 1)

/* This evaluates arguments multiple times */
#define FOREACH_INOTIFY_EVENT_FULL(e, buffer, sz, log_level)            \
        માટે (સંરચના inotify_event *e = ખાલી;                            \
             inotify_event_next(&buffer, sz, &e, log_level); )

#define FOREACH_INOTIFY_EVENT(e, buffer, sz)                    \
        FOREACH_INOTIFY_EVENT_FULL(e, buffer, sz, LOG_DEBUG)

#define FOREACH_INOTIFY_EVENT_WARN(e, buffer, sz)               \
        FOREACH_INOTIFY_EVENT_FULL(e, buffer, sz, LOG_WARNING)

સંઘ inotify_event_buffer {
        સંરચના inotify_event ev;
        uint8_t raw[INOTIFY_EVENT_MAX];
};

bool inotify_event_next(
                સંઘ inotify_event_buffer *buffer,
                size_t size,
                સંરચના inotify_event **iterator,
                પૂર્ણાંક log_level);

પૂર્ણાંક inotify_add_watch_fd(પૂર્ણાંક fd, પૂર્ણાંક what, uint32_t mask);
પૂર્ણાંક inotify_add_watch_and_warn(પૂર્ણાંક fd, અચલ અક્ષર *pathname, uint32_t mask);
