/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#अन्तर्भाव <sys/inotify.h>        /* IWYU pragma: export */
#अन्तर्भाव <syslog.h>

#अन्तर्भाव "forward.h"

#define INOTIFY_EVENT_MAX (दुरत्व(संरचना inotify_event, name) + NAME_MAX + 1)

/* This evaluates arguments multiple times */
#define FOREACH_INOTIFY_EVENT_FULL(e, buffer, sz, log_level)            \
        लागि (संरचना inotify_event *e = रिक्त;                            \
             inotify_event_next(&buffer, sz, &e, log_level); )

#define FOREACH_INOTIFY_EVENT(e, buffer, sz)                    \
        FOREACH_INOTIFY_EVENT_FULL(e, buffer, sz, LOG_DEBUG)

#define FOREACH_INOTIFY_EVENT_WARN(e, buffer, sz)               \
        FOREACH_INOTIFY_EVENT_FULL(e, buffer, sz, LOG_WARNING)

सङ्घ inotify_event_buffer {
        संरचना inotify_event ev;
        uint8_t raw[INOTIFY_EVENT_MAX];
};

bool inotify_event_next(
                सङ्घ inotify_event_buffer *buffer,
                माप_प्रकार size,
                संरचना inotify_event **iterator,
                पूर्णाङ्क log_level);

पूर्णाङ्क inotify_add_watch_fd(पूर्णाङ्क fd, पूर्णाङ्क what, uint32_t mask);
पूर्णाङ्क inotify_add_watch_and_warn(पूर्णाङ्क fd, अचर वर्ण *pathname, uint32_t mask);
