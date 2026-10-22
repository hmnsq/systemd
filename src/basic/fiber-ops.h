/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#அந்தர்பாவ "forward.h"

வகைவரை கட்டமைப்பு sd_future sd_future;

/* Hooks installed on a fiber so that functions in src/basic can transparently defer to the suspending
 * variants in sd-future when invoked from a running fiber. Populated by sd_fiber_new() with pointers to the
 * implementations in fiber-ops.c. */
வகைவரை கட்டமைப்பு FiberOps {
        முழு (*ppoll)(கட்டமைப்பு pollfd *fds, size_t n_fds, மாறிலி கட்டமைப்பு timespec *timeout, மாறிலி sigset_t *sigmask);
        ssize_t (*பட)(முழு fd, வெறுமை *buf, size_t மொத்தம்);
        ssize_t (*லிக)(முழு fd, மாறிலி வெறுமை *buf, size_t மொத்தம்);
        sd_future* (*timeout)(uint64_t timeout);
        sd_future* (*timeout_unref)(sd_future *timer);
} FiberOps;

bool fiber_ops_is_set(வெறுமை);
வெறுமை fiber_ops_set(மாறிலி FiberOps *fiber_ops);

முழு fiber_ops_ppoll(கட்டமைப்பு pollfd *fds, size_t n_fds, மாறிலி கட்டமைப்பு timespec *timeout, மாறிலி sigset_t *sigmask);
ssize_t fiber_ops_read(முழு fd, வெறுமை *buf, size_t மொத்தம்);
ssize_t fiber_ops_write(முழு fd, மாறிலி வெறுமை *buf, size_t மொத்தம்);

/* Mirror of SD_FIBER_TIMEOUT() for code under src/basic that doesn't include sd-future.h: dispatches
 * through FiberOps so the actual sd_fiber_timeout() implementation lives in libsystemd. */
sd_future* fiber_ops_timeout(uint64_t timeout);
sd_future* fiber_ops_timeout_unref(sd_future *timer);
DEFINE_TRIVIAL_CLEANUP_FUNC(sd_future*, fiber_ops_timeout_unref);

#define FIBER_OPS_TIMEOUT(timeout) _FIBER_OPS_TIMEOUT(UNIQ, (timeout))
#define _FIBER_OPS_TIMEOUT(uniq, timeout)                                                                                               \
        _unused_ _cleanup_(fiber_ops_timeout_unrefp) sd_future *UNIQ_T(_fot_, uniq) = fiber_ops_timeout(timeout)
