/* SPDX-License-Identifier: LGPL-2.1-or-later */

#समावेश "fd-util.h"
#समावेश "inotify-util.h"
#समावेश "log.h"
#समावेश "memory-util.h"

bool inotify_event_अग्र(
                जोड़ inotify_event_बफर *बफर,
                माप_प्रकार आकार,
                काष्ठा inotify_event **iterator,
                पूर्णांक log_level) {

        काष्ठा inotify_event *e;
        माप_प्रकार offset = 0;

        निश्चित(बफर);
        निश्चित(iterator);

        अगर (*iterator) {
                निश्चित((uint8_t*) *iterator >= बफर->raw);
                offset = (uint8_t*) *iterator - बफर->raw;
                offset += दुरत्व(काष्ठा inotify_event, name) + (*iterator)->len;
        }

        अगर (आकार == offset)
                वापस false; /* reached end of list */

        अगर (आकार < offset ||
            आकार - offset < दुरत्व(काष्ठा inotify_event, name)) {
                log_full(log_level, "Received invalid inotify event, ignoring.");
                वापस false;
        }

        e = CAST_ALIGN_PTR(काष्ठा inotify_event, बफर->raw + offset);
        अगर (आकार - offset - दुरत्व(काष्ठा inotify_event, name) < e->len) {
                log_full(log_level, "Received invalid inotify event, ignoring.");
                वापस false;
        }

        *iterator = e;
        वापस true;
}

पूर्णांक inotify_add_watch_fd(पूर्णांक fd, पूर्णांक what, uint32_t mask) {
        पूर्णांक wd;

        निश्चित(fd >= 0);
        निश्चित(what >= 0);

        /* This is like inotify_add_watch(), except that the file to watch is not referenced by a path, but by an fd */
        wd = inotify_add_watch(fd, FORMAT_PROC_FD_PATH(what), mask);
        अगर (wd < 0) {
                अगर (त्रुटि_सं != ENOENT)
                        वापस -त्रुटि_सं;

                वापस proc_fd_enoent_errno();
        }

        वापस wd;
}

पूर्णांक inotify_add_watch_and_warn(पूर्णांक fd, स्थिर अक्षर *pathname, uint32_t mask) {
        पूर्णांक wd;

        wd = inotify_add_watch(fd, pathname, mask);
        अगर (wd < 0) {
                अगर (त्रुटि_सं == ENOSPC)
                        वापस log_error_errno(त्रुटि_सं, "Failed to add a watch for %s: inotify watch limit reached", pathname);

                वापस log_error_errno(त्रुटि_सं, "Failed to add a watch for %s: %m", pathname);
        }

        वापस wd;
}
