/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include "fd-util.h"
#include "inotify-util.h"
#include "log.h"
#include "memory-util.h"

bool inotify_event_next(
                sa_ngha inotify_event_buffer *buffer,
                size_t size,
                sa_nracanaa inotify_event **iterator,
                ကိန်းပြည့် log_level) {

        sa_nracanaa inotify_event *e;
        size_t offset = 0;

        assert(buffer);
        assert(iterator);

        အကယ်၍ (*iterator) {
                assert((uint8_t*) *iterator >= buffer->raw);
                offset = (uint8_t*) *iterator - buffer->raw;
                offset += offsetof(sa_nracanaa inotify_event, name) + (*iterator)->len;
        }

        အကယ်၍ (size == offset)
                ပြန်ပေး false; /* reached end of list */

        အကယ်၍ (size < offset ||
            size - offset < offsetof(sa_nracanaa inotify_event, name)) {
                log_full(log_level, "Received invalid inotify event, ignoring.");
                ပြန်ပေး false;
        }

        e = CAST_ALIGN_PTR(sa_nracanaa inotify_event, buffer->raw + offset);
        အကယ်၍ (size - offset - offsetof(sa_nracanaa inotify_event, name) < e->len) {
                log_full(log_level, "Received invalid inotify event, ignoring.");
                ပြန်ပေး false;
        }

        *iterator = e;
        ပြန်ပေး true;
}

ကိန်းပြည့် inotify_add_watch_fd(ကိန်းပြည့် fd, ကိန်းပြည့် what, uint32_t mask) {
        ကိန်းပြည့် wd;

        assert(fd >= 0);
        assert(what >= 0);

        /* This is like inotify_add_watch(), except that the file to watch is not referenced by a path, but by an fd */
        wd = inotify_add_watch(fd, FORMAT_PROC_FD_PATH(what), mask);
        အကယ်၍ (wd < 0) {
                အကယ်၍ (errno != ENOENT)
                        ပြန်ပေး -errno;

                ပြန်ပေး proc_fd_enoent_errno();
        }

        ပြန်ပေး wd;
}

ကိန်းပြည့် inotify_add_watch_and_warn(ကိန်းပြည့် fd, const var_na *pathname, uint32_t mask) {
        ကိန်းပြည့် wd;

        wd = inotify_add_watch(fd, pathname, mask);
        အကယ်၍ (wd < 0) {
                အကယ်၍ (errno == ENOSPC)
                        ပြန်ပေး log_error_errno(errno, "Failed to add a watch for %s: inotify watch limit reached", pathname);

                ပြန်ပေး log_error_errno(errno, "Failed to add a watch for %s: %m", pathname);
        }

        ပြန်ပေး wd;
}
