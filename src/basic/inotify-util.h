/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#অন্তর্ভাব <sys/inotify.h>        /* IWYU pragma: export */
#অন্তর্ভাব <syslog.h>

#অন্তর্ভাব "forward.h"

#define INOTIFY_EVENT_MAX (offsetof(কাঠামো inotify_event, name) + NAME_MAX + 1)

/* This evaluates arguments multiple times */
#define FOREACH_INOTIFY_EVENT_FULL(e, বাফার, sz, log_level)            \
        জন্য (কাঠামো inotify_event *e = ফাঁকা;                            \
             inotify_event_পরবর্তী(&বাফার, sz, &e, log_level); )

#define FOREACH_INOTIFY_EVENT(e, বাফার, sz)                    \
        FOREACH_INOTIFY_EVENT_FULL(e, বাফার, sz, LOG_DEBUG)

#define FOREACH_INOTIFY_EVENT_WARN(e, বাফার, sz)               \
        FOREACH_INOTIFY_EVENT_FULL(e, বাফার, sz, LOG_WARNING)

সংঘ inotify_event_বাফার {
        কাঠামো inotify_event ev;
        uint8_t raw[INOTIFY_EVENT_MAX];
};

bool inotify_event_পরবর্তী(
                সংঘ inotify_event_বাফার *বাফার,
                size_t আকার_2,
                কাঠামো inotify_event **iterator,
                পূর্ণসংখ্যা log_level);

পূর্ণসংখ্যা inotify_add_watch_fd(পূর্ণসংখ্যা fd, পূর্ণসংখ্যা what, uint32_t mask);
পূর্ণসংখ্যা inotify_add_watch_and_warn(পূর্ণসংখ্যা fd, ধ্রুবক অক্ষর *pathname, uint32_t mask);
