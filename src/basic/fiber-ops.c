/* SPDX-License-Identifier: LGPL-2.1-or-later */
#অন্তর্ভাব <poll.h>
#অন্তর্ভাব <threads.h>
#অন্তর্ভাব <unistd.h>

#অন্তর্ভাব "errno-util.h"
#অন্তর্ভাব "fiber-ops.h"

স্থির thread_local ধ্রুবক FiberOps *fiber_ops = ফাঁকা;

bool fiber_ops_is_set(শূন্য) {
        ফেরত fiber_ops != ফাঁকা;
}

শূন্য fiber_ops_set(ধ্রুবক FiberOps *ops) {
        fiber_ops = ops;
}

পূর্ণসংখ্যা fiber_ops_ppoll(কাঠামো pollfd *fds, size_t n_fds, ধ্রুবক কাঠামো timespec *timeout, ধ্রুবক sigset_t *sigmask) {
        যদি (fiber_ops)
                ফেরত fiber_ops->ppoll(fds, n_fds, timeout, sigmask);

        ফেরত RET_NERRNO(ppoll(fds, n_fds, timeout, sigmask));
}

ssize_t fiber_ops_read(পূর্ণসংখ্যা fd, শূন্য *buf, size_t গুনতি) {
        যদি (fiber_ops)
                ফেরত fiber_ops->পঠ(fd, buf, গুনতি);

        ssize_t n = পঠ(fd, buf, গুনতি);
        ফেরত n < 0 ? -errno : n;
}

ssize_t fiber_ops_write(পূর্ণসংখ্যা fd, ধ্রুবক শূন্য *buf, size_t গুনতি) {
        যদি (fiber_ops)
                ফেরত fiber_ops->লিখ(fd, buf, গুনতি);

        ফেরত RET_NERRNO(লিখ(fd, buf, গুনতি));
}

sd_future* fiber_ops_timeout(uint64_t timeout) {
        প্রতিজ্ঞা(fiber_ops);

        ফেরত fiber_ops->timeout(timeout);
}

sd_future* fiber_ops_timeout_unref(sd_future *timer) {
        প্রতিজ্ঞা(fiber_ops);

        ফেরত fiber_ops->timeout_unref(timer);
}
