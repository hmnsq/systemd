/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ಅನ್ತರ್ಭಾವ "fd-util.h"
#ಅನ್ತರ್ಭಾವ "inotify-util.h"
#ಅನ್ತರ್ಭಾವ "log.h"
#ಅನ್ತರ್ಭಾವ "memory-util.h"

bool inotify_event_ಮುಂದಿನ(
                ಒಕ್ಕೂಟ inotify_event_ಬಫರ್ *ಬಫರ್,
                size_t ಗಾತ್ರ_2,
                ರಚನೆ inotify_event **iterator,
                ಪೂರ್ಣಾಂಕ log_level) {

        ರಚನೆ inotify_event *e;
        size_t ಸ್ಥಾನಾಂತರ = 0;

        ಪ್ರತಿಜ್ಞಾ(ಬಫರ್);
        ಪ್ರತಿಜ್ಞಾ(iterator);

        ಆದರೆ (*iterator) {
                ಪ್ರತಿಜ್ಞಾ((uint8_t*) *iterator >= ಬಫರ್->raw);
                ಸ್ಥಾನಾಂತರ = (uint8_t*) *iterator - ಬಫರ್->raw;
                ಸ್ಥಾನಾಂತರ += offsetof(ರಚನೆ inotify_event, name) + (*iterator)->len;
        }

        ಆದರೆ (ಗಾತ್ರ_2 == ಸ್ಥಾನಾಂತರ)
                ಹಿಂತಿರುಗಿ false; /* reached end of list */

        ಆದರೆ (ಗಾತ್ರ_2 < ಸ್ಥಾನಾಂತರ ||
            ಗಾತ್ರ_2 - ಸ್ಥಾನಾಂತರ < offsetof(ರಚನೆ inotify_event, name)) {
                log_ತುಂಬಿದ(log_level, "Received invalid inotify event, ignoring.");
                ಹಿಂತಿರುಗಿ false;
        }

        e = CAST_ALIGN_PTR(ರಚನೆ inotify_event, ಬಫರ್->raw + ಸ್ಥಾನಾಂತರ);
        ಆದರೆ (ಗಾತ್ರ_2 - ಸ್ಥಾನಾಂತರ - offsetof(ರಚನೆ inotify_event, name) < e->len) {
                log_ತುಂಬಿದ(log_level, "Received invalid inotify event, ignoring.");
                ಹಿಂತಿರುಗಿ false;
        }

        *iterator = e;
        ಹಿಂತಿರುಗಿ true;
}

ಪೂರ್ಣಾಂಕ inotify_ಕೂಡಿಸು_watch_fd(ಪೂರ್ಣಾಂಕ fd, ಪೂರ್ಣಾಂಕ what, uint32_t ಮುಖವಾಡ_3) {
        ಪೂರ್ಣಾಂಕ wd;

        ಪ್ರತಿಜ್ಞಾ(fd >= 0);
        ಪ್ರತಿಜ್ಞಾ(what >= 0);

        /* This is like inotify_add_watch(), except that the file to watch is not referenced by a path, but by an fd */
        wd = inotify_add_watch(fd, FORMAT_PROC_FD_ಮಾರ್ಗ(what), ಮುಖವಾಡ_3);
        ಆದರೆ (wd < 0) {
                ಆದರೆ (errno != ENOENT)
                        ಹಿಂತಿರುಗಿ -errno;

                ಹಿಂತಿರುಗಿ proc_fd_enoent_errno();
        }

        ಹಿಂತಿರುಗಿ wd;
}

ಪೂರ್ಣಾಂಕ inotify_ಕೂಡಿಸು_watch_and_warn(ಪೂರ್ಣಾಂಕ fd, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *pathname, uint32_t ಮುಖವಾಡ_3) {
        ಪೂರ್ಣಾಂಕ wd;

        wd = inotify_add_watch(fd, pathname, ಮುಖವಾಡ_3);
        ಆದರೆ (wd < 0) {
                ಆದರೆ (errno == ENOSPC)
                        ಹಿಂತಿರುಗಿ log_error_errno(errno, "Failed to add a watch for %s: inotify watch limit reached", pathname);

                ಹಿಂತಿರುಗಿ log_error_errno(errno, "Failed to add a watch for %s: %m", pathname);
        }

        ಹಿಂತಿರುಗಿ wd;
}
