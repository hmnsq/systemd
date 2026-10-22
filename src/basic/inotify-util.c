/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ਅਨ੍ਤਰ੍ਭਾਵ "fd-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "inotify-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "log.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "memory-util.h"

bool inotify_event_ਅਗਲਾ(
                ਸੰਘ inotify_event_ਬਫਰ *ਬਫਰ,
                size_t ਮਾਪ,
                ਬਣਤਰ inotify_event **iterator,
                ਪੂਰਨ log_level) {

        ਬਣਤਰ inotify_event *e;
        size_t ਵਿੱਥ = 0;

        ਪ੍ਰਤਿਜ੍ਞਾ(ਬਫਰ);
        ਪ੍ਰਤਿਜ੍ਞਾ(iterator);

        ਜੇ (*iterator) {
                ਪ੍ਰਤਿਜ੍ਞਾ((uint8_t*) *iterator >= ਬਫਰ->raw);
                ਵਿੱਥ = (uint8_t*) *iterator - ਬਫਰ->raw;
                ਵਿੱਥ += offsetof(ਬਣਤਰ inotify_event, name) + (*iterator)->len;
        }

        ਜੇ (ਮਾਪ == ਵਿੱਥ)
                ਵਾਪਸ false; /* reached end of list */

        ਜੇ (ਮਾਪ < ਵਿੱਥ ||
            ਮਾਪ - ਵਿੱਥ < offsetof(ਬਣਤਰ inotify_event, name)) {
                log_ਭਰਿਆ(log_level, "Received invalid inotify event, ignoring.");
                ਵਾਪਸ false;
        }

        e = CAST_ALIGN_PTR(ਬਣਤਰ inotify_event, ਬਫਰ->raw + ਵਿੱਥ);
        ਜੇ (ਮਾਪ - ਵਿੱਥ - offsetof(ਬਣਤਰ inotify_event, name) < e->len) {
                log_ਭਰਿਆ(log_level, "Received invalid inotify event, ignoring.");
                ਵਾਪਸ false;
        }

        *iterator = e;
        ਵਾਪਸ true;
}

ਪੂਰਨ inotify_ਜੋੜੋ_watch_fd(ਪੂਰਨ fd, ਪੂਰਨ what, uint32_t ਨਕਾਬ_3) {
        ਪੂਰਨ wd;

        ਪ੍ਰਤਿਜ੍ਞਾ(fd >= 0);
        ਪ੍ਰਤਿਜ੍ਞਾ(what >= 0);

        /* This is like inotify_add_watch(), except that the file to watch is not referenced by a path, but by an fd */
        wd = inotify_add_watch(fd, FORMAT_PROC_FD_ਰਸਤਾ(what), ਨਕਾਬ_3);
        ਜੇ (wd < 0) {
                ਜੇ (errno != ENOENT)
                        ਵਾਪਸ -errno;

                ਵਾਪਸ proc_fd_enoent_errno();
        }

        ਵਾਪਸ wd;
}

ਪੂਰਨ inotify_ਜੋੜੋ_watch_and_warn(ਪੂਰਨ fd, ਅਚਲ ਅੱਖਰ *pathname, uint32_t ਨਕਾਬ_3) {
        ਪੂਰਨ wd;

        wd = inotify_add_watch(fd, pathname, ਨਕਾਬ_3);
        ਜੇ (wd < 0) {
                ਜੇ (errno == ENOSPC)
                        ਵਾਪਸ log_error_errno(errno, "Failed to add a watch for %s: inotify watch limit reached", pathname);

                ਵਾਪਸ log_error_errno(errno, "Failed to add a watch for %s: %m", pathname);
        }

        ਵਾਪਸ wd;
}
