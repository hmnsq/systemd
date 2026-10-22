/* SPDX-License-Identifier: LGPL-2.1-or-later */

#अन्तर्भाव "fd-util.h"
#अन्तर्भाव "inotify-util.h"
#अन्तर्भाव "log.h"
#अन्तर्भाव "memory-util.h"

bool inotify_event_next(
                सङ्घ inotify_event_buffer *buffer,
                माप_प्रकार size,
                संरचना inotify_event **iterator,
                पूर्णाङ्क log_level) {

        संरचना inotify_event *e;
        माप_प्रकार offset = 0;

        प्रतिज्ञा(buffer);
        प्रतिज्ञा(iterator);

        यदि (*iterator) {
                प्रतिज्ञा((uint8_t*) *iterator >= buffer->raw);
                offset = (uint8_t*) *iterator - buffer->raw;
                offset += दुरत्व(संरचना inotify_event, name) + (*iterator)->len;
        }

        यदि (size == offset)
                प्रत्यागम false; /* reached end of list */

        यदि (size < offset ||
            size - offset < दुरत्व(संरचना inotify_event, name)) {
                log_full(log_level, "Received invalid inotify event, ignoring.");
                प्रत्यागम false;
        }

        e = CAST_ALIGN_PTR(संरचना inotify_event, buffer->raw + offset);
        यदि (size - offset - दुरत्व(संरचना inotify_event, name) < e->len) {
                log_full(log_level, "Received invalid inotify event, ignoring.");
                प्रत्यागम false;
        }

        *iterator = e;
        प्रत्यागम true;
}

पूर्णाङ्क inotify_add_watch_fd(पूर्णाङ्क fd, पूर्णाङ्क what, uint32_t mask) {
        पूर्णाङ्क wd;

        प्रतिज्ञा(fd >= 0);
        प्रतिज्ञा(what >= 0);

        /* This is like inotify_add_watch(), except that the file to watch is not referenced by a path, but by an fd */
        wd = inotify_add_watch(fd, FORMAT_PROC_FD_PATH(what), mask);
        यदि (wd < 0) {
                यदि (त्रुटि_सं != ENOENT)
                        प्रत्यागम -त्रुटि_सं;

                प्रत्यागम proc_fd_enoent_errno();
        }

        प्रत्यागम wd;
}

पूर्णाङ्क inotify_add_watch_and_warn(पूर्णाङ्क fd, ध्रुव वर्ण *pathname, uint32_t mask) {
        पूर्णाङ्क wd;

        wd = inotify_add_watch(fd, pathname, mask);
        यदि (wd < 0) {
                यदि (त्रुटि_सं == ENOSPC)
                        प्रत्यागम log_error_errno(त्रुटि_सं, "Failed to add a watch for %s: inotify watch limit reached", pathname);

                प्रत्यागम log_error_errno(त्रुटि_सं, "Failed to add a watch for %s: %m", pathname);
        }

        प्रत्यागम wd;
}
