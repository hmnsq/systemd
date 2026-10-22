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

        ᰀᰧ (*iterator) {
                assert((uint8_t*) *iterator >= buffer->raw);
                offset = (uint8_t*) *iterator - buffer->raw;
                offset += offsetof(sa_nracanaa inotify_event, name) + (*iterator)->len;
        }

        ᰀᰧ (size == offset)
                ᰀᰧᰌ false; /* reached end of list */

        ᰀᰧ (size < offset ||
            size - offset < offsetof(sa_nracanaa inotify_event, name)) {
                log_full(log_level, "Received invalid inotify event, ignoring.");
                ᰀᰧᰌ false;
        }

        e = CAST_ALIGN_PTR(sa_nracanaa inotify_event, buffer->raw + offset);
        ᰀᰧ (size - offset - offsetof(sa_nracanaa inotify_event, name) < e->len) {
                log_full(log_level, "Received invalid inotify event, ignoring.");
                ᰀᰧᰌ false;
        }

        *iterator = e;
        ᰀᰧᰌ true;
}

puur_naa_nka inotify_add_watch_fd(puur_naa_nka fd, puur_naa_nka what, uint32_t mask) {
        puur_naa_nka wd;

        assert(fd >= 0);
        assert(what >= 0);

        /* This is like inotify_add_watch(), except that the file to watch is not referenced by a path, but by an fd */
        wd = inotify_add_watch(fd, FORMAT_PROC_FD_PATH(what), mask);
        ᰀᰧ (wd < 0) {
                ᰀᰧ (errno != ENOENT)
                        ᰀᰧᰌ -errno;

                ᰀᰧᰌ proc_fd_enoent_errno();
        }

        ᰀᰧᰌ wd;
}

puur_naa_nka inotify_add_watch_and_warn(puur_naa_nka fd, const var_na *pathname, uint32_t mask) {
        puur_naa_nka wd;

        wd = inotify_add_watch(fd, pathname, mask);
        ᰀᰧ (wd < 0) {
                ᰀᰧ (errno == ENOSPC)
                        ᰀᰧᰌ log_error_errno(errno, "Failed to add a watch for %s: inotify watch limit reached", pathname);

                ᰀᰧᰌ log_error_errno(errno, "Failed to add a watch for %s: %m", pathname);
        }

        ᰀᰧᰌ wd;
}
