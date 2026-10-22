/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ہدایت once

#شامل <sys/fanotify.h>       /* IWYU pragma: export */
#شامل <sys/vfs.h>            /* IWYU pragma: export */

#شامل "forward.h"

اتحاد fanotify_event_بفر {
        ساخت fanotify_event_metadata em;
        uint8_t raw[4096];
};

قسمتعریف ساخت FanotifyEventIterator {
        ثابت uint8_t *ptr;                  /* cursor into the raw read buffer */
        size_t بایاں;                         /* remaining bytes */
        اتحاد fanotify_event_بفر aligned; /* 8-byte aligned scratch for one event */
} FanotifyEventIterator;

bool fanotify_event_iterator_اگلا(FanotifyEventIterator *i, ثابت ساخت fanotify_event_metadata **ret);

/* Iterates over the events in a buffer filled by read() on an fanotify fd. Each event is copied into an
 * 8-byte aligned scratch buffer first, so 'e' is always suitably aligned. 'buffer' must be a
 * union fanotify_event_buffer, 'sz' the number of bytes read (must be >= 0). FAN_EVENT_OK()/
 * FAN_EVENT_NEXT() can't be used, as they dereference the metadata in place while possibly unaligned. */
#تعریف_کرو _FOREACH_FANOTIFY_EVENT(e, بفر, sz, i)               \
        برائے (FanotifyEventIterator i =                          \
                     { .ptr = (بفر).raw, .بایاں = (sz), };    \
             i.ptr;                                             \
             i.ptr = تہی)                                      \
                برائے (ثابت ساخت fanotify_event_metadata *e;   \
                     fanotify_event_iterator_اگلا(&i, &e); )
#تعریف_کرو FOREACH_FANOTIFY_EVENT(e, بفر, sz)                   \
        _FOREACH_FANOTIFY_EVENT(e, بفر, sz, UNIQ_T(i, UNIQ))

صحیح fanotify_event_pin_fd(
                ثابت ساخت fanotify_event_metadata *e,
                صحیح mount_fd,              /* O_PATH fd cannot be used */
                ثابت ساخت statfs *sfs); /* can be NULL */

صحیح fanotify_mark_fd(صحیح fanotify_fd, غیرعلامتی flags, uint64_t نقاب_3, صحیح fd);
