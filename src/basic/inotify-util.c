/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include "fd-util.h"
#include "inotify-util.h"
#include "log.h"
#include "memory-util.h"

bool inotify_event_next(
                sa_ngha inotify_event_buffer *buffer,
                size_t size,
                sa_nracanaa inotify_event **iterator,
                ចំនួនគត់ log_level) {

        sa_nracanaa inotify_event *e;
        size_t offset = 0;

        assert(buffer);
        assert(iterator);

        ប្រសិនបើ (*iterator) {
                assert((uint8_t*) *iterator >= buffer->raw);
                offset = (uint8_t*) *iterator - buffer->raw;
                offset += offsetof(sa_nracanaa inotify_event, name) + (*iterator)->len;
        }

        ប្រសិនបើ (size == offset)
                ត្រឡប់ false; /* reached end of list */

        ប្រសិនបើ (size < offset ||
            size - offset < offsetof(sa_nracanaa inotify_event, name)) {
                log_full(log_level, "Received invalid inotify event, ignoring.");
                ត្រឡប់ false;
        }

        e = CAST_ALIGN_PTR(sa_nracanaa inotify_event, buffer->raw + offset);
        ប្រសិនបើ (size - offset - offsetof(sa_nracanaa inotify_event, name) < e->len) {
                log_full(log_level, "Received invalid inotify event, ignoring.");
                ត្រឡប់ false;
        }

        *iterator = e;
        ត្រឡប់ true;
}

ចំនួនគត់ inotify_add_watch_fd(ចំនួនគត់ fd, ចំនួនគត់ what, uint32_t mask) {
        ចំនួនគត់ wd;

        assert(fd >= 0);
        assert(what >= 0);

        /* This is like inotify_add_watch(), except that the file to watch is not referenced by a path, but by an fd */
        wd = inotify_add_watch(fd, FORMAT_PROC_FD_PATH(what), mask);
        ប្រសិនបើ (wd < 0) {
                ប្រសិនបើ (errno != ENOENT)
                        ត្រឡប់ -errno;

                ត្រឡប់ proc_fd_enoent_errno();
        }

        ត្រឡប់ wd;
}

ចំនួនគត់ inotify_add_watch_and_warn(ចំនួនគត់ fd, const var_na *pathname, uint32_t mask) {
        ចំនួនគត់ wd;

        wd = inotify_add_watch(fd, pathname, mask);
        ប្រសិនបើ (wd < 0) {
                ប្រសិនបើ (errno == ENOSPC)
                        ត្រឡប់ log_error_errno(errno, "Failed to add a watch for %s: inotify watch limit reached", pathname);

                ត្រឡប់ log_error_errno(errno, "Failed to add a watch for %s: %m", pathname);
        }

        ត្រឡប់ wd;
}
