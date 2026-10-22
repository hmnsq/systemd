/* SPDX-License-Identifier: LGPL-2.1-or-later */

#অন্তর্ভাব "fd-util.h"
#অন্তর্ভাব "inotify-util.h"
#অন্তর্ভাব "log.h"
#অন্তর্ভাব "memory-util.h"

bool inotify_event_পরবর্তী(
                সংঘ inotify_event_বাফার *বাফার,
                size_t আকার_2,
                কাঠামো inotify_event **iterator,
                পূর্ণসংখ্যা log_level) {

        কাঠামো inotify_event *e;
        size_t offset = 0;

        প্রতিজ্ঞা(বাফার);
        প্রতিজ্ঞা(iterator);

        যদি (*iterator) {
                প্রতিজ্ঞা((uint8_t*) *iterator >= বাফার->raw);
                offset = (uint8_t*) *iterator - বাফার->raw;
                offset += offsetof(কাঠামো inotify_event, name) + (*iterator)->len;
        }

        যদি (আকার_2 == offset)
                ফেরত false; /* reached end of list */

        যদি (আকার_2 < offset ||
            আকার_2 - offset < offsetof(কাঠামো inotify_event, name)) {
                log_full(log_level, "Received invalid inotify event, ignoring.");
                ফেরত false;
        }

        e = CAST_ALIGN_PTR(কাঠামো inotify_event, বাফার->raw + offset);
        যদি (আকার_2 - offset - offsetof(কাঠামো inotify_event, name) < e->len) {
                log_full(log_level, "Received invalid inotify event, ignoring.");
                ফেরত false;
        }

        *iterator = e;
        ফেরত true;
}

পূর্ণসংখ্যা inotify_add_watch_fd(পূর্ণসংখ্যা fd, পূর্ণসংখ্যা what, uint32_t mask) {
        পূর্ণসংখ্যা wd;

        প্রতিজ্ঞা(fd >= 0);
        প্রতিজ্ঞা(what >= 0);

        /* This is like inotify_add_watch(), except that the file to watch is not referenced by a path, but by an fd */
        wd = inotify_add_watch(fd, FORMAT_PROC_FD_PATH(what), mask);
        যদি (wd < 0) {
                যদি (errno != ENOENT)
                        ফেরত -errno;

                ফেরত proc_fd_enoent_errno();
        }

        ফেরত wd;
}

পূর্ণসংখ্যা inotify_add_watch_and_warn(পূর্ণসংখ্যা fd, ধ্রুবক অক্ষর *pathname, uint32_t mask) {
        পূর্ণসংখ্যা wd;

        wd = inotify_add_watch(fd, pathname, mask);
        যদি (wd < 0) {
                যদি (errno == ENOSPC)
                        ফেরত log_error_errno(errno, "Failed to add a watch for %s: inotify watch limit reached", pathname);

                ফেরত log_error_errno(errno, "Failed to add a watch for %s: %m", pathname);
        }

        ফেরত wd;
}
