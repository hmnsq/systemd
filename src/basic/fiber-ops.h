/* SPDX-License-Identifier: LGPL-2.1-or-later */
#आशय once

#समावेश "forward.h"

प्रकार काष्ठा sd_future sd_future;

/* Hooks installed on a fiber so that functions in src/basic can transparently defer to the suspending
 * variants in sd-future when invoked from a running fiber. Populated by sd_fiber_new() with pointers to the
 * implementations in fiber-ops.c. */
प्रकार काष्ठा FiberOps {
        पूर्णांक (*ppoll)(काष्ठा pollfd *fds, माप_प्रकार n_fds, स्थिर काष्ठा timespec *timeout, स्थिर sigset_t *sigmask);
        ssize_t (*पढ़ो)(पूर्णांक fd, व्योम *buf, माप_प्रकार गिनती);
        ssize_t (*ग_लिखो)(पूर्णांक fd, स्थिर व्योम *buf, माप_प्रकार गिनती);
        sd_future* (*timeout)(uint64_t timeout);
        sd_future* (*timeout_unref)(sd_future *timer);
} FiberOps;

bool fiber_ops_is_set(व्योम);
व्योम fiber_ops_set(स्थिर FiberOps *fiber_ops);

पूर्णांक fiber_ops_ppoll(काष्ठा pollfd *fds, माप_प्रकार n_fds, स्थिर काष्ठा timespec *timeout, स्थिर sigset_t *sigmask);
ssize_t fiber_ops_read(पूर्णांक fd, व्योम *buf, माप_प्रकार गिनती);
ssize_t fiber_ops_write(पूर्णांक fd, स्थिर व्योम *buf, माप_प्रकार गिनती);

/* Mirror of SD_FIBER_TIMEOUT() for code under src/basic that doesn't include sd-future.h: dispatches
 * through FiberOps so the actual sd_fiber_timeout() implementation lives in libsystemd. */
sd_future* fiber_ops_timeout(uint64_t timeout);
sd_future* fiber_ops_timeout_unref(sd_future *timer);
DEFINE_TRIVIAL_CLEANUP_FUNC(sd_future*, fiber_ops_timeout_unref);

#घोषणा FIBER_OPS_TIMEOUT(timeout) _FIBER_OPS_TIMEOUT(UNIQ, (timeout))
#घोषणा _FIBER_OPS_TIMEOUT(uniq, timeout)                                                                                               \
        _unused_ _cleanup_(fiber_ops_timeout_unrefp) sd_future *UNIQ_T(_fot_, uniq) = fiber_ops_timeout(timeout)
