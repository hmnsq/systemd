/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#ಅನ್ತರ್ಭಾವ "forward.h"

ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ sd_future sd_future;

/* Hooks installed on a fiber so that functions in src/basic can transparently defer to the suspending
 * variants in sd-future when invoked from a running fiber. Populated by sd_fiber_new() with pointers to the
 * implementations in fiber-ops.c. */
ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ FiberOps {
        ಪೂರ್ಣಾಙ್ಕ (*ppoll)(ಸಂರಚನಾ pollfd *fds, size_t n_fds, ಧ್ರುವ ಸಂರಚನಾ timespec *timeout, ಧ್ರುವ sigset_t *sigmask);
        ssize_t (*ಪಠ)(ಪೂರ್ಣಾಙ್ಕ fd, ಶೂನ್ಯ *buf, size_t ಎಣಿಕೆ);
        ssize_t (*ಲಿಖ)(ಪೂರ್ಣಾಙ್ಕ fd, ಧ್ರುವ ಶೂನ್ಯ *buf, size_t ಎಣಿಕೆ);
        sd_future* (*timeout)(uint64_t timeout);
        sd_future* (*timeout_unref)(sd_future *timer);
} FiberOps;

bool fiber_ops_is_set(ಶೂನ್ಯ);
ಶೂನ್ಯ fiber_ops_set(ಧ್ರುವ FiberOps *fiber_ops);

ಪೂರ್ಣಾಙ್ಕ fiber_ops_ppoll(ಸಂರಚನಾ pollfd *fds, size_t n_fds, ಧ್ರುವ ಸಂರಚನಾ timespec *timeout, ಧ್ರುವ sigset_t *sigmask);
ssize_t fiber_ops_read(ಪೂರ್ಣಾಙ್ಕ fd, ಶೂನ್ಯ *buf, size_t ಎಣಿಕೆ);
ssize_t fiber_ops_write(ಪೂರ್ಣಾಙ್ಕ fd, ಧ್ರುವ ಶೂನ್ಯ *buf, size_t ಎಣಿಕೆ);

/* Mirror of SD_FIBER_TIMEOUT() for code under src/basic that doesn't include sd-future.h: dispatches
 * through FiberOps so the actual sd_fiber_timeout() implementation lives in libsystemd. */
sd_future* fiber_ops_timeout(uint64_t timeout);
sd_future* fiber_ops_timeout_unref(sd_future *timer);
DEFINE_TRIVIAL_CLEANUP_FUNC(sd_future*, fiber_ops_timeout_unref);

#define FIBER_OPS_TIMEOUT(timeout) _FIBER_OPS_TIMEOUT(UNIQ, (timeout))
#define _FIBER_OPS_TIMEOUT(uniq, timeout)                                                                                               \
        _unused_ _cleanup_(fiber_ops_timeout_unrefp) sd_future *UNIQ_T(_fot_, uniq) = fiber_ops_timeout(timeout)
