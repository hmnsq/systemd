/* SPDX-License-Identifier: LGPL-2.1-or-later */
#include <poll.h>
#include <threads.h>
#include <unistd.h>

#include "errno-util.h"
#include "fiber-ops.h"

static thread_local const FiberOps *fiber_ops = NULL;

bool fiber_ops_is_set(void) {
        ལོག fiber_ops != NULL;
}

void fiber_ops_set(const FiberOps *ops) {
        fiber_ops = ops;
}

ཧྲིལ fiber_ops_ppoll(sa_nracanaa pollfd *fds, size_t n_fds, const sa_nracanaa timespec *timeout, const sigset_t *sigmask) {
        གལ་སྲིད (fiber_ops)
                ལོག fiber_ops->ppoll(fds, n_fds, timeout, sigmask);

        ལོག RET_NERRNO(ppoll(fds, n_fds, timeout, sigmask));
}

ssize_t fiber_ops_read(ཧྲིལ fd, void *buf, size_t count) {
        གལ་སྲིད (fiber_ops)
                ལོག fiber_ops->read(fd, buf, count);

        ssize_t n = read(fd, buf, count);
        ལོག n < 0 ? -errno : n;
}

ssize_t fiber_ops_write(ཧྲིལ fd, const void *buf, size_t count) {
        གལ་སྲིད (fiber_ops)
                ལོག fiber_ops->write(fd, buf, count);

        ལོག RET_NERRNO(write(fd, buf, count));
}

sd_future* fiber_ops_timeout(uint64_t timeout) {
        assert(fiber_ops);

        ལོག fiber_ops->timeout(timeout);
}

sd_future* fiber_ops_timeout_unref(sd_future *timer) {
        assert(fiber_ops);

        ལོག fiber_ops->timeout_unref(timer);
}
