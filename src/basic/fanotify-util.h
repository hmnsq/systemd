/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#شامل <sys/fanotify.h>       /* IWYU pragma: export */
#شامل <sys/vfs.h>            /* IWYU pragma: export */

#شامل "forward.h"

اجتماع fanotify_event_buffer {
        ساختار fanotify_event_metadata em;
        uint8_t raw[4096];
};

تعریفنوع ساختار FanotifyEventIterator {
        ثابت uint8_t *ptr;                  /* cursor into the raw read buffer */
        size_t چپ;                         /* remaining bytes */
        اجتماع fanotify_event_buffer aligned; /* 8-byte aligned scratch for one event */
} FanotifyEventIterator;

bool fanotify_event_iterator_next(FanotifyEventIterator *i, ثابت ساختار fanotify_event_metadata **ret);

/* Iterates over the events in a buffer filled by read() on an fanotify fd. Each event is copied into an
 * 8-byte aligned scratch buffer first, so 'e' is always suitably aligned. 'buffer' must be a
 * union fanotify_event_buffer, 'sz' the number of bytes read (must be >= 0). FAN_EVENT_OK()/
 * FAN_EVENT_NEXT() can't be used, as they dereference the metadata in place while possibly unaligned. */
#define _FOREACH_FANOTIFY_EVENT(e, buffer, sz, i)               \
        برای (FanotifyEventIterator i =                          \
                     { .ptr = (buffer).raw, .چپ = (sz), };    \
             i.ptr;                                             \
             i.ptr = پوچ)                                      \
                برای (ثابت ساختار fanotify_event_metadata *e;   \
                     fanotify_event_iterator_next(&i, &e); )
#define FOREACH_FANOTIFY_EVENT(e, buffer, sz)                   \
        _FOREACH_FANOTIFY_EVENT(e, buffer, sz, UNIQ_T(i, UNIQ))

صحیح fanotify_event_pin_fd(
                ثابت ساختار fanotify_event_metadata *e,
                صحیح mount_fd,              /* O_PATH fd cannot be used */
                ثابت ساختار statfs *sfs); /* can be NULL */

صحیح fanotify_mark_fd(صحیح fanotify_fd, unsigned flags, uint64_t mask, صحیح fd);
