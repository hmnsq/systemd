/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#अन्तर्भाव <sys/fanotify.h>       /* IWYU pragma: export */
#अन्तर्भाव <sys/vfs.h>            /* IWYU pragma: export */

#अन्तर्भाव "forward.h"

संघ fanotify_event_buffer {
        संरचना fanotify_event_metadata em;
        uint8_t raw[4096];
};

प्रकारव्याख्या संरचना FanotifyEventIterator {
        स्थिरांक uint8_t *ptr;                  /* cursor into the raw read buffer */
        माप_प्रकार डावा;                         /* remaining bytes */
        संघ fanotify_event_buffer aligned; /* 8-byte aligned scratch for one event */
} FanotifyEventIterator;

bool fanotify_event_iterator_next(FanotifyEventIterator *i, स्थिरांक संरचना fanotify_event_metadata **ret);

/* Iterates over the events in a buffer filled by read() on an fanotify fd. Each event is copied into an
 * 8-byte aligned scratch buffer first, so 'e' is always suitably aligned. 'buffer' must be a
 * union fanotify_event_buffer, 'sz' the number of bytes read (must be >= 0). FAN_EVENT_OK()/
 * FAN_EVENT_NEXT() can't be used, as they dereference the metadata in place while possibly unaligned. */
#define _FOREACH_FANOTIFY_EVENT(e, buffer, sz, i)               \
        साठी (FanotifyEventIterator i =                          \
                     { .ptr = (buffer).raw, .डावा = (sz), };    \
             i.ptr;                                             \
             i.ptr = रिक्त)                                      \
                साठी (स्थिरांक संरचना fanotify_event_metadata *e;   \
                     fanotify_event_iterator_next(&i, &e); )
#define FOREACH_FANOTIFY_EVENT(e, buffer, sz)                   \
        _FOREACH_FANOTIFY_EVENT(e, buffer, sz, UNIQ_T(i, UNIQ))

पूर्णांक fanotify_event_pin_fd(
                स्थिरांक संरचना fanotify_event_metadata *e,
                पूर्णांक mount_fd,              /* O_PATH fd cannot be used */
                स्थिरांक संरचना statfs *sfs); /* can be NULL */

पूर्णांक fanotify_mark_fd(पूर्णांक fanotify_fd, अचिन्हित flags, uint64_t mask, पूर्णांक fd);
