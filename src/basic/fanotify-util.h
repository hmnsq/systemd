/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#অন্তর্ভাৱ <sys/fanotify.h>       /* IWYU pragma: export */
#অন্তর্ভাৱ <sys/vfs.h>            /* IWYU pragma: export */

#অন্তর্ভাৱ "forward.h"

সঙ্ঘ fanotify_event_buffer {
        সংরচনা fanotify_event_metadata em;
        uint8_t raw[4096];
};

প্রকারনির্দেশ সংরচনা FanotifyEventIterator {
        ধ্রুৱ uint8_t *ptr;                  /* cursor into the raw read buffer */
        size_t বাম;                         /* remaining bytes */
        সঙ্ঘ fanotify_event_buffer aligned; /* 8-byte aligned scratch for one event */
} FanotifyEventIterator;

bool fanotify_event_iterator_next(FanotifyEventIterator *i, ধ্রুৱ সংরচনা fanotify_event_metadata **ret);

/* Iterates over the events in a buffer filled by read() on an fanotify fd. Each event is copied into an
 * 8-byte aligned scratch buffer first, so 'e' is always suitably aligned. 'buffer' must be a
 * union fanotify_event_buffer, 'sz' the number of bytes read (must be >= 0). FAN_EVENT_OK()/
 * FAN_EVENT_NEXT() can't be used, as they dereference the metadata in place while possibly unaligned. */
#define _FOREACH_FANOTIFY_EVENT(e, buffer, sz, i)               \
        বাবে (FanotifyEventIterator i =                          \
                     { .ptr = (buffer).raw, .বাম = (sz), };    \
             i.ptr;                                             \
             i.ptr = রিক্ত)                                      \
                বাবে (ধ্রুৱ সংরচনা fanotify_event_metadata *e;   \
                     fanotify_event_iterator_next(&i, &e); )
#define FOREACH_FANOTIFY_EVENT(e, buffer, sz)                   \
        _FOREACH_FANOTIFY_EVENT(e, buffer, sz, UNIQ_T(i, UNIQ))

পূর্ণাঙ্ক fanotify_event_pin_fd(
                ধ্রুৱ সংরচনা fanotify_event_metadata *e,
                পূর্ণাঙ্ক mount_fd,              /* O_PATH fd cannot be used */
                ধ্রুৱ সংরচনা statfs *sfs); /* can be NULL */

পূর্ণাঙ্ক fanotify_mark_fd(পূর্ণাঙ্ক fanotify_fd, unsigned flags, uint64_t mask, পূর্ণাঙ্ক fd);
