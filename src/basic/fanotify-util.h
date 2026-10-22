/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ਹਦਾਇਤ once

#ਅਨ੍ਤਰ੍ਭਾਵ <sys/fanotify.h>       /* IWYU pragma: export */
#ਅਨ੍ਤਰ੍ਭਾਵ <sys/vfs.h>            /* IWYU pragma: export */

#ਅਨ੍ਤਰ੍ਭਾਵ "forward.h"

ਸੰਘ fanotify_event_ਬਫਰ {
        ਬਣਤਰ fanotify_event_metadata em;
        uint8_t raw[4096];
};

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ FanotifyEventIterator {
        ਅਚਲ uint8_t *ptr;                  /* cursor into the raw read buffer */
        size_t ਖੱਬਾ;                         /* remaining bytes */
        ਸੰਘ fanotify_event_ਬਫਰ aligned; /* 8-byte aligned scratch for one event */
} FanotifyEventIterator;

bool fanotify_event_iterator_ਅਗਲਾ(FanotifyEventIterator *i, ਅਚਲ ਬਣਤਰ fanotify_event_metadata **ret);

/* Iterates over the events in a buffer filled by read() on an fanotify fd. Each event is copied into an
 * 8-byte aligned scratch buffer first, so 'e' is always suitably aligned. 'buffer' must be a
 * union fanotify_event_buffer, 'sz' the number of bytes read (must be >= 0). FAN_EVENT_OK()/
 * FAN_EVENT_NEXT() can't be used, as they dereference the metadata in place while possibly unaligned. */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ _FOREACH_FANOTIFY_EVENT(e, ਬਫਰ, sz, i)               \
        ਲਈ (FanotifyEventIterator i =                          \
                     { .ptr = (ਬਫਰ).raw, .ਖੱਬਾ = (sz), };    \
             i.ptr;                                             \
             i.ptr = ਨਿਰਾ)                                      \
                ਲਈ (ਅਚਲ ਬਣਤਰ fanotify_event_metadata *e;   \
                     fanotify_event_iterator_ਅਗਲਾ(&i, &e); )
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ FOREACH_FANOTIFY_EVENT(e, ਬਫਰ, sz)                   \
        _FOREACH_FANOTIFY_EVENT(e, ਬਫਰ, sz, UNIQ_T(i, UNIQ))

ਪੂਰਨ fanotify_event_pin_fd(
                ਅਚਲ ਬਣਤਰ fanotify_event_metadata *e,
                ਪੂਰਨ mount_fd,              /* O_PATH fd cannot be used */
                ਅਚਲ ਬਣਤਰ statfs *sfs); /* can be NULL */

ਪੂਰਨ fanotify_mark_fd(ਪੂਰਨ fanotify_fd, ਬਿਨਾਨਿਸ਼ਾਨੀ flags, uint64_t ਨਕਾਬ_3, ਪੂਰਨ fd);
