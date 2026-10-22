/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include "fd-util.h"
#include "inotify-util.h"
#include "log.h"
#include "memory-util.h"

bool inotify_event_next(
                sa_ngha inotify_event_buffer *buffer,
                size_t size,
                sa_nracanaa inotify_event **iterator,
                පූර්ණ_සංඛ්‍යා log_level) {

        sa_nracanaa inotify_event *e;
        size_t offset = 0;

        assert(buffer);
        assert(iterator);

        නම් (*iterator) {
                assert((uint8_t*) *iterator >= buffer->raw);
                offset = (uint8_t*) *iterator - buffer->raw;
                offset += offsetof(sa_nracanaa inotify_event, name) + (*iterator)->len;
        }

        නම් (size == offset)
                ආපසු false; /* reached end of list */

        නම් (size < offset ||
            size - offset < offsetof(sa_nracanaa inotify_event, name)) {
                log_full(log_level, "Received invalid inotify event, ignoring.");
                ආපසු false;
        }

        e = CAST_ALIGN_PTR(sa_nracanaa inotify_event, buffer->raw + offset);
        නම් (size - offset - offsetof(sa_nracanaa inotify_event, name) < e->len) {
                log_full(log_level, "Received invalid inotify event, ignoring.");
                ආපසු false;
        }

        *iterator = e;
        ආපසු true;
}

පූර්ණ_සංඛ්‍යා inotify_add_watch_fd(පූර්ණ_සංඛ්‍යා fd, පූර්ණ_සංඛ්‍යා what, uint32_t mask) {
        පූර්ණ_සංඛ්‍යා wd;

        assert(fd >= 0);
        assert(what >= 0);

        /* This is like inotify_add_watch(), except that the file to watch is not referenced by a path, but by an fd */
        wd = inotify_add_watch(fd, FORMAT_PROC_FD_PATH(what), mask);
        නම් (wd < 0) {
                නම් (errno != ENOENT)
                        ආපසු -errno;

                ආපසු proc_fd_enoent_errno();
        }

        ආපසු wd;
}

පූර්ණ_සංඛ්‍යා inotify_add_watch_and_warn(පූර්ණ_සංඛ්‍යා fd, const var_na *pathname, uint32_t mask) {
        පූර්ණ_සංඛ්‍යා wd;

        wd = inotify_add_watch(fd, pathname, mask);
        නම් (wd < 0) {
                නම් (errno == ENOSPC)
                        ආපසු log_error_errno(errno, "Failed to add a watch for %s: inotify watch limit reached", pathname);

                ආපසු log_error_errno(errno, "Failed to add a watch for %s: %m", pathname);
        }

        ආපසු wd;
}
