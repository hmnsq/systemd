/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#ଅନ୍ତର୍ଭାଵ <sys/fanotify.h>       /* IWYU pragma: export */
#ଅନ୍ତର୍ଭାଵ <sys/vfs.h>            /* IWYU pragma: export */

#ଅନ୍ତର୍ଭାଵ "forward.h"

ସଂଘ fanotify_event_buffer {
        ସଂରଚନା fanotify_event_metadata em;
        uint8_t raw[4096];
};

ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା FanotifyEventIterator {
        ଅଚଳ uint8_t *ptr;                  /* cursor into the raw read buffer */
        size_t ବାମ;                         /* remaining bytes */
        ସଂଘ fanotify_event_buffer aligned; /* 8-byte aligned scratch for one event */
} FanotifyEventIterator;

bool fanotify_event_iterator_next(FanotifyEventIterator *i, ଅଚଳ ସଂରଚନା fanotify_event_metadata **ret);

/* Iterates over the events in a buffer filled by read() on an fanotify fd. Each event is copied into an
 * 8-byte aligned scratch buffer first, so 'e' is always suitably aligned. 'buffer' must be a
 * union fanotify_event_buffer, 'sz' the number of bytes read (must be >= 0). FAN_EVENT_OK()/
 * FAN_EVENT_NEXT() can't be used, as they dereference the metadata in place while possibly unaligned. */
#define _FOREACH_FANOTIFY_EVENT(e, buffer, sz, i)               \
        ପାଇଁ (FanotifyEventIterator i =                          \
                     { .ptr = (buffer).raw, .ବାମ = (sz), };    \
             i.ptr;                                             \
             i.ptr = ଖାଲି)                                      \
                ପାଇଁ (ଅଚଳ ସଂରଚନା fanotify_event_metadata *e;   \
                     fanotify_event_iterator_next(&i, &e); )
#define FOREACH_FANOTIFY_EVENT(e, buffer, sz)                   \
        _FOREACH_FANOTIFY_EVENT(e, buffer, sz, UNIQ_T(i, UNIQ))

ପୂର୍ଣ୍ଣସଂଖ୍ୟା fanotify_event_pin_fd(
                ଅଚଳ ସଂରଚନା fanotify_event_metadata *e,
                ପୂର୍ଣ୍ଣସଂଖ୍ୟା mount_fd,              /* O_PATH fd cannot be used */
                ଅଚଳ ସଂରଚନା statfs *sfs); /* can be NULL */

ପୂର୍ଣ୍ଣସଂଖ୍ୟା fanotify_mark_fd(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fanotify_fd, unsigned flags, uint64_t mask, ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd);
