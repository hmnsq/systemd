/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ಸೂಚನೆ once

#ಅನ್ತರ್ಭಾವ "forward.h"

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ sd_future sd_future;

/* Hooks installed on a fiber so that functions in src/basic can transparently defer to the suspending
 * variants in sd-future when invoked from a running fiber. Populated by sd_fiber_new() with pointers to the
 * implementations in fiber-ops.c. */
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ FiberOps {
        ಪೂರ್ಣಾಂಕ (*ppoll)(ರಚನೆ pollfd *fds, size_t n_fds, ಸ್ಥಿರಾಂಕ ರಚನೆ timespec *timeout, ಸ್ಥಿರಾಂಕ sigset_t *sigmask);
        ssize_t (*ಪಠ)(ಪೂರ್ಣಾಂಕ fd, ಶೂನ್ಯ *buf, size_t ಎಣಿಕೆ_2);
        ssize_t (*ಲಿಖ)(ಪೂರ್ಣಾಂಕ fd, ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *buf, size_t ಎಣಿಕೆ_2);
        sd_future* (*timeout)(uint64_t timeout);
        sd_future* (*timeout_unref)(sd_future *timer);
} FiberOps;

bool fiber_ops_is_ಗಣ(ಶೂನ್ಯ);
ಶೂನ್ಯ fiber_ops_ಗಣ(ಸ್ಥಿರಾಂಕ FiberOps *fiber_ops);

ಪೂರ್ಣಾಂಕ fiber_ops_ppoll(ರಚನೆ pollfd *fds, size_t n_fds, ಸ್ಥಿರಾಂಕ ರಚನೆ timespec *timeout, ಸ್ಥಿರಾಂಕ sigset_t *sigmask);
ssize_t fiber_ops_read(ಪೂರ್ಣಾಂಕ fd, ಶೂನ್ಯ *buf, size_t ಎಣಿಕೆ_2);
ssize_t fiber_ops_write(ಪೂರ್ಣಾಂಕ fd, ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *buf, size_t ಎಣಿಕೆ_2);

/* Mirror of SD_FIBER_TIMEOUT() for code under src/basic that doesn't include sd-future.h: dispatches
 * through FiberOps so the actual sd_fiber_timeout() implementation lives in libsystemd. */
sd_future* fiber_ops_timeout(uint64_t timeout);
sd_future* fiber_ops_timeout_unref(sd_future *timer);
DEFINE_TRIVIAL_CLEANUP_FUNC(sd_future*, fiber_ops_timeout_unref);

#ವ್ಯಾಖ್ಯಾನಿಸು FIBER_OPS_TIMEOUT(timeout) _FIBER_OPS_TIMEOUT(UNIQ, (timeout))
#ವ್ಯಾಖ್ಯಾನಿಸು _FIBER_OPS_TIMEOUT(uniq, timeout)                                                                                               \
        ಬಳಸದ _cleanup_(fiber_ops_timeout_unrefp) sd_future *UNIQ_T(_fot_, uniq) = fiber_ops_timeout(timeout)
