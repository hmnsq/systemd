/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#અન્તર્ભાવ "forward.h"

પ્રકારવ્યાખ્યા સંરચના sd_future sd_future;

/* Hooks installed on a fiber so that functions in src/basic can transparently defer to the suspending
 * variants in sd-future when invoked from a running fiber. Populated by sd_fiber_new() with pointers to the
 * implementations in fiber-ops.c. */
પ્રકારવ્યાખ્યા સંરચના FiberOps {
        પૂર્ણાંક (*ppoll)(સંરચના pollfd *fds, size_t n_fds, અચલ સંરચના timespec *timeout, અચલ sigset_t *sigmask);
        ssize_t (*પઠ)(પૂર્ણાંક fd, શૂન્ય *buf, size_t ગણતરી);
        ssize_t (*લિખ)(પૂર્ણાંક fd, અચલ શૂન્ય *buf, size_t ગણતરી);
        sd_future* (*timeout)(uint64_t timeout);
        sd_future* (*timeout_unref)(sd_future *timer);
} FiberOps;

bool fiber_ops_is_set(શૂન્ય);
શૂન્ય fiber_ops_set(અચલ FiberOps *fiber_ops);

પૂર્ણાંક fiber_ops_ppoll(સંરચના pollfd *fds, size_t n_fds, અચલ સંરચના timespec *timeout, અચલ sigset_t *sigmask);
ssize_t fiber_ops_read(પૂર્ણાંક fd, શૂન્ય *buf, size_t ગણતરી);
ssize_t fiber_ops_write(પૂર્ણાંક fd, અચલ શૂન્ય *buf, size_t ગણતરી);

/* Mirror of SD_FIBER_TIMEOUT() for code under src/basic that doesn't include sd-future.h: dispatches
 * through FiberOps so the actual sd_fiber_timeout() implementation lives in libsystemd. */
sd_future* fiber_ops_timeout(uint64_t timeout);
sd_future* fiber_ops_timeout_unref(sd_future *timer);
DEFINE_TRIVIAL_CLEANUP_FUNC(sd_future*, fiber_ops_timeout_unref);

#define FIBER_OPS_TIMEOUT(timeout) _FIBER_OPS_TIMEOUT(UNIQ, (timeout))
#define _FIBER_OPS_TIMEOUT(uniq, timeout)                                                                                               \
        _unused_ _cleanup_(fiber_ops_timeout_unrefp) sd_future *UNIQ_T(_fot_, uniq) = fiber_ops_timeout(timeout)
