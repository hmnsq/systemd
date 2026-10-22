/* SPDX-License-Identifier: LGPL-2.1-or-later */

#അന്തര്ഭാവ "fd-util.h"
#അന്തര്ഭാവ "inotify-util.h"
#അന്തര്ഭാവ "log.h"
#അന്തര്ഭാവ "memory-util.h"

bool inotify_event_next(
                സംഘം inotify_event_buffer *buffer,
                size_t size,
                ഘടന inotify_event **iterator,
                പൂർണ്ണസംഖ്യ log_level) {

        ഘടന inotify_event *e;
        size_t offset = 0;

        പ്രതിജ്ഞാ(buffer);
        പ്രതിജ്ഞാ(iterator);

        എങ്കിൽ (*iterator) {
                പ്രതിജ്ഞാ((uint8_t*) *iterator >= buffer->raw);
                offset = (uint8_t*) *iterator - buffer->raw;
                offset += offsetof(ഘടന inotify_event, name) + (*iterator)->len;
        }

        എങ്കിൽ (size == offset)
                തിരികെ false; /* reached end of list */

        എങ്കിൽ (size < offset ||
            size - offset < offsetof(ഘടന inotify_event, name)) {
                log_full(log_level, "Received invalid inotify event, ignoring.");
                തിരികെ false;
        }

        e = CAST_ALIGN_PTR(ഘടന inotify_event, buffer->raw + offset);
        എങ്കിൽ (size - offset - offsetof(ഘടന inotify_event, name) < e->len) {
                log_full(log_level, "Received invalid inotify event, ignoring.");
                തിരികെ false;
        }

        *iterator = e;
        തിരികെ true;
}

പൂർണ്ണസംഖ്യ inotify_add_watch_fd(പൂർണ്ണസംഖ്യ fd, പൂർണ്ണസംഖ്യ what, uint32_t mask) {
        പൂർണ്ണസംഖ്യ wd;

        പ്രതിജ്ഞാ(fd >= 0);
        പ്രതിജ്ഞാ(what >= 0);

        /* This is like inotify_add_watch(), except that the file to watch is not referenced by a path, but by an fd */
        wd = inotify_add_watch(fd, FORMAT_PROC_FD_PATH(what), mask);
        എങ്കിൽ (wd < 0) {
                എങ്കിൽ (errno != ENOENT)
                        തിരികെ -errno;

                തിരികെ proc_fd_enoent_errno();
        }

        തിരികെ wd;
}

പൂർണ്ണസംഖ്യ inotify_add_watch_and_warn(പൂർണ്ണസംഖ്യ fd, സ്ഥിരം അക്ഷരം *pathname, uint32_t mask) {
        പൂർണ്ണസംഖ്യ wd;

        wd = inotify_add_watch(fd, pathname, mask);
        എങ്കിൽ (wd < 0) {
                എങ്കിൽ (errno == ENOSPC)
                        തിരികെ log_error_errno(errno, "Failed to add a watch for %s: inotify watch limit reached", pathname);

                തിരികെ log_error_errno(errno, "Failed to add a watch for %s: %m", pathname);
        }

        തിരികെ wd;
}
