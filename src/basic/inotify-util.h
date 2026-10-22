/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ਹਦਾਇਤ once

#ਅਨ੍ਤਰ੍ਭਾਵ <sys/inotify.h>        /* IWYU pragma: export */
#ਅਨ੍ਤਰ੍ਭਾਵ <syslog.h>

#ਅਨ੍ਤਰ੍ਭਾਵ "forward.h"

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ INOTIFY_EVENT_MAX (offsetof(ਬਣਤਰ inotify_event, name) + NAME_MAX + 1)

/* This evaluates arguments multiple times */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ FOREACH_INOTIFY_EVENT_ਭਰਿਆ(e, ਬਫਰ, sz, log_level)            \
        ਲਈ (ਬਣਤਰ inotify_event *e = ਨਿਰਾ;                            \
             inotify_event_ਅਗਲਾ(&ਬਫਰ, sz, &e, log_level); )

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ FOREACH_INOTIFY_EVENT(e, ਬਫਰ, sz)                    \
        FOREACH_INOTIFY_EVENT_ਭਰਿਆ(e, ਬਫਰ, sz, LOG_DEBUG)

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ FOREACH_INOTIFY_EVENT_WARN(e, ਬਫਰ, sz)               \
        FOREACH_INOTIFY_EVENT_ਭਰਿਆ(e, ਬਫਰ, sz, LOG_WARNING)

ਸੰਘ inotify_event_ਬਫਰ {
        ਬਣਤਰ inotify_event ev;
        uint8_t raw[INOTIFY_EVENT_MAX];
};

bool inotify_event_ਅਗਲਾ(
                ਸੰਘ inotify_event_ਬਫਰ *ਬਫਰ,
                size_t ਮਾਪ,
                ਬਣਤਰ inotify_event **iterator,
                ਪੂਰਨ log_level);

ਪੂਰਨ inotify_ਜੋੜੋ_watch_fd(ਪੂਰਨ fd, ਪੂਰਨ what, uint32_t ਨਕਾਬ_3);
ਪੂਰਨ inotify_ਜੋੜੋ_watch_and_warn(ਪੂਰਨ fd, ਅਚਲ ਅੱਖਰ *pathname, uint32_t ਨਕਾਬ_3);
