/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ಅನ್ತರ್ಭಾವ "fd-util.h"
#ಅನ್ತರ್ಭಾವ "inotify-util.h"
#ಅನ್ತರ್ಭಾವ "log.h"
#ಅನ್ತರ್ಭಾವ "memory-util.h"

bool inotify_event_next(
                ಸಙ್ಘ inotify_event_buffer *buffer,
                size_t size,
                ಸಂರಚನಾ inotify_event **iterator,
                ಪೂರ್ಣಾಙ್ಕ log_level) {

        ಸಂರಚನಾ inotify_event *e;
        size_t offset = 0;

        ಪ್ರತಿಜ್ಞಾ(buffer);
        ಪ್ರತಿಜ್ಞಾ(iterator);

        ಯದಿ (*iterator) {
                ಪ್ರತಿಜ್ಞಾ((uint8_t*) *iterator >= buffer->raw);
                offset = (uint8_t*) *iterator - buffer->raw;
                offset += offsetof(ಸಂರಚನಾ inotify_event, name) + (*iterator)->len;
        }

        ಯದಿ (size == offset)
                ಪ್ರತ್ಯಾಗಮ false; /* reached end of list */

        ಯದಿ (size < offset ||
            size - offset < offsetof(ಸಂರಚನಾ inotify_event, name)) {
                log_full(log_level, "Received invalid inotify event, ignoring.");
                ಪ್ರತ್ಯಾಗಮ false;
        }

        e = CAST_ALIGN_PTR(ಸಂರಚನಾ inotify_event, buffer->raw + offset);
        ಯದಿ (size - offset - offsetof(ಸಂರಚನಾ inotify_event, name) < e->len) {
                log_full(log_level, "Received invalid inotify event, ignoring.");
                ಪ್ರತ್ಯಾಗಮ false;
        }

        *iterator = e;
        ಪ್ರತ್ಯಾಗಮ true;
}

ಪೂರ್ಣಾಙ್ಕ inotify_add_watch_fd(ಪೂರ್ಣಾಙ್ಕ fd, ಪೂರ್ಣಾಙ್ಕ what, uint32_t mask) {
        ಪೂರ್ಣಾಙ್ಕ wd;

        ಪ್ರತಿಜ್ಞಾ(fd >= 0);
        ಪ್ರತಿಜ್ಞಾ(what >= 0);

        /* This is like inotify_add_watch(), except that the file to watch is not referenced by a path, but by an fd */
        wd = inotify_add_watch(fd, FORMAT_PROC_FD_PATH(what), mask);
        ಯದಿ (wd < 0) {
                ಯದಿ (errno != ENOENT)
                        ಪ್ರತ್ಯಾಗಮ -errno;

                ಪ್ರತ್ಯಾಗಮ proc_fd_enoent_errno();
        }

        ಪ್ರತ್ಯಾಗಮ wd;
}

ಪೂರ್ಣಾಙ್ಕ inotify_add_watch_and_warn(ಪೂರ್ಣಾಙ್ಕ fd, ಧ್ರುವ ವರ್ಣ *pathname, uint32_t mask) {
        ಪೂರ್ಣಾಙ್ಕ wd;

        wd = inotify_add_watch(fd, pathname, mask);
        ಯದಿ (wd < 0) {
                ಯದಿ (errno == ENOSPC)
                        ಪ್ರತ್ಯಾಗಮ log_error_errno(errno, "Failed to add a watch for %s: inotify watch limit reached", pathname);

                ಪ್ರತ್ಯಾಗಮ log_error_errno(errno, "Failed to add a watch for %s: %m", pathname);
        }

        ಪ್ರತ್ಯಾಗಮ wd;
}
