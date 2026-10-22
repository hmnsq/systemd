/* SPDX-License-Identifier: LGPL-2.1-or-later */

#شامل "fd-util.h"
#شامل "inotify-util.h"
#شامل "log.h"
#شامل "memory-util.h"

bool inotify_event_next(
                اجتماع inotify_event_buffer *buffer,
                size_t size,
                ساختار inotify_event **iterator,
                صحیح log_level) {

        ساختار inotify_event *e;
        size_t offset = 0;

        أكد(buffer);
        أكد(iterator);

        اگر (*iterator) {
                أكد((uint8_t*) *iterator >= buffer->raw);
                offset = (uint8_t*) *iterator - buffer->raw;
                offset += offsetof(ساختار inotify_event, name) + (*iterator)->len;
        }

        اگر (size == offset)
                بازگشت false; /* reached end of list */

        اگر (size < offset ||
            size - offset < offsetof(ساختار inotify_event, name)) {
                log_full(log_level, "Received invalid inotify event, ignoring.");
                بازگشت false;
        }

        e = CAST_ALIGN_PTR(ساختار inotify_event, buffer->raw + offset);
        اگر (size - offset - offsetof(ساختار inotify_event, name) < e->len) {
                log_full(log_level, "Received invalid inotify event, ignoring.");
                بازگشت false;
        }

        *iterator = e;
        بازگشت true;
}

صحیح inotify_add_watch_fd(صحیح fd, صحیح what, uint32_t mask) {
        صحیح wd;

        أكد(fd >= 0);
        أكد(what >= 0);

        /* This is like inotify_add_watch(), except that the file to watch is not referenced by a path, but by an fd */
        wd = inotify_add_watch(fd, FORMAT_PROC_FD_PATH(what), mask);
        اگر (wd < 0) {
                اگر (errno != ENOENT)
                        بازگشت -errno;

                بازگشت proc_fd_enoent_errno();
        }

        بازگشت wd;
}

صحیح inotify_add_watch_and_warn(صحیح fd, ثابت نویسه *pathname, uint32_t mask) {
        صحیح wd;

        wd = inotify_add_watch(fd, pathname, mask);
        اگر (wd < 0) {
                اگر (errno == ENOSPC)
                        بازگشت log_error_errno(errno, "Failed to add a watch for %s: inotify watch limit reached", pathname);

                بازگشت log_error_errno(errno, "Failed to add a watch for %s: %m", pathname);
        }

        بازگشت wd;
}
