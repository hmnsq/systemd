/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ଅନ୍ତର୍ଭାଵ <poll.h>
#ଅନ୍ତର୍ଭାଵ <threads.h>
#ଅନ୍ତର୍ଭାଵ <unistd.h>

#ଅନ୍ତର୍ଭାଵ "errno-util.h"
#ଅନ୍ତର୍ଭାଵ "fiber-ops.h"

ସ୍ଥିର thread_local ଅଚଳ FiberOps *fiber_ops = ଖାଲି;

bool fiber_ops_is_set(ଶୂନ୍ୟ) {
        ଫେରନ୍ତୁ fiber_ops != ଖାଲି;
}

ଶୂନ୍ୟ fiber_ops_set(ଅଚଳ FiberOps *ops) {
        fiber_ops = ops;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା fiber_ops_ppoll(ସଂରଚନା pollfd *fds, size_t n_fds, ଅଚଳ ସଂରଚନା timespec *timeout, ଅଚଳ sigset_t *sigmask) {
        ଯଦି (fiber_ops)
                ଫେରନ୍ତୁ fiber_ops->ppoll(fds, n_fds, timeout, sigmask);

        ଫେରନ୍ତୁ RET_NERRNO(ppoll(fds, n_fds, timeout, sigmask));
}

ssize_t fiber_ops_read(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd, ଶୂନ୍ୟ *buf, size_t ଗଣନା_2) {
        ଯଦି (fiber_ops)
                ଫେରନ୍ତୁ fiber_ops->ପଠ(fd, buf, ଗଣନା_2);

        ssize_t n = ପଠ(fd, buf, ଗଣନା_2);
        ଫେରନ୍ତୁ n < 0 ? -errno : n;
}

ssize_t fiber_ops_write(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd, ଅଚଳ ଶୂନ୍ୟ *buf, size_t ଗଣନା_2) {
        ଯଦି (fiber_ops)
                ଫେରନ୍ତୁ fiber_ops->ଲିଖ(fd, buf, ଗଣନା_2);

        ଫେରନ୍ତୁ RET_NERRNO(ଲିଖ(fd, buf, ଗଣନା_2));
}

sd_future* fiber_ops_timeout(uint64_t timeout) {
        ପ୍ରତିଜ୍ଞା(fiber_ops);

        ଫେରନ୍ତୁ fiber_ops->timeout(timeout);
}

sd_future* fiber_ops_timeout_unref(sd_future *timer) {
        ପ୍ରତିଜ୍ଞା(fiber_ops);

        ଫେରନ୍ତୁ fiber_ops->timeout_unref(timer);
}
