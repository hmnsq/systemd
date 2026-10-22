/* SPDX-License-Identifier: LGPL-2.1-or-later */
#निर्देशः once

#अन्तर्भाव <sys/inotify.h>        /* IWYU pragma: export */
#अन्तर्भाव <syslog.h>

#अन्तर्भाव "forward.h"

#परिभाषय INOTIFY_EVENT_MAX (offsetof(संरचना inotify_event, name) + NAME_MAX + 1)

/* This evaluates arguments multiple times */
#परिभाषय FOREACH_INOTIFY_EVENT_पूर्णम्(e, बफर, sz, log_level)            \
        प्रति (संरचना inotify_event *e = रिक्त;                            \
             inotify_event_अग्र(&बफर, sz, &e, log_level); )

#परिभाषय FOREACH_INOTIFY_EVENT(e, बफर, sz)                    \
        FOREACH_INOTIFY_EVENT_पूर्णम्(e, बफर, sz, LOG_DEBUG)

#परिभाषय FOREACH_INOTIFY_EVENT_WARN(e, बफर, sz)               \
        FOREACH_INOTIFY_EVENT_पूर्णम्(e, बफर, sz, LOG_WARNING)

सङ्घ inotify_event_बफर {
        संरचना inotify_event ev;
        uint8_t raw[INOTIFY_EVENT_MAX];
};

bool inotify_event_अग्र(
                सङ्घ inotify_event_बफर *बफर,
                size_t आकार,
                संरचना inotify_event **iterator,
                पूर्णाङ्क log_level);

पूर्णाङ्क inotify_सङ्कलय_watch_fd(पूर्णाङ्क fd, पूर्णाङ्क what, uint32_t आवरणम्_3);
पूर्णाङ्क inotify_सङ्कलय_watch_and_warn(पूर्णाङ्क fd, ध्रुव वर्ण *pathname, uint32_t आवरणम्_3);
