/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ಸೂಚನೆ once

#ಅನ್ತರ್ಭಾವ <sys/fanotify.h>       /* IWYU pragma: export */
#ಅನ್ತರ್ಭಾವ <sys/vfs.h>            /* IWYU pragma: export */

#ಅನ್ತರ್ಭಾವ "forward.h"

ಒಕ್ಕೂಟ fanotify_event_ಬಫರ್ {
        ರಚನೆ fanotify_event_metadata em;
        uint8_t raw[4096];
};

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ FanotifyEventIterator {
        ಸ್ಥಿರಾಂಕ uint8_t *ptr;                  /* cursor into the raw read buffer */
        size_t ಎಡ;                         /* remaining bytes */
        ಒಕ್ಕೂಟ fanotify_event_ಬಫರ್ aligned; /* 8-byte aligned scratch for one event */
} FanotifyEventIterator;

bool fanotify_event_iterator_ಮುಂದಿನ(FanotifyEventIterator *i, ಸ್ಥಿರಾಂಕ ರಚನೆ fanotify_event_metadata **ret);

/* Iterates over the events in a buffer filled by read() on an fanotify fd. Each event is copied into an
 * 8-byte aligned scratch buffer first, so 'e' is always suitably aligned. 'buffer' must be a
 * union fanotify_event_buffer, 'sz' the number of bytes read (must be >= 0). FAN_EVENT_OK()/
 * FAN_EVENT_NEXT() can't be used, as they dereference the metadata in place while possibly unaligned. */
#ವ್ಯಾಖ್ಯಾನಿಸು _FOREACH_FANOTIFY_EVENT(e, ಬಫರ್, sz, i)               \
        ಗಾಗಿ (FanotifyEventIterator i =                          \
                     { .ptr = (ಬಫರ್).raw, .ಎಡ = (sz), };    \
             i.ptr;                                             \
             i.ptr = ಖಾಲಿ)                                      \
                ಗಾಗಿ (ಸ್ಥಿರಾಂಕ ರಚನೆ fanotify_event_metadata *e;   \
                     fanotify_event_iterator_ಮುಂದಿನ(&i, &e); )
#ವ್ಯಾಖ್ಯಾನಿಸು FOREACH_FANOTIFY_EVENT(e, ಬಫರ್, sz)                   \
        _FOREACH_FANOTIFY_EVENT(e, ಬಫರ್, sz, UNIQ_T(i, UNIQ))

ಪೂರ್ಣಾಂಕ fanotify_event_pin_fd(
                ಸ್ಥಿರಾಂಕ ರಚನೆ fanotify_event_metadata *e,
                ಪೂರ್ಣಾಂಕ mount_fd,              /* O_PATH fd cannot be used */
                ಸ್ಥಿರಾಂಕ ರಚನೆ statfs *sfs); /* can be NULL */

ಪೂರ್ಣಾಂಕ fanotify_mark_fd(ಪೂರ್ಣಾಂಕ fanotify_fd, ಚಿಹ್ನೆರಹಿತ flags, uint64_t ಮುಖವಾಡ_3, ಪೂರ್ಣಾಂಕ fd);
