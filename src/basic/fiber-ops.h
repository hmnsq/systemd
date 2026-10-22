/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#അന്തര്ഭാവ "forward.h"

തരനിർവചനം ഘടന sd_future sd_future;

/* Hooks installed on a fiber so that functions in src/basic can transparently defer to the suspending
 * variants in sd-future when invoked from a running fiber. Populated by sd_fiber_new() with pointers to the
 * implementations in fiber-ops.c. */
തരനിർവചനം ഘടന FiberOps {
        പൂർണ്ണസംഖ്യ (*ppoll)(ഘടന pollfd *fds, size_t n_fds, സ്ഥിരം ഘടന timespec *timeout, സ്ഥിരം sigset_t *sigmask);
        ssize_t (*പഠ)(പൂർണ്ണസംഖ്യ fd, ശൂന്യം *buf, size_t എണ്ണം);
        ssize_t (*ലിഖ)(പൂർണ്ണസംഖ്യ fd, സ്ഥിരം ശൂന്യം *buf, size_t എണ്ണം);
        sd_future* (*timeout)(uint64_t timeout);
        sd_future* (*timeout_unref)(sd_future *timer);
} FiberOps;

bool fiber_ops_is_set(ശൂന്യം);
ശൂന്യം fiber_ops_set(സ്ഥിരം FiberOps *fiber_ops);

പൂർണ്ണസംഖ്യ fiber_ops_ppoll(ഘടന pollfd *fds, size_t n_fds, സ്ഥിരം ഘടന timespec *timeout, സ്ഥിരം sigset_t *sigmask);
ssize_t fiber_ops_read(പൂർണ്ണസംഖ്യ fd, ശൂന്യം *buf, size_t എണ്ണം);
ssize_t fiber_ops_write(പൂർണ്ണസംഖ്യ fd, സ്ഥിരം ശൂന്യം *buf, size_t എണ്ണം);

/* Mirror of SD_FIBER_TIMEOUT() for code under src/basic that doesn't include sd-future.h: dispatches
 * through FiberOps so the actual sd_fiber_timeout() implementation lives in libsystemd. */
sd_future* fiber_ops_timeout(uint64_t timeout);
sd_future* fiber_ops_timeout_unref(sd_future *timer);
DEFINE_TRIVIAL_CLEANUP_FUNC(sd_future*, fiber_ops_timeout_unref);

#define FIBER_OPS_TIMEOUT(timeout) _FIBER_OPS_TIMEOUT(UNIQ, (timeout))
#define _FIBER_OPS_TIMEOUT(uniq, timeout)                                                                                               \
        _unused_ _cleanup_(fiber_ops_timeout_unrefp) sd_future *UNIQ_T(_fot_, uniq) = fiber_ops_timeout(timeout)
