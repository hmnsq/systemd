/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#অন্তর্ভাব <sys/fanotify.h>       /* IWYU pragma: export */
#অন্তর্ভাব <sys/vfs.h>            /* IWYU pragma: export */

#অন্তর্ভাব "forward.h"

সংঘ fanotify_event_বাফার {
        কাঠামো fanotify_event_metadata em;
        uint8_t raw[4096];
};

প্রকারলেখ কাঠামো FanotifyEventIterator {
        ধ্রুবক uint8_t *ptr;                  /* cursor into the raw read buffer */
        size_t বাম;                         /* remaining bytes */
        সংঘ fanotify_event_বাফার aligned; /* 8-byte aligned scratch for one event */
} FanotifyEventIterator;

bool fanotify_event_iterator_পরবর্তী(FanotifyEventIterator *i, ধ্রুবক কাঠামো fanotify_event_metadata **ret);

/* Iterates over the events in a buffer filled by read() on an fanotify fd. Each event is copied into an
 * 8-byte aligned scratch buffer first, so 'e' is always suitably aligned. 'buffer' must be a
 * union fanotify_event_buffer, 'sz' the number of bytes read (must be >= 0). FAN_EVENT_OK()/
 * FAN_EVENT_NEXT() can't be used, as they dereference the metadata in place while possibly unaligned. */
#define _FOREACH_FANOTIFY_EVENT(e, বাফার, sz, i)               \
        জন্য (FanotifyEventIterator i =                          \
                     { .ptr = (বাফার).raw, .বাম = (sz), };    \
             i.ptr;                                             \
             i.ptr = ফাঁকা)                                      \
                জন্য (ধ্রুবক কাঠামো fanotify_event_metadata *e;   \
                     fanotify_event_iterator_পরবর্তী(&i, &e); )
#define FOREACH_FANOTIFY_EVENT(e, বাফার, sz)                   \
        _FOREACH_FANOTIFY_EVENT(e, বাফার, sz, UNIQ_T(i, UNIQ))

পূর্ণসংখ্যা fanotify_event_pin_fd(
                ধ্রুবক কাঠামো fanotify_event_metadata *e,
                পূর্ণসংখ্যা mount_fd,              /* O_PATH fd cannot be used */
                ধ্রুবক কাঠামো statfs *sfs); /* can be NULL */

পূর্ণসংখ্যা fanotify_mark_fd(পূর্ণসংখ্যা fanotify_fd, unsigned flags, uint64_t mask, পূর্ণসংখ্যা fd);
