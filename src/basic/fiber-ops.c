/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ಅನ್ತರ್ಭಾವ <poll.h>
#ಅನ್ತರ್ಭಾವ <threads.h>
#ಅನ್ತರ್ಭಾವ <unistd.h>

#ಅನ್ತರ್ಭಾವ "errno-util.h"
#ಅನ್ತರ್ಭಾವ "fiber-ops.h"

ಸ್ಥಿರ thread_local ಧ್ರುವ FiberOps *fiber_ops = ರಿಕ್ತ;

bool fiber_ops_is_set(ಶೂನ್ಯ) {
        ಪ್ರತ್ಯಾಗಮ fiber_ops != ರಿಕ್ತ;
}

ಶೂನ್ಯ fiber_ops_set(ಧ್ರುವ FiberOps *ops) {
        fiber_ops = ops;
}

ಪೂರ್ಣಾಙ್ಕ fiber_ops_ppoll(ಸಂರಚನಾ pollfd *fds, size_t n_fds, ಧ್ರುವ ಸಂರಚನಾ timespec *timeout, ಧ್ರುವ sigset_t *sigmask) {
        ಯದಿ (fiber_ops)
                ಪ್ರತ್ಯಾಗಮ fiber_ops->ppoll(fds, n_fds, timeout, sigmask);

        ಪ್ರತ್ಯಾಗಮ RET_NERRNO(ppoll(fds, n_fds, timeout, sigmask));
}

ssize_t fiber_ops_read(ಪೂರ್ಣಾಙ್ಕ fd, ಶೂನ್ಯ *buf, size_t ಎಣಿಕೆ) {
        ಯದಿ (fiber_ops)
                ಪ್ರತ್ಯಾಗಮ fiber_ops->ಪಠ(fd, buf, ಎಣಿಕೆ);

        ssize_t n = ಪಠ(fd, buf, ಎಣಿಕೆ);
        ಪ್ರತ್ಯಾಗಮ n < 0 ? -errno : n;
}

ssize_t fiber_ops_write(ಪೂರ್ಣಾಙ್ಕ fd, ಧ್ರುವ ಶೂನ್ಯ *buf, size_t ಎಣಿಕೆ) {
        ಯದಿ (fiber_ops)
                ಪ್ರತ್ಯಾಗಮ fiber_ops->ಲಿಖ(fd, buf, ಎಣಿಕೆ);

        ಪ್ರತ್ಯಾಗಮ RET_NERRNO(ಲಿಖ(fd, buf, ಎಣಿಕೆ));
}

sd_future* fiber_ops_timeout(uint64_t timeout) {
        ಪ್ರತಿಜ್ಞಾ(fiber_ops);

        ಪ್ರತ್ಯಾಗಮ fiber_ops->timeout(timeout);
}

sd_future* fiber_ops_timeout_unref(sd_future *timer) {
        ಪ್ರತಿಜ್ಞಾ(fiber_ops);

        ಪ್ರತ್ಯಾಗಮ fiber_ops->timeout_unref(timer);
}
