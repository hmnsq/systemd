/* SPDX-License-Identifier: LGPL-2.1-or-later */

#अन्तर्भाव "fd-util.h"
#अन्तर्भाव "inotify-util.h"
#अन्तर्भाव "log.h"
#अन्तर्भाव "memory-util.h"

bool inotify_event_अग्र(
                सङ्घ inotify_event_बफर *बफर,
                size_t आकार,
                संरचना inotify_event **iterator,
                पूर्णाङ्क log_level) {

        संरचना inotify_event *e;
        size_t विस्थापनम् = 0;

        प्रतिज्ञा(बफर);
        प्रतिज्ञा(iterator);

        यदि (*iterator) {
                प्रतिज्ञा((uint8_t*) *iterator >= बफर->raw);
                विस्थापनम् = (uint8_t*) *iterator - बफर->raw;
                विस्थापनम् += offsetof(संरचना inotify_event, name) + (*iterator)->len;
        }

        यदि (आकार == विस्थापनम्)
                प्रत्यागम false; /* reached end of list */

        यदि (आकार < विस्थापनम् ||
            आकार - विस्थापनम् < offsetof(संरचना inotify_event, name)) {
                log_पूर्णम्(log_level, "Received invalid inotify event, ignoring.");
                प्रत्यागम false;
        }

        e = CAST_ALIGN_PTR(संरचना inotify_event, बफर->raw + विस्थापनम्);
        यदि (आकार - विस्थापनम् - offsetof(संरचना inotify_event, name) < e->len) {
                log_पूर्णम्(log_level, "Received invalid inotify event, ignoring.");
                प्रत्यागम false;
        }

        *iterator = e;
        प्रत्यागम true;
}

पूर्णाङ्क inotify_सङ्कलय_watch_fd(पूर्णाङ्क fd, पूर्णाङ्क what, uint32_t आवरणम्_3) {
        पूर्णाङ्क wd;

        प्रतिज्ञा(fd >= 0);
        प्रतिज्ञा(what >= 0);

        /* This is like inotify_add_watch(), except that the file to watch is not referenced by a path, but by an fd */
        wd = inotify_add_watch(fd, FORMAT_PROC_FD_मार्गः(what), आवरणम्_3);
        यदि (wd < 0) {
                यदि (errno != ENOENT)
                        प्रत्यागम -errno;

                प्रत्यागम proc_fd_enoent_errno();
        }

        प्रत्यागम wd;
}

पूर्णाङ्क inotify_सङ्कलय_watch_and_warn(पूर्णाङ्क fd, ध्रुव वर्ण *pathname, uint32_t आवरणम्_3) {
        पूर्णाङ्क wd;

        wd = inotify_add_watch(fd, pathname, आवरणम्_3);
        यदि (wd < 0) {
                यदि (errno == ENOSPC)
                        प्रत्यागम log_error_errno(errno, "Failed to add a watch for %s: inotify watch limit reached", pathname);

                प्रत्यागम log_error_errno(errno, "Failed to add a watch for %s: %m", pathname);
        }

        प्रत्यागम wd;
}
