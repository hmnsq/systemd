/* SPDX-License-Identifier: LGPL-2.1-or-later */
#رهنمود once

#ضمن <sys/fanotify.h>       /* IWYU pragma: export */
#ضمن <sys/vfs.h>            /* IWYU pragma: export */

#ضمن "forward.h"

اتحاد fanotify_event_میانگیر {
        بنية fanotify_event_metadata em;
        uint8_t raw[4096];
};

تعريفنوع بنية FanotifyEventIterator {
        ثابت uint8_t *ptr;                  /* cursor into the raw read buffer */
        size_t چپ;                         /* remaining bytes */
        اتحاد fanotify_event_میانگیر aligned; /* 8-byte aligned scratch for one event */
} FanotifyEventIterator;

bool fanotify_event_iterator_بعدی(FanotifyEventIterator *i, ثابت بنية fanotify_event_metadata **ret);

/* Iterates over the events in a buffer filled by read() on an fanotify fd. Each event is copied into an
 * 8-byte aligned scratch buffer first, so 'e' is always suitably aligned. 'buffer' must be a
 * union fanotify_event_buffer, 'sz' the number of bytes read (must be >= 0). FAN_EVENT_OK()/
 * FAN_EVENT_NEXT() can't be used, as they dereference the metadata in place while possibly unaligned. */
#تعریف_کن _FOREACH_FANOTIFY_EVENT(e, میانگیر, sz, i)               \
        لكل (FanotifyEventIterator i =                          \
                     { .ptr = (میانگیر).raw, .چپ = (sz), };    \
             i.ptr;                                             \
             i.ptr = عدم)                                      \
                لكل (ثابت بنية fanotify_event_metadata *e;   \
                     fanotify_event_iterator_بعدی(&i, &e); )
#تعریف_کن FOREACH_FANOTIFY_EVENT(e, میانگیر, sz)                   \
        _FOREACH_FANOTIFY_EVENT(e, میانگیر, sz, UNIQ_T(i, UNIQ))

صحيح fanotify_event_pin_fd(
                ثابت بنية fanotify_event_metadata *e,
                صحيح mount_fd,              /* O_PATH fd cannot be used */
                ثابت بنية statfs *sfs); /* can be NULL */

صحيح fanotify_mark_fd(صحيح fanotify_fd, بیعلامت flags, uint64_t ماسک_3, صحيح fd);
