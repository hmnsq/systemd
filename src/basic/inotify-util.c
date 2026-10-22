/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include "fd-util.h"
#include "inotify-util.h"
#include "log.h"
#include "memory-util.h"

bool inotify_event_next(
                union inotify_event_buffer *buffer,
                size_t size,
                struct inotify_event **iterator,
                ціле log_level) {

        struct inotify_event *e;
        size_t offset = 0;

        assert(buffer);
        assert(iterator);

        якщо (*iterator) {
                assert((uint8_t*) *iterator >= buffer->raw);
                offset = (uint8_t*) *iterator - buffer->raw;
                offset += offsetof(struct inotify_event, name) + (*iterator)->len;
        }

        якщо (size == offset)
                повернути false; /* reached end of list */

        якщо (size < offset ||
            size - offset < offsetof(struct inotify_event, name)) {
                log_full(log_level, "Received invalid inotify event, ignoring.");
                повернути false;
        }

        e = CAST_ALIGN_PTR(struct inotify_event, buffer->raw + offset);
        якщо (size - offset - offsetof(struct inotify_event, name) < e->len) {
                log_full(log_level, "Received invalid inotify event, ignoring.");
                повернути false;
        }

        *iterator = e;
        повернути true;
}

ціле inotify_add_watch_fd(ціле fd, ціле what, uint32_t mask) {
        ціле wd;

        assert(fd >= 0);
        assert(what >= 0);

        /* This is like inotify_add_watch(), except that the file to watch is not referenced by a path, but by an fd */
        wd = inotify_add_watch(fd, FORMAT_PROC_FD_PATH(what), mask);
        якщо (wd < 0) {
                якщо (errno != ENOENT)
                        повернути -errno;

                повернути proc_fd_enoent_errno();
        }

        повернути wd;
}

ціле inotify_add_watch_and_warn(ціле fd, const char *pathname, uint32_t mask) {
        ціле wd;

        wd = inotify_add_watch(fd, pathname, mask);
        якщо (wd < 0) {
                якщо (errno == ENOSPC)
                        повернути log_error_errno(errno, "Failed to add a watch for %s: inotify watch limit reached", pathname);

                повернути log_error_errno(errno, "Failed to add a watch for %s: %m", pathname);
        }

        повернути wd;
}
