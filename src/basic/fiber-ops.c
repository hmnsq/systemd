/* SPDX-License-Identifier: LGPL-2.1-or-later */
#شامل <poll.h>
#شامل <threads.h>
#شامل <unistd.h>

#شامل "errno-util.h"
#شامل "fiber-ops.h"

ایستا thread_local ثابت FiberOps *fiber_ops = پوچ;

bool fiber_ops_is_set(تهی) {
        بازگشت fiber_ops != پوچ;
}

تهی fiber_ops_set(ثابت FiberOps *ops) {
        fiber_ops = ops;
}

صحیح fiber_ops_ppoll(ساختار pollfd *fds, size_t n_fds, ثابت ساختار timespec *timeout, ثابت sigset_t *sigmask) {
        اگر (fiber_ops)
                بازگشت fiber_ops->ppoll(fds, n_fds, timeout, sigmask);

        بازگشت RET_NERRNO(ppoll(fds, n_fds, timeout, sigmask));
}

ssize_t fiber_ops_read(صحیح fd, تهی *buf, size_t شمار) {
        اگر (fiber_ops)
                بازگشت fiber_ops->اقرأ(fd, buf, شمار);

        ssize_t n = اقرأ(fd, buf, شمار);
        بازگشت n < 0 ? -errno : n;
}

ssize_t fiber_ops_write(صحیح fd, ثابت تهی *buf, size_t شمار) {
        اگر (fiber_ops)
                بازگشت fiber_ops->اكتب(fd, buf, شمار);

        بازگشت RET_NERRNO(اكتب(fd, buf, شمار));
}

sd_future* fiber_ops_timeout(uint64_t timeout) {
        أكد(fiber_ops);

        بازگشت fiber_ops->timeout(timeout);
}

sd_future* fiber_ops_timeout_unref(sd_future *timer) {
        أكد(fiber_ops);

        بازگشت fiber_ops->timeout_unref(timer);
}
