/* SPDX-License-Identifier: LGPL-2.1-or-later */
#సూచన once

#అన్తర్భావ <sys/fanotify.h>       /* IWYU pragma: export */
#అన్తర్భావ <sys/vfs.h>            /* IWYU pragma: export */

#అన్తర్భావ "forward.h"

సంఘం fanotify_event_బఫరు {
        నిర్మాణం fanotify_event_metadata em;
        uint8_t raw[4096];
};

రకనిర్వచనం నిర్మాణం FanotifyEventIterator {
        స్థిరరాశి uint8_t *ptr;                  /* cursor into the raw read buffer */
        size_t ఎడమ;                         /* remaining bytes */
        సంఘం fanotify_event_బఫరు aligned; /* 8-byte aligned scratch for one event */
} FanotifyEventIterator;

bool fanotify_event_iterator_తదుపరి(FanotifyEventIterator *i, స్థిరరాశి నిర్మాణం fanotify_event_metadata **ret);

/* Iterates over the events in a buffer filled by read() on an fanotify fd. Each event is copied into an
 * 8-byte aligned scratch buffer first, so 'e' is always suitably aligned. 'buffer' must be a
 * union fanotify_event_buffer, 'sz' the number of bytes read (must be >= 0). FAN_EVENT_OK()/
 * FAN_EVENT_NEXT() can't be used, as they dereference the metadata in place while possibly unaligned. */
#నిర్వచించు _FOREACH_FANOTIFY_EVENT(e, బఫరు, sz, i)               \
        కొరకు (FanotifyEventIterator i =                          \
                     { .ptr = (బఫరు).raw, .ఎడమ = (sz), };    \
             i.ptr;                                             \
             i.ptr = ఖాళీ)                                      \
                కొరకు (స్థిరరాశి నిర్మాణం fanotify_event_metadata *e;   \
                     fanotify_event_iterator_తదుపరి(&i, &e); )
#నిర్వచించు FOREACH_FANOTIFY_EVENT(e, బఫరు, sz)                   \
        _FOREACH_FANOTIFY_EVENT(e, బఫరు, sz, UNIQ_T(i, UNIQ))

పూర్ణాంకం fanotify_event_pin_fd(
                స్థిరరాశి నిర్మాణం fanotify_event_metadata *e,
                పూర్ణాంకం mount_fd,              /* O_PATH fd cannot be used */
                స్థిరరాశి నిర్మాణం statfs *sfs); /* can be NULL */

పూర్ణాంకం fanotify_mark_fd(పూర్ణాంకం fanotify_fd, గుర్తులేని flags, uint64_t ముసుగు_3, పూర్ణాంకం fd);
