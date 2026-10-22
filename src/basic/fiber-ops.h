/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ਹਦਾਇਤ once

#ਅਨ੍ਤਰ੍ਭਾਵ "forward.h"

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ sd_future sd_future;

/* Hooks installed on a fiber so that functions in src/basic can transparently defer to the suspending
 * variants in sd-future when invoked from a running fiber. Populated by sd_fiber_new() with pointers to the
 * implementations in fiber-ops.c. */
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ FiberOps {
        ਪੂਰਨ (*ppoll)(ਬਣਤਰ pollfd *fds, size_t n_fds, ਅਚਲ ਬਣਤਰ timespec *timeout, ਅਚਲ sigset_t *sigmask);
        ssize_t (*ਪਠ)(ਪੂਰਨ fd, ਖਾਲੀ *buf, size_t ਗਿਣਤੀ_2);
        ssize_t (*ਲਿਖ)(ਪੂਰਨ fd, ਅਚਲ ਖਾਲੀ *buf, size_t ਗਿਣਤੀ_2);
        sd_future* (*timeout)(uint64_t timeout);
        sd_future* (*timeout_unref)(sd_future *timer);
} FiberOps;

bool fiber_ops_is_ਸੈੱਟ(ਖਾਲੀ);
ਖਾਲੀ fiber_ops_ਸੈੱਟ(ਅਚਲ FiberOps *fiber_ops);

ਪੂਰਨ fiber_ops_ppoll(ਬਣਤਰ pollfd *fds, size_t n_fds, ਅਚਲ ਬਣਤਰ timespec *timeout, ਅਚਲ sigset_t *sigmask);
ssize_t fiber_ops_read(ਪੂਰਨ fd, ਖਾਲੀ *buf, size_t ਗਿਣਤੀ_2);
ssize_t fiber_ops_write(ਪੂਰਨ fd, ਅਚਲ ਖਾਲੀ *buf, size_t ਗਿਣਤੀ_2);

/* Mirror of SD_FIBER_TIMEOUT() for code under src/basic that doesn't include sd-future.h: dispatches
 * through FiberOps so the actual sd_fiber_timeout() implementation lives in libsystemd. */
sd_future* fiber_ops_timeout(uint64_t timeout);
sd_future* fiber_ops_timeout_unref(sd_future *timer);
DEFINE_TRIVIAL_CLEANUP_FUNC(sd_future*, fiber_ops_timeout_unref);

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ FIBER_OPS_TIMEOUT(timeout) _FIBER_OPS_TIMEOUT(UNIQ, (timeout))
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ _FIBER_OPS_TIMEOUT(uniq, timeout)                                                                                               \
        ਅਣਵਰਤਿਆ _cleanup_(fiber_ops_timeout_unrefp) sd_future *UNIQ_T(_fot_, uniq) = fiber_ops_timeout(timeout)
