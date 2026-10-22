/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ضمن "fd-util.h"
#ضمن "inotify-util.h"
#ضمن "log.h"
#ضمن "memory-util.h"

bool inotify_event_بعدی(
                اتحاد inotify_event_میانگیر *میانگیر,
                size_t اندازه,
                بنية inotify_event **iterator,
                صحيح log_level) {

        بنية inotify_event *e;
        size_t جابه_جایی = 0;

        أكد(میانگیر);
        أكد(iterator);

        إذا (*iterator) {
                أكد((uint8_t*) *iterator >= میانگیر->raw);
                جابه_جایی = (uint8_t*) *iterator - میانگیر->raw;
                جابه_جایی += offsetof(بنية inotify_event, name) + (*iterator)->len;
        }

        إذا (اندازه == جابه_جایی)
                أرجع false; /* reached end of list */

        إذا (اندازه < جابه_جایی ||
            اندازه - جابه_جایی < offsetof(بنية inotify_event, name)) {
                log_پر(log_level, "Received invalid inotify event, ignoring.");
                أرجع false;
        }

        e = CAST_ALIGN_PTR(بنية inotify_event, میانگیر->raw + جابه_جایی);
        إذا (اندازه - جابه_جایی - offsetof(بنية inotify_event, name) < e->len) {
                log_پر(log_level, "Received invalid inotify event, ignoring.");
                أرجع false;
        }

        *iterator = e;
        أرجع true;
}

صحيح inotify_جمع_کن_watch_fd(صحيح fd, صحيح what, uint32_t ماسک_3) {
        صحيح wd;

        أكد(fd >= 0);
        أكد(what >= 0);

        /* This is like inotify_add_watch(), except that the file to watch is not referenced by a path, but by an fd */
        wd = inotify_add_watch(fd, FORMAT_PROC_FD_مسیر(what), ماسک_3);
        إذا (wd < 0) {
                إذا (errno != ENOENT)
                        أرجع -errno;

                أرجع proc_fd_enoent_errno();
        }

        أرجع wd;
}

صحيح inotify_جمع_کن_watch_and_warn(صحيح fd, ثابت حرف *pathname, uint32_t ماسک_3) {
        صحيح wd;

        wd = inotify_add_watch(fd, pathname, ماسک_3);
        إذا (wd < 0) {
                إذا (errno == ENOSPC)
                        أرجع log_error_errno(errno, "Failed to add a watch for %s: inotify watch limit reached", pathname);

                أرجع log_error_errno(errno, "Failed to add a watch for %s: %m", pathname);
        }

        أرجع wd;
}
