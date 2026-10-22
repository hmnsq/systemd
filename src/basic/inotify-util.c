/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include "fd-util.h"
#include "inotify-util.h"
#include "log.h"
#include "memory-util.h"

bool inotify_event_next(
                union inotify_event_buffer *buffer,
                size_t size,
                struct inotify_event **iterator,
                целое log_level) {

        struct inotify_event *e;
        size_t offset = 0;

        assert(buffer);
        assert(iterator);

        если (*iterator) {
                assert((uint8_t*) *iterator >= buffer->raw);
                offset = (uint8_t*) *iterator - buffer->raw;
                offset += offsetof(struct inotify_event, name) + (*iterator)->len;
        }

        если (size == offset)
                вернуть false; /* reached end of list */

        если (size < offset ||
            size - offset < offsetof(struct inotify_event, name)) {
                log_full(log_level, "Received invalid inotify event, ignoring.");
                вернуть false;
        }

        e = CAST_ALIGN_PTR(struct inotify_event, buffer->raw + offset);
        если (size - offset - offsetof(struct inotify_event, name) < e->len) {
                log_full(log_level, "Received invalid inotify event, ignoring.");
                вернуть false;
        }

        *iterator = e;
        вернуть true;
}

целое inotify_add_watch_fd(целое fd, целое what, uint32_t mask) {
        целое wd;

        assert(fd >= 0);
        assert(what >= 0);

        /* This is like inotify_add_watch(), except that the file to watch is not referenced by a path, but by an fd */
        wd = inotify_add_watch(fd, FORMAT_PROC_FD_PATH(what), mask);
        если (wd < 0) {
                если (errno != ENOENT)
                        вернуть -errno;

                вернуть proc_fd_enoent_errno();
        }

        вернуть wd;
}

целое inotify_add_watch_and_warn(целое fd, const char *pathname, uint32_t mask) {
        целое wd;

        wd = inotify_add_watch(fd, pathname, mask);
        если (wd < 0) {
                если (errno == ENOSPC)
                        вернуть log_error_errno(errno, "Failed to add a watch for %s: inotify watch limit reached", pathname);

                вернуть log_error_errno(errno, "Failed to add a watch for %s: %m", pathname);
        }

        вернуть wd;
}
