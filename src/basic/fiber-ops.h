/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#अन्तर्भाव "forward.h"

प्रकारनिर्देश संरचना sd_future sd_future;

/* Hooks installed on a fiber so that functions in src/basic can transparently defer to the suspending
 * variants in sd-future when invoked from a running fiber. Populated by sd_fiber_new() with pointers to the
 * implementations in fiber-ops.c. */
प्रकारनिर्देश संरचना FiberOps {
        पूर्णाङ्क (*ppoll)(संरचना pollfd *fds, size_t n_fds, ध्रुव संरचना timespec *timeout, ध्रुव sigset_t *sigmask);
        ssize_t (*पठ)(पूर्णाङ्क fd, शून्य *buf, size_t मोजणी);
        ssize_t (*लिख)(पूर्णाङ्क fd, ध्रुव शून्य *buf, size_t मोजणी);
        sd_future* (*timeout)(uint64_t timeout);
        sd_future* (*timeout_unref)(sd_future *timer);
} FiberOps;

bool fiber_ops_is_set(शून्य);
शून्य fiber_ops_set(ध्रुव FiberOps *fiber_ops);

पूर्णाङ्क fiber_ops_ppoll(संरचना pollfd *fds, size_t n_fds, ध्रुव संरचना timespec *timeout, ध्रुव sigset_t *sigmask);
ssize_t fiber_ops_read(पूर्णाङ्क fd, शून्य *buf, size_t मोजणी);
ssize_t fiber_ops_write(पूर्णाङ्क fd, ध्रुव शून्य *buf, size_t मोजणी);

/* Mirror of SD_FIBER_TIMEOUT() for code under src/basic that doesn't include sd-future.h: dispatches
 * through FiberOps so the actual sd_fiber_timeout() implementation lives in libsystemd. */
sd_future* fiber_ops_timeout(uint64_t timeout);
sd_future* fiber_ops_timeout_unref(sd_future *timer);
DEFINE_TRIVIAL_CLEANUP_FUNC(sd_future*, fiber_ops_timeout_unref);

#define FIBER_OPS_TIMEOUT(timeout) _FIBER_OPS_TIMEOUT(UNIQ, (timeout))
#define _FIBER_OPS_TIMEOUT(uniq, timeout)                                                                                               \
        _unused_ _cleanup_(fiber_ops_timeout_unrefp) sd_future *UNIQ_T(_fot_, uniq) = fiber_ops_timeout(timeout)
