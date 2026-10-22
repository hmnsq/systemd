/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include "fd-util.h"
#include "inotify-util.h"
#include "log.h"
#include "memory-util.h"

bool inotify_event_next(
                union inotify_event_buffer *buffer,
                size_t size,
                struct inotify_event **iterator,
                정수 log_level) {

        struct inotify_event *e;
        size_t offset = 0;

        assert(buffer);
        assert(iterator);

        만약 (*iterator) {
                assert((uint8_t*) *iterator >= buffer->raw);
                offset = (uint8_t*) *iterator - buffer->raw;
                offset += offsetof(struct inotify_event, name) + (*iterator)->len;
        }

        만약 (size == offset)
                반환 false; /* reached end of list */

        만약 (size < offset ||
            size - offset < offsetof(struct inotify_event, name)) {
                log_full(log_level, "Received invalid inotify event, ignoring.");
                반환 false;
        }

        e = CAST_ALIGN_PTR(struct inotify_event, buffer->raw + offset);
        만약 (size - offset - offsetof(struct inotify_event, name) < e->len) {
                log_full(log_level, "Received invalid inotify event, ignoring.");
                반환 false;
        }

        *iterator = e;
        반환 true;
}

정수 inotify_add_watch_fd(정수 fd, 정수 what, uint32_t mask) {
        정수 wd;

        assert(fd >= 0);
        assert(what >= 0);

        /* This is like inotify_add_watch(), except that the file to watch is not referenced by a path, but by an fd */
        wd = inotify_add_watch(fd, FORMAT_PROC_FD_PATH(what), mask);
        만약 (wd < 0) {
                만약 (errno != ENOENT)
                        반환 -errno;

                반환 proc_fd_enoent_errno();
        }

        반환 wd;
}

정수 inotify_add_watch_and_warn(정수 fd, const char *pathname, uint32_t mask) {
        정수 wd;

        wd = inotify_add_watch(fd, pathname, mask);
        만약 (wd < 0) {
                만약 (errno == ENOSPC)
                        반환 log_error_errno(errno, "Failed to add a watch for %s: inotify watch limit reached", pathname);

                반환 log_error_errno(errno, "Failed to add a watch for %s: %m", pathname);
        }

        반환 wd;
}
