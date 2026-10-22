/* SPDX-License-Identifier: LGPL-2.1-or-later */
#অন্তর্ভাৱ <poll.h>
#অন্তর্ভাৱ <threads.h>
#অন্তর্ভাৱ <unistd.h>

#অন্তর্ভাৱ "errno-util.h"
#অন্তর্ভাৱ "fiber-ops.h"

স্থির thread_local ধ্রুৱ FiberOps *fiber_ops = রিক্ত;

bool fiber_ops_is_set(শূন্য) {
        ঘূৰাই fiber_ops != রিক্ত;
}

শূন্য fiber_ops_set(ধ্রুৱ FiberOps *ops) {
        fiber_ops = ops;
}

পূর্ণাঙ্ক fiber_ops_ppoll(সংরচনা pollfd *fds, size_t n_fds, ধ্রুৱ সংরচনা timespec *timeout, ধ্রুৱ sigset_t *sigmask) {
        যদি (fiber_ops)
                ঘূৰাই fiber_ops->ppoll(fds, n_fds, timeout, sigmask);

        ঘূৰাই RET_NERRNO(ppoll(fds, n_fds, timeout, sigmask));
}

ssize_t fiber_ops_read(পূর্ণাঙ্ক fd, শূন্য *buf, size_t গুনতি) {
        যদি (fiber_ops)
                ঘূৰাই fiber_ops->পঠ(fd, buf, গুনতি);

        ssize_t n = পঠ(fd, buf, গুনতি);
        ঘূৰাই n < 0 ? -errno : n;
}

ssize_t fiber_ops_write(পূর্ণাঙ্ক fd, ধ্রুৱ শূন্য *buf, size_t গুনতি) {
        যদি (fiber_ops)
                ঘূৰাই fiber_ops->লিখ(fd, buf, গুনতি);

        ঘূৰাই RET_NERRNO(লিখ(fd, buf, গুনতি));
}

sd_future* fiber_ops_timeout(uint64_t timeout) {
        প্রতিজ্ঞা(fiber_ops);

        ঘূৰাই fiber_ops->timeout(timeout);
}

sd_future* fiber_ops_timeout_unref(sd_future *timer) {
        প্রতিজ্ঞা(fiber_ops);

        ঘূৰাই fiber_ops->timeout_unref(timer);
}
