/* SPDX-License-Identifier: LGPL-2.1-or-later */

#अन्तर्भाव "fd-util.h"
#अन्तर्भाव "inotify-util.h"
#अन्तर्भाव "log.h"
#अन्तर्भाव "memory-util.h"

bool inotify_event_next(
                सङ्घ inotify_event_buffer *buffer,
                size_t size,
                संरचना inotify_event **iterator,
                पूर्णाङ्क log_level) {

        संरचना inotify_event *e;
        size_t offset = 0;

        प्रतिज्ञा(buffer);
        प्रतिज्ञा(iterator);

        सचे (*iterator) {
                प्रतिज्ञा((uint8_t*) *iterator >= buffer->raw);
                offset = (uint8_t*) *iterator - buffer->raw;
                offset += offsetof(संरचना inotify_event, name) + (*iterator)->len;
        }

        सचे (size == offset)
                पटिनिवत्त false; /* reached end of list */

        सचे (size < offset ||
            size - offset < offsetof(संरचना inotify_event, name)) {
                log_full(log_level, "Received invalid inotify event, ignoring.");
                पटिनिवत्त false;
        }

        e = CAST_ALIGN_PTR(संरचना inotify_event, buffer->raw + offset);
        सचे (size - offset - offsetof(संरचना inotify_event, name) < e->len) {
                log_full(log_level, "Received invalid inotify event, ignoring.");
                पटिनिवत्त false;
        }

        *iterator = e;
        पटिनिवत्त true;
}

पूर्णाङ्क inotify_add_watch_fd(पूर्णाङ्क fd, पूर्णाङ्क what, uint32_t mask) {
        पूर्णाङ्क wd;

        प्रतिज्ञा(fd >= 0);
        प्रतिज्ञा(what >= 0);

        /* This is like inotify_add_watch(), except that the file to watch is not referenced by a path, but by an fd */
        wd = inotify_add_watch(fd, FORMAT_PROC_FD_PATH(what), mask);
        सचे (wd < 0) {
                सचे (errno != ENOENT)
                        पटिनिवत्त -errno;

                पटिनिवत्त proc_fd_enoent_errno();
        }

        पटिनिवत्त wd;
}

पूर्णाङ्क inotify_add_watch_and_warn(पूर्णाङ्क fd, ध्रुव वर्ण *pathname, uint32_t mask) {
        पूर्णाङ्क wd;

        wd = inotify_add_watch(fd, pathname, mask);
        सचे (wd < 0) {
                सचे (errno == ENOSPC)
                        पटिनिवत्त log_error_errno(errno, "Failed to add a watch for %s: inotify watch limit reached", pathname);

                पटिनिवत्त log_error_errno(errno, "Failed to add a watch for %s: %m", pathname);
        }

        पटिनिवत्त wd;
}
