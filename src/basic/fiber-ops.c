/* SPDX-License-Identifier: LGPL-2.1-or-later */
#అన్తర్భావ <poll.h>
#అన్తర్భావ <threads.h>
#అన్తర్భావ <unistd.h>

#అన్తర్భావ "errno-util.h"
#అన్తర్భావ "fiber-ops.h"

స్థిర thread_local స్థిరరాశి FiberOps *fiber_ops = ఖాళీ;

bool fiber_ops_is_సమితి(శూన్య) {
        తిరిగి fiber_ops != ఖాళీ;
}

శూన్య fiber_ops_సమితి(స్థిరరాశి FiberOps *ops) {
        fiber_ops = ops;
}

పూర్ణాంకం fiber_ops_ppoll(నిర్మాణం pollfd *fds, size_t n_fds, స్థిరరాశి నిర్మాణం timespec *timeout, స్థిరరాశి sigset_t *sigmask) {
        ఒకవేళ (fiber_ops)
                తిరిగి fiber_ops->ppoll(fds, n_fds, timeout, sigmask);

        తిరిగి RET_NERRNO(ppoll(fds, n_fds, timeout, sigmask));
}

ssize_t fiber_ops_read(పూర్ణాంకం fd, శూన్య *buf, size_t లెక్క) {
        ఒకవేళ (fiber_ops)
                తిరిగి fiber_ops->పఠ(fd, buf, లెక్క);

        ssize_t n = పఠ(fd, buf, లెక్క);
        తిరిగి n < 0 ? -errno : n;
}

ssize_t fiber_ops_write(పూర్ణాంకం fd, స్థిరరాశి శూన్య *buf, size_t లెక్క) {
        ఒకవేళ (fiber_ops)
                తిరిగి fiber_ops->లిఖ(fd, buf, లెక్క);

        తిరిగి RET_NERRNO(లిఖ(fd, buf, లెక్క));
}

sd_future* fiber_ops_timeout(uint64_t timeout) {
        ప్రతిజ్ఞా(fiber_ops);

        తిరిగి fiber_ops->timeout(timeout);
}

sd_future* fiber_ops_timeout_unref(sd_future *timer) {
        ప్రతిజ్ఞా(fiber_ops);

        తిరిగి fiber_ops->timeout_unref(timer);
}
