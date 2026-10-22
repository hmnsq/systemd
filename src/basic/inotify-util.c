/* SPDX-License-Identifier: LGPL-2.1-or-later */

#شامل "fd-util.h"
#شامل "inotify-util.h"
#شامل "log.h"
#شامل "memory-util.h"

bool inotify_event_اگلا(
                اتحاد inotify_event_بفر *بفر,
                size_t سائز,
                ساخت inotify_event **iterator,
                صحیح log_level) {

        ساخت inotify_event *e;
        size_t انحراف = 0;

        أكد(بفر);
        أكد(iterator);

        اگر (*iterator) {
                أكد((uint8_t*) *iterator >= بفر->raw);
                انحراف = (uint8_t*) *iterator - بفر->raw;
                انحراف += offsetof(ساخت inotify_event, name) + (*iterator)->len;
        }

        اگر (سائز == انحراف)
                واپس false; /* reached end of list */

        اگر (سائز < انحراف ||
            سائز - انحراف < offsetof(ساخت inotify_event, name)) {
                log_بھرا(log_level, "Received invalid inotify event, ignoring.");
                واپس false;
        }

        e = CAST_ALIGN_PTR(ساخت inotify_event, بفر->raw + انحراف);
        اگر (سائز - انحراف - offsetof(ساخت inotify_event, name) < e->len) {
                log_بھرا(log_level, "Received invalid inotify event, ignoring.");
                واپس false;
        }

        *iterator = e;
        واپس true;
}

صحیح inotify_جوڑو_watch_fd(صحیح fd, صحیح what, uint32_t نقاب_3) {
        صحیح wd;

        أكد(fd >= 0);
        أكد(what >= 0);

        /* This is like inotify_add_watch(), except that the file to watch is not referenced by a path, but by an fd */
        wd = inotify_add_watch(fd, FORMAT_PROC_FD_راستہ(what), نقاب_3);
        اگر (wd < 0) {
                اگر (errno != ENOENT)
                        واپس -errno;

                واپس proc_fd_enoent_errno();
        }

        واپس wd;
}

صحیح inotify_جوڑو_watch_and_warn(صحیح fd, ثابت حرف *pathname, uint32_t نقاب_3) {
        صحیح wd;

        wd = inotify_add_watch(fd, pathname, نقاب_3);
        اگر (wd < 0) {
                اگر (errno == ENOSPC)
                        واپس log_error_errno(errno, "Failed to add a watch for %s: inotify watch limit reached", pathname);

                واپس log_error_errno(errno, "Failed to add a watch for %s: %m", pathname);
        }

        واپس wd;
}
