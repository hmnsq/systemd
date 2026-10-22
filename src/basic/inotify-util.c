/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include "fd-util.h"
#include "inotify-util.h"
#include "log.h"
#include "memory-util.h"

bool inotify_event_next(
                union inotify_event_buffer *buffer,
                size_t size,
                struct inotify_event **iterator,
                цео log_level) {

        struct inotify_event *e;
        size_t offset = 0;

        assert(buffer);
        assert(iterator);

        ако (*iterator) {
                assert((uint8_t*) *iterator >= buffer->raw);
                offset = (uint8_t*) *iterator - buffer->raw;
                offset += offsetof(struct inotify_event, name) + (*iterator)->len;
        }

        ако (size == offset)
                врати false; /* reached end of list */

        ако (size < offset ||
            size - offset < offsetof(struct inotify_event, name)) {
                log_full(log_level, "Received invalid inotify event, ignoring.");
                врати false;
        }

        e = CAST_ALIGN_PTR(struct inotify_event, buffer->raw + offset);
        ако (size - offset - offsetof(struct inotify_event, name) < e->len) {
                log_full(log_level, "Received invalid inotify event, ignoring.");
                врати false;
        }

        *iterator = e;
        врати true;
}

цео inotify_add_watch_fd(цео fd, цео what, uint32_t mask) {
        цео wd;

        assert(fd >= 0);
        assert(what >= 0);

        /* This is like inotify_add_watch(), except that the file to watch is not referenced by a path, but by an fd */
        wd = inotify_add_watch(fd, FORMAT_PROC_FD_PATH(what), mask);
        ако (wd < 0) {
                ако (errno != ENOENT)
                        врати -errno;

                врати proc_fd_enoent_errno();
        }

        врати wd;
}

цео inotify_add_watch_and_warn(цео fd, const char *pathname, uint32_t mask) {
        цео wd;

        wd = inotify_add_watch(fd, pathname, mask);
        ако (wd < 0) {
                ако (errno == ENOSPC)
                        врати log_error_errno(errno, "Failed to add a watch for %s: inotify watch limit reached", pathname);

                врати log_error_errno(errno, "Failed to add a watch for %s: %m", pathname);
        }

        врати wd;
}
