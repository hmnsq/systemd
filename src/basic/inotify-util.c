/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ضمن "fd-util.h"
#ضمن "inotify-util.h"
#ضمن "log.h"
#ضمن "memory-util.h"

bool inotify_event_next(
                اتحاد inotify_event_buffer *buffer,
                size_t size,
                بنية inotify_event **iterator,
                صحيح log_level) {

        بنية inotify_event *e;
        size_t offset = 0;

        أكد(buffer);
        أكد(iterator);

        אם (*iterator) {
                أكد((uint8_t*) *iterator >= buffer->raw);
                offset = (uint8_t*) *iterator - buffer->raw;
                offset += offsetof(بنية inotify_event, name) + (*iterator)->len;
        }

        אם (size == offset)
                החזר false; /* reached end of list */

        אם (size < offset ||
            size - offset < offsetof(بنية inotify_event, name)) {
                log_full(log_level, "Received invalid inotify event, ignoring.");
                החזר false;
        }

        e = CAST_ALIGN_PTR(بنية inotify_event, buffer->raw + offset);
        אם (size - offset - offsetof(بنية inotify_event, name) < e->len) {
                log_full(log_level, "Received invalid inotify event, ignoring.");
                החזר false;
        }

        *iterator = e;
        החזר true;
}

صحيح inotify_add_watch_fd(صحيح fd, صحيح what, uint32_t mask) {
        صحيح wd;

        أكد(fd >= 0);
        أكد(what >= 0);

        /* This is like inotify_add_watch(), except that the file to watch is not referenced by a path, but by an fd */
        wd = inotify_add_watch(fd, FORMAT_PROC_FD_PATH(what), mask);
        אם (wd < 0) {
                אם (errno != ENOENT)
                        החזר -errno;

                החזר proc_fd_enoent_errno();
        }

        החזר wd;
}

صحيح inotify_add_watch_and_warn(صحيح fd, ثابت حرف *pathname, uint32_t mask) {
        صحيح wd;

        wd = inotify_add_watch(fd, pathname, mask);
        אם (wd < 0) {
                אם (errno == ENOSPC)
                        החזר log_error_errno(errno, "Failed to add a watch for %s: inotify watch limit reached", pathname);

                החזר log_error_errno(errno, "Failed to add a watch for %s: %m", pathname);
        }

        החזר wd;
}
