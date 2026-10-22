/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include "fd-util.h"
#include "inotify-util.h"
#include "log.h"
#include "memory-util.h"

bool inotify_event_next(
                union inotify_event_buffer *buffer,
                size_t size,
                struct inotify_event **iterator,
                整数 log_level) {

        struct inotify_event *e;
        size_t offset = 0;

        assert(buffer);
        assert(iterator);

        もし (*iterator) {
                assert((uint8_t*) *iterator >= buffer->raw);
                offset = (uint8_t*) *iterator - buffer->raw;
                offset += offsetof(struct inotify_event, name) + (*iterator)->len;
        }

        もし (size == offset)
                戻る false; /* reached end of list */

        もし (size < offset ||
            size - offset < offsetof(struct inotify_event, name)) {
                log_full(log_level, "Received invalid inotify event, ignoring.");
                戻る false;
        }

        e = CAST_ALIGN_PTR(struct inotify_event, buffer->raw + offset);
        もし (size - offset - offsetof(struct inotify_event, name) < e->len) {
                log_full(log_level, "Received invalid inotify event, ignoring.");
                戻る false;
        }

        *iterator = e;
        戻る true;
}

整数 inotify_add_watch_fd(整数 fd, 整数 what, uint32_t mask) {
        整数 wd;

        assert(fd >= 0);
        assert(what >= 0);

        /* This is like inotify_add_watch(), except that the file to watch is not referenced by a path, but by an fd */
        wd = inotify_add_watch(fd, FORMAT_PROC_FD_PATH(what), mask);
        もし (wd < 0) {
                もし (errno != ENOENT)
                        戻る -errno;

                戻る proc_fd_enoent_errno();
        }

        戻る wd;
}

整数 inotify_add_watch_and_warn(整数 fd, const char *pathname, uint32_t mask) {
        整数 wd;

        wd = inotify_add_watch(fd, pathname, mask);
        もし (wd < 0) {
                もし (errno == ENOSPC)
                        戻る log_error_errno(errno, "Failed to add a watch for %s: inotify watch limit reached", pathname);

                戻る log_error_errno(errno, "Failed to add a watch for %s: %m", pathname);
        }

        戻る wd;
}
