/* SPDX-License-Identifier: LGPL-2.1-or-later */

#অন্তর্ভাৱ "fd-util.h"
#অন্তর্ভাৱ "inotify-util.h"
#অন্তর্ভাৱ "log.h"
#অন্তর্ভাৱ "memory-util.h"

bool inotify_event_next(
                সঙ্ঘ inotify_event_buffer *buffer,
                size_t size,
                সংরচনা inotify_event **iterator,
                পূর্ণাঙ্ক log_level) {

        সংরচনা inotify_event *e;
        size_t offset = 0;

        প্রতিজ্ঞা(buffer);
        প্রতিজ্ঞা(iterator);

        যদি (*iterator) {
                প্রতিজ্ঞা((uint8_t*) *iterator >= buffer->raw);
                offset = (uint8_t*) *iterator - buffer->raw;
                offset += offsetof(সংরচনা inotify_event, name) + (*iterator)->len;
        }

        যদি (size == offset)
                ঘূৰাই false; /* reached end of list */

        যদি (size < offset ||
            size - offset < offsetof(সংরচনা inotify_event, name)) {
                log_full(log_level, "Received invalid inotify event, ignoring.");
                ঘূৰাই false;
        }

        e = CAST_ALIGN_PTR(সংরচনা inotify_event, buffer->raw + offset);
        যদি (size - offset - offsetof(সংরচনা inotify_event, name) < e->len) {
                log_full(log_level, "Received invalid inotify event, ignoring.");
                ঘূৰাই false;
        }

        *iterator = e;
        ঘূৰাই true;
}

পূর্ণাঙ্ক inotify_add_watch_fd(পূর্ণাঙ্ক fd, পূর্ণাঙ্ক what, uint32_t mask) {
        পূর্ণাঙ্ক wd;

        প্রতিজ্ঞা(fd >= 0);
        প্রতিজ্ঞা(what >= 0);

        /* This is like inotify_add_watch(), except that the file to watch is not referenced by a path, but by an fd */
        wd = inotify_add_watch(fd, FORMAT_PROC_FD_PATH(what), mask);
        যদি (wd < 0) {
                যদি (errno != ENOENT)
                        ঘূৰাই -errno;

                ঘূৰাই proc_fd_enoent_errno();
        }

        ঘূৰাই wd;
}

পূর্ণাঙ্ক inotify_add_watch_and_warn(পূর্ণাঙ্ক fd, ধ্রুৱ ৱর্ণ *pathname, uint32_t mask) {
        পূর্ণাঙ্ক wd;

        wd = inotify_add_watch(fd, pathname, mask);
        যদি (wd < 0) {
                যদি (errno == ENOSPC)
                        ঘূৰাই log_error_errno(errno, "Failed to add a watch for %s: inotify watch limit reached", pathname);

                ঘূৰাই log_error_errno(errno, "Failed to add a watch for %s: %m", pathname);
        }

        ঘূৰাই wd;
}
