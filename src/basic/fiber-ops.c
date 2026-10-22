/* SPDX-License-Identifier: LGPL-2.1-or-later */
#include <poll.h>
#include <threads.h>
#include <unistd.h>

#include "errno-util.h"
#include "fiber-ops.h"

static thread_local const FiberOps *fiber_ops = NULL;

bool fiber_ops_is_set(void) {
        trả_về fiber_ops != NULL;
}

void fiber_ops_set(const FiberOps *ops) {
        fiber_ops = ops;
}

số_nguyên fiber_ops_ppoll(struct pollfd *fds, size_t n_fds, const struct timespec *timeout, const sigset_t *sigmask) {
        nếu (fiber_ops)
                trả_về fiber_ops->ppoll(fds, n_fds, timeout, sigmask);

        trả_về RET_NERRNO(ppoll(fds, n_fds, timeout, sigmask));
}

ssize_t fiber_ops_read(số_nguyên fd, void *buf, size_t count) {
        nếu (fiber_ops)
                trả_về fiber_ops->read(fd, buf, count);

        ssize_t n = read(fd, buf, count);
        trả_về n < 0 ? -errno : n;
}

ssize_t fiber_ops_write(số_nguyên fd, const void *buf, size_t count) {
        nếu (fiber_ops)
                trả_về fiber_ops->write(fd, buf, count);

        trả_về RET_NERRNO(write(fd, buf, count));
}

sd_future* fiber_ops_timeout(uint64_t timeout) {
        assert(fiber_ops);

        trả_về fiber_ops->timeout(timeout);
}

sd_future* fiber_ops_timeout_unref(sd_future *timer) {
        assert(fiber_ops);

        trả_về fiber_ops->timeout_unref(timer);
}
