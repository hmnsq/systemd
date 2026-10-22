/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include "fd-util.h"
#include "inotify-util.h"
#include "log.h"
#include "memory-util.h"

bool inotify_event_next(
                union inotify_event_buffer *buffer,
                size_t size,
                struct inotify_event **iterator,
                số_nguyên log_level) {

        struct inotify_event *e;
        size_t offset = 0;

        assert(buffer);
        assert(iterator);

        nếu (*iterator) {
                assert((uint8_t*) *iterator >= buffer->raw);
                offset = (uint8_t*) *iterator - buffer->raw;
                offset += offsetof(struct inotify_event, name) + (*iterator)->len;
        }

        nếu (size == offset)
                trả_về false; /* reached end of list */

        nếu (size < offset ||
            size - offset < offsetof(struct inotify_event, name)) {
                log_full(log_level, "Received invalid inotify event, ignoring.");
                trả_về false;
        }

        e = CAST_ALIGN_PTR(struct inotify_event, buffer->raw + offset);
        nếu (size - offset - offsetof(struct inotify_event, name) < e->len) {
                log_full(log_level, "Received invalid inotify event, ignoring.");
                trả_về false;
        }

        *iterator = e;
        trả_về true;
}

số_nguyên inotify_add_watch_fd(số_nguyên fd, số_nguyên what, uint32_t mask) {
        số_nguyên wd;

        assert(fd >= 0);
        assert(what >= 0);

        /* This is like inotify_add_watch(), except that the file to watch is not referenced by a path, but by an fd */
        wd = inotify_add_watch(fd, FORMAT_PROC_FD_PATH(what), mask);
        nếu (wd < 0) {
                nếu (errno != ENOENT)
                        trả_về -errno;

                trả_về proc_fd_enoent_errno();
        }

        trả_về wd;
}

số_nguyên inotify_add_watch_and_warn(số_nguyên fd, const char *pathname, uint32_t mask) {
        số_nguyên wd;

        wd = inotify_add_watch(fd, pathname, mask);
        nếu (wd < 0) {
                nếu (errno == ENOSPC)
                        trả_về log_error_errno(errno, "Failed to add a watch for %s: inotify watch limit reached", pathname);

                trả_về log_error_errno(errno, "Failed to add a watch for %s: %m", pathname);
        }

        trả_về wd;
}
