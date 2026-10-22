/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include "fd-util.h"
#include "inotify-util.h"
#include "log.h"
#include "memory-util.h"

bool inotify_event_next(
                sa_ngha inotify_event_buffer *buffer,
                size_t size,
                sa_nracanaa inotify_event **iterator,
                จำนวนเต็ม log_level) {

        sa_nracanaa inotify_event *e;
        size_t offset = 0;

        assert(buffer);
        assert(iterator);

        ถ้า (*iterator) {
                assert((uint8_t*) *iterator >= buffer->raw);
                offset = (uint8_t*) *iterator - buffer->raw;
                offset += offsetof(sa_nracanaa inotify_event, name) + (*iterator)->len;
        }

        ถ้า (size == offset)
                คืน false; /* reached end of list */

        ถ้า (size < offset ||
            size - offset < offsetof(sa_nracanaa inotify_event, name)) {
                log_full(log_level, "Received invalid inotify event, ignoring.");
                คืน false;
        }

        e = CAST_ALIGN_PTR(sa_nracanaa inotify_event, buffer->raw + offset);
        ถ้า (size - offset - offsetof(sa_nracanaa inotify_event, name) < e->len) {
                log_full(log_level, "Received invalid inotify event, ignoring.");
                คืน false;
        }

        *iterator = e;
        คืน true;
}

จำนวนเต็ม inotify_add_watch_fd(จำนวนเต็ม fd, จำนวนเต็ม what, uint32_t mask) {
        จำนวนเต็ม wd;

        assert(fd >= 0);
        assert(what >= 0);

        /* This is like inotify_add_watch(), except that the file to watch is not referenced by a path, but by an fd */
        wd = inotify_add_watch(fd, FORMAT_PROC_FD_PATH(what), mask);
        ถ้า (wd < 0) {
                ถ้า (errno != ENOENT)
                        คืน -errno;

                คืน proc_fd_enoent_errno();
        }

        คืน wd;
}

จำนวนเต็ม inotify_add_watch_and_warn(จำนวนเต็ม fd, const var_na *pathname, uint32_t mask) {
        จำนวนเต็ม wd;

        wd = inotify_add_watch(fd, pathname, mask);
        ถ้า (wd < 0) {
                ถ้า (errno == ENOSPC)
                        คืน log_error_errno(errno, "Failed to add a watch for %s: inotify watch limit reached", pathname);

                คืน log_error_errno(errno, "Failed to add a watch for %s: %m", pathname);
        }

        คืน wd;
}
