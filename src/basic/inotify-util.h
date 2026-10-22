/* SPDX-License-Identifier: LGPL-2.1-or-later */
#رهنمود once

#ضمن <sys/inotify.h>        /* IWYU pragma: export */
#ضمن <syslog.h>

#ضمن "forward.h"

#تعریف_کن INOTIFY_EVENT_MAX (offsetof(بنية inotify_event, name) + NAME_MAX + 1)

/* This evaluates arguments multiple times */
#تعریف_کن FOREACH_INOTIFY_EVENT_پر(e, میانگیر, sz, log_level)            \
        لكل (بنية inotify_event *e = عدم;                            \
             inotify_event_بعدی(&میانگیر, sz, &e, log_level); )

#تعریف_کن FOREACH_INOTIFY_EVENT(e, میانگیر, sz)                    \
        FOREACH_INOTIFY_EVENT_پر(e, میانگیر, sz, LOG_DEBUG)

#تعریف_کن FOREACH_INOTIFY_EVENT_WARN(e, میانگیر, sz)               \
        FOREACH_INOTIFY_EVENT_پر(e, میانگیر, sz, LOG_WARNING)

اتحاد inotify_event_میانگیر {
        بنية inotify_event ev;
        uint8_t raw[INOTIFY_EVENT_MAX];
};

bool inotify_event_بعدی(
                اتحاد inotify_event_میانگیر *میانگیر,
                size_t اندازه,
                بنية inotify_event **iterator,
                صحيح log_level);

صحيح inotify_جمع_کن_watch_fd(صحيح fd, صحيح what, uint32_t ماسک_3);
صحيح inotify_جمع_کن_watch_and_warn(صحيح fd, ثابت حرف *pathname, uint32_t ماسک_3);
