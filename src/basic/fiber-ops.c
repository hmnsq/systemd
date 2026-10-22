/* SPDX-License-Identifier: LGPL-2.1-or-later */
#अन्तर्भाव <poll.h>
#अन्तर्भाव <threads.h>
#अन्तर्भाव <unistd.h>

#अन्तर्भाव "errno-util.h"
#अन्तर्भाव "fiber-ops.h"

स्थिर thread_local अचर FiberOps *fiber_ops = रिक्त;

bool fiber_ops_is_set(शून्य) {
        फर्काऊ fiber_ops != रिक्त;
}

शून्य fiber_ops_set(अचर FiberOps *ops) {
        fiber_ops = ops;
}

पूर्णाङ्क fiber_ops_ppoll(संरचना pollfd *fds, माप_प्रकार n_fds, अचर संरचना timespec *timeout, अचर sigset_t *sigmask) {
        यदि (fiber_ops)
                फर्काऊ fiber_ops->ppoll(fds, n_fds, timeout, sigmask);

        फर्काऊ RET_NERRNO(ppoll(fds, n_fds, timeout, sigmask));
}

ssize_t fiber_ops_read(पूर्णाङ्क fd, शून्य *buf, माप_प्रकार गन्ती) {
        यदि (fiber_ops)
                फर्काऊ fiber_ops->पठ(fd, buf, गन्ती);

        ssize_t n = पठ(fd, buf, गन्ती);
        फर्काऊ n < 0 ? -त्रुटि_सं : n;
}

ssize_t fiber_ops_write(पूर्णाङ्क fd, अचर शून्य *buf, माप_प्रकार गन्ती) {
        यदि (fiber_ops)
                फर्काऊ fiber_ops->लिख(fd, buf, गन्ती);

        फर्काऊ RET_NERRNO(लिख(fd, buf, गन्ती));
}

sd_future* fiber_ops_timeout(uint64_t timeout) {
        प्रतिज्ञा(fiber_ops);

        फर्काऊ fiber_ops->timeout(timeout);
}

sd_future* fiber_ops_timeout_unref(sd_future *timer) {
        प्रतिज्ञा(fiber_ops);

        फर्काऊ fiber_ops->timeout_unref(timer);
}
