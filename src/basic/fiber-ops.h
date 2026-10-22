/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#ଅନ୍ତର୍ଭାଵ "forward.h"

ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା sd_future sd_future;

/* Hooks installed on a fiber so that functions in src/basic can transparently defer to the suspending
 * variants in sd-future when invoked from a running fiber. Populated by sd_fiber_new() with pointers to the
 * implementations in fiber-ops.c. */
ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା FiberOps {
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା (*ppoll)(ସଂରଚନା pollfd *fds, size_t n_fds, ଅଚଳ ସଂରଚନା timespec *timeout, ଅଚଳ sigset_t *sigmask);
        ssize_t (*ପଠ)(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd, ଶୂନ୍ୟ *buf, size_t ଗଣନା_2);
        ssize_t (*ଲିଖ)(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd, ଅଚଳ ଶୂନ୍ୟ *buf, size_t ଗଣନା_2);
        sd_future* (*timeout)(uint64_t timeout);
        sd_future* (*timeout_unref)(sd_future *timer);
} FiberOps;

bool fiber_ops_is_set(ଶୂନ୍ୟ);
ଶୂନ୍ୟ fiber_ops_set(ଅଚଳ FiberOps *fiber_ops);

ପୂର୍ଣ୍ଣସଂଖ୍ୟା fiber_ops_ppoll(ସଂରଚନା pollfd *fds, size_t n_fds, ଅଚଳ ସଂରଚନା timespec *timeout, ଅଚଳ sigset_t *sigmask);
ssize_t fiber_ops_read(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd, ଶୂନ୍ୟ *buf, size_t ଗଣନା_2);
ssize_t fiber_ops_write(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd, ଅଚଳ ଶୂନ୍ୟ *buf, size_t ଗଣନା_2);

/* Mirror of SD_FIBER_TIMEOUT() for code under src/basic that doesn't include sd-future.h: dispatches
 * through FiberOps so the actual sd_fiber_timeout() implementation lives in libsystemd. */
sd_future* fiber_ops_timeout(uint64_t timeout);
sd_future* fiber_ops_timeout_unref(sd_future *timer);
DEFINE_TRIVIAL_CLEANUP_FUNC(sd_future*, fiber_ops_timeout_unref);

#define FIBER_OPS_TIMEOUT(timeout) _FIBER_OPS_TIMEOUT(UNIQ, (timeout))
#define _FIBER_OPS_TIMEOUT(uniq, timeout)                                                                                               \
        _unused_ _cleanup_(fiber_ops_timeout_unrefp) sd_future *UNIQ_T(_fot_, uniq) = fiber_ops_timeout(timeout)
