/* SPDX-License-Identifier: LGPL-2.1-or-later */
#निर्देशः once

#अन्तर्भाव <sys/fanotify.h>       /* IWYU pragma: export */
#अन्तर्भाव <sys/vfs.h>            /* IWYU pragma: export */

#अन्तर्भाव "forward.h"

सङ्घ fanotify_event_बफर {
        संरचना fanotify_event_metadata em;
        uint8_t raw[4096];
};

प्रकारनिर्देश संरचना FanotifyEventIterator {
        ध्रुव uint8_t *ptr;                  /* cursor into the raw read buffer */
        size_t वामः;                         /* remaining bytes */
        सङ्घ fanotify_event_बफर aligned; /* 8-byte aligned scratch for one event */
} FanotifyEventIterator;

bool fanotify_event_iterator_अग्र(FanotifyEventIterator *i, ध्रुव संरचना fanotify_event_metadata **ret);

/* Iterates over the events in a buffer filled by read() on an fanotify fd. Each event is copied into an
 * 8-byte aligned scratch buffer first, so 'e' is always suitably aligned. 'buffer' must be a
 * union fanotify_event_buffer, 'sz' the number of bytes read (must be >= 0). FAN_EVENT_OK()/
 * FAN_EVENT_NEXT() can't be used, as they dereference the metadata in place while possibly unaligned. */
#परिभाषय _FOREACH_FANOTIFY_EVENT(e, बफर, sz, i)               \
        प्रति (FanotifyEventIterator i =                          \
                     { .ptr = (बफर).raw, .वामः = (sz), };    \
             i.ptr;                                             \
             i.ptr = रिक्त)                                      \
                प्रति (ध्रुव संरचना fanotify_event_metadata *e;   \
                     fanotify_event_iterator_अग्र(&i, &e); )
#परिभाषय FOREACH_FANOTIFY_EVENT(e, बफर, sz)                   \
        _FOREACH_FANOTIFY_EVENT(e, बफर, sz, UNIQ_T(i, UNIQ))

पूर्णाङ्क fanotify_event_pin_fd(
                ध्रुव संरचना fanotify_event_metadata *e,
                पूर्णाङ्क mount_fd,              /* O_PATH fd cannot be used */
                ध्रुव संरचना statfs *sfs); /* can be NULL */

पूर्णाङ्क fanotify_mark_fd(पूर्णाङ्क fanotify_fd, अचिह्नित flags, uint64_t आवरणम्_3, पूर्णाङ्क fd);
