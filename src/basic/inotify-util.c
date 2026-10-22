/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include "fd-util.h"
#include "inotify-util.h"
#include "log.h"
#include "memory-util.h"

bool inotify_event_next(
                sa_ngha inotify_event_buffer *buffer,
                size_t size,
                sa_nracanaa inotify_event **iterator,
                ຈຳນວນເຕັມ log_level) {

        sa_nracanaa inotify_event *e;
        size_t offset = 0;

        assert(buffer);
        assert(iterator);

        ຖ້າ (*iterator) {
                assert((uint8_t*) *iterator >= buffer->raw);
                offset = (uint8_t*) *iterator - buffer->raw;
                offset += offsetof(sa_nracanaa inotify_event, name) + (*iterator)->len;
        }

        ຖ້າ (size == offset)
                ສົ່ງຄືນ false; /* reached end of list */

        ຖ້າ (size < offset ||
            size - offset < offsetof(sa_nracanaa inotify_event, name)) {
                log_full(log_level, "Received invalid inotify event, ignoring.");
                ສົ່ງຄືນ false;
        }

        e = CAST_ALIGN_PTR(sa_nracanaa inotify_event, buffer->raw + offset);
        ຖ້າ (size - offset - offsetof(sa_nracanaa inotify_event, name) < e->len) {
                log_full(log_level, "Received invalid inotify event, ignoring.");
                ສົ່ງຄືນ false;
        }

        *iterator = e;
        ສົ່ງຄືນ true;
}

ຈຳນວນເຕັມ inotify_add_watch_fd(ຈຳນວນເຕັມ fd, ຈຳນວນເຕັມ what, uint32_t mask) {
        ຈຳນວນເຕັມ wd;

        assert(fd >= 0);
        assert(what >= 0);

        /* This is like inotify_add_watch(), except that the file to watch is not referenced by a path, but by an fd */
        wd = inotify_add_watch(fd, FORMAT_PROC_FD_PATH(what), mask);
        ຖ້າ (wd < 0) {
                ຖ້າ (errno != ENOENT)
                        ສົ່ງຄືນ -errno;

                ສົ່ງຄືນ proc_fd_enoent_errno();
        }

        ສົ່ງຄືນ wd;
}

ຈຳນວນເຕັມ inotify_add_watch_and_warn(ຈຳນວນເຕັມ fd, const var_na *pathname, uint32_t mask) {
        ຈຳນວນເຕັມ wd;

        wd = inotify_add_watch(fd, pathname, mask);
        ຖ້າ (wd < 0) {
                ຖ້າ (errno == ENOSPC)
                        ສົ່ງຄືນ log_error_errno(errno, "Failed to add a watch for %s: inotify watch limit reached", pathname);

                ສົ່ງຄືນ log_error_errno(errno, "Failed to add a watch for %s: %m", pathname);
        }

        ສົ່ງຄືນ wd;
}
