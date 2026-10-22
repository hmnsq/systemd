/* SPDX-License-Identifier: LGPL-2.1-or-later */

#અન્તર્ભાવ "fd-util.h"
#અન્તર્ભાવ "inotify-util.h"
#અન્તર્ભાવ "log.h"
#અન્તર્ભાવ "memory-util.h"

bool inotify_event_next(
                સંઘ inotify_event_buffer *buffer,
                size_t size,
                સંરચના inotify_event **iterator,
                પૂર્ણાંક log_level) {

        સંરચના inotify_event *e;
        size_t offset = 0;

        પ્રતિજ્ઞા(buffer);
        પ્રતિજ્ઞા(iterator);

        જો (*iterator) {
                પ્રતિજ્ઞા((uint8_t*) *iterator >= buffer->raw);
                offset = (uint8_t*) *iterator - buffer->raw;
                offset += offsetof(સંરચના inotify_event, name) + (*iterator)->len;
        }

        જો (size == offset)
                પાછા false; /* reached end of list */

        જો (size < offset ||
            size - offset < offsetof(સંરચના inotify_event, name)) {
                log_full(log_level, "Received invalid inotify event, ignoring.");
                પાછા false;
        }

        e = CAST_ALIGN_PTR(સંરચના inotify_event, buffer->raw + offset);
        જો (size - offset - offsetof(સંરચના inotify_event, name) < e->len) {
                log_full(log_level, "Received invalid inotify event, ignoring.");
                પાછા false;
        }

        *iterator = e;
        પાછા true;
}

પૂર્ણાંક inotify_add_watch_fd(પૂર્ણાંક fd, પૂર્ણાંક what, uint32_t mask) {
        પૂર્ણાંક wd;

        પ્રતિજ્ઞા(fd >= 0);
        પ્રતિજ્ઞા(what >= 0);

        /* This is like inotify_add_watch(), except that the file to watch is not referenced by a path, but by an fd */
        wd = inotify_add_watch(fd, FORMAT_PROC_FD_PATH(what), mask);
        જો (wd < 0) {
                જો (errno != ENOENT)
                        પાછા -errno;

                પાછા proc_fd_enoent_errno();
        }

        પાછા wd;
}

પૂર્ણાંક inotify_add_watch_and_warn(પૂર્ણાંક fd, અચલ અક્ષર *pathname, uint32_t mask) {
        પૂર્ણાંક wd;

        wd = inotify_add_watch(fd, pathname, mask);
        જો (wd < 0) {
                જો (errno == ENOSPC)
                        પાછા log_error_errno(errno, "Failed to add a watch for %s: inotify watch limit reached", pathname);

                પાછા log_error_errno(errno, "Failed to add a watch for %s: %m", pathname);
        }

        પાછા wd;
}
