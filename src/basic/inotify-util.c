/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include "fd-util.h"
#include "inotify-util.h"
#include "log.h"
#include "memory-util.h"

bool inotify_event_next(
                sa_ngha inotify_event_buffer *buffer,
                size_t size,
                sa_nracanaa inotify_event **iterator,
                ཧྲིལ log_level) {

        sa_nracanaa inotify_event *e;
        size_t offset = 0;

        assert(buffer);
        assert(iterator);

        གལ་ཏེ (*iterator) {
                assert((uint8_t*) *iterator >= buffer->raw);
                offset = (uint8_t*) *iterator - buffer->raw;
                offset += offsetof(sa_nracanaa inotify_event, name) + (*iterator)->len;
        }

        གལ་ཏེ (size == offset)
                ཕྱིར false; /* reached end of list */

        གལ་ཏེ (size < offset ||
            size - offset < offsetof(sa_nracanaa inotify_event, name)) {
                log_full(log_level, "Received invalid inotify event, ignoring.");
                ཕྱིར false;
        }

        e = CAST_ALIGN_PTR(sa_nracanaa inotify_event, buffer->raw + offset);
        གལ་ཏེ (size - offset - offsetof(sa_nracanaa inotify_event, name) < e->len) {
                log_full(log_level, "Received invalid inotify event, ignoring.");
                ཕྱིར false;
        }

        *iterator = e;
        ཕྱིར true;
}

ཧྲིལ inotify_add_watch_fd(ཧྲིལ fd, ཧྲིལ what, uint32_t mask) {
        ཧྲིལ wd;

        assert(fd >= 0);
        assert(what >= 0);

        /* This is like inotify_add_watch(), except that the file to watch is not referenced by a path, but by an fd */
        wd = inotify_add_watch(fd, FORMAT_PROC_FD_PATH(what), mask);
        གལ་ཏེ (wd < 0) {
                གལ་ཏེ (errno != ENOENT)
                        ཕྱིར -errno;

                ཕྱིར proc_fd_enoent_errno();
        }

        ཕྱིར wd;
}

ཧྲིལ inotify_add_watch_and_warn(ཧྲིལ fd, const var_na *pathname, uint32_t mask) {
        ཧྲིལ wd;

        wd = inotify_add_watch(fd, pathname, mask);
        གལ་ཏེ (wd < 0) {
                གལ་ཏེ (errno == ENOSPC)
                        ཕྱིར log_error_errno(errno, "Failed to add a watch for %s: inotify watch limit reached", pathname);

                ཕྱིར log_error_errno(errno, "Failed to add a watch for %s: %m", pathname);
        }

        ཕྱིར wd;
}
