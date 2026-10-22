/* SPDX-License-Identifier: LGPL-2.1-or-later */
#अन्तर्भाव <poll.h>
#अन्तर्भाव <threads.h>
#अन्तर्भाव <unistd.h>

#अन्तर्भाव "errno-util.h"
#अन्तर्भाव "fiber-ops.h"

स्थिर thread_local ध्रुव FiberOps *fiber_ops = रिक्त;

bool fiber_ops_is_set(शून्य) {
        प्रत्यागम fiber_ops != रिक्त;
}

शून्य fiber_ops_set(ध्रुव FiberOps *ops) {
        fiber_ops = ops;
}

पूर्णाङ्क fiber_ops_ppoll(संरचना pollfd *fds, size_t n_fds, ध्रुव संरचना timespec *timeout, ध्रुव sigset_t *sigmask) {
        यदि (fiber_ops)
                प्रत्यागम fiber_ops->ppoll(fds, n_fds, timeout, sigmask);

        प्रत्यागम RET_NERRNO(ppoll(fds, n_fds, timeout, sigmask));
}

ssize_t fiber_ops_read(पूर्णाङ्क fd, शून्य *buf, size_t मोजणी) {
        यदि (fiber_ops)
                प्रत्यागम fiber_ops->पठ(fd, buf, मोजणी);

        ssize_t n = पठ(fd, buf, मोजणी);
        प्रत्यागम n < 0 ? -errno : n;
}

ssize_t fiber_ops_write(पूर्णाङ्क fd, ध्रुव शून्य *buf, size_t मोजणी) {
        यदि (fiber_ops)
                प्रत्यागम fiber_ops->लिख(fd, buf, मोजणी);

        प्रत्यागम RET_NERRNO(लिख(fd, buf, मोजणी));
}

sd_future* fiber_ops_timeout(uint64_t timeout) {
        प्रतिज्ञा(fiber_ops);

        प्रत्यागम fiber_ops->timeout(timeout);
}

sd_future* fiber_ops_timeout_unref(sd_future *timer) {
        प्रतिज्ञा(fiber_ops);

        प्रत्यागम fiber_ops->timeout_unref(timer);
}
