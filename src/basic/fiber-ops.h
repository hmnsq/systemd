/* SPDX-License-Identifier: LGPL-2.1-or-later */
#సూచన once

#అన్తర్భావ "forward.h"

రకనిర్వచనం నిర్మాణం sd_future sd_future;

/* Hooks installed on a fiber so that functions in src/basic can transparently defer to the suspending
 * variants in sd-future when invoked from a running fiber. Populated by sd_fiber_new() with pointers to the
 * implementations in fiber-ops.c. */
రకనిర్వచనం నిర్మాణం FiberOps {
        పూర్ణాంకం (*ppoll)(నిర్మాణం pollfd *fds, size_t n_fds, స్థిరరాశి నిర్మాణం timespec *timeout, స్థిరరాశి sigset_t *sigmask);
        ssize_t (*పఠ)(పూర్ణాంకం fd, శూన్య *buf, size_t లెక్క);
        ssize_t (*లిఖ)(పూర్ణాంకం fd, స్థిరరాశి శూన్య *buf, size_t లెక్క);
        sd_future* (*timeout)(uint64_t timeout);
        sd_future* (*timeout_unref)(sd_future *timer);
} FiberOps;

bool fiber_ops_is_సమితి(శూన్య);
శూన్య fiber_ops_సమితి(స్థిరరాశి FiberOps *fiber_ops);

పూర్ణాంకం fiber_ops_ppoll(నిర్మాణం pollfd *fds, size_t n_fds, స్థిరరాశి నిర్మాణం timespec *timeout, స్థిరరాశి sigset_t *sigmask);
ssize_t fiber_ops_read(పూర్ణాంకం fd, శూన్య *buf, size_t లెక్క);
ssize_t fiber_ops_write(పూర్ణాంకం fd, స్థిరరాశి శూన్య *buf, size_t లెక్క);

/* Mirror of SD_FIBER_TIMEOUT() for code under src/basic that doesn't include sd-future.h: dispatches
 * through FiberOps so the actual sd_fiber_timeout() implementation lives in libsystemd. */
sd_future* fiber_ops_timeout(uint64_t timeout);
sd_future* fiber_ops_timeout_unref(sd_future *timer);
DEFINE_TRIVIAL_CLEANUP_FUNC(sd_future*, fiber_ops_timeout_unref);

#నిర్వచించు FIBER_OPS_TIMEOUT(timeout) _FIBER_OPS_TIMEOUT(UNIQ, (timeout))
#నిర్వచించు _FIBER_OPS_TIMEOUT(uniq, timeout)                                                                                               \
        వాడని _cleanup_(fiber_ops_timeout_unrefp) sd_future *UNIQ_T(_fot_, uniq) = fiber_ops_timeout(timeout)
