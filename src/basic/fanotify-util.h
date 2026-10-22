/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#અન્તર્ભાવ <sys/fanotify.h>       /* IWYU pragma: export */
#અન્તર્ભાવ <sys/vfs.h>            /* IWYU pragma: export */

#અન્તર્ભાવ "forward.h"

સંઘ fanotify_event_buffer {
        સંરચના fanotify_event_metadata em;
        uint8_t raw[4096];
};

પ્રકારવ્યાખ્યા સંરચના FanotifyEventIterator {
        અચલ uint8_t *ptr;                  /* cursor into the raw read buffer */
        size_t ડાબું;                         /* remaining bytes */
        સંઘ fanotify_event_buffer aligned; /* 8-byte aligned scratch for one event */
} FanotifyEventIterator;

bool fanotify_event_iterator_next(FanotifyEventIterator *i, અચલ સંરચના fanotify_event_metadata **ret);

/* Iterates over the events in a buffer filled by read() on an fanotify fd. Each event is copied into an
 * 8-byte aligned scratch buffer first, so 'e' is always suitably aligned. 'buffer' must be a
 * union fanotify_event_buffer, 'sz' the number of bytes read (must be >= 0). FAN_EVENT_OK()/
 * FAN_EVENT_NEXT() can't be used, as they dereference the metadata in place while possibly unaligned. */
#define _FOREACH_FANOTIFY_EVENT(e, buffer, sz, i)               \
        માટે (FanotifyEventIterator i =                          \
                     { .ptr = (buffer).raw, .ડાબું = (sz), };    \
             i.ptr;                                             \
             i.ptr = ખાલી)                                      \
                માટે (અચલ સંરચના fanotify_event_metadata *e;   \
                     fanotify_event_iterator_next(&i, &e); )
#define FOREACH_FANOTIFY_EVENT(e, buffer, sz)                   \
        _FOREACH_FANOTIFY_EVENT(e, buffer, sz, UNIQ_T(i, UNIQ))

પૂર્ણાંક fanotify_event_pin_fd(
                અચલ સંરચના fanotify_event_metadata *e,
                પૂર્ણાંક mount_fd,              /* O_PATH fd cannot be used */
                અચલ સંરચના statfs *sfs); /* can be NULL */

પૂર્ણાંક fanotify_mark_fd(પૂર્ણાંક fanotify_fd, unsigned flags, uint64_t mask, પૂર્ણાંક fd);
