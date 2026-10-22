/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ہدایت once

#شامل <sys/inotify.h>        /* IWYU pragma: export */
#شامل <syslog.h>

#شامل "forward.h"

#تعریف_کرو INOTIFY_EVENT_MAX (offsetof(ساخت inotify_event, name) + NAME_MAX + 1)

/* This evaluates arguments multiple times */
#تعریف_کرو FOREACH_INOTIFY_EVENT_بھرا(e, بفر, sz, log_level)            \
        برائے (ساخت inotify_event *e = تہی;                            \
             inotify_event_اگلا(&بفر, sz, &e, log_level); )

#تعریف_کرو FOREACH_INOTIFY_EVENT(e, بفر, sz)                    \
        FOREACH_INOTIFY_EVENT_بھرا(e, بفر, sz, LOG_DEBUG)

#تعریف_کرو FOREACH_INOTIFY_EVENT_WARN(e, بفر, sz)               \
        FOREACH_INOTIFY_EVENT_بھرا(e, بفر, sz, LOG_WARNING)

اتحاد inotify_event_بفر {
        ساخت inotify_event ev;
        uint8_t raw[INOTIFY_EVENT_MAX];
};

bool inotify_event_اگلا(
                اتحاد inotify_event_بفر *بفر,
                size_t سائز,
                ساخت inotify_event **iterator,
                صحیح log_level);

صحیح inotify_جوڑو_watch_fd(صحیح fd, صحیح what, uint32_t نقاب_3);
صحیح inotify_جوڑو_watch_and_warn(صحیح fd, ثابت حرف *pathname, uint32_t نقاب_3);
