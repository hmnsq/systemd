/* SPDX-License-Identifier: LGPL-2.1-or-later */
#आशय once

#समावेश <sys/fanotify.h>       /* IWYU pragma: export */
#समावेश <sys/vfs.h>            /* IWYU pragma: export */

#समावेश "forward.h"

जोड़ fanotify_event_बफर {
        काष्ठा fanotify_event_metadata em;
        uint8_t raw[4096];
};

प्रकार काष्ठा FanotifyEventIterator {
        स्थिर uint8_t *ptr;                  /* cursor into the raw read buffer */
        माप_प्रकार बायाँ;                         /* remaining bytes */
        जोड़ fanotify_event_बफर aligned; /* 8-byte aligned scratch for one event */
} FanotifyEventIterator;

bool fanotify_event_iterator_अग्र(FanotifyEventIterator *i, स्थिर काष्ठा fanotify_event_metadata **ret);

/* Iterates over the events in a buffer filled by read() on an fanotify fd. Each event is copied into an
 * 8-byte aligned scratch buffer first, so 'e' is always suitably aligned. 'buffer' must be a
 * union fanotify_event_buffer, 'sz' the number of bytes read (must be >= 0). FAN_EVENT_OK()/
 * FAN_EVENT_NEXT() can't be used, as they dereference the metadata in place while possibly unaligned. */
#घोषणा _FOREACH_FANOTIFY_EVENT(e, बफर, sz, i)               \
        क्रम (FanotifyEventIterator i =                          \
                     { .ptr = (बफर).raw, .बायाँ = (sz), };    \
             i.ptr;                                             \
             i.ptr = शून्य)                                      \
                क्रम (स्थिर काष्ठा fanotify_event_metadata *e;   \
                     fanotify_event_iterator_अग्र(&i, &e); )
#घोषणा FOREACH_FANOTIFY_EVENT(e, बफर, sz)                   \
        _FOREACH_FANOTIFY_EVENT(e, बफर, sz, UNIQ_T(i, UNIQ))

पूर्णांक fanotify_event_pin_fd(
                स्थिर काष्ठा fanotify_event_metadata *e,
                पूर्णांक mount_fd,              /* O_PATH fd cannot be used */
                स्थिर काष्ठा statfs *sfs); /* can be NULL */

पूर्णांक fanotify_mark_fd(पूर्णांक fanotify_fd, अचिन्हित flags, uint64_t mask, पूर्णांक fd);
