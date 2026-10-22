/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ضمن <poll.h>
#ضمن <threads.h>
#ضمن <unistd.h>

#ضمن "errno-util.h"
#ضمن "fiber-ops.h"

ساكن thread_local ثابت FiberOps *fiber_ops = عدم;

bool fiber_ops_is_سېټ(فراغ) {
        أرجع fiber_ops != عدم;
}

فراغ fiber_ops_سېټ(ثابت FiberOps *ops) {
        fiber_ops = ops;
}

صحيح fiber_ops_ppoll(بنية pollfd *fds, size_t n_fds, ثابت بنية timespec *timeout, ثابت sigset_t *sigmask) {
        إذا (fiber_ops)
                أرجع fiber_ops->ppoll(fds, n_fds, timeout, sigmask);

        أرجع RET_NERRNO(ppoll(fds, n_fds, timeout, sigmask));
}

ssize_t fiber_ops_read(صحيح fd, فراغ *buf, size_t شمېر) {
        إذا (fiber_ops)
                أرجع fiber_ops->اقرأ(fd, buf, شمېر);

        ssize_t n = اقرأ(fd, buf, شمېر);
        أرجع n < 0 ? -errno : n;
}

ssize_t fiber_ops_write(صحيح fd, ثابت فراغ *buf, size_t شمېر) {
        إذا (fiber_ops)
                أرجع fiber_ops->اكتب(fd, buf, شمېر);

        أرجع RET_NERRNO(اكتب(fd, buf, شمېر));
}

sd_future* fiber_ops_timeout(uint64_t timeout) {
        أكد(fiber_ops);

        أرجع fiber_ops->timeout(timeout);
}

sd_future* fiber_ops_timeout_unref(sd_future *timer) {
        أكد(fiber_ops);

        أرجع fiber_ops->timeout_unref(timer);
}
