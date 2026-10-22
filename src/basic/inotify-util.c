/* SPDX-License-Identifier: LGPL-2.1-or-later */

#அந்தர்பாவ "fd-util.h"
#அந்தர்பாவ "inotify-util.h"
#அந்தர்பாவ "log.h"
#அந்தர்பாவ "memory-util.h"

bool inotify_event_next(
                ஒன்றிப்பு inotify_event_buffer *buffer,
                size_t size,
                கட்டமைப்பு inotify_event **iterator,
                முழு log_level) {

        கட்டமைப்பு inotify_event *e;
        size_t offset = 0;

        ப்ரதிஜ்ஞா(buffer);
        ப்ரதிஜ்ஞா(iterator);

        எனில் (*iterator) {
                ப்ரதிஜ்ஞா((uint8_t*) *iterator >= buffer->raw);
                offset = (uint8_t*) *iterator - buffer->raw;
                offset += offsetof(கட்டமைப்பு inotify_event, name) + (*iterator)->len;
        }

        எனில் (size == offset)
                திரும்பு false; /* reached end of list */

        எனில் (size < offset ||
            size - offset < offsetof(கட்டமைப்பு inotify_event, name)) {
                log_full(log_level, "Received invalid inotify event, ignoring.");
                திரும்பு false;
        }

        e = CAST_ALIGN_PTR(கட்டமைப்பு inotify_event, buffer->raw + offset);
        எனில் (size - offset - offsetof(கட்டமைப்பு inotify_event, name) < e->len) {
                log_full(log_level, "Received invalid inotify event, ignoring.");
                திரும்பு false;
        }

        *iterator = e;
        திரும்பு true;
}

முழு inotify_add_watch_fd(முழு fd, முழு what, uint32_t mask) {
        முழு wd;

        ப்ரதிஜ்ஞா(fd >= 0);
        ப்ரதிஜ்ஞா(what >= 0);

        /* This is like inotify_add_watch(), except that the file to watch is not referenced by a path, but by an fd */
        wd = inotify_add_watch(fd, FORMAT_PROC_FD_PATH(what), mask);
        எனில் (wd < 0) {
                எனில் (errno != ENOENT)
                        திரும்பு -errno;

                திரும்பு proc_fd_enoent_errno();
        }

        திரும்பு wd;
}

முழு inotify_add_watch_and_warn(முழு fd, மாறிலி எழுத்து *pathname, uint32_t mask) {
        முழு wd;

        wd = inotify_add_watch(fd, pathname, mask);
        எனில் (wd < 0) {
                எனில் (errno == ENOSPC)
                        திரும்பு log_error_errno(errno, "Failed to add a watch for %s: inotify watch limit reached", pathname);

                திரும்பு log_error_errno(errno, "Failed to add a watch for %s: %m", pathname);
        }

        திரும்பு wd;
}
