/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ಅನ್ತರ್ಭಾವ <poll.h>
#ಅನ್ತರ್ಭಾವ <threads.h>
#ಅನ್ತರ್ಭಾವ <unistd.h>

#ಅನ್ತರ್ಭಾವ "errno-util.h"
#ಅನ್ತರ್ಭಾವ "fiber-ops.h"

ಸ್ಥಿರ thread_local ಸ್ಥಿರಾಂಕ FiberOps *fiber_ops = ಖಾಲಿ;

bool fiber_ops_is_ಗಣ(ಶೂನ್ಯ) {
        ಹಿಂತಿರುಗಿ fiber_ops != ಖಾಲಿ;
}

ಶೂನ್ಯ fiber_ops_ಗಣ(ಸ್ಥಿರಾಂಕ FiberOps *ops) {
        fiber_ops = ops;
}

ಪೂರ್ಣಾಂಕ fiber_ops_ppoll(ರಚನೆ pollfd *fds, size_t n_fds, ಸ್ಥಿರಾಂಕ ರಚನೆ timespec *timeout, ಸ್ಥಿರಾಂಕ sigset_t *sigmask) {
        ಆದರೆ (fiber_ops)
                ಹಿಂತಿರುಗಿ fiber_ops->ppoll(fds, n_fds, timeout, sigmask);

        ಹಿಂತಿರುಗಿ RET_NERRNO(ppoll(fds, n_fds, timeout, sigmask));
}

ssize_t fiber_ops_read(ಪೂರ್ಣಾಂಕ fd, ಶೂನ್ಯ *buf, size_t ಎಣಿಕೆ_2) {
        ಆದರೆ (fiber_ops)
                ಹಿಂತಿರುಗಿ fiber_ops->ಪಠ(fd, buf, ಎಣಿಕೆ_2);

        ssize_t n = ಪಠ(fd, buf, ಎಣಿಕೆ_2);
        ಹಿಂತಿರುಗಿ n < 0 ? -errno : n;
}

ssize_t fiber_ops_write(ಪೂರ್ಣಾಂಕ fd, ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *buf, size_t ಎಣಿಕೆ_2) {
        ಆದರೆ (fiber_ops)
                ಹಿಂತಿರುಗಿ fiber_ops->ಲಿಖ(fd, buf, ಎಣಿಕೆ_2);

        ಹಿಂತಿರುಗಿ RET_NERRNO(ಲಿಖ(fd, buf, ಎಣಿಕೆ_2));
}

sd_future* fiber_ops_timeout(uint64_t timeout) {
        ಪ್ರತಿಜ್ಞಾ(fiber_ops);

        ಹಿಂತಿರುಗಿ fiber_ops->timeout(timeout);
}

sd_future* fiber_ops_timeout_unref(sd_future *timer) {
        ಪ್ರತಿಜ್ಞಾ(fiber_ops);

        ಹಿಂತಿರುಗಿ fiber_ops->timeout_unref(timer);
}
