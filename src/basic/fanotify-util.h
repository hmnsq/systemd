/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#ಅನ್ತರ್ಭಾವ <sys/fanotify.h>       /* IWYU pragma: export */
#ಅನ್ತರ್ಭಾವ <sys/vfs.h>            /* IWYU pragma: export */

#ಅನ್ತರ್ಭಾವ "forward.h"

ಸಙ್ಘ fanotify_event_buffer {
        ಸಂರಚನಾ fanotify_event_metadata em;
        uint8_t raw[4096];
};

ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ FanotifyEventIterator {
        ಧ್ರುವ uint8_t *ptr;                  /* cursor into the raw read buffer */
        size_t ಎಡ;                         /* remaining bytes */
        ಸಙ್ಘ fanotify_event_buffer aligned; /* 8-byte aligned scratch for one event */
} FanotifyEventIterator;

bool fanotify_event_iterator_next(FanotifyEventIterator *i, ಧ್ರುವ ಸಂರಚನಾ fanotify_event_metadata **ret);

/* Iterates over the events in a buffer filled by read() on an fanotify fd. Each event is copied into an
 * 8-byte aligned scratch buffer first, so 'e' is always suitably aligned. 'buffer' must be a
 * union fanotify_event_buffer, 'sz' the number of bytes read (must be >= 0). FAN_EVENT_OK()/
 * FAN_EVENT_NEXT() can't be used, as they dereference the metadata in place while possibly unaligned. */
#define _FOREACH_FANOTIFY_EVENT(e, buffer, sz, i)               \
        ಪ್ರತಿ (FanotifyEventIterator i =                          \
                     { .ptr = (buffer).raw, .ಎಡ = (sz), };    \
             i.ptr;                                             \
             i.ptr = ರಿಕ್ತ)                                      \
                ಪ್ರತಿ (ಧ್ರುವ ಸಂರಚನಾ fanotify_event_metadata *e;   \
                     fanotify_event_iterator_next(&i, &e); )
#define FOREACH_FANOTIFY_EVENT(e, buffer, sz)                   \
        _FOREACH_FANOTIFY_EVENT(e, buffer, sz, UNIQ_T(i, UNIQ))

ಪೂರ್ಣಾಙ್ಕ fanotify_event_pin_fd(
                ಧ್ರುವ ಸಂರಚನಾ fanotify_event_metadata *e,
                ಪೂರ್ಣಾಙ್ಕ mount_fd,              /* O_PATH fd cannot be used */
                ಧ್ರುವ ಸಂರಚನಾ statfs *sfs); /* can be NULL */

ಪೂರ್ಣಾಙ್ಕ fanotify_mark_fd(ಪೂರ್ಣಾಙ್ಕ fanotify_fd, unsigned flags, uint64_t mask, ಪೂರ್ಣಾಙ್ಕ fd);
