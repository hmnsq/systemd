/* SPDX-License-Identifier: LGPL-2.1-or-later */

#अन्तर्भाव "fd-util.h"
#अन्तर्भाव "inotify-util.h"
#अन्तर्भाव "log.h"
#अन्तर्भाव "memory-util.h"

bool inotify_event_next(
                संघ inotify_event_buffer *buffer,
                माप_प्रकार size,
                संरचना inotify_event **iterator,
                पूर्णांक log_level) {

        संरचना inotify_event *e;
        माप_प्रकार offset = 0;

        प्रतिज्ञा(buffer);
        प्रतिज्ञा(iterator);

        जर (*iterator) {
                प्रतिज्ञा((uint8_t*) *iterator >= buffer->raw);
                offset = (uint8_t*) *iterator - buffer->raw;
                offset += दुरत्व(संरचना inotify_event, name) + (*iterator)->len;
        }

        जर (size == offset)
                परत false; /* reached end of list */

        जर (size < offset ||
            size - offset < दुरत्व(संरचना inotify_event, name)) {
                log_full(log_level, "Received invalid inotify event, ignoring.");
                परत false;
        }

        e = CAST_ALIGN_PTR(संरचना inotify_event, buffer->raw + offset);
        जर (size - offset - दुरत्व(संरचना inotify_event, name) < e->len) {
                log_full(log_level, "Received invalid inotify event, ignoring.");
                परत false;
        }

        *iterator = e;
        परत true;
}

पूर्णांक inotify_add_watch_fd(पूर्णांक fd, पूर्णांक what, uint32_t mask) {
        पूर्णांक wd;

        प्रतिज्ञा(fd >= 0);
        प्रतिज्ञा(what >= 0);

        /* This is like inotify_add_watch(), except that the file to watch is not referenced by a path, but by an fd */
        wd = inotify_add_watch(fd, FORMAT_PROC_FD_PATH(what), mask);
        जर (wd < 0) {
                जर (त्रुटि_सं != ENOENT)
                        परत -त्रुटि_सं;

                परत proc_fd_enoent_errno();
        }

        परत wd;
}

पूर्णांक inotify_add_watch_and_warn(पूर्णांक fd, स्थिरांक अक्षर *pathname, uint32_t mask) {
        पूर्णांक wd;

        wd = inotify_add_watch(fd, pathname, mask);
        जर (wd < 0) {
                जर (त्रुटि_सं == ENOSPC)
                        परत log_error_errno(त्रुटि_सं, "Failed to add a watch for %s: inotify watch limit reached", pathname);

                परत log_error_errno(त्रुटि_सं, "Failed to add a watch for %s: %m", pathname);
        }

        परत wd;
}
