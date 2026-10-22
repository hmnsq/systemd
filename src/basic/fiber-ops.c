/* SPDX-License-Identifier: LGPL-2.1-or-later */
#अन्तर्भाव <poll.h>
#अन्तर्भाव <threads.h>
#अन्तर्भाव <unistd.h>

#अन्तर्भाव "errno-util.h"
#अन्तर्भाव "fiber-ops.h"

स्थिर thread_local स्थिरांक FiberOps *fiber_ops = रिक्त;

bool fiber_ops_is_set(शून्य) {
        परत fiber_ops != रिक्त;
}

शून्य fiber_ops_set(स्थिरांक FiberOps *ops) {
        fiber_ops = ops;
}

पूर्णांक fiber_ops_ppoll(संरचना pollfd *fds, माप_प्रकार n_fds, स्थिरांक संरचना timespec *timeout, स्थिरांक sigset_t *sigmask) {
        जर (fiber_ops)
                परत fiber_ops->ppoll(fds, n_fds, timeout, sigmask);

        परत RET_NERRNO(ppoll(fds, n_fds, timeout, sigmask));
}

ssize_t fiber_ops_read(पूर्णांक fd, शून्य *buf, माप_प्रकार मोजणी) {
        जर (fiber_ops)
                परत fiber_ops->पठ(fd, buf, मोजणी);

        ssize_t n = पठ(fd, buf, मोजणी);
        परत n < 0 ? -त्रुटि_सं : n;
}

ssize_t fiber_ops_write(पूर्णांक fd, स्थिरांक शून्य *buf, माप_प्रकार मोजणी) {
        जर (fiber_ops)
                परत fiber_ops->लिख(fd, buf, मोजणी);

        परत RET_NERRNO(लिख(fd, buf, मोजणी));
}

sd_future* fiber_ops_timeout(uint64_t timeout) {
        प्रतिज्ञा(fiber_ops);

        परत fiber_ops->timeout(timeout);
}

sd_future* fiber_ops_timeout_unref(sd_future *timer) {
        प्रतिज्ञा(fiber_ops);

        परत fiber_ops->timeout_unref(timer);
}
