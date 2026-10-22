/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ضمن "fd-util.h"
#ضمن "inotify-util.h"
#ضمن "log.h"
#ضمن "memory-util.h"

bool inotify_event_اگلا(
                اتحاد inotify_event_بفر *بفر,
                size_t ناپ,
                بنية inotify_event **iterator,
                صحيح log_level) {

        بنية inotify_event *e;
        size_t وتھ = 0;

        أكد(بفر);
        أكد(iterator);

        إذا (*iterator) {
                أكد((uint8_t*) *iterator >= بفر->raw);
                وتھ = (uint8_t*) *iterator - بفر->raw;
                وتھ += offsetof(بنية inotify_event, name) + (*iterator)->len;
        }

        إذا (ناپ == وتھ)
                أرجع false; /* reached end of list */

        إذا (ناپ < وتھ ||
            ناپ - وتھ < offsetof(بنية inotify_event, name)) {
                log_بھریا(log_level, "Received invalid inotify event, ignoring.");
                أرجع false;
        }

        e = CAST_ALIGN_PTR(بنية inotify_event, بفر->raw + وتھ);
        إذا (ناپ - وتھ - offsetof(بنية inotify_event, name) < e->len) {
                log_بھریا(log_level, "Received invalid inotify event, ignoring.");
                أرجع false;
        }

        *iterator = e;
        أرجع true;
}

صحيح inotify_جوڑو_watch_fd(صحيح fd, صحيح what, uint32_t نقاب_3) {
        صحيح wd;

        أكد(fd >= 0);
        أكد(what >= 0);

        /* This is like inotify_add_watch(), except that the file to watch is not referenced by a path, but by an fd */
        wd = inotify_add_watch(fd, FORMAT_PROC_FD_رستہ(what), نقاب_3);
        إذا (wd < 0) {
                إذا (errno != ENOENT)
                        أرجع -errno;

                أرجع proc_fd_enoent_errno();
        }

        أرجع wd;
}

صحيح inotify_جوڑو_watch_and_warn(صحيح fd, ثابت حرف *pathname, uint32_t نقاب_3) {
        صحيح wd;

        wd = inotify_add_watch(fd, pathname, نقاب_3);
        إذا (wd < 0) {
                إذا (errno == ENOSPC)
                        أرجع log_error_errno(errno, "Failed to add a watch for %s: inotify watch limit reached", pathname);

                أرجع log_error_errno(errno, "Failed to add a watch for %s: %m", pathname);
        }

        أرجع wd;
}
