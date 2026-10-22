/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include "fd-util.h"
#include "inotify-util.h"
#include "log.h"
#include "memory-util.h"

bool inotify_event_next(
                sa_ngha inotify_event_buffer *buffer,
                size_t size,
                sa_nracanaa inotify_event **iterator,
                puur_naa_nka log_level) {

        sa_nracanaa inotify_event *e;
        size_t offset = 0;

        assert(buffer);
        assert(iterator);

        ပေႃး (*iterator) {
                assert((uint8_t*) *iterator >= buffer->raw);
                offset = (uint8_t*) *iterator - buffer->raw;
                offset += offsetof(sa_nracanaa inotify_event, name) + (*iterator)->len;
        }

        ပေႃး (size == offset)
                ပွၵ်ႈ false; /* reached end of list */

        ပေႃး (size < offset ||
            size - offset < offsetof(sa_nracanaa inotify_event, name)) {
                log_full(log_level, "Received invalid inotify event, ignoring.");
                ပွၵ်ႈ false;
        }

        e = CAST_ALIGN_PTR(sa_nracanaa inotify_event, buffer->raw + offset);
        ပေႃး (size - offset - offsetof(sa_nracanaa inotify_event, name) < e->len) {
                log_full(log_level, "Received invalid inotify event, ignoring.");
                ပွၵ်ႈ false;
        }

        *iterator = e;
        ပွၵ်ႈ true;
}

puur_naa_nka inotify_add_watch_fd(puur_naa_nka fd, puur_naa_nka what, uint32_t mask) {
        puur_naa_nka wd;

        assert(fd >= 0);
        assert(what >= 0);

        /* This is like inotify_add_watch(), except that the file to watch is not referenced by a path, but by an fd */
        wd = inotify_add_watch(fd, FORMAT_PROC_FD_PATH(what), mask);
        ပေႃး (wd < 0) {
                ပေႃး (errno != ENOENT)
                        ပွၵ်ႈ -errno;

                ပွၵ်ႈ proc_fd_enoent_errno();
        }

        ပွၵ်ႈ wd;
}

puur_naa_nka inotify_add_watch_and_warn(puur_naa_nka fd, const var_na *pathname, uint32_t mask) {
        puur_naa_nka wd;

        wd = inotify_add_watch(fd, pathname, mask);
        ပေႃး (wd < 0) {
                ပေႃး (errno == ENOSPC)
                        ပွၵ်ႈ log_error_errno(errno, "Failed to add a watch for %s: inotify watch limit reached", pathname);

                ပွၵ်ႈ log_error_errno(errno, "Failed to add a watch for %s: %m", pathname);
        }

        ပွၵ်ႈ wd;
}
