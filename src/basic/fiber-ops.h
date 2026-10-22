/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#অন্তর্ভাব "forward.h"

প্রকারনির্দেশ সংরচনা sd_future sd_future;

/* Hooks installed on a fiber so that functions in src/basic can transparently defer to the suspending
 * variants in sd-future when invoked from a running fiber. Populated by sd_fiber_new() with pointers to the
 * implementations in fiber-ops.c. */
প্রকারনির্দেশ সংরচনা FiberOps {
        পূর্ণাঙ্ক (*ppoll)(সংরচনা pollfd *fds, size_t n_fds, ধ্রুব সংরচনা timespec *timeout, ধ্রুব sigset_t *sigmask);
        ssize_t (*পঠ)(পূর্ণাঙ্ক fd, শূন্য *buf, size_t গুনতি);
        ssize_t (*লিখ)(পূর্ণাঙ্ক fd, ধ্রুব শূন্য *buf, size_t গুনতি);
        sd_future* (*timeout)(uint64_t timeout);
        sd_future* (*timeout_unref)(sd_future *timer);
} FiberOps;

bool fiber_ops_is_set(শূন্য);
শূন্য fiber_ops_set(ধ্রুব FiberOps *fiber_ops);

পূর্ণাঙ্ক fiber_ops_ppoll(সংরচনা pollfd *fds, size_t n_fds, ধ্রুব সংরচনা timespec *timeout, ধ্রুব sigset_t *sigmask);
ssize_t fiber_ops_read(পূর্ণাঙ্ক fd, শূন্য *buf, size_t গুনতি);
ssize_t fiber_ops_write(পূর্ণাঙ্ক fd, ধ্রুব শূন্য *buf, size_t গুনতি);

/* Mirror of SD_FIBER_TIMEOUT() for code under src/basic that doesn't include sd-future.h: dispatches
 * through FiberOps so the actual sd_fiber_timeout() implementation lives in libsystemd. */
sd_future* fiber_ops_timeout(uint64_t timeout);
sd_future* fiber_ops_timeout_unref(sd_future *timer);
DEFINE_TRIVIAL_CLEANUP_FUNC(sd_future*, fiber_ops_timeout_unref);

#define FIBER_OPS_TIMEOUT(timeout) _FIBER_OPS_TIMEOUT(UNIQ, (timeout))
#define _FIBER_OPS_TIMEOUT(uniq, timeout)                                                                                               \
        _unused_ _cleanup_(fiber_ops_timeout_unrefp) sd_future *UNIQ_T(_fot_, uniq) = fiber_ops_timeout(timeout)
