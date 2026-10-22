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

        𐤀𐤌 (*iterator) {
                أكد((uint8_t*) *iterator >= buffer->raw);
                offset = (uint8_t*) *iterator - buffer->raw;
                offset += offsetof(بنية inotify_event, name) + (*iterator)->len;
        }

        𐤀𐤌 (size == offset)
                أرجع false; /* reached end of list */

        𐤀𐤌 (size < offset ||
            size - offset < offsetof(بنية inotify_event, name)) {
                log_full(log_level, "Received invalid inotify event, ignoring.");
                أرجع false;
        }

        e = CAST_ALIGN_PTR(بنية inotify_event, buffer->raw + offset);
        𐤀𐤌 (size - offset - offsetof(بنية inotify_event, name) < e->len) {
                log_full(log_level, "Received invalid inotify event, ignoring.");
                أرجع false;
        }

        *iterator = e;
        أرجع true;
}

صحيح inotify_add_watch_fd(صحيح fd, صحيح what, uint32_t mask) {
        صحيح wd;

        أكد(fd >= 0);
        أكد(what >= 0);

        /* This is like inotify_add_watch(), except that the file to watch is not referenced by a path, but by an fd */
        wd = inotify_add_watch(fd, FORMAT_PROC_FD_PATH(what), mask);
        𐤀𐤌 (wd < 0) {
                𐤀𐤌 (errno != ENOENT)
                        أرجع -errno;

                أرجع proc_fd_enoent_errno();
        }

        أرجع wd;
}

صحيح inotify_add_watch_and_warn(صحيح fd, ثابت حرف *pathname, uint32_t mask) {
        صحيح wd;

        wd = inotify_add_watch(fd, pathname, mask);
        𐤀𐤌 (wd < 0) {
                𐤀𐤌 (errno == ENOSPC)
                        أرجع log_error_errno(errno, "Failed to add a watch for %s: inotify watch limit reached", pathname);

                أرجع log_error_errno(errno, "Failed to add a watch for %s: %m", pathname);
        }

        أرجع wd;
}
