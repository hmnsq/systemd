/* SPDX-License-Identifier: LGPL-2.1-or-later */
#অন্তর্ভাব <poll.h>
#অন্তর্ভাব <threads.h>
#অন্তর্ভাব <unistd.h>

#অন্তর্ভাব "errno-util.h"
#অন্তর্ভাব "fiber-ops.h"

স্থির thread_local ধ্রুব FiberOps *fiber_ops = রিক্ত;

bool fiber_ops_is_set(শূন্য) {
        প্রত্যাগম fiber_ops != রিক্ত;
}

শূন্য fiber_ops_set(ধ্রুব FiberOps *ops) {
        fiber_ops = ops;
}

পূর্ণাঙ্ক fiber_ops_ppoll(সংরচনা pollfd *fds, size_t n_fds, ধ্রুব সংরচনা timespec *timeout, ধ্রুব sigset_t *sigmask) {
        যদি (fiber_ops)
                প্রত্যাগম fiber_ops->ppoll(fds, n_fds, timeout, sigmask);

        প্রত্যাগম RET_NERRNO(ppoll(fds, n_fds, timeout, sigmask));
}

ssize_t fiber_ops_read(পূর্ণাঙ্ক fd, শূন্য *buf, size_t গুনতি) {
        যদি (fiber_ops)
                প্রত্যাগম fiber_ops->পঠ(fd, buf, গুনতি);

        ssize_t n = পঠ(fd, buf, গুনতি);
        প্রত্যাগম n < 0 ? -errno : n;
}

ssize_t fiber_ops_write(পূর্ণাঙ্ক fd, ধ্রুব শূন্য *buf, size_t গুনতি) {
        যদি (fiber_ops)
                প্রত্যাগম fiber_ops->লিখ(fd, buf, গুনতি);

        প্রত্যাগম RET_NERRNO(লিখ(fd, buf, গুনতি));
}

sd_future* fiber_ops_timeout(uint64_t timeout) {
        প্রতিজ্ঞা(fiber_ops);

        প্রত্যাগম fiber_ops->timeout(timeout);
}

sd_future* fiber_ops_timeout_unref(sd_future *timer) {
        প্রতিজ্ঞা(fiber_ops);

        প্রত্যাগম fiber_ops->timeout_unref(timer);
}
