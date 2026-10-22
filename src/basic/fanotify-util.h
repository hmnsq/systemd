/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#அந்தர்பாவ <sys/fanotify.h>       /* IWYU pragma: export */
#அந்தர்பாவ <sys/vfs.h>            /* IWYU pragma: export */

#அந்தர்பாவ "forward.h"

ஒன்றிப்பு fanotify_event_buffer {
        கட்டமைப்பு fanotify_event_metadata em;
        uint8_t raw[4096];
};

வகைவரை கட்டமைப்பு FanotifyEventIterator {
        மாறிலி uint8_t *ptr;                  /* cursor into the raw read buffer */
        size_t இடது;                         /* remaining bytes */
        ஒன்றிப்பு fanotify_event_buffer aligned; /* 8-byte aligned scratch for one event */
} FanotifyEventIterator;

bool fanotify_event_iterator_next(FanotifyEventIterator *i, மாறிலி கட்டமைப்பு fanotify_event_metadata **ret);

/* Iterates over the events in a buffer filled by read() on an fanotify fd. Each event is copied into an
 * 8-byte aligned scratch buffer first, so 'e' is always suitably aligned. 'buffer' must be a
 * union fanotify_event_buffer, 'sz' the number of bytes read (must be >= 0). FAN_EVENT_OK()/
 * FAN_EVENT_NEXT() can't be used, as they dereference the metadata in place while possibly unaligned. */
#define _FOREACH_FANOTIFY_EVENT(e, buffer, sz, i)               \
        க்கு (FanotifyEventIterator i =                          \
                     { .ptr = (buffer).raw, .இடது = (sz), };    \
             i.ptr;                                             \
             i.ptr = வெற்று)                                      \
                க்கு (மாறிலி கட்டமைப்பு fanotify_event_metadata *e;   \
                     fanotify_event_iterator_next(&i, &e); )
#define FOREACH_FANOTIFY_EVENT(e, buffer, sz)                   \
        _FOREACH_FANOTIFY_EVENT(e, buffer, sz, UNIQ_T(i, UNIQ))

முழு fanotify_event_pin_fd(
                மாறிலி கட்டமைப்பு fanotify_event_metadata *e,
                முழு mount_fd,              /* O_PATH fd cannot be used */
                மாறிலி கட்டமைப்பு statfs *sfs); /* can be NULL */

முழு fanotify_mark_fd(முழு fanotify_fd, unsigned flags, uint64_t mask, முழு fd);
