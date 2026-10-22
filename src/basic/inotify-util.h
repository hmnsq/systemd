/* SPDX-License-Identifier: LGPL-2.1-or-later */
#आशय once

#समावेश <sys/inotify.h>        /* IWYU pragma: export */
#समावेश <syslog.h>

#समावेश "forward.h"

#घोषणा INOTIFY_EVENT_MAX (दुरत्व(काष्ठा inotify_event, name) + NAME_MAX + 1)

/* This evaluates arguments multiple times */
#घोषणा FOREACH_INOTIFY_EVENT_FULL(e, बफर, sz, log_level)            \
        क्रम (काष्ठा inotify_event *e = शून्य;                            \
             inotify_event_अग्र(&बफर, sz, &e, log_level); )

#घोषणा FOREACH_INOTIFY_EVENT(e, बफर, sz)                    \
        FOREACH_INOTIFY_EVENT_FULL(e, बफर, sz, LOG_DEBUG)

#घोषणा FOREACH_INOTIFY_EVENT_WARN(e, बफर, sz)               \
        FOREACH_INOTIFY_EVENT_FULL(e, बफर, sz, LOG_WARNING)

जोड़ inotify_event_बफर {
        काष्ठा inotify_event ev;
        uint8_t raw[INOTIFY_EVENT_MAX];
};

bool inotify_event_अग्र(
                जोड़ inotify_event_बफर *बफर,
                माप_प्रकार आकार,
                काष्ठा inotify_event **iterator,
                पूर्णांक log_level);

पूर्णांक inotify_add_watch_fd(पूर्णांक fd, पूर्णांक what, uint32_t mask);
पूर्णांक inotify_add_watch_and_warn(पूर्णांक fd, स्थिर अक्षर *pathname, uint32_t mask);
