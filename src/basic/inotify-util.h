/* SPDX-License-Identifier: LGPL-2.1-or-later */
#هدايت once

#ضمن <sys/inotify.h>        /* IWYU pragma: export */
#ضمن <syslog.h>

#ضمن "forward.h"

#وصف_ڪريو INOTIFY_EVENT_MAX (offsetof(بنية inotify_event, name) + NAME_MAX + 1)

/* This evaluates arguments multiple times */
#وصف_ڪريو FOREACH_INOTIFY_EVENT_ڀريل(e, بفر, sz, log_level)            \
        لكل (بنية inotify_event *e = عدم;                            \
             inotify_event_ايندڙ(&بفر, sz, &e, log_level); )

#وصف_ڪريو FOREACH_INOTIFY_EVENT(e, بفر, sz)                    \
        FOREACH_INOTIFY_EVENT_ڀريل(e, بفر, sz, LOG_DEBUG)

#وصف_ڪريو FOREACH_INOTIFY_EVENT_WARN(e, بفر, sz)               \
        FOREACH_INOTIFY_EVENT_ڀريل(e, بفر, sz, LOG_WARNING)

اتحاد inotify_event_بفر {
        بنية inotify_event ev;
        uint8_t raw[INOTIFY_EVENT_MAX];
};

bool inotify_event_ايندڙ(
                اتحاد inotify_event_بفر *بفر,
                size_t سائيز,
                بنية inotify_event **iterator,
                صحيح log_level);

صحيح inotify_جوڙيو_watch_fd(صحيح fd, صحيح what, uint32_t نقاب_3);
صحيح inotify_جوڙيو_watch_and_warn(صحيح fd, ثابت حرف *pathname, uint32_t نقاب_3);
