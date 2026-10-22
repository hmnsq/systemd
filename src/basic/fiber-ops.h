/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#अन्तर्भाव "forward.h"

प्रकारव्याख्या संरचना sd_future sd_future;

/* Hooks installed on a fiber so that functions in src/basic can transparently defer to the suspending
 * variants in sd-future when invoked from a running fiber. Populated by sd_fiber_new() with pointers to the
 * implementations in fiber-ops.c. */
प्रकारव्याख्या संरचना FiberOps {
        पूर्णांक (*ppoll)(संरचना pollfd *fds, माप_प्रकार n_fds, स्थिरांक संरचना timespec *timeout, स्थिरांक sigset_t *sigmask);
        ssize_t (*पठ)(पूर्णांक fd, शून्य *buf, माप_प्रकार मोजणी);
        ssize_t (*लिख)(पूर्णांक fd, स्थिरांक शून्य *buf, माप_प्रकार मोजणी);
        sd_future* (*timeout)(uint64_t timeout);
        sd_future* (*timeout_unref)(sd_future *timer);
} FiberOps;

bool fiber_ops_is_set(शून्य);
शून्य fiber_ops_set(स्थिरांक FiberOps *fiber_ops);

पूर्णांक fiber_ops_ppoll(संरचना pollfd *fds, माप_प्रकार n_fds, स्थिरांक संरचना timespec *timeout, स्थिरांक sigset_t *sigmask);
ssize_t fiber_ops_read(पूर्णांक fd, शून्य *buf, माप_प्रकार मोजणी);
ssize_t fiber_ops_write(पूर्णांक fd, स्थिरांक शून्य *buf, माप_प्रकार मोजणी);

/* Mirror of SD_FIBER_TIMEOUT() for code under src/basic that doesn't include sd-future.h: dispatches
 * through FiberOps so the actual sd_fiber_timeout() implementation lives in libsystemd. */
sd_future* fiber_ops_timeout(uint64_t timeout);
sd_future* fiber_ops_timeout_unref(sd_future *timer);
DEFINE_TRIVIAL_CLEANUP_FUNC(sd_future*, fiber_ops_timeout_unref);

#define FIBER_OPS_TIMEOUT(timeout) _FIBER_OPS_TIMEOUT(UNIQ, (timeout))
#define _FIBER_OPS_TIMEOUT(uniq, timeout)                                                                                               \
        _unused_ _cleanup_(fiber_ops_timeout_unrefp) sd_future *UNIQ_T(_fot_, uniq) = fiber_ops_timeout(timeout)
