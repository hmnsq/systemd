/* SPDX-License-Identifier: LGPL-2.1-or-later */
#شامل <poll.h>
#شامل <threads.h>
#شامل <unistd.h>

#شامل "errno-util.h"
#شامل "fiber-ops.h"

ساکن thread_local ثابت FiberOps *fiber_ops = تہی;

bool fiber_ops_is_سیٹ(خالی) {
        واپس fiber_ops != تہی;
}

خالی fiber_ops_سیٹ(ثابت FiberOps *ops) {
        fiber_ops = ops;
}

صحیح fiber_ops_ppoll(ساخت pollfd *fds, size_t n_fds, ثابت ساخت timespec *timeout, ثابت sigset_t *sigmask) {
        اگر (fiber_ops)
                واپس fiber_ops->ppoll(fds, n_fds, timeout, sigmask);

        واپس RET_NERRNO(ppoll(fds, n_fds, timeout, sigmask));
}

ssize_t fiber_ops_read(صحیح fd, خالی *buf, size_t گنتی) {
        اگر (fiber_ops)
                واپس fiber_ops->اقرأ(fd, buf, گنتی);

        ssize_t n = اقرأ(fd, buf, گنتی);
        واپس n < 0 ? -errno : n;
}

ssize_t fiber_ops_write(صحیح fd, ثابت خالی *buf, size_t گنتی) {
        اگر (fiber_ops)
                واپس fiber_ops->اكتب(fd, buf, گنتی);

        واپس RET_NERRNO(اكتب(fd, buf, گنتی));
}

sd_future* fiber_ops_timeout(uint64_t timeout) {
        أكد(fiber_ops);

        واپس fiber_ops->timeout(timeout);
}

sd_future* fiber_ops_timeout_unref(sd_future *timer) {
        أكد(fiber_ops);

        واپس fiber_ops->timeout_unref(timer);
}
