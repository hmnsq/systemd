/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ہدایت once

#ضمن "forward.h"

تعريفنوع بنية sd_future sd_future;

/* Hooks installed on a fiber so that functions in src/basic can transparently defer to the suspending
 * variants in sd-future when invoked from a running fiber. Populated by sd_fiber_new() with pointers to the
 * implementations in fiber-ops.c. */
تعريفنوع بنية FiberOps {
        صحيح (*ppoll)(بنية pollfd *fds, size_t n_fds, ثابت بنية timespec *timeout, ثابت sigset_t *sigmask);
        ssize_t (*اقرأ)(صحيح fd, فراغ *buf, size_t گنتی);
        ssize_t (*اكتب)(صحيح fd, ثابت فراغ *buf, size_t گنتی);
        sd_future* (*timeout)(uint64_t timeout);
        sd_future* (*timeout_unref)(sd_future *timer);
} FiberOps;

bool fiber_ops_is_سیٹ(فراغ);
فراغ fiber_ops_سیٹ(ثابت FiberOps *fiber_ops);

صحيح fiber_ops_ppoll(بنية pollfd *fds, size_t n_fds, ثابت بنية timespec *timeout, ثابت sigset_t *sigmask);
ssize_t fiber_ops_read(صحيح fd, فراغ *buf, size_t گنتی);
ssize_t fiber_ops_write(صحيح fd, ثابت فراغ *buf, size_t گنتی);

/* Mirror of SD_FIBER_TIMEOUT() for code under src/basic that doesn't include sd-future.h: dispatches
 * through FiberOps so the actual sd_fiber_timeout() implementation lives in libsystemd. */
sd_future* fiber_ops_timeout(uint64_t timeout);
sd_future* fiber_ops_timeout_unref(sd_future *timer);
DEFINE_TRIVIAL_CLEANUP_FUNC(sd_future*, fiber_ops_timeout_unref);

#تعریف_کرو FIBER_OPS_TIMEOUT(timeout) _FIBER_OPS_TIMEOUT(UNIQ, (timeout))
#تعریف_کرو _FIBER_OPS_TIMEOUT(uniq, timeout)                                                                                               \
        بن_ورتیا _cleanup_(fiber_ops_timeout_unrefp) sd_future *UNIQ_T(_fot_, uniq) = fiber_ops_timeout(timeout)
